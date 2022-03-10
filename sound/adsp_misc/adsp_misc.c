/*
 * adsp_misc.c
 *
 * adsp misc for QC platform audio factory test
 *
 * Copyright (c) 2017-2019 Huawei Technologies Co., Ltd.
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
#include <linux/ioctl.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/of_device.h>
#include <huawei_platform/log/hw_log.h>
#include <sound/adsp_misc_interface.h>

#ifdef CONFIG_HUAWEI_DSM_AUDIO_MODULE
#define CONFIG_HUAWEI_DSM_AUDIO
#endif
#ifdef CONFIG_HUAWEI_DSM_AUDIO
#include <dsm/dsm_pub.h>
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define AFE_PORT_ID_RX_DEFALUT 0x9000
#define AFE_PORT_ID_TX_DEFALUT 0x9001
#define CALI_VALUE_MIN_DEFALUT 4000
#define CALI_VALUE_MAX_DEFALUT 12000
#define SMARTPA_NUM_DEFALUT 1

#define TUNING_RW_MAX_SIZE 4096

#define HWLOG_TAG adsp_misc
HWLOG_REGIST();

static struct adsp_misc_priv *g_adsp_misc_pdata;

/* default function, for smartpa algo not adpt */
static int cali_start_default(struct adsp_misc_priv *pdata)
{
	UNUSED(pdata);
	return 0;
}

static int cali_stop_default(struct adsp_misc_priv *pdata)
{
	UNUSED(pdata);
	return 0;
}

static int cali_re_update_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int get_current_r0_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int get_current_f0_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int get_current_te_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int get_current_q_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int smartpa_algo_control_default(struct adsp_misc_priv *pdata,
	uint32_t enable)
{
	UNUSED(pdata);
	UNUSED(enable);
	return 0;
}

static int smartpa_cal_write_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int smartpa_cal_read_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int smartpa_set_algo_scene_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static int smartpa_set_algo_battery_default(struct adsp_misc_priv *pdata,
	void *data, size_t len)
{
	UNUSED(pdata);
	UNUSED(data);
	UNUSED(len);
	return 0;
}

static struct smartpa_capability_intf adsp_misc_cap_intf[CHIP_VENDOR_MAX] = {
	{ /* MAXIM */
		cali_start_default,
		cali_stop_default,
		cali_re_update_default,
		get_current_r0_default,
		get_current_f0_default,
		get_current_te_default,
		get_current_q_default,
		smartpa_algo_control_default,
		smartpa_cal_write_default,
		smartpa_cal_read_default,
		smartpa_set_algo_scene_default,
		smartpa_set_algo_battery_default
	},
	{ /* NXP */
		tfa_smartpa_cali_start,
		tfa_smartpa_cali_stop,
		tfa_cali_re_update,
		tfa_get_current_r0,
		tfa_get_current_f0,
		tfa_get_current_te,
		tfa_get_current_q,
		tfa_algo_control,
		tfa_smartpa_cal_write,
		tfa_smartpa_cal_read,
		smartpa_set_algo_scene_default,
		smartpa_set_algo_battery_default
	},
	{ /* TI */
		tas_smartpa_cali_start,
		tas_smartpa_cali_stop,
		tas_cali_re_update,
		tas_get_current_r0,
		tas_get_current_f0,
		tas_get_current_te,
		tas_get_current_q,
		tas_algo_control,
		tas_smartpa_cal_write,
		tas_smartpa_cal_read,
		smartpa_set_algo_scene_default,
		smartpa_set_algo_battery_default
	},
	{ /* CS */
		cali_start_default,
		cali_stop_default,
		cali_re_update_default,
		get_current_r0_default,
		get_current_f0_default,
		get_current_te_default,
		get_current_q_default,
		smartpa_algo_control_default,
		smartpa_cal_write_default,
		smartpa_cal_read_default,
		smartpa_set_algo_scene_default,
		smartpa_set_algo_battery_default
	},
	{ /* CUSTOMIZE */
		cust_smartpa_cali_start,
		cust_smartpa_cali_stop,
		cust_cali_re_update,
		cust_get_current_r0,
		cust_get_current_f0,
		cust_get_current_te,
		cust_get_current_q,
		cust_algo_control,
		cust_smartpa_cal_write,
		cust_smartpa_cal_read,
		cust_smartpa_set_algo_scene,
		cust_smartpa_set_algo_battery
	},
	{ /* RT */
		cali_start_default,
		cali_stop_default,
		cali_re_update_default,
		get_current_r0_default,
		get_current_f0_default,
		get_current_te_default,
		get_current_q_default,
		smartpa_algo_control_default,
		smartpa_cal_write_default,
		smartpa_cal_read_default,
		smartpa_set_algo_scene_default
	},
	{ /* AWINIC */
		awinic_smartpa_cali_start,
		awinic_smartpa_cali_stop,
		awinic_cali_re_update,
		awinic_get_current_r0,
		awinic_get_current_f0,
		awinic_get_current_te,
		awinic_get_current_q,
		awinic_algo_control,
		awinic_smartpa_cal_write,
		awinic_smartpa_cal_read,
		smartpa_set_algo_scene_default
	},
	{ /* FOURSEMI */
		cali_start_default,
		cali_stop_default,
		cali_re_update_default,
		get_current_r0_default,
		get_current_f0_default,
		get_current_te_default,
		get_current_q_default,
		smartpa_algo_control_default,
		smartpa_cal_write_default,
		smartpa_cal_read_default,
		smartpa_set_algo_scene_default,
		smartpa_set_algo_battery_default
	},
};

