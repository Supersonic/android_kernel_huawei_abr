/*
 * mm/mem_track/mm_ion_dump.c	   ion track function
 *
 * Copyright(C) 2020 Huawei Technologies Co., Ltd. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/dma-buf.h>
#include <linux/fdtable.h>
#include <linux/sched/signal.h>
#include <linux/mem_track/mem_trace.h>
#include <linux/fdtable.h>
#include <linux/ion.h>
#include <linux/version.h>

#ifndef CONFIG_ARCH_QCOM
#include "ion_priv.h"
#elif (KERNEL_VERSION(5, 4, 0) <= LINUX_VERSION_CODE)
#include "ion_private.h"
#endif

static struct dma_buf * file_to_dma_buf(struct file *file)
{
	return file->private_data;
}

unsigned long mm_ion_total(void)
{
#if (KERNEL_VERSION(5, 4, 0) <= LINUX_VERSION_CODE)
	return ion_get_total_heap_bytes();
#else
	return get_ion_total();
#endif
}
EXPORT_SYMBOL(mm_ion_total);

/* this func must be in ion.c */
static inline struct ion_buffer *get_ion_buf(struct dma_buf *dbuf)
{
	return dbuf->priv;
}

static int mm_ion_debug_process_cb(const void *data,
	struct file *f, unsigned int fd)
{
	struct dma_buf *dbuf = NULL;
	struct ion_buffer *ibuf = NULL;

	if (!is_dma_buf_file(f))
		return 0;

	dbuf = file_to_dma_buf(f);
	if (!dbuf)
		return 0;

	if (!is_ion_dma_buf(dbuf))
		return 0;

	ibuf = get_ion_buf(dbuf);
	if (!ibuf)
		return 0;

	return 0;
}

int mm_ion_proecss_info(void)
{
	struct task_struct *tsk = NULL;

	pr_err("Detail process info is below:\n");
	pr_err("%-16.s%3s%-16s%-16s%-16s%-16s%-16s%-12s%-12s%-12s%-12s%-12s\n",
		"taskname", "", "pid", "fd", "sz", "heapid", "magic", "total_cost", "sema_cost",
		"alloc1_cost", "drain_cost", "alloc2_cost");

	rcu_read_lock();
	for_each_process(tsk) {
		if (tsk->flags & PF_KTHREAD)
			continue;

		task_lock(tsk);
		(void)iterate_fd(tsk->files, 0,
			mm_ion_debug_process_cb, (void *)tsk);
		task_unlock(tsk);
	}
	rcu_read_unlock();

	return 0;
}

static size_t mm_ion_detail_cb(const void *data,
	struct file *f, unsigned int fd)
{
	struct dma_buf *dbuf = NULL;
	struct ion_buffer *ibuf = NULL;

	if (!is_dma_buf_file(f))
		return 0;

	dbuf = file_to_dma_buf(f);
	if (!dbuf)
		return 0;

	if (!is_ion_dma_buf(dbuf))
		return 0;

	ibuf = get_ion_buf(dbuf);
	if (!ibuf)
		return 0;

	return dbuf->size;
}

static size_t ion_iterate_fd(struct files_struct *files, unsigned int n,
	size_t (*f)(const void *, struct file *, unsigned int), const void *p)
{
	struct fdtable *fdt = NULL;
	size_t res = 0;
	struct file *file = NULL;

	if (!files)
		return 0;
	spin_lock(&files->file_lock);
	for (fdt = files_fdtable(files); n < fdt->max_fds; n++) {
		file = rcu_dereference_check_fdtable(files, fdt->fd[n]);
		if (!file)
			continue;
		res += f(p, file, n);
	}
	spin_unlock(&files->file_lock);
	return res;
}

size_t mm_get_ion_detail(void *buf, size_t len)
{
	size_t cnt = 0;
	size_t size;
	struct task_struct *tsk = NULL;
	struct mm_ion_detail_info *info =
		(struct mm_ion_detail_info *)buf;

	if (!buf)
		return cnt;
	rcu_read_lock();
	for_each_process(tsk) {
		if (tsk->flags & PF_KTHREAD)
			continue;
		if (cnt >= len) {
			rcu_read_unlock();
			return len;
		}
		task_lock(tsk);
		size = ion_iterate_fd(tsk->files, 0,
			mm_ion_detail_cb, (void *)tsk);
		task_unlock(tsk);
		if (size) {
			(info + cnt)->size = size;
			(info + cnt)->pid = tsk->pid;
			cnt++;
		}
	}
	rcu_read_unlock();
	return cnt;
}
EXPORT_SYMBOL(mm_get_ion_detail);

