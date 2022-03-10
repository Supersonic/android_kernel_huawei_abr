/*
 * hw_futex_pi.c
 *
 * Description: customize futex pi and support qos and vip_prio inheritance
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <trace/events/sched.h>
#include <chipset_common/linux/hw_rtmutex.h>
#ifdef CONFIG_HW_QOS_THREAD
#include <chipset_common/hwqos/hwqos_common.h>
#endif
#ifdef CONFIG_HW_VIP_THREAD
#include <chipset_common/hwcfs/hwcfs_common.h>
#endif

unsigned int g_hw_futex_pi_enabled __read_mostly;

#ifdef CONFIG_HW_QOS_THREAD
static bool rt_mutex_adjust_qos(struct task_struct *p,
	struct task_struct *pi_task)
{
	int pi_qos;
	int oldqos = get_task_qos(p);
	int qos = get_task_normal_qos(p);

	if (pi_task) {
		pi_qos = get_task_qos(pi_task);
		qos = max(qos, ((pi_qos < MIN_INHERIT_QOS) ?
		    VALUE_QOS_INVALID : pi_qos));
	}

	if (qos > oldqos && pi_task) {
		trace_sched_pi_setqos(p, oldqos, qos);
		dynamic_qos_enqueue(p, pi_task, DYNAMIC_QOS_FUTEX);
		return true;
	} else if (qos < oldqos) {
		trace_sched_pi_setqos(p, oldqos, qos);
		dynamic_qos_dequeue(p, DYNAMIC_QOS_FUTEX);
		return true;
	}
	return false;
}
#endif

#ifdef CONFIG_HW_VIP_THREAD
static void rt_mutex_adjust_vip_critical(struct task_struct *p,
	struct task_struct *pi_task)
{
	bool pi_vip = pi_task ? test_task_vip(pi_task) : false;
	bool is_vip = test_dynamic_vip(p, DYNAMIC_VIP_FUTEX);

	if (!is_vip && pi_vip) {
		trace_sched_pi_setvip(p, is_vip, pi_vip);
		dynamic_vip_enqueue(p, DYNAMIC_VIP_FUTEX,
		    pi_task->vip_depth - 1);
	} else if (is_vip && !pi_vip) {
		trace_sched_pi_setvip(p, is_vip, pi_vip);
		dynamic_vip_dequeue(p, DYNAMIC_VIP_FUTEX);
	}
}
#endif

unsigned int rt_mutex_calculate_vip_prio(struct task_struct *p,
	struct task_struct *pi_task)
{
	unsigned int vip_prio = 0;

	if (unlikely(!p))
		return 0;

#ifdef CONFIG_HW_QOS_THREAD
	if (unlikely(!is_hw_futex_pi_enabled()) ||
	    !is_hw_futex_pi_qos_enabled() ||
	    !rt_mutex_adjust_qos(p, pi_task))
#else
	if (unlikely(!is_hw_futex_pi_enabled()))
#endif
#ifdef CONFIG_HUAWEI_SCHED_VIP
		return p->vip_prio;
#else
		return 0;
#endif

#ifdef CONFIG_HW_VIP_THREAD
	rt_mutex_adjust_vip_critical(p, pi_task);
#endif
#ifdef CONFIG_HUAWEI_SCHED_VIP
	if (pi_task && (pi_task->vip_prio >= MIN_INHERIT_VIP_PRIO) &&
	    (pi_task->vip_prio > p->normal_vip_prio))
		vip_prio = pi_task->vip_prio;
	else
		vip_prio = p->normal_vip_prio;
#endif
	return vip_prio;
}

#ifdef CONFIG_HUAWEI_SCHED_VIP
static inline bool vip_prio_equal(unsigned int left, unsigned int right)
{
	return (left == right) || ((left < MIN_INHERIT_VIP_PRIO) &&
	    (right < MIN_INHERIT_VIP_PRIO));
}

static inline bool prio_equal(struct task_struct *p, int prio,
	unsigned int vip_prio, bool major)
{
	return major ? vip_prio_equal(vip_prio, p->vip_prio) :
	    ((vip_prio == p->vip_prio) && (prio == p->prio));
}
#else
static inline bool prio_equal(struct task_struct *p, int prio,
	unsigned int vip_prio, bool major)
{
	return major || (prio == p->prio);
}
#endif

static inline bool mix_prio_equal(struct task_struct *p, int prio,
	unsigned int vip_prio, bool major)
{
	int rt = rt_prio(prio);

	return (rt && (prio == p->prio) && !dl_prio(prio)) ||
	    (!rt && !rt_prio(p->prio) && prio_equal(p, prio, vip_prio, major));
}

bool rt_mutex_mix_prio_equal(struct task_struct *p, int prio,
	unsigned int vip_prio, struct task_struct *pi_task)
{
	struct rt_mutex_waiter *waiter = NULL;

	if (unlikely(!p))
		return false;
	waiter = pi_task ? task_top_pi_waiter(p) : NULL;
	return likely(is_hw_futex_pi_enabled()) ?
	    mix_prio_equal(p, prio, vip_prio, waiter && waiter->major_only) :
	    (prio == p->prio && !dl_prio(prio));
}

#ifdef CONFIG_SECCOMP_FILTER
bool can_skip_filter(int this_syscall)
{
	bool is_futex = false;

	if (unlikely(READ_ONCE(current->seccomp.filter) == NULL))
		return false;
#ifdef CONFIG_COMPAT
	#define __NR_FUTEX32    240
	is_futex = ((this_syscall == __NR_FUTEX32) &&
	    test_thread_flag(TIF_32BIT)) ||
	    ((this_syscall == __NR_futex) &&
	    !test_thread_flag(TIF_32BIT));
#else
	is_futex = (this_syscall == __NR_futex);
#endif
	if (likely(!is_futex))
		return false;

	switch (task_pt_regs(current)->regs[1] & FUTEX_CMD_MASK) {
	case FUTEX_LOCK_PI:
	case FUTEX_UNLOCK_PI:
		if (likely(is_hw_futex_pi_enabled()))
			return true;
		break;
	default:
		break;
	}
	return false;
}
#endif
