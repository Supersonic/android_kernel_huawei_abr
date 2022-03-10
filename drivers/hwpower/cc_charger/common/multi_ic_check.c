// SPDX-License-Identifier: GPL-2.0
/*
 * multi_ic_check.c
 *
 * multi ic check interface for power module
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

#include <chipset_common/hwpower/direct_charge/multi_ic_check.h>
#include <chipset_common/hwpower/battery/battery_temp.h>
#include <chipset_common/hwpower/common_module/power_dsm.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_time.h>
#include <chipset_common/hwpower/charger/charger_common_interface.h>
#include <chipset_common/hwpower/hardware_monitor/btb_check.h>
#include <huawei_platform/hwpower/common_module/power_platform.h>

#define MULTI_ERR_STRING_LEN  256
#define HIGH_POWER_CUR_TH     800

#define HWLOG_TAG multi_ic_check
HWLOG_REGIST();

static void multi_ic_parse_ratio_para(struct device_node *np,
	struct multi_ic_ratio_para *info, const char *name)
{
	int row, col, len;
	int data[MULTI_IC_RATIO_PARA_LEVEL * MULTI_IC_RATIO_ERR_MAX] = { 0 };

	len = power_dts_read_string_array(power_dts_tag(HWLOG_TAG), np,
		name, data, MULTI_IC_RATIO_PARA_LEVEL, MULTI_IC_RATIO_ERR_MAX);
	if (len < 0)
		return;

	for (row = 0; row < len / MULTI_IC_RATIO_ERR_MAX; row++) {
		col = row * MULTI_IC_RATIO_ERR_MAX + MULTI_IC_RATIO_ERR_CHECK_CNT;
		info[row].error_cnt = data[col];
		col = row * MULTI_IC_RATIO_ERR_MAX + MULTI_IC_RATIO_MIN;
		info[row].ratio_min = data[col];
		col = row * MULTI_IC_RATIO_ERR_MAX + MULTI_IC_RATIO_MAX;
		info[row].ratio_max = data[col];
		col = row * MULTI_IC_RATIO_ERR_MAX + MULTI_IC_RATIO_DMD_LEVEL;
		info[row].dmd_level = data[col];
		col = row * MULTI_IC_RATIO_ERR_MAX + MULTI_IC_RATIO_LIMIT_CURRENT;
		info[row].limit_current = data[col];
	}
}

static void multi_ic_parse_diff_para(struct device_node *np,
	struct multi_ic_diff_para *info, const char *name)
{
	int row, col, len;
	int data[MULTI_IC_DIFF_PARA_LEVEL * MULTI_IC_DIFF_MAX] = { 0 };

	len = power_dts_read_string_array(power_dts_tag(HWLOG_TAG), np,
		name, data, MULTI_IC_DIFF_PARA_LEVEL, MULTI_IC_DIFF_MAX);
	if (len < 0)
		return;

	for (row = 0; row < len / MULTI_IC_DIFF_MAX; row++) {
		col = row * MULTI_IC_DIFF_MAX + MULTI_IC_DIFF_CHECK_CNT;
		info[row].error_cnt = data[col];
		col = row * MULTI_IC_DIFF_MAX + MULTI_IC_DIFF_VALUE;
		info[row].diff_val = data[col];
		col = row * MULTI_IC_DIFF_MAX + MULTI_IC_DIFF_DMD_LEVEL;
		info[row].dmd_level = data[col];
		col = row * MULTI_IC_DIFF_MAX + MULTI_IC_DIFF_LIMIT_CURRENT;
		info[row].limit_current = data[col];
	}
}

static void multi_ic_parse_thre_para(struct device_node *np,
	struct multi_ic_thre_para *info, const char *name)
{
	int row, col, len;
	int data[CHARGE_PATH_MAX_NUM * MULTI_IC_THRE_MAX] = { 0 };

	len = power_dts_read_string_array(power_dts_tag(HWLOG_TAG), np,
		name, data, CHARGE_PATH_MAX_NUM, MULTI_IC_THRE_MAX);
	if (len < 0)
		return;

	for (row = 0; row < len / MULTI_IC_THRE_MAX; row++) {
		col = row * MULTI_IC_THRE_MAX + MULTI_IC_THRE_IBUS_LTH;
		info[row].ibus_lth = data[col];
		col = row * MULTI_IC_THRE_MAX + MULTI_IC_THRE_VBAT_HTH;
		info[row].vbat_hth = data[col];
	}
}

void multi_ic_parse_check_para(struct device_node *np,
	struct multi_ic_check_para *info)
{
	if (!np || !info)
		return;

	multi_ic_parse_thre_para(np, info->thre_para, "thre_para");
	multi_ic_parse_ratio_para(np, info->curr_ratio, "current_ratio");
	multi_ic_parse_diff_para(np, info->vbat_error, "vbat_error");
	multi_ic_parse_diff_para(np, info->tbat_error, "tbat_error");
}

static int multi_ic_check_get_dmd_num(int dmd_level)
{
	switch (dmd_level) {
	case MULTI_IC_DMD_LEVEL_INFO:
		return DSM_MULTI_CHARGE_CURRENT_RATIO_INFO;
	case MULTI_IC_DMD_LEVEL_WARNING:
		return DSM_MULTI_CHARGE_CURRENT_RATIO_WARNING;
	case MULTI_IC_DMD_LEVEL_ERROR:
		return DSM_MULTI_CHARGE_CURRENT_RATIO_ERROR;
	default:
		return -EPERM;
	}
}

static void multi_ic_dsm_report_dmd(int working_mode, int type, int dmd_level,
	struct multi_ic_check_para *info, const char *buf, int buf_size)
{
	char tmp_buf[MULTI_ERR_STRING_LEN] = { 0 };
	int ibus_main = 0;
	int ibus_aux = 0;
	int tbat_main = 0;
	int tbat_aux = 0;
	int ibus_ratio;
	int dmd_num;

	if (!buf || (buf_size >= MULTI_ERR_STRING_LEN))
		return;

	if ((dmd_level < MULTI_IC_DMD_LEVEL_BEGIN) ||
		(dmd_level >= MULTI_IC_DMD_LEVEL_END))
		return;

	if (info->report_info[dmd_level])
		return;

	dmd_num = multi_ic_check_get_dmd_num(dmd_level);
	if (dmd_num < 0)
		return;

	(void)dcm_get_ic_ibus(working_mode, CHARGE_IC_MAIN, &ibus_main);
	(void)dcm_get_ic_ibus(working_mode, CHARGE_IC_AUX, &ibus_aux);

	if (!ibus_aux)
		return;
	/* multiplied by 10 to calc ratio */
	ibus_ratio = ibus_main * 10 / ibus_aux;
	(void)bat_temp_get_temperature(BTB_TEMP_0, &tbat_main);
	(void)bat_temp_get_temperature(BTB_TEMP_1, &tbat_aux);

	snprintf(tmp_buf, sizeof(tmp_buf),
		"%s error_type = %d, Ibus_ratio = %d, capacity = %d, curr = %dmA,"
		"avg_curr = %dmA, Ibus1 = %dmA, Ibus2 = %dmA, batt_volt1 = %dmV, "
		"batt_volt2 = %dmV,temp1 = %d, temp2 = %d, charger_type = %u\n",
		buf, type, ibus_ratio, power_platform_get_battery_ui_capacity(),
		-power_platform_get_battery_current(),
		charge_get_battery_current_avg(), ibus_main, ibus_aux,
		dcm_get_ic_vbtb_with_comp(working_mode, CHARGE_IC_MAIN, info->vbat_comp),
		dcm_get_ic_vbtb_with_comp(working_mode, CHARGE_IC_AUX, info->vbat_comp),
		tbat_main, tbat_aux, charge_get_charger_type());

	hwlog_info("%s\n", tmp_buf);
	power_dsm_report_dmd(POWER_DSM_BATTERY, dmd_num, tmp_buf);
	info->report_info[dmd_level] = 1; /* report flag */
}

