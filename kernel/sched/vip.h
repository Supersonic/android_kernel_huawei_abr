/*
 * vip.h
 *
 * huawei vip scheduling
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __VIP_H__
#define __VIP_H__

struct sg_lb_stats;
struct sd_lb_stats;
struct lb_env;

enum vip_preempt_type {
	NO_VIP_PREEMPT = 0,
	VIP_PREEMPT_OTHER,
	VIP_PREEMPT_TOPAPP,
	VIP_PREEMPT_VIP,
};

struct rq;
extern unsigned int vip_task_load_balance_delay; /* 1ms */

#ifdef CONFIG_HUAWEI_SCHED_VIP
extern void sched_enqueue_vip_thread(struct rq *rq, struct task_struct *p);
extern void sched_dequeue_vip_thread(struct rq *rq, struct task_struct *p);
extern struct task_struct *pick_next_vip_thread(struct rq *rq);

extern int find_vip_cpu(struct task_struct *p);
extern int
find_lowest_vip_cpu(struct task_struct *p, struct cpumask *search_cpus);

extern enum vip_preempt_type
vip_preempt_classify(struct rq *rq, struct task_struct *prev,
		     struct task_struct *next, int wake_sync);

extern void fixup_vip_preempt_type(struct rq *rq);

extern unsigned int select_allowed_cpu(struct task_struct *p,
				       struct cpumask *allowed_cpus);

extern void sched_vip_init_rq_data(struct rq *rq);

extern void sched_vip_thread(struct rq *rq, struct task_struct *prev,
			     struct task_struct *next, int wake_sync);

extern bool
should_balance_vip_thread(struct sg_lb_stats *busiest,
			  struct lb_env *env,
			  struct sd_lb_stats *sds);

extern bool check_for_migration_vip_thread(struct task_struct *p,
					   int prev_cpu, int *new_cpu);
#else
static inline void
sched_enqueue_vip_thread(struct rq *rq, struct task_struct *p) {}
static inline void
sched_dequeue_vip_thread(struct rq *rq, struct task_struct *p) {}
static inline struct task_struct *pick_next_vip_thread(struct rq *rq)
{
	return NULL;
}

static inline int find_vip_cpu(struct task_struct *p)
{
	return -1;
}

static inline int
find_lowest_vip_cpu(struct task_struct *p, struct cpumask *search_cpus)
{
	return -1;
}

static inline enum vip_preempt_type
vip_preempt_classify(struct rq *rq, struct task_struct *prev,
		     struct task_struct *next, int wake_sync)
{
	return NO_VIP_PREEMPT;
}

static inline void fixup_vip_preempt_type(struct rq *rq)
{
}

static inline unsigned int select_allowed_cpu(struct task_struct *p,
					      struct cpumask *allowed_cpus)
{
	return -1;
}

static inline void sched_vip_init_rq_data(struct rq *rq)
{
}

static inline void sched_vip_thread(struct rq *rq, struct task_struct *prev,
				    struct task_struct *next, int wake_sync)
{
}

static inline bool
should_balance_vip_thread(struct sg_lb_stats *busiest,
			  struct lb_env *env,
			  struct sd_lb_stats *sds)
{
	return false;
}

static inline bool
check_for_migration_vip_thread(struct task_struct *p, int prev_cpu, int *new_cpu)
{
	return false;
}
#endif
#endif
