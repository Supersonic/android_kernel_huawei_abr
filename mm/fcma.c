#include <linux/cma.h>
#include <linux/mm_types.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/list.h>
#include <linux/migrate.h>
#include <linux/rmap.h>
#include <linux/swap.h>
#include <linux/fcma.h>
#include "cma.h"

atomic_t fcma_migrate_flag;
atomic_t fcma_page_num = ATOMIC_INIT(0);

struct fcma_work {
	struct work_struct work;
	struct page *page;
};

int get_fcma_page_num(void)
{
	return atomic_read(&fcma_page_num);
}

void fcma_page_num_sub(int delta)
{
	atomic_sub(delta, &fcma_page_num);
}

void fcma_page_num_add(int delta)
{
	atomic_add(delta, &fcma_page_num);
}

void fcma_page_num_dec()
{
	atomic_dec(&fcma_page_num);
}

void fcma_page_num_inc()
{
	atomic_inc(&fcma_page_num);
}

void set_fcma_migrate_flag(enum migratetype mt)
{
	if (mt != MIGRATE_FCMA)
		return;

	atomic_set(&fcma_migrate_flag, 1);
}

void clear_fcma_migrate_flag(enum migratetype mt)
{
	if (mt != MIGRATE_FCMA)
		return;

	atomic_set(&fcma_migrate_flag, 0);
}

int test_fcma_migrate_flag(void)
{
	return atomic_read(&fcma_migrate_flag);
}

int fcma_cluster_max = 60000;

static struct workqueue_struct *fcma_wq;

static int __init fcma_wq_init(void)
{
	fcma_wq = alloc_workqueue("fcma_work", 0, 1);

	return 0;
}
core_initcall(fcma_wq_init);

LIST_HEAD(fcma_list);
int in_fcma_area(struct page *page)
{
       unsigned long start_pfn;
       unsigned long end_pfn;
       unsigned long pfn;
       struct cma *cma;

       if (list_empty(&fcma_list) || !page)
	       return 0;

       list_for_each_entry(cma, &fcma_list, list) {
	       start_pfn = cma->base_pfn;
	       end_pfn = start_pfn + cma->count;
	       pfn = page_to_pfn(page);

	       if (start_pfn <= pfn && pfn < end_pfn)
		       return 1;
       }

       return 0;
}

struct page *fcma_new_page(struct page *page, unsigned long private)
{
	if (test_fcma_migrate_flag())
		return NULL;

	return __page_cache_alloc(GFP_FCMA);
}

static bool fcma_check_page(struct page *page)
{
	struct address_space *mapping;
	struct inode *inode;

	mapping = page_mapping(page);

	if (!mapping)
		return false;

	inode = mapping->host;
	if (!inode)
		return false;

	if (inode_is_open_for_write(inode) || !S_ISREG(inode->i_mode))
		return false;

#if defined(CONFIG_TASK_PROTECT_LRU) || defined(CONFIG_MEMCG_PROTECT_LRU)
	if (inode->i_protect)
		return false;
#endif
	if (!(inode->i_flags & S_ONEREAD))
		return false;

	return true;
}

static void fcma_replace_page_work_fn(struct work_struct *work)
{
	struct fcma_work *fwork = container_of(work, struct fcma_work, work);
	struct page *page = fwork->page;
	LIST_HEAD(pagelist);
	int ret;

	lru_add_drain();

	if (!PageLRU(page) || !fcma_check_page(page)) {
		put_page(page);
		goto out;
	}

	ret = isolate_lru_page(page);
	put_page(page);
	if (ret)
		goto out;

	list_add(&page->lru, &pagelist);
	ret = migrate_pages(&pagelist, fcma_new_page, NULL, FCMA_PAGE_HINT,
			    MIGRATE_SYNC, MR_CONTIG_RANGE);
	if (ret)
		putback_movable_pages(&pagelist);
out:
	kfree(fwork);
}

int __init fcma_init_reserved_mem(phys_addr_t base, phys_addr_t size,
				  unsigned int order_per_bit,
				  const char *name,
				  struct cma **res_cma)
{
	int ret;

	ret = cma_init_reserved_mem(base, size, order_per_bit, name, res_cma);
	if (!ret) {
		(*res_cma)->is_fcma = true;
		list_add(&(*res_cma)->list, &fcma_list);
		fcma_page_num_add(size / PAGE_SIZE);
	}

	return ret;
}

void fcma_replace_page(struct page *page)
{
	struct fcma_work *fwork;

	if (!fcma_wq)
		return;

	if (!fcma_check_page(page))
		return;

	fwork = kmalloc(sizeof(*fwork), GFP_ATOMIC);
	if (!fwork)
		return;

	get_page(page);
	fwork->page = page;

	INIT_WORK(&fwork->work, fcma_replace_page_work_fn);
	queue_work_on(raw_smp_processor_id(), fcma_wq, &fwork->work);
	return;
}
