/*
 * ti_smartamp.c
 *
 * ti smartpa related definition for adsp misc
 *
 * Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.
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

#include "adsp_misc.h"

#include <huawei_platform/log/hw_log.h>
#include <sound/soc.h>
#include <sound/adsp_misc_interface.h>
#include <linux/module.h>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define HWLOG_TAG adsp_misc
HWLOG_REGIST();

/* Below 3 should be same as in aDSP code */
#define AFE_PARAM_ID_SMARTAMP_DEFAULT 0x10001166
#define AFE_SMARTAMP_MODULE_RX 0x1000C003 /* Rx module */
#define AFE_SMARTAMP_MODULE_TX 0x1000C002 /* Tx module */

#define CAPI_V2_TAS_TX_ENABLE 0x10012D14
#define CAPI_V2_TAS_TX_CFG 0x10012D16
#define CAPI_V2_TAS_RX_ENABLE 0x10012D13
#define CAPI_V2_TAS_RX_CFG 0x10012D15

#define MAX_DSP_PARAM_INDEX 600
#define TAS_PAYLOAD_SIZE 14
#define TAS_GET_PARAM 1
#define TAS_SET_PARAM 0
#define CHANNEL0 1
#define CHANNEL1 2
#define MAX_CHANNELS 2

#define TAS_SA_GET_F0 3810
#define TAS_SA_GET_Q 3811
#define TAS_SA_GET_TV 3812
#define TAS_SA_GET_RE 3813
#define TAS_SA_CALIB_INIT 3814
#define TAS_SA_CALIB_DEINIT 3815
#define TAS_SA_SET_RE 3816
#define TAS_SA_SET_PROFILE 3819
#define TAS_SA_GET_STATUS 3821
#define TAS_SA_SET_TCAL 3823

#define CALIB_START 1
#define CALIB_STOP 2
#define TEST_START 3
#define TEST_STOP 4

#define SLAVE1 0x98
#define SLAVE2 0x9A
#define SLAVE3 0x9C
#define SLAVE4 0x9E

#define TAS_SA_IS_SPL_IDX(x) ((((x) >= 3810) && ((x) < 3899)) ? 1 : 0)
#define LENGTH_1 1
#define TAS_CALC_PARAM_IDX(index, length, channel) \
	((index) | (length << 16) | (channel << 24))

/* Random Numbers is to handle global data corruption */
#define TAS_FALSE 0x01010101
#define TAS_TRUE 0x10101010

static int tisa_afe_algo_ctrl(u8 *user_data, uint32_t param_id,
		uint8_t get_set, uint32_t length, uint32_t module_id)
{
	UNUSED(user_data);
	UNUSED(param_id);
	UNUSED(get_set);
	UNUSED(length);
	UNUSED(module_id);

	return 0;
}

static int tisa_get_current_param(char *data, size_t size, int cmd)
{
	int ret;
	int param_id;

	UNUSED(size);
	switch (cmd) {
	case TAS_SA_GET_RE:
		param_id = TAS_CALC_PARAM_IDX(TAS_SA_GET_RE,
			LENGTH_1, CHANNEL0);
		break;
	case TAS_SA_GET_TV:
		param_id = TAS_CALC_PARAM_IDX(TAS_SA_GET_TV,
			LENGTH_1, CHANNEL0);
		break;
	case TAS_SA_GET_Q:
		param_id = TAS_CALC_PARAM_IDX(TAS_SA_GET_Q,
			LENGTH_1, CHANNEL0);
		break;
	case TAS_SA_GET_F0:
		param_id = TAS_CALC_PARAM_IDX(TAS_SA_GET_F0,
			LENGTH_1, CHANNEL0);
		break;
	default:
		hwlog_err("%s:unsupport type value\n", __func__);
		return -EFAULT;
	}

	ret = tisa_afe_algo_ctrl((u8 *)data, param_id, TAS_GET_PARAM,
			sizeof(uint32_t), AFE_SMARTAMP_MODULE_RX);
	if (ret < 0)
		hwlog_err("%s: Failed to get param, ret=%d", __func__, ret);

	hwlog_info("get param value=0x%x, cmd", (uint32_t)data, cmd);
	return ret;
}


int tas_smartpa_cali_start(struct adsp_misc_priv *pdata)
{
	UNUSED(pdata);
	return 0;
}
EXPORT_SYMBOL(tas_smartpa_cali_start);

int tas_smartpa_cali_stop(struct adsp_misc_priv *pdata)
{
	UNUSED(pdata);
	return 0;
}
EXPORT_SYMBOL(tas_smartpa_cali_stop);

int tas_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL(tas_cali_re_update);

int tas_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;

	UNUSED(pdata);
	if (data == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}
	ret = tisa_get_current_param(data, len, TAS_SA_GET_RE);
	return ret;
}
EXPORT_SYMBOL(tas_get_current_r0);

int tas_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;

	UNUSED(pdata);
	if (data == NULL) {
		pr_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}
	ret = tisa_get_current_param(data, len, TAS_SA_GET_F0);
	return ret;
}
EXPORT_SYMBOL(tas_get_current_f0);

int tas_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;

	UNUSED(pdata);
	if (data == NULL) {
		pr_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}
	ret = tisa_get_current_param(data, len, TAS_SA_GET_TV);
	return ret;
}
EXPORT_SYMBOL(tas_get_current_te);

int tas_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;

	UNUSED(pdata);
	if (data == NULL) {
		pr_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}
	ret = tisa_get_current_param(data, len, TAS_SA_GET_Q);
	return ret;
}
EXPORT_SYMBOL(tas_get_current_q);

int tas_algo_control(struct adsp_misc_priv *pdata, uint32_t enable)
{
	UNUSED(pdata);
	UNUSED(enable);

	return 0;
}
EXPORT_SYMBOL(tas_algo_control);

int tas_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL(tas_smartpa_cal_write);

int tas_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL(tas_smartpa_cal_read);

MODULE_DESCRIPTION("tas smartpa adsp misc driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
