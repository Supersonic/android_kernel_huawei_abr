/*
 * huawei_battery.c
 *
 * huawei battery driver
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

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/platform_device.h>
#include <linux/pmic-voter.h>
#include <linux/power/charger-manager.h>
#include <linux/power/huawei_battery.h>
#include <linux/power/huawei_charger.h>
#include <linux/slab.h>
#include <linux/spmi.h>
#include <linux/sysfs.h>
#include <linux/usb/otg.h>
#include <linux/version.h>
#include <chipset_common/hwpower/common_module/power_cmdline.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_icon.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <chipset_common/hwpower/common_module/power_wakeup.h>
#include <chipset_common/hwpower/common_module/power_vote.h>
#include <chipset_common/hwpower/hvdcp_charge/hvdcp_charge.h>
#include <huawei_platform/hwpower/common_module/power_platform.h>
#include <huawei_platform/hwpower/power_proxy.h>
#include <huawei_platform/power/direct_charger/direct_charger.h>

static struct huawei_battery_info *g_info;

#define IN_THERMAL_VOTER "IN_THERMAL_VOTER"
#define IBUS_DETECT_VOTER "IBUS_DETECT_VOTER"
#define ICL_USER_VOTER "ICL_USER_VOLTER"
#define ICL_WLRX_VOTER "ICL_WLRX_VOTER"
#define FCC_JEITA_VOTER "FCC_JEITA_VOTER"
#define FCC_USER_VOTER "FCC_USER_VOTER"
#define FIVE_DEGREE_VOTER "FIVE_DEGREE_VOTER"
#define VBAT_JEITA_VOTER "VBAT_JEITA_VOTER"
#define VBAT_USER_VOTER "VBAT_USER_VOTER"
#define VBAT_BASP_VOTER "VBAT_BASP_VOTER"
#define BCH_JEITA_VOTER "BCH_JEITA_VOTER"
#define BCH_WARM_WR_VOTER   "BCH_WARM_WR_VOTER"
#define BCH_USER_VOTER  "BCH_USER_VOTER"

#define VOL_CUTOFF_FILTER_MAX_CNT 3
#define DEFAULT_VOL_CUTOFF_NORMAL 3000000

static int g_vol_cutoff_filter_cnt = 0;

#define CHARGE_PSY_NAME "huawei_charge"
#define FCC_VOTE_OBJECT "BATT: fcc"
#define USB_ICL_VOTE_OBJECT "BATT: usb_icl"
#define VBAT_VOTE_OBJECT "BATT: vbat"
#define BCH_VOTE_OBJECT "BATT: bch"
static int set_prop_safety_timer_enable(struct huawei_battery_info *info, int use_timer);
static int battery_jeita_solution(struct huawei_battery_info *info);

/***************************************************
 Function: batt_disable_temp_limit
 Description: disable jeita solution for heat case
 Parameters:
    info            huawei_battery_info
 Return:
    0 means unregister success
    negative means fail
***************************************************/
static int chg_no_temp_limit = 0;
static int batt_disable_temp_limit(struct huawei_battery_info *info)
{
	int rc = 0;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	return rc;
}

/***************************************************
 Function: chg_no_temp_limit_set
 Description: file node set interface
 Parameters:
    val         the value need to set
    kp          kernel_param
 Return:
    0 means unregister success
    negative means fail
***************************************************/
static int chg_no_temp_limit_set(const char *val, const struct kernel_param *kp)
{
	int rc;
	struct power_supply *batt_psy = NULL;
	struct huawei_battery_info *info = NULL;

	if (!val || !kp) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = param_set_int(val, kp);
	if (rc) {
		pr_err("Unable to set chg_no_temp_limit: %d\n", rc);
		return rc;
	}
	batt_psy = power_supply_get_by_name(CHARGE_PSY_NAME);
	if (!batt_psy) {
		pr_err("batt psy not found\n");
		return 0;
	}
	info = power_supply_get_drvdata(batt_psy);

	return batt_disable_temp_limit(info);
}

static struct kernel_param_ops chg_no_temp_limit_ops = {
	.set = chg_no_temp_limit_set,
	.get = param_get_int,
};
module_param_cb(chg_no_temp_limit, &chg_no_temp_limit_ops, &chg_no_temp_limit, 0644);

/***************************************************
 Function: chg_no_timer_set
 Description: disable safety timer for charging
 Parameters:
    val         the value set by userspace
    kp          the kernel param related with
                    this chg_no_timer property
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static int chg_no_timer = 0;
static int chg_no_timer_set(const char *val, const struct kernel_param *kp)
{
	int rc;
	struct power_supply *batt_psy = NULL;
	struct huawei_battery_info *info = NULL;

	if (!val || !kp) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = param_set_int(val, kp);
	if (rc) {
		pr_err("Unable to set chg_no_timer: %d\n", rc);
		return rc;
	}
	batt_psy = power_supply_get_by_name(CHARGE_PSY_NAME);
	if (!batt_psy) {
		pr_err("batt psy not foudn\n");
		return 0;
	}
	info = power_supply_get_drvdata(batt_psy);

	rc = set_prop_safety_timer_enable(info, !chg_no_timer);
	return rc;
}
static struct kernel_param_ops chg_no_timer_ops = {
	.set = chg_no_timer_set,
	.get = param_get_int,
};
module_param_cb(chg_no_timer, &chg_no_timer_ops, &chg_no_timer, 0644);

/***************************************************
 Function: batt_set_usb_current
 Description: set input current for usb
 Parameters:
    info                huawei_battery_info
 Return:
    0 means read successful
    negative mean fail
***************************************************/
#define DEFAULT_USB_CURRENT 500000
static int chg_usb_current = 0;
static int batt_set_usb_current(struct huawei_battery_info *info)
{
	int rc;

	if (!info) {
		pr_err("%s: Invalid param\n", __func__);
		return -EINVAL;
	}

	if (chg_usb_current > 0) {
		rc = power_supply_set_int_property_value_with_psy(info->usb_psy,
			POWER_SUPPLY_PROP_CURRENT_MAX, chg_usb_current);
		if (rc)
			dev_err(info->dev, "Set bk battery input current fail, rc = %d\n", rc);
	} else {
		rc = power_supply_set_int_property_value_with_psy(info->usb_psy,
			POWER_SUPPLY_PROP_CURRENT_MAX, DEFAULT_USB_CURRENT);
		if (rc)
			dev_err(info->dev, "Set effective battery input current fail, rc = %d\n", rc);
	}
	return rc;
}

