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

#include <chipset_common/hwpower/battery/battery_temp.h>
#include <chipset_common/hwpower/direct_charge/direct_charge_ic_manager.h>
#include <huawei_platform/hwpower/common_module/power_platform.h>

#define HWLOG_TAG direct_charge_charging_info
HWLOG_REGIST();

void dc_update_multi_info(int mode, struct dc_charge_info *info, int *vbat_comp)
{
	int i;
	int ret;
	int coul_ibat;
	int ibat[CHARGE_IC_MAX_NUM];
	int ibus[CHARGE_IC_MAX_NUM];

	if (!info || !vbat_comp)
		return;

	coul_ibat = -power_platform_get_battery_current();
	if (coul_ibat < info->coul_ibat)
		return;

	for (i = 0; i < CHARGE_PATH_MAX_NUM; i++) {
		ret = dcm_get_ic_ibat(mode, BIT(i), &ibat[i]);
		ret += dcm_get_ic_ibus(mode, BIT(i), &ibus[i]);
		if (ret || (ibus[i] < info->ibus[i]) || (ibat[i] < info->ibat[i]))
			return;
	}

	info->coul_ibat = coul_ibat;
	for (i = 0; i < CHARGE_PATH_MAX_NUM; i++) {
		info->ibat[i] = ibat[i];
		info->ibus[i] = ibus[i];
		info->ic_name[i] = dcm_get_ic_name(mode, BIT(i));
		info->vbat[i] = dcm_get_ic_vbtb_with_comp(mode, BIT(i), vbat_comp);
		dcm_get_ic_vout(mode, BIT(i), &info->vout[i]);
		bat_temp_get_temperature(i, &info->tbat[i]);
	}
}

void dc_update_single_info(int mode, struct dc_charge_info *info, int *vbat_comp)
{
	if (!info || !vbat_comp)
		return;

	info->channel_num = 1; /* single ic num */
	info->ic_name[0] = dcm_get_ic_name(mode, CHARGE_IC_MAIN);
	dcm_get_ic_ibus(mode, CHARGE_IC_MAIN, &info->ibus[0]);
	dcm_get_ic_vout(mode, CHARGE_IC_MAIN, &info->vout[0]);
	dcm_get_ic_ibat(mode, CHARGE_IC_MAIN, &info->ibat[0]);
	info->coul_ibat = -power_platform_get_battery_current();
	info->vbat[0] = dcm_get_ic_vbtb_with_comp(mode, CHARGE_IC_MAIN, vbat_comp);
}