int register_adsp_intf(struct smartpa_afe_interface *intf)
{
	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return -EPROBE_DEFER;
	}

	if (IS_ERR_OR_NULL(intf)) {
		hwlog_err("%s: intf ptr err %d\n",
			__func__, IS_ERR_OR_NULL(intf));
		return -EINVAL;
	}

	g_adsp_misc_pdata->intf = intf;
	return 0;

}
EXPORT_SYMBOL(register_adsp_intf);

static void refresh_smartpa_algo_status(int port_id)
{
	unsigned int chip_vendor;

	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return;
	}

	chip_vendor = g_adsp_misc_pdata->pa_info.chip_vendor;
	if (chip_vendor == CHIP_VENDOR_CUSTOMIZE) {
		hwlog_info("%s: chip_vendor is customize, not to refresh_smartpa_algo\n",
			 __func__);
		return;
	}

	if (chip_vendor >= CHIP_VENDOR_MAX) {
		hwlog_err("%s: chip_vendor:%d is invalid\n", __func__,
			chip_vendor);
		return;
	}

	if (port_id != g_adsp_misc_pdata->rx_port_id) {
		hwlog_err("%s: port_id:0x%x not equal rx_port_id:0x%x\n",
			__func__, port_id, g_adsp_misc_pdata->rx_port_id);
		return;
	}
	adsp_misc_cap_intf[chip_vendor].cali_re_update(g_adsp_misc_pdata,
		g_adsp_misc_pdata->calibration_value,
		g_adsp_misc_pdata->pa_info.pa_num * sizeof(uint32_t));

	if (g_adsp_misc_pdata->cali_start)
		adsp_misc_cap_intf[chip_vendor].cali_start(g_adsp_misc_pdata);
};

static bool get_smartpa_num(unsigned int *pa_num)
{
	unsigned int chip_vendor;

	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return false;
	}

	if (pa_num == NULL) {
		hwlog_err("%s: pa_num is NULL\n", __func__);
		return false;
	}

	chip_vendor = g_adsp_misc_pdata->pa_info.chip_vendor;

	if (g_adsp_misc_pdata->smartpa_num !=
		g_adsp_misc_pdata->pa_info.pa_num)
		hwlog_warn("%s:smartpa_num:%d not equal:%d\n", __func__,
			g_adsp_misc_pdata->smartpa_num,
			g_adsp_misc_pdata->pa_info.pa_num);

	*pa_num = g_adsp_misc_pdata->smartpa_num;
	return true;
}

static bool get_smartpa_rx_port_id(unsigned int *port_id)
{
	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return false;
	}

	if (port_id == NULL) {
		hwlog_err("%s: port_id is NULL\n", __func__);
		return false;
	}

	*port_id = g_adsp_misc_pdata->rx_port_id;
	return true;
}

static bool get_smartpa_tx_port_id(unsigned int *port_id)
{
	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return false;
	}

	if (port_id == NULL) {
		hwlog_err("%s: port_id is NULL\n", __func__);
		return false;
	}

	*port_id = g_adsp_misc_pdata->tx_port_id;
	return true;
}

struct smartpa_misc_interface smartpa_misc_interface = {
	.refresh_smartpa_algo_status = refresh_smartpa_algo_status,
	.get_smartpa_num = get_smartpa_num,
	.get_smartpa_rx_port_id = get_smartpa_rx_port_id,
	.get_smartpa_tx_port_id = get_smartpa_tx_port_id,
};

int register_smartpa_misc_intf(struct smartpa_misc_interface **intf)
{
	if (IS_ERR_OR_NULL(intf)) {
		hwlog_err("%s: intf ptr err %d\n",
			__func__, IS_ERR_OR_NULL(intf));
		return -EINVAL;
	}

	*intf = &smartpa_misc_interface;
	return 0;
}
EXPORT_SYMBOL(register_smartpa_misc_intf);

static int adsp_misc_open(struct inode *inode, struct file *filp)
{
	UNUSED(inode);
	UNUSED(filp);
	return 0;
}