/***************************************************
 Function: chg_usb_current_set
 Description: set input current for usb
 Parameters:
    val         the value set by userspace
    kp          the kernel param related with
                    this chg_usb_current property
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static int chg_usb_current_set(const char *val, const struct kernel_param *kp)
{
	int rc;
	struct power_supply *batt_psy = NULL;
	struct huawei_battery_info *info = NULL;

	if (!val || !kp) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = param_set_int(val, kp);
	if (rc) {
		pr_err("Unable to set chg_usb_current; %d\n", rc);
		return rc;
	}
	chg_usb_current *= POWER_UA_PER_MA;
	batt_psy = power_supply_get_by_name(CHARGE_PSY_NAME);
	if (!batt_psy) {
		pr_err("batt psy not found\n");
		return 0;
	}
	info = power_supply_get_drvdata(batt_psy);

	return batt_set_usb_current(info);
}
static struct kernel_param_ops chg_usb_current_ops = {
	.set = chg_usb_current_set,
	.get = param_get_int,
};
module_param_cb(chg_usb_current, &chg_usb_current_ops, &chg_usb_current, 0644);

/***************************************************
 Function: set_fastchg_current_cb
 Description: the callback fucntion for voter,
            used to set fast charge current
 Parameters:
    dev         the device which own vote point
    fcc_ma      the fastchg current voted
    client      voter
    last_fcc_ma the last fastchg current voted
    last_client the last fastchg current voter
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static int set_fastchg_current_cb(struct power_vote_object *obj,
	void *data, int fcc_ua, const char *client)
{
	struct huawei_battery_info *info = data;

	if (!info || !info->bk_batt_psy) {
		pr_err("Invalid bk battery power supply\n");
		return -EINVAL;
	}

	if (fcc_ua < 0) {
		pr_err("No voters\n");
		return 0;
	}

	return power_supply_set_int_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX, fcc_ua);
}

/***************************************************
 Function: set_usb_current_limit_cb
 Description: the callback fucntion for voter,
            used to set charger input current
 Parameters:
    dev         the device which own vote point
    fcc_ma      the input current voted
    client      voter
    last_fcc_ma the last input current voted
    last_client the last input current voter
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static int set_usb_current_limit_cb(struct power_vote_object *obj,
	void *data, int icl_ma, const char *client)
{
	struct huawei_battery_info *info = data;

	if (!info || !info->bk_batt_psy) {
		pr_err("Invalid bk battery power supply\n");
		return -EINVAL;
	}

	if (icl_ma < 0) {
		pr_err("No voters\n");
		return 0;
	}

	if (chg_usb_current > icl_ma)
		icl_ma = chg_usb_current;

	return power_supply_set_int_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_INPUT_CURRENT_MAX, icl_ma);
}

/***************************************************
 Function: set_vbat_cb
 Description: the callback fucntion for voter,
            used to set charge terminal voltage
 Parameters:
    dev         the device which own vote point
    fcc_ma      the terminal voltage voted
    client      voter
    last_fcc_ma the last terminal voltage voted
    last_client the last terminal voltage voter
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static int set_vbat_cb(struct power_vote_object *obj, void *data,
	int vbat_mv, const char *client)
{
	struct huawei_battery_info *info = data;

	if (!info || !info->bk_batt_psy) {
		pr_err("Invalid bk battery power supply\n");
		return -EINVAL;
	}

	if (vbat_mv < 0) {
		pr_err("No voters\n");
		return 0;
	}

	return power_supply_set_int_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_VOLTAGE_MAX, vbat_mv * POWER_UV_PER_MV);
}

/***************************************************
 Function: charging_suspend_cb
 Description: the callback fucntion for voter,
            used to suspend/resume charging
 Parameters:
    dev         the device which own vote point
    fcc_ma      the charging status voted
    client      voter
    last_fcc_ma the last charging status voted
    last_client the last charging status voter
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static int charging_suspend_cb(struct power_vote_object *obj, void *data,
	int suspend, const char *client)
{
	struct huawei_battery_info *info = data;

	if (!info || !info->bk_batt_psy) {
		pr_err("Invalid bk battery power supply\n");
		return -EINVAL;
	}

	if (suspend < 0) {
		pr_err("No voters\n");
		suspend = false;
	}

	return power_supply_set_int_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_BATTERY_CHARGING_ENABLED, !suspend);
}

/***************************************************
 Function: battery_detect_ibus_work
 Description: ibus detect function, used to detect
        the real ibus the charger can support
 Parameters:
    work            work_struct
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static void battery_detect_ibus_work(struct work_struct *work)
{
	struct huawei_battery_info *info = NULL;
	static int ibus_err_count = 0;
	int iusb_ma;
	int current_adjusted;
	union power_supply_propval val = {0, };
	int rc;

	if (!work) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return;
	}

	info = container_of(work, struct huawei_battery_info,
		ibus_detect_work.work);
	if (!info) {
		pr_err("%s: battery info is NULL, fatal error\n", __func__);
		return;
	}

	rc = power_supply_get_property_value_with_psy(info->usb_psy,
		POWER_SUPPLY_PROP_PRESENT, &val);
	if (rc < 0)
		return;

	if (!val.intval) {
		dev_info(info->dev, "Usb absent, Dont do ibus detect\n");
		info->ibus_detecting = false;
		ibus_err_count = 0;
	}

	if (BATTERY_IBUS_CONFIRM_COUNT <= ibus_err_count) {
		ibus_err_count = 0;
		iusb_ma = IINLIM_1000;
	} else {
		rc = power_supply_get_property_value_with_psy(info->bk_batt_psy,
			POWER_SUPPLY_PROP_INPUT_CURRENT_NOW, &val);
		if (rc < 0)
			val.intval = IINLIM_1000;
		iusb_ma = val.intval;
	}

	dev_info(info->dev, "detect input current, iusb_ma = %d\n", iusb_ma);
	if (iusb_ma >= (IINLIM_1000)) {
		if (((IINLIM_1500 + BATTERY_IBUS_MARGIN_MA)) < iusb_ma)
			current_adjusted = IINLIM_2000;
		else if (((IINLIM_1000 + BATTERY_IBUS_MARGIN_MA)) < iusb_ma)
			current_adjusted = IINLIM_1500;
		else
			current_adjusted = IINLIM_1000;
		ibus_err_count = 0;
		info->ibus_detecting = false;
		info->current_adjusted_ma = current_adjusted;
		rc = power_vote_set(USB_ICL_VOTE_OBJECT,
			IBUS_DETECT_VOTER, true, current_adjusted);
		if (rc < 0)
			dev_err(info->dev, "Vote for usb current limit fail, rc %d\n", rc);
		if (info->thermal_limit_current) {
			rc = power_vote_set(USB_ICL_VOTE_OBJECT,
				IN_THERMAL_VOTER, true, info->thermal_limit_current);
			if (rc < 0)
				dev_err(info->dev, "Vote for thermal limit fail, rc %d\n", rc);
		}
	} else {
		ibus_err_count++;
		dev_err(info->dev, "Get ibus error and try,count %d\n", ibus_err_count);
		schedule_delayed_work(&info->ibus_detect_work,
			msecs_to_jiffies(BATTERY_IBUS_WORK_DELAY_TIME));
	}
}

/***************************************************
 Function: batt_imp_curr_limit
 Description: current limit by software jeita
 Parameters:
    info            huawei_battery_info
    curr            the current need to limit
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static inline int batt_imp_curr_limit(struct huawei_battery_info *info, int curr)
{
	int rc = 0;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (!info->jeita_hw_curr_limit) {
		pr_info("Soft Limit current %d for jeita\n", curr);
		rc = power_vote_set(FCC_VOTE_OBJECT, FCC_JEITA_VOTER, true, curr);
	}
	if (rc < 0)
		dev_err(info->dev, "jeita limit fcc(%dmA) fail, rc = %d\n", curr, rc);
	return 0;
}

/***************************************************
 Function: batt_imp_volt_limit
 Description: voltage limit by software jeita
 Parameters:
    info            huawei_battery_info
    volt            the voltage need to limit
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static inline int batt_imp_volt_limit(struct huawei_battery_info *info, int volt)
{
	int rc = 0;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (!info->jeita_hw_volt_limit)
		rc = power_vote_set(VBAT_VOTE_OBJECT, VBAT_JEITA_VOTER, true, volt);
	if (rc < 0)
		dev_err(info->dev, "jeita limit vbat(%dmV) fail, rc = %d\n", volt, rc);
	return 0;
}

/***************************************************
 Function: batt_imp_chg_dis
 Description: disable charging by software jeita
 Parameters:
    info            huawei_battery_info
    enable          enable/disable flag
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static inline int batt_imp_chg_dis(struct huawei_battery_info *info, bool enable)
{
	int rc = 0;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (!info->jeita_hw_chg_dis)
		rc = power_vote_set(BCH_VOTE_OBJECT, BCH_JEITA_VOTER, enable, 0);
	if (rc < 0)
		dev_err(info->dev, "jeita enable charging fail, rc = %d\n", rc);
	return 0;
}

/***************************************************
 Function: battery_jeita_solution
 Description: jeita solution main function
 Parameters:
    info            huawei_battery_info
 Return:
    0 means read successful
    negative mean fail
***************************************************/
static int battery_jeita_solution(struct huawei_battery_info *info)
{
	union power_supply_propval val = {0, };
	static int last_health = POWER_SUPPLY_HEALTH_UNKNOWN;
	int rc;
	int health;

	if (!info) {
		pr_err("Invalid battery info point\n");
		return -EINVAL;
	}

	rc = power_supply_get_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_HEALTH, &val);
	if (rc) {
		dev_err(info->dev, "Cannot get health from bk battery, rc = %d\n", rc);
		return rc;
	}
	health = val.intval;

	if (IS_ENABLED(CONFIG_HLTHERM_RUNTEST) || chg_no_temp_limit)
		if ((health != POWER_SUPPLY_HEALTH_WARM) \
			&& (health != POWER_SUPPLY_HEALTH_OVERHEAT))
			health = POWER_SUPPLY_HEALTH_GOOD;

	pr_info("battery jeita solution: get health %d, "
		"last health %d\n", health, last_health);

	if (health == last_health)
		return 0;

	switch (health) {
	case POWER_SUPPLY_HEALTH_GOOD:
		if (last_health == POWER_SUPPLY_HEALTH_WARM)
			power_vote_set(BCH_VOTE_OBJECT, BCH_WARM_WR_VOTER, true, 0);
		batt_imp_curr_limit(info, info->fastchg_max_current);
		batt_imp_volt_limit(info, info->fastchg_max_voltage);
		batt_imp_chg_dis(info, false);
		if (last_health == POWER_SUPPLY_HEALTH_WARM)
			power_vote_set(BCH_VOTE_OBJECT, BCH_WARM_WR_VOTER, false, 0);
		break;
	case POWER_SUPPLY_HEALTH_COOL:
		batt_imp_curr_limit(info, info->fastchg_cool_current);
		batt_imp_volt_limit(info, info->fastchg_cool_voltage);
		batt_imp_chg_dis(info, false);
		break;
	case POWER_SUPPLY_HEALTH_WARM:
		batt_imp_curr_limit(info, info->fastchg_warm_current);
		batt_imp_volt_limit(info, info->fastchg_warm_voltage);
		batt_imp_chg_dis(info, false);
		break;
	case POWER_SUPPLY_HEALTH_OVERHEAT:
	case POWER_SUPPLY_HEALTH_COLD:
		batt_imp_chg_dis(info, true);
		break;
	default:
		break;
	}

	last_health = health;
	return 0;
}

