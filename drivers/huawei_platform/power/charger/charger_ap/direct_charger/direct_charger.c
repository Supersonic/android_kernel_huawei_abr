/*
 * direct_charger.c
 *
 * direct charger driver
 *
 * Copyright (c) 2012-2020 Huawei Technologies Co., Ltd.
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

#include <linux/power/huawei_charger.h>
#include <linux/random.h>
#include <chipset_common/hwpower/battery/battery_temp.h>
#include <chipset_common/hwpower/adapter/adapter_detect.h>
#include <chipset_common/hwpower/common_module/power_cmdline.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_delay.h>
#include <chipset_common/hwpower/common_module/power_dsm.h>
#include <chipset_common/hwpower/common_module/power_time.h>
#include <chipset_common/hwpower/common_module/power_wakeup.h>
#include <chipset_common/hwpower/common_module/power_supply_application.h>
#include <chipset_common/hwpower/hardware_ic/charge_pump.h>
#include <chipset_common/hwpower/protocol/adapter_protocol.h>
#include <huawei_platform/power/battery_voltage.h>
#include <huawei_platform/power/direct_charger/direct_charger.h>
#include <chipset_common/hwpower/wireless_charge/wireless_tx_pwr_ctrl.h>
#include <huawei_platform/usb/hw_pd_dev.h>
#include <chipset_common/hwpower/battery/battery_parallel.h>

#define HWLOG_TAG direct_charge
HWLOG_REGIST();

/* for ops */
static struct direct_charge_device *g_di;

static unsigned int g_dc_mode;
static bool g_is_abnormal_scp_adp;

#define CC_UNSAFE_MAX_CURRENT    3000

static const char * const g_dc_stage[DC_STAGE_END] = {
	[DC_STAGE_DEFAULT] = "dc_stage_default",
	[DC_STAGE_ADAPTER_DETECT] = "dc_stage_adapter_detect",
	[DC_STAGE_SWITCH_DETECT] = "dc_stage_switch_detect",
	[DC_STAGE_CHARGE_INIT] = "dc_stage_charge_init",
	[DC_STAGE_SECURITY_CHECK] = "dc_stage_security_check",
	[DC_STAGE_SUCCESS] = "dc_stage_success",
	[DC_STAGE_CHARGING] = "dc_stage_charging",
	[DC_STAGE_CHARGE_DONE] = "dc_stage_charge_done",
};

void direct_charge_set_di(struct direct_charge_device *di)
{
	if (di)
		g_di = di;
	else
		hwlog_err("di is null\n");
}

struct direct_charge_device *direct_charge_get_di(void)
{
	if (!g_di) {
		hwlog_err("g_di is null\n");
		return NULL;
	}

	return g_di;
}

struct direct_charge_device *direct_charge_get_di_by_mode(int mode)
{
	struct direct_charge_device *l_di = NULL;

	switch (mode) {
	case SC4_MODE:
		sc4_get_di(&l_di);
		return l_di;
	case SC_MODE:
		sc_get_di(&l_di);
		return l_di;
	case LVC_MODE:
		lvc_get_di(&l_di);
		return l_di;
	default:
		return NULL;
	}
}

void direct_charge_set_start_time(void)
{
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;
	u32 cur_time = power_get_current_kernel_time().tv_sec;

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	if (lvc_di)
		lvc_di->direct_charge_start_time = cur_time;

	if (sc_di)
		sc_di->direct_charge_start_time = cur_time;

	if (sc4_di)
		sc4_di->direct_charge_start_time = cur_time;
}

void direct_charge_set_abnormal_adp_flag(bool flag)
{
	hwlog_info("abnormal_adp_flag=%d\n", flag);
	g_is_abnormal_scp_adp = flag;
}

bool direct_charge_get_abnormal_adp_flag(void)
{
	return g_is_abnormal_scp_adp;
}

/* get the stage of direct charge */
unsigned int direct_charge_get_stage_status(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return DC_STAGE_DEFAULT;

	return l_di->dc_stage;
}

/* get the stage of direct charge */
const char *direct_charge_get_stage_status_string(unsigned int stage)
{
	if ((stage >= DC_STAGE_BEGIN) && (stage < DC_STAGE_END))
		return g_dc_stage[stage];

	return "illegal stage_status";
}

/* set the stage of direct charge */
void direct_charge_set_stage_status(unsigned int stage)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	l_di->dc_stage = stage;

	hwlog_info("set_stage_status: stage=%d\n", l_di->dc_stage);
}

void direct_charge_set_stage_status_default(void)
{
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	hwlog_info("set all dc mode to default\n");

	if (lvc_di)
		lvc_di->dc_stage = DC_STAGE_DEFAULT;

	if (sc_di)
		sc_di->dc_stage = DC_STAGE_DEFAULT;

	if (sc4_di)
		sc4_di->dc_stage = DC_STAGE_DEFAULT;
}

int direct_charge_in_charging_stage(void)
{
	if (direct_charge_get_stage_status() == DC_STAGE_CHARGING)
		return DC_IN_CHARGING_STAGE;

	return DC_NOT_IN_CHARGING_STAGE;
}

int direct_charge_get_working_mode(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return UNDEFINED_MODE;

	return l_di->working_mode;
}

void direct_charge_set_local_mode(int set, unsigned int dc_mode)
{
	if (set == AND_SET)
		g_dc_mode &= dc_mode;

	if (set == OR_SET)
		g_dc_mode |= dc_mode;
}

unsigned int direct_charge_get_local_mode(void)
{
	return g_dc_mode;
}

unsigned int direct_charge_update_local_mode(unsigned int mode)
{
	unsigned int local_mode = mode;
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);
	if (!lvc_di && !sc_di && !sc4_di) {
		hwlog_info("local not support direct_charge\n");
		return UNDEFINED_MODE;
	}
	hwlog_info("original local mode %u\n", mode);

	if (sc4_di) {
		if ((sc4_di->sysfs_enable_charger == 0) &&
			!(sc4_di->sysfs_mainsc_enable_charger ^ sc4_di->sysfs_auxsc_enable_charger)) {
			local_mode &= ~SC4_MODE;
			hwlog_info("disable sc4 mode, local_mode %u\n",
				local_mode);
		}
	}
	if (sc_di) {
		if ((sc_di->sysfs_enable_charger == 0) &&
			!(sc_di->sysfs_mainsc_enable_charger ^ sc_di->sysfs_auxsc_enable_charger)) {
			local_mode &= ~SC_MODE;
			hwlog_info("disable sc mode, local_mode %u\n",
				local_mode);
		}
	}
	if (lvc_di) {
		if (lvc_di->sysfs_enable_charger == 0) {
			local_mode &= ~LVC_MODE;
			hwlog_info("disable lvc mode, local_mode %u\n",
				local_mode);
		}
	}

	return local_mode;
}

int direct_charge_is_failed(void)
{
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	if (direct_charge_get_can_enter_status() == false) {
		hwlog_info("can not enter dc\n");
		return 1;
	}

	if (lvc_di) {
		if (lvc_di->error_cnt >= DC_ERR_CNT_MAX) {
			hwlog_info("lvc error exceed retry max\n");
			return 1;
		} else {
			return 0;
		}
	}

	if (sc_di) {
		if (sc_di->error_cnt >= DC_ERR_CNT_MAX) {
			hwlog_info("sc error exceed retry max\n");
			return 1;
		} else {
			return 0;
		}
	}

	if (sc4_di) {
		if (sc4_di->error_cnt >= DC_ERR_CNT_MAX) {
			hwlog_info("sc4 error exceed retry max\n");
			return 1;
		} else {
			return 0;
		}
	}

	return 0;
}

void direct_charge_set_disable_flags(int val, int type)
{
	lvc_set_disable_flags(val, type);
	sc_set_disable_flags(val, type);
	sc4_set_disable_flags(val, type);
}

int direct_charge_get_info(enum direct_charge_info_type type, int *value)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di || !value)
		return -1;

	switch (type) {
	case CC_CABLE_DETECT_OK:
		if (l_di->cc_cable_detect_enable &&
			l_di->cc_cable_detect_ok == 0)
			*value = 0;
		else
			*value = 1;
		return 0;
	default:
		return -1;
	}
}

/* get the maximum current allowed by direct charging at specified temp */
int direct_charge_battery_temp_handler(int value)
{
	int i;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return 0;

	for (i = 0; i < DC_TEMP_LEVEL; ++i) {
		if (value >= l_di->temp_para[i].temp_min &&
			value < l_di->temp_para[i].temp_max)
			return l_di->temp_para[i].temp_cur_max;
	}

	hwlog_err("current temp is illegal, temp=%d\n", value);
	return 0;
}

/* get the maximum current with specified battery */
int direct_charge_get_battery_max_current(void)
{
	int i;
	int max_cur;
	const char *brand = NULL;
	int temp = 25; /* normal temperature is 25c */
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	max_cur = l_di->orig_volt_para_p[0].volt_info[0].cur_th_high;
	brand = power_supply_app_get_bat_brand();

	for (i = 0; i < l_di->stage_group_size; i++) {
		if (!l_di->orig_volt_para_p[i].bat_info.parse_ok)
			continue;

		if (!strstr(brand, l_di->orig_volt_para_p[i].bat_info.batid))
			continue;

		if ((temp >= l_di->orig_volt_para_p[i].bat_info.temp_high) ||
			(temp < l_di->orig_volt_para_p[i].bat_info.temp_low))
			continue;

		max_cur = l_di->orig_volt_para_p[i].volt_info[0].cur_th_high;
		break;
	}

	hwlog_info("stage_group_size=%d, bat_brand=%s, bat_max_cur=%d\n",
		l_di->stage_group_size, brand, max_cur);

	return max_cur;
}

bool direct_charge_get_stop_charging_complete_flag(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return true;

	if (l_di->scp_stop_charging_complete_flag) {
		hwlog_info("scp_stop_charging_complete_flag is set\n");
		return true;
	}

	return false;
}

