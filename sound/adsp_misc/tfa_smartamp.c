/*
 * tfa_smartamp.c
 *
 * tfa smartpa related definition for adsp misc
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

#include <linux/delay.h>
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

#define TFA_CALI_CONVERT_FACTOR 65536

#define TFA_CALI_VALUE_INT_TO_DEC(a) ((a) * 1000 / TFA_CALI_CONVERT_FACTOR)
#define TFA_ADSP_CMD_SIZE_LIMIT  512

#define TFA_CALI_UPDATE_CMD_LEN 9
#define TFA_CALI_GET_PARAM_CMD_LEN 6
#define TFA_GET_PARAM_CMD_LEN 21
#define TFA_ALGO_CTRL_CMD_LEN 6

#define TFA_R0_CALI_IDX_0 0
#define TFA_R0_CALI_IDX_1 3
#define TFA_R0_CALI_IDX_2 32
#define TFA_R0_CALI_IDX_3 35

#define TFA_R0_IDX_0 3
#define TFA_R0_IDX_1 6
#define TFA_R0_IDX_2 35
#define TFA_R0_IDX_3 38

#define TFA_F0_IDX_0 15
#define TFA_F0_IDX_1 18
#define TFA_F0_IDX_2 47
#define TFA_F0_IDX_3 50

#define TFA_TE_IDX_0 9
#define TFA_TE_IDX_1 12
#define TFA_TE_IDX_2 41
#define TFA_TE_IDX_3 44

#define TFA_ADSP_CMD_MASK 0xff
#define TFA_3CMD_TO_ADSP_BUF(buf, cmd_id0, cmd_id1, cmd_id2) \
do { \
	(buf)[0] = ((cmd_id0) & TFA_ADSP_CMD_MASK); \
	(buf)[1] = ((cmd_id1) & TFA_ADSP_CMD_MASK); \
	(buf)[2] = ((cmd_id2) & TFA_ADSP_CMD_MASK); \
} while (0)

#define TFA_6CMD_TO_ADSP_BUF(buf, cmd_id0, cmd_id1, cmd_id2, \
	cmd_id3, cmd_id4, cmd_id5) \
do { \
	(buf)[0] = ((cmd_id0) & TFA_ADSP_CMD_MASK); \
	(buf)[1] = ((cmd_id1) & TFA_ADSP_CMD_MASK); \
	(buf)[2] = ((cmd_id2) & TFA_ADSP_CMD_MASK); \
	(buf)[3] = ((cmd_id3) & TFA_ADSP_CMD_MASK); \
	(buf)[4] = ((cmd_id4) & TFA_ADSP_CMD_MASK); \
	(buf)[5] = ((cmd_id5) & TFA_ADSP_CMD_MASK); \
} while (0)


#define TFA_3VALUE_CALC_PARAM(buf, index) \
	(((buf)[index + 0] << 16) + ((buf)[index + 1] << 8) + ((buf)[index + 2]))

#define AFE_MODULE_ID_TFADSP_RX 0x1000B911
#define AFE_MODULE_ID_TFADSP_TX 0x1000B912
#define AFE_PARAM_ID_TFADSP_TX_SET_ENABLE 0x1000B920
#define AFE_PARAM_ID_TFADSP_RX_CFG 0x1000B921
#define AFE_PARAM_ID_TFADSP_RX_GET_RESULT 0x1000B922
#define AFE_PARAM_ID_TFADSP_RX_SET_BYPASS 0x1000B923
#define AFE_PARAM_ID_TFADSP_RX_SET_HAPTIC_GAIN 0x1000B98F

#define TFA_PARSE_DTSI_INT_DEALUT 0

struct tfa_smartpa_priv {
	struct adsp_misc_priv *pdata;
	uint32_t algo_control;
};

struct tfa_smartpa_parm {
	uint8_t msg_value[TFA_ADSP_CMD_SIZE_LIMIT];
};

static uint8_t memtrack[] = {
	0x00, 0x80, 0x0b, 0x00, 0x00, 0x06,
	0x22, 0x00, 0x00, 0x22, 0x00, 0x01,
	0x22, 0x00, 0x04, 0x22, 0x00, 0x05,
	0x22, 0x00, 0x13, 0x22, 0x00, 0x14
};

static struct tfa_smartpa_priv *g_tfa_data;

static int tfa_smartpakit_cal_apr(struct adsp_misc_priv *pdata,
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

static int tfa_smartpakit_cal_in_band(struct adsp_misc_priv *pdata,
	struct smartpa_common_param common_param, int dst_port,
	void *cmd, size_t cmd_size)
{
	if (pdata->intf->send_cal_in_band == NULL) {
		hwlog_err("%s: function registration not done\n", __func__);
		return -EFAULT;
	}

	return pdata->intf->send_cal_in_band(common_param, dst_port,
						cmd, cmd_size);
}

static int tfa98xx_adsp_cmd(struct adsp_misc_priv *pdata,
	uint8_t *buf, ssize_t size)
{
	int ret;
	struct smartpa_common_param common_param = {0};

	if (buf == NULL)
		return -EINVAL;

	common_param.module_id = AFE_MODULE_ID_TFADSP_RX;
	common_param.port_id = pdata->rx_port_id;
	common_param.param_id = AFE_PARAM_ID_TFADSP_RX_CFG; // write

	ret = tfa_smartpakit_cal_in_band(pdata, common_param, 0, buf, size);
	mdelay(4); /* need delay 4ms for next step */
	/* This is from NXP, which is adapt to the tfa algorithm */
	if ((ret == 0) && (buf[2] >= 0x80)) { /* set 0x80 for algo need */
		/* read */
		common_param.param_id = AFE_PARAM_ID_TFADSP_RX_GET_RESULT;
		ret = tfa_smartpakit_cal_apr(pdata, common_param,
				buf, size, DSP_READ);
	}

	return ret;
}