static int adsp_misc_release(struct inode *inode, struct file *filp)
{
	UNUSED(inode);
	UNUSED(filp);
	hwlog_debug("%s: Device released\n", __func__);
	return 0;
}

#ifndef CONFIG_FINAL_RELEASE
static ssize_t adsp_misc_read(struct file *file, char __user *buf,
	size_t nbytes, loff_t *pos)
{
	int ret;
	uint8_t *buffer = NULL;
	unsigned int chip_vendor;

	if ((nbytes == 0) || (buf == NULL) || (nbytes > TUNING_RW_MAX_SIZE))
		return -EINVAL;

	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return -EINVAL;
	}

	hwlog_debug("%s: Read %d bytes from adsp_misc\n", __func__, nbytes);
	mutex_lock(&g_adsp_misc_pdata->adsp_misc_mutex);

	buffer = kzalloc(nbytes, GFP_KERNEL);
	if (buffer == NULL) {
		ret = -ENOMEM;
		goto read_err_out;
	}

	chip_vendor = g_adsp_misc_pdata->pa_info.chip_vendor;
	if (chip_vendor >= CHIP_VENDOR_MAX) {
		hwlog_err("%s: chip_vendor:%d is invaild\n",
			__func__, chip_vendor);
		return -EINVAL;
	}

	ret = adsp_misc_cap_intf[chip_vendor].smartpa_cal_read(
		g_adsp_misc_pdata, buffer, nbytes);
	if (ret) {
		hwlog_err("dsp_msg_read error: %d\n", ret);
		ret = -EFAULT;
		goto read_err_out;
	}

	ret = copy_to_user(buf, buffer, nbytes);
	if (ret) {
		hwlog_err("copy_to_user error: %d\n", ret);
		ret = -EFAULT;
		goto read_err_out;
	}

	kfree(buffer);
	*pos += nbytes;
	mutex_unlock(&g_adsp_misc_pdata->adsp_misc_mutex);
	return (ssize_t)nbytes;

read_err_out:
	kfree(buffer);
	mutex_unlock(&g_adsp_misc_pdata->adsp_misc_mutex);
	return (ssize_t)ret;
}

static ssize_t adsp_misc_write(struct file *file, const char __user *buf,
	size_t nbytes, loff_t *ppos)
{
	uint8_t *buffer = NULL;
	int ret;
	unsigned int chip_vendor;

	UNUSED(file);
	UNUSED(ppos);
	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return -EINVAL;
	}

	hwlog_debug("%s: Write %d bytes to adsp_misc\n", __func__, nbytes);
	if ((nbytes == 0) || (buf == NULL) || (nbytes > TUNING_RW_MAX_SIZE))
		return -EINVAL;


	mutex_lock(&g_adsp_misc_pdata->adsp_misc_mutex);

	buffer = kmalloc(nbytes, GFP_KERNEL);
	if (buffer == NULL) {
		ret = -ENOMEM;
		goto write_err_out;
	}

	if (copy_from_user(buffer, buf, nbytes)) {
		hwlog_err("Copy from user space err\n");
		ret = -EFAULT;
		goto write_err_out;
	}

	chip_vendor = g_adsp_misc_pdata->pa_info.chip_vendor;
	if (chip_vendor >= CHIP_VENDOR_MAX) {
		hwlog_err("%s: chip_vendor:%d is invaild\n", __func__,
			chip_vendor);
		return -EINVAL;
	}

	ret = adsp_misc_cap_intf[chip_vendor].smartpa_cal_write(
			g_adsp_misc_pdata, buffer, nbytes);
	if (ret) {
		hwlog_err("dsp_msg error: %d\n", ret);
		goto write_err_out;
	}

	kfree(buffer);
	mutex_unlock(&g_adsp_misc_pdata->adsp_misc_mutex);
	return (ssize_t)nbytes;

write_err_out:
	kfree(buffer);
	mutex_unlock(&g_adsp_misc_pdata->adsp_misc_mutex);
	return (ssize_t)ret;
}
#endif


static int soc_adsp_send_param(struct adsp_misc_ctl_info *param,
	unsigned char *data, unsigned int len,
	struct adsp_misc_priv *pdata)
{
	int ret = 0;
	int cmd;
	unsigned int chip_vendor;

	if (param == NULL) {
		hwlog_err("%s,invalid input param\n", __func__);
		return -EINVAL;
	}

	cmd = param->cmd;
	chip_vendor = param->pa_info.chip_vendor;
	if (chip_vendor >= CHIP_VENDOR_MAX) {
		hwlog_err("%s, chip_vendor:%d invalid input param\n",
			__func__, chip_vendor);
		return -EINVAL;
	}

