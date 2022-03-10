/*
 * wireless_charge_psy.c
 *
 * wireless charge driver, function as power supply
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

#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_icon.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_ui_ne.h>
#include <chipset_common/hwpower/wireless_charge/wireless_rx_status.h>
#include <huawei_platform/hwpower/common_module/power_platform.h>
#include <huawei_platform/power/huawei_charger_adaptor.h>
#include <huawei_platform/power/huawei_charger_common.h>
#include <huawei_platform/power/wireless/wireless_charger.h>

#define HWLOG_TAG wireless_rx_psy
HWLOG_REGIST();

static int wlrx_psy_chg_type_changed(bool online)
{
	unsigned int chg_type;

	chg_type = charge_get_charger_type();
	if (!online && (chg_type != CHARGER_TYPE_WIRELESS)) {
		hwlog_err("chg_type_changed: charger_type=%d\n", chg_type);
		return 0;
	}

	if (online)
		charge_set_charger_type(CHARGER_TYPE_WIRELESS);
	else if (wlrx_get_wired_channel_state() != WIRED_CHANNEL_ON)
		charge_set_charger_type(CHARGER_REMOVED);

	return 0;
}

void wlrx_handle_sink_event(bool sink_flag)
{
	if (sink_flag) {
		power_event_bnc_notify(POWER_BNT_CONNECT, POWER_NE_WIRELESS_CONNECT, NULL);
		power_event_bnc_notify(POWER_BNT_CHARGING, POWER_NE_CHARGING_START, NULL);
	} else {
		power_event_bnc_notify(POWER_BNT_CONNECT, POWER_NE_WIRELESS_DISCONNECT, NULL);
		if (wlrx_get_wired_channel_state() != WIRED_CHANNEL_ON) {
			power_icon_notify(ICON_TYPE_INVALID);
			power_event_bnc_notify(POWER_BNT_CHARGING, POWER_NE_CHARGING_STOP, NULL);
		}
	}

	if (wlrx_psy_chg_type_changed(sink_flag) < 0)
		hwlog_err("%s: report psy chg_type failed\n", __func__);
}

static int wlrx_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	unsigned int chg_type;

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 0;
		chg_type = charge_get_charger_type();
		if (chg_type == CHARGER_TYPE_WIRELESS)
			val->intval = 1;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static enum power_supply_property g_wlrx_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static const struct power_supply_desc g_wlrx_desc = {
	.name = "Wireless",
	.type = POWER_SUPPLY_TYPE_WIRELESS,
	.properties = g_wlrx_props,
	.num_properties = ARRAY_SIZE(g_wlrx_props),
	.get_property = wlrx_get_property,
};

int wlrx_power_supply_register(struct platform_device *pdev)
{
	struct power_supply *psy = NULL;

	psy = power_supply_register(&pdev->dev, &g_wlrx_desc, NULL);
	if (IS_ERR(psy)) {
		hwlog_err("power_supply_register failed\n");
		return PTR_ERR(psy);
	}

	return 0;
}
