/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
 * Description: hyperhold implement
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
 * Author:	He Biao <hebiao6@huawei.com>
 *		Wang Cheng Ke <wangchengke2@huawei.com>
 *		Wang Fa <fa.wang@huawei.com>
 *
 * Create: 2020-4-16
 *
 */

#define pr_fmt(fmt) "[HYPERHOLD]" fmt

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/zsmalloc.h>

#include "hyperhold_internal.h"
#include "hyperhold_area.h"
#include "hyperhold_cache.h"
#include "hyperhold_list.h"

struct mem_cgroup *get_mem_cgroup(unsigned short mcg_id)
{
	struct mem_cgroup *mcg = NULL;

	rcu_read_lock();
	mcg = mem_cgroup_from_id(mcg_id);
	rcu_read_unlock();

	return mcg;
}

static bool fragment_dec(bool prev_flag, bool next_flag,
			 struct hyperhold_stat *stat)
{
	if (prev_flag && next_flag) {
		atomic64_inc(&stat->frag_cnt);
		return false;
	}

	if (prev_flag || next_flag)
		return false;

	return true;
}

static bool fragment_inc(bool prev_flag, bool next_flag,
			 struct hyperhold_stat *stat)
{
	if (prev_flag && next_flag) {
		atomic64_dec(&stat->frag_cnt);
		return false;
	}

	if (prev_flag || next_flag)
		return false;

	return true;
}

static bool prev_is_cont(struct hyperhold_area *area, int ext_id, int mcg_id)
{
	int prev;

	if (is_first_idx(ext_idx(area, ext_id), mcg_idx(area, mcg_id),
			 area->ext_table))
		return false;
	prev = prev_idx(ext_idx(area, ext_id), area->ext_table);

	return (prev >= 0) && (ext_idx(area, ext_id) == prev + 1);
}

static bool next_is_cont(struct hyperhold_area *area, int ext_id, int mcg_id)
{
	int next;

	if (is_last_idx(ext_idx(area, ext_id), mcg_idx(area, mcg_id),
			area->ext_table))
		return false;
	next = next_idx(ext_idx(area, ext_id), area->ext_table);

	return (next >= 0) && (ext_idx(area, ext_id) + 1 == next);
}

static void ext_fragment_sub(struct hyperhold_area *area, int ext_id)
{
	bool prev_flag = false;
	bool next_flag = false;
	int mcg_id;
	struct hyperhold_stat *stat = hyperhold_get_stat_obj();

	if (!stat) {
		hh_print(HHLOG_ERR, "NULL stat\n");
		return;
	}

	if (!area->ext_table) {
		hh_print(HHLOG_ERR, "NULL table\n");
		return;
	}
	if (ext_id < 0 || ext_id >= area->nr_exts) {
		hh_print(HHLOG_ERR, "ext = %d invalid\n", ext_id);
		return;
	}

	mcg_id = hh_list_get_mcgid(ext_idx(area, ext_id), area->ext_table);
	if (mcg_id <= 0 || mcg_id >= area->nr_mcgs) {
		hh_print(HHLOG_ERR, "mcg_id = %d invalid\n", mcg_id);
		return;
	}

	atomic64_dec(&stat->ext_cnt);
	area->mcg_id_cnt[mcg_id]--;
	if (area->mcg_id_cnt[mcg_id] == 0) {
		atomic64_dec(&stat->mcg_cnt);
		atomic64_dec(&stat->frag_cnt);
		return;
	}

	prev_flag = prev_is_cont(area, ext_id, mcg_id);
	next_flag = next_is_cont(area, ext_id, mcg_id);

	if (fragment_dec(prev_flag, next_flag, stat))
		atomic64_dec(&stat->frag_cnt);
}

