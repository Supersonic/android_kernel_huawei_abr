/*
 * cust_smartamp.c
 *
 * cust smartpa related definition for adsp misc
 *
 * Copyright (c) 2021-2021 Hucustei Technologies Co., Ltd.
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

#define CUST_AFE_MODULE_ID 0x11111610
#define CUST_AFE_MSG_ID 0x11111613
#define CUST_AFE_MSG_GET_ID 0x11111614

#define CUST_PARSE_DTSI_INT_DEALUT 0

struct cust_smartpa_priv {
	struct adsp_misc_priv *pdata;
	uint32_t algo_control;
};

static struct cust_smartpa_priv *g_cust_data;

static int cust_smartpakit_cal_apr(struct adsp_misc_priv *pdata,
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

static int cust_set_smartpa_cmd_to_adsp(struct adsp_misc_priv *pdata, unsigned short cmd)
{
	int ret;
	struct smartpa_common_param smartpa_common_param = {0};
	struct adsp_misc_data_pkg smartpa_parm = {0};

	smartpa_common_param.module_id = CUST_AFE_MODULE_ID;
	smartpa_common_param.param_id = CUST_AFE_MSG_ID;
	smartpa_common_param.port_id = pdata->rx_port_id;

	smartpa_parm.cmd = cmd;

	ret = cust_smartpakit_cal_apr(pdata, smartpa_common_param,
		&smartpa_parm, sizeof(struct adsp_misc_data_pkg),
		DSP_WRITE);

	return ret;
}

static int cust_set_smartpa_cmd_to_adsp_with_params(struct adsp_misc_priv *pdata,
	void *data, size_t len, unsigned short cmd)
{
	int ret;
	size_t size;
	struct smartpa_common_param smartpa_common_param = {0};
	struct adsp_misc_data_pkg *packet = NULL;

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	hwlog_info("%s: enter\n", __func__);
	smartpa_common_param.module_id = CUST_AFE_MODULE_ID;
	smartpa_common_param.param_id = CUST_AFE_MSG_ID;
	smartpa_common_param.port_id = pdata->rx_port_id;

	size = sizeof(*packet) + len;

	packet = kzalloc(size, GFP_KERNEL);
	if (packet == NULL) {
		hwlog_err("%s: packet is NULL", __FUNCTION__);
		return -EINVAL;
	}

	packet->cmd = cmd;
	packet->size = len;
	memcpy(packet->data, data, len);

	ret = cust_smartpakit_cal_apr(pdata, smartpa_common_param,
				packet,
				size, DSP_WRITE);
	kfree(packet);
	packet = NULL;
	return ret;
}

static int cust_get_smartpa_result_from_adsp(struct adsp_misc_priv *pdata, void *data, size_t len, unsigned short cmd)
{
	int ret = 0;
	size_t size;
	struct smartpa_common_param smartpa_common_param = {0};
	struct adsp_misc_data_pkg *packet = NULL;

	hwlog_info("%s: enter len:%d\n", __func__, len);
	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n", __func__,
			len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	smartpa_common_param.module_id = CUST_AFE_MODULE_ID;
	smartpa_common_param.param_id = CUST_AFE_MSG_GET_ID;
	smartpa_common_param.port_id = pdata->rx_port_id;

	size = sizeof(*packet) + MAX_PA_NUM * sizeof(int);

	packet = kzalloc(size, GFP_KERNEL);
	if (packet == NULL) {
		hwlog_err("%s: packet is NULL", __FUNCTION__);
		return -EINVAL;
	}

	packet->cmd = cmd;
	packet->size = MAX_PA_NUM * sizeof(int);

	ret += cust_smartpakit_cal_apr(pdata, smartpa_common_param,
			packet, size,
			DSP_WRITE);
	ret += cust_smartpakit_cal_apr(pdata, smartpa_common_param,
			packet, size,
			DSP_READ);

	memcpy(data, packet->data, len);
	kfree(packet);
	packet = NULL;

	return ret;
}

int cust_smartpa_cali_start(struct adsp_misc_priv *pdata)
{
	int ret = 0;

	hwlog_info("%s: enter\n", __func__);
	if (pdata->cali_start) {
		hwlog_info("%s: CALIBRATE_MODE_START\n", __func__);
		ret = cust_set_smartpa_cmd_to_adsp(pdata, CALIBRATE_MODE_START);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(cust_smartpa_cali_start);

int cust_smartpa_cali_stop(struct adsp_misc_priv *pdata)
{
	int ret = 0;

	hwlog_info("%s: enter\n", __func__);
	if (pdata->cali_start) {
		hwlog_info("%s: CALIBRATE_MODE_stop\n", __func__);
		ret = cust_set_smartpa_cmd_to_adsp(pdata, CALIBRATE_MODE_STOP);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(cust_smartpa_cali_stop);

int cust_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);
	return cust_set_smartpa_cmd_to_adsp_with_params(pdata, data, len, SET_CALIBRATION_VALUE);
}
EXPORT_SYMBOL_GPL(cust_cali_re_update);

int cust_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);

	return cust_get_smartpa_result_from_adsp(pdata, data, len, GET_CURRENT_R0);
}
EXPORT_SYMBOL_GPL(cust_get_current_r0);

int cust_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);

	return cust_get_smartpa_result_from_adsp(pdata, data, len, GET_CURRENT_F0);
}
EXPORT_SYMBOL_GPL(cust_get_current_f0);

int cust_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);

	return cust_get_smartpa_result_from_adsp(pdata, data, len, GET_CURRENT_TEMPRATURE);
}
EXPORT_SYMBOL_GPL(cust_get_current_te);

int cust_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);

	return cust_get_smartpa_result_from_adsp(pdata, data, len, GET_CURRENT_Q);
}
EXPORT_SYMBOL_GPL(cust_get_current_q);

int cust_algo_control(struct adsp_misc_priv *pdata, uint32_t enable)
{
	int ret = 0;

	hwlog_info("%s: enter enable:%d\n", __func__, enable);

	if (pdata == NULL) {
		hwlog_err("%s: pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (enable == 1) {
		ret = cust_set_smartpa_cmd_to_adsp(pdata, SMARTPA_ALGO_ENABLE);
	} else {
		ret = cust_set_smartpa_cmd_to_adsp(pdata, SMARTPA_ALGO_DISABLE);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(cust_algo_control);

int cust_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}
EXPORT_SYMBOL_GPL(cust_smartpa_cal_write);

int cust_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL_GPL(cust_smartpa_cal_read);

int cust_smartpa_set_algo_scene(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);
	return cust_set_smartpa_cmd_to_adsp_with_params(pdata, data, len, SET_ALGO_SECENE);
}
EXPORT_SYMBOL_GPL(cust_smartpa_set_algo_scene);

int cust_smartpa_set_algo_battery(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);
	return cust_set_smartpa_cmd_to_adsp_with_params(pdata, data, len, SMARTPA_SET_BATTERY);
}
EXPORT_SYMBOL_GPL(cust_smartpa_set_algo_battery);

static const struct of_device_id cust_smartamp_of_match[] = {
	{ .compatible = "huawei,customize_smartamp", },
	{},
};
MODULE_DEVICE_TABLE(of, cust_smartamp_of_match);

static void cust_smartamp_parse_dtsi(struct device_node *np,
	struct cust_smartpa_priv *pdata)
{
	int ret;
	const char *algo_control_str = "algo_control_need";

	ret = of_property_read_u32(np, algo_control_str, &pdata->algo_control);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			algo_control_str);
		pdata->algo_control = CUST_PARSE_DTSI_INT_DEALUT;
	}
}

static int cust_smartamp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	const struct of_device_id *match = NULL;

	g_cust_data = kzalloc(sizeof(struct cust_smartpa_priv), GFP_KERNEL);
	if (g_cust_data == NULL) {
		hwlog_err("%s:kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	match = of_match_device(cust_smartamp_of_match, dev);
	if (match == NULL) {
		hwlog_err("%s:get device info err\n", __func__);
		goto cust_smartamp_probe_err;
	}

	/* get parent data */
	g_cust_data->pdata = (struct adsp_misc_priv *)dev_get_drvdata(pdev->dev.parent);
	cust_smartamp_parse_dtsi(node, g_cust_data);
	hwlog_info("%s: cust smartamp registed success\n", __func__);

	return 0;

cust_smartamp_probe_err:
	kfree(g_cust_data);
	g_cust_data = NULL;
	return -EINVAL;
}

static int cust_smartamp_remove(struct platform_device *pdev)
{
	if (g_cust_data == NULL)
		return 0;

	kfree(g_cust_data);
	g_cust_data = NULL;
	return 0;
}

static struct platform_driver cust_smartamp_driver = {
	.driver = {
		.name	= "customize_smartamp",
		.owner	= THIS_MODULE,
		.of_match_table = cust_smartamp_of_match,
	},
	.probe	= cust_smartamp_probe,
	.remove = cust_smartamp_remove,
};

static int __init cust_smartamp_init(void)
{
	return platform_driver_register(&cust_smartamp_driver);
}

static void __exit cust_smartamp_exit(void)
{
	platform_driver_unregister(&cust_smartamp_driver);
}

fs_initcall(cust_smartamp_init);
module_exit(cust_smartamp_exit);

MODULE_DESCRIPTION("cust smartpa adsp misc driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
