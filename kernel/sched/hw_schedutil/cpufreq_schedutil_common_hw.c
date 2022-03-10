#ifdef CONFIG_HW_ED_TASK
#include "../hw_task/hw_ed_task.h"
#endif
#include <linux/version.h>

#ifndef CONFIG_ARCH_LAHAINA
#ifndef SCHED_CPUFREQ_RT
#define SCHED_CPUFREQ_RT 0
#endif
#endif

/*
 * cpufreq_schedutil_common_hw.c
 *
 * Copyright (c) 2016-2020 Huawei Technologies Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

void sugov_mark_util_change(int cpu, unsigned int flags)
{
	struct sugov_cpu *sg_cpu = &per_cpu(sugov_cpu, cpu);
	struct sugov_policy *sg_policy = NULL;
#ifdef CONFIG_HW_CPU_FREQ_GOV_SCHEDUTIL
	struct sugov_cpu_hw *sg_cpu_hw = &sg_cpu->sg_cpu_hw;
	bool skip_min_sample_time = false;
	bool skip_hispeed_logic = false;
	struct sugov_tunables_hw *tunables_hw = NULL;
	struct sugov_policy_hw *sg_policy_hw = NULL;
#endif

	sg_policy = sg_cpu->sg_policy;
	if (!sg_policy)
		return;

#ifdef CONFIG_HW_CPU_FREQ_GOV_SCHEDUTIL
	if (!sg_cpu_hw->enabled)
		return;

	sg_policy_hw = &sg_policy->sg_policy_hw;
	if (!sg_policy_hw->governor_enabled)
		return;

	tunables_hw = &sg_policy->tunables->tunables_hw;
	flags = check_freq_reporting_policy(cpu, flags);
	if (!flags)
		return;

	sg_cpu->flags |= flags;

	if (flags & INTER_CLUSTER_MIGRATION_SRC)
		if (tunables_hw->fast_ramp_down)
			skip_min_sample_time = true;

	if (flags & INTER_CLUSTER_MIGRATION_DST)
		if (tunables_hw->fast_ramp_up)
			skip_hispeed_logic = true;

#ifdef CONFIG_HW_ED_TASK
	if (flags & CLEAR_ED_TASK)
		skip_min_sample_time = true;

	if (flags & ADD_ED_TASK)
		skip_hispeed_logic = true;
#endif

#ifdef CONFIG_HW_TOP_TASK_SKIP_HISPEED_LOGIC
	if (flags & ADD_TOP_TASK)
		skip_hispeed_logic = true;
#endif
	if (flags & FORCE_UPDATE) {
		skip_min_sample_time = true;
		skip_hispeed_logic = true;
	}

#ifdef CONFIG_HW_RTG_FRAME
	if (flags & FRAME_UPDATE) {
#ifdef CONFIG_HW_RTG_FRAME_NO_FORCE_FAST_DOWN
		if (tunables_hw->fast_ramp_down)
#endif
			skip_min_sample_time = true;
		skip_hispeed_logic = true;
	}
#endif

#ifdef CONFIG_SCHED_TASK_UTIL_CLAMP
	if (flags & (RQ_SET_MIN_UTIL | RQ_ENQUEUE_MIN_UTIL))
		skip_hispeed_logic = true;
#endif

	if (skip_min_sample_time)
		atomic_set(&sg_policy_hw->skip_min_sample_time, 1);
	if (skip_hispeed_logic)
		atomic_set(&sg_policy_hw->skip_hispeed_logic, 1);
#endif

	sg_policy->util_changed = true;
}

void sugov_check_freq_update(int cpu)
{
	struct sugov_cpu *sg_cpu = NULL;
	struct sugov_policy *sg_policy = NULL;

#ifdef CONFIG_HW_CPU_FREQ_GOV_SCHEDUTIL
	struct sugov_cpu_hw *sg_cpu_hw = NULL;
	struct sugov_policy_hw *sg_policy_hw = NULL;
#endif

	if (cpu >= nr_cpu_ids) /*lint !e737*/
		return;

	sg_cpu = &per_cpu(sugov_cpu, cpu);
	sg_policy = sg_cpu->sg_policy;
	if (!sg_policy)
		return;

#ifdef CONFIG_HW_CPU_FREQ_GOV_SCHEDUTIL
	sg_cpu_hw = &sg_cpu->sg_cpu_hw;
	if (!sg_cpu_hw->enabled)
		return;

	sg_policy_hw = &sg_policy->sg_policy_hw;
	if (!sg_policy_hw->governor_enabled)
		return;
#endif

	if (sg_policy->util_changed)
#ifdef CONFIG_ARCH_LAHAINA
		cpufreq_update_util(cpu_rq(cpu), SCHED_CPUFREQ_HW);
#else
		cpufreq_update_util(cpu_rq(cpu), SCHED_CPUFREQ_RT);
#endif
}

/* Prevent rasing sugov work in very low load case. */
static bool should_update_freq(int cpu)
{
#ifndef CONFIG_HAVE_QCOM_SCHED_WALT
	struct rq *rq = cpu_rq(cpu);
	if (rq->cluster->cur_freq > rq->cluster->min_freq)
		return true;
#endif

	if (boosted_freq_policy_util(cpu) >
	    (capacity_curr_of(cpu) >> 1))
		return true;

	if (sugov_iowait_boost_pending(cpu))
		return true;

	/*
	 * Lowest freq and low util and no iowait boost.
	 * No need to trigger freq update.
	 */
	return false;
}

void mark_util_change_for_rollover(struct task_struct *p, struct rq *rq)
{
	int cpu = cpu_of(rq);

	/* Rollover condition: p is curr task and p sees new window. */
	if (p != rq->curr)
		return;
#ifndef CONFIG_HAVE_QCOM_SCHED_WALT
#ifdef CONFIG_HW_SCHED_PRED_LOAD
	/* When pred load in use, mark PRED_LOAD_WINDOW_ROLLOVER only. */
	if (use_pred_load(cpu)) {
		if (p->ravg.mark_start < rq->predl_window_start) {
			if (should_update_freq(cpu))
				sugov_mark_util_change(cpu,
					PRED_LOAD_WINDOW_ROLLOVER);

			trace_predl_window_rollover(cpu);
		}

		return;
	}
#endif
#endif

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0) )
	if (p->wts.mark_start < rq->wrq.window_start) {
#else
	if (p->ravg.mark_start < rq->window_start) {
#endif
		if (should_update_freq(cpu))
			sugov_mark_util_change(cpu, WALT_WINDOW_ROLLOVER);

		trace_walt_window_rollover(cpu);
	}
}
