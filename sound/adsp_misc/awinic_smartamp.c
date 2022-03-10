/*
 * awinic_smartamp.c
 *
 * awinic smartpa related definition for adsp misc
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

#include <linux/module.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <huawei_platform/log/hw_log.h>
#include <sound/adsp_misc_interface.h>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define HWLOG_TAG adsp_misc
HWLOG_REGIST();

#define AW_CALI_VALUE_INT_TO_DEC(a) ((a) * 1000 / 4096)
#define AW_CALI_CAL_FACTOR 16

#define AFE_MODULE_ID_AWDSP_TX 0x10013D00
#define AFE_MODULE_ID_AWDSP_RX 0x10013D01
#define AFE_PARAM_ID_AWDSP_RX_SET_ENABLE 0x10013D11
#define AFE_PARAM_ID_AWDSP_TX_SET_ENABLE 0x10013D13
#define AFE_PARAM_ID_AWDSP_RX_PARAMS 0x10013D12

#define AW_MSG_ID_ENABLE_CALI 0x00000001
#define AW_MSG_ID_ENABLE_HMUTE 0x00000002
#define AW_MSG_ID_F0_Q 0x00000003
#define AW_MSG_ID_DIRECT_CUR_FLAG 0x00000006
#define AW_MSG_ID_SPK_STATUS 0x00000007

/* dsp params id */
#define AW_MSG_ID_RX_SET_ENABLE 0x10013D11
#define AW_MSG_ID_PARAMS 0x10013D12
#define AW_MSG_ID_TX_SET_ENABLE 0x10013D13
#define AW_MSG_ID_VMAX_L 0X10013D17
#define AW_MSG_ID_VMAX_R 0X10013D18
#define AW_MSG_ID_CALI_CFG_L 0X10013D19
#define AW_MSG_ID_CALI_CFG_R 0x10013d1A
#define AW_MSG_ID_RE_L 0x10013d1B
#define AW_MSG_ID_RE_R 0X10013D1C
#define AW_MSG_ID_NOISE_L 0X10013D1D
#define AW_MSG_ID_NOISE_R 0X10013D1E
#define AW_MSG_ID_F0_L 0X10013D1F
#define AW_MSG_ID_F0_R 0X10013D20
#define AW_MSG_ID_REAL_DATA_L 0X10013D21
#define AW_MSG_ID_REAL_DATA_R 0X10013D22
#define AFE_MSG_ID_MSG_0 0X10013D2A
#define AFE_MSG_ID_MSG_1 0X10013D2B
#define AW_MSG_ID_PARAMS_1 0x10013D2D
#define AW_MSG_ID_SPIN 0x10013D2E

#define AW_DSP_MSG_HDR_VER 1
#define AW_PARSE_DTSI_INT_DEALUT 0

enum aw_dsp_msg_type {
	AW_DSP_MSG_TYPE_DATA = 0,
	AW_DSP_MSG_TYPE_CMD = 1,
};

enum {
	AW_MSG_PARAM_ID_0 = 0,
	AW_MSG_PARAM_ID_1 = 1,
	AW_MSG_PARAM_ID_MAX,
};

struct aw_msg_hdr {
	int32_t type;
	int32_t opcode_id;
	int32_t version;
	int32_t reseriver[3];
};

struct aw_smartpa_parm {
	struct aw_msg_hdr hdr;
	uint32_t msg_value;
};

struct aw_smartpa_data {
	int32_t data[MAX_PA_NUM];
};

struct awinic_smartpa_priv {
	struct adsp_misc_priv *pdata;
	uint32_t algo_control;
};

static struct awinic_smartpa_priv *g_awinic_data;

static int aw_smartpakit_cal_apr(struct adsp_misc_priv *pdata,
	struct smartpa_common_param common_param, void *buf,
	size_t buf_size, unsigned int write)
{
	if (IS_ERR_OR_NULL(pdata->intf->send_smartpakit_cal_apr)) {
		hwlog_err("%s: send_smartpakit_cal_apr ptr is err\n", __func__);
		return -EFAULT;
	}

	return pdata->intf->send_smartpakit_cal_apr(common_param, buf,
				buf_size, write);
}

static int aw_smartpakit_cal_in_band(struct adsp_misc_priv *pdata,
	struct smartpa_common_param common_param, int dst_port,
	void *cmd, size_t cmd_size)
{
	if (IS_ERR_OR_NULL(pdata->intf->send_cal_in_band)) {
		hwlog_err("%s: send_cal_in_band ptr is err\n", __func__);
		return -EFAULT;
	}

	return pdata->intf->send_cal_in_band(common_param, dst_port,
				cmd, cmd_size);
}