void direct_charge_set_stop_charging_flag(int value)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	l_di->stop_charging_flag_error = value;
}

int direct_charge_get_stop_charging_flag(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	if (l_di->stop_charging_flag_error) {
		hwlog_info("stop_charging_flag_error is set\n");
		return -1;
	}

	return 0;
}

int direct_charge_get_vbus(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return 0;

	return l_di->vadapt;
}

int direct_charge_get_ibus(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return 0;

	return l_di->iadapt;
}

static int direct_charge_get_bat_voltage(int *value)
{
	int btb_vol;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	/* last cc&cv use the higher vbat */
	if (l_di->use_higher_vbat && (l_di->cur_stage >= (DOUBLE_SIZE * l_di->stage_size - 2)))
		btb_vol = dcm_get_ic_max_vbtb_with_comp(l_di->working_mode,
			l_di->comp_para.vbat_comp);
	else
		btb_vol = dcm_get_ic_vbtb_with_comp(l_di->working_mode, l_di->cur_mode,
			l_di->comp_para.vbat_comp);

	if (btb_vol <= 0) {
		dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
			DC_EH_GET_VBAT, NULL);
		direct_charge_set_stop_charging_flag(1);
		*value = 0;

		hwlog_err("get battery voltage fail\n");
		return -1;
	}

	*value = btb_vol;
	return 0;
}

int direct_charge_get_bat_current(int *value)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	if (dcm_get_total_ibat(l_di->working_mode, l_di->cur_mode, value)) {
		dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
			DC_EH_GET_IBAT, NULL);
		direct_charge_set_stop_charging_flag(1);
		*value = 0;

		hwlog_err("get battery current fail\n");
		return -1;
	}

	return 0;
}

int direct_charge_get_bat_sys_voltage(int *value)
{
	/* default one battery */
	if (hw_battery_get_series_num() == 1)
		return direct_charge_get_bat_voltage(value);

	/* get total voltage for multi battery */
	*value = hw_battery_voltage(BAT_ID_ALL);
	return 0;
}

int direct_charge_get_device_vbus(int *value)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	if (dcm_get_ic_vbus(l_di->working_mode, l_di->cur_mode, value)) {
		dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
			DC_EH_GET_DEVICE_VBUS, NULL);
		direct_charge_set_stop_charging_flag(1);
		*value = 0;

		hwlog_err("get lvc or sc device vbus fail\n");
		return -1;
	}

	return 0;
}

int direct_charge_get_device_ibus(int *value)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	if (dcm_get_ic_ibus(l_di->working_mode, l_di->cur_mode, value)) {
		dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
			DC_EH_GET_DEVICE_IBUS, NULL);
		direct_charge_set_stop_charging_flag(1);
		*value = 0;

		hwlog_err("get lvc or sc device ibus fail\n");
		return -1;
	}

	return 0;
}

static int direct_charge_get_device_temp(int *value)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	if (dcm_get_ic_temp(l_di->working_mode, l_di->cur_mode, value)) {
		dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
			DC_EH_GET_DEVICE_TEMP, NULL);
		direct_charge_set_stop_charging_flag(1);
		*value = 0;

		hwlog_err("get lvc or sc device temp fail\n");
		return -1;
	}

	return 0;
}

int direct_charge_get_device_close_status(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return 1;

	if (dcm_is_ic_close(l_di->working_mode, l_di->cur_mode)) {
		dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
			DC_EH_DEVICE_IS_CLOSE, NULL);
		direct_charge_set_stop_charging_flag(1);
		hwlog_err("lvc or sc device is close\n");
		return 1;
	}

	return 0;
}

static void direct_charge_get_vbus_vbat(int *vbus, int *vbat)
{
	*vbus = power_supply_app_get_usb_voltage_now();
	*vbat = power_supply_app_get_bat_voltage_now();
	hwlog_info("vbus=%d, vbat=%d\n", *vbus, *vbat);
}

static int direct_charge_set_adapter_voltage_step_by_step(int end)
{
	int vol = STEP_VOL_START;
	int step = 3000; /* step in 3000mv */
	int cnt = 10; /* max times */
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	if (!dcm_is_ic_support_prepare(l_di->working_mode, l_di->cur_mode))
		return dc_set_adapter_voltage(end);

	if (dc_set_adapter_voltage(vol))
		return -1;

	if (dcm_prepare_enable_ic(l_di->working_mode, l_di->cur_mode))
		return -1;

	while (cnt--) {
		vol += step;
		hwlog_info("vol step open:%d\n", vol);

		if (vol < end) {
			if (dc_set_adapter_voltage(vol))
				return -1;
			usleep_range(5000, 5200); /* sleep 5ms */
		} else {
			return dc_set_adapter_voltage(end);
		}
	}

	return -1;
}

int direct_charge_get_adapter_gain_current(
	struct direct_charge_device *di)
{
	int adapter_type = dc_get_adapter_type();

	switch (adapter_type) {
	case ADAPTER_TYPE_10V2A:
		return di->gain_curr_10v2a;
	default:
		return 0;
	}
}

static int direct_charge_get_device_delta_err(struct direct_charge_device *di)
{
	int adapter_type = dc_get_adapter_type();

	switch (adapter_type) {
	case ADAPTER_TYPE_10V2P25A:
		return di->delta_err_10v2p25a;
	default:
		return di->delta_err;
	}
}

void direct_charge_force_disable_dc_path(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	dcm_enable_ic(l_di->working_mode, l_di->cur_mode, DC_IC_DISABLE);
	dc_close_aux_wired_channel();
}

int direct_charge_init_adapter_and_device(int flag)
{
	char tmp_buf[ERR_NO_STRING_SIZE] = { 0 };
	int ret;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	ret = dc_init_adapter(l_di->init_adapter_vset);
	if (ret) {
		snprintf(tmp_buf, sizeof(tmp_buf), "adapter data init fail\n");
		goto fail_init;
	}

	if (dcm_init_ic(l_di->working_mode, l_di->cur_mode)) {
		snprintf(tmp_buf, sizeof(tmp_buf), "device ls init fail\n");
		goto fail_init;
	}

	if (dcm_init_batinfo(l_di->working_mode, l_di->cur_mode)) {
		snprintf(tmp_buf, sizeof(tmp_buf), "device bi init fail\n");
		goto fail_init;
	}

	hwlog_info("direct charge init succ\n");
	return 0;

fail_init:

	dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
		DC_EH_INIT_DIRECT_CHARGE, tmp_buf);

	return -1;
}

/* switch charging path to normal charging path */
int direct_charge_switch_path_to_normal_charging(void)
{
	msleep(WAIT_LS_DISCHARGE); /* need to wait device discharge */
	return dc_close_dc_channel();
}

/* switch charging path to lvc or sc charging path */
int direct_charge_switch_path_to_dc_charging(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	if (l_di->scp_work_on_charger) {
		dc_adapter_protocol_power_supply(ENABLE);
		charge_enable_eoc(EOC_DISABLE);
		reset_cur_delay_eoc_count();
		if (l_di->use_force_sleep)
			charge_enable_force_sleep(HIZ_MODE_ENABLE);
		else
			charge_set_hiz_enable(HIZ_MODE_ENABLE);
	}

	msleep(DT_MSLEEP_100MS);
	return dc_open_dc_channel();
}

int direct_charge_open_charging_path(void)
{
	int vbat = 0;
	int ibus = 0;
	int soc;
	int vbat_max = power_supply_app_get_bat_voltage_max();
	int max_dc_bat_vol;
	int adjust_times = MAX_TIMES_FOR_SET_ADAPTER_VOL;
	char tmp_buf[ERR_NO_STRING_SIZE] = { 0 };
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;
	soc = power_supply_app_get_bat_capacity();
	direct_charge_get_bat_sys_voltage(&vbat);

	/* first: set voltage and current to adapter */
	l_di->adaptor_vset = vbat * l_di->dc_volt_ratio + l_di->init_delt_vset;
	if (l_di->max_adapter_vset < l_di->adaptor_vset)
		l_di->adaptor_vset = l_di->max_adapter_vset;
	l_di->adaptor_iset = CURRENT_SET_FOR_RES_DETECT + l_di->init_delt_iset;
	direct_charge_set_adapter_voltage_step_by_step(l_di->adaptor_vset);
	dc_set_adapter_current(l_di->adaptor_iset);
	if (!direct_charge_check_enable_status() || direct_charge_get_stop_charging_flag())
		return -1;

	msleep(DT_MSLEEP_50MS);

	/* second: enable lvc or sc device */
	dc_open_aux_wired_channel();
	if (dcm_enable_ic(l_di->working_mode, l_di->cur_mode, DC_IC_ENABLE)) {
		snprintf(tmp_buf, sizeof(tmp_buf), "ls enable fail\n");
		goto fail_open;
	}
	if (!direct_charge_check_enable_status() || direct_charge_get_stop_charging_flag())
		return -1;

	power_usleep(DT_USLEEP_10MS);

	if (direct_charge_get_device_ibus(&ibus))
		return -1;

	max_dc_bat_vol = vbat_max > l_di->max_dc_bat_vol ?
		vbat_max : l_di->max_dc_bat_vol;

	hwlog_info("[%d]: Vapt_set=%d, Ibus_ls=%d Vbat_max=%d\n",
		adjust_times, l_di->adaptor_vset, ibus, max_dc_bat_vol);

	/* third: adjust adapter voltage until current to 800ma */
	while (ibus < MIN_CURRENT_FOR_RES_DETECT) {
		direct_charge_get_bat_voltage(&vbat);
		if (max_dc_bat_vol < vbat) {
			snprintf(tmp_buf, sizeof(tmp_buf),
				"vbat too high, Vapt_set=%d\n", vbat);
			goto fail_open;
		}

		if (!(adjust_times % DC_OPEN_CHECK_IC_CYCLE) &&
			dcm_is_ic_close(l_di->working_mode, l_di->cur_mode)) {
			hwlog_info("ic is closed, need open\n");
			if (dcm_enable_ic(l_di->working_mode, l_di->cur_mode, DC_IC_ENABLE)) {
				snprintf(tmp_buf, sizeof(tmp_buf), "ls enable fail\n");
				goto fail_open;
			}
		}

		adjust_times--;
		if (adjust_times == 0) {
			snprintf(tmp_buf, sizeof(tmp_buf),
				"try too many times, Ibus_ls=%d\n", ibus);
			goto fail_open;
		}

		l_di->adaptor_vset += l_di->vstep;
		dc_set_adapter_voltage(l_di->adaptor_vset);
		if (!direct_charge_check_enable_status() || direct_charge_get_stop_charging_flag())
			goto fail_open;
		/* delay 5ms */
		usleep_range(5000, 5100);
		direct_charge_get_device_ibus(&ibus);

		hwlog_info("[%d]: Vapt_set=%d, Ibus_ls=%d\n",
			adjust_times, l_di->adaptor_vset, ibus);
	}

	hwlog_err("charging path open succ\n");
	return 0;

fail_open:
	dc_fill_eh_buf(l_di->dsm_buff, sizeof(l_di->dsm_buff),
		DC_EH_OPEN_DIRECT_CHARGE_PATH, tmp_buf);
	if (soc >= BATTERY_CAPACITY_HIGH_TH) {
		l_di->dc_open_retry_cnt += 1;
		hwlog_info("current battery capacity is %d, over threshold %d\n",
		soc, BATTERY_CAPACITY_HIGH_TH);
	}

	hwlog_err("charging path open fail\n");
	return -1;
}

