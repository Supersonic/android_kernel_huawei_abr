// SPDX-License-Identifier: GPL-2.0
/*
 * direct_charge_charging_info.c
 *
 * charging info for direct charge module
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

#ifndef _DIRECT_CHARGE_CHARGING_INFO_H_
#define _DIRECT_CHARGE_CHARGING_INFO_H_

#include <chipset_common/hwpower/direct_charge/direct_charge_ic.h>

struct dc_charge_info {
	int succ_flag;
	const char *ic_name[CHARGE_IC_MAX_NUM];
	int channel_num;
	int coul_ibat;
	int ibus[CHARGE_IC_MAX_NUM];
	int ibat[CHARGE_IC_MAX_NUM];
	int vbat[CHARGE_IC_MAX_NUM];
	int vout[CHARGE_IC_MAX_NUM];
	int tbat[CHARGE_IC_MAX_NUM];
};

#ifdef CONFIG_DIRECT_CHARGER
void dc_update_multi_info(int mode, struct dc_charge_info *info, int *vbat_comp);
void dc_update_single_info(int mode, struct dc_charge_info *info, int *vbat_comp);
#else
static inline void dc_update_multi_info(int mode, struct dc_charge_info *info, int *vbat_comp)
{
}

static inline void dc_update_single_info(int mode, struct dc_charge_info *info, int *vbat_comp)
{
}
#endif /* CONFIG_DIRECT_CHARGER */

#endif /* _DIRECT_CHARGE_CHARGING_INFO_H_ */