/***************************************************
 Function: battery_jeita_work
 Description: the work used to implementation jeita
        solution
 Parameters:
    work                work_struct
 Return: NA
***************************************************/
static void battery_jeita_work(struct work_struct *work)
{
	struct huawei_battery_info *info = NULL;

	if (!work) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return;
	}

	info = container_of(work, struct huawei_battery_info, jeita_work.work);

	battery_jeita_solution(info);
}

/***************************************************
 Function: battery_health_handler
 Description: used to accept battery health change
        notification
 Parameters:
    self                notifier_block
    event               event id
    data                private data, can be NULL
 Return:
    0 means success
    negative means fail
***************************************************/
int battery_health_handler(void)
{
	struct huawei_battery_info *info = NULL;
	struct power_supply *batt_psy = power_supply_get_by_name(CHARGE_PSY_NAME);

	if (!batt_psy) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	info = power_supply_get_drvdata(batt_psy);
	if (info->disable_huawei_func) {
		pr_err("not use huawei solution\n");
		return 0;
	}
	pr_info("Schdule jeita work\n");
	schedule_delayed_work(&info->jeita_work, 0);

	return 0;
}

/***************************************************
 Function: battery_monitor_charging_work
 Description: monitor work for current limit in
        [0 ~ 5] temperature
 Parameters:
    work            work_struct
 Return: NA
***************************************************/
#define MONITOR_CHARGING_DELAY_TIME 30000
#define MONITOR_CHARGING_QUICKEN_DELAY_TIME 1000
#define MONITOR_CHARGING_QUICKEN_TIMES      1

#define VBAT_LOW_TH     3400000
static void battery_monitor_charging_work(struct work_struct *work)
{
	struct huawei_battery_info *info = NULL;
	union power_supply_propval val = {0, };
	int rc;

	pr_info("%s enter\n", __func__);
	if (!work) {
		pr_err("invalid param, fatal error\n");
		return;
	}

	info = container_of(work, struct huawei_battery_info,
		monitor_charging_work.work);
	if (!info) {
		pr_err("invalid battery info point\n");
		return;
	}

	charge_set_monitor_work_flag(CHARGE_MONITOR_WORK_NEED_START);

	pr_info("%s try fcp check, info->chr_type = %d\n", __func__,
		info->chr_type);

	rc = power_supply_get_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_FLASH_ACTIVE, &val);
	if (rc == 0 && val.intval)
		info->enable_hv_charging = 0;
	pr_info("%s flash val.intval= %d  info->enable_hv_charging = %d\n", __func__,
		val.intval, info->enable_hv_charging);

	if (((info->chr_type == CHARGER_TYPE_STANDARD) || (info->chr_type == CHARGER_TYPE_FCP))
		&& direct_charge_get_stop_charging_complete_flag()) {
		if (direct_charge_in_charging_stage() == DC_NOT_IN_CHARGING_STAGE)
			direct_charge_check(info->enable_hv_charging);

		if (direct_charge_in_charging_stage() == DC_NOT_IN_CHARGING_STAGE &&
			(charge_get_quicken_work_flag() != MONITOR_CHARGING_QUICKEN_TIMES)) {
			pr_info("%s begin fcp check\n", __func__);
			fcp_charge_check(info);
		}
	}
	pr_info("%s chr_type=%d, charging_stage=%d, standard=%d\n", __func__,
		info->chr_type, direct_charge_in_charging_stage(),
		CHARGER_TYPE_STANDARD);
	/* here implement jeita standard */
	if (charge_get_quicken_work_flag() == MONITOR_CHARGING_QUICKEN_TIMES)
		schedule_delayed_work(&info->monitor_charging_work,
			msecs_to_jiffies(MONITOR_CHARGING_QUICKEN_DELAY_TIME));
	else
		schedule_delayed_work(&info->monitor_charging_work,
			msecs_to_jiffies(MONITOR_CHARGING_DELAY_TIME));
}
/***************************************************
 Function: sync_max_voltage
 Description: sync max voltage between bms and bk battery
 Parameters:
    info            huawei_battery_info
 Return:
    0 means sccuess, negative means error
***************************************************/
static int sync_max_voltage(struct huawei_battery_info *info)
{
	int rc;
	union power_supply_propval val = {0, };

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = info->bms_psy->desc->get_property(info->bms_psy,
		POWER_SUPPLY_PROP_VOLTAGE_MAX, &val);
	if (rc < 0)
		return rc;

	rc = power_vote_set(VBAT_VOTE_OBJECT, VBAT_BASP_VOTER, true, val.intval);

	return rc;
}

/***************************************************
 Function: battery_sync_voltage_work
 Description: sync max voltage between bms and bk battery

 Parameters:
    work            work_struct
 Return: NA
***************************************************/
static void battery_sync_voltage_work(struct work_struct *work)
{
	struct huawei_battery_info *info = NULL;

	if (!work) {
		pr_err("Invalid param, fatal error\n");
		return;
	}

	info = container_of(work, struct huawei_battery_info,
		sync_voltage_work.work);
	if (!info) {
		pr_err("Invalid battery info point\n");
		return;
	}

	sync_max_voltage(info);
}

