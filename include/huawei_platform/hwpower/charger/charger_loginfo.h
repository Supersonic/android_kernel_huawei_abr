/* SPDX-License-Identifier: GPL-2.0 */
/*
 * charger_loginfo.h
 *
 * charger loginfo interface
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

#ifndef _CHARGER_LOGINFO_H_
#define _CHARGER_LOGINFO_H_

#define CHARGER_LOGINFO_BUF_LEN 32

struct charge_dump_data {
	int online;
	int in_type;
	int ch_en;
	int in_status;
	int in_health;
	int bat_present;
	int real_temp;
	int temp;
	int vol;
	int cur;
	int capacity;
	int real_soc;
	int ibus;
	int usb_vol;
	int cycle_count;
	int bat_fcc;
	int fsoc;
	int msoc;
	char type[CHARGER_LOGINFO_BUF_LEN];
	char status[CHARGER_LOGINFO_BUF_LEN];
	char health[CHARGER_LOGINFO_BUF_LEN];
	char on[CHARGER_LOGINFO_BUF_LEN];
};

#ifdef CONFIG_HUAWEI_CHARGER
void charger_loginfo_register(struct charge_device_info *di);
#else
static inline void charger_loginfo_register(struct charge_device_info *di)
{
}
#endif /* CONFIG_HUAWEI_CHARGER */

#endif /* _CHARGER_LOGINFO_H_ */