static void ext_fragment_add(struct hyperhold_area *area, int ext_id)
{
	bool prev_flag = false;
	bool next_flag = false;
	int mcg_id;
	struct hyperhold_stat *stat = hyperhold_get_stat_obj();

	if (!stat) {
		hh_print(HHLOG_ERR, "NULL stat\n");
		return;
	}

	if (!area->ext_table) {
		hh_print(HHLOG_ERR, "NULL table\n");
		return;
	}
	if (ext_id < 0 || ext_id >= area->nr_exts) {
		hh_print(HHLOG_ERR, "ext = %d invalid\n", ext_id);
		return;
	}

	mcg_id = hh_list_get_mcgid(ext_idx(area, ext_id), area->ext_table);
	if (mcg_id <= 0 || mcg_id >= area->nr_mcgs) {
		hh_print(HHLOG_ERR, "mcg_id = %d invalid\n", mcg_id);
		return;
	}

	atomic64_inc(&stat->ext_cnt);
	if (area->mcg_id_cnt[mcg_id] == 0) {
		area->mcg_id_cnt[mcg_id]++;
		atomic64_inc(&stat->frag_cnt);
		atomic64_inc(&stat->mcg_cnt);
		return;
	}
	area->mcg_id_cnt[mcg_id]++;

	prev_flag = prev_is_cont(area, ext_id, mcg_id);
	next_flag = next_is_cont(area, ext_id, mcg_id);

	if (fragment_inc(prev_flag, next_flag, stat))
		atomic64_inc(&stat->frag_cnt);
}

static int extent_bit2id(struct hyperhold_area *area, int bit)
{
	if (bit < 0 || bit >= area->nr_exts) {
		hh_print(HHLOG_ERR, "bit = %d invalid\n", bit);
		return -EINVAL;
	}

	return area->nr_exts - bit - 1;
}

static int extent_id2bit(struct hyperhold_area *area, int id)
{
	if (id < 0 || id >= area->nr_exts) {
		hh_print(HHLOG_ERR, "id = %d invalid\n", id);
		return -EINVAL;
	}

	return area->nr_exts - id - 1;
}

int obj_idx(struct hyperhold_area *area, int idx)
{
	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (idx < 0 || idx >= area->nr_objs) {
		hh_print(HHLOG_ERR, "idx = %d invalid\n", idx);
		return -EINVAL;
	}

	return idx;
}

int ext_idx(struct hyperhold_area *area, int idx)
{
	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (idx < 0 || idx >= area->nr_exts) {
		hh_print(HHLOG_ERR, "idx = %d invalid\n", idx);
		return -EINVAL;
	}

	return idx + area->nr_objs;
}

int mcg_idx(struct hyperhold_area *area, int idx)
{
	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (idx <= 0 || idx >= area->nr_mcgs) {
		hh_print(HHLOG_ERR, "idx = %d invalid\n", idx);
		return -EINVAL;
	}

	return idx + area->nr_objs + area->nr_exts;
}

static struct hh_list_head *get_obj_table_node(int idx, void *private)
{
	struct hyperhold_area *area = private;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return NULL;
	}
	if (idx < 0) {
		hh_print(HHLOG_ERR, "idx = %d invalid\n", idx);
		return NULL;
	}
	if (idx < area->nr_objs)
		return &area->lru[idx];
	idx -= area->nr_objs;
	if (idx < area->nr_exts)
		return &area->rmap[idx];
	idx -= area->nr_exts;
	if (idx > 0 && idx < area->nr_mcgs) {
		struct mem_cgroup *mcg = get_mem_cgroup(idx);

		if (!mcg)
			goto err_out;
		return (struct hh_list_head *)(&mcg->zram_lru);
	}
err_out:
	hh_print(HHLOG_ERR, "idx = %d invalid\n", idx);

	return NULL;
}

static void free_obj_list_table(struct hyperhold_area *area)
{
	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return;
	}

	if (area->lru) {
		vfree(area->lru);
		area->lru = NULL;
	}
	if (area->rmap) {
		vfree(area->rmap);
		area->rmap = NULL;
	}

	kfree(area->obj_table);
	area->obj_table = NULL;
}

static int init_obj_list_table(struct hyperhold_area *area)
{
	int i;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}

	area->lru = vzalloc(sizeof(struct hh_list_head) * area->nr_objs);
	if (!area->lru) {
		hh_print(HHLOG_ERR, "area->lru alloc failed\n");
		goto err_out;
	}
	area->rmap = vzalloc(sizeof(struct hh_list_head) * area->nr_exts);
	if (!area->rmap) {
		hh_print(HHLOG_ERR, "area->rmap alloc failed\n");
		goto err_out;
	}
	area->obj_table = alloc_table(get_obj_table_node, area, GFP_KERNEL);
	if (!area->obj_table) {
		hh_print(HHLOG_ERR, "area->obj_table alloc failed\n");
		goto err_out;
	}
	for (i = 0; i < area->nr_objs; i++)
		hh_list_init(obj_idx(area, i), area->obj_table);
	for (i = 0; i < area->nr_exts; i++)
		hh_list_init(ext_idx(area, i), area->obj_table);

	hh_print(HHLOG_INFO, "hyperhold obj list table init OK.\n");
	return 0;