/***************************************************
 Function: get_batt_health
 Description: get the real battery health
 Parameters:
    info            huawei_battery_info
 Return:
    negative mean fail
    others are battery health, possiable value:
        POWER_SUPPLY_HEALTH_OVERHEAT
        POWER_SUPPLY_HEALTH_COLD
        POWER_SUPPLY_HEALTH_WARM
        POWER_SUPPLY_HEALTH_COOL
        POWER_SUPPLY_HEALTH_GOOD
***************************************************/
static int get_batt_health(struct huawei_battery_info *info)
{
	int rc;
	int batt_temp;
	union power_supply_propval val = {0, };
	int batt_vol;

	if (!info) {
		pr_err("Invali param for %s\n", __func__);
		return -EINVAL;
	}

	if (IS_ENABLED(CONFIG_HLTHERM_RUNTEST) || chg_no_temp_limit)
		return POWER_SUPPLY_HEALTH_GOOD;

	rc = power_supply_get_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_TEMP, &val);
	if (rc < 0) {
		pr_err("Cannot get battery temp\n");
		return -EINVAL;
	}

	batt_temp = val.intval;

	rc = power_supply_get_int_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_VOLTAGE_NOW, &batt_vol);
	if (!rc) {
		if (batt_vol < info->cutoff_voltage_normal) {
			if (g_vol_cutoff_filter_cnt >= VOL_CUTOFF_FILTER_MAX_CNT) {
				pr_err("vol low, cutoff. batt_vol = %d\n", batt_vol);
				return POWER_SUPPLY_HEALTH_UNDERVOLTAGE;
			} else {
				g_vol_cutoff_filter_cnt++;
			}
		} else {
			g_vol_cutoff_filter_cnt = 0;
		}
	} else {
		pr_err("Cannot get battery vol\n");
	}

	if (batt_temp > BATT_HEALTH_TEMP_HOT)
		return POWER_SUPPLY_HEALTH_OVERHEAT;
	else if (batt_temp < BATT_HEALTH_TEMP_COLD)
		return POWER_SUPPLY_HEALTH_COLD;
	else if (batt_temp > BATT_HEALTH_TEMP_WARM)
		return POWER_SUPPLY_HEALTH_WARM;
	else if (batt_temp < BATT_HEALTH_TEMP_COOL)
		return POWER_SUPPLY_HEALTH_COOL;
	else
		return POWER_SUPPLY_HEALTH_GOOD;
}

/***************************************************
 Function: get_prop_charge_full_design
 Description: get charge full design from fg psy
 Parameters:
    info            huawei_battery_info
 Return:
    battery full design capacity
***************************************************/
#define DEFAULT_BATT_FULL_CHG_CAPACITY  0
static int get_prop_charge_full_design(struct huawei_battery_info *info)
{
	int rc;
	union power_supply_propval val = {0, };

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (info->charger_batt_capacity_mah > 0)
		return info->charger_batt_capacity_mah * POWER_UA_PER_MA;

	rc = power_supply_get_property_value_with_psy(info->bms_psy,
		POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, &val);
	if (rc < 0) {
		pr_err("Couldnot get charge full rc = %d\n", rc);
		return DEFAULT_BATT_FULL_CHG_CAPACITY;
	}
	return val.intval;
}

/***************************************************
 Function: get_prop_batt_resistance_id
 Description: get battery id ressitance
 Parameters:
    info            huawei_battery_info
 Return:
    battery id resistance
***************************************************/
#define DEFAULT_BATT_RESISTANCE_ID  0
static int get_prop_batt_resistance_id(struct huawei_battery_info *info)
{
	int rc;
	union power_supply_propval val = {0, };

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = power_supply_get_property_value_with_psy(info->bms_psy,
		POWER_SUPPLY_PROP_RESISTANCE_ID, &val);
	if (rc < 0) {
		pr_err("Couldn't get resistance id rc = %d\n", rc);
		return DEFAULT_BATT_RESISTANCE_ID;
	}
	return val.intval;
}

/***************************************************
 Function: get_prop_charge_full
 Description: get charge full capacity
 Parameters:
    info            huawei_battery_info
 Return:
    battery charge full capacity, learned by FG
***************************************************/
#define DEFAULT_BATT_FULL_CHG_CAPACITY  0
static int get_prop_charge_full(struct huawei_battery_info *info)
{
	int rc;
	union power_supply_propval val = {0, };

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = power_supply_get_property_value_with_psy(info->bms_psy,
		POWER_SUPPLY_PROP_CHARGE_FULL, &val);
	if (rc < 0) {
		pr_err("Couldn't get charge_full rc = %d\n", rc);
		return DEFAULT_BATT_FULL_CHG_CAPACITY;
	}
	return val.intval;
}

/***************************************************
 Function: get_prop_batt_profile
 Description: get battery profile status
 Parameters:
    info            huawei_battery_info
 Return:
    battery profile status
***************************************************/
static int get_prop_batt_profile(struct huawei_battery_info *info)
{
	int rc;
	union power_supply_propval val = {0, };

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = power_supply_get_property_value_with_psy(info->bms_psy,
		POWER_SUPPLY_PROP_PROFILE_STATUS, &val);
	if (rc < 0) {
		pr_err("Couldn't get profile status rc = %d\n", rc);
		return 0;
	}
	return val.intval;
}

/***************************************************
 Function: get_prop_cycle_count
 Description: get battery charging cycle count
 Parameters:
    info            huawei_battery_info
 Return:
    battery charging cycle count
***************************************************/
static int get_prop_cycle_count(struct huawei_battery_info *info)
{
	int rc;
	union power_supply_propval val = {0, };

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	rc = power_supply_get_property_value_with_psy(info->bms_psy,
		POWER_SUPPLY_PROP_CYCLE_COUNT, &val);
	if (rc < 0) {
		pr_err("Couldnot get cycle count, rc = %d\n", rc);
		return 0;
	}
	return val.intval;
}

/***************************************************
 Function: set_prop_iin_thermal
 Description: set iin thermal current limit
 Parameters:
    info            huawei_battery_info
    val             iin thermal limit
 Return:
    0 mean set current limit success
    negative mean set iin thermal limit fail
***************************************************/
static int set_prop_iin_thermal(struct huawei_battery_info *info, int value)
{
	int rc = 0;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (value <= 0)
		info->thermal_limit_current = info->usb_icl_max;
	else
		info->thermal_limit_current = min(value, info->usb_icl_max);
	pr_err("%s %d %d %d\n", __func__, value, info->usb_icl_max,
		info->thermal_limit_current);
	if (!info->ibus_detecting)
		rc = power_vote_set(USB_ICL_VOTE_OBJECT, IN_THERMAL_VOTER,
			true, info->thermal_limit_current);
	return rc;
}

/***************************************************
 Function: set_prop_safety_timer_enable
 Description: enable/disable charging safety timer
 Parameters:
    info            huawei_battery_info
    user_timer      enable/disable flag
 Return:
    0 mean enable/disable timer success
    negative mean enable/disable timer fail
***************************************************/
static int set_prop_safety_timer_enable(struct huawei_battery_info *info,
	int use_timer)
{
	int val;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	if (chg_no_timer || IS_ENABLED(CONFIG_HLTHERM_RUNTEST))
		val = 0;
	else
		val = use_timer;

	return power_supply_set_int_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_SAFETY_TIMER_ENABLE, val);
}

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
static int set_prop_input_current_max(struct huawei_battery_info *info, int curr)
{
	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	return power_supply_set_int_property_value("usb",
		POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT, curr * POWER_UA_PER_MA);
}
#else
static int set_prop_input_current_max(struct huawei_battery_info *info, int curr)
{
	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	return vote(info->usb_icl_votable, ICL_USER_VOTER, true, curr);
}
#endif /* CONFIG_QTI_PMIC_GLINK */

static void huawei_charger_release(struct huawei_battery_info *info)
{
	charge_reset_quicken_work_flag();
	charge_set_monitor_work_flag(CHARGE_MONITOR_WORK_NEED_STOP);
	if (adapter_set_default_param(ADAPTER_PROTOCOL_FCP))
		pr_err("fcp set default param failed\n");
#ifdef CONFIG_DIRECT_CHARGER
	if (!direct_charge_get_cutoff_normal_flag())
		direct_charge_set_stage_status_default();
	power_icon_notify(ICON_TYPE_INVALID);
	/* notify stop charging event */
	power_event_bnc_notify(POWER_BNT_CHARGING, POWER_NE_CHARGING_STOP, NULL);
	/* notify event to power ui */
	power_ui_event_notify(POWER_UI_NE_DEFAULT, NULL);
#endif
	cancel_delayed_work_sync(&info->monitor_charging_work);
	hvdcp_reset_flags();
	if (adapter_stop_charging_config(ADAPTER_PROTOCOL_FCP))
		pr_err("fcp stop charge config failed\n");
	hvdcp_set_charging_stage(HVDCP_STAGE_DEFAUTL);
	hvdcp_set_charging_flag(false);
	set_first_insert(0); /* clear first insert flag */
#ifdef CONFIG_DIRECT_CHARGER
	if (!direct_charge_get_cutoff_normal_flag())
		direct_charge_exit();
	direct_charge_update_cutoff_normal_flag();
	dc_set_adapter_default_param();
	(void)dcm_reset_and_init_ic_reg(SC_MODE, CHARGE_IC_MAIN);
#endif
}

