/* SPDX-License-Identifier: GPL-2.0 */
/*
 * power_time.h
 *
 * time for power module
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

#ifndef _POWER_TIME_H_
#define _POWER_TIME_H_

#include <linux/rtc.h>
#include <linux/time64.h>
#include <linux/timekeeping.h>

struct timespec power_get_current_kernel_time(void);
struct timespec64 power_get_current_kernel_time64(void);
void power_get_timeofday(struct timeval *tv);
time64_t power_get_monotonic_boottime(void);
void power_get_local_time(struct timeval *time, struct rtc_time *tm);
bool power_is_within_time_interval(u8 start_hour, u8 start_min, u8 end_hour, u8 end_min);

#endif /* _POWER_TIME_H_ */
