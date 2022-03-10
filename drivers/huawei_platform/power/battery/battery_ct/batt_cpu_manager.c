// SPDX-License-Identifier: GPL-2.0
/*
 * batt_cpu_manager.c
 *
 * cpu manager for batt_ct
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#include "batt_cpu_manager.h"
#include <linux/cpu.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>
#include <huawei_platform/hwpower/common_module/power_platform.h>

#define HWLOG_TAG batt_cpu_manager
HWLOG_REGIST();

static struct mutex g_manager_lock;
static int g_is_inited;

static int batt_cpu_isolate(bool plug, unsigned int cpu)
{
	int cnt = 3; /* retry count for cpu isolate */
	int ret;

	do {
		if (plug)
			ret = sched_unisolate_cpu(cpu);
		else
			ret = sched_isolate_cpu(cpu);

		if (!ret)
			return ret;
	} while (--cnt > 0);

	return ret;
}

int batt_cpu_single_core_set(void)
{
	int cpu_id;
	int ret;

	hwlog_info("[%s] begin locking\n", __func__);
	mutex_lock(&g_manager_lock);
	hwlog_info("[%s] locking\n", __func__);

	for (cpu_id = 1; cpu_id < BATT_CORE_NUM; cpu_id++) {
		ret = batt_cpu_isolate(false, cpu_id);
		if (ret) {
			hwlog_err("sched_isolate_cpu fail, id %d : ret %d\n",
				cpu_id, ret);
			return -EINVAL;
		}
	}
	hwlog_info("[%s] sched_isolate_cpu sucess\n", __func__);

	return 0;
}

void batt_turn_on_all_cpu_cores(void)
{
	int cpu_id;
	int ret;

	for (cpu_id = 1; cpu_id < BATT_CORE_NUM; cpu_id++) {
		ret = batt_cpu_isolate(true, cpu_id);
		if (ret)
			hwlog_err("cpu_up fail, id %d : ret %d\n",
				cpu_id, ret);
	}

	mutex_unlock(&g_manager_lock);
	hwlog_info("[%s] sched_unisolate_cpu end\n", __func__);
}

static int __init batt_cpu_manager_init(void)
{
	if (!g_is_inited) {
		mutex_init(&g_manager_lock);
		g_is_inited = 1;
	}
	return 0;
}

static void __exit batt_cpu_manager_exit(void)
{
	return;
}

subsys_initcall_sync(batt_cpu_manager_init);
module_exit(batt_cpu_manager_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("batt cpu manager module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