static void charger_plug_work(struct work_struct *work)
{
	struct huawei_battery_info *info = container_of(work, struct huawei_battery_info,
		charger_plug_work);

	huawei_charger_release(info);
	power_wakeup_unlock(info->chg_ws, false);
}

static void force_switch_dc_channel(bool enable)
{
	if (!g_info->buck_need_sc_sw_on)
		return;

	if (enable)
		wired_chsw_set_wired_channel(WIRED_CHANNEL_SC, WIRED_CHANNEL_RESTORE);
	else
		wired_chsw_set_wired_channel(WIRED_CHANNEL_SC, WIRED_CHANNEL_CUTOFF);
}

static int proc_charger_plug_action(struct huawei_battery_info *info, int vbus_rise)
{
	pr_info("%s: vbus_rise = %d\n", __func__, vbus_rise);

	if (vbus_rise) {
		power_wakeup_lock(info->chg_ws, false);
		force_switch_dc_channel(true);
	} else {
		force_switch_dc_channel(false);
		schedule_work(&info->charger_plug_work);
	}

	return 0;
}

static void charger_action_work(struct work_struct *work)
{
	bool flag = TRUE;
	struct huawei_battery_info *info = container_of(work, struct huawei_battery_info,
		charger_action_work);

	if (!info) {
		pr_err("invalid battery info point\n");
		return;
	}

	flag = schedule_delayed_work(&info->monitor_charging_work, 0);
	if (!flag) {
		cancel_delayed_work_sync(&info->monitor_charging_work);
		flag = schedule_delayed_work(&info->monitor_charging_work, 0);
		pr_info("charger_action_work:work one faild, flag = %d\n", flag);
	}
}

static int proc_charger_action(struct huawei_battery_info *info, int type)
{
	info->chr_type = charge_convert_charger_type(type);
	if (info->chr_type != CHARGER_TYPE_STANDARD)
		return 0;

	schedule_work(&info->charger_action_work);

	return 0;
}

static int enable_hv_charging(struct huawei_battery_info *info, int flash_flag)
{
	int rc = 0;

	if (!info)
		return -EINVAL;

	pr_err("enable_hv_charging = %d, flash_flag = %d\n",
		info->enable_hv_charging, flash_flag);
	if (info->enable_hv_charging) {
		if (flash_flag) {
			pr_err("disable sc/fcp\n");
			info->enable_hv_charging = 0;
			set_direct_flag(1); /* flash is working */
			if (info->chr_type == CHARGER_TYPE_STANDARD)
				cam_stop_charging();
			force_stop_fcp_charging(info);
		}
	} else {
		if (!flash_flag) {
			set_direct_flag(0); /* camera exit */
			info->enable_hv_charging = 1;
		}
	}
	return rc;
}

#define DEFAULT_BATT_HALF_CAPACITY  50

static int get_bms_capacity_raw(struct huawei_battery_info *info)
{
	int rc;
	int val = 0;

	if (!info)
		return -EINVAL;

	rc = power_supply_get_int_property_value_with_psy(info->bms_psy,
		POWER_SUPPLY_PROP_CAPACITY_RAW, &val);
	if (rc < 0) {
		pr_err("Couldn't get bms raw rc = %d\n", rc);
		return DEFAULT_BATT_HALF_CAPACITY;
	}
	return val;
}

static int get_bms_current_avg(struct huawei_battery_info *info)
{
	int rc;
	int val = 0;

	if (!info)
		return -EINVAL;

	rc = power_supply_get_int_property_value_with_psy(info->bms_psy,
		POWER_SUPPLY_PROP_CURRENT_AVG, &val);
	if (rc < 0) {
		pr_err("Couldn't get bms raw rc = %d\n", rc);
		return 0;
	}
	return val;
}

int battery_get_soc(void)
{
	int soc;

	if (!power_proxy_get_capacity(&soc))
		return soc;
	if (!g_info)
		return DEFAULT_BATT_HALF_CAPACITY;
	return get_bms_capacity_raw(g_info);
}

int battery_get_bat_avg_current(void)
{
	int avg_val;

	if (!power_proxy_get_bat_current_avg(&avg_val))
		return avg_val * 10; /* 10: unit convert to 0.1mA */

	return get_bms_current_avg(g_info);
}

static int get_batt_present(struct huawei_battery_info *info)
{
	int rc;
	int val = 0;

	if (!info)
		return 1;

	rc = power_supply_get_int_property_value_with_psy(info->bk_batt_psy,
		POWER_SUPPLY_PROP_PRESENT, &val);
	if (rc < 0) {
		pr_err("Couldn't get bk batt raw rc = %d\n", rc);
		return 1;
	}
	return val;
}

int battery_get_present(void)
{
	int batt_present = 0;

	if (!power_proxy_is_battery_exit(&batt_present))
		return batt_present;
	if (!g_info)
		return 1;
	return get_batt_present(g_info);
}

static enum power_supply_property battery_ext_props[] = {
	POWER_SUPPLY_PROP_CYCLE_COUNT,
	POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_FLASH_ACTIVE,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_CHG_TYPE,
};

/***************************************************
 Function: battery_get_property
 Description: get battery power supply property interface
 Parameters:
    psy             power supply instance
    prop            the property need to get
    val             the place to hold values
 Return:
    0 mean get prop successful
    negative mean get prop fail
 Usage:
    For the properties we have to handle in this driver,
 need to return directly after we get the valid property
 value, otherwise we will get the property value from
 original charger driver and then do post process.
***************************************************/
static int battery_get_property(struct power_supply *psy,
	enum power_supply_property prop, union power_supply_propval *val)
{
	int rc;
	struct huawei_battery_info *info = NULL;

	if (!psy || !val) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	info = power_supply_get_drvdata(psy);

	/* preprocess */
	switch (prop) {
	case POWER_SUPPLY_PROP_IIN_THERMAL:
		val->intval = info->thermal_limit_current;
		return 0;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = get_batt_health(info);
		if (val->intval < 0)
			break;
		return 0;
	case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN:
		val->intval = get_prop_charge_full_design(info);
		return 0;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
		return power_supply_get_property_value_with_psy(
			info->bms_psy, prop, val);
	case POWER_SUPPLY_PROP_RESISTANCE_ID:
		val->intval = get_prop_batt_resistance_id(info);
		return 0;
	case POWER_SUPPLY_PROP_CHARGE_FULL:
		val->intval = get_prop_charge_full(info);
		return 0;
	case POWER_SUPPLY_PROP_PROFILE_STATUS:
		val->intval = get_prop_batt_profile(info);
		return 0;
	case POWER_SUPPLY_PROP_CYCLE_COUNT:
		val->intval = get_prop_cycle_count(info);
		return 0;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (info->fake_soc >= EMPTY_SOC &&
			info->fake_soc <= FULL_SOC) {
			val->intval = info->fake_soc;
			return 0;
		}
		break;
	case POWER_SUPPLY_PROP_TEMP:
		if (IS_ENABLED(CONFIG_HLTHERM_RUNTEST) || chg_no_temp_limit) {
			val->intval = DEFAULT_BATT_TEMP;
			return 0;
		}
		break;
	case POWER_SUPPLY_PROP_INPUT_CURRENT_MAX:
		val->intval = info->usb_icl_max;
		return 0;
	default:
		break;
	}

	/* call bk_battery function */
	rc = power_supply_get_property_value_with_psy(info->bk_batt_psy, prop, val);

