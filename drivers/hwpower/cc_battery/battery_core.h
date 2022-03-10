/* SPDX-License-Identifier: GPL-2.0 */
/*
 * battery_core.h
 *
 * huawei battery core features
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
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

#ifndef _BATTERY_CORE_H_
#define _BATTERY_CORE_H_

#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/notifier.h>
#include <chipset_common/hwpower/common_module/power_algorithm.h>
#include <chipset_common/hwpower/coul/coul_interface.h>

#define BAT_CORE_WORK_PARA_COL          3
#define BAT_CORE_WORK_PARA_ROW          6
#define BAT_CORE_WORK_INTERVAL_NORMAL   10000
#define BAT_CORE_WORK_INTERVAL_MAX      30000
#define BAT_CORE_CAPACITY_FULL          100
#define BAT_CORE_COLD_TEMP              (-200)
#define BAT_CORE_OVERHEAT_TEMP          600

#define BAT_CORE_TEMP_TYPE_RAW_COMP     0
#define BAT_CORE_TEMP_TYPE_MIXED        1
#define BAT_CORE_NTC_PARA_LEVEL         10
#define BAT_CORE_TEMP_COMP_THRE         200 /* 20c */
#define BAT_CORE_TEMP_MULTIPLE          10

enum bat_core_temp_comp_para_info {
	BAT_CORE_NTC_PARA_ICHG = 0,
	BAT_CORE_NTC_PARA_VALUE,
	BAT_CORE_NTC_PARA_TOTAL,
};

struct bat_core_capacity_level {
	int min_cap;
	int max_cap;
	int level;
};

struct bat_core_monitor_para {
	int min_cap;
	int max_cap;
	int interval;
};

struct bat_core_config {
	int voltage_now_scale;
	int voltage_max_scale;
	int charge_fcc_scale;
	int charge_rm_scale;
	int coul_type;
	int temp_type;
	int work_para_cols;
	int ntc_compensation_is;
	struct bat_core_monitor_para work_para[BAT_CORE_WORK_PARA_ROW];
	struct compensation_para temp_comp_para[BAT_CORE_NTC_PARA_LEVEL];
};

struct bat_core_data {
	int exist;
	int charge_status;
	int health;
	int ui_capacity;
	int capacity_level;
	int temp_now;
	int cycle_count;
	int fcc;
	int voltage_max_now;
	int capacity_rm;
};

struct bat_core_device {
	struct device *dev;
	struct power_supply *main_psy;
	struct power_supply *aux_psy;
	int work_interval;
	struct delayed_work monitor_work;
	struct wakeup_source *wakelock;
	struct mutex data_lock;
	struct notifier_block event_nb;
	struct bat_core_config config;
	struct bat_core_data data;
};

#ifdef CONFIG_HUAWEI_BATTERY_CORE
int bat_core_get_coul_type(void);
#else
static inline int bat_core_get_coul_type(void)
{
	return COUL_TYPE_MAIN;
}
#endif /* CONFIG_HUAWEI_BATTERY_CORE */

#endif /* _BATTERY_CORE_H_ */