	memcpy(&pdata->pa_info, &param->pa_info,
		sizeof(pdata->pa_info));
#ifndef CONFIG_FINAL_RELEASE
	hwlog_info("%s: enter, cmd = %d, chip_vendor = %d, len:%d\n", __func__,
		 cmd, chip_vendor, len);
#endif
	switch (cmd) {
	case SMARTPA_ALGO_ENABLE:
		hwlog_info("%s: SMARTPA_ALGO_ENABLE\n", __func__);
		ret = adsp_misc_cap_intf[chip_vendor].algo_control(pdata,
				PA_ALGO_ENABLE);
		break;
	case SMARTPA_ALGO_DISABLE:
		hwlog_info("%s: SMARTPA_ALGO_DISABLE\n", __func__);
		ret = adsp_misc_cap_intf[chip_vendor].algo_control(pdata,
				PA_ALGO_DISABLE);

		if (!IS_ERR_OR_NULL(pdata->intf->cal_unmap_memory))
			pdata->intf->cal_unmap_memory();
		break;
	case CALIBRATE_MODE_START:
		hwlog_info("%s: CALIBRATE_MODE_START\n", __func__);
		pdata->cali_start = true;
		ret = adsp_misc_cap_intf[chip_vendor].cali_start(pdata);
		break;
	case CALIBRATE_MODE_STOP:
		hwlog_info("%s: CALIBRATE_MODE_STOP\n", __func__);
		pdata->cali_start = false;
		break;
	case SET_CALIBRATION_VALUE:
		hwlog_info("%s: SET_CALIBRATION_VALUE enter, size = %u\n",
			__func__, param->size / sizeof(unsigned int));
		if (chip_vendor != 4) {
			if (param->size !=
				pdata->pa_info.pa_num * sizeof(uint32_t) ||
				param->size >
				sizeof(pdata->calibration_value)) {
				hwlog_err("%s: param size %u is error\n",
					 __func__, param->size);
				return -EINVAL;
			}
			memcpy(pdata->calibration_value,
				param->data, param->size);
		} else {
			if (param->size == sizeof(struct calib_oeminfo_common))
				memcpy(&pdata->calib_oeminfo, param->data,
					 param->size);
			else
				pr_err("%s: size is not correct, size = %d\n",
					 __func__, param->size);
		}

		ret = adsp_misc_cap_intf[chip_vendor].cali_re_update(pdata,
				param->data, param->size);
		break;
	case GET_CURRENT_R0:
		hwlog_info("%s: GET_CURRENT_R0\n", __func__);
		ret = adsp_misc_cap_intf[chip_vendor].get_current_r0(pdata,
				data, len);
		break;
	case GET_CURRENT_TEMPRATURE:
		hwlog_info("%s: GET_CURRENT_TEMPRATURE\n", __func__);
		ret = adsp_misc_cap_intf[chip_vendor].get_current_te(pdata,
				data, len);
		break;
	case GET_CURRENT_F0:
		hwlog_info("%s: GET_CURRENT_F0\n", __func__);
		ret = adsp_misc_cap_intf[chip_vendor].get_current_f0(pdata,
				data, len);
		break;
	case GET_CURRENT_Q:
		hwlog_info("%s: GET_CURRENT_Q\n", __func__);
		ret = adsp_misc_cap_intf[chip_vendor].get_current_q(pdata,
				data, len);
		break;
	case SET_ALGO_SECENE:
		hwlog_info("%s: SET_ALGO_SCENE\n", __func__);
		if (param->size <= 0 || param->size > MAX_PA_PARAS_SIZE) {
			pr_err("%s: size is not correct, size = %d\n", __func__,
				 param->size);
			return -EINVAL;
		}
		ret = adsp_misc_cap_intf[chip_vendor].set_algo_scene(pdata,
			param->data, param->size);
		break;
	case SMARTPA_SET_BATTERY:
		hwlog_info("%s: SMARTPA_SET_BATTERY\n", __func__);
		if (param->size != sizeof(struct battery_param)) {
			pr_err("%s: size is not correct, size = %d\n", __func__,
				 param->size);
			return -EINVAL;
		}
		ret = adsp_misc_cap_intf[chip_vendor].set_algo_battery(pdata,
			param->data, param->size);
		break;
	default:
		break;
	}

	if (ret)
		pr_err("%s: send cmd = %d, ret = %d\n", __func__, cmd, ret);

	return ret;
}