	/* post process */
	switch (prop) {
	case POWER_SUPPLY_PROP_SYSTEM_TEMP_LEVEL:
		if (rc < 0) {
			val->intval = 0;
			rc = 0;
		}
		break;
	case POWER_SUPPLY_PROP_CHARGER_PRESENT:
		if (rc < 0)
			rc = power_supply_get_property_value_with_psy(info->usb_psy, prop, val);
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (power_cmdline_is_factory_mode() &&
			(val->intval <= FACTORY_SOC_LOW)) {
			val->intval = FACTORY_SOC_LOW;
			return 0;
		}
		break;
	default:
		break;
	}

	return rc;
}

/***************************************************
 Function: battery_set_property
 Description: set battery power supply property interface
 Parameters:
    psy             power supply instance
    prop            the property need to set
    val             the place to hold values
 Return:
    0 mean set prop successful
    negative mean get prop fail
 Usage:
    For the properties we have to handle in this driver,
 need to return directly after we set the property
 value, otherwise we will set the property value to
 original charger driver and then do post process.
***************************************************/
static int battery_set_property(struct power_supply *psy,
	enum power_supply_property prop, const union power_supply_propval *val)
{
	int rc = 0;
	struct huawei_battery_info *info = NULL;

	if (!psy || !val) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	pr_info("%s prop = %d, val->intval = %d\n", __func__, prop, val->intval);

	info = power_supply_get_drvdata(psy);
	if (!info) {
		pr_err("%s: invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	/* pre process */
	switch (prop) {
	case POWER_SUPPLY_PROP_IIN_THERMAL:
		rc = set_prop_iin_thermal(info, val->intval);
		goto OUT;
	case POWER_SUPPLY_PROP_CAPACITY:
		info->fake_soc = val->intval;
		goto OUT;
	case POWER_SUPPLY_PROP_SAFETY_TIMER_ENABLE:
		rc = set_prop_safety_timer_enable(info, val->intval);
		goto OUT;
	case POWER_SUPPLY_PROP_INPUT_CURRENT_MAX:
		rc = set_prop_input_current_max(info, val->intval);
		goto OUT;
	case POWER_SUPPLY_PROP_BATTERY_CHARGING_ENABLED:
		rc = power_vote_set(BCH_VOTE_OBJECT, BCH_USER_VOTER, !val->intval, 0);
		pr_err("%s: bch_votable charging_suspend_cb = %d\n", __func__, !val->intval);
		goto OUT;
	case POWER_SUPPLY_PROP_VOLTAGE_BASP:
		rc = power_vote_set(VBAT_VOTE_OBJECT, VBAT_BASP_VOTER, true, val->intval);
		goto OUT;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		rc = power_vote_set(VBAT_VOTE_OBJECT, VBAT_USER_VOTER, true, val->intval);
		goto OUT;
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX:
		rc = power_vote_set(FCC_VOTE_OBJECT, FCC_USER_VOTER, true, val->intval);
		goto OUT;
	case POWER_SUPPLY_PROP_CHG_PLUGIN:
		rc = proc_charger_plug_action(info, val->intval);
		pr_info("%s POWER_SUPPLY_PROP_CHG_PLUGIN\n", __func__);
		return rc;
	case POWER_SUPPLY_PROP_CHG_TYPE:
		rc = proc_charger_action(info, val->intval);
		pr_info("%s set_charge_type_mode return %d\n", __func__, rc);
		return rc;
	case POWER_SUPPLY_PROP_FLASH_ACTIVE:
		rc = enable_hv_charging(info, val->intval);
		pr_info("%s enable_hv_charging flash_flag = %d, return %d\n",
			__func__, val->intval, rc);
		break;
	default:
		break;
	}

	/* call bk_battery function */
	rc = power_supply_set_property_value_with_psy(info->bk_batt_psy, prop, val);

	/* post process */
	switch (prop) {
	default:
		break;
	}

OUT:
	if (info->batt_psy)
		power_supply_changed(info->batt_psy);
	return rc;
}

/***************************************************
 Function: battery_property_is_writeable
 Description: check battery property writeable
 Parameters:
    psy             power supply instance
    prop            the property need to check
 Return:
    0 mean property is not writeable
    1 mean property is writeable
***************************************************/
static int battery_property_is_writeable(struct power_supply *psy,
	enum power_supply_property prop)
{
	int rc;
	struct huawei_battery_info *info = NULL;

	if (!psy || (!psy->desc)) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return 0;
	}
	info = power_supply_get_drvdata(psy);
	if (!info || (!info->bk_batt_psy)) {
		pr_info("%s info or bk battery is null \n", __func__);
		return 1;
	}

	/* pre process */
	switch (prop) {
	case POWER_SUPPLY_PROP_IIN_THERMAL:
	case POWER_SUPPLY_PROP_CAPACITY:
	case POWER_SUPPLY_PROP_SAFETY_TIMER_ENABLE:
	case POWER_SUPPLY_PROP_INPUT_CURRENT_MAX:
	case POWER_SUPPLY_PROP_CHG_TYPE:
	case POWER_SUPPLY_PROP_CHG_PLUGIN:
	case POWER_SUPPLY_PROP_FLASH_ACTIVE:
		rc = 1;
		return rc;
	default:
		break;
	}

	rc = info->bk_batt_psy->desc->property_is_writeable(info->bk_batt_psy, prop);

	if (rc < 0) {
		rc = 0;
		dev_warn(info->dev, "Check prop %d is writeable fail\n", prop);
	}

	/* post process */
	switch (prop) {
	default:
		break;
	}
	return rc;
}

/***************************************************
 Function: battery_initial_status
 Description: determine the battery init status
 Parameters:
    info            huawei_battery_info
 Return:
    0 mean init status successful
    negative mean fail
***************************************************/
#define CHG_UNSATE_TIMER_CURRENT 900000
static int battery_initial_status(struct huawei_battery_info *info)
{
	/* use -1 as invalid soc */
	info->fake_soc = -1;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	if (IS_ENABLED(CONFIG_HLTHERM_RUNTEST)) {
		/* for high/low temp test
		 * 1. disable charging temp limit
		 * 2. disable charging safety timer
		 */
		chg_no_timer = 1;
		set_prop_safety_timer_enable(info, false);
	} else if (IS_ENABLED(CONFIG_LONG_TIME_TEST_WITH_USB)) {
		/* for long time test
		 * 1. disalbe charging safety timer
		 * 2. enlarge usb curren to 900mA
		 */
		chg_no_timer = 1;
		chg_usb_current = CHG_UNSATE_TIMER_CURRENT;
		set_prop_safety_timer_enable(info, false);
		batt_set_usb_current(info);
	}

	return 0;
}

/***************************************************
 Function: battery_parse_jeita_config
 Description: parse jeita config
 Parameters:
    node            device node need to parse
    info            huawei_battery_info
 Return:
    0 mean parse device node successful
    negative mean parse device node fail
***************************************************/
#define of_read_u32(name, var, def) \
	rc = of_property_read_u32(node, "huawei," name, &var); \
	if (rc < 0) { \
		dev_info(info->dev, "Cannot get " name ", use default %d\n", def); \
		var = def; \
	}

static int battery_parse_jeita_config(struct device_node *node,
	struct huawei_battery_info *info)
{
	int rc;
	u32 tmp[CHG_DIS_REG_NUM] = {0};