int direct_charge_detect_adapter(void)
{
	unsigned int prot_type = adapter_detect_ping_protocol_type();
	int ret = dc_select_adapter_protocol_type(prot_type);

	hwlog_info("adapter detect: prot_type=%x ret=%d\n", prot_type, ret);
	return ret;
}

static bool direct_charge_check_adapter_voltage_range(
	struct direct_charge_device *di)
{
	int max_vadp = 0;
	int min_vadp = 0;
	int bat_num = hw_battery_get_series_num();
	int ratio = di->dc_volt_ratio;
	int delta = di->init_delt_vset;
	int max_vbat = di->max_dc_bat_vol;
	int min_vbat = di->min_dc_bat_vol;
	int max_cfg_volt;
	int min_cfg_volt;
	int ret;

	ret = dc_get_adapter_max_voltage(&max_vadp);
	if (ret) {
		hwlog_err("get adapter max voltage fail\n");
		return false;
	}

	ret = dc_get_adapter_min_voltage(&min_vadp);
	if (ret) {
		hwlog_err("get adapter min voltage fail\n");
		return false;
	}

	hwlog_info("max_vadp=%d, max_vbat=%d, bat_num=%d, ratio=%d, delta=%d\n",
		max_vadp, max_vbat, bat_num, ratio, delta);
	hwlog_info("min_vadp=%d, min_vbat=%d, bat_num=%d, ratio=%d, delta=%d\n",
		min_vadp, min_vbat, bat_num, ratio, delta);

	/*
	 * how to calculate max configure voltage as below:
	 * max battery voltage * battery number * ratio + delta
	 */
	max_cfg_volt = max_vbat * bat_num * ratio + delta;
	min_cfg_volt = min_vbat * bat_num * ratio + delta;

	if ((max_vadp > max_cfg_volt) && (min_vadp < min_cfg_volt))
		return true;

	return false;
}

static int direct_charge_redetect_adapter_by_voltage(unsigned int mode)
{
	unsigned int new_mode = ADAPTER_SUPPORT_UNDEFINED;
	unsigned int local_mode = direct_charge_get_local_mode();
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;

	/* we return the raw mode if not b_class adapter */
	if (!(mode & (ADAPTER_SUPPORT_LVC | ADAPTER_SUPPORT_SC | ADAPTER_SUPPORT_SC4)))
		return mode;

	/* we return the raw mode if local mode not support lvc or sc */
	if (!(local_mode & (LVC_MODE | SC_MODE | SC4_MODE)))
		return mode;

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	if (lvc_di && (local_mode & LVC_MODE)) {
		if (direct_charge_check_adapter_voltage_range(lvc_di))
			new_mode |= ADAPTER_SUPPORT_LVC;
	}

	if (sc_di && (local_mode & SC_MODE)) {
		if (direct_charge_check_adapter_voltage_range(sc_di))
			new_mode |= ADAPTER_SUPPORT_SC;
	}

	if (sc4_di && (local_mode & SC4_MODE)) {
		if (direct_charge_check_adapter_voltage_range(sc4_di))
			new_mode |= ADAPTER_SUPPORT_SC4;
	}

	hwlog_info("local_mode=%x, adapter_mode=%x, new_adapter_mode=%x\n",
		local_mode, mode, new_mode);

	if (!new_mode)
		direct_charge_set_abnormal_adp_flag(true);

	return new_mode;
}

unsigned int direct_charge_detect_adapter_support_mode(void)
{
	unsigned int adp_mode = ADAPTER_SUPPORT_UNDEFINED;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return adp_mode;

	adp_mode = dc_get_adapter_support_mode();
	adp_mode &= dc_update_adapter_support_mode();

	if (l_di->adaptor_detect_by_voltage)
		return direct_charge_redetect_adapter_by_voltage(adp_mode);

	return adp_mode;
}

int direct_charge_detect_adapter_again(void)
{
	int ret;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	ret = direct_charge_detect_adapter();
	if (ret) {
		direct_charge_set_stage_status(DC_STAGE_DEFAULT);
		return -1;
	}

	adapter_test_set_result(l_di->adaptor_test_result_type,
		AT_DETECT_SUCC);
	return 0;
}

static void direct_charge_reset_para_in_stop(struct direct_charge_device *di)
{
	if (!di)
		return;

	di->error_cnt = 0;
	di->otp_cnt = 0;
	di->adp_otp_cnt = 0;
	di->reverse_ocp_cnt = 0;
	di->dc_open_retry_cnt = 0;
	di->full_path_resistance = DC_ERROR_RESISTANCE;
	di->second_path_resistance = DC_ERROR_RESISTANCE;
	di->second_resist_check_ok = false;
	dc_mmi_set_succ_flag(di->working_mode, DC_ERROR_ADAPTER_DETECT);
	dc_mmi_set_test_flag(false);
	di->dc_succ_flag = DC_ERROR;
	di->cc_cable_detect_ok = 0;
	di->dc_err_report_flag = FALSE;
	di->sc_conv_ocp_count = 0;
	di->low_temp_hysteresis = 0;
	di->high_temp_hysteresis = 0;
	di->cable_type = DC_UNKNOWN_CABLE;
	di->orig_cable_type = DC_UNKNOWN_CABLE;
	di->max_pwr = 0;
	di->pri_inversion = false;
	di->adp_antifake_failed_cnt = 0;
	di->adp_antifake_result = ADAPTER_ANTIFAKE_INIT;
	direct_charge_set_abnormal_adp_flag(false);
	dc_clean_eh_buf(di->dsm_buff, sizeof(di->dsm_buff));
	dc_reset_adapter_power_curve(di);
	di->cc_safe = true;
}

static void direct_charge_reset_para_in_exit(struct direct_charge_device *di)
{
	int i;

	if (!di)
		return;

	di->full_path_resistance = DC_ERROR_RESISTANCE;
	di->second_path_resistance = DC_ERROR_RESISTANCE;
	di->second_resist_check_ok = false;
	dc_mmi_set_succ_flag(di->working_mode, DC_ERROR_ADAPTER_DETECT);
	dc_mmi_set_test_flag(false);
	direct_charge_set_stop_charging_flag(0);
	di->dc_succ_flag = DC_ERROR;
	di->scp_stop_charging_flag_info = 0;
	di->cur_stage = 0;
	di->pre_stage = 0;
	di->vbat = 0;
	di->ibat = 0;
	di->vadapt = 0;
	di->iadapt = 0;
	di->ls_vbus = 0;
	di->ls_ibus = 0;
	di->compensate_v = 0;
	di->cc_cable_detect_ok = 0;
	di->ibat_abnormal_cnt = 0;
	di->max_adaptor_cur = 0;
	di->dc_open_retry_cnt = 0;
	di->otp_cnt = 0;
	di->adp_otp_cnt = 0;
	di->reverse_ocp_cnt = 0;
	di->dc_err_report_flag = FALSE;
	di->cc_safe = true;
	di->low_temp_hysteresis = 0;
	di->high_temp_hysteresis = 0;
	di->cable_type = DC_UNKNOWN_CABLE;
	di->orig_cable_type = DC_UNKNOWN_CABLE;
	di->max_pwr = 0;
	di->bat_temp_err_flag = false;
	di->pri_inversion = false;
	di->multi_ic_check_info.limit_current = di->iin_thermal_default;
	di->multi_ic_error_cnt = 0;
	for (i = 0; i < DC_MODE_TOTAL; i++)
		di->rt_test_para[i].rt_test_result = false;
	direct_charge_set_abnormal_adp_flag(false);
	dc_clean_eh_buf(di->dsm_buff, sizeof(di->dsm_buff));
	dc_reset_adapter_power_curve(di);
	memset(di->multi_ic_check_info.report_info, 0,
		sizeof(di->multi_ic_check_info.report_info));
	memset(di->multi_ic_check_info.ibus_error_num, 0,
		sizeof(di->multi_ic_check_info.ibus_error_num));
	memset(di->multi_ic_check_info.vbat_error_num, 0,
		sizeof(di->multi_ic_check_info.vbat_error_num));
	memset(di->multi_ic_check_info.tbat_error_num, 0,
		sizeof(di->multi_ic_check_info.tbat_error_num));
	memset(di->multi_ic_mode_para.ic_error_cnt, 0,
		sizeof(di->multi_ic_mode_para.ic_error_cnt));
}

