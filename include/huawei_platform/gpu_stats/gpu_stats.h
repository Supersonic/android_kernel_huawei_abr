/*
 * Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#ifndef HUAWEI_GPU_STATS_H
#define HUAWEI_GPU_STATS_H

#include <linux/devfreq.h>
struct hw_gpu_opp_stats {
	unsigned long long freq;
	unsigned long long active;
	unsigned long long idle;
};

#ifdef CONFIG_HUAWEI_GPU_OPP_STATS
void hw_gpu_update_opp_cost(unsigned long total_time,
	unsigned long busy_time, unsigned int idx);
int hw_gpu_query_opp_cost(struct hw_gpu_opp_stats *report, int opp_num);
int hw_gpu_init_opp_cost(struct devfreq *devfreq);
unsigned int hw_gpu_get_opp_cnt(void);
void hw_gpu_close_opp_cost(void);
#else
static void hw_gpu_update_opp_cost(unsigned long total_time,
	unsigned long busy_time, unsigned int idx) { }
static int hw_gpu_query_opp_cost(struct hw_gpu_opp_stats *report, int opp_num)
{
	return -EINVAL;
}
static int hw_gpu_init_opp_cost(struct devfreq *devfreq)
{
	return -EINVAL;
}
static unsigned int hw_gpu_get_opp_cnt(void)
{
	return 0;
}
static void hw_gpu_close_opp_cost(void) { }
#endif

#endif
