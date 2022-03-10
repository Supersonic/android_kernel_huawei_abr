/* SPDX-License-Identifier: GPL-2.0 */
/*
 * power_proxy.h
 *
 * charger or battery proxy
 *
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

#ifndef _POWER_PROXY_H_
#define _POWER_PROXY_H_

#include <linux/errno.h>

#ifdef CONFIG_HUAWEI_POWER_PROXY
int power_proxy_get_filter_sum(int base);
void power_proxy_sync_filter_soc(int rep_soc, int round_soc, int base);
void power_proxy_cancle_capacity_work(void);
void power_proxy_restart_capacity_work(void);
int power_proxy_get_bat_voltage(int *val);
int power_proxy_get_bat_current(int *val);
int power_proxy_get_bat_current_avg(int *val);
int power_proxy_get_bat_temperature(int *val);
int power_proxy_get_capacity(int *val);
int power_proxy_get_ui_capacity(int *val);
int power_proxy_is_battery_exit(int *val);
#else
static inline int power_proxy_get_filter_sum(int base)
{
	return 0;
}

static inline void power_proxy_sync_filter_soc(int rep_soc,
	int round_soc, int base)
{
}

static inline void power_proxy_cancle_capacity_work(void)
{
}

static inline void power_proxy_restart_capacity_work(void)
{
}

static inline int power_proxy_get_bat_voltage(int *val)
{
	return -EPERM;
}

static inline int power_proxy_get_bat_current(int *val)
{
	return -EPERM;
}

static inline int power_proxy_get_bat_current_avg(int *val)
{
	return -EPERM;
}

static inline int power_proxy_get_bat_temperature(int *val)
{
	return -EPERM;
}

static inline int power_proxy_get_capacity(int *val)
{
	return -EPERM;
}

static inline int power_proxy_get_ui_capacity(int *val)
{
	return -EPERM;
}

static inline int power_proxy_is_battery_exit(int *val)
{
	return -EPERM;
}
#endif /* CONFIG_HUAWEI_POWER_PROXY */

#endif /* _POWER_PROXY_H_ */
