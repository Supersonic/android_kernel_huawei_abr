// SPDX-License-Identifier: GPL-2.0
/*
 * battery_core.c
 *
 * huawei battery core features
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

#include "battery_core.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_supply.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/battery/battery_temp.h>
#include "battery_model/battery_model.h"
#include "battery_ui_capacity/battery_ui_capacity.h"
#include "battery_fault/battery_fault.h"
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_printk.h>

#define HWLOG_TAG battery_core
HWLOG_REGIST();

static struct bat_core_device *g_bat_core_dev;

static enum power_supply_property bat_core_props[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_CYCLE_COUNT,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_AVG,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_MODEL_NAME,
};

static int bat_core_get_prop(struct power_supply *psy,
	enum power_supply_property prop,
	union power_supply_propval *pval)
{
	struct bat_core_device *bat_dev = power_supply_get_drvdata(psy);
	int type = COUL_TYPE_MAIN;
	int rc = 0;

	if (!psy || !pval)
		return -EINVAL;

	if (bat_dev->aux_psy == psy)
		type = COUL_TYPE_AUX;

	switch (prop) {
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_ONLINE:
		pval->intval = coul_interface_is_battery_exist(type);
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		pval->intval = coul_interface_get_battery_capacity(type);
		break;
	case POWER_SUPPLY_PROP_TEMP:
		pval->intval = coul_interface_get_battery_temperature(type);
		break;
	case POWER_SUPPLY_PROP_CYCLE_COUNT:
		pval->intval = coul_interface_get_battery_cycle(type);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		pval->intval = coul_interface_get_battery_voltage(type);
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		pval->intval = coul_interface_get_battery_current(type);
		break;
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		pval->intval = coul_interface_get_battery_avg_current(type);
		break;
	case POWER_SUPPLY_PROP_CHARGE_FULL:
		pval->intval = coul_interface_get_battery_fcc(type);
		break;
	case POWER_SUPPLY_PROP_MODEL_NAME:
		pval->strval = coul_interface_get_coul_model(type);
		break;
	default:
		rc = -EINVAL;
		break;
	}

	if (prop != POWER_SUPPLY_PROP_MODEL_NAME)
		hwlog_info("battery id = %d prop = %d pval = %d\n",
			type, prop, pval->intval);
	else
		hwlog_info("battery id = %d prop = %d pval = %s\n",
			type, prop, pval->strval);
	return rc;
}

static const struct power_supply_desc batt_gauge_desc = {
	.name = "battery_gauge",
	.type = POWER_SUPPLY_TYPE_UNKNOWN,
	.properties = bat_core_props,
	.num_properties = ARRAY_SIZE(bat_core_props),
	.get_property = bat_core_get_prop,
};

static const struct power_supply_desc batt_gauge_aux_desc = {
	.name = "battery_gauge_aux",
	.type = POWER_SUPPLY_TYPE_UNKNOWN,
	.properties = bat_core_props,
	.num_properties = ARRAY_SIZE(bat_core_props),
	.get_property = bat_core_get_prop,
};

static int bat_core_reg_guage_psy(struct bat_core_device *di)
{
	struct power_supply_config psy_cfg = {};

	psy_cfg.of_node = di->dev->of_node;
	psy_cfg.drv_data = di;
	if (di->config.coul_type == COUL_TYPE_MAIN) {
		di->main_psy = devm_power_supply_register(di->dev,
			&batt_gauge_desc, &psy_cfg);
	} else if (di->config.coul_type == COUL_TYPE_AUX) {
		di->aux_psy = devm_power_supply_register(di->dev,
			&batt_gauge_aux_desc, &psy_cfg);
	} else {
		di->main_psy = devm_power_supply_register(di->dev,
			&batt_gauge_desc, &psy_cfg);
		di->aux_psy = devm_power_supply_register(di->dev,
			&batt_gauge_aux_desc, &psy_cfg);
	}
	return 0;
}

static int bat_core_get_status(void)
{
	int val = POWER_SUPPLY_DEFAULT_STATUS;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		val = di->data.charge_status;
		mutex_unlock(&di->data_lock);
	}

	return val;
}

static int bat_core_get_health(void)
{
	int val = POWER_SUPPLY_DEFAULT_HEALTH;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		val = di->data.health;
		mutex_unlock(&di->data_lock);
	}

	return val;
}

static int bat_core_get_exist(void)
{
	int val = POWER_SUPPLY_DEFAULT_ONLINE;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		val = di->data.exist;
		mutex_unlock(&di->data_lock);
	}

	return val;
}

static int bat_core_get_technology(void)
{
	struct bat_core_device *di = g_bat_core_dev;

	if (!di || !di->data.exist)
		return POWER_SUPPLY_DEFAULT_TECHNOLOGY;

	return bat_model_get_technology();
}

static int bat_core_get_temp(void)
{
	int val = POWER_SUPPLY_DEFAULT_TEMP;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		val = di->data.temp_now;
		mutex_unlock(&di->data_lock);
	}

	return val;
}

static int bat_core_get_cycle_count(void)
{
	int val = POWER_SUPPLY_DEFAULT_CYCLE_COUNT;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		val = di->data.cycle_count;
		mutex_unlock(&di->data_lock);
	}

	return val;
}

static int bat_core_get_voltage_now(void)
{
	int val = POWER_SUPPLY_DEFAULT_VOLTAGE_NOW;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		val = coul_interface_get_battery_voltage(di->config.coul_type);
		val *= di->config.voltage_now_scale;
	} else {
		val *= POWER_UV_PER_MV;
	}

	return val;
}

static int bat_core_get_voltage_max(void)
{
	int val = bat_model_get_vbat_max();
	struct bat_core_device *di = g_bat_core_dev;

	if (di)
		val *= di->config.voltage_max_scale;
	else
		val *= POWER_UV_PER_MV;

	return val;
}

static int bat_core_get_voltage_max_now(void)
{
	int val = bat_model_get_vbat_max();
	struct bat_core_device *di = g_bat_core_dev;

	if (di)
		val = di->data.voltage_max_now;
	else
		val *= POWER_UV_PER_MV;

	return val;
}

static int bat_core_set_voltage_max_now(int value)
{
	int max = bat_model_get_vbat_max();
	int dec;
	struct bat_core_device *di = g_bat_core_dev;

	if (!di)
		return -ENODEV;

	max *= di->config.voltage_max_scale;
	if (value > max)
		return -EINVAL;

	di->data.voltage_max_now = value;
	dec = (max - value) / di->config.voltage_max_scale;
	return coul_interface_set_vterm_dec(di->config.coul_type, dec);
}

static int bat_core_get_current_now(void)
{
	int val = POWER_SUPPLY_DEFAULT_CURRENT_NOW;
	struct bat_core_device *di = g_bat_core_dev;

	if (di)
		val = coul_interface_get_battery_current(di->config.coul_type);

	return val;
}

static int bat_core_get_current_avg(void)
{
	int val = POWER_SUPPLY_DEFAULT_CURRENT_NOW;
	struct bat_core_device *di = g_bat_core_dev;

	if (di)
		val = coul_interface_get_battery_avg_current(di->config.coul_type);

	return val;
}

static int bat_core_get_fcc(void)
{
	int val = POWER_SUPPLY_DEFAULT_CAPACITY_FCC;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		val = di->data.fcc * di->config.charge_fcc_scale;
		mutex_unlock(&di->data_lock);
	}

	return val;
}

static int bat_core_get_design_fcc(void)
{
	int val = bat_model_get_design_fcc();
	struct bat_core_device *di = g_bat_core_dev;

	if (di)
		val *= di->config.charge_fcc_scale;

	return val;
}

static int bat_core_get_capacity_level(void)
{
	int val = POWER_SUPPLY_DEFAULT_CAPACITY_LEVEL;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		val = di->data.capacity_level;
		mutex_unlock(&di->data_lock);
	}

	return val;
}

static int bat_core_update_capacity_level(int ui_capacity)
{
	const struct bat_core_capacity_level level_config[] = {
		{ 0,  5,   POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL },
		{ 5,  15,  POWER_SUPPLY_CAPACITY_LEVEL_LOW      },
		{ 15, 95,  POWER_SUPPLY_CAPACITY_LEVEL_NORMAL   },
		{ 95, 100, POWER_SUPPLY_CAPACITY_LEVEL_HIGH     },
	};
	int i;

	if (ui_capacity == BAT_CORE_CAPACITY_FULL)
		return POWER_SUPPLY_CAPACITY_LEVEL_FULL;
	for (i = 0; i < ARRAY_SIZE(level_config); i++) {
		if ((ui_capacity >= level_config[i].min_cap) &&
			(ui_capacity < level_config[i].max_cap))
			return level_config[i].level;
	}
	return POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN;
}

static void bat_core_update_charge_status(struct bat_core_device *di,
	int status)
{
	int cur_status;

	if (!di->data.exist)
		di->data.exist = coul_interface_is_battery_exist(di->config.coul_type);
	if (!di->data.exist) {
		cur_status = POWER_SUPPLY_STATUS_UNKNOWN;
		goto change_status;
	}

	if ((status == POWER_SUPPLY_STATUS_CHARGING) &&
		(di->data.ui_capacity == BAT_CORE_CAPACITY_FULL)) {
		cur_status = POWER_SUPPLY_STATUS_FULL;
		goto change_status;
	}
	cur_status = status;

change_status:
	hwlog_info("update charge status exist=%d,in=%d,last=%d,cur=%d,cap=%d\n",
		di->data.exist, status, di->data.charge_status, cur_status,
		di->data.ui_capacity);
	if (di->data.charge_status != cur_status) {
		di->data.charge_status = cur_status;
		power_supply_sync_changed("Battery");
		power_supply_sync_changed("battery");
	}
}

static int bat_core_get_ui_capacity(void)
{
	int capacity = bat_ui_capacity();
	int level = bat_core_update_capacity_level(capacity);
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		mutex_lock(&di->data_lock);
		di->data.ui_capacity = capacity;
		di->data.capacity_level = level;
		bat_core_update_charge_status(di, di->data.charge_status);
		mutex_unlock(&di->data_lock);
		hwlog_info("read ui_cap=%d,level=%d,status=%d\n", di->data.ui_capacity,
			di->data.capacity_level, di->data.charge_status);
	}
	return capacity;
}

static int bat_core_get_capacity_rm(void)
{
	int val = 0;
	struct bat_core_device *di = g_bat_core_dev;

	if (di) {
		val = coul_interface_get_battery_rm(di->config.coul_type);
		if (val < 0)
			return 0;
		val *= di->config.charge_rm_scale;
	}

	return val;
}

static struct power_supply_ops bat_core_raw_power_supply_ops = {
	.type_name = "raw_bat",
	.get_status_prop = bat_core_get_status,
	.get_health_prop = bat_core_get_health,
	.get_present_prop = bat_core_get_exist,
	.get_online_prop = bat_core_get_exist,
	.get_technology_prop = bat_core_get_technology,
	.get_temp_prop = bat_core_get_temp,
	.get_cycle_count_prop = bat_core_get_cycle_count,
	.get_voltage_now_prop = bat_core_get_voltage_now,
	.get_voltage_max_prop = bat_core_get_voltage_max_now,
	.get_voltage_max_design_prop = bat_core_get_voltage_max,
	.get_current_now_prop = bat_core_get_current_now,
	.get_current_avg_prop = bat_core_get_current_avg,
	.get_capacity_prop = bat_core_get_ui_capacity,
	.get_capacity_level_prop = bat_core_get_capacity_level,
	.get_capacity_fcc_prop = bat_core_get_fcc,
	.get_charge_full_prop = bat_core_get_fcc,
	.get_charge_full_design_prop = bat_core_get_design_fcc,
	.get_brand_prop = bat_model_get_brand,
	.get_capacity_rm_prop = bat_core_get_capacity_rm,
};

static struct power_supply_ops bat_core_assist_power_supply_ops = {
	.type_name = "assist_bat",
	.get_online_prop = bat_core_get_exist,
	.get_capacity_prop = bat_ui_raw_capacity,
	.get_voltage_max_prop = bat_core_get_voltage_max_now,
	.set_voltage_max_prop = bat_core_set_voltage_max_now,
};

static int bat_core_event_notifier_call(struct notifier_block *nb,
	unsigned long event, void *data)
{
	int status;
	struct bat_core_device *di = g_bat_core_dev;

	if (!di)
		return NOTIFY_OK;

	switch (event) {
	case POWER_NE_CHARGING_START:
		status = POWER_SUPPLY_STATUS_CHARGING;
		break;
	case POWER_NE_CHARGING_STOP:
		status = POWER_SUPPLY_STATUS_DISCHARGING;
		break;
	case POWER_NE_CHARGING_SUSPEND:
		status = POWER_SUPPLY_STATUS_NOT_CHARGING;
		break;
	default:
		return NOTIFY_OK;
	}

	hwlog_info("receive event=%lu,status=%d\n", event, status);
	mutex_lock(&di->data_lock);
	bat_core_update_charge_status(di, status);
	mutex_unlock(&di->data_lock);
	return NOTIFY_OK;
}

#ifdef CONFIG_HLTHERM_RUNTEST
static int bat_core_temp(struct bat_core_device *di)
{
	return POWER_SUPPLY_DEFAULT_TEMP;
}
#else
static int bat_core_ntc_compensation_temp(struct bat_core_device *di,
	int temp_val, int cur_temp)
{
	int temp_with_compensation = temp_val;
	struct common_comp_data comp_data;

	if (!di)
		return temp_with_compensation;

	comp_data.refer = abs(cur_temp);
	comp_data.para_size = BAT_CORE_NTC_PARA_LEVEL;
	comp_data.para = di->config.temp_comp_para;
	if ((di->config.ntc_compensation_is) &&
		(temp_val >= BAT_CORE_TEMP_COMP_THRE))
		temp_with_compensation = power_get_compensation_value(temp_val,
			&comp_data);

	hwlog_info("temp_compensation=%d temp_no_comp=%d ichg=%d\n",
		temp_with_compensation, temp_val, cur_temp);
	return temp_with_compensation;
}

static int bat_core_temp(struct bat_core_device *di)
{
	int raw_temp = -40; /* default temperature -40 degree */
	int bat_curr;

	if (di->config.temp_type == BAT_CORE_TEMP_TYPE_MIXED) {
		bat_temp_get_temperature(BAT_TEMP_MIXED, &raw_temp);
		return raw_temp * BAT_CORE_TEMP_MULTIPLE;
	}

	raw_temp = coul_interface_get_battery_temperature(di->config.coul_type);
	bat_curr = coul_interface_get_battery_current(di->config.coul_type);

	return bat_core_ntc_compensation_temp(di, raw_temp, bat_curr);
}
#endif /* CONFIG_HLTHERM_RUNTEST */

