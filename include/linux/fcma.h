/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __FCMA_H__
#define __FCMA_H__

#include <linux/types.h>

#define FCMA_PAGE_HINT 0x666

struct cma;

extern int fcma_cluster_max;

int fcma_init_reserved_mem(phys_addr_t base, phys_addr_t size,
			   unsigned int order_per_bit,
			   const char *name,
			   struct cma **res_cma);
void fcma_replace_page(struct page *page);
void set_fcma_migrate_flag(enum migratetype);
void clear_fcma_migrate_flag(enum migratetype);
int test_fcma_migrate_flag(void);
int in_fcma_area(struct page *page);
int get_fcma_page_num(void);
void fcma_page_num_add(int);
void fcma_page_num_sub(int);
void fcma_page_num_dec(void);
void fcma_page_num_inc(void);
#endif