static void multi_ic_check_update_limit_curr(struct multi_ic_check_para *info,
	int limit_current)
{
	if ((limit_current != 0) && (info->limit_current > limit_current))
		info->limit_current = limit_current;
}

static int multi_ic_check_threshold(int working_mode, struct multi_ic_check_para *info,
	int volt_ratio)
{
	int i;
	int vbat[CHARGE_PATH_MAX_NUM];
	int ibus[CHARGE_PATH_MAX_NUM];
	int tmp_ibat = 0;

	if (!info || !volt_ratio)
		return -EINVAL;

	for (i = 0; i < CHARGE_PATH_MAX_NUM; i++) {
		vbat[i] = dcm_get_ic_vbtb_with_comp(working_mode, BIT(i), info->vbat_comp);
		if ((vbat[i] < MULTI_IC_CHECK_VBAT_LOW_TH) ||
			(vbat[i] > MULTI_IC_CHEKC_VBAT_HIGH_TH)) {
			hwlog_err("vbat is over limit, vbat[%d]=%d\n", i, vbat[i]);
			return -EPERM;
		}
		if (info->thre_para[i].vbat_hth && (vbat[i] > info->thre_para[i].vbat_hth))
			goto end;

		dcm_get_ic_ibus(working_mode, BIT(i), &ibus[i]);
		dcm_get_path_max_ibat(working_mode, BIT(i), &tmp_ibat);
		if (info->thre_para[i].ibus_lth && (ibus[i] < info->thre_para[i].ibus_lth))
			goto end;
		if (tmp_ibat && (ibus[i] > tmp_ibat / volt_ratio))
			goto end;
	}

	return 0;
end:
	hwlog_info("set force_single_path_flag true\n");
	info->force_single_path_flag = true;
	return 0;
}