static void bat_core_update_health(struct bat_core_device *di)
{
	int health;

	if (!di->data.exist) {
		di->data.health = POWER_SUPPLY_HEALTH_UNKNOWN;
		return;
	}

	if (di->data.temp_now < BAT_CORE_COLD_TEMP)
		health = POWER_SUPPLY_HEALTH_COLD;
	else if (di->data.temp_now > BAT_CORE_OVERHEAT_TEMP)
		health = POWER_SUPPLY_HEALTH_OVERHEAT;
	else if (bat_fault_is_cutoff_vol())
		health = POWER_SUPPLY_HEALTH_UNDERVOLTAGE;
	else
		health = POWER_SUPPLY_HEALTH_GOOD;

	if (health != di->data.health) {
		hwlog_info("health change from %d to %d\n", di->data.health,
			health);
		di->data.health = health;
	}
}

static void bat_core_update_data(struct bat_core_device *di)
{
	struct bat_core_data t_data;

	mutex_lock(&di->data_lock);
	memcpy(&t_data, &di->data, sizeof(struct bat_core_data));

	t_data.exist = coul_interface_is_battery_exist(di->config.coul_type);
	t_data.fcc = coul_interface_get_battery_fcc(di->config.coul_type);
	if (!t_data.exist) {
		t_data.health = POWER_SUPPLY_HEALTH_UNKNOWN;
		t_data.capacity_level = POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN;
		t_data.temp_now = POWER_SUPPLY_DEFAULT_TEMP;
	} else {
		t_data.temp_now = bat_core_temp(di);
		t_data.cycle_count = coul_interface_get_battery_cycle(di->config.coul_type);
	}

	memcpy(&di->data, &t_data, sizeof(struct bat_core_data));
	bat_core_update_health(di);
	mutex_unlock(&di->data_lock);
}