int tfa_smartpa_cali_start(struct adsp_misc_priv *pdata)
{
	UNUSED(pdata);

	return 0;
}
EXPORT_SYMBOL(tfa_smartpa_cali_start);

int tfa_smartpa_cali_stop(struct adsp_misc_priv *pdata)
{
	UNUSED(pdata);

	return 0;
}
EXPORT_SYMBOL(tfa_smartpa_cali_stop);

/* calib data format (int)(7.08*65536) */
static bool tfa_check_smartpa_calib_range(struct adsp_misc_priv *pdata,
	void *data, int len)
{
	int i;
	int *value = (int *)data;
	int calib_r;

	for (i = 0; i < pdata->pa_info.pa_num; i++) {
		calib_r = TFA_CALI_VALUE_INT_TO_DEC(value[i]);
		if (calib_r < pdata->cali_min_limit ||
			calib_r > pdata->cali_max_limit) {
			hwlog_err("%s: pa%d calib_r:%d %d,len %zu is invaild\n",
				__func__, i, value[i], calib_r, len);
			return false;
		}
	}

	return true;
}

int tfa_cali_re_update(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret = 0;
	struct tfa_smartpa_parm tfa_smartpa_parm = {0};
	uint8_t *value = (uint8_t *)data;
	uint8_t *cmd = tfa_smartpa_parm.msg_value;
	uint8_t *tran_buf = (uint8_t *)(&cmd[3]); /* cmd start at cmd[3] */

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n", __func__,
			len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	if (!tfa_check_smartpa_calib_range(pdata, data, len)) {
		hwlog_err("%s: smartpa calib in not in range\n", __func__);
		return -EINVAL;
	}

	if (pdata->cali_start) {
		hwlog_info("%s: cali start, not update cali value\n", __func__);
		return ret;
	}

	/* accord algo need order fill first part cmd */
	TFA_3CMD_TO_ADSP_BUF(cmd, 0x00, 0x81, 0x05);

	switch (pdata->pa_info.pa_num) {
	case SINGLE_PA:
		/* accord algo need order fill the second part cmd */
		TFA_6CMD_TO_ADSP_BUF(tran_buf, value[2], value[1],
			value[0], value[2], value[1], value[0]);
		ret = tfa98xx_adsp_cmd(pdata, cmd, TFA_CALI_UPDATE_CMD_LEN);
		break;
	case DOUBLE_PA:
		/* accord algo need order fill the second part cmd */
		TFA_6CMD_TO_ADSP_BUF(tran_buf, value[2], value[1],
			value[0], value[6], value[5], value[4]);
		ret = tfa98xx_adsp_cmd(pdata, cmd, TFA_CALI_UPDATE_CMD_LEN);
		break;
	case FOUR_PA:
		/* accord algo need order fill the second part cmd */
		TFA_6CMD_TO_ADSP_BUF(tran_buf, value[2], value[1],
			value[0], value[6], value[5], value[4]);
		ret = tfa98xx_adsp_cmd(pdata, cmd, TFA_CALI_UPDATE_CMD_LEN);

		/* the first cmd need to reset to 0x10, need by algo */
		cmd[0] = 0x10;
		/* accord algo need order fill the third part cmd */
		TFA_6CMD_TO_ADSP_BUF(tran_buf, value[10], value[9],
			value[8], value[14], value[13], value[12]);
		ret += tfa98xx_adsp_cmd(pdata, cmd, TFA_CALI_UPDATE_CMD_LEN);
			break;

	default:
		break;
	}

	return ret;
}
EXPORT_SYMBOL(tfa_cali_re_update);

