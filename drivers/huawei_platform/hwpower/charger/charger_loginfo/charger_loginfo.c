// SPDX-License-Identifier: GPL-2.0
/*
 * charger_loginfo.c
 *
 * charger loginfo interface
 *
 * Copyright (C) 2021-2021 Huawei Technologies Co., Ltd.
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

#include <chipset_common/hwpower/charger/charger_common_interface.h>
#include <chipset_common/hwpower/common_module/power_log.h>
#include <chipset_common/hwpower/common_module/power_cmdline.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <huawei_platform/hwpower/common_module/power_platform.h>
#include <huawei_platform/hwpower/charger/charger_loginfo.h>

static void charger_loginfo_convert_charging_status(int val, char *buf, unsigned int len)
{
	if (!buf)
		return;

	switch (val) {
	case POWER_SUPPLY_STATUS_UNKNOWN:
		strncpy(buf, "UNKNOWN", len);
		break;
	case POWER_SUPPLY_STATUS_CHARGING:
		strncpy(buf, "CHARGING", len);
		break;
	case POWER_SUPPLY_STATUS_DISCHARGING:
		strncpy(buf, "DISCHARGING", len);
		break;
	case POWER_SUPPLY_STATUS_NOT_CHARGING:
		strncpy(buf, "NOTCHARGING", len);
		break;
	case POWER_SUPPLY_STATUS_FULL:
		strncpy(buf, "FULL", len);
		break;
	default:
		break;
	}
}

static void charger_loginfo_convert_bat_health(int val, char *buf, unsigned int len)
{
	if (!buf)
		return;

	switch (val) {
	case POWER_SUPPLY_HEALTH_OVERHEAT:
		strncpy(buf, "OVERHEAT", len);
		break;
	case POWER_SUPPLY_HEALTH_COLD:
		strncpy(buf, "COLD", len);
		break;
	case POWER_SUPPLY_HEALTH_WARM:
		strncpy(buf, "WARM", len);
		break;
	case POWER_SUPPLY_HEALTH_COOL:
		strncpy(buf, "COOL", len);
		break;
	case POWER_SUPPLY_HEALTH_GOOD:
		strncpy(buf, "GOOD", len);
		break;
	default:
		break;
	}
}

static void charger_loginfo_get_shutdown_flag(char *buf, unsigned int len)
{
	if (!buf)
		return;

	if (power_cmdline_is_powerdown_charging_mode())
		strncpy(buf, "OFF", len);
	else
		strncpy(buf, "ON", len);
}

static int charger_loginfo_dump_content(char *buffer, int size, void *dev_data)
{
	struct charge_device_info *di = (struct charge_device_info *)dev_data;
	struct charge_dump_data d_data = { 0 };

	if (!buffer || !di)
		return -EINVAL;

	if (power_supply_check_psy_available("usb", &di->usb_psy)) {
		d_data.online = power_supply_return_int_property_value_with_psy(di->usb_psy,
			POWER_SUPPLY_PROP_ONLINE);
		if (power_supply_check_psy_available("pc_port", &di->pc_port_psy))
			d_data.online = d_data.online || power_supply_return_int_property_value_with_psy(
				di->pc_port_psy, POWER_SUPPLY_PROP_ONLINE);
		d_data.in_type = charge_get_charger_type();
		strncpy(d_data.type, charge_get_charger_type_name(d_data.in_type), sizeof(d_data.type));
		d_data.usb_vol = power_supply_app_get_usb_voltage_now();
	}
	if (power_supply_check_psy_available("battery", &di->batt_psy)) {
		(void)huawei_charger_get_charge_enable(&d_data.ch_en);
		d_data.in_status = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_STATUS);
		charger_loginfo_convert_charging_status(d_data.in_status, d_data.status,
			sizeof(d_data.status));
		d_data.in_health = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_HEALTH);
		charger_loginfo_convert_bat_health(d_data.in_health, d_data.health,
			sizeof(d_data.health));
		d_data.bat_present = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_PRESENT);
		d_data.temp = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_TEMP);
		d_data.vol = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_VOLTAGE_NOW);
		d_data.cur = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_CURRENT_NOW);
		d_data.capacity = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_CAPACITY);
		d_data.bat_fcc = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_CAPACITY_FCC);
		d_data.cycle_count = power_supply_return_int_property_value_with_psy(di->batt_psy,
			POWER_SUPPLY_PROP_CYCLE_COUNT);
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
		d_data.ibus = power_supply_return_int_property_value_with_psy(di->usb_psy,
			POWER_SUPPLY_PROP_CURRENT_NOW);
#else
		d_data.ibus = power_supply_return_int_property_value_with_psy(di->usb_psy,
			POWER_SUPPLY_PROP_INPUT_CURRENT_NOW);
#endif
	}
	if (power_supply_check_psy_available("bk_battery", &di->bk_batt_psy))
		d_data.real_temp = power_supply_return_int_property_value_with_psy(di->bk_batt_psy,
			POWER_SUPPLY_PROP_TEMP);
	if (power_supply_check_psy_available("bms", &di->bms_psy))
		d_data.real_soc = power_supply_return_int_property_value_with_psy(di->bms_psy,
			POWER_SUPPLY_PROP_CAPACITY_RAW);
	charger_loginfo_get_shutdown_flag(d_data.on, sizeof(d_data.on));

	(void)snprintf(buffer, size, "%-6d %-10s  %-7d"
		"  %-7d  %-5d  %-12s  %-6s"
		"  %-7d  %-4d  %-6d  %-7d"
		"  %-7d  %-4d  %-9d  %-8d"
		"  %-11d  %-7d  %-4s",
		d_data.online, d_data.type, d_data.usb_vol,
		di->sysfs_data.iin_thl, d_data.ch_en, d_data.status, d_data.health,
		d_data.bat_present, d_data.temp, d_data.real_temp, d_data.vol,
		d_data.cur, d_data.capacity, d_data.real_soc,d_data.bat_fcc,
		d_data.cycle_count, d_data.ibus, d_data.on);
	return 0;
}

static int charger_loginfo_dump_head(char *buf, int size, void *dev_data)
{
	struct charge_device_info *di = (struct charge_device_info *)dev_data;

	if (!buf || !di)
		return -EINVAL;

	(void)snprintf(buf, size, "online in_type     usb_vol"
		"  iin_thl  ch_en  status        health"
		"  present  temp  temp_r  vol"
		"      cur      cap   real_soc   bat_fcc"
		"   cycle_count  ibus     mode");
	return 0;
}

static struct power_log_ops charger_loginfo_ops = {
	.dev_name = "batt_info",
	.dump_log_head = charger_loginfo_dump_head,
	.dump_log_content = charger_loginfo_dump_content,
};

void charger_loginfo_register(struct charge_device_info *di)
{
	charger_loginfo_ops.dev_data = (void *)di;
	(void)power_log_ops_register(&charger_loginfo_ops);
}