static void bat_core_update_work_interval(struct bat_core_device *di)
{
	int i;
	int capacity;

	mutex_lock(&di->data_lock);
	capacity = di->data.ui_capacity;
	mutex_unlock(&di->data_lock);

	if (di->config.work_para_cols == 0) {
		di->work_interval = BAT_CORE_WORK_INTERVAL_NORMAL;
		return;
	}

	di->work_interval = BAT_CORE_WORK_INTERVAL_MAX;
	for (i = 0; i < di->config.work_para_cols; i++) {
		if ((capacity >= di->config.work_para[i].min_cap) &&
			(capacity < di->config.work_para[i].max_cap)) {
			di->work_interval = di->config.work_para[i].interval;
			break;
		}
	}
}

static void bat_core_work(struct work_struct *work)
{
	struct bat_core_device *di = container_of(work, struct bat_core_device,
		monitor_work.work);

	__pm_wakeup_event(di->wakelock, jiffies_to_msecs(HZ));
	bat_core_update_data(di);
	bat_core_update_work_interval(di);
	power_wakeup_unlock(di->wakelock, false);

	queue_delayed_work(system_power_efficient_wq, &di->monitor_work,
		msecs_to_jiffies(di->work_interval));
}

int bat_core_get_coul_type(void)
{
	if (g_bat_core_dev)
		return g_bat_core_dev->config.coul_type;

	return COUL_TYPE_MAIN;
}