void direct_charge_exit(void)
{
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;

	hwlog_info("exit\n");

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	if (!lvc_di && !sc_di && !sc4_di) {
		hwlog_info("local not support direct_charge\n");
		return;
	}

	if (!power_cmdline_is_factory_mode())
		direct_charge_set_disable_flags(DC_CLEAR_DISABLE_FLAGS,
			DC_DISABLE_SYS_NODE);

	direct_charge_set_can_enter_status(true);
	direct_charge_reset_para_in_exit(lvc_di);
	direct_charge_reset_para_in_exit(sc_di);
	direct_charge_reset_para_in_exit(sc4_di);
}

int direct_charge_get_cutoff_normal_flag(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return 0;

	return l_di->cutoff_normal_flag;
}

void direct_charge_update_cutoff_normal_flag(void)
{
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;

	hwlog_info("update_cutoff_normal_flag\n");

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	if (lvc_di)
		direct_charge_reset_para_in_stop(lvc_di);

	if (sc_di)
		direct_charge_reset_para_in_stop(sc_di);

	if (sc4_di)
		direct_charge_reset_para_in_stop(sc4_di);
}

static void direct_charge_update_charging_volt_param(
	struct direct_charge_volt_para *para, int stage_size)
{
	int i, bat_max_cur, bat_max_volt;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di || !para)
		return;

	/* 1000ma : current threshold, 20mv : voltage threshold */
	bat_max_cur = para[0].cur_th_high + 1000;
	bat_max_volt = para[stage_size - 1].vol_th + 20;
	if (l_di->volt_para_test_flag) {
		for (i = 0; i < l_di->stage_size; i++) {
			if (l_di->volt_para[0].cur_th_high > bat_max_cur)
				l_di->volt_para[0].cur_th_high = bat_max_cur;
			if (l_di->volt_para[i].vol_th > bat_max_volt)
				l_di->volt_para[i].vol_th = bat_max_volt;
			hwlog_info("%4d %5d %5d\n", l_di->volt_para[i].vol_th,
				l_di->volt_para[i].cur_th_high, l_di->volt_para[i].cur_th_low);
		}
	} else {
		l_di->stage_size = stage_size;
		memcpy(l_di->volt_para, para, sizeof(l_di->volt_para));
	}
	memcpy(l_di->orig_volt_para, l_di->volt_para, sizeof(l_di->orig_volt_para));
}

static void direct_charge_select_charging_volt_param(void)
{
	int i;
	const char *brand = NULL;
	int tbat;
	int group_cur = 0;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	brand = power_supply_app_get_bat_brand();
	tbat = l_di->bat_temp_before_charging;

	hwlog_info("stage_group_size=%d, bat_brand=%s, bat_temp=%d\n",
		l_di->stage_group_size, brand, tbat);

	for (i = 0; i < l_di->stage_group_size; i++) {
		if (!l_di->orig_volt_para_p[i].bat_info.parse_ok)
			continue;

		if (!strstr(brand, l_di->orig_volt_para_p[i].bat_info.batid))
			continue;

		if ((tbat >= l_di->orig_volt_para_p[i].bat_info.temp_high) ||
			(tbat < l_di->orig_volt_para_p[i].bat_info.temp_low))
			continue;

		group_cur = i;
		hwlog_info("[%d]: bat_id=%s, temp_low=%d, temp_high=%d\n", i,
			l_di->orig_volt_para_p[group_cur].bat_info.batid,
			l_di->orig_volt_para_p[group_cur].bat_info.temp_low,
			l_di->orig_volt_para_p[group_cur].bat_info.temp_high);
		break;
	}

	direct_charge_update_charging_volt_param(l_di->orig_volt_para_p[group_cur].volt_info,
		l_di->orig_volt_para_p[group_cur].stage_size);
}

static int direct_charge_get_imax_with_cur_mode(struct direct_charge_device *di)
{
	int ibat = 0;

	if (di->multi_ic_mode_para.support_multi_ic && di->cur_mode != CHARGE_MULTI_IC) {
		dcm_get_ic_max_ibat(di->working_mode, di->cur_mode, &ibat);
		if (!ibat)
			ibat = DC_SINGLEIC_CURRENT_LIMIT;
	}

	return ibat;
}

bool direct_charge_check_timeout(void)
{
	u32 delta_time;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return false;

	delta_time = power_get_current_kernel_time().tv_sec - l_di->direct_charge_start_time;
	if ((l_di->charge_time > 0) && (delta_time > l_di->charge_time))
		return true;

	return false;
}

static int direct_charge_get_imax_by_time(struct direct_charge_device *di)
{
	u32 delta_time;
	int i;

	if (!di->time_para_parse_ok)
		return 0;

	delta_time = power_get_current_kernel_time().tv_sec - di->direct_charge_start_time;
	for (i = 0; i < DC_TIME_PARA_LEVEL; i++) {
		if (delta_time >= di->time_para[i].time_th)
			break;
	}

	if (i >= DC_TIME_PARA_LEVEL)
		return 0;

	return di->time_para[i].ibat_max;
}

static int direct_charge_cal_max_power_time(struct direct_charge_device *di)
{
	if (di->limit_max_pwr.run_time >= di->limit_max_pwr.max_power_timeout) {
		di->limit_max_pwr.start_timer_flag = false;
		return di->limit_max_pwr.limit_current;
	}

	if (di->dc_volt_ratio == 0)
		return 0;

	/* calculate max power maintain time, if it is interrupted, time will be suspended */
	if (di->cur_ibat_th_high / di->dc_volt_ratio < di->max_adaptor_cur) {
		if (di->limit_max_pwr.start_timer_flag)
			di->limit_max_pwr.start_timer_flag = false;
		return 0;
	}

	if (!di->limit_max_pwr.start_timer_flag) {
		di->limit_max_pwr.start_timer_flag = true;
		di->limit_max_pwr.start_time = power_get_current_kernel_time().tv_sec;
		return 0;
	}

	di->limit_max_pwr.run_time += power_get_current_kernel_time().tv_sec -
		di->limit_max_pwr.start_time;
	di->limit_max_pwr.start_time = power_get_current_kernel_time().tv_sec;
	return 0;
}

static void direct_charge_get_max_power_para(struct direct_charge_device *di)
{
	int i;
	int adapter_type = dc_get_adapter_type();

	if (!di->max_power_time[0].adatper_type)
		return;

	for (i = 0; i < DC_MAX_POWER_TIME_PARA_LEVEL; i++) {
		if (adapter_type != di->max_power_time[i].adatper_type)
			continue;
		di->limit_max_pwr.limit_current = di->max_power_time[i].limit_current;
		di->limit_max_pwr.max_power_timeout = di->max_power_time[i].max_power_time;
		hwlog_info("max_power_timeout=%d,max_power_limit_current=%d\n",
			di->limit_max_pwr.max_power_timeout, di->limit_max_pwr.limit_current);
		return;
	}
}

static int direct_charge_select_adp_max_curr(struct direct_charge_device *di)
{
	int adp_max_cur;

	adp_max_cur = dc_get_adapter_max_current(di->adaptor_vset);
	adp_max_cur += direct_charge_get_adapter_gain_current(di);
	if (adp_max_cur != di->max_adaptor_cur) {
		hwlog_info("adp_max_cur=%d\n", adp_max_cur);
		di->max_adaptor_cur = adp_max_cur;
	}
	if (di->use_5a && (adp_max_cur == 4500)) /* 4500ma */
		adp_max_cur = di->volt_para[0].cur_th_high;

	/* some adapters support power drop, will reduce power when OTP */
	(void)dc_get_power_drop_current(&adp_max_cur);
	return adp_max_cur;
}

static int direct_charge_select_ichg_thermal(struct direct_charge_device *di)
{
	int ichg_thermal_cur;
	unsigned int idx;

	idx = (di->cur_mode == CHARGE_MULTI_IC) ? DC_DUAL_CHANNEL : DC_SINGLE_CHANNEL;
	di->sysfs_iin_thermal = di->sysfs_iin_thermal_array[idx];

	if (di->ichg_control_enable)
		ichg_thermal_cur = di->sysfs_iin_thermal_ichg_control;
	else
		ichg_thermal_cur = di->sysfs_iin_thermal;

	hwlog_info("ichg_thermal_cur=%d\n", ichg_thermal_cur);
	return ichg_thermal_cur;
}

static void direct_charge_select_comp_param(struct direct_charge_device *di)
{
	const char *ic_name = NULL;

	/* get vbat compensation of different ic */
	ic_name = dcm_get_ic_name(di->working_mode, CHARGE_IC_AUX);
	dc_get_vbat_comp_para(ic_name, &di->comp_para);
	memcpy(di->multi_ic_check_info.vbat_comp, di->comp_para.vbat_comp,
		sizeof(di->comp_para.vbat_comp));
}

static int direct_charge_get_imax_by_batt_balance(struct direct_charge_device *di)
{
	struct bat_balance_info info[BAT_PARALLEL_COUNT];
	u32 bat_num = 1; /* default one battery */
	struct bat_balance_cur result;
	struct power_supply *psy = NULL;

	/* update 1st battery info */
	power_supply_get_int_property_value("battery_gauge",
		POWER_SUPPLY_PROP_CURRENT_NOW, &info[0].cur);
	info[0].vol = dcm_get_ic_vbtb_with_comp(di->working_mode, CHARGE_IC_MAIN,
		di->multi_ic_check_info.vbat_comp);
	bat_temp_get_temperature(BTB_TEMP_0, &info[0].temp);

	if (power_supply_check_psy_available("battery_gauge_aux", &psy)) {
		bat_num = 2; /* 2: two battery */
		power_supply_get_int_property_value("battery_gauge_aux",
			POWER_SUPPLY_PROP_CURRENT_NOW, &info[1].cur);
		info[1].vol = dcm_get_ic_vbtb_with_comp(di->working_mode, CHARGE_IC_AUX,
			di->multi_ic_check_info.vbat_comp);
		if (info[1].vol < 0)
			info[1].vol = info[0].vol;
		bat_temp_get_temperature(BTB_TEMP_1, &info[1].temp);
	}

	if (bat_balance_get_cur_info(info, bat_num, &result))
		return 0;

	hwlog_info("batt_balance_limit = %d", result.total_cur);
	return result.total_cur;
}