err_out:
	free_obj_list_table(area);
	hh_print(HHLOG_ERR, "hyperhold obj list table init failed.\n");

	return -ENOMEM;
}

static struct hh_list_head *get_ext_table_node(int idx, void *private)
{
	struct hyperhold_area *area = private;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return NULL;
	}

	if (idx < area->nr_objs)
		goto err_out;
	idx -= area->nr_objs;
	if (idx < area->nr_exts)
		return &area->ext[idx];
	idx -= area->nr_exts;
	if (idx > 0 && idx < area->nr_mcgs) {
		struct mem_cgroup *mcg = get_mem_cgroup(idx);

		if (!mcg)
			return NULL;
		return (struct hh_list_head *)(&mcg->ext_lru);
	}
err_out:
	hh_print(HHLOG_ERR, "idx = %d invalid\n", idx);

	return NULL;
}

static void free_ext_list_table(struct hyperhold_area *area)
{
	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return;
	}

	if (area->ext)
		vfree(area->ext);

	kfree(area->ext_table);
}

static int init_ext_list_table(struct hyperhold_area *area)
{
	int i;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	area->ext = vzalloc(sizeof(struct hh_list_head) * area->nr_exts);
	if (!area->ext)
		goto err_out;
	area->ext_table = alloc_table(get_ext_table_node, area, GFP_KERNEL);
	if (!area->ext_table)
		goto err_out;
	for (i = 0; i < area->nr_exts; i++)
		hh_list_init(ext_idx(area, i), area->ext_table);
	hh_print(HHLOG_INFO, "hyperhold ext list table init OK.\n");
	return 0;
err_out:
	free_ext_list_table(area);
	hh_print(HHLOG_ERR, "hyperhold ext list table init failed.\n");

	return -ENOMEM;
}

unsigned long hyperhold_memcg_shrink_cache(struct hyperhold_area *area,
		struct mem_cgroup *mcg,
		unsigned long nr_pages)
{
	int idx;
	unsigned long freed = 0;
	int mcg_id = mcg->id.id;

	hh_lock_list(mcg_idx(area, mcg_id), area->ext_table);
	hh_list_for_each_entry_reverse(idx, mcg_idx(area, mcg_id),
					area->ext_table) {
		int ext_id = idx - area->nr_objs;
		long size = atomic64_read(&area->ext_stored_size[ext_id]);
		if (size < CACHE_SIZE_LOW)
			continue;
		if (!hyperhold_cache_delete(area->cache, ext_id)) {
			freed += EXTENT_PG_CNT;
			if (freed >= nr_pages)
				break;
		}
	}
	hh_unlock_list(mcg_idx(area, mcg_id), area->ext_table);

	return freed;
}

unsigned long hyperhold_shrink_cache(struct hyperhold_area *area,
		unsigned long nr_pages)
{
	unsigned long freed = 0;
	struct mem_cgroup *mcg = NULL;

	for (mcg = get_next_memcg(NULL); mcg; mcg = get_next_memcg(mcg)) {
		if (atomic64_read(&mcg->memcg_reclaimed.app_score) == 0)
			break;
		if (!mcg->zram)
			continue;
		freed += hyperhold_memcg_shrink_cache(area, mcg,
			nr_pages - freed);
		if (freed >= nr_pages)
			break;
	}

	if (mcg) {
		get_next_memcg_break(mcg);
		mcg = NULL;
	}

	return freed;
}

unsigned long hyperhold_cache_count_objs(struct shrinker *sh,
		struct shrink_control *sc)
{
	struct hyperhold_area *area =
			container_of(sh, struct hyperhold_area, cache_shrinker);

	hh_print(HHLOG_DEBUG, "run cache counter\n");
	return (unsigned long)area->cache->nr_cached_exts * EXTENT_PG_CNT;
}

unsigned long hyperhold_cache_scan_objs(struct shrinker *sh,
		struct shrink_control *sc)
{
	struct hyperhold_area *area =
			container_of(sh, struct hyperhold_area, cache_shrinker);
	unsigned long freed = 0;