static void bat_core_fault_notify(unsigned int event)
{
	struct bat_core_device *di = g_bat_core_dev;

	if (!di)
		return;

	switch (event) {
	case POWER_NE_COUL_LOW_VOL:
		mod_delayed_work(system_power_efficient_wq, &di->monitor_work,
			msecs_to_jiffies(0));
		break;
	default:
		break;
	}
	hwlog_info("fault event notify=%d\n", event);
}

static const struct bat_fault_ops g_bat_core_fault_ops = {
	.notify = bat_core_fault_notify,
};

static void bat_core_data_init(struct bat_core_device *di)
{
	di->data.ui_capacity = POWER_SUPPLY_DEFAULT_CAPACITY;
	di->data.capacity_level = POWER_SUPPLY_DEFAULT_CAPACITY_LEVEL;
	di->data.health = POWER_SUPPLY_DEFAULT_HEALTH;
	di->data.fcc = POWER_SUPPLY_DEFAULT_CAPACITY_FCC;
	di->data.charge_status = POWER_SUPPLY_STATUS_DISCHARGING;
	di->data.exist = coul_interface_is_battery_exist(di->config.coul_type);
	di->data.voltage_max_now = bat_model_get_vbat_max() * di->config.voltage_max_scale;
}

static void bat_core_parse_work_para(struct device_node *np,
	struct bat_core_config *config)
{
	int i;
	int len;