static int soc_adsp_send_ultrasonic_param(
	struct adsp_misc_ultrasonic_info *param, unsigned char *data,
	unsigned int len, struct adsp_misc_priv *pdata)
{
	int ret = 0;
	int cmd;

	hwlog_info("%s: enter\n", __func__);

	if (param == NULL) {
		hwlog_err("%s,invalid input param\n", __func__);
		return -EINVAL;
	}

	if (param->size <= 0) {
		pr_err("%s: size is not correct, size = %d\n", __func__,
			 param->size);
		return -EINVAL;
	}

	cmd = param->cmd;
	ret = cust_ultrasonic_set_param(pdata, param->data, param->size,
		 param->cmd);
	if (ret)
		pr_err("%s: send cmd = %d, ret = %d\n", __func__, cmd,
			 ret);

	return ret;
}

static int soc_adsp_send_karaoke_param(struct adsp_misc_karaoke_info *param,
	unsigned char *data, unsigned int len, struct adsp_misc_priv *pdata)
{
	int ret = 0;
	int dataType;
	int value;

	hwlog_info("%s: enter\n", __func__);

	if (param == NULL) {
		hwlog_err("%s,invalid input param\n", __func__);
		return -EINVAL;
	}

	if (param->size <= 0) {
		pr_err("%s: size is not correct, size = %d\n", __func__,
			 param->size);
		return -EINVAL;
	}

	dataType = param->dataType;
	value = param->value;
	ret = cust_karaoke_set_param(pdata, param->data, param->size,
		 param->dataType, param->value);
	if (ret)
		pr_err("%s: send dataType = %d, value = %d, ret = %d\n",
			 __func__, dataType, value, ret);

	return ret;
}

static int soc_adsp_get_inparam(struct adsp_ctl_param *adsp_param,
	struct adsp_misc_ctl_info **param_in)
{
	unsigned int par_len;
	struct adsp_misc_ctl_info *par_in = NULL;

	par_len = adsp_param->in_len;
#ifndef CONFIG_FINAL_RELEASE
	hwlog_info("%s: param in len is %d, in_param is %p",
		__func__, par_len, adsp_param->param_in);
#endif
	if ((par_len < MIN_PARAM_IN) || (adsp_param->param_in == NULL)) {
		hwlog_err("%s,param_in from user space is error\n", __func__);
		return -EINVAL;
	}

	par_in = kzalloc(par_len, GFP_KERNEL);
	if (par_in == NULL)
		return -ENOMEM;

	if (copy_from_user(par_in, adsp_param->param_in, par_len)) {
		hwlog_err("%s: get param copy_from_user fail\n", __func__);
		return -EFAULT;
	}
	*param_in = par_in;
	return 0;
}

static int soc_adsp_get_ultrasonicparam(struct adsp_ctl_param *adsp_param,
	struct adsp_misc_ultrasonic_info **param_in)
{
	unsigned int par_len;
	struct adsp_misc_ultrasonic_info *par_in = NULL;

	par_len = adsp_param->in_len;
#ifndef CONFIG_FINAL_RELEASE
	hwlog_info("%s: param in len is %d, in_param is %p",
		__func__, par_len, adsp_param->param_in);
#endif
	if (adsp_param->param_in == NULL) {
		hwlog_err("%s,param_in from user space is error\n", __func__);
		return -EINVAL;
	}

	par_in = kzalloc(par_len, GFP_KERNEL);
	if (par_in == NULL)
		return -ENOMEM;

	if (copy_from_user(par_in, adsp_param->param_in, par_len)) {
		hwlog_err("%s: get param copy_from_user fail\n", __func__);
		return -EFAULT;
	}
	*param_in = par_in;
	return 0;
}

static int soc_adsp_get_karaoke_param(struct adsp_ctl_param *adsp_param,
	struct adsp_misc_karaoke_info **param_in)
{
	unsigned int par_len;
	struct adsp_misc_karaoke_info *par_in = NULL;

	par_len = adsp_param->in_len;
#ifndef CONFIG_FINAL_RELEASE
	hwlog_info("%s: param in len is %d, in_param is %p",
		__func__, par_len, adsp_param->param_in);
#endif
	if (adsp_param->param_in == NULL) {
		hwlog_err("%s,param_in from user space is error\n", __func__);
		return -EINVAL;
	}

	par_in = kzalloc(par_len, GFP_KERNEL);
	if (par_in == NULL)
		return -ENOMEM;

	if (copy_from_user(par_in, adsp_param->param_in, par_len)) {
		hwlog_err("%s: get param copy_from_user fail\n", __func__);
		return -EFAULT;
	}
	*param_in = par_in;
	return 0;
}