static int tfa_get_current_params_prepare(struct adsp_misc_priv *pdata)
{
	int ret;
	struct tfa_smartpa_parm tfa_smartpa_parm = {0};

	if (pdata->cali_start)
		return 0;

	if (sizeof(memtrack) > TFA_ADSP_CMD_SIZE_LIMIT) {
		hwlog_err("%s: memtrack length is too large\n", __func__);
		return -EINVAL;
	}

	memcpy(&tfa_smartpa_parm.msg_value, memtrack, sizeof(memtrack));

	tfa_smartpa_parm.msg_value[0] = 0x00; /* set cmd mask */
	ret = tfa98xx_adsp_cmd(pdata, tfa_smartpa_parm.msg_value,
		sizeof(memtrack));

	if (pdata->pa_info.pa_num == FOUR_PA) {
		tfa_smartpa_parm.msg_value[0] |= 0x10; /* set cmd mask */
		ret += tfa98xx_adsp_cmd(pdata, tfa_smartpa_parm.msg_value,
				sizeof(memtrack));
	}

	msleep(50); /* need delay 50ms for next step */
	return ret;
}

static int get_current_r0_in_cali_mode(struct adsp_misc_priv *pdata,
	uint8_t *buffer, int *params)
{
	int ret;
	/* second part start at cmd[32] */
	uint8_t *tran_buf = (uint8_t *)(&buffer[32]);

	/* accord algo need order fill first part cmd */
	TFA_3CMD_TO_ADSP_BUF(buffer,
		pdata->pa_info.pa_num == SINGLE_PA ? 0x04 : 0x00,
		0x81, 0x85);

	ret = tfa98xx_adsp_cmd(pdata, buffer, TFA_CALI_GET_PARAM_CMD_LEN);
	if (ret == 0) {
		/* get buf(0~2) to fill params[0] for PA0 */
		params[0] = TFA_3VALUE_CALC_PARAM(buffer, TFA_R0_CALI_IDX_0);
		if (pdata->pa_info.pa_num > SINGLE_PA)
			/* get buf(3~5) to fill params[1] for PA1 */
			params[1] = TFA_3VALUE_CALC_PARAM(buffer,
				TFA_R0_CALI_IDX_1);
	}

	if (pdata->pa_info.pa_num == FOUR_PA) {
		/* accord algo need order fill second part cmd */
		TFA_3CMD_TO_ADSP_BUF(tran_buf, 0x10, 0x81, 0x85);
		/* send cmd at buf[32] to adsp */
		ret = tfa98xx_adsp_cmd(pdata, &buffer[32],
				TFA_CALI_GET_PARAM_CMD_LEN);
		if (ret == 0) {
			/* get buf(32~34) to fill params[0] for PA2 */
			params[2] = TFA_3VALUE_CALC_PARAM(buffer,
							TFA_R0_CALI_IDX_2);
			/* get buf(35~37) to fill params[0] for PA3 */
			params[3] = TFA_3VALUE_CALC_PARAM(buffer,
							TFA_R0_CALI_IDX_3);
		}
	}
	return ret;
}