static void multi_ic_check_info(int working_mode, struct multi_ic_check_para *info)
{
	int ibat = 0;
	int i;
	int ibus_main = 0;
	int ibus_aux = 0;
	int ibus_ratio;
	char tmp_buf[MULTI_ERR_STRING_LEN] = { 0 };
	struct multi_ic_ratio_para *info_para = NULL;

	if (info->report_info[MULTI_IC_DMD_LEVEL_INFO])
		return;

	(void)dcm_get_ic_ibat(working_mode, CHARGE_IC_MAIN, &ibat);
	if (ibat < info->ibat_th)
		return;

	(void)dcm_get_ic_ibus(working_mode, CHARGE_IC_MAIN, &ibus_main);
	(void)dcm_get_ic_ibus(working_mode, CHARGE_IC_AUX, &ibus_aux);

	if (!ibus_aux)
		return;

	for (i = 0; i < MULTI_IC_RATIO_PARA_LEVEL; i++) {
		if ((info->curr_ratio[i].dmd_level == MULTI_IC_DMD_LEVEL_INFO) &&
			(info->curr_ratio[i].ratio_max != 0)) {
			info_para = &info->curr_ratio[i];
			break;
		}
	}

	if (!info_para)
		return;
	/* multiplied by 10 to calc ratio */
	ibus_ratio = ibus_main * 10 / ibus_aux;

	if ((ibus_ratio <= (int)info_para->ratio_max) &&
		(ibus_ratio >= (int)info_para->ratio_min)) {
		snprintf(tmp_buf, sizeof(tmp_buf),
			"Ibus_ratio = %d, Ibus1 = %dmA, Ibus2 = %dmA\n",
			ibus_ratio, ibus_main, ibus_aux);
		multi_ic_dsm_report_dmd(working_mode, MULTI_IC_ERROR_TYPE_INFO,
			MULTI_IC_DMD_LEVEL_INFO, info, tmp_buf, strlen(tmp_buf));
	}
}

static void multi_ic_check_ibus(int working_mode, struct multi_ic_check_para *info)
{
	int ibus_main = 0;
	int ibus_aux = 0;
	int ibus_ratio;
	int i;
	char tmp_buf[MULTI_ERR_STRING_LEN] = { 0 };
	u32 cur_time = power_get_current_kernel_time().tv_sec;

	if (cur_time - info->multi_ic_start_time < MULTI_IC_CHECK_TIMEOUT)
		return;

	(void)dcm_get_ic_ibus(working_mode, CHARGE_IC_MAIN, &ibus_main);
	(void)dcm_get_ic_ibus(working_mode, CHARGE_IC_AUX, &ibus_aux);

	if (!ibus_aux)
		return;
	/* multiplied by 10 to calc ratio */
	ibus_ratio = ibus_main * 10 / ibus_aux;

	for (i = 0; i < MULTI_IC_RATIO_PARA_LEVEL; i++) {
		if (info->ibus_error_num[i] >= info->curr_ratio[i].error_cnt)
			continue;

		if ((ibus_ratio <= (int)info->curr_ratio[i].ratio_max) &&
			(ibus_ratio >= (int)info->curr_ratio[i].ratio_min))
			continue;

		info->ibus_error_num[i]++;
		hwlog_info("check ibus error, ibus_main=%d, ibus_aux=%d, ibus_ratio=%d, cnt=%u\n",
			ibus_main, ibus_aux, ibus_ratio, info->ibus_error_num[i]);
		if (info->ibus_error_num[i] == info->curr_ratio[i].error_cnt) {
			snprintf(tmp_buf, sizeof(tmp_buf),
				"Ibus_ratio = %d, Ibus1 = %dmA, Ibus2 = %dmA\n",
				ibus_ratio, ibus_main, ibus_aux);
			multi_ic_dsm_report_dmd(working_mode, MULTI_IC_ERROR_TYPE_IBUS,
				info->curr_ratio[i].dmd_level, info, tmp_buf, strlen(tmp_buf));
			multi_ic_check_update_limit_curr(info, info->curr_ratio[i].limit_current);
		}
		return;
	}
}