	if (!node || !info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	if (!info->jeita_use_interrupt) {
		dev_info(info->dev, "Using loop solutino for jeita\n");
		return 0;
	}
	info->jeita_use_interrupt = of_property_read_bool(node,
		"huawei,jeita-use-interrupt");
	if (!info->jeita_use_interrupt) {
		dev_info(info->dev, "Using loop solutino for jeita\n");
		return 0;
	}

	dev_info(info->dev, "Using interrupt solution for jeita\n");
	info->jeita_hw_curr_limit = of_property_read_bool(node,
		"huawei,jeita-hardware-current-limit");
	info->jeita_hw_volt_limit = of_property_read_bool(node,
		"huawei,jeita-hardware-voltage-limit");
	info->jeita_hw_chg_dis = of_property_read_bool(node,
		"huawei,jeita-hardware-charge-disable");
	if (IS_ENABLED(CONFIG_HLTHERM_RUNTEST))
		info->jeita_hw_chg_dis = false;
	dev_info(info->dev, "Using %s curr limit, %s volt limit, %s chg disable\n",
		info->jeita_hw_curr_limit ? "hardware" : "software",
		info->jeita_hw_volt_limit ? "hardware" : "software",
		info->jeita_hw_chg_dis ? "hardware" : "software");
	if (!info->jeita_hw_chg_dis)
		return 0;
	rc = of_property_read_u32_array(node,
		"huawei,jeita-hardware-charge-disable-reg", tmp, CHG_DIS_REG_NUM);
	if (rc) {
		dev_err(info->dev, "Get jeita hw chg dis reg fail, rc = %d\n", rc);
		info->jeita_hw_chg_dis = false;
	} else {
		info->jeita_chg_dis_reg = tmp[0];
		info->jeita_chg_dis_mask = tmp[1];
		info->jeita_chg_dis_value = tmp[2];
		dev_info(info->dev, "jeita param 0x%x, 0x%x, 0x%x\n",
			info->jeita_chg_dis_reg,
			info->jeita_chg_dis_mask,
			info->jeita_chg_dis_value);
	}

	return 0;
}

/***************************************************
 Function: battery_parse_config
 Description: parse curr & volt config
 Parameters:
    node            device node need to parse
    info            huawei_battery_info
 Return:
    0 mean parse device node successful
    negative mean parse device node fail
***************************************************/
static int battery_parse_config(struct device_node *node,
	struct huawei_battery_info *info)
{
	int rc = 0;

	if (!node || !info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	of_read_u32("fastchg-max-current",
		info->fastchg_max_current, DEFAULT_FASTCHG_MAX_CURRENT);
	pr_info("%s: fastchg max current %dmA\n",
		__func__, info->fastchg_max_current);

	of_read_u32("fastchg-max-voltage",
		info->fastchg_max_voltage, DEFAULT_FASTCHG_MAX_VOLTAGE);
	pr_info("%s: fastchg max voltage %dmV\n",
		__func__, info->fastchg_max_voltage);

	of_read_u32("fastchg-warm-current",
		info->fastchg_warm_current, DEFAULT_FASTCHG_WARM_CURRENT);
	pr_info("%s: fastchg warm current %dmA\n",
		__func__, info->fastchg_warm_current);

	of_read_u32("fastchg-warm-voltage",
		info->fastchg_warm_voltage, DEFAULT_FASTCHG_WARM_VOLTAGE);
	pr_info("%s: fastchg warm voltage %dmV\n",
		__func__, info->fastchg_warm_voltage);

	of_read_u32("fastchg-cool-current",
		info->fastchg_cool_current, DEFAULT_FASTCHG_COOL_CURRENT);
	pr_info("%s: fastchg cool current %dmA\n",
		__func__, info->fastchg_cool_current);

	of_read_u32("fastchg-cool-voltage",
		info->fastchg_cool_voltage, DEFAULT_FASTCHG_COOL_VOLTAGE);
	pr_info("%s: fastchg cool voltage %dmV\n",
		__func__, info->fastchg_cool_voltage);

	of_read_u32("usb-icl-max", info->usb_icl_max, DEFAULT_USB_ICL_CURRENT);
	pr_info("%s: usb icl max %dmA\n", __func__, info->usb_icl_max);

	of_read_u32("customize-cool-lower-limit",
		info->customize_cool_lower_limit, DEFAULT_CUSTOM_COOL_LOW_TH);
	pr_info("%s: customize cool lower limit %d\n",
		__func__, info->customize_cool_lower_limit);

	of_read_u32("customize-cool-upper-limit",
		info->customize_cool_upper_limit, DEFAULT_CUSTOM_COOL_UP_TH);
	pr_info("%s: customize cool upper limit %d\n",
		__func__, info->customize_cool_upper_limit);

	of_read_u32("fastchg-current-customize-cool-ma",
		info->fastchg_current_customize_cool, DEFAULT_CUSTOM_COOL_CURRENT);
	pr_info("%s: fastchg current cusome cool ma %d\n",
		__func__, info->fastchg_current_customize_cool);

	of_read_u32("charger-batt-capacity-mah", info->charger_batt_capacity_mah, 0);
	pr_info("%s: charger battery capacity mah is %d\n",
		__func__, info->charger_batt_capacity_mah);

	info->ibus_detect_disable = of_property_read_bool(node,
		"huawei,ibus-detect-disable");
	pr_info("%s: ibus detect feature is %s\n",
		__func__, info->ibus_detect_disable ? "disabled" : "enabled");

	of_read_u32("cutoff_voltage_normal", info->cutoff_voltage_normal,
		DEFAULT_VOL_CUTOFF_NORMAL);
	pr_info("%s: charger battery cutoff vol is %d\n",
		__func__, info->cutoff_voltage_normal);
	return 0;
}

/***************************************************
 Function: battery_parse_dt
 Description: parse dts config interface
 Parameters:
    node            device node need to parse
    info            huawei_battery_info
 Return:
    0 mean parse device node successful
    negative mean parse device node fail
***************************************************/
static int battery_parse_dt(struct huawei_battery_info *info)
{
	int rc;
	struct device_node *node = NULL;

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	node = info->dev->of_node;
	rc = battery_parse_jeita_config(node, info);
	if (rc)
		return rc;

	rc = battery_parse_config(node, info);
	if (rc)
		return rc;

	return 0;
}


/***************************************************
 Function: battery_prepare_properties
 Description: prepare the properties battey psy support
 Parameters:
    info                huawei_battery_info
 Return:
    0 mean prepare properties successful
    negative mean prepare fail
***************************************************/
static int battery_prepare_properties(struct huawei_battery_info *info)
{
	int bk_batt_props = 0;
	int batt_ext_props = ARRAY_SIZE(battery_ext_props);

	if (!info) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	info->num_props = bk_batt_props + batt_ext_props;

	pr_info("%s: info->num_props=%d\n", __func__, info->num_props);
	info->batt_props = devm_kzalloc(info->dev,
		info->num_props * sizeof(enum power_supply_property), GFP_KERNEL);
	if (!info->batt_props) {
		dev_err(info->dev, "alloc memory for battery properties fail\n");
		return -ENOMEM;
	}

	memcpy(info->batt_props + bk_batt_props, battery_ext_props,
		sizeof(enum power_supply_property) * batt_ext_props);

	return 0;
}
static  struct power_supply_desc battery_psy_desc = {
	.name = CHARGE_PSY_NAME,
	.type = POWER_SUPPLY_TYPE_UNKNOWN,
	.get_property = battery_get_property,
	.set_property = battery_set_property,
	.external_power_changed =  NULL,
	.property_is_writeable = battery_property_is_writeable,
};

static int hw_init_batt_psy(struct huawei_battery_info *info)
{
	struct power_supply_config batt_cfg = {};
	int rc = 0;

	batt_cfg.drv_data = info;
	batt_cfg.of_node = info->dev->of_node;
	battery_psy_desc.num_properties = info->num_props;
	battery_psy_desc.properties = info->batt_props;
	info->batt_psy = power_supply_register(info->dev,
		(const struct power_supply_desc*) (&battery_psy_desc), &batt_cfg);

	if (IS_ERR(info->batt_psy)) {
		pr_err("Couldn't register battery power supply\n");
		return PTR_ERR(info->batt_psy);
	}

	return rc;
}

/***************************************************
 Function: huawei_battery_probe
 Description: battery driver probe
 Parameters:
    pdev            the device related with battery
 Return:
    0 mean drvier probe successful
    negative mean probe fail
***************************************************/
static int huawei_battery_probe(struct platform_device *pdev)
{
	int ret;
	struct huawei_battery_info *info = NULL;
	struct device_node *node = NULL;
	const char *psy_name = NULL;

	if (!pdev) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}
	node = pdev->dev.of_node;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "Unable to alloc memory!\n");
		return -ENOMEM;
	}
	info->dev = &pdev->dev;
	platform_set_drvdata(pdev, info);