	atomic64_inc(&area->cache_shrinker_runs);
	hh_print(HHLOG_DEBUG, "run cache shrinker, total %ld, nr = %lu\n",
		atomic64_read(&area->cache_shrinker_runs), sc->nr_to_scan);
	freed = hyperhold_shrink_cache(area, sc->nr_to_scan);
	atomic64_add(freed, &area->cache_shrinker_reclaim_pages);
	hh_print(HHLOG_DEBUG, "cache shrinker reclaimed %ld, total %ld\n",
		freed, atomic64_read(&area->cache_shrinker_reclaim_pages));

	return freed;
}

void free_hyperhold_area(struct hyperhold_area *area)
{
	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return;
	}

	vfree(area->bitmap);
	vfree(area->ext_stored_pages);
	vfree(area->ext_stored_size);
	hyperhold_cache_free(area->cache);
	unregister_shrinker(&area->cache_shrinker);
	free_obj_list_table(area);
	free_ext_list_table(area);
	vfree(area);
}

bool hp_cache_init(struct hyperhold_area *area)
{
	area->fault_out_cache_enable = false;
	area->batch_out_cache_enable = false;
	area->reclaim_in_cache_enable = false;
	area->cache = hyperhold_cache_alloc(area->nr_exts);
	if (!area->cache) {
		hh_print(HHLOG_ERR, "alloc hyperhold cache failed\n");
	return false;
	}
	area->cache_shrinker.count_objects = hyperhold_cache_count_objs;
	area->cache_shrinker.scan_objects = hyperhold_cache_scan_objs;
	area->cache_shrinker.seeks = DEFAULT_SEEKS;
	if (register_shrinker(&area->cache_shrinker)) {
		hh_print(HHLOG_ERR, "register cache shrinker failed\n");
	return false;
	}
	return true;
}

struct hyperhold_area *alloc_hyperhold_area(unsigned long ori_size,
					    unsigned long comp_size)
{
	struct hyperhold_area *area = vzalloc(sizeof(struct hyperhold_area));
	if (!area) {
		hh_print(HHLOG_ERR, "area alloc failed\n");
		goto err_out;
	}
	if (comp_size & (EXTENT_SIZE - 1)) {
		hh_print(HHLOG_ERR,
			 "disksize = %ld align invalid (32K align needed)\n",
			 comp_size);
		goto err_out;
	}
	area->size = comp_size;
	area->nr_exts = comp_size >> EXTENT_SHIFT;
	area->nr_mcgs = MEM_CGROUP_ID_MAX;
	area->nr_objs = ori_size >> PAGE_SHIFT;
	area->bitmap = vzalloc(BITS_TO_LONGS(area->nr_exts) * sizeof(long));
	if (!area->bitmap) {
		hh_print(HHLOG_ERR, "area->bitmap alloc failed\n");
		goto err_out;
	}
	area->ext_stored_pages = vzalloc(sizeof(atomic_t) * area->nr_exts);
	if (!area->ext_stored_pages) {
		hh_print(HHLOG_ERR, "area->ext_stored_pages alloc failed\n");
		goto err_out;
	}
	area->ext_stored_size = vzalloc(sizeof(atomic64_t) * area->nr_exts);
	if (!area->ext_stored_size) {
		hh_print(HHLOG_ERR, "area->ext_stored_size alloc failed\n");
		goto err_out;
	}
	if (!hp_cache_init(area))
	    goto err_out;
	if (init_obj_list_table(area)) {
		hh_print(HHLOG_ERR, "init obj list table failed\n");
		goto err_out;
	}
	if (init_ext_list_table(area)) {
		hh_print(HHLOG_ERR, "init ext list table failed\n");
		goto err_out;
	}
	hh_print(HHLOG_INFO, "hyperhold_area init OK.\n");
	return area;
err_out:
	free_hyperhold_area(area);
	hh_print(HHLOG_ERR, "hyperhold_area init failed.\n");
	return NULL;
}