static void multi_ic_check_vbat(int working_mode, struct multi_ic_check_para *info)
{
	int vbat_main;
	int vbat_aux;
	unsigned int delta_volt;
	int i;
	char tmp_buf[MULTI_ERR_STRING_LEN] = { 0 };

	vbat_main = dcm_get_ic_vbtb_with_comp(working_mode, CHARGE_IC_MAIN, info->vbat_comp);
	vbat_aux = dcm_get_ic_vbtb_with_comp(working_mode, CHARGE_IC_AUX, info->vbat_comp);
	delta_volt = vbat_main > vbat_aux ? (vbat_main - vbat_aux) : (vbat_aux - vbat_main);

	for (i = 0; i < MULTI_IC_DIFF_PARA_LEVEL; i++) {
		if (info->vbat_error_num[i] >= info->vbat_error[i].error_cnt)
			continue;

		if (delta_volt <= info->vbat_error[i].diff_val)
			continue;

		info->vbat_error_num[i]++;
		hwlog_info("check delta_vbat error, main_vbat=%d, aux_vbat=%d, cnt=%u\n",
			vbat_main, vbat_aux, info->vbat_error_num[i]);
		if (info->vbat_error_num[i] == info->vbat_error[i].error_cnt) {
			snprintf(tmp_buf, sizeof(tmp_buf),
				"delta_vbat = %u, main_vbat = %d, aux_vbat = %d\n",
				delta_volt, vbat_main, vbat_aux);
			multi_ic_dsm_report_dmd(working_mode, MULTI_IC_ERROR_TYPE_VBAT,
				info->vbat_error[i].dmd_level, info, tmp_buf, strlen(tmp_buf));
			multi_ic_check_update_limit_curr(info,
				info->vbat_error[i].limit_current);
		}
		return;
	}
}

static void multi_ic_check_tbat(int working_mode, struct multi_ic_check_para *info)
{
	int tbat_main = 0;
	int tbat_aux = 0;
	unsigned int delta_temp;
	int i;
	char tmp_buf[MULTI_ERR_STRING_LEN] = { 0 };

	(void)bat_temp_get_rt_temperature(BTB_TEMP_0, &tbat_main);
	(void)bat_temp_get_rt_temperature(BTB_TEMP_1, &tbat_aux);
	delta_temp = tbat_main > tbat_aux ? (tbat_main - tbat_aux) : (tbat_aux - tbat_main);

	for (i = 0; i < MULTI_IC_DIFF_PARA_LEVEL; i++) {
		if (info->tbat_error_num[i] >= info->tbat_error[i].error_cnt)
			continue;
		if (delta_temp <= info->tbat_error[i].diff_val)
			continue;

		info->tbat_error_num[i]++;
		hwlog_info("check delta_tbat error, main_tbat=%d, aux_tbat=%d, cnt=%u\n",
			tbat_main, tbat_aux, info->tbat_error_num[i]);
		if (info->tbat_error_num[i] == info->tbat_error[i].error_cnt) {
			snprintf(tmp_buf, sizeof(tmp_buf),
				"delta_tbat = %u, main_tbat = %d, aux_tbat = %d\n",
				delta_temp, tbat_main, tbat_aux);
			multi_ic_dsm_report_dmd(working_mode, MULTI_IC_ERROR_TYPE_VBAT,
				info->tbat_error[i].dmd_level, info, tmp_buf, strlen(tmp_buf));
			multi_ic_check_update_limit_curr(info,
				info->tbat_error[i].limit_current);
		}
		return;
	}
}

static bool multi_ic_check_disable_status(int working_mode, int mode, int volt_ratio)
{
	int i;
	int ibus[CHARGE_PATH_MAX_NUM];
	int total_ibus = 0;
	int total_ibat = 0;

	for (i = 0; i < CHARGE_PATH_MAX_NUM; i++) {
		dcm_get_ic_ibus(working_mode, BIT(i), &ibus[i]);
		total_ibus += ibus[i];
	}

	dcm_get_total_ibat(working_mode, mode, &total_ibat);
	if ((total_ibus * volt_ratio - total_ibat) > HIGH_POWER_CUR_TH) {
		hwlog_info("high power scenario, skip dmd report\n");
		return true;
	}

	return false;
}

