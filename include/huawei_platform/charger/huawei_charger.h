/*
 * huawei_charger.h
 *
 * huawei charger driver interface
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

#ifndef _HUAWEI_CHARGER_H_
#define _HUAWEI_CHARGER_H_

#include <linux/power/huawei_charger.h>

struct charger_device {
};

struct charger_ops {
	int (*chip_init)(struct charger_device *, struct charge_init_data *);
	int (*get_dieid)(struct charger_device *, char *, unsigned int);
	int (*dev_check)(struct charger_device *);
	int (*set_input_current)(struct charger_device *, u32);
	int (*set_charging_current)(struct charger_device *, u32);
	int (*get_charging_current)(struct charger_device *, u32 *);
	int (*set_constant_voltage)(struct charger_device *, u32);
	int (*get_terminal_voltage)(struct charger_device *);
	int (*set_eoc_current)(struct charger_device *, u32);
	int (*enable_termination)(struct charger_device *, bool);
	int (*set_force_term_enable)(struct charger_device *, int);
	int (*enable)(struct charger_device *, bool);
	int (*get_charge_state)(struct charger_device *, unsigned int *);
	int (*get_charger_state)(struct charger_device *);
	int (*set_watchdog_timer)(struct charger_device *, int);
	int (*kick_wdt)(struct charger_device *);
	int (*set_batfet_disable)(struct charger_device *, int);
	int (*enable_hz)(struct charger_device *, bool en);
	int (*get_vbus_adc)(struct charger_device *, u32 *);
	int (*get_ibus_adc)(struct charger_device *, u32 *);
	int (*get_vbat_sys)(struct charger_device *);
	int (*check_input_dpm_state)(struct charger_device *);
	int (*check_input_vdpm_state)(struct charger_device *);
	int (*check_input_idpm_state)(struct charger_device *);
	int (*set_mivr)(struct charger_device *, u32);
	int (*set_vbus_vset)(struct charger_device *, u32);
	int (*set_uvp_ovp)(struct charger_device *);
	int (*soft_vbatt_ovp_protect)(struct charger_device *);
	int (*rboost_buck_limit)(struct charger_device *);
	int (*stop_charge_config)(struct charger_device *);
	int (*enable_otg)(struct charger_device *, bool);
	int (*set_boost_current_limit)(struct charger_device *, u32);
	int (*set_otg_switch_mode_enable)(struct charger_device *, int);
	int (*dump_registers)(struct charger_device *);
	int (*get_iin_set)(struct charger_device *);
	int (*get_vbat)(struct charger_device *);
	int (*get_vusb)(struct charger_device *, int *);
	int (*set_pretrim_code)(struct charger_device *, int);
	int (*get_dieid_for_nv)(struct charger_device *, u8 *, unsigned int);
};

#endif /* _HUAWEI_CHARGER_H_ */