void direct_charge_select_charging_param(void)
{
	int cur_th_high;
	int vbat_th;
	int vol_th;
	int tbat = 0;
	int tbat_max_cur;
	int mode_max_cur;
	int time_max_cur;
	int cable_max_cur;
	int max_power_cur;
	int path_resist;
	int path_resist_max_cur;
	int adp_max_cur;
	int ichg_thermal_cur;
	int batt_limit;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	/* unit: mv */
	l_di->compensate_v = l_di->ibat * l_di->compensate_r / POWER_UV_PER_MV;
	/* cur_stage include cc and cv stage so divide 2 */
	vbat_th = l_di->volt_para[l_di->cur_stage / 2].vol_th +
		l_di->compensate_v;
	vol_th = l_di->volt_para[l_di->stage_size - 1].vol_th;
	if (l_di->all_stage_compensate_r_en)
		l_di->cur_vbat_th = vbat_th;
	else
		l_di->cur_vbat_th = vbat_th < vol_th ? vbat_th : vol_th;

	/* cur_stage include cc and cv stage so divide 2 */
	l_di->cur_ibat_th_low = l_di->volt_para[l_di->cur_stage / 2].cur_th_low;

	/* cur_stage include cc and cv stage so divide 2 */
	cur_th_high = l_di->volt_para[l_di->cur_stage / 2].cur_th_high;

	/* step-1: get max current with specified temp */
	bat_temp_get_temperature(BAT_TEMP_MIXED, &tbat);
	tbat_max_cur = direct_charge_battery_temp_handler(tbat);
	if (tbat_max_cur == 0) {
		hwlog_info("temp=%d, can not do direct charging\n", tbat);
		if (tbat < DC_LOW_TEMP_MAX)
			l_di->low_temp_hysteresis = l_di->orig_low_temp_hysteresis;
		if (tbat >= DC_HIGH_TEMP_MAX)
			l_di->high_temp_hysteresis = l_di->orig_high_temp_hysteresis;
		l_di->scp_stop_charging_flag_info = 1;
		l_di->bat_temp_err_flag = true;
	}
	cur_th_high = cur_th_high > tbat_max_cur ? tbat_max_cur : cur_th_high;

	/* step-2: get max current with adapter */
	adp_max_cur = direct_charge_select_adp_max_curr(l_di);
	cur_th_high = cur_th_high > l_di->dc_volt_ratio * adp_max_cur ?
		l_di->dc_volt_ratio * adp_max_cur : cur_th_high;

	/* step-3: get max current with cable type */
	cable_max_cur = dc_get_cable_max_current();
	if (l_di->cc_cable_detect_enable && cable_max_cur)
		cur_th_high = (cur_th_high > cable_max_cur) ? cable_max_cur : cur_th_high;
	hwlog_info("cc_cable=%d, cable_max_cur=%d, cur_th_high=%d\n",
		l_di->cc_cable_detect_ok, cable_max_cur, cur_th_high);

	/* step-4: get max current with specified resist */
	path_resist = l_di->full_path_resistance;
	path_resist_max_cur = dc_resist_handler(l_di->working_mode, path_resist);
	if (path_resist_max_cur != 0)
		cur_th_high = cur_th_high > path_resist_max_cur ?
			path_resist_max_cur : cur_th_high;

	/* get max current with specified secondary resist */
	path_resist_max_cur = dc_second_resist_handler();
	if (path_resist_max_cur != 0)
		cur_th_high = cur_th_high > path_resist_max_cur ?
			path_resist_max_cur : cur_th_high;

	/* step-5: get max current with cur_mode */
	mode_max_cur = direct_charge_get_imax_with_cur_mode(l_di);
	if (mode_max_cur > 0)
		cur_th_high = cur_th_high > mode_max_cur ? mode_max_cur : cur_th_high;

	/* step-6: get max current with time limit */
	time_max_cur = direct_charge_get_imax_by_time(l_di);
	if (time_max_cur > 0)
		cur_th_high = cur_th_high > time_max_cur ? time_max_cur : cur_th_high;

	/* step-7: error check limit current, only multi mode effected */
	if (l_di->multi_ic_check_info.limit_current > 0 && l_di->cur_mode == CHARGE_MULTI_IC)
		cur_th_high = (cur_th_high > l_di->multi_ic_check_info.limit_current) ?
			l_di->multi_ic_check_info.limit_current : cur_th_high;

	/* step-8: get max current with thermal */
	ichg_thermal_cur = direct_charge_select_ichg_thermal(l_di);
	if (!dc_mmi_get_test_flag())
		cur_th_high = (cur_th_high > ichg_thermal_cur) ?
			ichg_thermal_cur : cur_th_high;

	/* calculate max power maintain time and limit current */
	max_power_cur = direct_charge_cal_max_power_time(l_di);
	if (max_power_cur > 0)
		cur_th_high = cur_th_high > max_power_cur ? max_power_cur : cur_th_high;

	batt_limit = direct_charge_get_imax_by_batt_balance(l_di);
	if (batt_limit > 0)
		cur_th_high = cur_th_high > batt_limit ? batt_limit : cur_th_high;

	l_di->cur_ibat_th_high = cur_th_high;

	/* enable sc and limit cur to 3000mA even when cc is unsafe when cc_unsafe_sc_enable is true */
	if (l_di->cc_unsafe_sc_enable && !l_di->cc_safe)
		l_di->cur_ibat_th_high = l_di->cur_ibat_th_high > CC_UNSAFE_MAX_CURRENT ?
			CC_UNSAFE_MAX_CURRENT : l_di->cur_ibat_th_high;
}

static int direct_charge_jump_stage_if_need(int cur_stage)
{
	int i;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return -1;

	/* 2: cc & cv */
	for (i = 0; i < 2 * l_di->stage_size; ++i) {
		if (l_di->stage_need_to_jump[i] == -1)
			return cur_stage;

		if (cur_stage == l_di->stage_need_to_jump[i]) {
			hwlog_info("jump stage to %d\n", cur_stage);
			return direct_charge_jump_stage_if_need(cur_stage + 1);
		}
	}

	return cur_stage;
}

void direct_charge_select_charging_stage(void)
{
	int i;
	int vbat_th;
	int cur_stage = 0;
	int stage_size;
	int vbat = 0;
	int ibat = 0;
	int iadp = 0;
	int max_cnt = 10; /* 10 counts */
	char tmp_buf[ERR_NO_STRING_SIZE] = { 0 };
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	direct_charge_get_bat_voltage(&vbat);
	direct_charge_get_bat_current(&ibat);
	dc_get_adapter_current(&iadp);

	stage_size = l_di->stage_size;
	l_di->vbat = vbat;
	l_di->ibat = ibat;

	if (iadp < l_di->ibat_abnormal_th)
		l_di->ibat_abnormal_cnt++;
	else
		l_di->ibat_abnormal_cnt = 0;

	if (l_di->ibat_abnormal_cnt > max_cnt) {
		direct_charge_set_stop_charging_flag(1);
		snprintf(tmp_buf, sizeof(tmp_buf),
			"ibat abnormal, stop direct charge\n");
		dc_fill_eh_buf(l_di->dsm_buff,
			sizeof(l_di->dsm_buff), DC_EH_IBAT_ABNORMAL, tmp_buf);
		return;
	}

	l_di->pre_stage = l_di->cur_stage;
	for (i = stage_size - 1; i >= 0; --i) {
		vbat_th = l_di->volt_para[i].vol_th + l_di->compensate_v;
		if (!l_di->all_stage_compensate_r_en)
			vbat_th = (vbat_th > l_di->volt_para[stage_size - 1].vol_th) ?
				l_di->volt_para[stage_size - 1].vol_th : vbat_th;
		/* 0: cc stage, 1: cv stage, 2: max stage */
		if (vbat >= vbat_th && ibat <= l_di->volt_para[i].cur_th_low) {
			cur_stage = 2 * i + 2;
			break;
		} else if (vbat >= vbat_th) {
			cur_stage = 2 * i + 1;
			break;
		}
	}

	if (i < 0)
		cur_stage = 0;

	if (cur_stage < l_di->pre_stage)
		cur_stage = l_di->pre_stage;

	if (l_di->first_cc_stage_timer_in_min) {
		/* 0: cc stage, 1: cv stage, 2: max stage */
		if (cur_stage == 0 || cur_stage == 1) {
			if (time_after(jiffies, l_di->first_cc_stage_timeout)) {
				hwlog_info("first_cc_stage in %d min, set stage=2\n",
					l_di->first_cc_stage_timer_in_min);
				cur_stage = 2;
			}
		}
	}

	if (cur_stage != l_di->cur_stage)
		l_di->cur_stage = direct_charge_jump_stage_if_need(cur_stage);
}