void hyperhold_free_extent(struct hyperhold_area *area, int ext_id)
{
	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return;
	}
	if (ext_id < 0 || ext_id >= area->nr_exts) {
		hh_print(HHLOG_ERR, "INVALID ext %d\n", ext_id);
		return;
	}
	hh_print(HHLOG_DEBUG, "free ext id = %d.\n", ext_id);

	if (hyperhold_cache_delete(area->cache, ext_id) == -EBUSY) {
		hh_print(HHLOG_ERR, "delete busy cache, ext = %d\n", ext_id);
		WARN_ON_ONCE(1);
	}

	hh_list_set_mcgid(ext_idx(area, ext_id), area->ext_table, 0);
	/*lint -e548*/
	if (!test_and_clear_bit(extent_id2bit(area, ext_id), area->bitmap)) {
		hh_print(HHLOG_ERR, "bit not set, ext = %d\n", ext_id);
		WARN_ON_ONCE(1);
	}
	/*lint +e548*/
	atomic_dec(&area->stored_exts);
}

static int alloc_bitmap(unsigned long *bitmap, int max, int last_bit)
{
	int bit;

	if (!bitmap) {
		hh_print(HHLOG_ERR, "NULL bitmap.\n");
		return -EINVAL;
	}
retry:
	bit = find_next_zero_bit(bitmap, max, last_bit);
	if (bit == max) {
		if (last_bit == 0) {
			hh_print(HHLOG_ERR, "alloc bitmap failed.\n");
			return -ENOSPC;
		}
		last_bit = 0;
		goto retry;
	}
	if (test_and_set_bit(bit, bitmap))
		goto retry;

	return bit;
}

int hyperhold_alloc_extent(struct hyperhold_area *area, struct mem_cgroup *mcg)
{
	int last_bit;
	int bit;
	int ext_id;
	int mcg_id;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (!mcg) {
		hh_print(HHLOG_ERR, "NULL mcg\n");
		return -EINVAL;
	}

	last_bit = atomic_read(&area->last_alloc_bit);
	hh_print(HHLOG_DEBUG, "last_bit = %d.\n", last_bit);
	bit = alloc_bitmap(area->bitmap, area->nr_exts, last_bit);
	if (bit < 0) {
		hh_print(HHLOG_ERR, "alloc bitmap failed.\n");
		return bit;
	}
	ext_id = extent_bit2id(area, bit);
	/*lint -e548*/
	mcg_id = hh_list_get_mcgid(ext_idx(area, ext_id), area->ext_table);
	if (mcg_id) {
		hh_print(HHLOG_ERR, "already has mcg %d, ext %d\n",
				mcg_id, ext_id);
		goto err_out;
	}
	/*lint +e548*/
	hh_list_set_mcgid(ext_idx(area, ext_id), area->ext_table, mcg->id.id);

	atomic_set(&area->last_alloc_bit, bit);
	atomic_inc(&area->stored_exts);
	hh_print(HHLOG_DEBUG, "extent %d init OK.\n", ext_id);
	hh_print(HHLOG_DEBUG, "mcg_id = %d, ext id = %d\n", mcg->id.id, ext_id);

	return ext_id;
err_out:
	clear_bit(bit, area->bitmap);
	WARN_ON_ONCE(1); /*lint !e548*/
	return -EBUSY;

}

int get_extent(struct hyperhold_area *area, int ext_id)
{
	int mcg_id;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (ext_id < 0 || ext_id >= area->nr_exts) {
		hh_print(HHLOG_ERR, "ext = %d invalid\n", ext_id);
		return -EINVAL;
	}

	if (!hh_list_clear_priv(ext_idx(area, ext_id), area->ext_table))
		return -EBUSY;
	mcg_id = hh_list_get_mcgid(ext_idx(area, ext_id), area->ext_table);
	if (mcg_id) {
		ext_fragment_sub(area, ext_id);
		hh_list_del(ext_idx(area, ext_id), mcg_idx(area, mcg_id),
			    area->ext_table);
	}
	hh_print(HHLOG_DEBUG, "ext id = %d\n", ext_id);

	return ext_id;
}

