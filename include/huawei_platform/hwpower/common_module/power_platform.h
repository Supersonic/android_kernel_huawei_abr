/* SPDX-License-Identifier: GPL-2.0 */
/*
 * power_platform.h
 *
 * differentiated interface related to chip platform
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

#ifndef _POWER_PLATFORM_H_
#define _POWER_PLATFORM_H_

#include <linux/errno.h>
#include <linux/notifier.h>
#include <chipset_common/hwpower/common_module/power_supply_application.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <huawei_platform/power/huawei_charger_common.h>
#include <huawei_platform/power/huawei_charger_adaptor.h>
#include <huawei_platform/power/direct_charger/direct_charger.h>
#include <huawei_platform/hwpower/power_proxy.h>
#include <huawei_platform/hwpower/common_module/power_glink.h>
#include <linux/power/huawei_battery.h>
#include <huawei_platform/power/huawei_charger.h>

static inline int power_platform_get_filter_soc(int base)
{
	return power_proxy_get_filter_sum(base);
}

static inline void power_platform_sync_filter_soc(int rep_soc,
	int round_soc, int base)
{
	power_proxy_sync_filter_soc(rep_soc, round_soc, base);
}

static inline void power_platform_cancle_capacity_work(void)
{
	power_proxy_cancle_capacity_work();
}

static inline void power_platform_restart_capacity_work(void)
{
	power_proxy_restart_capacity_work();
}

static inline int power_platform_get_adc_sample(int adc_channel)
{
	return -EPERM;
}

static inline int power_platform_get_adc_voltage(int adc_channel)
{
	return -EPERM;
}

static inline int power_platform_get_battery_id_voltage(void)
{
	return 0;
}

static inline int power_platform_get_battery_ui_capacity(void)
{
	int val = 0;

	(void)power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_CAPACITY, &val);
	return val;
}

static inline int power_platform_get_battery_temperature(void)
{
	int val = -400; /* default is -40 centigrade */

	(void)power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_TEMP, &val);
	return val / 10; /* 10: convert temperature unit */
}

static inline int power_platform_get_rt_battery_temperature(void)
{
	return power_platform_get_battery_temperature();
}

static inline int power_platform_get_battery_current(void)
{
	return battery_get_bat_current();
}

static inline int power_platform_is_battery_removed(void)
{
	return 0;
}

static inline int power_platform_is_battery_exit(void)
{
	return 1;
}

static inline int power_platform_get_vbus_status(void)
{
	/* 1: exist if vbus > 2.8V, otherwise 0: not exist */
	return (charge_get_vbus() >= 2800) ? 1 : 0;
}

static inline int power_platform_enable_ext_pmic_boost(int value)
{
	return 0;
}

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK) && defined(CONFIG_INPUT_QCOM_HV_HAPTICS)
int haptics_write_reg_val(u16 base, u8 offset, u8 *val, u32 length);
static inline int power_platform_enable_int_pmic_boost(int value)
{
	u8 reg_val = 0;

	(void)haptics_write_reg_val(0, 0x46, &reg_val, 1); /* 0 cfg_addr, HAPTICS_RTC_TIMER_EN_CTL 0x46 */
	return power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_VBOOST, (u32 *)&value, GLINK_DATA_ONE);
}
#else
static inline int power_platform_enable_int_pmic_boost(int value)
{
	return 0;
}
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) && defined(CONFIG_INPUT_QCOM_HV_HAPTICS) */

static inline bool power_platform_usb_state_is_host(void)
{
	return false;
}

static inline bool power_platform_pogopin_is_support(void)
{
	return false;
}

static inline bool power_platform_pogopin_otg_from_buckboost(void)
{
	return false;
}

static inline int power_platform_powerkey_register_notifier(struct notifier_block *nb)
{
	return 0;
}

static inline int power_platform_powerkey_unregister_notifier(struct notifier_block *nb)
{
	return 0;
}

static inline bool power_platform_is_powerkey_up(unsigned long event)
{
	return false;
}

static inline bool power_platform_get_sysfs_wdt_disable_flag(void)
{
	return false;
}

static inline void power_platform_charge_stop_sys_wdt(void)
{
}

static inline void power_platform_charge_feed_sys_wdt(unsigned int timeout)
{
}

static inline int power_platform_set_max_input_current(void)
{
	return -EPERM;
}

static inline void power_platform_start_acr_calc(void)
{
}

static inline int power_platform_get_acr_resistance(int *acr_value)
{
	return -EPERM;
}

#ifdef CONFIG_DIRECT_CHARGER
static inline bool power_platform_in_dc_charging_stage(void)
{
	return direct_charge_in_charging_stage() == DC_IN_CHARGING_STAGE;
}
#else
static inline bool power_platform_in_dc_charging_stage(void)
{
	return false;
}
#endif /* CONFIG_DIRECT_CHARGER */

static inline void power_platform_set_charge_hiz(int enable)
{
	charge_set_hiz_enable(enable);
}

static inline int power_platform_get_cv_limit(void)
{
	return 0;
}

static inline void power_platform_buck_vbus_change_handler(int vbus)
{
}

#endif /* _POWER_PLATFORM_H_ */