static int get_current_r0_in_common_mode(struct adsp_misc_priv *pdata,
	uint8_t *buffer, int *params)
{
	int ret;
	/* second part start at cmd[32] */
	uint8_t *tran_buf = (uint8_t *)(&buffer[32]);

	/* accord algo need order fill first part cmd */
	TFA_3CMD_TO_ADSP_BUF(buffer, 0x00, 0x80, 0x8b);
	ret = tfa98xx_adsp_cmd(pdata, buffer, TFA_GET_PARAM_CMD_LEN);
	if (ret == 0) {
		/* get buf(3~5) to fill params[0] for PA0 */
		params[0] = TFA_3VALUE_CALC_PARAM(buffer, TFA_R0_IDX_0);
		if (pdata->pa_info.pa_num > SINGLE_PA)
			/* get buf(6~8) to fill params[1] for PA1 */
			params[1] = TFA_3VALUE_CALC_PARAM(buffer, TFA_R0_IDX_1);
	}

	if (pdata->pa_info.pa_num == FOUR_PA) {
		/* accord algo need order fill second part cmd */
		TFA_3CMD_TO_ADSP_BUF(tran_buf, 0x10, 0x80, 0x8b);
		/* send cmd at buf[32] to adsp */
		ret += tfa98xx_adsp_cmd(pdata, &buffer[32],
				TFA_GET_PARAM_CMD_LEN);
		if (ret == 0) {
			/* get buf(35~37) to fill params[0] for PA2 */
			params[2] = TFA_3VALUE_CALC_PARAM(buffer, TFA_R0_IDX_2);
			/* get buf(38~40) to fill params[0] for PA3 */
			params[3] = TFA_3VALUE_CALC_PARAM(buffer, TFA_R0_IDX_3);
		}
	}
	return ret;
}

int tfa_get_current_r0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;
	int i;
	struct tfa_smartpa_parm tfa_smartpa_parm = {0};
	int *params = (int *)data;
	uint8_t *buffer = tfa_smartpa_parm.msg_value;

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n", __func__,
			len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	ret = tfa_get_current_params_prepare(pdata);
	if (ret < 0) {
		hwlog_err("%s: get_current_params_prepare failed\n", __func__);
		return ret;
	}

	if (pdata->cali_start)
		ret = get_current_r0_in_cali_mode(pdata, buffer, params);
	else
		ret = get_current_r0_in_common_mode(pdata, buffer, params);

	for (i = 0; i < len / sizeof(uint32_t); i++)
		hwlog_info("%s:Get current R0[%d] = %d\n",
			__func__, i, params[i]);

	return ret;
}
EXPORT_SYMBOL(tfa_get_current_r0);

int tfa_get_current_f0(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;
	int i;
	struct tfa_smartpa_parm tfa_smartpa_parm = {0};
	int *params = (int *)data;
	uint8_t *buffer = tfa_smartpa_parm.msg_value;
	/* second part start at cmd[32] */
	uint8_t *tran_buf = (uint8_t *)(&buffer[32]);

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n",
			__func__, len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	ret = tfa_get_current_params_prepare(pdata);
	if (ret < 0) {
		hwlog_err("%s: get_current_params_prepare failed\n", __func__);
		return ret;
	}

	/* accord algo need order fill first part cmd */
	TFA_3CMD_TO_ADSP_BUF(buffer, 0x00, 0x80, 0x8b);
	ret = tfa98xx_adsp_cmd(pdata, buffer, TFA_GET_PARAM_CMD_LEN);
	if (ret == 0) {
		/* get buf(15~17) to fill params[0] for PA0 */
		params[0] = TFA_3VALUE_CALC_PARAM(buffer, TFA_F0_IDX_0);
		if (pdata->pa_info.pa_num > SINGLE_PA)
			/* get buf(18~20) to fill params[1] for PA1 */
			params[1] = TFA_3VALUE_CALC_PARAM(buffer, TFA_F0_IDX_1);
	}

	if (pdata->pa_info.pa_num == FOUR_PA) {
		/* accord algo need order fill second part cmd */
		TFA_3CMD_TO_ADSP_BUF(tran_buf, 0x10, 0x80, 0x8b);
		/* send cmd at buf[32] to adsp */
		ret += tfa98xx_adsp_cmd(pdata, &buffer[32],
			TFA_GET_PARAM_CMD_LEN);
		if (ret == 0) {
			/* get buf(47~49) to fill params[0] for PA2 */
			params[2] = TFA_3VALUE_CALC_PARAM(buffer, TFA_F0_IDX_2);
			/* get buf(50~52) to fill params[0] for PA3 */
			params[3] = TFA_3VALUE_CALC_PARAM(buffer, TFA_F0_IDX_3);
		}
	}

	for (i = 0; i < len / sizeof(uint32_t); i++)
		hwlog_info("%s:Get current F0[%d] = %d\n",
			__func__, i, params[i]);

	return ret;
}
EXPORT_SYMBOL(tfa_get_current_f0);