int awinic_smartpa_cali_start(struct adsp_misc_priv *pdata)
{
	int ret = 0;
	struct smartpa_common_param smartpa_common_param = {0};
	struct aw_smartpa_parm aw_smartpa_parm = {0};

	if (pdata->cali_start) {
		hwlog_info("%s: CALIBRATE_MODE_START\n", __func__);
		smartpa_common_param.module_id = AFE_MODULE_ID_AWDSP_RX;
		smartpa_common_param.param_id = AFE_MSG_ID_MSG_0;
		smartpa_common_param.port_id = pdata->rx_port_id;

		aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_ENABLE_CALI;
		aw_smartpa_parm.hdr.type = AW_DSP_MSG_TYPE_DATA;
		aw_smartpa_parm.hdr.version = AW_DSP_MSG_HDR_VER;
		aw_smartpa_parm.msg_value = 1;// enable cali

		ret = aw_smartpakit_cal_apr(pdata, smartpa_common_param,
			&aw_smartpa_parm, sizeof(struct aw_smartpa_parm),
			DSP_WRITE);
		if (pdata->pa_info.pa_num > AW_MSG_PARAM_ID_MAX) {
			hwlog_info("%s: pa_num is %d\n", __func__, pdata->pa_info.pa_num);
			smartpa_common_param.module_id = AFE_MODULE_ID_AWDSP_RX;
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_1;
			smartpa_common_param.port_id = pdata->rx_port_id;

			aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_ENABLE_CALI;
			aw_smartpa_parm.hdr.type = AW_DSP_MSG_TYPE_DATA;
			aw_smartpa_parm.hdr.version = AW_DSP_MSG_HDR_VER;
			aw_smartpa_parm.msg_value = 1;// enable cali

			ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
				&aw_smartpa_parm, sizeof(struct aw_smartpa_parm),
				DSP_WRITE);
		}
	}

	return ret;
}
EXPORT_SYMBOL_GPL(awinic_smartpa_cali_start);

int awinic_smartpa_cali_stop(struct adsp_misc_priv *pdata)
{
	UNUSED(pdata);

	return 0;
}
EXPORT_SYMBOL_GPL(awinic_smartpa_cali_stop);

static bool awinic_check_smartpa_calib_range(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	int i;
	int *value = (int *)data;
	int calib_r;

	for (i = 0; i < pdata->pa_info.pa_num; i++)
		hwlog_debug("%s: pa%d calib_r:%d, len %zu\n",
			__func__, i, value[i], len);

	for (i = 0; i < pdata->pa_info.pa_num; i++) {
		calib_r = AW_CALI_VALUE_INT_TO_DEC(value[i] / AW_CALI_CAL_FACTOR);
		if (calib_r < pdata->cali_min_limit ||
			calib_r > pdata->cali_max_limit) {
			hwlog_err("%s: pa%d calib_r:%d %d is invaild\n",
				__func__, i, value[i], calib_r);
			return false;
		}

		value[i] = value[i] / AW_CALI_CAL_FACTOR;
	}

	return true;
}

int awinic_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret = 0;
	int i;
	int *value = (int *)data;
	struct smartpa_common_param smartpa_common_param = {0};
	struct aw_smartpa_parm aw_smartpa_parm = {0};

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n",
			__func__, len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	if (!awinic_check_smartpa_calib_range(pdata, data, len)) {
		hwlog_err("%s: smartpa calib in not in range\n", __func__);
		return -EINVAL;
	}

	if (pdata->cali_start) {
		hwlog_info("%s: cali start, not update cali value\n", __func__);
		return ret;
	}

	smartpa_common_param.module_id = AFE_MODULE_ID_AWDSP_RX;
	smartpa_common_param.port_id = pdata->rx_port_id;
	aw_smartpa_parm.hdr.type = AW_DSP_MSG_TYPE_DATA;
	aw_smartpa_parm.hdr.version = AW_DSP_MSG_HDR_VER;
	for (i = 0; i < pdata->pa_info.pa_num; i++) {
		if (i < AW_MSG_PARAM_ID_MAX)
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_0;
		else
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_1;

		if (i % AW_MSG_PARAM_ID_MAX)
			aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_RE_R;
		else
			aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_RE_L;

		aw_smartpa_parm.msg_value = value[i];// cali_re

		ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
				&aw_smartpa_parm,
				sizeof(struct aw_smartpa_parm), DSP_WRITE);
		hwlog_info("%s: channel %d cali_re:0x%x %d ret:%d\n", __func__,
			i, aw_smartpa_parm.msg_value,
			AW_CALI_VALUE_INT_TO_DEC(aw_smartpa_parm.msg_value),
			ret);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(awinic_cali_re_update);

