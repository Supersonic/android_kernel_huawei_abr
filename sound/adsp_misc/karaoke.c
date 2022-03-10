/*
 * karaoke.c
 *
 * karaoke related definition for adsp misc
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


#define KARAOKE_RX_V2_MODULE_ID 0x11111210
#define KARAOKE_MSG_GET_ID 0x11111213
#define KARAOKE_RX_PORT_ID 0xB030


static int karaoke_cal_apr(struct adsp_misc_priv *pdata,
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

static int cust_set_karaoke_cmd_to_adsp_with_params(
	struct adsp_misc_priv *pdata, void *data, size_t len,
	unsigned short dataType, unsigned short value)
{
	int ret;
	size_t size;
	struct smartpa_common_param smartpa_common_param = {0};
	struct adsp_misc_karaoke_data_pkg *packet = NULL;

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	hwlog_info("%s: enter\n", __func__);
	smartpa_common_param.module_id = KARAOKE_RX_V2_MODULE_ID;
	smartpa_common_param.param_id = KARAOKE_MSG_GET_ID;
	smartpa_common_param.port_id = KARAOKE_RX_PORT_ID;

	size = sizeof(*packet) + len;

	packet = kzalloc(size, GFP_KERNEL);
	if (packet == NULL) {
		hwlog_err("%s: packet is NULL", __func__);
		return -EINVAL;
	}
	packet->dataType = dataType;
	packet->value = value;
	packet->size = len;
	memcpy(packet->data, data, len);

	ret = karaoke_cal_apr(pdata, smartpa_common_param,
				packet,
				size, DSP_WRITE);
	kfree(packet);
	packet = NULL;
	return ret;
}

static int cust_get_karaoke_result_from_adsp(struct adsp_misc_priv *pdata,
	void *data, size_t len, unsigned short dataType, unsigned short value)
{
	int ret = 0;
	size_t size;
	struct smartpa_common_param smartpa_common_param = {0};
	struct adsp_misc_karaoke_data_pkg *packet = NULL;

	hwlog_info("%s: enter len:%d\n", __func__, len);
	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	smartpa_common_param.module_id = KARAOKE_RX_V2_MODULE_ID;
	smartpa_common_param.param_id = KARAOKE_MSG_GET_ID;
	smartpa_common_param.port_id = KARAOKE_RX_PORT_ID;

	size = sizeof(*packet) + len;

	packet = kzalloc(size, GFP_KERNEL);
	if (packet == NULL) {
		hwlog_err("%s: packet is NULL", __func__);
		return -EINVAL;
	}
	packet->dataType = dataType;
	packet->value = value;
	packet->size = len;

	ret += karaoke_cal_apr(pdata, smartpa_common_param,
			packet, size,
			DSP_WRITE);
	ret += karaoke_cal_apr(pdata, smartpa_common_param,
			packet, size,
			DSP_READ);

	memcpy(data, packet->data, len);
	kfree(packet);
	packet = NULL;

	return ret;
}

int cust_karaoke_set_param(struct adsp_misc_priv *pdata, void *data, size_t len,
	unsigned short dataType, unsigned short value)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);
	return cust_set_karaoke_cmd_to_adsp_with_params(pdata, data, len,
		dataType, value);
}
EXPORT_SYMBOL_GPL(cust_karaoke_set_param);

int cust_karaoke_get_param(struct adsp_misc_priv *pdata, void *data, size_t len,
	unsigned short dataType, unsigned short value)
{
	hwlog_info("%s: enter len:%d\n", __func__, len);
	return cust_get_karaoke_result_from_adsp(pdata, data, len, dataType,
		value);
}
EXPORT_SYMBOL_GPL(cust_karaoke_get_param);

MODULE_DESCRIPTION("karaoke adsp misc driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
