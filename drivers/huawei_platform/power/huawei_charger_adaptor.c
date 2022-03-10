/*
 * huawei_charger_adaaptor.c
 *
 * huawei charger adaaptor for power module
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

#include <huawei_platform/power/huawei_charger_adaptor.h>
#include <linux/kernel.h>
#include <linux/power/huawei_charger.h>
#include <linux/power/huawei_battery.h>
#include <linux/power/charger-manager.h>
#include <linux/notifier.h>
#include <huawei_platform/log/hw_log.h>
#include <chipset_common/hwpower/charger/charger_common_interface.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_icon.h>
#include <chipset_common/hwpower/common_module/power_ui_ne.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_supply_application.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <chipset_common/hwpower/common_module/power_delay.h>
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
#include <huawei_platform/hwpower/common_module/power_glink.h>
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */
#include <chipset_common/hwpower/common_module/power_delay.h>
#include <chipset_common/hwpower/direct_charge/direct_charge_adapter.h>
#include <chipset_common/hwpower/hardware_channel/vbus_channel.h>

#define HWLOG_TAG huawei_charger_adaptor
HWLOG_REGIST();

#define DEFAULT_CAP              50

void charge_set_adapter_voltage(int val, unsigned int type, unsigned int delay_time)
{
}

int charge_otg_mode_enable(int enable, unsigned int user)
{
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
	if (user == VBUS_CH_USER_WR_TX)
		return power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_WLSBST,
			(u32 *)&enable, GLINK_DATA_ONE);
#endif
	return 0;
}

void charge_send_uevent(int input_events)
{
}

void charge_request_charge_monitor(void)
{
	if (charge_get_monitor_work_flag() == CHARGE_MONITOR_WORK_NEED_STOP)
		return;

	if (charge_get_charger_type() == CHARGER_REMOVED) {
		hwlog_info("charger already plugged out\n");
		return;
	}

	if (charge_get_charger_type() == CHARGER_TYPE_STANDARD)
		power_supply_set_int_property_value("huawei_charge",
			POWER_SUPPLY_PROP_CHG_TYPE, CHARGER_TYPE_STANDARD);
}

int charger_manager_notifier(struct charger_manager *info, int event)
{
	return 0;
}

void wired_connect_send_icon_uevent(int icon_type)
{
	hwlog_info("%s enter,icon_type=%d\n", __func__, icon_type);

	power_icon_notify(icon_type);
}

void wired_disconnect_send_icon_uevent(void)
{
	power_icon_notify(ICON_TYPE_INVALID);
}

int charge_enable_force_sleep(bool enable)
{
	return 0;
}

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
int charge_set_hiz_enable(int hz_enable)
{
	int ret;
	u32 value;
	u32 id = POWER_GLINK_PROP_ID_SET_INPUT_SUSPEND;

	if (hz_enable)
		value = 1; /* Hiz enable */
	else
		value = 0;

	ret = power_glink_set_property_value(id, &value, GLINK_DATA_ONE);
	msleep(DT_MSLEEP_200MS);
	return ret;
}
#else
int charge_set_hiz_enable(int hz_enable)
{
	int rc;
	struct charge_device_info *di = NULL;

	di = get_charger_device_info();
	if (!di) {
		hwlog_err("%s g_di is null\n", __func__);
		return 0;
	}

	if (!power_supply_check_psy_available("battery", &di->batt_psy)) {
		hwlog_err("charge_device is not ready! cannot set runningtest\n");
		return -EINVAL;
	}

	rc = power_supply_set_int_property_value_with_psy(di->batt_psy,
		POWER_SUPPLY_PROP_HIZ_MODE, hz_enable);
	if (rc < 0)
		hwlog_err("%s enable hz fail\n", __func__);
	msleep(DT_MSLEEP_250MS);
	return rc;
}
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

int adaptor_cfg_for_wltx_vbus(int vol, int cur)
{
	hwlog_info("adaptor_cfg for wireless_tx: vol=%d, cur=%d\n", vol, cur);
	return dc_set_adapter_output_capability(vol, cur, 0);
}

int charge_get_done_type(void)
{
	int rc;
	union power_supply_propval val = {0, };
	struct charge_device_info *di = NULL;

	di = get_charger_device_info();
	if (!di) {
		hwlog_err("%s g_di is null\n", __func__);
		return CHARGE_DONE_NON;
	}

	rc = power_supply_get_property_value_with_psy(di->batt_psy,
		POWER_SUPPLY_PROP_CHARGE_DONE, &val);
	if (rc < 0) {
		hwlog_err("%s charge done fail\n", __func__);
		return CHARGE_DONE_NON;
	}
	return val.intval;
}

static int first_check;
int get_first_insert(void)
{
	return first_check;
}

void set_first_insert(int flag)
{
	pr_info("set insert flag %d\n", flag);
	first_check = flag;
}

static int charge_get_vwls(void)
{
	u32 vwls = 0;

#if IS_ENABLED(CONFIG_HUAWEI_POWER_GLINK)
	(void)power_glink_get_property_value(POWER_GLINK_PROP_ID_SET_WLSBST, &vwls, GLINK_DATA_ONE);
#endif
	return vwls;
}

int charge_get_vbus(void)
{
	int vbus = power_supply_app_get_usb_voltage_now();
	int vwls = charge_get_vwls();

	return vbus >= vwls ? vbus : vwls;
}

unsigned int charge_get_charging_state(void)
{
	return CHAGRE_STATE_NORMAL;
}

int charger_dev_get_ibus(u32 *ibus)
{
	int rc;
	union power_supply_propval val = {0, };
	struct charge_device_info *di = NULL;

	di = get_charger_device_info();
	if (!di) {
		hwlog_err("%s g_di is null\n", __func__);
		return 0;
	}

	rc = power_supply_get_property_value_with_psy(di->usb_psy,
		POWER_SUPPLY_PROP_INPUT_CURRENT_NOW, &val);
	if (rc < 0) {
		hwlog_err("%s get vbus vol fail\n", __func__);
		return -1;
	}
	*ibus = val.intval;
	return 0;
}

signed int battery_get_bat_current(void)
{
	int rc;
	union power_supply_propval val = {0, };
	struct charge_device_info *di = NULL;

	di = get_charger_device_info();
	if (!di) {
		hwlog_err("%s g_di is null\n", __func__);
		return 0;
	}

	rc = power_supply_get_property_value_with_psy(di->batt_psy,
		POWER_SUPPLY_PROP_CURRENT_NOW, &val);
	if (rc < 0) {
		hwlog_err("%s get vbus vol fail\n", __func__);
		return 0;
	}
	/* -val.intval is a negative number indicating charging */
	return -val.intval;
}

int charge_get_ibus(void)
{
	u32 ibus_curr = 0;

	(void)charger_dev_get_ibus(&ibus_curr);
	return ibus_curr / POWER_UA_PER_MA;
}

int charge_get_vusb(void)
{
	return -EINVAL;
}

int charge_set_mivr(u32 volt)
{
	return -EINVAL;
}

int charge_set_vbus_vset(u32 volt)
{
	return -EINVAL;
}

int charge_enable_eoc(bool eoc_enable)
{
	return 0;
}

void reset_cur_delay_eoc_count(void)
{
}

void charger_detect_init(void)
{
}

void charger_detect_release(void)
{
}