void direct_charge_soh_policy(void)
{
	int i;
	int volt_max;
	int cur_max;
	bool print = false;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	if ((l_di->stage_size < 1) || (l_di->stage_size > DC_VOLT_LEVEL)) {
		hwlog_err("stage_size %d out of range\n", l_di->stage_size);
		return;
	}

	volt_max = l_di->orig_volt_para[l_di->stage_size - 1].vol_th -
		l_di->vterm_dec;
	if (l_di->volt_para[l_di->stage_size - 1].vol_th != volt_max) {
		l_di->volt_para[l_di->stage_size - 1].vol_th = volt_max;
		print = true;
	}

	for (i = 0; i < l_di->stage_size - 1; i++)
		l_di->volt_para[i].vol_th =
			(l_di->orig_volt_para[i].vol_th < volt_max) ?
			l_di->orig_volt_para[i].vol_th : volt_max;

	if (l_di->ichg_ratio == 0)
		l_di->ichg_ratio = BASP_PARA_SCALE;
	cur_max = l_di->orig_volt_para[0].cur_th_high *
		l_di->ichg_ratio / BASP_PARA_SCALE;
	if (l_di->volt_para[0].cur_th_high != cur_max) {
		l_di->volt_para[0].cur_th_high = cur_max;
		print = true;
	}
	l_di->volt_para[0].cur_th_low =
		l_di->orig_volt_para[0].cur_th_low;

	for (i = 1; i < l_di->stage_size; i++) {
		l_di->volt_para[i].cur_th_high =
			(l_di->orig_volt_para[i].cur_th_high <= cur_max) ?
			l_di->orig_volt_para[i].cur_th_high : cur_max;
		l_di->volt_para[i].cur_th_low =
			l_di->orig_volt_para[i].cur_th_low;
	}

	if (!print)
		return;

	hwlog_info("dc_volt_dec:%d, dc_cur_ratio:%d\n",
		l_di->vterm_dec, l_di->ichg_ratio);
	for (i = 0; i < l_di->stage_size; i++)
		hwlog_info("volt_para[%d]: vol_th:%d cur_th_high:%d cur_th_low:%d\n",
			i, l_di->volt_para[i].vol_th,
			l_di->volt_para[i].cur_th_high,
			l_di->volt_para[i].cur_th_low);
}

static int direct_charge_get_vstep(struct direct_charge_device *di)
{
	int i;
	int ibat = 0;

	/* only cc stage support increase vstep except for the last cc */
	if (di->cur_stage % 2 || (di->cur_stage >= (di->stage_size - 1) * 2))
		return di->vstep;

	if (direct_charge_get_bat_current(&ibat))
		return di->vstep;

	/* only new adapter and not factory version support increase vstep */
	if (!power_cmdline_is_factory_mode() &&
		(dc_get_adapter_type() != ADAPTER_TYPE_11V6A) &&
		(dc_get_adapter_type() != ADAPTER_TYPE_YLR_20V5A_CAR))
		return di->vstep;

	for (i = 0; i < DC_VSTEP_PARA_LEVEL; i++) {
		if (di->vstep_para[i].curr_gap == 0)
			break;
		if (di->cur_ibat_th_high - ibat > di->vstep_para[i].curr_gap)
			return di->vstep_para[i].vstep;
	}

	return di->vstep;
}

static void direct_charge_set_multi_sc_result(int ibat,
	struct direct_charge_device *di)
{
	unsigned int mode = di->cur_mode;

	if (mode & CHARGE_IC_MAIN) {
		if (ibat > (int)di->rt_test_para[DC_NORMAL_MODE].rt_curr_th)
			di->rt_test_para[DC_NORMAL_MODE].rt_test_result = true;
		if (ibat > (int)di->rt_test_para[DC_CHAN1_MODE].rt_curr_th)
			di->rt_test_para[DC_CHAN1_MODE].rt_test_result = true;
	}
	if (mode & CHARGE_IC_AUX) {
		if (ibat > (int)di->rt_test_para[DC_CHAN2_MODE].rt_curr_th)
			di->rt_test_para[DC_CHAN2_MODE].rt_test_result = true;
	}
}

static void direct_charge_set_rt_test_result(int ibat,
	struct direct_charge_device *di)
{
	if (!di || !power_cmdline_is_factory_mode())
		return;

	if (di->multi_ic_mode_para.support_multi_ic) {
		direct_charge_set_multi_sc_result(ibat, di);
	} else {
		if (ibat > (int)di->rt_test_para[DC_NORMAL_MODE].rt_curr_th)
			di->rt_test_para[DC_NORMAL_MODE].rt_test_result = true;
	}
}

void direct_charge_regulation(void)
{
	int ret;
	int iadp = 0;
	int iadp_set = 0;
	int vbat;
	int ibat;
	int vbat_th;
	int ibat_th_high;
	int ibat_th_low;
	int ratio;
	int delta_err;
	int vstep;
	char tmp_buf[ERR_NO_STRING_SIZE] = { 0 };
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	dc_get_adapter_current(&iadp);
	dc_get_adapter_current_set(&iadp_set);
	dc_get_adapter_voltage(&l_di->vadapt);
	dc_get_adapter_temp(&l_di->tadapt);
	direct_charge_get_device_ibus(&l_di->ls_ibus);
	direct_charge_get_device_vbus(&l_di->ls_vbus);
	direct_charge_get_device_temp(&l_di->tls);
	vstep = direct_charge_get_vstep(l_di);

	delta_err = direct_charge_get_device_delta_err(l_di);
	l_di->iadapt = iadp;
	vbat = l_di->vbat;
	ibat = l_di->ibat;
	vbat_th = l_di->cur_vbat_th;
	ibat_th_high = l_di->cur_ibat_th_high;
	ibat_th_low = l_di->cur_ibat_th_low;

	hwlog_info("cur_stage[%d]: vbat=%d vbat_th=%d\t"
		"ibat=%d ibat_th_high=%d ibat_th_low=%d\t"
		"vadp=%d iadp=%d iadp_set=%d\t"
		"ls_vbus=%d ls_ibus=%d tadp=%d tls=%d\n",
		l_di->cur_stage, vbat, vbat_th,
		ibat, ibat_th_high, ibat_th_low,
		l_di->vadapt, iadp, iadp_set,
		l_di->ls_vbus, l_di->ls_ibus, l_di->tadapt, l_di->tls);

	/* record rt adapter test result when test succ */
	direct_charge_set_rt_test_result(ibat, l_di);

	/* secondary resistance check */
	if (l_di->second_resist_check_en && l_di->cc_cable_detect_ok &&
		(l_di->second_resist_check_ok == false))
		dc_calculate_second_path_resistance();

	/* check temperature with device */
	if (l_di->tls > l_di->max_tls) {
		snprintf(tmp_buf, sizeof(tmp_buf),
			"tls=%d > %d, stop direct charge\n",
			l_di->tls, l_di->max_tls);
		dc_fill_eh_buf(l_di->dsm_buff,
			sizeof(l_di->dsm_buff), DC_EH_TLS_ABNORMAL, tmp_buf);
		direct_charge_set_stop_charging_flag(1);
		return;
	}

	/* check temperature with adapter */
	if (l_di->tadapt > l_di->max_tadapt) {
		snprintf(tmp_buf, sizeof(tmp_buf),
			"tadp=%d > %d, stop direct charge\n",
			l_di->tadapt, l_di->max_tadapt);
		dc_fill_eh_buf(l_di->dsm_buff,
			sizeof(l_di->dsm_buff), DC_EH_TADP_ABNORMAL, tmp_buf);

		l_di->adp_otp_cnt++;
		if (l_di->adp_otp_cnt >= ADP_OTP_CNT) {
			l_di->adp_otp_cnt = ADP_OTP_CNT;
			power_dsm_report_dmd(POWER_DSM_BATTERY,
				DSM_DIRECT_CHARGE_ADAPTER_OTP, tmp_buf);
		}

		direct_charge_set_stop_charging_flag(1);
		return;
	}

	/* keep communication with device within 1 second */
	ret = direct_charge_get_device_close_status();
	if (ret)
		return;

	ratio = l_di->dc_volt_ratio;
	if (ratio == 0) {
		hwlog_err("do nothing, ratio is 0\n");
		return;
	}

	/* 2: cc and cv stage */
	if (l_di->cur_stage % 2) {
		if (vbat > vbat_th) {
			l_di->adaptor_vset += ratio * (vbat_th - vbat);
			dc_set_adapter_voltage(l_di->adaptor_vset);
			return;
		}

		if (iadp > ibat_th_high / ratio) {
			l_di->adaptor_vset -= vstep;
			dc_set_adapter_voltage(l_di->adaptor_vset);
			return;
		}

		if (ibat > ibat_th_high) {
			l_di->adaptor_vset -= vstep;
			dc_set_adapter_voltage(l_di->adaptor_vset);
			return;
		}

		if (ibat > ibat_th_high - delta_err) {
			hwlog_info("do nothing\n");
			return;
		}

		/* adjust 1000ma/ratio per step */
		if (l_di->adaptor_iset < (ibat_th_high - 1000) / ratio) {
			l_di->adaptor_iset += 1000 / ratio;
			dc_set_adapter_current(l_di->adaptor_iset);
			return;
		} else if (l_di->adaptor_iset < ibat_th_high / ratio) {
			l_di->adaptor_iset = ibat_th_high / ratio;
			dc_set_adapter_current(l_di->adaptor_iset);
			return;
		} else if (iadp < (ibat_th_high - delta_err) / ratio) {
			l_di->adaptor_vset += vstep;
			dc_set_adapter_voltage(l_di->adaptor_vset);
			return;
		}
	} else {
		if (iadp > ibat_th_high / ratio) {
			l_di->adaptor_vset -= vstep;
			dc_set_adapter_voltage(l_di->adaptor_vset);
			return;
		}

		if (ibat > ibat_th_high) {
			l_di->adaptor_vset -= vstep;
			dc_set_adapter_voltage(l_di->adaptor_vset);
			return;
		}

		if (ibat > ibat_th_high - delta_err) {
			hwlog_info("do nothing\n");
			return;
		}

		/* adjust 1000ma/ratio per step */
		if (l_di->adaptor_iset < (ibat_th_high - 1000) / ratio) {
			l_di->adaptor_iset += 1000 / ratio;
			dc_set_adapter_current(l_di->adaptor_iset);
			return;
		} else if (l_di->adaptor_iset < ibat_th_high / ratio) {
			l_di->adaptor_iset = ibat_th_high / ratio;
			dc_set_adapter_current(l_di->adaptor_iset);
			return;
		} else if (iadp < (ibat_th_high - delta_err) / ratio) {
			l_di->adaptor_vset += vstep;
			dc_set_adapter_voltage(l_di->adaptor_vset);
			return;
		}
	}
}

