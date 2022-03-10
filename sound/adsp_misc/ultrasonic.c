/*
 * ultrasonic.c
 *
 * ultrasonic related definition for adsp misc
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

#define ULTRASONIC_AFE_MODULE_ID 0x11111700
#define ULTRASONIC_AFE_MSG_ID 0x11111701
#define ULTRASONIC_AFE_MSG_GET_ID 0x11111702
#define ULTRASOUND_SWITCH 0x11111703
#define ULTRASONIC_RX_PORT_ID 0xB030

static int ultrasonic_cal_apr(struct adsp_misc_priv *pdata,
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

static int cust_set_ultrasonic_cmd_to_adsp_with_params(struct adsp_misc_priv *pdata,
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
	smartpa_common_param.module_id = ULTRASONIC_AFE_MODULE_ID;
	smartpa_common_param.param_id = ULTRASOUND_SWITCH;
	smartpa_common_param.port_id = ULTRASONIC_RX_PORT_ID;

	size = sizeof(*packet) + len;

	packet = kzalloc(size, GFP_KERNEL);
	if (packet == NULL) {
		hwlog_err("%s: packet is NULL", __FUNCTION__);
		return -EINVAL;
	}
	packet->cmd = cmd;
	packet->size = len;
	memcpy(packet->data, data, len);

	ret = ultrasonic_cal_apr(pdata, smartpa_common_param,
				packet,
				size, DSP_WRITE);
	kfree(packet);
	packet = NULL;
	return ret;
}

static int cust_get_ultrasonic_result_from_adsp(struct adsp_misc_priv *pdata, void *data, size_t len, unsigned short cmd)
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

	smartpa_common_param.module_id = ULTRASONIC_AFE_MODULE_ID;
	smartpa_common_param.param_id = ULTRASONIC_AFE_MSG_GET_ID;
	smartpa_common_param.port_id = ULTRASONIC_RX_PORT_ID;;
	size = sizeof(*packet) + MAX_PA_NUM * sizeof(int);

	packet = kzalloc(size, GFP_KERNEL);
	if (packet == NULL) {
		hwlog_err("%s: packet is NULL", __FUNCTION__);
		return -EINVAL;
	}
	packet->cmd = cmd;
	packet->size = MAX_PA_NUM * sizeof(int);

	ret += ultrasonic_cal_apr(pdata, smartpa_common_param,
			packet, size,
			DSP_WRITE);
	ret += ultrasonic_cal_apr(pdata, smartpa_common_param,
			packet, size,
			DSP_READ);

	memcpy(data, packet->data, len);
	kfree(packet);
	packet = NULL;

	return ret;
}

int cust_ultrasonic_set_param(struct adsp_misc_priv *pdata, void *data, size_t len, unsigned short cmd)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);
	return cust_set_ultrasonic_cmd_to_adsp_with_params(pdata, data, len, cmd);
}
EXPORT_SYMBOL_GPL(cust_ultrasonic_set_param);

int cust_ultrasonic_get_param(struct adsp_misc_priv *pdata, void *data, size_t len, unsigned short cmd)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);
	return cust_get_ultrasonic_result_from_adsp(pdata, data, len, cmd);
}
EXPORT_SYMBOL_GPL(cust_ultrasonic_get_param);

MODULE_DESCRIPTION("ultrasonic adsp misc driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
