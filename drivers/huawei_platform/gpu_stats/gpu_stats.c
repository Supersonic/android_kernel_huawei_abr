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

#include <huawei_platform/gpu_stats/gpu_stats.h>
#include <linux/slab.h>

static struct hw_gpu_opp_stats *freq_stats;
static unsigned int opp_size;

int hw_gpu_query_opp_cost(struct hw_gpu_opp_stats *report, int opp_num)
{
	if (report == NULL)
		return -EINVAL;
	if (freq_stats == NULL)
		return -EINVAL;
	if (opp_size == opp_num) {
		memcpy(report, freq_stats, opp_num * sizeof(struct hw_gpu_opp_stats));
		return 0;
	}

	return -EINVAL;
}

unsigned int hw_gpu_get_opp_cnt(void)
{
	return opp_size;
}

void hw_gpu_update_opp_cost(unsigned long total_time,
	unsigned long busy_time, unsigned int idx)
{
	if (freq_stats && idx < opp_size) {
		freq_stats[idx].active += busy_time;
		freq_stats[idx].idle += total_time - busy_time;
	}

	return;
}

int hw_gpu_init_opp_cost(struct devfreq *devfreq)
{
	int i;

	if (devfreq == NULL)
		return -EINVAL;
	if (devfreq->profile == NULL)
		return -EINVAL;
	if (devfreq->profile->freq_table == NULL)
		return -EINVAL;

	opp_size = devfreq->profile->max_state;

	if (opp_size <= 0)
		return -EINVAL;

	freq_stats = kzalloc(sizeof(struct hw_gpu_opp_stats) * opp_size, GFP_KERNEL);
	if (freq_stats == NULL) {
		hw_gpu_close_opp_cost();
		return -EINVAL;
	}

	for (i = 0; i < opp_size; i++)
		freq_stats[i].freq = devfreq->profile->freq_table[i];

	return 0;
}

void hw_gpu_close_opp_cost(void)
{
	opp_size = 0;
	if (freq_stats) {
		kfree(freq_stats);
		freq_stats = NULL;
	}
}
