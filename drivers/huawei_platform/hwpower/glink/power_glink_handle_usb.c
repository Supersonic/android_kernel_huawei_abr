// SPDX-License-Identifier: GPL-2.0
/*
 * power_glink_handle_usb.c
 *
 * glink channel for handle usb
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

#include <linux/device.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_ui_ne.h>
#include <chipset_common/hwpower/common_module/power_icon.h>
#include <chipset_common/hwusb/hw_usb.h>
#include <huawei_platform/hwpower/common_module/power_glink.h>
#include <huawei_platform/usb/hw_pd_dev.h>

#define HWLOG_TAG power_glink_handle_usb
HWLOG_REGIST();

static void power_glink_handle_usb_connect_message(u32 msg)
{
	u32 data;

	switch (msg) {
	case POWER_GLINK_NOTIFY_VAL_USB_DISCONNECT:
		data = 0;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_CHARGER, &data);
		break;
	case POWER_GLINK_NOTIFY_VAL_USB_CONNECT:
		break;
	default:
		break;
	}
}

static void power_glink_handle_usb_dc_connect_message(u32 msg)
{
	u32 data;
	int typec_state = PD_DPM_USB_TYPEC_DEVICE_ATTACHED;

	switch (msg) {
	case POWER_GLINK_NOTIFY_VAL_STOP_CHARGING:
		data = 0;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_CHARGER, &data);
		pd_dpm_set_emark_current(PD_DPM_CURR_3A);
		pd_dpm_set_is_direct_charge_cable(1);
		typec_state = PD_DPM_USB_TYPEC_NONE;
		pd_dpm_set_typec_state(typec_state);
		break;
	case POWER_GLINK_NOTIFY_VAL_START_CHARGING:
		pd_dpm_set_typec_state(typec_state);
		break;
	default:
		break;
	}
}

static void power_glink_handle_usb_typec_message(u32 msg)
{
	u32 cc_orientation;
	struct otg_ocp_para *para = NULL;
	int typec_state = PD_DPM_USB_TYPEC_DEVICE_ATTACHED;

	switch (msg) {
	case POWER_GLINK_NOTIFY_VAL_ORIENTATION_CC1:
		cc_orientation = 0;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_ORIENTATION_CC,
			&cc_orientation);
		break;
	case POWER_GLINK_NOTIFY_VAL_ORIENTATION_CC2:
		cc_orientation = 1;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_ORIENTATION_CC,
			&cc_orientation);
		break;
	case POWER_GLINK_NOTIFY_VAL_DEBUG_ACCESSORY:
		pd_dpm_set_is_direct_charge_cable(0);
		break;
	case POWER_GLINK_NOTIFY_VAL_UFP:
	case POWER_GLINK_NOTIFY_VAL_POWERCABLE_NOUFP:
	case POWER_GLINK_NOTIFY_VAL_POWERCABLE_UFP:
		para = pd_dpm_get_otg_ocp_check_para();
		if (para)
			power_event_bnc_notify(POWER_BNT_OTG, POWER_NE_OTG_OCP_CHECK_START, para);
		hwlog_info("otg insert\n");
		break;
	case POWER_GLINK_NOTIFY_VAL_TYPEC_NONE:
		para = pd_dpm_get_otg_ocp_check_para();
		if (para)
			power_event_bnc_notify(POWER_BNT_OTG, POWER_NE_OTG_OCP_CHECK_STOP, NULL);
		hwlog_info("otg remove\n");
		typec_state = PD_DPM_USB_TYPEC_NONE;
		/* fall through */
	case POWER_GLINK_NOTIFY_VAL_TYPEC_DFP:
		pd_dpm_set_is_direct_charge_cable(1);
		break;
	case POWER_GLINK_NOTIFY_VAL_TYPEC_SHORT:
		pd_dpm_cc_dynamic_protect();
		break;
	default:
		break;
	}
	pd_dpm_set_typec_state(typec_state);
}

static void power_glink_handle_usb_pd_message(u32 msg)
{
	u32 data;
	u32 vconn_enable;
	u32 vbus_enable;

	switch (msg) {
	case POWER_GLINK_NOTIFY_VAL_PD_CHARGER:
		data = 1;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_CHARGER, &data);
		break;
	case POWER_GLINK_NOTIFY_VAL_PD_VCONN_ENABLE:
		vconn_enable = 1;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_SOURCE_VCONN, &vconn_enable);
		break;
	case POWER_GLINK_NOTIFY_VAL_PD_VCONN_DISABLE:
		vconn_enable = 0;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_SOURCE_VCONN, &vconn_enable);
		break;
	case POWER_GLINK_NOTIFY_VAL_PD_VBUS_ENABLE:
		vbus_enable = 1;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_SOURCE_VBUS, &vbus_enable);
		break;
	case POWER_GLINK_NOTIFY_VAL_PD_VBUS_DISABLE:
		vbus_enable = 0;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_SOURCE_VBUS, &vbus_enable);
		break;
	case POWER_GLINK_NOTIFY_VAL_PD_QUICK_CHARGE:
		if (power_icon_inquire() == ICON_TYPE_SUPER)
			break;

		data = 1;
		power_event_bnc_notify(POWER_BNT_HW_PD, POWER_NE_HW_PD_QUCIK_CHARGE, &data);
		break;
	default:
		break;
	}
}

void power_glink_handle_usb_notify_message(u32 id, u32 msg)
{
	switch (id) {
	case POWER_GLINK_NOTIFY_ID_USB_CONNECT_EVENT:
		power_glink_handle_usb_connect_message(msg);
		break;
	case POWER_GLINK_NOTIFY_ID_DC_CONNECT_EVENT:
		power_glink_handle_usb_dc_connect_message(msg);
		break;
	case POWER_GLINK_NOTIFY_ID_TYPEC_EVENT:
		power_glink_handle_usb_typec_message(msg);
		break;
	case POWER_GLINK_NOTIFY_ID_PD_EVENT:
		power_glink_handle_usb_pd_message(msg);
		break;
	case POWER_GLINK_NOTIFY_ID_EMARK_EVENT:
		pd_dpm_set_emark_current(msg);
		break;
	default:
		break;
	}
}
