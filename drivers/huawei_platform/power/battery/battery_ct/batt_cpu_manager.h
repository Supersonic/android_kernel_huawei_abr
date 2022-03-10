/* SPDX-License-Identifier: GPL-2.0 */
/*
 * batt_cpu_manager.h
 *
 * icon interface for power module
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

#ifndef _BATT_CPU_MANAGER_H_
#define _BATT_CPU_MANAGER_H_

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/mutex.h>

#define BATT_WAIT_CPU_SWITCH_DONE  10 /* 10 ms */
#define BATT_CORE_NUM              8

int batt_cpu_single_core_set(void);
void batt_turn_on_all_cpu_cores(void);

#endif /* _BATT_CPU_MANAGER_H_ */