static int soc_adsp_parse_param_info(void __user *arg, int compat_mode,
	struct adsp_ctl_param *adsp_param)
{
	memset(adsp_param, 0, sizeof(*adsp_param));

#ifdef CONFIG_COMPAT
	if (compat_mode == 0) {
#endif // CONFIG_COMPAT
		struct misc_io_sync_param par;

		memset(&par, 0, sizeof(par));
#ifndef CONFIG_FINAL_RELEASE
		hwlog_info("%s: copy_from_user b64 %p\n", __func__, arg);
#endif
		if (copy_from_user(&par, arg, sizeof(par))) {
			hwlog_err("%s: copy_from_user fail\n", __func__);
			return -EFAULT;
		}

		adsp_param->out_len = par.out_len;
		adsp_param->param_out = (void __user *)par.out_param;
		adsp_param->in_len = par.in_len;
		adsp_param->param_in = (void __user *)par.in_param;

#ifdef CONFIG_COMPAT
	} else {
		struct misc_io_sync_param_compat par_compat;

		memset(&par_compat, 0, sizeof(par_compat));
#ifndef CONFIG_FINAL_RELEASE
		hwlog_info("%s: copy_from_user b32 %p\n", __func__, arg);
#endif
		if (copy_from_user(&par_compat, arg, sizeof(par_compat))) {
			hwlog_err("%s: copy_from_user fail\n", __func__);
			return -EFAULT;
		}

		adsp_param->out_len = par_compat.out_len;
		adsp_param->param_out = compat_ptr(par_compat.out_param);
		adsp_param->in_len = par_compat.in_len;
		adsp_param->param_in = compat_ptr(par_compat.in_param);
	}
#endif //CONFIG_COMPAT
	return 0;
}

static int soc_adsp_handle_sync_param(void __user *arg, int compat_mode,
	struct adsp_misc_priv *pdata)
{
	int ret;
	struct adsp_misc_ctl_info *param_in = NULL;
	unsigned int param_out_len;
	struct adsp_misc_data_pkg *result = NULL;
	struct adsp_ctl_param adsp_param;

	if (arg == NULL) {
		hwlog_err("%s: Invalid input arg, exit\n", __func__);
		return -EINVAL;
	}

	ret = soc_adsp_parse_param_info(arg, compat_mode, &adsp_param);
	if (ret < 0)
		return ret;

	ret = soc_adsp_get_inparam(&adsp_param, &param_in);
	if (ret < 0)
		goto err;

	param_out_len = adsp_param.out_len;
	if ((param_out_len > MIN_PARAM_OUT) && (adsp_param.param_out != NULL)) {
		result = kzalloc(param_out_len, GFP_KERNEL);
		if (result == NULL) {
			ret = -ENOMEM;
			goto err;
		}
		result->size = param_out_len - sizeof(*result);
		ret = soc_adsp_send_param(param_in, result->data, result->size,
				pdata);
		if (ret < 0)
			goto err;

		if (copy_to_user(adsp_param.param_out, result, param_out_len)) {
			hwlog_err("%s:copy_to_user fail\n", __func__);
			ret = -EFAULT;
		}
	} else {
		ret = soc_adsp_send_param(param_in, NULL, 0, pdata);
	}

err:
	if (param_in != NULL)
		kfree(param_in);
	if (result != NULL)
		kfree(result);
	return ret;
}

static int soc_adsp_handle_ultrasonic_param(void __user *arg, int compat_mode,
	struct adsp_misc_priv *pdata)
{
	int ret;
	struct adsp_misc_ultrasonic_info *param_in = NULL;
	unsigned int param_out_len;
	struct adsp_misc_data_pkg *result = NULL;
	struct adsp_ctl_param adsp_param;

	hwlog_info("%s: enter\n", __func__);
	if (arg == NULL) {
		hwlog_err("%s: Invalid input arg, exit\n", __func__);
		return -EINVAL;
	}

	ret = soc_adsp_parse_param_info(arg, compat_mode, &adsp_param);
	if (ret < 0)
		return ret;

	ret = soc_adsp_get_ultrasonicparam(&adsp_param, &param_in);
	if (ret < 0)
		goto err;

	param_out_len = adsp_param.out_len;
	if ((param_out_len > MIN_PARAM_OUT) && (adsp_param.param_out != NULL)) {
		result = kzalloc(param_out_len, GFP_KERNEL);
		if (result == NULL) {
			ret = -ENOMEM;
			goto err;
		}
		result->size = param_out_len - sizeof(*result);
		ret = soc_adsp_send_ultrasonic_param(param_in, result->data,
			 result->size, pdata);
		if (ret < 0)
			goto err;

		if (copy_to_user(adsp_param.param_out, result, param_out_len)) {
			hwlog_err("%s:copy_to_user fail\n", __func__);
			ret = -EFAULT;
		}
	} else {
		ret = soc_adsp_send_ultrasonic_param(param_in, NULL, 0, pdata);
	}

err:
	if (param_in != NULL)
		kfree(param_in);
	if (result != NULL)
		kfree(result);
	return ret;
}