	len = power_dts_read_string_array(power_dts_tag(HWLOG_TAG),
		np,
		"work_interval_para",
		(int *)config->work_para,
		BAT_CORE_WORK_PARA_ROW,
		BAT_CORE_WORK_PARA_COL);
	if (len <= 0) {
		config->work_para_cols = 0;
		return;
	}

	config->work_para_cols = len / BAT_CORE_WORK_PARA_COL;
	for (i = 0; i < config->work_para_cols; i++)
		hwlog_info("word para%d: %d - %d = %d\n", i,
			config->work_para[i].min_cap,
			config->work_para[i].max_cap,
			config->work_para[i].interval);
}

static void bat_core_parse_temp_para(struct device_node *np,
	struct bat_core_config *config)
{
	int array_len;
	int i;
	int idata = 0;
	const char *string = NULL;
	int col;
	int row;

	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"temp_type", &config->temp_type,
		BAT_CORE_TEMP_TYPE_RAW_COMP);

	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"ntc_compensation_is", &config->ntc_compensation_is, 0);
	if (!config->ntc_compensation_is)
		return;

	array_len = power_dts_read_count_strings(power_dts_tag(HWLOG_TAG), np,
		"ntc_temp_compensation_para",
		BAT_CORE_NTC_PARA_LEVEL, BAT_CORE_NTC_PARA_TOTAL);
	if (array_len < 0) {
		config->ntc_compensation_is = 0;
		return;
	}

	for (i = 0; i < array_len; i++) {
		if (power_dts_read_string_index(power_dts_tag(HWLOG_TAG),
			np, "ntc_temp_compensation_para", i, &string)) {
			config->ntc_compensation_is = 0;
			return;
		}

		if (kstrtoint(string, POWER_BASE_DEC, &idata)) {
			config->ntc_compensation_is = 0;
			return;
		}

		col = i % BAT_CORE_NTC_PARA_TOTAL;
		row = i / BAT_CORE_NTC_PARA_TOTAL;

		switch (col) {
		case BAT_CORE_NTC_PARA_ICHG:
			config->temp_comp_para[row].refer = idata;
			break;
		case BAT_CORE_NTC_PARA_VALUE:
			config->temp_comp_para[row].comp_value = idata;
			break;
		default:
			hwlog_err("temp_comp_para get failed\n");
			break;
		}

		hwlog_info("config->comp_para[%d][%d] is %d\n", row, col, idata);
	}
}

