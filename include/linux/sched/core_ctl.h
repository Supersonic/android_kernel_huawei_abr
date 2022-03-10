/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2016, 2019-2020, The Linux Foundation. All rights reserved.
 */

#ifndef __CORE_CTL_H
#define __CORE_CTL_H

#include <linux/types.h>

#define MAX_CPUS_PER_CLUSTER 6
#define MAX_CLUSTERS 3

#define BIG_CPU 7
#define BIG_CLUSTER_ISOLATE 1
#define BIG_CLUSTER_UNISOLATE 0

struct core_ctl_notif_data {
	unsigned int nr_big;
	unsigned int coloc_load_pct;
	unsigned int ta_util_pct[MAX_CLUSTERS];
	unsigned int cur_cap_pct[MAX_CLUSTERS];
};

struct notifier_block;

#ifdef CONFIG_SCHED_WALT
extern int core_ctl_set_boost(bool boost);
extern void core_ctl_notifier_register(struct notifier_block *n);
extern void core_ctl_notifier_unregister(struct notifier_block *n);
extern void isolate_unisolate_notifier_register(struct notifier_block *n);
extern void isolate_unisolate_notifier_unregister(struct notifier_block *n);
extern void __core_ctl_check(u64 wallclock);
#else
static inline int core_ctl_set_boost(bool boost)
{
	return 0;
}
static inline void core_ctl_notifier_register(struct notifier_block *n) {}
static inline void core_ctl_notifier_unregister(struct notifier_block *n) {}
static inline void isolate_unisolate_notifier_register(struct notifier_block *n) {}
static inline void isolate_unisolate_notifier_unregister(struct notifier_block *n) {}
static inline void _core_ctl_check(u64 wallclock) {}
#endif
#endif