int awinic_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret = 0;
	int i;
	int *value = (int *)data;
	struct smartpa_common_param smartpa_common_param = {0};
	struct aw_smartpa_parm aw_smartpa_parm = {0};
	struct aw_smartpa_data aw_smartpa_data = {0};

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n", __func__,
			len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	smartpa_common_param.module_id = AFE_MODULE_ID_AWDSP_RX;
	smartpa_common_param.port_id = pdata->rx_port_id;
	aw_smartpa_parm.hdr.type = AW_DSP_MSG_TYPE_CMD;
	aw_smartpa_parm.hdr.version = AW_DSP_MSG_HDR_VER;
	aw_smartpa_parm.msg_value = 1; // enable cali
	for (i = 0; i < pdata->pa_info.pa_num; i++) {
		if (i < AW_MSG_PARAM_ID_MAX)
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_0;
		else
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_1;

		if (i % AW_MSG_PARAM_ID_MAX)
			aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_REAL_DATA_R;
		else
			aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_REAL_DATA_L;

		ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
			&aw_smartpa_parm, sizeof(struct aw_smartpa_parm),
			DSP_WRITE);

		ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
			&aw_smartpa_data, sizeof(struct aw_smartpa_data),
			DSP_READ);

		hwlog_info("%s: channel %d r0:0x%x %d om \n", __func__,
			i, aw_smartpa_data.data[0],
			AW_CALI_VALUE_INT_TO_DEC(aw_smartpa_data.data[0]));

		/* the r0 value is in data[0] */
		value[i] = aw_smartpa_data.data[0] * AW_CALI_CAL_FACTOR;
	}

	return ret;
}
EXPORT_SYMBOL_GPL(awinic_get_current_r0);

int awinic_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret = 0;
	int i;
	struct smartpa_common_param smartpa_common_param = {0};
	struct aw_smartpa_parm aw_smartpa_parm = {0};
	struct aw_smartpa_data aw_smartpa_data = {0};

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n", __func__,
			len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	smartpa_common_param.module_id = AFE_MODULE_ID_AWDSP_RX;
	smartpa_common_param.port_id = pdata->rx_port_id;
	aw_smartpa_parm.hdr.type = AW_DSP_MSG_TYPE_CMD;
	aw_smartpa_parm.hdr.version = AW_DSP_MSG_HDR_VER;
	aw_smartpa_parm.msg_value = 1;// enable cali
	for (i = 0; i < pdata->pa_info.pa_num; i++) {
		if (i < AW_MSG_PARAM_ID_MAX)
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_0;
		else
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_1;

		if (i % AW_MSG_PARAM_ID_MAX)
			aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_F0_R;
		else
			aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_F0_L;

		ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
				&aw_smartpa_parm,
				sizeof(struct aw_smartpa_parm), DSP_WRITE);

		ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
				&aw_smartpa_data.data[i],
				sizeof(uint32_t), DSP_READ);

		hwlog_info("%s:channel %d f0:0x%x %d\n", __func__,
			i, aw_smartpa_data.data[i], aw_smartpa_data.data[i]);
	}

	memcpy(data, &aw_smartpa_data, len);
	return ret;
}
EXPORT_SYMBOL_GPL(awinic_get_current_f0);

int awinic_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret = 0;
	int i;
	int *value = data;
	struct smartpa_common_param smartpa_common_param = {0};
	struct aw_smartpa_parm aw_smartpa_parm = {0};
	struct aw_smartpa_data aw_smartpa_data = {0};

	if (pdata == NULL || data == NULL) {
		hwlog_err("%s: pdata or data is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n", __func__,
			len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	smartpa_common_param.module_id = AFE_MODULE_ID_AWDSP_RX;
	smartpa_common_param.port_id = pdata->rx_port_id;
	aw_smartpa_parm.hdr.type = AW_DSP_MSG_TYPE_CMD;
	aw_smartpa_parm.hdr.version = AW_DSP_MSG_HDR_VER;
	aw_smartpa_parm.hdr.opcode_id = AW_MSG_ID_SPK_STATUS;
	aw_smartpa_parm.msg_value = 1;// enable cali
	for (i = 0; i < pdata->pa_info.pa_num; i++) {
		if (i < AW_MSG_PARAM_ID_MAX)
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_0;
		else
			smartpa_common_param.param_id = AFE_MSG_ID_MSG_1;

		ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
				&aw_smartpa_parm,
				sizeof(struct aw_smartpa_parm), DSP_WRITE);

		ret += aw_smartpakit_cal_apr(pdata, smartpa_common_param,
				&aw_smartpa_data,
				sizeof(struct aw_smartpa_data), DSP_READ);

		/* get te from alog is in data[2] */
		hwlog_info("%s:channel %d te:0x%x %d\n", __func__,
			i, aw_smartpa_data.data[2], aw_smartpa_data.data[2]);

		/* get te from alog is in data[2] */
		value[i] = aw_smartpa_data.data[2];
	}
	return ret;
}
EXPORT_SYMBOL_GPL(awinic_get_current_te);