static void bat_core_parse_dts(struct bat_core_device *di)
{
	struct device_node *np = di->dev->of_node;

	if (!np)
		return;

	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"coul_type", &di->config.coul_type, COUL_TYPE_MAIN);

	bat_core_parse_temp_para(np, &di->config);

	bat_core_parse_work_para(np, &di->config);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"voltage_now_scale", &di->config.voltage_now_scale, 1);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"voltage_max_scale", &di->config.voltage_max_scale, 1);
	if (!di->config.voltage_max_scale)
		di->config.voltage_max_scale = POWER_UV_PER_MV;

	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"charge_fcc_scale", &di->config.charge_fcc_scale, 1);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"charge_rm_scale", &di->config.charge_rm_scale, 1);
}

static int bat_core_probe(struct platform_device *pdev)
{
	int ret;
	struct bat_core_device *di = NULL;

	if (!pdev || !pdev->dev.of_node)
		return -ENODEV;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	di->dev = &pdev->dev;
	g_bat_core_dev = di;
	bat_core_parse_dts(di);
	mutex_init(&di->data_lock);
	bat_core_data_init(di);

	ret = power_supply_ops_register(&bat_core_raw_power_supply_ops);
	if (ret) {
		hwlog_err("bat_core_raw_power_supply_ops register fail\n");
		goto fail_free_mem;
	}
	ret = power_supply_ops_register(&bat_core_assist_power_supply_ops);
	if (ret) {
		hwlog_err("bat_core_assist_power_supply_ops register fail\n");
		goto fail_free_mem;
	}
	di->event_nb.notifier_call = bat_core_event_notifier_call;
	ret = power_event_bnc_register(POWER_BNT_CHARGING, &di->event_nb);
	if (ret)
		goto fail_free_mem;

	platform_set_drvdata(pdev, di);

	bat_core_reg_guage_psy(di);

	di->wakelock = power_wakeup_source_register(di->dev, "battery_core");
	if (!di->wakelock) {
		ret = -ENOMEM;
		goto fail_free_mem;
	}

	di->work_interval = BAT_CORE_WORK_INTERVAL_NORMAL;
	INIT_DELAYED_WORK(&di->monitor_work, bat_core_work);
	queue_delayed_work(system_power_efficient_wq, &di->monitor_work, 0);
	bat_fault_register_ops(&g_bat_core_fault_ops);

	return 0;

fail_free_mem:
	mutex_destroy(&di->data_lock);
	kfree(di);
	g_bat_core_dev = NULL;
	return ret;
}

static int bat_core_remove(struct platform_device *pdev)
{
	struct bat_core_device *di = platform_get_drvdata(pdev);

	if (!di)
		return -ENODEV;

	platform_set_drvdata(pdev, NULL);
	cancel_delayed_work(&di->monitor_work);
	power_event_bnc_unregister(POWER_BNT_CHARGING, &di->event_nb);
	mutex_destroy(&di->data_lock);
	power_wakeup_source_unregister(di->wakelock);
	kfree(di);
	g_bat_core_dev = NULL;
	return 0;
}

#ifdef CONFIG_PM
static int bat_core_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct bat_core_device *di = platform_get_drvdata(pdev);

	if (!di)
		return 0;

	cancel_delayed_work(&di->monitor_work);
	return 0;
}

static int bat_core_resume(struct platform_device *pdev)
{
	struct bat_core_device *di = platform_get_drvdata(pdev);

	if (!di)
		return 0;

	queue_delayed_work(system_power_efficient_wq, &di->monitor_work, 0);
	return 0;
}
#endif /* CONFIG_PM */

static const struct of_device_id bat_core_match_table[] = {
	{
		.compatible = "huawei,battery_core",
		.data = NULL,
	},
	{},
};

static struct platform_driver bat_core_driver = {
	.probe = bat_core_probe,
	.remove = bat_core_remove,
#ifdef CONFIG_PM
	.suspend = bat_core_suspend,
	.resume = bat_core_resume,
#endif /* CONFIG_PM */
	.driver = {
		.name = "huawei,battery_core",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(bat_core_match_table),
	},
};

static int __init bat_core_init(void)
{
	return platform_driver_register(&bat_core_driver);
}

static void __exit bat_core_exit(void)
{
	platform_driver_unregister(&bat_core_driver);
}

device_initcall_sync(bat_core_init);
module_exit(bat_core_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("battery core driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