static int soc_adsp_handle_karaoke_param(void __user *arg, int compat_mode,
	struct adsp_misc_priv *pdata)
{
	int ret;
	struct adsp_misc_karaoke_info *param_in = NULL;
	struct adsp_misc_data_pkg *result = NULL;
	struct adsp_ctl_param adsp_param;

	hwlog_info("%s: enter\n", __func__);
	if (arg == NULL) {
		hwlog_err("%s: Invalid input arg, exit\n", __func__);
		return -EINVAL;
	}

	ret = soc_adsp_parse_param_info(arg, compat_mode, &adsp_param);
	if (ret < 0)
		return ret;

	ret = soc_adsp_get_karaoke_param(&adsp_param, &param_in);
	if (ret < 0)
		goto err;


	ret = soc_adsp_send_karaoke_param(param_in, NULL, 0, pdata);
err:
	if (param_in != NULL)
		kfree(param_in);
	if (result != NULL)
		kfree(result);
	return ret;
}


static int adsp_misc_do_ioctl(struct file *file, unsigned int command,
					 void __user *arg, int compat_mode)
{
	int ret;

	if (g_adsp_misc_pdata == NULL) {
		hwlog_err("%s: adsp_misc in not initial\n", __func__);
		return -EINVAL;
	}

	hwlog_debug("%s: enter, cmd:0x%x compat_mode=%d\n",
		__func__, command, compat_mode);
	if (file == NULL) {
		hwlog_err("%s: invalid argument\n", __func__);
		ret = -EFAULT;
		goto out;
	}

	switch (command) {
	case ADSP_MISC_IOCTL_ASYNCMSG:
		ret = -EFAULT;
		break;
	case ADSP_MISC_IOCTL_SYNCMSG:
		mutex_lock(&g_adsp_misc_pdata->adsp_misc_mutex);
		ret = soc_adsp_handle_sync_param(arg, compat_mode,
				g_adsp_misc_pdata);
		mutex_unlock(&g_adsp_misc_pdata->adsp_misc_mutex);
		break;
	case ADSP_MISC_IOCTL_ULTRASONICMSG:
		mutex_lock(&g_adsp_misc_pdata->adsp_misc_mutex);
		ret = soc_adsp_handle_ultrasonic_param(arg, compat_mode,
				g_adsp_misc_pdata);
		mutex_unlock(&g_adsp_misc_pdata->adsp_misc_mutex);
		break;
	case ADSP_MISC_IOCTL_KARAOKEMSG:
		mutex_lock(&g_adsp_misc_pdata->adsp_misc_mutex);
		ret = soc_adsp_handle_karaoke_param(arg, compat_mode,
				g_adsp_misc_pdata);
		mutex_unlock(&g_adsp_misc_pdata->adsp_misc_mutex);
		break;
	default:
		ret = -EFAULT;
		break;
	}

out:
	return ret;
}

static long adsp_misc_ioctl(struct file *file, unsigned int command,
			    unsigned long arg)
{
	return adsp_misc_do_ioctl(file, command, (void __user *)arg, 0);
}

#ifdef CONFIG_COMPAT
static long adsp_misc_ioctl_compat(struct file *file, unsigned int command,
				   unsigned long arg)
{
	switch (command) {
	case ADSP_MISC_IOCTL_ASYNCMSG_COMPAT:
		command = ADSP_MISC_IOCTL_ASYNCMSG;
		break;
	case ADSP_MISC_IOCTL_SYNCMSG_COMPAT:
		command = ADSP_MISC_IOCTL_SYNCMSG;
		break;
	case ADSP_MISC_IOCTL_ULTRASONICMSG_COMPAT:
		command = ADSP_MISC_IOCTL_ULTRASONICMSG;
		break;
	case ADSP_MISC_IOCTL_KARAOKEMSG_COMPAT:
		command = ADSP_MISC_IOCTL_KARAOKEMSG;
		break;
	default:
		break;
	}
	return adsp_misc_do_ioctl(file, command,
				  compat_ptr((unsigned int)arg), 1);
}
#else
#define adsp_misc_ioctl_compat NULL
#endif  // CONFIG_COMPAT

static const struct file_operations adsp_misc_fops = {
	.owner          = THIS_MODULE,
	.open           = adsp_misc_open,
	.release        = adsp_misc_release,
#ifndef CONFIG_FINAL_RELEASE
	.read           = adsp_misc_read, // 24bits BIG ENDING DATA
	.write          = adsp_misc_write, // 24bits BIG ENDING DATA
#endif
	.unlocked_ioctl = adsp_misc_ioctl, // 32bits LITTLE ENDING DATA
#ifdef CONFIG_COMPAT
	.compat_ioctl   = adsp_misc_ioctl_compat, // 32bits LITTLE ENDING DATA
#endif
};