static void direct_charge_init_priority_inversion_param(
	struct direct_charge_device *di)
{
	di->pri_inversion = false;
}

bool direct_charge_check_priority_inversion(void)
{
	int r_cur;
	struct direct_charge_device *other = NULL;
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return false;

	if (l_di->working_mode == SC4_MODE) {
		sc_get_di(&other);
		if (!other)
			return false;
		if (l_di->sysfs_iin_thermal == l_di->iin_thermal_default || l_di->sysfs_iin_thermal == 0)
			return false;
		if (l_di->sysfs_iin_thermal > l_di->cur_inversion || l_di->sysfs_iin_thermal > l_di->cur_ibat_th_high)
			return false;
		if (other->sysfs_iin_thermal && l_di->sysfs_iin_thermal >= other->sysfs_iin_thermal)
			return false;
		if (l_di->max_adaptor_cur && l_di->sysfs_iin_thermal > l_di->max_adaptor_cur)
			return false;
		if (!l_di->cc_cable_detect_ok && l_di->sysfs_iin_thermal > other->max_current_for_nonstd_cable)
			return false;
		r_cur = dc_resist_handler(SC_MODE, abs(other->full_path_resistance));
		if (r_cur && l_di->sysfs_iin_thermal > r_cur)
			return false;
		if (((direct_charge_get_local_mode() & l_di->adapter_type) &
			SC_MODE) && !other->dc_err_report_flag) {
			hwlog_info("trigger inversion, sc4 switch to sc\n");
			l_di->pri_inversion = true;
			other->pri_inversion = false;
			return true;
		}
	} else if (l_di->working_mode == SC_MODE) {
		lvc_get_di(&other);
		if (!other)
			return false;
		if (l_di->sysfs_iin_thermal == l_di->iin_thermal_default || l_di->sysfs_iin_thermal == 0)
			return false;
		if (l_di->sysfs_iin_thermal > l_di->cur_inversion || l_di->sysfs_iin_thermal > l_di->cur_ibat_th_high)
			return false;
		if (other->sysfs_iin_thermal && l_di->sysfs_iin_thermal >= other->sysfs_iin_thermal)
			return false;
		if (l_di->max_adaptor_cur && l_di->sysfs_iin_thermal > l_di->max_adaptor_cur)
			return false;
		if (!l_di->cc_cable_detect_ok && l_di->sysfs_iin_thermal > other->max_current_for_nonstd_cable)
			return false;
		r_cur = dc_resist_handler(LVC_MODE, abs(other->full_path_resistance));
		if (r_cur && l_di->sysfs_iin_thermal > r_cur)
			return false;
		if (((direct_charge_get_local_mode() & l_di->adapter_type) &
			LVC_MODE) && !other->dc_err_report_flag) {
			hwlog_info("trigger inversion, sc switch to lvc\n");
			l_di->pri_inversion = true;
			other->pri_inversion = false;
			return true;
		}
	} else if (l_di->working_mode == LVC_MODE) {
		if (l_di->local_mode & SC4_MODE)
			sc4_get_di(&other);
		else
			sc_get_di(&other);
		if (!other)
			return false;
		if (!other->pri_inversion)
			return false;
		if (other->sysfs_iin_thermal <= other->cur_inversion)
			return false;
		if (l_di->cur_ibat_th_high >= other->sysfs_iin_thermal)
			return false;
		if (!other->dc_err_report_flag) {
			hwlog_info("trigger inversion, lvc switch to sc\n");
			l_di->pri_inversion = true;
			other->pri_inversion = false;
			return true;
		}
	}
	return false;
}

bool direct_charge_is_priority_inversion(void)
{
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	if ((!lvc_di && !sc_di) || (!lvc_di && !sc4_di) || (!sc_di && !sc4_di))
		return false;

	if (lvc_di && lvc_di->pri_inversion)
		return true;
	if (sc_di && sc_di->pri_inversion)
		return true;
	if (sc4_di && sc4_di->pri_inversion)
		return true;

	return false;
}

void direct_charge_update_charge_info(void)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di || l_di->stop_charging_flag_error)
		return;

	if (l_di->multi_ic_mode_para.support_multi_ic) {
		l_di->curr_info.channel_num = CHARGE_PATH_MAX_NUM;
	} else {
		dc_update_single_info(l_di->working_mode, &l_di->curr_info, l_di->comp_para.vbat_comp);
		return;
	}

	if (l_di->cur_mode != CHARGE_MULTI_IC)
		return;

	dc_update_multi_info(l_di->working_mode, &l_di->curr_info, l_di->comp_para.vbat_comp);
}

int direct_charge_fault_notifier_call(struct notifier_block *nb,
	unsigned long event, void *data)
{
	struct direct_charge_device *di = NULL;
	unsigned int stage = direct_charge_get_stage_status();

	if (!nb) {
		hwlog_err("nb is null\n");
		return NOTIFY_OK;
	}

	di = container_of(nb, struct direct_charge_device, fault_nb);
	if (!di) {
		hwlog_err("di is null\n");
		return NOTIFY_OK;
	}

	if ((stage < DC_STAGE_SECURITY_CHECK) || (stage == DC_STAGE_CHARGE_DONE)) {
		hwlog_err("ignore notifier when not in direct charging\n");
		return NOTIFY_OK;
	}

	di->charge_fault = event;
	di->fault_data = (struct nty_data *)data;
	schedule_work(&di->fault_work);

	return NOTIFY_OK;
}

int direct_charge_start_charging(void)
{
	int t;
	int msecs;
	struct direct_charge_device *l_di = direct_charge_get_di();

	hwlog_info("start_charging\n");

	if (!l_di)
		return -1;

	direct_charge_set_start_time();

	/* clear charge curr info */
	memset(&l_di->curr_info, 0, sizeof(l_di->curr_info));

	/* first: send charging event */
	charge_send_uevent(VCHRG_START_AC_CHARGING_EVENT);

	/* second: request wake lock */
	power_wakeup_lock(l_di->charging_lock, false);

	/* third: calculate first cc stage timeout */
	hwlog_info("first_cc_stage time=%d\n",
		l_di->first_cc_stage_timer_in_min);
	if (l_di->first_cc_stage_timer_in_min) {
		/* 8a maximum 5 min, ap will not suspend, so use jiffies */
		msecs = l_di->first_cc_stage_timer_in_min * 60 * MSEC_PER_SEC;
		l_di->first_cc_stage_timeout = jiffies +
			msecs_to_jiffies(msecs);
	}

	/* max power time */
	direct_charge_get_max_power_para(l_di);
	direct_charge_select_comp_param(l_di);

	/* fourth: select charging voltage parameter */
	direct_charge_select_charging_volt_param();

	/* fifth: select charging stage */
	direct_charge_select_charging_stage();

	/* sixth: execute basp policy */
	direct_charge_soh_policy();

	if (l_di->cur_stage == 0) {
		/* 500: unit is 500ma */
		l_di->volt_para[0].cur_th_high -= l_di->sc_conv_ocp_count * 500;
		hwlog_info("sc_conv_ocp_count=%d, cur_th_high[0]=%d\n",
			l_di->sc_conv_ocp_count,
			l_di->volt_para[0].cur_th_high);
	}

	/* seventh: select charging parameter */
	direct_charge_select_charging_param();

	/* eighth: config watchdog */
	if (dcm_config_ic_watchdog(l_di->working_mode, CHARGE_MULTI_IC,
		WATCHDOG_TIMEOUT)) {
		multi_ic_check_set_ic_error_flag(l_di->cur_mode, &l_di->multi_ic_mode_para);
		return -1;
	}
	l_di->can_stop_kick_wdt = 0;

	adapter_test_set_result(l_di->adaptor_test_result_type,
		AT_PROTOCOL_FINISH_SUCC);
	direct_charge_set_stage_status(DC_STAGE_CHARGING);

	direct_charge_init_priority_inversion_param(l_di);

	/* ninth: start charging control timer */
	t = l_di->charge_control_interval;
	hrtimer_start(&l_di->control_timer,
		ktime_set(t / MSEC_PER_SEC, (t % MSEC_PER_SEC) * USEC_PER_SEC),
		HRTIMER_MODE_REL);

	/* tenth: start threshold calculation timer */
	t = l_di->threshold_caculation_interval;
	hrtimer_start(&l_di->calc_thld_timer,
		ktime_set(t / MSEC_PER_SEC, (t % MSEC_PER_SEC) * USEC_PER_SEC),
		HRTIMER_MODE_REL);

	/* eleventh: start kick watchdog timer */
	t = KICK_WATCHDOG_TIME;
	hrtimer_start(&l_di->kick_wtd_timer,
		ktime_set(t / MSEC_PER_SEC, (t % MSEC_PER_SEC) * USEC_PER_SEC),
		HRTIMER_MODE_REL);
	return 0;
}

void set_direct_flag(int flag)
{
	struct direct_charge_device *l_di = direct_charge_get_di();

	if (!l_di)
		return;

	l_di->hv_flag = flag;
}

void cam_stop_charging(void)
{
	int ret;
	struct direct_charge_device *l_di = direct_charge_get_di();
	unsigned int scp_adp_mode = ADAPTER_SUPPORT_UNDEFINED;

	if (!l_di || !l_di->cam_flash_stop)
		return;

	hwlog_info("cam stop charging enter\n");
	adapter_get_support_mode(ADAPTER_PROTOCOL_SCP, &scp_adp_mode);
	if ((scp_adp_mode & ADAPTER_SUPPORT_SC) != ADAPTER_SUPPORT_SC)
		return;

	ret = dcm_enable_ic(l_di->working_mode, l_di->cur_mode, 0);
	if (ret)
		hwlog_err("ls enable fail\n");

	if (direct_charge_get_stage_status() >= DC_STAGE_ADAPTER_DETECT) {
		dc_set_adapter_voltage(5000);
		ret = adapter_set_output_mode(ADAPTER_PROTOCOL_SCP, 0);
		if (ret)
			hwlog_err("adaptor reset vol fail\n");
	}

	if (!strstr(saved_command_line, "androidboot.swtype=factory")) {
		if (l_di->use_force_sleep)
			charge_enable_force_sleep(HIZ_MODE_DISABLE);
		else
			charge_set_hiz_enable(HIZ_MODE_DISABLE);
	}
	hwlog_info("cam stop charging end\n");
}

