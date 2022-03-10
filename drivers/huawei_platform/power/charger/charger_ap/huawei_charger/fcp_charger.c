#include <linux/power/charger-manager.h>
#include <chipset_common/hwpower/common_module/power_cmdline.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_icon.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_supply_application.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <chipset_common/hwpower/protocol/adapter_protocol.h>
#include <huawei_platform/hwpower/common_module/power_platform.h>
#include <huawei_platform/power/direct_charger/direct_charger.h>
#include <huawei_platform/power/huawei_charger_adaptor.h>
#include <huawei_platform/usb/hw_pd_dev.h>
#include <chipset_common/hwpower/adapter/adapter_detect.h>
#include <chipset_common/hwpower/hvdcp_charge/hvdcp_charge.h>

#define HWLOG_TAG fcp_charger
HWLOG_REGIST();

static void fcp_start_detect_no_hv_mode(void)
{
	int ret;

	ret = hvdcp_detect_adapter();
	if (ret == 0)
		wired_connect_send_icon_uevent(ICON_TYPE_QUICK);
}

static int fcp_start_charging(struct huawei_battery_info *info)
{
	int ret;

	hvdcp_set_charging_stage(HVDCP_STAGE_SUPPORT_DETECT);

	/* check whether support fcp detect */
	if (adapter_get_protocol_register_state(ADAPTER_PROTOCOL_FCP)) {
		hwlog_err("not support fcp\n");
		return -1;
	}

	/* detect fcp adapter */
	hvdcp_set_charging_stage(HVDCP_STAGE_ADAPTER_DETECT);
	ret = hvdcp_detect_adapter();
	if (ret)
		return ret;

	adapter_test_set_result(AT_TYPE_FCP, AT_DETECT_SUCC);
	hvdcp_set_charging_stage(HVDCP_STAGE_ADAPTER_ENABLE);

	ret = hvdcp_set_adapter_voltage(ADAPTER_9V * POWER_MV_PER_V);
	if (ret) {
		(void)hvdcp_reset_master();
		return 1;
	}

	hvdcp_set_vboost_retry_count(0);

	if (charge_get_monitor_work_flag() == CHARGE_MONITOR_WORK_NEED_STOP) {
		hvdcp_set_charging_stage(HVDCP_STAGE_DEFAUTL);
		return 0;
	}
	adapter_test_set_result(AT_TYPE_FCP, AT_PROTOCOL_FINISH_SUCC);
#ifdef CONFIG_DIRECT_CHARGER
	wired_connect_send_icon_uevent(ICON_TYPE_QUICK);
#endif
	power_icon_notify(ICON_TYPE_QUICK);
	info->chr_type = CHARGER_TYPE_FCP;
	hvdcp_set_charging_stage(HVDCP_STAGE_SUCCESS);
	hvdcp_set_charging_flag(true);

	charge_set_charger_type(CHARGER_TYPE_FCP);

	msleep(CHIP_RESP_TIME);
	hwlog_info("fcp charging start success!\n");
	return 0;
}

void force_stop_fcp_charging(struct huawei_battery_info *info)
{
	if (hvdcp_get_charging_stage() != HVDCP_STAGE_SUCCESS)
		return;
	if (info->enable_hv_charging && !charge_get_reset_adapter_source())
		return;

	if (!hvdcp_exit_charging())
		return;

	hwlog_info("reset adapter by user\n");
	hvdcp_set_charging_stage(HVDCP_STAGE_RESET_ADAPTER);

	info->chr_type = CHARGER_TYPE_STANDARD;

	/* Get chg type det power supply */
	(void)power_supply_set_int_property_value("charger",
		POWER_SUPPLY_PROP_CHARGE_TYPE, info->chr_type);

	charge_set_charger_type(CHARGER_TYPE_STANDARD);

	msleep(CHIP_RESP_TIME);
}

void fcp_charge_check(struct huawei_battery_info *info)
{
	int ret;
	int i;
	bool cc_vbus_short = false;
	bool adapter_enable = false;
	bool adapter_detect = false;

	hwlog_err("in fcp_charge_check, hv = %d, stage = %u, success = %d\n",
		info->enable_hv_charging, hvdcp_get_charging_stage(), HVDCP_STAGE_SUCCESS);

	if (charge_get_monitor_work_flag() == CHARGE_MONITOR_WORK_NEED_STOP)
		return;

	if (cc_vbus_short) {
		hwlog_err("cc match rp3.0, can not do fcp charge\n");
		return;
	}

	if (hvdcp_get_charging_stage() == HVDCP_STAGE_SUCCESS)
		hvdcp_check_master_status();

	if (!hvdcp_check_charger_type(info->chr_type))
		return;

	if (!dc_get_adapter_antifake_result()) {
		hwlog_info("[%s] adapter is fake\n", __func__);
		return;
	}

	if ((info->enable_hv_charging == false) && (get_first_insert() == 1)) {
		hwlog_info("fcp first insert direct check\n");
		fcp_start_detect_no_hv_mode();
		set_first_insert(0);
		return;
	}

	if (hvdcp_check_running_current((int)hvdcp_get_rt_current_thld()))
		hvdcp_set_rt_result(true);

	if (direct_charge_is_failed() &&
		(hvdcp_get_charging_stage() < HVDCP_STAGE_SUCCESS))
		hvdcp_set_charging_stage(HVDCP_STAGE_DEFAUTL);
	if (info->enable_hv_charging &&
		(((hvdcp_get_charging_stage() == HVDCP_STAGE_DEFAUTL) &&
		!(charge_get_reset_adapter_source() & (1 << RESET_ADAPTER_SOURCE_WLTX))) ||
		((hvdcp_get_charging_stage() == HVDCP_STAGE_RESET_ADAPTER) &&
		(charge_get_reset_adapter_source() == 0)))) {
		ret = fcp_start_charging(info);
		for (i = 0; i < 3 && ret == 1; i++) {
			/* reset adapter and try again */
			if (hvdcp_reset_operate(HVDCP_RESET_ADAPTER) < 0)
				break;
			ret = fcp_start_charging(info);
		}
		if (ret == 1) {
			/* reset fsa9688 chip and try again */
			if (hvdcp_reset_operate(HVDCP_RESET_MASTER) == 0)
				ret = fcp_start_charging(info);
		}
		if ((ret == 1) && hvdcp_check_adapter_retry_count()) {
			hvdcp_set_charging_stage(HVDCP_STAGE_DEFAUTL);
			return;
		}

		if (ret == 1) {
			adapter_enable = hvdcp_check_adapter_enable_count();
			adapter_detect = hvdcp_check_adapter_detect_count();
			if (adapter_enable && adapter_detect)
				hvdcp_set_charging_stage(HVDCP_STAGE_DEFAUTL);
		}

		hwlog_info("[%s]fcp stage  %s !!! \n", __func__,
			hvdcp_get_charging_stage_string(hvdcp_get_charging_stage()));
	}

	force_stop_fcp_charging(info);
}