static int adsp_miscdev_create(struct platform_device *pdev,
	struct adsp_misc_priv *pdata)
{
	pdata->misc_dev.minor = MISC_DYNAMIC_MINOR;
	pdata->misc_dev.name = "adsp_misc";
	pdata->misc_dev.fops = &adsp_misc_fops;
	pdata->misc_dev.parent = &(pdev->dev);

	return misc_register(&pdata->misc_dev);
}

static void adsp_misc_parse_dtsi(struct device_node *np,
	struct adsp_misc_priv *pdata)
{
	int ret;
	const char *rx_port_id_str = "rx_port_id";
	const char *tx_port_id_str = "tx_port_id";
	const char *cali_max_limit_str = "cali_max_limit";
	const char *cali_min_limit_str = "cali_min_limit";
	const char *smartpa_num = "smartpa_num";

	ret = of_property_read_u32(np, rx_port_id_str, &pdata->rx_port_id);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			rx_port_id_str);
		pdata->rx_port_id = AFE_PORT_ID_RX_DEFALUT;
	}

	ret = of_property_read_u32(np, tx_port_id_str, &pdata->tx_port_id);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			tx_port_id_str);
		pdata->tx_port_id = AFE_PORT_ID_TX_DEFALUT;
	}

	ret = of_property_read_u32(np, cali_max_limit_str,
			&pdata->cali_max_limit);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			cali_max_limit_str);
		pdata->cali_max_limit = CALI_VALUE_MAX_DEFALUT;
	}

	ret = of_property_read_u32(np, cali_min_limit_str,
			&pdata->cali_min_limit);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			cali_min_limit_str);
		pdata->cali_min_limit = CALI_VALUE_MIN_DEFALUT;
	}

	ret = of_property_read_u32(np, smartpa_num,
			&pdata->smartpa_num);
	if (ret) {
		hwlog_warn("%s: could not find %s entry in dt\n", __func__,
			smartpa_num);
		pdata->smartpa_num = SMARTPA_NUM_DEFALUT;
	}
}

static const struct of_device_id adsp_misc_child_match_table[] = {
	{
		.compatible = "huawei,tfa_smartamp",
		.data = NULL,
	},
	{
		.compatible = "huawei,awinic_smartamp",
		.data = NULL,
	},
	{
		.compatible = "huawei,ti_smartamp",
		.data = NULL,
	},
	{
		.compatible = "huawei,customize_smartamp",
		.data = NULL
	},
};

static int adsp_misc_probe(struct platform_device *pdev)
{
	struct adsp_misc_priv *pdata = NULL;
	int ret = 0;

	struct device_node *np = pdev->dev.of_node;

	pdata = kzalloc(sizeof(struct adsp_misc_priv), GFP_KERNEL);
	if (pdata == NULL) {
		hwlog_err("pdata is NULL\n");
		return -ENOMEM;
	}

	if (np == NULL) {
		hwlog_err("np is NULL\n");
		goto probe_err_out;
	}

	adsp_misc_parse_dtsi(np, pdata);
	g_adsp_misc_pdata = pdata;
	ret = adsp_miscdev_create(pdev, pdata);
	if (ret != 0) {
		hwlog_err("%s: register miscdev failed:%d\n", __func__, ret);
			goto probe_err_out;
	}
	mutex_init(&(pdata->adsp_misc_mutex));
	platform_set_drvdata(pdev, pdata);
	of_platform_populate(np, adsp_misc_child_match_table, NULL, &pdev->dev);

	pr_err("%s: misc dev registed success\n", __func__);
	return 0;

probe_err_out:
	kfree(pdata);
	g_adsp_misc_pdata = NULL;
	return ret;
}

static int adsp_misc_remove(struct platform_device *pdev)
{
	struct adsp_misc_priv *pdata = platform_get_drvdata(pdev);

	if (pdata == NULL)
		return 0;

	mutex_destroy(&(pdata->adsp_misc_mutex));
	misc_deregister(&pdata->misc_dev);
	kfree(pdata);
	g_adsp_misc_pdata = NULL;
	return 0;
}

static const struct of_device_id adsp_misc_match_table[] = {
	{ .compatible = "hw,hw_adsp_misc", },
	{ },
};


static struct platform_driver adsp_misc_driver = {
	.driver = {
		.name = "hw_adsp_misc",
		.owner = THIS_MODULE,
		.of_match_table = adsp_misc_match_table,
	},

	.probe = adsp_misc_probe,
	.remove = adsp_misc_remove,
};

static int __init adsp_misc_init(void)
{
	return platform_driver_register(&adsp_misc_driver);
}

static void __exit adsp_misc_exit(void)
{
	platform_driver_unregister(&adsp_misc_driver);
}

fs_initcall(adsp_misc_init);
module_exit(adsp_misc_exit);

/* lint -e753 */
MODULE_DESCRIPTION("Huawei adsp_misc driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
