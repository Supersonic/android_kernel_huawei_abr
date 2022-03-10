/*
 * huawei_charger_common.h
 *
 * common interface for huawei charger driver
 *
 * Copyright (c) 2012-2019 Huawei Technologies Co., Ltd.
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

#ifndef _HUAWEI_CHARGER_COMMON_H_
#define _HUAWEI_CHARGER_COMMON_H_

#include <linux/types.h>
#include <chipset_common/hwpower/charger/charger_common_interface.h>

int charge_check_input_dpm_state(void);
int get_charger_bat_id_vol(void);
int charge_enable_force_sleep(bool enable);
int charge_enable_eoc(bool eoc_enable);
bool huawei_charge_done(void);
int mt_get_charger_status(void);

bool charge_check_charger_ts(void);
bool charge_check_charger_otg_state(void);
int charge_set_charge_state(int state);
int get_charge_current_max(void);
void reset_cur_delay_eoc_count(void);
void clear_cur_delay_eoc_count(void);
void huawei_check_delay_en_eoc(void);
int charge_enable_powerpath(bool path_enable);
void charge_set_adapter_voltage(int val, unsigned int type, unsigned int delay_time);
int charge_otg_mode_enable(int enable, unsigned int user);

#endif /* _HUAWEI_CHARGER_COMMON_H_ */