	info->disable_huawei_func = of_property_read_bool(node,
		"huawei,disable-huawei-func");

	ret = of_property_read_string(node, "huawei,bms-psy-name", &psy_name);

	if (ret)
		psy_name = "bms";
	info->bms_psy = power_supply_get_by_name(psy_name);

	if (!info->bms_psy) {
		dev_err(info->dev, "Cannot get bms psy %s\n", psy_name);
		ret = -EPROBE_DEFER;
	}

	ret = of_property_read_string(node, "huawei,bk_battery-psy-name", &psy_name);
	if (ret)
		psy_name = "bk_battery";
	info->bk_batt_psy = power_supply_get_by_name(psy_name);
	if (!info->bk_batt_psy) {
		dev_err(info->dev, "Cannot get bk_batery psy %s\n", psy_name);
		ret = -EPROBE_DEFER;
	}

	ret = of_property_read_string(node, "huawei,usb-psy-name", &psy_name);
	if (ret)
		psy_name = "usb";
	info->usb_psy = power_supply_get_by_name(psy_name);
	if (!info->usb_psy) {
		dev_err(info->dev, "Cannot get usb psy %s\n", psy_name);
		ret = -EPROBE_DEFER;
	}

	ret = of_property_read_u32(node, "buck_need_sc_sw_on", &info->buck_need_sc_sw_on);
	if (ret)
		info->buck_need_sc_sw_on = 0;

	/* fast charge current votable */
	power_vote_create_object(FCC_VOTE_OBJECT, POWER_VOTE_SET_MIN, set_fastchg_current_cb, info);

	/* usb input current limit votable */
	power_vote_create_object(USB_ICL_VOTE_OBJECT, POWER_VOTE_SET_MIN, set_usb_current_limit_cb, info);

	/* battery voltage votable */
	power_vote_create_object(VBAT_VOTE_OBJECT, POWER_VOTE_SET_MIN, set_vbat_cb, info);

	/* battery charging enable votable */
	power_vote_create_object(BCH_VOTE_OBJECT, POWER_VOTE_SET_ANY, charging_suspend_cb, info);

	ret = battery_parse_dt(info);
	if (ret) {
		dev_err(&pdev->dev, "Cannot parse the dts, pleae check\n");
		goto PARSE_DT_FAIL;
	}

	ret = battery_initial_status(info);
	if (ret) {
		dev_err(&pdev->dev, "Init battery status fail, please check\n");
		goto INIT_STATUS_FAIL;
	}

	INIT_DELAYED_WORK(&info->monitor_charging_work, battery_monitor_charging_work);
	if (!info->disable_huawei_func) {
		INIT_DELAYED_WORK(&info->ibus_detect_work, battery_detect_ibus_work);
		INIT_DELAYED_WORK(&info->jeita_work, battery_jeita_work);
	}
	INIT_DELAYED_WORK(&info->sync_voltage_work, battery_sync_voltage_work);
	INIT_WORK(&info->charger_action_work, charger_action_work);
	INIT_WORK(&info->charger_plug_work, charger_plug_work);

	/* try to register psy */
	ret = battery_prepare_properties(info);
	if (ret < 0) {
		dev_err(&pdev->dev, "battery prepare properties error ret = %d\n", ret);
		goto PREP_PROP_FAIL;
	}

	ret = hw_init_batt_psy(info);
	if (ret < 0) {
		pr_err("Couldn't register battery power supply\n");
		goto PSY_REG_FAIL;
	}

	info->chg_ws = power_wakeup_source_register(info->dev, "huawei-chg");
	if (!info->chg_ws) {
		pr_err("Couldn't register chg_ws wakelock\n");
		goto PSY_REG_FAIL;
	}

	g_info = info;
	pr_info("%s ok\n", __func__);

	info->chr_type = charge_get_charger_type();
	if (info->chr_type == CHARGER_TYPE_STANDARD)
		schedule_delayed_work(&info->monitor_charging_work, 0);

	info->enable_hv_charging = 1;

	return 0;

PSY_REG_FAIL:
	if (info->batt_props)
		devm_kfree(info->dev, info->batt_props);
	info->batt_props = NULL;
	info->num_props = 0;
	if (info->batt_psy)
		power_supply_unregister(info->batt_psy);
PREP_PROP_FAIL:
INIT_STATUS_FAIL:
PARSE_DT_FAIL:
	devm_kfree(&pdev->dev, info);
	info = NULL;
	return ret;
}

/***************************************************
 Function: huawei_battery_remove
 Description: battery driver remove
 Parameters:
    pdev            the device related with battery
 Return:
    0 mean driver remove successful
    negative mean remove fail
***************************************************/
static int huawei_battery_remove(struct platform_device *pdev)
{
	struct huawei_battery_info *info = NULL;

	if (!pdev) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	info = dev_get_drvdata(&pdev->dev);

	cancel_delayed_work_sync(&info->ibus_detect_work);
	cancel_delayed_work_sync(&info->monitor_charging_work);
	power_supply_unregister(info->batt_psy);
	power_vote_destroy_object(FCC_VOTE_OBJECT);
	power_vote_destroy_object(USB_ICL_VOTE_OBJECT);
	power_vote_destroy_object(VBAT_VOTE_OBJECT);
	power_vote_destroy_object(BCH_VOTE_OBJECT);
	devm_kfree(&pdev->dev, info->batt_props);
	devm_kfree(&pdev->dev, info);
	g_info = NULL;
	return 0;
}

/***************************************************
 Function: huawei_battery_suspend
 Description: battery driver suspend
 Parameters:
    pdev            the device related with battery
 Return:
    0 mean driver suspend successful
    negative mean suspend fail
***************************************************/
static int huawei_battery_suspend(struct device *dev)
{
	struct huawei_battery_info *info = NULL;

	if (!dev) {
		pr_err("%s: Invalid param, fatal error\n", __func__);
		return -EINVAL;
	}

	info = dev_get_drvdata(dev);
	if (info->disable_huawei_func)
		return 0;

	cancel_delayed_work_sync(&info->ibus_detect_work);
	cancel_delayed_work_sync(&info->monitor_charging_work);
	if (info->jeita_use_interrupt)
		flush_delayed_work(&info->jeita_work);
	return 0;
}

/***************************************************
 Function: huawei_battery_resume
 Description: battery driver resume
 Parameters:
    pdev            the device related with battery
 Return:
    0 mean driver resume successful
***************************************************/
static int huawei_battery_resume(struct device *dev)
{
	unsigned int chg_type = CHARGER_REMOVED;
	struct huawei_battery_info *info = NULL;

	if (!dev) {
		pr_err("Invalid param for %s\n", __func__);
		return 0;
	}

	info = dev_get_drvdata(dev);
	if (info->disable_huawei_func)
		return 0;

	chg_type = charge_get_charger_type();
	if ((chg_type == CHARGER_TYPE_STANDARD) ||
		(chg_type == CHARGER_TYPE_FCP))
		schedule_delayed_work(&info->monitor_charging_work,
			msecs_to_jiffies(MONITOR_CHARGING_DELAY_TIME));
	return 0;
}

static const struct dev_pm_ops huawei_battery_pm_ops = {
	.suspend = huawei_battery_suspend,
	.resume = huawei_battery_resume,
};

static struct of_device_id battery_match_table[] = {
	{
		.compatible = "huawei,battery",
		.data = NULL,
	},
	{
	},
};

static struct platform_driver huawei_battery_driver = {
	.probe = huawei_battery_probe,
	.remove = huawei_battery_remove,
	.driver = {
		.name = "huawei,battery",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(battery_match_table),
		.pm = &huawei_battery_pm_ops,
	},
};

static int __init huawei_battery_init(void)
{
	return platform_driver_register(&huawei_battery_driver);
}
static void __exit huawei_battery_exit(void)
{
	platform_driver_unregister(&huawei_battery_driver);
}

late_initcall(huawei_battery_init);
module_exit(huawei_battery_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei battery module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
