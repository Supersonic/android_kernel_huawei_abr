/*
 * hw_top_task.h
 *
 * huawei top task header file
 *
 * Copyright (c) 2019, Huawei Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __HW_TOP_TASK_H__
#define __HW_TOP_TASK_H__

#include <linux/sched.h>
#ifndef CONFIG_HAVE_QCOM_TOP_TASK
#include "walt.h"
#if defined(CONFIG_HW_CPU_FREQ_GOV_SCHEDUTIL) || defined(CONFIG_HW_MIGRATION_NOTIFY)
/* Is frequency of two cpus synchronized with each other? */
static inline int same_freq_domain(int src_cpu, int dst_cpu)
{
	struct rq *rq = cpu_rq(src_cpu);

	if (src_cpu == dst_cpu)
		return 1;

	return cpumask_test_cpu(dst_cpu, &rq->freq_domain_cpumask);
}
#else
static inline int same_freq_domain(int src_cpu, int dst_cpu) { return 0; }
#endif
#endif /* !CONFIG_HAVE_QCOM_TOP_TASK */

#ifdef CONFIG_HW_TOP_TASK
#define DEFAULT_TOP_TASK_HIST_SIZE		RAVG_HIST_SIZE_MAX
#define DEFAULT_TOP_TASK_STATS_POLICY		WINDOW_STATS_RECENT
#define DEFAULT_TOP_TASK_STATS_EMPTY_WINDOW	false
#define TOP_TASK_IGNORE_UTIL 80
void rollover_top_task_load(struct task_struct *p, int nr_full_windows);
void top_task_load_update_history(struct rq *rq, struct task_struct *p,
				     u32 runtime, int samples, int event);
void update_top_task_load(struct task_struct *p, struct rq *rq,
				int event, u64 wallclock, bool account_busy);
unsigned long top_task_util(struct rq *rq);
void sugov_mark_top_task(struct rq *rq, u32 load);
#ifdef CONFIG_HAVE_QCOM_TOP_TASK
static inline unsigned int task_load_freq(struct task_struct *p) { return p->wts.load_avg; }
static inline unsigned int get_curr_load(struct task_struct *p) { return p->wts.curr_load; }
static inline unsigned int get_load_sum(struct task_struct *p) { return p->wts.load_sum; }
static inline u64 get_mark_start(struct task_struct *p) { return p->wts.mark_start; }
static inline u64 get_window_start(struct rq *rq) { return rq->wrq.window_start; }
static inline bool get_top_task_stats_empty_window(struct rq *rq)
{
	return rq->wrq.top_task_stats_empty_window;
}
static inline unsigned int* get_top_task_load_sum_history(struct task_struct *p)
{
	return &p->wts.load_sum_history[0];
}
static inline unsigned int get_top_task_stats_policy(struct rq *rq)
{
	return rq->wrq.top_task_stats_policy;
}
static inline unsigned int get_top_task_hist_size(struct rq *rq) { return rq->wrq.top_task_hist_size; }
static inline void set_prev_load(struct task_struct *p, u32 load) { p->wts.curr_load = load; }
static inline void set_curr_load(struct task_struct *p, u32 load) { p->wts.curr_load = load; }
static inline void set_load_sum(struct task_struct *p, u32 load) { p->wts.load_sum = load; }
static inline void set_load_avg(struct task_struct *p, u32 load) { p->wts.load_avg = load; }
static inline void add_load_sum(struct task_struct *p, u32 load) { p->wts.load_sum += load; }

#else
static inline unsigned int task_load_freq(struct task_struct *p) { return p->ravg.load_avg; }
static inline unsigned int get_curr_load(struct task_struct *p) { return p->ravg.curr_load; }
static inline unsigned int get_load_sum(struct task_struct *p) { return p->ravg.load_sum; }
static inline u64 get_mark_start(struct task_struct *p) { return p->ravg.mark_start; }
static inline u64 get_window_start(struct rq *rq) { return rq->window_start; }
static inline bool get_top_task_stats_empty_window(struct rq *rq)
{
	return rq->top_task_stats_empty_window;
}
static inline unsigned int* get_top_task_load_sum_history(struct task_struct *p)
{
	return &p->ravg.load_sum_history[0];
}
static inline unsigned int get_top_task_stats_policy(struct rq *rq)
{
	return rq->top_task_stats_policy;
}
static inline unsigned int get_top_task_hist_size(struct rq *rq) { return rq->top_task_hist_size; }
static inline void set_prev_load(struct task_struct *p, u32 load) { p->ravg.curr_load = load; }
static inline void set_curr_load(struct task_struct *p, u32 load) { p->ravg.curr_load = load; }
static inline void set_load_avg(struct task_struct *p, u32 load) { p->ravg.load_avg = load; }
static inline void add_load_sum(struct task_struct *p, u32 load) { p->ravg.load_sum += load; }
void top_task_exit(struct task_struct *p, struct rq *rq);
struct top_task_entry {
	u8 count, preferidle_count;
};
#endif /* !CONFIG_HAVE_QCOM_TOP_TASK */
#else /* !CONFIG_HW_TOP_TASK */
static inline unsigned long top_task_util(struct rq *rq) { return 0; }
static inline void top_task_exit(struct task_struct *p, struct rq *rq) {}
#endif /* CONFIG_HW_TOP_TASK */

#endif