int awinic_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL_GPL(awinic_get_current_q);

int awinic_algo_control(struct adsp_misc_priv *pdata, uint32_t enable)
{
	int ret;
	struct smartpa_common_param smartpa_common_param = {0};

	if (g_awinic_data) {
		if (!g_awinic_data->algo_control) {
			hwlog_debug("%s: no need algo_control\n", __func__);
			return 0;
		}
	}

	if (pdata == NULL) {
		hwlog_err("%s: pdata is NULL\n", __func__);
		return -EINVAL;
	}
	smartpa_common_param.module_id = AFE_MODULE_ID_AWDSP_RX;
	smartpa_common_param.port_id = pdata->rx_port_id;
	smartpa_common_param.param_id = AFE_PARAM_ID_AWDSP_RX_SET_ENABLE;

	ret = aw_smartpakit_cal_in_band(pdata, smartpa_common_param,
			0, &enable, sizeof(uint32_t));

	return ret;
}
EXPORT_SYMBOL_GPL(awinic_algo_control);

int awinic_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL_GPL(awinic_smartpa_cal_write);

int awinic_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL_GPL(awinic_smartpa_cal_read);

static const struct of_device_id awinic_smartamp_of_match[] = {
	{ .compatible = "huawei,awinic_smartamp", },
	{},
};
MODULE_DEVICE_TABLE(of, awinic_smartamp_of_match);

static void awinic_smartamp_parse_dtsi(struct device_node *np,
	struct awinic_smartpa_priv *pdata)
{
	int ret;
	const char *algo_control_str = "algo_control_need";

	ret = of_property_read_u32(np, algo_control_str, &pdata->algo_control);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			algo_control_str);
		pdata->algo_control = AW_PARSE_DTSI_INT_DEALUT;
	}
}

static int awinic_smartamp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	const struct of_device_id *match = NULL;

	g_awinic_data = kzalloc(sizeof(struct awinic_smartpa_priv), GFP_KERNEL);
	if (g_awinic_data == NULL) {
		hwlog_err("%s:kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	match = of_match_device(awinic_smartamp_of_match, dev);
	if (match == NULL) {
		hwlog_err("%s:get device info err\n", __func__);
		goto awinic_smartamp_probe_err;
	}

	/* get parent data */
	g_awinic_data->pdata = (struct adsp_misc_priv *)dev_get_drvdata(pdev->dev.parent);
	awinic_smartamp_parse_dtsi(node, g_awinic_data);
	hwlog_info("%s: awinic smartamp registed success\n", __func__);

	return 0;

awinic_smartamp_probe_err:
	kfree(g_awinic_data);
	g_awinic_data = NULL;
	return -EINVAL;
}

static int awinic_smartamp_remove(struct platform_device *pdev)
{
	if (g_awinic_data == NULL)
		return 0;

	kfree(g_awinic_data);
	g_awinic_data = NULL;
	return 0;
}

static struct platform_driver awinic_smartamp_driver = {
	.driver = {
		.name   = "awinic_smartamp",
		.owner  = THIS_MODULE,
		.of_match_table = awinic_smartamp_of_match,
	},
	.probe  = awinic_smartamp_probe,
	.remove = awinic_smartamp_remove,
};

static int __init awinic_smartamp_init(void)
{
	return platform_driver_register(&awinic_smartamp_driver);
}

static void __exit awinic_smartamp_exit(void)
{
	platform_driver_unregister(&awinic_smartamp_driver);
}

fs_initcall(awinic_smartamp_init);
module_exit(awinic_smartamp_exit);

MODULE_DESCRIPTION("awinic smartpa adsp misc driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");