int mulit_ic_check(int working_mode, int mode, struct multi_ic_check_para *info, int volt_ratio)
{
	u32 cur_time = power_get_current_kernel_time().tv_sec;

	if (!info)
		return -EPERM;

	if (mode != CHARGE_MULTI_IC)
		return 0;

	if (cur_time - info->multi_ic_start_time < MULTI_IC_CHECK_TIMEOUT)
		return 0;

	if (multi_ic_check_threshold(working_mode, info, volt_ratio) < 0)
		return -EPERM;

	if (multi_ic_check_disable_status(working_mode, mode, volt_ratio))
		return 0;

	multi_ic_check_info(working_mode, info);
	multi_ic_check_ibus(working_mode, info);
	multi_ic_check_vbat(working_mode, info);
	multi_ic_check_tbat(working_mode, info);

	if (info->limit_current < 0)
		return -EPERM;

	return 0;
}

static void multi_check_btb_result(int type, struct multi_ic_check_mode_para *para)
{
	u32 result = 0;

	btb_ck_get_result_now(type, &result);
	hwlog_info("btb check begin, type is %d\n", type);
	if (result == 0)
		return;
	if (result & MAIN_BAT_BTB_ERR) {
		para->ic_error_cnt[CHARGE_IC_TYPE_MAIN] = MULTI_IC_CHECK_ERR_CNT_MAX;
		para->ic_error_cnt[CHARGE_IC_TYPE_AUX] = MULTI_IC_CHECK_ERR_CNT_MAX;
		hwlog_info("main btb check fail, can not direct charge\n");
	}
	if (result & AUX_BAT_BTB_ERR) {
		para->ic_error_cnt[CHARGE_IC_TYPE_AUX] = MULTI_IC_CHECK_ERR_CNT_MAX;
		hwlog_info("aux btb check fail\n");
	}
	if (result & BTB_BAT_DIFF_ERR) {
		para->ic_error_cnt[CHARGE_IC_TYPE_AUX] = MULTI_IC_CHECK_ERR_CNT_MAX;
		hwlog_info("btb diff check fail\n");
	}
}

int multi_ic_check_select_tbat_id(struct multi_ic_check_mode_para *para)
{
	int temp_result = 0;

	if (!para || !para->support_multi_ic || !para->support_select_temp)
		return BAT_TEMP_MIXED;

	btb_ck_get_result_now(BTB_TEMP_CHECK, &temp_result);
	switch (temp_result) {
	case MAIN_BAT_BTB_ERR:
		return BTB_TEMP_1;
	case AUX_BAT_BTB_ERR:
		return BTB_TEMP_0;
	default:
		break;
	}

	return BAT_TEMP_MIXED;
}

int multi_ic_check_select_init_mode(struct multi_ic_check_mode_para *para, int *mode)
{
	u32 i;

	if (!para || !mode)
		return -EPERM;

	if (!para->support_multi_ic) {
		*mode = CHARGE_IC_MAIN;
		return 0;
	}

	/* 1st: check btb result */
	multi_check_btb_result(BTB_VOLT_CHECK, para);
	multi_check_btb_result(BTB_TEMP_CHECK, para);
	/* 2nd: check ic error cnt */
	for (i = 0; i < CHARGE_PATH_MAX_NUM; i++) {
		if (para->ic_error_cnt[i] < MULTI_IC_CHECK_ERR_CNT_MAX) {
			*mode = BIT(i);
			return 0;
		}
	}

	hwlog_info("all ic is error, can not enter direct charge\n");
	return -EPERM;
}

void multi_ic_check_set_ic_error_flag(int flag, struct multi_ic_check_mode_para *para)
{
	if (!para)
		return;

	switch (flag) {
	case CHARGE_IC_MAIN:
		if (para->ic_error_cnt[CHARGE_IC_TYPE_MAIN] < MULTI_IC_CHECK_ERR_CNT_MAX)
			para->ic_error_cnt[CHARGE_IC_TYPE_MAIN]++;
		break;
	case CHARGE_IC_AUX:
		if (para->ic_error_cnt[CHARGE_IC_TYPE_AUX] < MULTI_IC_CHECK_ERR_CNT_MAX)
			para->ic_error_cnt[CHARGE_IC_TYPE_AUX]++;
		break;
	default:
		break;
	}

	hwlog_info("[err_flag] main=%d, aux=%d\n", para->ic_error_cnt[CHARGE_IC_TYPE_MAIN],
		para->ic_error_cnt[CHARGE_IC_TYPE_AUX]);
}

int multi_ic_check_ic_status(struct multi_ic_check_mode_para *para)
{
	int i;

	if (!para)
		return -EPERM;

	for (i = 0; i < CHARGE_PATH_MAX_NUM; i++) {
		if (para->ic_error_cnt[i] >= MULTI_IC_CHECK_ERR_CNT_MAX)
			return -EPERM;
	}

	return 0;
}