void put_extent(struct hyperhold_area *area, int ext_id)
{
	int mcg_id;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return;
	}
	if (ext_id < 0 || ext_id >= area->nr_exts) {
		hh_print(HHLOG_ERR, "ext = %d invalid\n", ext_id);
		return;
	}

	mcg_id = hh_list_get_mcgid(ext_idx(area, ext_id), area->ext_table);
	if (mcg_id) {
		hh_lock_list(mcg_idx(area, mcg_id), area->ext_table);
		hh_list_add_nolock(ext_idx(area, ext_id), mcg_idx(area, mcg_id),
			area->ext_table);
		ext_fragment_add(area, ext_id);
		hh_unlock_list(mcg_idx(area, mcg_id), area->ext_table);
	}
	/*lint -e548*/
	if (!hh_list_set_priv(ext_idx(area, ext_id), area->ext_table)) {
		hh_print(HHLOG_ERR, "private not set, ext = %d\n", ext_id);
		WARN_ON_ONCE(1);
		return;
	}
	/*lint +e548*/
	hh_print(HHLOG_DEBUG, "put extent %d.\n", ext_id);
}

int get_memcg_extent(struct hyperhold_area *area, struct mem_cgroup *mcg,
		bool (*filter)(struct hyperhold_area *area, int ext_id))
{
	int mcg_id;
	int ext_id = -ENOENT;
	int idx;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (!area->ext_table) {
		hh_print(HHLOG_ERR, "NULL table\n");
		return -EINVAL;
	}
	if (!mcg) {
		hh_print(HHLOG_ERR, "NULL mcg\n");
		return -EINVAL;
	}

	mcg_id = mcg->id.id;
	hh_lock_list(mcg_idx(area, mcg_id), area->ext_table);
	hh_list_for_each_entry(idx, mcg_idx(area, mcg_id), area->ext_table) { /*lint !e666*/
		if (filter && !filter(area, idx - area->nr_objs))
			continue;
		if (hh_list_clear_priv(idx, area->ext_table)) {
			ext_id = idx - area->nr_objs;
			break;
		}
	}
	if (ext_id >= 0 && ext_id < area->nr_exts) {
		ext_fragment_sub(area, ext_id);
		hh_list_del_nolock(idx, mcg_idx(area, mcg_id), area->ext_table);
		hh_print(HHLOG_DEBUG, "ext id = %d\n", ext_id);
	}
	hh_unlock_list(mcg_idx(area, mcg_id), area->ext_table);

	return ext_id;
}

int get_memcg_zram_entry(struct hyperhold_area *area, struct mem_cgroup *mcg)
{
	int mcg_id, idx;
	int index = -ENOENT;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (!area->obj_table) {
		hh_print(HHLOG_ERR, "NULL table\n");
		return -EINVAL;
	}
	if (!mcg) {
		hh_print(HHLOG_ERR, "NULL mcg\n");
		return -EINVAL;
	}

	mcg_id = mcg->id.id;
	hh_lock_list(mcg_idx(area, mcg_id), area->obj_table);
	hh_list_for_each_entry(idx, mcg_idx(area, mcg_id), area->obj_table) { /*lint !e666*/
		index = idx;
		break;
	}
	hh_unlock_list(mcg_idx(area, mcg_id), area->obj_table);

	return index;
}

int get_extent_zram_entry(struct hyperhold_area *area, int ext_id)
{
	int index = -ENOENT;
	int idx;

	if (!area) {
		hh_print(HHLOG_ERR, "NULL area\n");
		return -EINVAL;
	}
	if (!area->obj_table) {
		hh_print(HHLOG_ERR, "NULL table\n");
		return -EINVAL;
	}
	if (ext_id < 0 || ext_id >= area->nr_exts) {
		hh_print(HHLOG_ERR, "ext = %d invalid\n", ext_id);
		return -EINVAL;
	}

	hh_lock_list(ext_idx(area, ext_id), area->obj_table);
	hh_list_for_each_entry(idx, ext_idx(area, ext_id), area->obj_table) { /*lint !e666*/
		index = idx;
		break;
	}
	hh_unlock_list(ext_idx(area, ext_id), area->obj_table);

	return index;
}
int hp_high_ext_num(int tot)
{
	return (tot) * AREA_WATERMARK_HIGH / 100;
}

int hp_low_ext_num(int tot)
{
	return (tot) * AREA_WATERMARK_LOW / 100;
}

int esentry_extid(unsigned long e)
{
	return (e) >> EXTENT_SHIFT;
}

int esentry_pgid(unsigned long e)
{
	return ((e) & ((1<<EXTENT_SHIFT) - 1))>> PAGE_SHIFT;
}

int esentry_pgoff(unsigned long e)
{
	return (e) & (PAGE_SIZE - 1);
}