static bool direct_charge_adapter_plugout_msleep_exit(void)
{
	unsigned int type = charge_get_charger_type();

	if (type == CHARGER_REMOVED)
		return true;

	return false;
}

void direct_charge_stop_charging(void)
{
	int i, ret;
	int vbus = 0;
	int vbat = 0;
	int vadp = 0;
	bool to_set_default = false;
	struct direct_charge_device *lvc_di = NULL;
	struct direct_charge_device *sc_di = NULL;
	struct direct_charge_device *sc4_di = NULL;
	struct direct_charge_device *l_di = direct_charge_get_di();

	hwlog_info("stop_charging\n");

	if (!l_di)
		return;

	lvc_get_di(&lvc_di);
	sc_get_di(&sc_di);
	sc4_get_di(&sc4_di);

	if (!lvc_di && !sc_di && !sc4_di) {
		hwlog_info("local not support direct_charge");
		return;
	}

	if (l_di->stop_charging_flag_error &&
		l_di->hv_flag == 0) /* ignore error */
		l_di->error_cnt += 1;

	if (l_di->stop_charging_flag_error ||
		l_di->scp_stop_charging_flag_info ||
		(l_di->sysfs_enable_charger == 0) ||
		l_di->force_disable) {
		if (!power_cmdline_is_factory_mode())
			direct_charge_set_stage_status(DC_STAGE_DEFAULT);
		else
			to_set_default = true;
	} else if (l_di->pri_inversion) {
		hwlog_info("pri_inversion,set state in function exit\n");
	} else {
		direct_charge_set_stage_status(DC_STAGE_CHARGE_DONE);
	}

	if (wltx_need_disable_wired_dc())
		direct_charge_set_disable_flags(DC_SET_DISABLE_FLAGS,
			DC_DISABLE_WIRELESS_TX);

	dc_preparation_before_stop();

	if (dcm_enable_ic(l_di->working_mode, l_di->cur_mode, 0))
		hwlog_err("ls enable fail\n");

	if (dcm_use_two_stage() && (l_di->working_mode == SC4_MODE)) {
		dc_get_adapter_voltage(&vadp);
		if (vadp > ADAPTER_9V * POWER_MV_PER_V) {
			dc_set_adapter_voltage(ADAPTER_9V * POWER_MV_PER_V);
			power_msleep(DT_MSLEEP_100MS, 0, NULL);
		}
		dcm_enable_first_level_ic(CP_TYPE_AUX, false);
		dcm_set_first_level_ic_mode(DC_IC_BP_MODE);
		wired_chsw_set_wired_channel(WIRED_CHANNEL_BUCK, WIRED_CHANNEL_RESTORE);
		power_msleep(DT_MSLEEP_300MS, DT_MSLEEP_50MS, direct_charge_adapter_plugout_msleep_exit);
	}
	ret = dc_set_adapter_default();
	if (ret)
		hwlog_err("set adapter default state fail\n");

	l_di->can_stop_kick_wdt = 1;

	if (l_di->stop_charging_sleep_enable)
		power_msleep(DT_MSLEEP_15S, DT_MSLEEP_50MS, direct_charge_adapter_plugout_msleep_exit);

	if (l_di->scp_work_on_charger) {
		dc_adapter_protocol_power_supply(DISABLE);
		if (l_di->use_force_sleep)
			charge_enable_force_sleep(HIZ_MODE_DISABLE);
		else
			charge_set_hiz_enable(HIZ_MODE_DISABLE);
	}

	/* 1: enable discharge with device */
	if (dcm_discharge_ic(l_di->working_mode,  l_di->cur_mode, 1))
		hwlog_err("ls enable discharge fail\n");
	else
		hwlog_info("ls enable discharge succ\n");

	/* switch to normal charging path */
	dc_switch_charging_path(WIRED_CHANNEL_BUCK);

	/* 0: disable discharge with device */
	if (dcm_discharge_ic(l_di->working_mode, l_di->cur_mode, 0))
		hwlog_err("ls disable discharge fail\n");
	else
		hwlog_info("ls disable discharge succ\n");

	msleep(DT_MSLEEP_20MS);
	if (dcm_use_two_stage() && (l_di->working_mode == SC4_MODE)) {
		dcm_enable_first_level_ic(CP_TYPE_MAIN, false);
		if (dcm_enable_irq(l_di->working_mode, CHARGE_IC_MAIN, DC_IRQ_ENABLE))
			hwlog_err("enable irq fail\n");
	}

	/* judging whether the adapter is disconnect */
	direct_charge_get_vbus_vbat(&vbus, &vbat);
	if ((vbus < VBUS_ON_THRESHOLD) ||
		(vbat - vbus) > VBAT_VBUS_DIFFERENCE) {
		hwlog_info("direct charger disconnect\n");

		if (!power_cmdline_is_factory_mode())
			direct_charge_set_disable_flags(DC_CLEAR_DISABLE_FLAGS,
				DC_DISABLE_SYS_NODE);

		direct_charge_set_disable_flags(DC_CLEAR_DISABLE_FLAGS,
			DC_DISABLE_WIRELESS_TX);
		direct_charge_reset_para_in_stop(lvc_di);
		direct_charge_reset_para_in_stop(sc_di);
		direct_charge_reset_para_in_stop(sc4_di);

		wired_disconnect_send_icon_uevent();
	}

	ret = dcm_exit_ic(l_di->working_mode, CHARGE_MULTI_IC);
	if (ret)
		hwlog_err("ls exit fail\n");

	if (dcm_exit_batinfo(l_di->working_mode, l_di->cur_mode))
		hwlog_err("bi exit fail\n");

	ret = dc_reset_operate(DC_RESET_MASTER);
	if (ret)
		hwlog_err("soft reset master fail\n");

	if (to_set_default)
		direct_charge_set_stage_status(DC_STAGE_DEFAULT);

	/* cancel threshold timer and work */
	hrtimer_cancel(&l_di->calc_thld_timer);
	cancel_work_sync(&l_di->calc_thld_work);

	/* cancel watchdog timer and work */
	dcm_config_ic_watchdog(l_di->working_mode, CHARGE_MULTI_IC, 0); /* 0:wdt disable */
	hrtimer_cancel(&l_di->kick_wtd_timer);
	cancel_work_sync(&l_di->kick_wtd_work);

	/* reset parameter and release resource */
	dc_close_aux_wired_channel();
	direct_charge_set_stop_charging_flag(0);
	dc_mmi_set_test_flag(false);
	l_di->scp_stop_charging_flag_info = 0;
	l_di->cur_stage = 0;
	l_di->pre_stage = 0;
	l_di->vbat = 0;
	l_di->ibat = 0;
	l_di->vadapt = 0;
	l_di->iadapt = 0;
	l_di->ls_vbus = 0;
	l_di->ls_ibus = 0;
	l_di->compensate_v = 0;
	l_di->ibat_abnormal_cnt = 0;
	l_di->max_adaptor_cur = 0;
	l_di->multi_ic_check_info.force_single_path_flag = false;
	memset(&l_di->limit_max_pwr, 0, sizeof(l_di->limit_max_pwr));
	power_wakeup_unlock(l_di->charging_lock, false);
	l_di->scp_stop_charging_complete_flag = 1;
	l_di->adaptor_test_result_type = AT_TYPE_OTHER;
	l_di->working_mode = UNDEFINED_MODE;
	l_di->hv_flag = 0;
	l_di->cur_mode = CHARGE_IC_MAIN;
	if (l_di->multi_ic_check_info.limit_current < 0)
		l_di->multi_ic_check_info.limit_current = l_di->iin_thermal_default;
	for (i = 0; i < DC_MODE_TOTAL; i++)
		l_di->rt_test_para[i].rt_test_result = false;

	power_event_bnc_notify(POWER_BNT_DC, POWER_NE_DC_STOP_CHARGE, NULL);
	if (l_di->pri_inversion || direct_charge_check_switch_sc4_to_sc_mode()) {
		if (l_di->pri_inversion)
			direct_charge_set_stage_status(DC_STAGE_DEFAULT);
		charge_request_charge_monitor();
	}
}

unsigned int huawei_get_basp_vterm_dec(void)
{
	struct direct_charge_device *di = direct_charge_get_di();

	if (!di)
		return -1;

	return di->vterm_dec;
}

void huawei_set_basp_vterm_dec(int dec)
{
	struct direct_charge_device *di = direct_charge_get_di();

	if (!di)
		return;

	if (dec < BASP_VTERM_DEC_MIN || dec > BASP_VTERM_DEC_MAX) {
		hwlog_err("%s invalid vterm_dec:%d\n", __func__, dec);
		return;
	}

	hwlog_info("%s set basp vterm_dec: %d\n", __func__, dec);
	di->vterm_dec = dec;
	return;
}

unsigned int huawei_get_basp_ichg_ratio(void)
{
	struct direct_charge_device *di = direct_charge_get_di();

	if (!di)
		return -1;

	return di->ichg_ratio;
}

void huawei_set_basp_ichg_ratio(int ratio)
{
	struct direct_charge_device *di = direct_charge_get_di();

	if (!di)
		return;
	/* 10: convert to 100 percent */
	ratio *= 10;
	if (ratio < BASP_ICHG_RATIO_MIN || ratio > BASP_ICHG_RATIO_MAX) {
		hwlog_err("%s invalid ichg_ratio: %d\n", __func__, ratio);
		return;
	}

	hwlog_info("%s set basp ichg_ratio: %d\n", __func__, ratio);
	di->ichg_ratio = ratio;
	return;
}