int tfa_get_current_te(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;
	int i;
	struct tfa_smartpa_parm tfa_smartpa_parm = {0};
	int *params = (int *)data;
	uint8_t *buffer = tfa_smartpa_parm.msg_value;
	/* second part start at cmd[32] */
	uint8_t *tran_buf = (uint8_t *)(&buffer[32]);

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (len != pdata->pa_info.pa_num * sizeof(uint32_t)) {
		hwlog_err("%s: len:%d, pa_num:%d is inval\n", __func__,
			len, pdata->pa_info.pa_num);
		return -EINVAL;
	}

	ret = tfa_get_current_params_prepare(pdata);
	if (ret < 0) {
		hwlog_err("%s: get_current_params_prepare failed\n", __func__);
		return ret;
	}

	/* accord algo need order fill first part cmd */
	TFA_3CMD_TO_ADSP_BUF(buffer, 0x00, 0x80, 0x8b);
	ret = tfa98xx_adsp_cmd(pdata, buffer, TFA_GET_PARAM_CMD_LEN);
	if (ret == 0) {
		/* get buf(9~11) to fill params[0] for PA0 */
		params[0] = TFA_3VALUE_CALC_PARAM(buffer, TFA_TE_IDX_0);
		if (pdata->pa_info.pa_num > 1)
			/* get buf(12~14) to fill params[1] for PA1 */
			params[1] = TFA_3VALUE_CALC_PARAM(buffer, TFA_TE_IDX_1);
	}

	if (pdata->pa_info.pa_num == FOUR_PA) {
		/* accord algo need order fill second part cmd */
		TFA_3CMD_TO_ADSP_BUF(tran_buf, 0x10, 0x80, 0x8b);
		/* send cmd at buf[32] to adsp */
		ret += tfa98xx_adsp_cmd(pdata, &buffer[32],
			TFA_GET_PARAM_CMD_LEN);
		if (ret == 0) {
			/* get buf(41~43) to fill params[0] for PA2 */
			params[2] = TFA_3VALUE_CALC_PARAM(buffer, TFA_TE_IDX_2);
			/* get buf(44~46) to fill params[0] for PA3 */
			params[3] = TFA_3VALUE_CALC_PARAM(buffer, TFA_TE_IDX_3);
		}
	}

	for (i = 0; i < len / sizeof(uint32_t); i++)
		hwlog_info("%s:Get current Temp[%d] = %d\n",
			__func__, i, params[i]);

	return ret;
}
EXPORT_SYMBOL(tfa_get_current_te);

int tfa_get_current_q(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);

	return 0;
}
EXPORT_SYMBOL(tfa_get_current_q);

int tfa_algo_control(struct adsp_misc_priv *pdata, uint32_t enable)
{
	int ret;

	struct tfa_smartpa_parm tfa_smartpa_parm = {0};
	uint8_t *cmd = &(tfa_smartpa_parm.msg_value[0]);

	if (pdata == NULL) {
		hwlog_err("%s: pdata is NULL\n", __func__);
		return -EINVAL;
	}

	if (g_tfa_data) {
		if (!g_tfa_data->algo_control) {
			hwlog_debug("%s: no need algo_control\n", __func__);
			return 0;
		}
	}
	/* accord algo need order fill the second part cmd */
	TFA_6CMD_TO_ADSP_BUF(cmd, 0x00, 0x80,
		0x3f, 0x00, 0x00, enable ? 0x00 : 0x01);

	ret = tfa98xx_adsp_cmd(pdata, cmd, TFA_ALGO_CTRL_CMD_LEN);
	if (ret)
		hwlog_err("%s:set failed\n", __func__);

	return ret;
}
EXPORT_SYMBOL(tfa_algo_control);