size_t mm_get_ion_size_by_pid(pid_t pid)
{
	size_t size;
	struct task_struct *tsk = NULL;

	rcu_read_lock();
	tsk = find_task_by_vpid(pid);
	if (!tsk) {
		rcu_read_unlock();
		return 0;
	}
	if (tsk->flags & PF_KTHREAD) {
		rcu_read_unlock();
		return 0;
	}
	task_lock(tsk);
	size = ion_iterate_fd(tsk->files, 0,
		mm_ion_detail_cb, (void *)tsk);
	task_unlock(tsk);

	rcu_read_unlock();
	return size;
}
EXPORT_SYMBOL(mm_get_ion_size_by_pid);

void mm_ion_process_summary_info(void)
{
	struct task_struct *tsk = NULL;
	size_t tsksize;

	pr_err("Summary process info is below:\n");
	pr_err("%-16.s%3s%-16s%-16s\n", "taskname", "",
	"pid", "totalsize");

	rcu_read_lock();
	for_each_process(tsk) {
		if (tsk->flags & PF_KTHREAD)
			continue;

		task_lock(tsk);
		tsksize = ion_iterate_fd(tsk->files, 0,
				mm_ion_detail_cb, (void *)tsk);
		if (tsksize)
			pr_err("%-16.s%3s%-16d%-16zu\n",
			tsk->comm, "", tsk->pid, tsksize);
		task_unlock(tsk);
	}
	rcu_read_unlock();
}

bool is_ion_buf(const char *name)
{
	int idx;
	const char target[] = "ion-";

	if (strlen(name) < strlen(target))
		return false;

	for (idx = 0; idx < strlen(target); idx++) {
		if (name[idx] != target[idx])
			return false;
	}
	return true;
}

void mm_ion_detail_info(void)
{
	struct ion_buffer *buffer = NULL;

#if (KERNEL_VERSION(5, 4, 0) <= LINUX_VERSION_CODE)
	struct dma_buf *buf_obj = NULL;
	struct dma_buf_list db_list = get_dma_buf_list();

	mutex_lock(&db_list.lock);
	list_for_each_entry(buf_obj, &db_list.head, list_node) {
		if (buf_obj == NULL || buf_obj->priv == NULL)
			continue;

		if (mutex_lock_interruptible(&buf_obj->lock)) {
			pr_info("error locking buffer object: skipping\n");
			continue;
		}

		if (buf_obj->exp_name == NULL && !is_ion_buf(buf_obj->exp_name)) {
			mutex_unlock(&buf_obj->lock);
			continue;
		}

		buffer = (struct ion_buffer *)buf_obj->priv;
		if (buffer->heap != NULL && buffer->heap->name != NULL) {
			pr_info("%16zu %16s %s\n", buf_obj->size,
				buffer->heap->name, buf_obj->exp_name);
		}
		mutex_unlock(&buf_obj->lock);
	}
	mutex_unlock(&db_list.lock);
#else
	struct ion_device *dev = get_ion_device();
	struct rb_node *n = NULL;
	if (!dev)
		return -EINVAL;

	mutex_lock(&dev->buffer_lock);
	for (n = rb_first(&dev->buffers); n; n = rb_next(n)) {
		buffer = rb_entry(n, struct ion_buffer, node);

		if ((buffer->heap->type != ION_HEAP_TYPE_CARVEOUT) &&
			buffer->heap->name)
			pr_info("%16zu %16s\n", buffer->size, buffer->heap->name);
	}
	mutex_unlock(&dev->buffer_lock);
#endif
}

int mm_ion_memory_info(bool verbose)
{
	pr_info("ion total size:%lu\n", mm_ion_total());
	if (!verbose)
		return 0;

	mm_ion_detail_info();

	mm_ion_process_summary_info();

	mm_ion_proecss_info();

	return 0;
}
EXPORT_SYMBOL(mm_ion_memory_info);