int tfa_smartpa_cal_write(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;
	struct smartpa_common_param common_param = {0};

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	common_param.module_id = AFE_MODULE_ID_TFADSP_RX;
	common_param.port_id = pdata->rx_port_id;
	common_param.param_id = AFE_PARAM_ID_TFADSP_RX_CFG; // write

	ret = tfa_smartpakit_cal_apr(pdata, common_param, data, len, DSP_WRITE);
	if (ret)
		hwlog_err("%s:write failed\n", __func__);

	return ret;
}
EXPORT_SYMBOL(tfa_smartpa_cal_write);


int tfa_smartpa_cal_read(struct adsp_misc_priv *pdata, void *data, size_t len)
{
	int ret;
	struct smartpa_common_param common_param = {0};

	if (data == NULL || pdata == NULL) {
		hwlog_err("%s: data or pdata is NULL\n", __func__);
		return -EINVAL;
	}

	common_param.module_id = AFE_MODULE_ID_TFADSP_RX;
	common_param.port_id = pdata->rx_port_id;
	common_param.param_id = AFE_PARAM_ID_TFADSP_RX_GET_RESULT; // read
	ret = tfa_smartpakit_cal_apr(pdata, common_param,
			data, len, DSP_READ);

	if (ret)
		hwlog_err("%s:read failed\n", __func__);

	return ret;
}
EXPORT_SYMBOL(tfa_smartpa_cal_read);

static const struct of_device_id tfa_smartamp_of_match[] = {
	{ .compatible = "huawei,tfa_smartamp", },
	{},
};
MODULE_DEVICE_TABLE(of, tfa_smartamp_of_match);

static void tfa_smartamp_parse_dtsi(struct device_node *np,
	struct tfa_smartpa_priv *pdata)
{
	int ret;
	const char *algo_control_str = "algo_control_need";

	ret = of_property_read_u32(np, algo_control_str, &pdata->algo_control);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			algo_control_str);
		pdata->algo_control = TFA_PARSE_DTSI_INT_DEALUT;
	}
}

static int tfa_smartamp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	const struct of_device_id *match = NULL;

	g_tfa_data = kzalloc(sizeof(struct tfa_smartpa_priv), GFP_KERNEL);
	if (g_tfa_data == NULL) {
		hwlog_err("%s:kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	match = of_match_device(tfa_smartamp_of_match, dev);
	if (match == NULL) {
		hwlog_err("%s:get device info err\n", __func__);
		goto tfa_smartamp_probe_err;
	}

	/* get parent data */
	g_tfa_data->pdata = (struct adsp_misc_priv *)dev_get_drvdata(pdev->dev.parent);
	tfa_smartamp_parse_dtsi(node, g_tfa_data);
	hwlog_info("%s: tfa smartamp registed success\n", __func__);

	return 0;

tfa_smartamp_probe_err:
	kfree(g_tfa_data);
	g_tfa_data = NULL;
	return -EINVAL;
}

static int tfa_smartamp_remove(struct platform_device *pdev)
{
	if (g_tfa_data == NULL)
		return 0;

	kfree(g_tfa_data);
	g_tfa_data = NULL;
	return 0;
}

static struct platform_driver tfa_smartamp_driver = {
	.driver = {
		.name   = "tfa_smartamp",
		.owner  = THIS_MODULE,
		.of_match_table = tfa_smartamp_of_match,
	},
	.probe  = tfa_smartamp_probe,
	.remove = tfa_smartamp_remove,
};

static int __init tfa_smartamp_init(void)
{
	return platform_driver_register(&tfa_smartamp_driver);
}

static void __exit tfa_smartamp_exit(void)
{
	platform_driver_unregister(&tfa_smartamp_driver);
}

fs_initcall(tfa_smartamp_init);
module_exit(tfa_smartamp_exit);

MODULE_DESCRIPTION("tfa smartpa adsp misc driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
