/*
 * scharger_v600_base.c
 *
 * HI6526 charger basic common inner api
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
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

#include "scharger_v600.h"

static int scp_adapter_reg_read(struct hi6526_device_info *di, u8 *val, u8 reg);
static int scp_adapter_reg_write(struct hi6526_device_info *di, u8 val, u8 reg);

/* record SchargerV600 i2c trans error, and when need, notify it */
static void scharger_i2c_err_monitor(struct hi6526_device_info *di)
{
	di->i2c_err_cnt++;
	if (di->i2c_err_cnt >= I2C_ERR_MAX_COUNT)
		di->i2c_err_cnt = RESET_VAL_ZERO;
}

static int __switch_page(struct hi6526_device_info *di, u16 reg)
{
	u8 page_idx;
	int ret;

	page_idx = (reg >> 8) & 0xff; /* 8: the length of register */
	/* 0x80: page size */
	if ((reg >= 0x80) && (page_idx != di->i2c_reg_page)) {
		ret = i2c_smbus_write_byte_data(di->client, REG_PAGE_SELECT,
						page_idx);
		if (!ret)
			di->i2c_reg_page = page_idx;
		else
			return ret;
	}
	return 0;
}

int hi6526_write_block(struct hi6526_device_info *di, u16 reg, u8 *value, unsigned int num_bytes)
{
	int ret;

	if (di == NULL || value == NULL) {
		scharger_err("%s input is NULL!\n", __func__);
		return -ENOMEM;
	}

	rt_mutex_lock(&di->i2c_lock);

	ret = __switch_page(di, reg);
	if (ret) {
		scharger_err("i2c_write page fail\n");
		scharger_i2c_err_monitor(di);
		rt_mutex_unlock(&di->i2c_lock);
		return -EIO;
	}

	if (num_bytes > 1) {
		/* reg & 0xff: get the lower 8 bits of reg */
		ret = i2c_smbus_write_i2c_block_data(di->client, reg & 0xff,
						     num_bytes, value);
	} else {
		u8 data = *value;

		ret = i2c_smbus_write_byte_data(di->client, reg & 0xff, data);
	}
	if (ret) {
		scharger_err("i2c_write failed to transfer all messages\n");
		scharger_i2c_err_monitor(di);
	}

	rt_mutex_unlock(&di->i2c_lock);

	return ret;
}

int hi6526_read_block(struct hi6526_device_info *di, u16 reg, u8 *value, unsigned int num_bytes)
{
	int ret;

	if (di == NULL || value == NULL) {
		scharger_err("%s input is NULL!\n", __func__);
		return -ENOMEM;
	}

	rt_mutex_lock(&di->i2c_lock);

	ret = __switch_page(di, reg);
	if (ret) {
		scharger_err("i2c_write page fail\n");
		scharger_i2c_err_monitor(di);
		rt_mutex_unlock(&di->i2c_lock);
		return -EIO;
	}
	if (num_bytes > 1) {
		/* reg & 0xff: get the lower 8 bits of reg */
		ret = i2c_smbus_read_i2c_block_data(di->client, reg & 0xff,
						    num_bytes, value);
	} else {
		ret = i2c_smbus_read_byte_data(di->client, reg & 0xff);
		if (ret >= 0)
			*(u8 *)value = (u8)(u32)ret;
	}

	if (ret < 0) {
		scharger_err("i2c_read failed to transfer all messages\n");
		scharger_i2c_err_monitor(di);
	} else {
		ret = 0;
	}

	rt_mutex_unlock(&di->i2c_lock);

	return ret;
}

/* 0-success or others-fail */
int hi6526_write(struct hi6526_device_info *di, u16 reg, u8 value)
{
	return hi6526_write_block(di, reg, &value, 1);
}

/* 0-success or others-fail */
int hi6526_read(struct hi6526_device_info *di, u16 reg, u8 *value)
{
	if (value == NULL) {
		scharger_err("%s input is NULL!\n", __func__);
		return -ENOMEM;
	}
	return hi6526_read_block(di, reg, value, 1);
}

/* 0-success or others-fail */
int hi6526_write_mask(struct hi6526_device_info *di, u16 reg, u8 mask, u8 shift, u8 value)
{
	int ret;
	u8 val = 0;

	ret = hi6526_read(di, reg, &val);
	if (ret < 0)
		return ret;

	val &= ~mask;
	val |= ((value << shift) & mask);

	ret = hi6526_write(di, reg, val);

	return ret;
}

/* 0-success or others-fail */
int hi6526_read_mask(struct hi6526_device_info *di, u16 reg, u8 mask, u8 shift, u8 *value)
{
	int ret;
	u8 val = 0;

	if (value == NULL) {
		scharger_err("%s input is NULL!\n", __func__);
		return -ENOMEM;
	}
	ret = hi6526_read(di, reg, &val);
	if (ret < 0)
		return ret;
	val &= mask;
	val >>= shift;
	*value = val;

	return 0;
}

/* return value: 0-success or others-fail */
static int hi6526_set_adc_channel(struct hi6526_device_info *di, u32 channel)
{
	u8 val_l = 0x00;
	u8 val_h = 0x00;

	if (channel < CHG_ADC_CH_L_MAX) {
		if (channel == CHG_ADC_CH_IBUS)
			val_h |= (1 << (CHG_ADC_CH_IBUS_REF - CHG_ADC_CH_L_MAX));
		val_l = (1 << channel);
	} else {
		val_h = (1 << (channel - CHG_ADC_CH_L_MAX));
	}
	(void)hi6526_write(di, CHG_ADC_CH_SEL_L, val_l);
	(void)hi6526_write(di, CHG_ADC_CH_SEL_H, val_h);

	return 0;
}

/* return value: 0-success or others-fail */
static int hi6526_adc_enable(struct hi6526_device_info *di, u32 enable)
{
	if (enable) {
		(void)hi6526_write_mask(di, CHG_ADC_START_REG, CHG_ADC_START_MSK,
			CHG_ADC_START_SHIFT, FALSE);
		(void)hi6526_write(di, CHG_ADC_CTRL1, CHG_ADC_CTRL1_DEFAULT_VAL);
		/* 0x29 bit4 set 1, switch to hkadc */
		(void)hi6526_write_mask(di, CHG_ADC_CTRL_REG, CHG_ADC_HKADC_MSK,
			CHG_ADC_HKADC_SHIFT, true);
		(void)hi6526_write(di, CHG_ADC_CTRL_REG, 0x90); /* adc en */
	} else {
		/* 0x29 bit4 set0, switch hkadc to adc */
		(void)hi6526_write_mask(di, CHG_ADC_CTRL_REG, CHG_ADC_HKADC_MSK,
			CHG_ADC_HKADC_SHIFT, false);
		mdelay(1);
		(void)hi6526_write(di, CHG_ADC_CTRL_REG, 0x00);
	}

	return 0;
}

static int hi6526_get_adc_conv_status(struct hi6526_device_info *di, u8 *value)
{
	int ret;

	ret = hi6526_read(di, CHG_ADC_CONV_STATUS_REG, value);
	if (ret || (*value & CHG_PULSE_NO_CHG_FLAG_MSK)) {
		*value = 0;
		return -1;
	}

	*value = !!(*value & CHG_ADC_CONV_STATUS_MSK);
	return 0;
}

void hi6526_set_adc_acr_mode(int enable)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return;
	}

	if (enable)
		(void)__hi6526_get_chip_temp(di, &di->chip_temp);

	mutex_lock(&di->adc_conv_lock);
	if (enable) {
		(void)hi6526_write(di, CHG_ADC_CTRL_REG, CHG_ADC_CTRL_DEFAULT_VAL);
		(void)hi6526_write(di, SOH_SCHARGER_HKADC_H,
			SOH_SCHARGER_H_ACR_CHANNEL);
		(void)hi6526_write(di, SOH_SCHARGER_HKADC_L,
			SOH_SCHARGER_L_ACR_CHANNEL);
		(void)hi6526_write(di, CHG_ADC_CTRL1, CHG_ADC_CTRL1_ACR_VAL);
		di->chg_mode = SOH_MODE;
	} else {
		di->chg_mode = NONE;
	}
	mutex_unlock(&di->adc_conv_lock);
}

int hi6526_adc_loop_enable(struct hi6526_device_info *di, int enable)
{
	if (enable && !di->loop_enabled) {
		/* 0x2B is assigned to 0x67 */
		(void)hi6526_write(di, CHG_ADC_CTRL1, CHG_ADC_CTRL1_DEFAULT_VAL);
		/* sel ibus_ref/TSBAT/TSBUS/TSCHIP, 0x2c is assigned 0x39 */
		(void)hi6526_write(di, CHG_ADC_CH_SEL_H, 0x39);
		/* sel vusb/ibas/vbas/vout/vbat/ibat, 0x2d is assigned 0x3f */
		(void)hi6526_write(di, CHG_ADC_CH_SEL_L, 0x3F);
		/* loop, 0x29 = 0x30 enable loop and hkadc */
		(void)hi6526_write(di, CHG_ADC_CTRL_REG, 0x30);
		(void)hi6526_write(di, CHG_ADC_CTRL_REG, 0xB0); /* enable adc */
		/* start conver, 0x2A is assigned to 0x01 */
		(void)hi6526_write(di, CHG_ADC_START_REG, 0x01);
		di->loop_enabled = 1;
		mdelay(2); /* 2: need 2mS for next read */
	} else if (!enable && di->loop_enabled) {
		/* disable start conver, 0x2A is assigned to 0x00 */
		(void)hi6526_write(di, CHG_ADC_START_REG, 0x00);
		/* 0x29 bit4 set0, switch hkadc to adc */
		(void)hi6526_write_mask(di, CHG_ADC_CTRL_REG, CHG_ADC_HKADC_MSK,
			CHG_ADC_HKADC_SHIFT, false);
		/* 0x29 bit5 set0, disable loop mode */
		(void)hi6526_write_mask(di, CHG_ADC_CTRL_REG, CHG_ADC_LOOP_MSK,
			CHG_ADC_LOOP_SHIFT, false);
		mdelay(1);
		/* disable adc en, 0x29 is assigned to 0 */
		(void)hi6526_write(di, CHG_ADC_CTRL_REG, 0x0);
		di->loop_enabled = 0;
	}

	return 0;
}

static int wait_for_adc(struct hi6526_device_info *di, u8 reg)
{
	int ret;
	u8 reg2 = reg;
	int i;

	for (i = 0; i < 10; i++) { /* 10: 10 times according to chip set */
		ret = hi6526_get_adc_conv_status(di, &reg2);
		if (ret) {
			scharger_err("HI6526 read ADC CONV STAT fail!\n");
			continue;
		}
		/* if ADC Conversion finished, hkadc_valid bit will be 1 */
		if (reg2 == 1)
			break;
		msleep(1);
	}
	return i;
}

int hi6526_get_adc_value(struct hi6526_device_info *di, u32 chan, u32 *data)
{
	int rets[RET_SIZE_8] = {0};
	int ret;
	u8 lvc_mode = 0;
	u8 sc_mode = 0;
	u32 data_tmp;
	int i;
	u8 adc_data[2] = {0}; /* 2: length of adc */

	if (di == NULL || data == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -ENOMEM;
	}
	mutex_lock(&di->adc_conv_lock);
	if (di->chg_mode == SOH_MODE) {
		scharger_err("%s acr or dcr calculating!\n", __func__);
		mutex_unlock(&di->adc_conv_lock);
		return 0;
	}
	if (!di->get_adc_value_enable &&
		(charge_get_charger_type() == CHARGER_REMOVED)) {
		mutex_unlock(&di->adc_conv_lock);
		return 0;
	}
	__pm_stay_awake(di->adc_wakelock);
	(void)hi6526_read_mask(di, LVC_CHG_MODE_REG, LVC_CHG_MODE_MASK,
		LVC_CHG_MODE_SHIFT, &lvc_mode);
	(void)hi6526_read_mask(di, SC_CHG_MODE_REG, SC_CHG_MODE_MASK,
		SC_CHG_MODE_SHIFT, &sc_mode);

	/* multi-channel & loop mode */
	if ((di->param_dts.adc_single_mode_en == 0) && (lvc_mode || sc_mode)) {
		hi6526_adc_loop_enable(di, CHG_ADC_EN);

		/* request data */
		(void)hi6526_write(di, CHG_ADC_RD_SEQ, 0x01);
		/* 2: the addr increment step is 2 */
		rets[0] = hi6526_read(di, CHG_ADC_DATA_L_REG + (chan * 2), &adc_data[0]);
		rets[1] = hi6526_read(di, CHG_ADC_DATA_H_REG + (chan * 2), &adc_data[1]);
		/* 8: reg len is 8 */
		/* adc_data[1] & 0x3f:get lower 6 bits of adc_data[1] */
		*data = (u32)adc_data[0] | ((u32)(adc_data[1] & 0x3f) << 8);

		if ((chan == CHG_ADC_CH_VBUS || chan == CHG_ADC_CH_VUSB) && lvc_mode) {
			data_tmp = *data * 4 / 10 + 1500; /* transfer code to val */
			if (data_tmp > HI6526_MAX_U32) {
				scharger_err("%s data = %u, out of u32 range, and is now set to 0!\n",
					     __func__, data_tmp);
				*data = 0;
			} else {
				*data = data_tmp;
			}
		}

		__pm_relax(di->adc_wakelock);
		mutex_unlock(&di->adc_conv_lock);
		return 0;
	}

	/* signel-channel & signel mode */
	rets[2] = hi6526_adc_loop_enable(di, CHG_ADC_DIS);
	rets[3] = hi6526_set_adc_channel(di, chan);
	rets[4] = hi6526_adc_enable(di, CHG_ADC_EN);

	rets[5] = hi6526_write_mask(di, CHG_ADC_START_REG, CHG_ADC_START_MSK,
		CHG_ADC_START_SHIFT, TRUE);
	ret = (rets[0] || rets[1] || rets[2] || rets[3] || rets[4] || rets[5]);
	if (ret) {
		scharger_err("set covn fail! ret =%d\n", ret);
		(void)hi6526_adc_enable(di, CHG_ADC_DIS);
		__pm_relax(di->adc_wakelock);
		mutex_unlock(&di->adc_conv_lock);
		return -1;
	}
	/* The conversion result is ready after tCONV, max 10ms */
	i = wait_for_adc(di, 0);
	if (i == 10) { /* 10: succ flag */
		scharger_err("Wait for ADC CONV timeout!\n");
		(void)hi6526_adc_enable(di, CHG_ADC_DIS);
		__pm_relax(di->adc_wakelock);
		mutex_unlock(&di->adc_conv_lock);
		return -1;
	}
	/* 2: addr increment step is 2 */
	rets[0] = hi6526_read(di, CHG_ADC_DATA_L_REG + (chan * 2), &adc_data[0]);
	rets[1] = hi6526_read(di, CHG_ADC_DATA_H_REG + (chan * 2), &adc_data[1]);
	/* 8: reg len */
	*data = (u32)adc_data[0] | ((u32)(adc_data[1] & 0x3f) << 8);
	rets[2] = hi6526_adc_enable(di, CHG_ADC_DIS);
	ret = (rets[0] || rets[1] || rets[2]);
	if (ret) {
		scharger_err("[%s]get ibus_ref_data fail,ret:%d\n",
							__func__, ret);
		(void)hi6526_adc_enable(di, CHG_ADC_DIS);
		__pm_relax(di->adc_wakelock);
		mutex_unlock(&di->adc_conv_lock);
		return -1;
	}
	__pm_relax(di->adc_wakelock);
	mutex_unlock(&di->adc_conv_lock);
	return 0;
}

/*
 * Description:   get chip temperature
 * Return:        temerature (°)
 * Remart:        [VPTAT_ACR] equals to [2500] times [code(denary)] / [4095 (mV)]
 *                [temp] dquals to [1308.518] minus [4.2392] times [T]
 */
int __hi6526_get_chip_temp(struct hi6526_device_info *di, int *temp)
{
	int ret;
	u32 val = 0;

	if (temp == NULL) {
		scharger_err("%s input temp is NULL!\n", __func__);
		return -ENOMEM;
	}
	ret = hi6526_get_adc_value(di, CHG_ADC_CH_TSCHIP, &val);
	if (ret) {
		scharger_err("[%s]get vbat_data fail,ret:%d\n", __func__, ret);
		return -1;
	}

	if (val == 0)
		return -1;
	/*
	 * code val to actual val
	 * 2500: total range of temperature;
	 * 4096: 12 bits to record temperature, 2^12 = 4096;
	 * 13085180: base value, for chip set
	 * 10000: conversion scale
	 * 42392: the reference value
	 */
	val = (2500 * val) / 4096;
	*temp = (int)(13085180 - val * 10000) / (42392);
	return 0;
}

int hi6526_get_chip_temp(int *temp, void *dev_data)
{
	struct hi6526_device_info *di = (struct hi6526_device_info *)dev_data;

	if (di == NULL || temp  == NULL)
		return -1;
	if (di->chg_mode == SOH_MODE) {
		*temp = di->chip_temp;
		return 0;
	} else if ((di->param_dts.adc_single_mode_en == 0) &&
		((di->chg_mode == LVC) || (di->chg_mode == SC))) {
		*temp = 25; /* 25: set to 25 arbitarily */
		return 0;
	}

	(void)__hi6526_get_chip_temp(di, temp);
	scharger_inf("[%s]:%d\n", __func__, *temp);

	return 0;
}

int hi6526_get_vbat_execute(void *dev_data)
{
	int ret;
	u32 val = 0;
	struct hi6526_device_info *di = (struct hi6526_device_info *)dev_data;

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}
	ret = hi6526_get_adc_value(di, CHG_ADC_CH_VBAT, &val);
	if (ret) {
		scharger_err("[%s]get vbat_data fail,ret:%d\n", __func__, ret);
		return -1;
	}

	return val;
}

int hi6526_get_vbat(struct charger_device *dev)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}
	return hi6526_get_vbat_execute(di);
}

int hi6526_get_ibat(int *ibat_ma, void *dev_data)
{
	int ret;
	u32 val = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL || ibat_ma == NULL) {
		scharger_err("[%s]input is NULL, ibat_ma is assigned to 0\n",
			     __func__);
		return -1;
	}

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_IBAT, &val);
	if (ret) {
		scharger_err("[%s]get Ibat_data fail,ret:%d, ibat_ma is assigned to 0\n",
			     __func__, ret);
		/* assigned to 0 when fails, same as where it is called */
		*ibat_ma = 0;
		return -1;
	}

	if (di->param_dts.r_coul_mohm == R_MOHM_10)
		val /= (R_MOHM_10 / R_MOHM_5);

	if (di->param_dts.r_coul_mohm == R_MOHM_1)
		val *= (R_MOHM_2 / R_MOHM_1);

	*ibat_ma = (int)val;
	return 0;
}

int hi6526_get_vbus_mv(struct charger_device *dev, unsigned int *vbus_mv)
{
	int ret;
	u32 result = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL || vbus_mv == NULL) {
		scharger_err("[%s]input is NULL, vbus_mv is assigned to 0\n",
			     __func__);
		return -1;
	}

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_VBUS, &result);
	if (ret) {
		scharger_err("[%s]get vbus_mv fail,ret:%d, vbus_mv is assigned to 0\n",
			     __func__, ret);
		/* assigned to 0 when fails, same as where it is called */
		*vbus_mv = 0;
		return -1;
	}

	if ((charge_get_charger_type() == CHARGER_REMOVED) &&
		(result < VBUS_2600_MV))
		result = 0;

	*vbus_mv = (unsigned int)result;
	return ret;
}

int hi6526_get_vbus_mv2(int *vbus_mv, void *dev_data)
{
	int ret;
	u32 result = 0;
	struct hi6526_device_info *di = (struct hi6526_device_info *)dev_data;

	if (di == NULL || vbus_mv == NULL) {
		scharger_err("[%s]get vbus_mv fail, vbus_mv is assigned to 0\n",
			     __func__);
		return -1;
	}

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_VBUS, &result);
	if (ret) {
		scharger_err("[%s]get vbus_mv fail,ret:%d, vbus_mv is assigned to 0\n",
			     __func__, ret);
		/* assigned to 0 when fails, same as where it is called */
		*vbus_mv = 0;
		return -1;
	}

	if ((charge_get_charger_type() == CHARGER_REMOVED) &&
		(result < VBUS_2600_MV))
		result = 0;

	*vbus_mv = (int)result;
	return ret;
}

int hi6526_get_vout(int *vout_mv, void *dev_data)
{
	int ret;
	u32 result = 0;
	struct hi6526_device_info *di = (struct hi6526_device_info *)dev_data;

	if (di == NULL || vout_mv == NULL) {
		scharger_err("[%s]get vout_mv fail, vout_mv is assigned to 0\n",
			     __func__);
		return -1;
	}

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_VOUT, &result);
	if (ret) {
		scharger_err("[%s]get vout_mv fail,ret:%d, vout_mv is assigned to 0\n",
			     __func__, ret);
		/* assigned to 0 when fails, same as where it is called */
		*vout_mv = 0;
		return -1;
	}

	*vout_mv = (int)result;
	return ret;
}

int hi6526_get_vusb(int *vusb_mv)
{
	int ret;
	u32 result = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL || vusb_mv == NULL) {
		scharger_err("[%s]get vusb_mv fail, vusb_mv is assigned to 0\n",
			     __func__);
		return -1;
	}

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_VUSB, &result);
	if (ret) {
		scharger_err("[%s]get vusb_mv fail,ret:%d, vusb_mv is assigned to 0\n",
			     __func__, ret);
		/* assigned to 0 when fails, same as where it is called */
		*vusb_mv = 0;
		return -1;
	}

	*vusb_mv = (int)result;
	return ret;
}

/* NTC Table */
static int t_v_table[][2] = {
	{ -273, 4095 }, { -40, 3764 }, { -36, 3689 }, { -32, 3602 }, { -28, 3500 },
	{ -24, 3387 }, { -20, 3261 }, { -16, 3122 }, { -12, 2973 }, { -8, 2814 },
	{ -4, 2650 }, { 0, 2480 }, { 4, 2308 }, { 8, 2136 }, { 12, 1967 },
	{ 16, 1803 }, { 20, 1646 }, { 24, 1497 }, { 28, 1360 }, { 32, 1230 },
	{ 36, 1111 }, { 40, 1001}, { 44, 903 }, { 48, 812 }, { 52, 729 },
	{ 56, 655 }, { 60, 590 }, { 64, 531 }, { 74, 406 }, { 84, 313 },
	{ 125, 110 }, { 0, 0 },
};

static int charger_adc_to_temp(int temp_volt)
{
	int temprature = 0;
	int i;

	if (temp_volt >= t_v_table[0][1])
		return t_v_table[0][0];

	if (temp_volt <= t_v_table[T_V_ARRAY_LENGTH - 1][1])
		return t_v_table[T_V_ARRAY_LENGTH - 1][0];

	/* else */
	for (i = 0; i < T_V_ARRAY_LENGTH; i++) {
		if (temp_volt == t_v_table[i][1])
			return t_v_table[i][0];

		if (temp_volt > t_v_table[i][1])
			break;
	}
	if (i == 0)
		return t_v_table[0][0];

	if ((t_v_table[i][1] - t_v_table[i - 1][1]) != 0)
		temprature = t_v_table[i - 1][0] +
			(long)(temp_volt - t_v_table[i - 1][1]) *
			(t_v_table[i][0] - t_v_table[i - 1][0]) /
			(t_v_table[i][1] - t_v_table[i - 1][1]);

	return temprature;
}

int hi6526_get_tsbat(int *ts_bat)
{
	int ret;
	u32 adc_code = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di || !ts_bat) {
		scharger_err("[%s]get ts_bat fail, ts_bat is assigned to 0\n",
			__func__);
		return -1;
	}

	if (di->param_dts.adc_tsbat_support == 0) {
		*ts_bat = -1;
		return 0;
	}

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_TSBAT, &adc_code);
	if (ret) {
		scharger_err("[%s]get ts_bat fail,ret:%d\n", __func__, ret);
		/* assigned to 0 when fails, same as where it is called */
		*ts_bat = 0;
		return -1;
	}

	*ts_bat = charger_adc_to_temp(adc_code);
	scharger_dbg("[%s]adc_code %d, ts_bat %d\n",
		__func__, adc_code, *ts_bat);
	return 0;
}

int hi6526_get_vusb_force(struct charger_device *dev, int *vusb_mv)
{
	int ret;
	u32 result = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL || vusb_mv == NULL) {
		scharger_err("[%s]get vusb_mv fail, vusb_mv is assigned to 0\n",
			     __func__);
		return -1;
	}

	di->get_adc_value_enable = true;
	ret = hi6526_get_adc_value(di, CHG_ADC_CH_VUSB, &result);
	if (ret) {
		scharger_err("[%s]get vusb_mv fail,ret:%d, vusb_mv is assigned to 0\n",
			     __func__, ret);
		/* assigned to 0 when fails, same as where it is called */
		*vusb_mv = 0;
		di->get_adc_value_enable = false;
		return -1;
	}

	di->get_adc_value_enable = false;
	*vusb_mv = (int)result;
	return ret;
}

static int hi6526_get_dp_dm_volt(struct hi6526_device_info *di, int ch)
{
	int ret;
	u32 vol_mv = 0;

	(void)hi6526_write_mask(di, DPDM_PMID_SEL_REG, DPDM_PMID_SEL_MASK,
		DPDM_PMID_SEL_SHIFT, SEL_DPDM);
	(void)hi6526_write_mask(di, CHG_ADC_APPDET_REG, CHG_ADC_APPDET_CHSEL_MSK,
		CHG_ADC_APPDET_CHSEL_SHIFT, ch);
	(void)hi6526_write_mask(di, CHG_ADC_APPDET_REG, APPLE_DETECT_MASK,
		APPLE_DETECT_SHIFT, APPLE_DETECT_ENABLE);

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_DPDM, &vol_mv);
	if (ret != 0)
		scharger_err("[%s] hi6526_get_adc_value error\n", __func__);
	(void)hi6526_write_mask(di, CHG_ADC_APPDET_REG, APPLE_DETECT_MASK,
		APPLE_DETECT_SHIFT, APPLE_DETECT_DISABLE);
	/*
	 * transfer from code val to actual val
	 * 3750: vol gain
	 * 4096: total gear, namely 2^12
	 */
	vol_mv = vol_mv * 3750 / 4096;

	scharger_dbg("[%s] %u\n", __func__, vol_mv);

	return vol_mv;
}

static void hi6526_dp_res_det_iqsel(struct hi6526_device_info *di, int det_current)
{
	u8 code;

	if (det_current == IQ_SEL_1UA) {
		code = 0x00; /* 0x00: IQ setlect 1uA */
	} else if (det_current == IQ_SEL_10UA) {
		code = 0x01; /* 0x01: IQ setlect 10uA */
	} else if (det_current == IQ_SEL_100UA) {
		code = 0x03; /* 0x03: IQ setlect 100uA */
	} else {
		code = 0x03; /* 0x03: IQ setlect 100uA */
		scharger_inf("[%s] det_current is error %d\n",
			     __func__, det_current);
	}

	(void)hi6526_write_mask(di, DP_RES_IQSEL_REG, DP_RES_IQSEL_MASK,
		DP_RES_IQSEL_SHIFT, det_current);
}

int hi6526_get_dp_res(struct hi6526_device_info *di)
{
	u32 code = 0;
	int res;
	int ret;
	int dp_volt;
	int irsel = IQ_SEL_1UA;

	do {
		dp_volt = hi6526_get_dp_dm_volt(di, DP_DET);
		irsel = irsel * 10; /* 10: gear transfer */
		hi6526_dp_res_det_iqsel(di, irsel);
	} while (dp_volt < DP_VOLT_200MV && irsel <= IQ_SEL_100UA);

	(void)hi6526_write_mask(di, DPDM_PMID_SEL_REG, DPDM_PMID_SEL_MASK,
		DPDM_PMID_SEL_SHIFT, SEL_DPDM);
	(void)hi6526_write_mask(di, CHG_ADC_APPDET_REG, CHG_ADC_APPDET_CHSEL_MSK,
		CHG_ADC_APPDET_CHSEL_SHIFT, DP_DET);
	(void)hi6526_write_mask(di, CHG_ADC_APPDET_REG, APPLE_DETECT_MASK,
		APPLE_DETECT_SHIFT, APPLE_DETECT_DISABLE);

	(void)hi6526_write_mask(di, CHG_ADC_APPDET_REG, CHG_ADC_DP_RES_DET_MSK,
		CHG_ADC_DP_RES_DET_SHIFT, DP_RES_DET_EN);

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_DPDM, &code);
	if (ret != 0)
		scharger_err("[%s] hi6526_get_adc_value error\n", __func__);
	(void)hi6526_write_mask(di, CHG_ADC_APPDET_REG, CHG_ADC_DP_RES_DET_MSK,
		CHG_ADC_DP_RES_DET_SHIFT, DP_RES_DET_DIS);
	hi6526_dp_res_det_iqsel(di, IQ_SEL_1UA);
	/*
	 * code val to actual val
	 * 2500000: dp gain
	 * 4096: total gear
	 */
	res = (long)code * 2500000 / 4096 / irsel;

	scharger_dbg("[%s] %d\n", __func__, res);

	return 0;
}

int hi6526_get_ibus_ma_exec(struct hi6526_device_info *di, u32 *ibus)
{
	int ret;
	u8 ilimit = 0;
	u32 ibus_ref = 0;

	if (!di || !ibus)
		return -1;

	ret = hi6526_get_adc_value(di, CHG_ADC_CH_IBUS, ibus);
	if (ret) {
		scharger_err("[%s]get ibus_data fail,ret:%d\n", __func__, ret);
		return -1;
	}

	hi6526_get_adc_value(di, CHG_ADC_CH_IBUS_REF, &ibus_ref);
	/* 100: ibus is illegal when ibus_ref less than 100 */
	if ((int)ibus_ref < 100) {
		scharger_err("[%s]ibus_ref:%d\n", __func__, (int)ibus_ref);
		*ibus = 0;
	}

	if ((*ibus == IBUS_INVALID_VAL) ||
		(charge_get_charger_type() == CHARGER_REMOVED))
		*ibus = 0;

	if (di->chg_mode != LVC && di->chg_mode != SC) {
		(void)hi6526_read_mask(di, CHG_INPUT_SOURCE_REG, CHG_ILIMIT_MSK,
			CHG_ILIMIT_SHIFT, &ilimit);
		if (ilimit < CHG_ILIMIT_600MA)
			*ibus = *ibus / CHG_IBUS_DIV;
	}

	return *ibus;
}

int hi6526_get_ibus_ma(struct charger_device *dev, u32 *ibus)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}

	return hi6526_get_ibus_ma_exec(di, ibus);
}

/*
 * Parameters: efuse_id: efuse1/efuse2/efuse3
 *             offset: offset byte of efuse
 *             value: efuse value
 * return value: 0-success or others-fail
 */
int hi6526_efuse_read(int efuse_id, u8 offset, u8 *value, u8 pre_cali)
{
	int ret = 0;
	int efuse_select;
	u16 set_reg, get_reg;
	u8 set_shift, set_mask;
	struct hi6526_device_info *di = get_hi6526_device();

	if (!value || !di) {
		scharger_err("%s input value is NULL\n", __func__);
		return -ENOMEM;
	}
	if (efuse_id == EFUSE1) {
		set_reg = EFUSE1_SEL_REG;
		set_shift = EFUSE1_SEL_SHIFT;
		set_mask = EFUSE1_SEL_MASK;
		get_reg = EFUSE1_RDATA_REG;
		efuse_select = EFUSE1_SELECT;
	} else if (efuse_id == EFUSE2) {
		set_reg = EFUSE2_SEL_REG;
		set_shift = EFUSE2_SEL_SHIFT;
		set_mask = EFUSE2_SEL_MASK;
		get_reg = EFUSE2_RDATA_REG;
		efuse_select = EFUSE2_SELECT;
	} else if (efuse_id == EFUSE3) {
		set_reg = EFUSE3_SEL_REG;
		set_shift = EFUSE3_SEL_SHIFT;
		set_mask = EFUSE3_SEL_MASK;
		get_reg = EFUSE3_RDATA_REG;
		efuse_select = EFUSE3_SELECT;
	} else if (efuse_id == EFUSE4) {
		set_reg = EFUSE4_SEL_REG;
		set_shift = EFUSE4_SEL_SHIFT;
		set_mask = EFUSE4_SEL_MASK;
		get_reg = EFUSE4_RDATA_REG;
		efuse_select = EFUSE4_SELECT;
	} else {
		return -1;
	}

	if (pre_cali)
		ret += hi6526_write_mask(di, EFUSE_SEL_REG,
			EFUSE_PDOB_SEL_MASK,
			EFUSE_PDOB_SEL_SHIFT,
			EFUSE_PDOB_SEL_CALI_EN);

	/* 0x60 bit0 write efuse id */
	ret += hi6526_write_mask(di, EFUSE_SEL_REG, EFUSE_SEL_MASK,
		EFUSE_SEL_SHIFT, efuse_select);
	/* 0x70 bit3 write 1 */
	ret += hi6526_write_mask(di, EFUSE_EN_REG, EFUSE_EN_MASK,
		EFUSE_EN_SHIFT, EFUSE_EN);
	if (efuse_id == EFUSE4)
		/* 0x70 bit7 write 1 */
		ret += hi6526_write_mask(di, EFUSE_EN_REG,
			EFUSE4_3_4_TESTBUS_SEL_MASK,
			EFUSE4_3_4_TESTBUS_SEL_SHIFT,
			EFUSE4_4_TESTBUS_SEL);

	/*
	 * read efuse flash buffer
	 * 0x61 bit7 & bit3 write 1
	 */
	ret += hi6526_write_mask(di, EFUSE4_CFG, EFUSE4_CFG_RD_MODE_SEL_MASK,
		EFUSE4_CFG_RD_MODE_SHIFT,
		EFUSE4_CFG_RD_MODE_64BIT_FLUSH);
	ret += hi6526_write_mask(di, EFUSE4_CFG,
		EFUSE4_CFG_RD_CTRL_SEL_MASK,
		EFUSE4_CFG_RD_CTRL_SHIFT,
		EFUSE4_CFG_RD_CTRL_EN);

	ret += hi6526_write_mask(di, set_reg, set_mask, set_shift, offset);
	mdelay(1);
	ret += hi6526_read(di, get_reg, value);
	ret += hi6526_write_mask(di, EFUSE_EN_REG, EFUSE_EN_MASK,
		EFUSE_EN_SHIFT, EFUSE_DIS);
	if (efuse_id == EFUSE4)
		ret += hi6526_write_mask(di, EFUSE_EN_REG,
			EFUSE4_3_4_TESTBUS_SEL_MASK,
			EFUSE4_3_4_TESTBUS_SEL_SHIFT,
			EFUSE4_3_TESTBUS_SEL);

	/*
	 * disable efuse flash buffer
	 * 0x61 bit7 & bit3 write 0
	 */
	ret += hi6526_write_mask(di, EFUSE4_CFG,
		EFUSE4_CFG_RD_MODE_SEL_MASK,
		EFUSE4_CFG_RD_MODE_SHIFT,
		EFUSE4_CFG_RD_MODE_BIT_FLUSH);
	ret += hi6526_write_mask(di, EFUSE4_CFG,
		EFUSE4_CFG_RD_CTRL_SEL_MASK,
		EFUSE4_CFG_RD_CTRL_SHIFT,
		EFUSE4_CFG_RD_CTRL_DIS);

	/* 0x60 bit4  write 1 */
	if (pre_cali)
		ret += hi6526_write_mask(di, EFUSE_SEL_REG,
			EFUSE_PDOB_SEL_MASK,
			EFUSE_PDOB_SEL_SHIFT,
			EFUSE_PDOB_SEL_CALI_DIS);

	if (ret)
		*value = 0;

	return ret;
}

int hi6526_efuse_write(int efuse_id, u8 offset, u8 value, int pre_cali)
{
	int ret = 0;
	int efuse_select;
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}

	if (efuse_id == EFUSE1) {
		efuse_select = EFUSE1_SELECT;
	} else if (efuse_id == EFUSE2) {
		efuse_select = EFUSE2_SELECT;
	} else if (efuse_id == EFUSE3) {
		efuse_select = EFUSE3_SELECT;
	} else if (efuse_id == EFUSE4) {
		efuse_select = EFUSE4_SELECT;
	} else {
		scharger_err("%s efuse_id %d error!\n", __func__, efuse_id);
		return -1;
	}

	if (pre_cali)
		ret += hi6526_write_mask(di, EFUSE_SEL_REG,
			EFUSE_PDOB_SEL_MASK,
			EFUSE_PDOB_SEL_SHIFT,
			EFUSE_PDOB_SEL_CALI_EN);

	ret += hi6526_write_mask(di, EFUSE_SEL_REG, EFUSE_SEL_MASK,
		EFUSE_SEL_SHIFT, efuse_select);
	ret += hi6526_write_mask(di, EFUSE_PDOB_PRE_ADDR_WE,
		EFUSE_PDOB_PRE_ADDR_MASK,
		EFUSE_PDOB_PRE_ADDR_SHIFT, offset);
	ret += hi6526_write_mask(di, EFUSE_PDOB_PRE_WDATA,
		EFUSE_PDOB_PRE_WDATA_MASK,
		EFUSE_PDOB_PRE_WDATA_SHIFT, value);
	mdelay(1);
	ret += hi6526_write_mask(di, EFUSE_PDOB_PRE_ADDR_WE,
		EFUSE_PDOB_PRE_WE_MASK,
		EFUSE_PDOB_PRE_WE_SHIFT,
		EFUSE_PDOB_PRE_WE_EN);
	ret += hi6526_write_mask(di, EFUSE_PDOB_PRE_ADDR_WE,
		EFUSE_PDOB_PRE_WE_MASK,
		EFUSE_PDOB_PRE_WE_SHIFT,
		EFUSE_PDOB_PRE_WE_DIS);
	ret += hi6526_write_mask(di, EFUSE_SEL_REG,
		EFUSE_PDOB_SEL_MASK,
		EFUSE_PDOB_SEL_SHIFT,
		EFUSE_PDOB_SEL_CALI_DIS);
	if (ret)
		scharger_err("%s ret=%d!\n", __func__, ret);

	return ret;
}

int hi6526_reset_otg_watchdog_timer(void)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}

	(void)hi6526_write_mask(di, WATCHDOG_SOFT_RST_REG,
		WD_RST_N_MSK,
		WATCHDOG_TIMER_SHIFT,
		WATCHDOG_TIMER_RST);
	return 0;
}

int hi6526_reset_watchdog_timer_execute(void *dev_data)
{
	struct hi6526_device_info *di = (struct hi6526_device_info *)dev_data;
	u32 ibus = 0;

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}

	if (di->chg_mode != SC && di->chg_mode != LVC) {
		di->wdg_ibus_abn_cnt = 0;
		/* kick watchdog */
		(void)hi6526_write_mask(di, WATCHDOG_SOFT_RST_REG,
					WD_RST_N_MSK,
					WATCHDOG_TIMER_SHIFT,
					WATCHDOG_TIMER_RST);
		return 0;
	}

	hi6526_get_ibus_ma_exec(di, &ibus);
	if (di->ucp_work_first_run && ibus > IBUS_OCP_START_VAL)
		di->ucp_work_first_run = 0;

	if (ibus < IBUS_ABNORMAL_VAL && !di->ucp_work_first_run)
		di->wdg_ibus_abn_cnt++;
	else
		di->wdg_ibus_abn_cnt = 0;

	/* hi6526_get_ibus_ma have sleep() in the function, which will
	cause thread switching and cause chg_mode changes to buck */
	if ((di->chg_mode != SC) && (di->chg_mode != LVC)) {
		di->wdg_ibus_abn_cnt = 0;
		return 0;
	}

	if (di->wdg_ibus_abn_cnt >= IBUS_ABNORMAL_CNT) {
		scharger_inf("%s:cnt %d, ibus %d, chg_mode %d\n", __func__,
			di->wdg_ibus_abn_cnt, ibus, di->chg_mode);
		di->wdg_ibus_abn_cnt = 0;
		di->dc_ibus_ucp_happened = 1;
		hi6526_direct_charge_fault_handle(di, IRQ_IBUS_DC_UCP_MASK);
		hi6526_lvc_enable(0, di);
		hi6526_sc_enable(0, di);
		return 0;
	}

	return hi6526_write_mask(di, WATCHDOG_SOFT_RST_REG, WD_RST_N_MSK,
				 WATCHDOG_TIMER_SHIFT, WATCHDOG_TIMER_RST);
}

int hi6526_reset_watchdog_timer(struct charger_device *dev)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}
	return hi6526_reset_watchdog_timer_execute(di);
}

static void hi6526_opt_cfg_parse_dts(struct device_node *np,
				     struct hi6526_device_info *di)
{
	int ret;

	ret = of_property_read_u32(np, "lvc_vusb2vbus_drop_en",
				   (u32 *)&(di->param_dts.lvc_vusb2vbus_drop_en));
	if (ret) {
		scharger_err("get lvc_vusb2vbus_drop_en err,set default value\n");
		di->param_dts.lvc_vusb2vbus_drop_en = 1;
	}
	scharger_inf("%s lvc_ibat_regulator=%d\n", __func__,
		     di->param_dts.lvc_vusb2vbus_drop_en);

	ret = of_property_read_u32(np, "sc_vusb2vbus_drop_en",
				   (u32 *)&(di->param_dts.sc_vusb2vbus_drop_en));
	if (ret) {
		scharger_err("get sc_vusb2vbus_drop_en err,set default value\n");
		di->param_dts.sc_vusb2vbus_drop_en = 1;
	}
	scharger_inf("%s sc_vusb2vbus_drop_en=%d\n",
		     __func__, di->param_dts.sc_vusb2vbus_drop_en);
}

static void dc_regulator_parse_dts(struct device_node *np,
				   struct hi6526_device_info *di)
{
	int ret;

	ret = of_property_read_u32(np, "lvc_ibat_regulator",
				   (u32 *)&(di->param_dts.lvc_regulator.ibat));
	if (ret) {
		scharger_err("get lvc_ibat_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts lvc_ibat_regulator=%d\n",
		     di->param_dts.lvc_regulator.ibat);

	ret = of_property_read_u32(np, "lvc_vbat_regulator",
				   (u32 *)&(di->param_dts.lvc_regulator.vbat));
	if (ret) {
		scharger_err("get lvc_vbat_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts lvc_vbat_regulator=%d\n",
		     di->param_dts.lvc_regulator.vbat);

	ret = of_property_read_u32(np, "lvc_vout_regulator",
				   (u32 *)&(di->param_dts.lvc_regulator.vout));
	if (ret) {
		scharger_err("get lvc_vout_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts lvc_vout_regulator=%d\n",
		     di->param_dts.lvc_regulator.vout);

	ret = of_property_read_u32(np, "lvc_ibus_regulator",
				   (u32 *)&(di->param_dts.lvc_regulator.ibus));
	if (ret) {
		scharger_err("get lvc_ibus_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts lvc_ibus_regulator=%d\n",
		     di->param_dts.lvc_regulator.ibus);

	ret = of_property_read_u32(np, "sc_ibat_regulator",
				   (u32 *)&(di->param_dts.sc_regulator.ibat));
	if (ret) {
		scharger_err("get sc_ibat_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts sc_ibat_regulator=%d\n",
		     di->param_dts.sc_regulator.ibat);

	ret = of_property_read_u32(np, "sc_vbat_regulator",
				   (u32 *)&(di->param_dts.sc_regulator.vbat));
	if (ret) {
		scharger_err("get sc_vbat_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts sc_vbat_regulator=%d\n",
		di->param_dts.sc_regulator.vbat);

	ret = of_property_read_u32(np, "sc_vout_regulator",
		(u32 *)&(di->param_dts.sc_regulator.vout));
	if (ret) {
		scharger_err("get sc_vout_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts sc_vout_regulator=%d\n",
		di->param_dts.sc_regulator.vout);

	ret = of_property_read_u32(np, "sc_ibus_regulator",
		(u32 *)&(di->param_dts.sc_regulator.ibus));
	if (ret) {
		scharger_err("get sc_ibus_regulator failed\n");
		return;
	}
	scharger_inf("regulator_prase_dts sc_ibus_regulator=%d\n",
		di->param_dts.sc_regulator.ibus);
}

static void hi6526_parse_adc_dts(struct device_node *np,
	struct hi6526_device_info *di)
{
	int ret;

	ret = of_property_read_u32(np, "adc_single_mode_en",
		&(di->param_dts.adc_single_mode_en));
	if (ret)
		di->param_dts.adc_single_mode_en = 0;

	scharger_inf("prase_dts adc_single_mode_en=%d\n",
		di->param_dts.adc_single_mode_en);

	ret = of_property_read_u32(np, "adc_tsbat_support",
		&(di->param_dts.adc_tsbat_support));
	if (ret)
		di->param_dts.adc_tsbat_support = 0;

	scharger_inf("prase_dts adc_tsbat_support=%d\n",
		di->param_dts.adc_tsbat_support);
}

void hi6526_parse_dts(struct device_node *np, struct hi6526_device_info *di)
{
	int ret;
	struct device_node *batt_node = NULL;

	if (np == NULL || di == NULL) {
		scharger_err("%s input is NULL!\n", __func__);
		return;
	}
	/* 80,224: the initial current and voltage value */
	di->param_dts.bat_comp = 80;
	di->param_dts.vclamp = 224;
	di->is_board_type = BAT_BOARD_ASIC;

	power_gpio_config_output(np, "rst_gpio", "rst_gpio", &di->rst_gpio, 1);

	ret = of_property_read_u32(np, "ic_role",
		(u32 *)&(di->param_dts.ic_role));
	if (ret) {
		di->param_dts.ic_role = IC_ONE;
		scharger_inf("get ic_role err,use default value main!\n");
	}
	ret = of_property_read_u32(np, "enable_v600_buck",
		&(di->param_dts.enable_v600_buck));
	if (ret) {
		di->param_dts.enable_v600_buck = 0;
		scharger_inf("get enable_v600_buck err,use default value disable!\n");
	}
	ret = of_property_read_u32(np, "r_coul_mohm",
		(u32 *)&(di->param_dts.r_coul_mohm));
	if (ret) {
		di->param_dts.r_coul_mohm = R_MOHM_DEFAULT;
		scharger_inf("get r_coul_mohm err,use default value 2 mohm!\n");
	}
	scharger_inf("prase_dts r_coul_mohm=%d\n", di->param_dts.r_coul_mohm);

	ret = of_property_read_u32(np, "bat_comp", (u32 *)&(di->param_dts.bat_comp));
	if (ret) {
		scharger_err("get bat_comp failed\n");
		return;
	}
	scharger_inf("prase_dts bat_comp=%d\n", di->param_dts.bat_comp);

	ret = of_property_read_u32(np, "vclamp", (u32 *)&(di->param_dts.vclamp));
	if (ret) {
		scharger_err("get vclamp failed\n");
		return;
	}
	scharger_inf("prase_dts vclamp=%d\n", di->param_dts.vclamp);

	ret = of_property_read_u32(np, "fcp_support",
		(u32 *)&(di->param_dts.fcp_support));
	if (ret) {
		scharger_err("get fcp_support failed\n");
		return;
	}
	scharger_inf("prase_dts fcp_support=%d\n", di->param_dts.fcp_support);

	ret = of_property_read_u32(np, "scp_support",
		(u32 *)&(di->param_dts.scp_support));
	if (ret) {
		scharger_err("get scp_support failed\n");
		return;
	}
	scharger_inf("prase_dts scp_support=%d\n", di->param_dts.scp_support);
	ret = of_property_read_u32(np, "mask_pmic_irq_support",
		(u32 *)&(di->param_dts.mask_pmic_irq_support));
	if (ret) {
		scharger_err("get mask_pmic_irq_support failed\n");
		di->param_dts.mask_pmic_irq_support = 0;
	}
	scharger_inf("prase_dts mask_pmic_irq_support=%d\n",
		di->param_dts.mask_pmic_irq_support);

	batt_node = of_find_compatible_node(NULL, NULL, "huawei,bci_battery");
	if (batt_node != NULL) {
		if (of_property_read_u32(batt_node, "battery_board_type",
			&di->is_board_type)) {
			scharger_err("get battery_board_type fail!\n");
			di->is_board_type = BAT_BOARD_ASIC;
		}
	} else {
		scharger_err("get bci_battery fail!\n");
		di->is_board_type = BAT_BOARD_ASIC;
	}

	ret = of_property_read_u32(np, "dp_th_res",
		(u32 *)&(di->param_dts.dp_th_res));
	if (ret)
		scharger_err("get dp_th_res failed\n");

	scharger_inf("prase_dts dp_th_res=%d\n", di->param_dts.dp_th_res);

	ret = of_property_read_u32(np, "dbg_check_en",
		&(di->param_dts.dbg_check_en));
	if (ret) {
		di->param_dts.dbg_check_en = DISABLE_DBG_CHECK;
		scharger_err("get dbg_check_en failed\n");
	}
	scharger_inf("prase_dts dbg_check_en=%d\n", di->param_dts.dbg_check_en);

	hi6526_parse_adc_dts(np, di);
	dc_regulator_parse_dts(np, di);
	hi6526_opt_cfg_parse_dts(np, di);
}

static int reg_judge_v600(u8 reg_v1, u8 reg_v2, u8 reg_v3, u8 reg_v4)
{
	if ((reg_v1 & CHG_FCP_ACK) && (reg_v1 & CHG_FCP_CMDCPL) &&
		!(reg_v2 & (CHG_FCP_CRCRX | CHG_FCP_PARRX))) {
		return 0;
	} else if (((reg_v1 & CHG_FCP_CRCPAR) ||
		(reg_v3 & CHG_FCP_INIT_HAND_FAIL) ||
		(reg_v4 & CHG_FCP_ENABLE_HAND_FAIL)) &&
		(reg_v2 & CHG_FCP_PROTSTAT)) {
		scharger_inf("%s:FCP_TRANSFER_FAIL,slave status changed:ISR1=0x%x,ISR2=0x%x,ISR3=0x%x,ISR4=0x%x\n",
			     __func__, reg_v1, reg_v2, reg_v3, reg_v4);
		return -1;
	} else if (reg_v1 & CHG_FCP_NACK) {
		scharger_inf(
			"%s:FCP_TRANSFER_FAIL,slave nack: ISR1=0x%x,ISR2=0x%x\n",
			__func__, reg_v1, reg_v2);
		return -1;
	} else if ((reg_v2 & CHG_FCP_CRCRX) || (reg_v2 & CHG_FCP_PARRX) ||
		(reg_v3 & CHG_FCP_TAIL_HAND_FAIL)) {
		scharger_inf("%s:FCP_TRANSFER_FAIL, CRCRX_PARRX_ERROR:ISR1=0x%x,ISR2=0x%x, ISR3=0x%x\n",
			     __func__, reg_v1, reg_v2, reg_v3);
		return -1;
	} else {
		return 1; /* 1: indicates failure of reg judge */
	}
}

static int hi6526_fcp_cmd_transfer_check(struct hi6526_device_info *di)
{
	int rets[RET_SIZE_8] = {0};
	u8 reg_val1 = 0;
	u8 reg_val2 = 0;
	u8 reg_val3 = 0;
	u8 reg_val4 = 0;
	int i = 0;
	int ret;

	if (di == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -ENOMEM;
	}
	/* read accp interrupt registers until value is not zero */
	do {
		usleep_range(12000, 13000); /* (12000, 13000): sleep range */
		rets[0] = hi6526_read(di, CHG_FCP_ISR1_REG, &reg_val1);
		rets[1] = hi6526_read(di, CHG_FCP_ISR2_REG, &reg_val2);
		rets[2] = hi6526_read(di, CHG_FCP_IRQ3_REG, &reg_val3);
		rets[3] = hi6526_read(di, CHG_FCP_IRQ4_REG, &reg_val4);
		ret = (rets[0] || rets[1] || rets[2] || rets[3]);
		if (ret) {
			scharger_err("%s:reg read failed!\n", __func__);
			break;
		}
		if (reg_val1 || reg_val2) {
			ret = reg_judge_v600(reg_val1, reg_val2, reg_val3, reg_val4);
			if (ret == -1 || ret == 0)
				return ret;
			else
				scharger_inf("%s:FCP_TRANSFER_FAIL, ISR1=0x%x, ISR2=0x%x, ISR3=0x%x, total time = %dms\n",
					     __func__, reg_val1, reg_val2,
					     reg_val3, i * 10); /* 10: unit transfer */
		}
		i++;
		if (di->dc_ibus_ucp_happened)
			i = FCP_ACK_RETRY_CYCLE;
	} while (i < FCP_ACK_RETRY_CYCLE);

	scharger_inf("%s:fcp adapter transfer time out,total time %d ms\n",
		__func__, i * 10); /* 10 time difference */
	return -1;
}

/* Description: disable accp protocol and enable again */
static void hi6526_fcp_protocol_restart(struct hi6526_device_info *di)
{
	u8 reg_val = 0;
	int ret, i;

	if (di == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return;
	}

	/* disable accp protocol */
	mutex_lock(&di->fcp_detect_lock);
	(void)hi6526_write_mask(di, CHG_FCP_CTRL_REG, CHG_FCP_EN_MSK,
		CHG_FCP_EN_SHIFT, FALSE);
	usleep_range(9000, 10000); /* (9000, 10000): sleep range */
	(void)hi6526_write_mask(di, CHG_FCP_CTRL_REG, CHG_FCP_EN_MSK,
		CHG_FCP_EN_SHIFT, TRUE);
	/* detect fcp charger, wait for ping succ */
	for (i = 0; i < HI6526_RESTART_TIME; i++) {
		usleep_range(9000, 10000); /* (9000, 10000): sleep range */
		ret = hi6526_read(di, CHG_FCP_STATUS_REG, &reg_val);
		if (ret) {
			scharger_err("%s:read det attach err,ret:%d\n", __func__, ret);
			continue;
		}

		if (((reg_val & (CHG_FCP_DVC_MSK | CHG_FCP_ATTATCH_MSK)) ==
			CHG_FCP_SLAVE_GOOD))
			break;
	}

	if (i == HI6526_RESTART_TIME) {
		scharger_err("%s:wait for slave fail\n", __func__);
		mutex_unlock(&di->fcp_detect_lock);
		return;
	}
	mutex_unlock(&di->fcp_detect_lock);
	scharger_err("%s:disable and enable fcp protocol accp status is 0x%x\n",
		__func__, reg_val);
}

int hi6526_fcp_adapter_reg_read_block(u8 reg, u8 *val, u8 num, void *dev_data)
{
	int rets[RET_SIZE_8] = {0};
	int ret, i;
	u8 reg_val1 = 0;
	u8 reg_val2 = 0;
	u8 data_len;
	u8 *p = NULL;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL || val == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -ENOMEM;
	}

	mutex_lock(&di->accp_adapter_reg_lock);

	data_len = num < FCP_DATA_LEN ? num : FCP_DATA_LEN;
	p = val;

	for (i = 0; i < FCP_RETRY_TIME; i++) {
		/* before send cmd, clear accp interrupt registers */
		rets[0] = hi6526_read(di, CHG_FCP_ISR1_REG, &reg_val1);
		rets[1] = hi6526_read(di, CHG_FCP_ISR2_REG, &reg_val2);
		if (reg_val1 != 0)
			rets[2] = hi6526_write(di, CHG_FCP_ISR1_REG, reg_val1);
		if (reg_val2 != 0)
			rets[3] = hi6526_write(di, CHG_FCP_ISR2_REG, reg_val2);

		rets[4] = hi6526_write(di, CHG_FCP_CMD_REG, CHG_FCP_CMD_MBRRD);
		rets[5] = hi6526_write(di, CHG_FCP_ADDR_REG, reg);
		rets[6] = hi6526_write(di, CHG_FCP_LEN_REG, data_len);
		rets[7] = hi6526_write_mask(di, CHG_FCP_CTRL_REG, CHG_FCP_SNDCMD_MSK,
			CHG_FCP_SNDCMD_SHIFT, CHG_FCP_EN);

		ret = (rets[0] || rets[1] || rets[2] || rets[3] || rets[4] || rets[5] || rets[6] || rets[7]);
		if (ret) {
			scharger_err("%s: read error ret is %d\n", __func__, ret);
			mutex_unlock(&di->accp_adapter_reg_lock);
			return HI6526_FAIL;
		}

		/* check cmd transfer success or fail */
		if (hi6526_fcp_cmd_transfer_check(di) == 0) {
			/* recived data from adapter */
			ret = hi6526_read_block(di, CHG_FCP_RDATA_REG, p, data_len);
			break;
		}
		hi6526_fcp_protocol_restart(di);
		if (di->dc_ibus_ucp_happened)
			i = FCP_RETRY_TIME;
	}
	if (i >= FCP_RETRY_TIME) {
		scharger_err("%s:ack error,retry %d times\n", __func__, i);
		ret = HI6526_FAIL;
	}
	mutex_unlock(&di->accp_adapter_reg_lock);

	if (ret)
		return ret;

	num -= data_len;
	if (num) {
		p += data_len;
		reg += data_len;
		ret = hi6526_fcp_adapter_reg_read_block(reg, p, num, di);
		if (ret) {
			scharger_err("%s: read error ret is %d\n", __func__, ret);
			return HI6526_FAIL;
		}
	}

	return 0;
}

int hi6526_fcp_adapter_reg_read(u8 *val, u8 reg)
{
	int rets[RET_SIZE_8] = {0};
	int ret, i;
	u8 reg_val1 = 0;
	u8 reg_val2 = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL || val == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -ENOMEM;
	}

	mutex_lock(&di->accp_adapter_reg_lock);
	for (i = 0; i < FCP_RETRY_TIME; i++) {
		/* before send cmd, clear accp interrupt registers */
		rets[0] = hi6526_read(di, CHG_FCP_ISR1_REG, &reg_val1);
		rets[1] = hi6526_read(di, CHG_FCP_ISR2_REG, &reg_val2);
		if (reg_val1 != 0)
			rets[2] = hi6526_write(di, CHG_FCP_ISR1_REG, reg_val1);

		if (reg_val2 != 0)
			rets[3] = hi6526_write(di, CHG_FCP_ISR2_REG, reg_val2);

		rets[4] = hi6526_write(di, CHG_FCP_CMD_REG, CHG_FCP_CMD_SBRRD);
		rets[5] = hi6526_write(di, CHG_FCP_ADDR_REG, reg);
		rets[6] = hi6526_write_mask(di, CHG_FCP_CTRL_REG, CHG_FCP_SNDCMD_MSK,
			CHG_FCP_SNDCMD_SHIFT, CHG_FCP_EN);
		ret = (rets[0] || rets[1] || rets[2] || rets[3] || rets[4] || rets[5] || rets[6]);
		if (ret) {
			scharger_err("%s: write error ret is %d\n", __func__, ret);
			mutex_unlock(&di->accp_adapter_reg_lock);
			return HI6526_FAIL;
		}

		/* check cmd transfer success or fail */
		if (hi6526_fcp_cmd_transfer_check(di) == 0) {
			/* recived data from adapter */
			ret = hi6526_read(di, CHG_FCP_RDATA_REG, val);
			break;
		}
		hi6526_fcp_protocol_restart(di);
		if (di->dc_ibus_ucp_happened)
			i = FCP_RETRY_TIME;
	}
	if (i >= FCP_RETRY_TIME) {
		scharger_err("%s:ack error,retry %d times\n", __func__, i);
		ret = HI6526_FAIL;
	}
	mutex_unlock(&di->accp_adapter_reg_lock);

	return ret;
}

int hi6526_fcp_adapter_reg_write(u8 val, u8 reg)
{
	int rets[RET_SIZE_8] = {0};
	int ret, i;
	u8 reg_val1 = 0;
	u8 reg_val2 = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -ENOMEM;
	}

	mutex_lock(&di->accp_adapter_reg_lock);
	for (i = 0; i < FCP_RETRY_TIME; i++) {
		/* before send cmd, clear accp interrupt registers */
		rets[0] = hi6526_read(di, CHG_FCP_ISR1_REG, &reg_val1);
		rets[1] = hi6526_read(di, CHG_FCP_ISR2_REG, &reg_val2);
		if (reg_val1 != 0)
			rets[2] = hi6526_write(di, CHG_FCP_ISR1_REG, reg_val1);
		if (reg_val2 != 0)
			rets[3] = hi6526_write(di, CHG_FCP_ISR2_REG, reg_val2);

		rets[4] = hi6526_write(di, CHG_FCP_CMD_REG, CHG_FCP_CMD_SBRWR);
		rets[5] = hi6526_write(di, CHG_FCP_ADDR_REG, reg);
		rets[6] = hi6526_write(di, CHG_FCP_WDATA_REG, val);
		rets[7] = hi6526_write_mask(di, CHG_FCP_CTRL_REG, CHG_FCP_SNDCMD_MSK,
			CHG_FCP_SNDCMD_SHIFT, CHG_FCP_EN);

		ret = (rets[0] || rets[1] || rets[2] || rets[3] || rets[4] || rets[5] || rets[6] || rets[7]);
		if (ret) {
			scharger_err("%s: write error ret is %d\n", __func__, ret);
			mutex_unlock(&di->accp_adapter_reg_lock);
			return HI6526_FAIL;
		}

		/* check cmd transfer success or fail */
		if (hi6526_fcp_cmd_transfer_check(di) == 0)
			break;
		hi6526_fcp_protocol_restart(di);
		if (di->dc_ibus_ucp_happened)
			i = FCP_RETRY_TIME;
	}
	if (i >= FCP_RETRY_TIME) {
		scharger_err("%s:ack error,retry %d times\n", __func__, i);
		ret = HI6526_FAIL;
	}

	mutex_unlock(&di->accp_adapter_reg_lock);
	return ret;
}

/* check adapter voltage is around expected voltage */
static int hi6526_fcp_adapter_vol_check(struct hi6526_device_info *di, int adapter_vol_mv)
{
	int i, ret;
	int adc_vol = 0;

	if ((adapter_vol_mv < FCP_ADAPTER_MIN_VOL) ||
		(adapter_vol_mv > FCP_ADAPTER_MAX_VOL)) {
		scharger_err("%s: check vol out of range, input vol = %dmV\n", __func__,
			     adapter_vol_mv);
		return -1;
	}

	for (i = 0; i < FCP_ADAPTER_VOL_CHECK_TIMEOUT; i++) {
		ret = hi6526_get_vbus_mv2((unsigned int *)&adc_vol, di);
		if (ret)
			continue;
		if ((adc_vol > (adapter_vol_mv - FCP_ADAPTER_VOL_CHECK_ERROR)) &&
			(adc_vol < (adapter_vol_mv + FCP_ADAPTER_VOL_CHECK_ERROR))) {
			break;
		}
		msleep(FCP_ADAPTER_VOL_CHECK_POLLTIME);
	}

	if (i == FCP_ADAPTER_VOL_CHECK_TIMEOUT) {
		scharger_err("%s: check vol timeout, input vol = %dmV\n", __func__,
			     adapter_vol_mv);
		return -1;
	}
	scharger_inf("%s: check vol success, input vol = %dmV, spent %dms\n",
		     __func__, adapter_vol_mv, i * FCP_ADAPTER_VOL_CHECK_POLLTIME);
	return 0;
}

static int hi6526_fcp_ping_success(struct hi6526_device_info *di)
{
	u8 reg_val = 0;
	int i, ret;

	/* enable fcp_en */
	(void)hi6526_write_mask(di, CHG_FCP_CTRL_REG, CHG_FCP_EN_MSK,
		CHG_FCP_EN_SHIFT, TRUE);

	/* detect fcp charger, wait for ping succ */
	for (i = 0; i < CHG_FCP_DETECT_MAX_COUT; i++) {
		ret = hi6526_read(di, CHG_FCP_STATUS_REG, &reg_val);
		scharger_err("%s:wait for ping succ:0x%x\n", __func__, reg_val);

		if (ret) {
			scharger_err("%s:read det attach err,ret:%d\n", __func__, ret);
			continue;
		}
		if (charge_get_charger_type() == CHARGER_REMOVED) {
			scharger_err("%s:adapter plug out!\n", __func__);
			break;
		}
		if (((reg_val & (CHG_FCP_DVC_MSK | CHG_FCP_ATTATCH_MSK)) ==
			CHG_FCP_SLAVE_GOOD)) {
			break;
		}

		msleep(CHG_FCP_POLL_TIME);
	}
	if (i == CHG_FCP_DETECT_MAX_COUT || (charge_get_charger_type() == CHARGER_REMOVED)) {
		(void)hi6526_write_mask(di, CHG_FCP_CTRL_REG, CHG_FCP_EN_MSK,
			CHG_FCP_EN_SHIFT, FALSE);
		(void)hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG,
			CHG_FCP_CMP_EN_MSK,
			CHG_FCP_CMP_EN_SHIFT, FALSE);
		(void)hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG,
			CHG_FCP_DET_EN_MSK,
			CHG_FCP_DET_EN_SHIFT, FALSE);
		scharger_err("fcp adapter detect failed,reg[0x%x]=0x%x\n",
			     CHG_FCP_STATUS_REG, reg_val);
		return CHG_FCP_ADAPTER_DETECT_FAIL; /* not fcp adapter */
	}
	scharger_inf("fcp adapter detect ok\n");
	return CHG_FCP_ADAPTER_DETECT_SUCC;
}

/*
 * Return: 0: success
 *        -1: other fail
 *         1:fcp adapter but detect fail
 */
static int hi6526_fcp_adapter_detect(struct hi6526_device_info *di)
{
	u8 reg_val1 = 0;
	u8 reg_val2 = 0;
	int ret, i, ret0, ret1;

	mutex_lock(&di->fcp_detect_lock);
	ret = hi6526_read(di, CHG_FCP_STATUS_REG, &reg_val2);

	scharger_err("%s:CHG_FCP_STATUS_REG2:0x%x\n", __func__, reg_val2);
	if (ret) {
		scharger_err("%s:read det attach err,ret:%d\n", __func__, ret);
		mutex_unlock(&di->fcp_detect_lock);
		return -1;
	}

	if ((reg_val2 & (CHG_FCP_DVC_MSK | CHG_FCP_ATTATCH_MSK)) ==
		CHG_FCP_SLAVE_GOOD) {
		mutex_unlock(&di->fcp_detect_lock);
		scharger_inf("fcp adapter detect ok\n");
		return CHG_FCP_ADAPTER_DETECT_SUCC;
	}
	ret0 = hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG, CHG_FCP_DET_EN_MSK,
		CHG_FCP_DET_EN_SHIFT, TRUE);
	ret1 = hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG, CHG_FCP_CMP_EN_MSK,
		CHG_FCP_CMP_EN_SHIFT, TRUE);
	ret = (ret0 || ret1);
	if (ret) {
		scharger_err("%s:FCP enable detect fail,ret:%d\n", __func__, ret);
		(void)hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG,
			CHG_FCP_CMP_EN_MSK,
			CHG_FCP_CMP_EN_SHIFT, FALSE);
		(void)hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG,
			CHG_FCP_DET_EN_MSK,
			CHG_FCP_DET_EN_SHIFT, FALSE);
		mutex_unlock(&di->fcp_detect_lock);
		return -1;
	}
	/* wait for fcp_set */
	for (i = 0; i < CHG_FCP_DETECT_MAX_COUT; i++) {
		ret = hi6526_read(di, CHG_FCP_SET_STATUS_REG, &reg_val1);
		scharger_err("%s:CHG_FCP_SET_STATUS_REG1 0x%x\n", __func__, reg_val1);
		if (ret) {
			scharger_err("%s:read det attach err,ret:%d\n", __func__, ret);
			continue;
		}
		if (charge_get_charger_type() == CHARGER_REMOVED) {
			scharger_err("%s:adapter plug out!\n", __func__);
			break;
		}
		if (reg_val1 & CHG_FCP_SET_STATUS_MSK)
			break;
		msleep(CHG_FCP_POLL_TIME);
	}
	if (i == CHG_FCP_DETECT_MAX_COUT || (charge_get_charger_type() == CHARGER_REMOVED)) {
		(void)hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG,
			CHG_FCP_CMP_EN_MSK,
			CHG_FCP_CMP_EN_SHIFT, FALSE);
		(void)hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG,
			CHG_FCP_DET_EN_MSK,
			CHG_FCP_DET_EN_SHIFT, FALSE);
		mutex_unlock(&di->fcp_detect_lock);
		scharger_err("%s:CHG_FCP_ADAPTER_DETECT_OTHER return\n", __func__);
		return CHG_FCP_ADAPTER_DETECT_OTHER;
	}

	/* detect fcp charger, enable fcp_en and wait for ping succ */
	ret = hi6526_fcp_ping_success(di);
	mutex_unlock(&di->fcp_detect_lock);
	return ret;
}

/* 0: success,other: fail */
int hi6526_is_support_fcp(void)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -ENOMEM;
	}
	if (di->param_dts.fcp_support != 0) {
		scharger_inf("support fcp charge\n");
		return 0;
	} else {
		return 1;
	}
}

/* 0: success, other: fail */
int hi6526_fcp_master_reset(void *dev_data)
{
	int ret0, ret1, ret2;
	struct hi6526_device_info *di = (struct hi6526_device_info *)dev_data;

	ret0 = hi6526_write(di, CHG_FCP_SOFT_RST_REG, CHG_FCP_SOFT_RST_VAL);
	msleep(10); /* 10: need 10mS for next read */
	ret1 = hi6526_write(di, CHG_FCP_SOFT_RST_REG, CHG_FCP_SOFT_RST_DEFAULT);
	ret2 =
		hi6526_write(di, CHG_FCP_CTRL_REG, 0); /* clear fcp_en and fcp_master_rst */
	if (ret0 || ret1 || ret2)
		return -1;
	return 0;
}

int hi6526_global_reset(struct hi6526_device_info *di)
{
	int ret0;
	int ret1;

	ret0 = hi6526_write(di, SOFT_RST_CTL_REG, CHG_RST_CTRL_VAL);
	ret1 = hi6526_write(di, SOFT_RST_CTL_REG, CHG_RST_CTRL_VAL_DEFAULT);
	if (ret0 || ret1)
		return -1; /* -1:fail 0:succ */
	return 0;
}

/* 0: success, other: fail */
int hi6526_fcp_adapter_reset(void *dev_data)
{
	int ret, ret0, ret1, ret2;
	struct hi6526_device_info *di = get_hi6526_device();

	ret0 = hi6526_set_vbus_vset(NULL, VBUS_VSET_5V);
	ret1 = hi6526_set_vbus_ovp(di, VBUS_VSET_12V);
	ret2 = hi6526_write(di, CHG_FCP_CTRL_REG, CHG_FCP_EN_MSK |
		CHG_FCP_MSTR_RST_MSK);
	ret = (ret0 || ret1 || ret2);
	if (ret) {
		scharger_err("%s:send rst cmd failed\n", __func__);
		return ret;
	}

	ret = hi6526_fcp_adapter_vol_check(di, FCP_ADAPTER_RST_VOL);
	if (ret) {
		/* clear fcp_en and fcp_master_rst */
		ret0 = hi6526_write(di, CHG_FCP_CTRL_REG, 0);
		scharger_err("%s:adc check adapter output voltage failed\n", __func__);
		if (ret0)
			return ret0;
		return ret;
	}
	/* clear fcp_en and fcp_master_rst */
	ret0 = hi6526_write(di, CHG_FCP_CTRL_REG, 0);
	ret1 = hi6526_config_opt_param(di, VBUS_VSET_5V);
	ret2 = hi6526_set_vbus_ovp(di, VBUS_VSET_5V);
	if (ret0 || ret1 || ret2)
		return -1;
	return 0;
}

int hi6526_scp_reg_read_block(int reg, int *val, int num, void *dev_data)
{
	int ret, i;
	u8 data = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (val == NULL) {
		scharger_err("val is null\n");
		return -1;
	}

	di->scp_error_flag = SCP_NO_ERR;

	for (i = 0; i < num; i++) {
		ret = scp_adapter_reg_read(di, &data, reg + i);
		if (ret) {
			scharger_err("scp read failed(reg=0x%x)\n", reg + i);
			return -1;
		}
		val[i] = data;
	}

	return 0;
}

int hi6526_scp_reg_write_block(int reg, const int *val, int num, void *dev_data)
{
	int ret, i;
	struct hi6526_device_info *di = get_hi6526_device();

	if (val == NULL) {
		scharger_err("val is null\n");
		return -1;
	}

	di->scp_error_flag = SCP_NO_ERR;

	for (i = 0; i < num; i++) {
		ret = scp_adapter_reg_write(di, val[i], reg + i);
		if (ret) {
			scharger_err("scp write failed(reg=0x%x)\n", reg + i);
			return -1;
		}
	}

	return 0;
}

int hi6526_fcp_reg_read_block(int reg, int *val, int num, void *dev_data)
{
	int ret, i;
	u8 data = 0;

	if (val == NULL) {
		scharger_err("val is null\n");
		return -1;
	}

	for (i = 0; i < num; i++) {
		ret = hi6526_fcp_adapter_reg_read(&data, reg + i);
		if (ret) {
			scharger_err("fcp read failed(reg=0x%x)\n", reg + i);
			return -1;
		}
		val[i] = data;
	}
	return 0;
}

int hi6526_fcp_reg_write_block(int reg, const int *val, int num, void *dev_data)
{
	int ret, i;

	if (val == NULL) {
		scharger_err("val is null\n");
		return -1;
	}

	for (i = 0; i < num; i++) {
		ret = hi6526_fcp_adapter_reg_write(val[i], reg + i);
		if (ret) {
			scharger_err("fcp write failed(reg=0x%x)\n", reg + i);
			return -1;
		}
	}

	return 0;
}

int hi6526_fcp_detect_adapter(void *dev_data)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL) {
		scharger_err("hi6526_device_info is null\n");
		return -1;
	}

	return hi6526_fcp_adapter_detect(di);
}

int hi6526_scp_detect_adapter(void *dev_data)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL) {
		scharger_err("hi6526_device_info is null\n");
		return -1;
	}

	return hi6526_fcp_adapter_detect(di);
}

int hi6526_pre_init(void *dev_data)
{
	int ret;

	ret = hi6526_scp_master_init();

	return ret;
}

int hi6526_fcp_stop_charge_config(void *dev_data)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}
	scharger_dbg("hi6526_fcp_master_reset");
	(void)hi6526_fcp_master_reset(di);
	(void)hi6526_write_mask(di, CHG_FCP_DET_CTRL_REG, CHG_FCP_DET_EN_MSK,
				CHG_FCP_DET_EN_SHIFT, FALSE);
	return 0;
}

int hi6526_is_fcp_charger_type(void *dev_data)
{
	u8 reg_val = 0;
	int ret;
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return FALSE;
	}

	if (hi6526_is_support_fcp()) {
		scharger_err("%s:NOT SUPPORT FCP!\n", __func__);
		return FALSE;
	}
	ret = hi6526_read(di, FCP_ADAPTER_CNTL_REG, &reg_val);
	if (ret) {
		scharger_err("%s reg read fail!\n", __func__);
		return FALSE;
	}
	if ((reg_val & HI6526_ACCP_CHARGER_DET) == HI6526_ACCP_CHARGER_DET)
		return TRUE;
	return FALSE;
}

int hi6526_fcp_read_switch_status(void *dev_data)
{
	return 0;
}

int hi6526_is_support_scp(void)
{
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}

	if (!di || !di->param_dts.scp_support)
		return HI6526_FAIL;

	return 0;
}

static int scp_adapter_reg_read(struct hi6526_device_info *di, u8 *val, u8 reg)
{
	int ret;

	if (di->scp_error_flag) {
		scharger_err("%s:scp timeout happened,do not read reg=%u\n",
			     __func__, reg);
		return HI6526_FAIL;
	}

	ret = hi6526_fcp_adapter_reg_read(val, reg);
	if (ret) {
		scharger_err("%s:error reg=%u\n", __func__, reg);
		if (reg != SCP_ADP_TYPE0)
			di->scp_error_flag = SCP_IS_ERR;

		return HI6526_FAIL;
	}

	return 0;
}

static int scp_adapter_reg_write(struct hi6526_device_info *di, u8 val, u8 reg)
{
	int ret;

	if (di->scp_error_flag) {
		scharger_err("%s:scp timeout happened,do not write reg=%u\n",
			     __func__, reg);
		return HI6526_FAIL;
	}

	ret = hi6526_fcp_adapter_reg_write(val, reg);
	if (ret) {
		scharger_err("%s:error reg=%u\n", __func__, reg);
		di->scp_error_flag = SCP_IS_ERR;

		return HI6526_FAIL;
	}

	return 0;
}

int hi6526_scp_master_init(void)
{
	int ret, ret0, ret1, ret2, ret3, ret4, ret5, ret6, ret7;
	struct hi6526_device_info *di = get_hi6526_device();

	if (!di) {
		scharger_err("%s hi6526_device_info is NULL\n", __func__);
		return -1;
	}

	ret0 = hi6526_set_charge_enable(NULL, CHG_DISABLE);
	ret1 = hi6526_set_buck_enable(CHG_DISABLE, di);
	ret2 = hi6526_set_vbus_vset(NULL, VBUS_VSET_12V);
	ret3 = hi6526_set_fast_safe_timer(CHG_FASTCHG_TIMER_5H);
	ret4 = hi6526_set_watchdog_timer(NULL, WATCHDOG_TIMER_40_S);
	ret5 = hi6526_set_batfet_ctrl(di, CHG_BATFET_EN);
	ret6 = hi6526_set_otg_current(NULL, BOOST_LIM_1000);
	ret7 = hi6526_set_otg_enable(NULL, CHARGE_OTG_DISABLE);

	ret = (ret0 || ret1 || ret2 || ret3 || ret4 || ret5 || ret6 || ret7);
	if (ret) {
		scharger_inf("%s:%d fail\n", __func__, ret);
		return -1;
	}
	return 0;
}

int hi6526_scp_chip_reset(void *dev_data)
{
	int ret;

	ret = hi6526_fcp_master_reset(dev_data);
	if (ret) {
		scharger_inf("%s:hi6526_fcp_master_reset fail!\n", __func__);
		return HI6526_FAIL;
	}

	return 0;
}

int hi6526_scp_adaptor_reset(void *dev_data)
{
	return hi6526_fcp_adapter_reset(dev_data);
}

int hi6526_efuse_get_dieid(struct charger_device *dev, u8 *dieid, unsigned int len)
{
	int ret = 0;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -1;
	}
	if (dieid == NULL) {
		scharger_err("%s: dieid is null\n", __func__);
		return -1;
	}

	memset(dieid, 0, len);
	if (di->hi6526_version == CHIP_VERSION_V100) {
		if (len < HI6526_DIEID_LENGTH_V100) {
			scharger_err("%s: V100 %u, mem length is not enough!\n",
				     __func__, len);
			return -1;
		}
		/* 0: efuse number 0 */
		ret += hi6526_efuse_read(EFUSE3, EFUSE_BYTE5, &dieid[0], 0);
		ret += hi6526_efuse_read(EFUSE3, EFUSE_BYTE6, &dieid[1], 0);
		ret += hi6526_efuse_read(EFUSE3, EFUSE_BYTE7, &dieid[2], 0);
	} else {
		if (len < HI6526_DIEID_LENGTH) {
			scharger_err("%s: %u, mem length is not enough!\n",
				     __func__, len);
			return -1;
		}
		/* 0: efuse number 0 */
		ret += hi6526_efuse_read(EFUSE4, EFUSE_BYTE0, &dieid[4], 0);
		ret += hi6526_efuse_read(EFUSE4, EFUSE_BYTE1, &dieid[5], 0);
		ret += hi6526_efuse_read(EFUSE4, EFUSE_BYTE2, &dieid[6], 0);
		ret += hi6526_efuse_read(EFUSE4, EFUSE_BYTE3, &dieid[7], 0);
		ret += hi6526_efuse_read(EFUSE3, EFUSE_BYTE4, &dieid[0], 0);
		ret += hi6526_efuse_read(EFUSE3, EFUSE_BYTE5, &dieid[1], 0);
		ret += hi6526_efuse_read(EFUSE3, EFUSE_BYTE6, &dieid[2], 0);
		ret += hi6526_efuse_read(EFUSE3, EFUSE_BYTE7, &dieid[3], 0);
	}

	if (ret)
		return -1;
	return 0;
}

int hi6526_get_dieid_str(struct charger_device *dev, char *dieid, unsigned int len)
{
	u8 val[HI6526_DIEID_LENGTH] = {0};
	int ret;
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL) {
		scharger_err("%s hi6526_device_info is NULL!\n", __func__);
		return -1;
	}
	if (dieid == NULL) {
		scharger_err("%s: dieid is null\n", __func__);
		return -1;
	}
	if (len < HI6526_DIEID_LENGTH) {
		scharger_err("%s: mem length is not enough!\n", __func__);
		return -1;
	}
	memset(dieid, 0, len);
	ret = hi6526_efuse_get_dieid(dev, val, HI6526_DIEID_LENGTH);
	if (ret)
		return -1;

	if (di->hi6526_version == CHIP_VERSION_V100)
		ret = snprintf(dieid, len,
				 "\r\nHi6526v100:0x%02x%02x%02x\r\n",
				 val[2], val[1], val[0]);
	else
		ret = snprintf(dieid, len,
				 "\r\nHi6526v200:0x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
				 val[7], val[6], val[5], val[4], val[3],
				 val[2], val[1], val[0]);
	if (ret < 0)
		scharger_err("%s, offset %d, snprintf_s error\n",
			     __func__, ret);

	return 0;
}

int hi6526_chip_init(struct charger_device *dev, struct charge_init_data *init_crit)
{
	int i;
	int ret[RET_SIZE_21] = {0};
	struct hi6526_device_info *di = get_hi6526_device();

	if (di == NULL || init_crit == NULL) {
		scharger_err("%s hi6526_device_info or charge_init_data NULL!\n", __func__);
		return -ENOMEM;
	}

	scharger_inf("%s init_crit->vbus %d\n", __func__, init_crit->vbus);
	di->charger_type = init_crit->charger_type;
	switch (init_crit->vbus) {
	case ADAPTER_5V:
		ret[0] = hi6526_ibat_res_sel(di);
		ret[1] = hi6526_config_opt_param(di, VBUS_VSET_5V);
		ret[2] = hi6526_dpm_init();
		ret[3] = hi6526_set_vbus_vset(dev, VBUS_VSET_5V);
		break;
	case ADAPTER_9V:
		ret[0] = hi6526_config_opt_param(di, VBUS_VSET_5V);
		ret[1] = hi6526_set_vbus_uvp_ovp(di, VBUS_VSET_9V);
		break;
	case ADAPTER_12V:
		ret[0] = hi6526_config_opt_param(di, VBUS_VSET_12V);
		ret[1] = hi6526_set_vbus_uvp_ovp(di, VBUS_VSET_12V);
		break;
	default:
		scharger_err("%s: init mode err\n", __func__);
		return -EINVAL;
	}
	switch (init_crit->charger_type) {
	case CHARGER_TYPE_WIRELESS:
		hi6526_vusb_uv_det_enable(0);
		break;
	default:
		hi6526_vusb_uv_det_enable(1);
		break;
	}
	/*
	 * ret[]: this array is to store the init status of chip ops,
	 * for each term, 0: succ, others: fail
	 */
	ret[5] = hi6526_set_charge_enable(dev, CHG_DISABLE);
	ret[6] = hi6526_set_recharge_vol(CHG_RECHG_150);
	ret[7] = hi6526_set_fast_safe_timer(CHG_FASTCHG_TIMER_20H);
	ret[8] = hi6526_set_term_enable(dev, CHG_TERM_DIS);
	ret[9] = hi6526_set_input_current(dev, CHG_ILIMIT_475);
	ret[10] = hi6526_set_charge_current(dev, CHG_FAST_ICHG_500MA);
	ret[11] = hi6526_set_terminal_voltage(dev, CHG_FAST_VCHG_4400);
	ret[12] = hi6526_set_terminal_current(dev, CHG_TERM_ICHG_150MA);
	ret[13] = hi6526_set_watchdog_timer(dev, WATCHDOG_TIMER_DIS);
	ret[14] = hi6526_set_precharge_current(CHG_PRG_ICHG_200MA);
	ret[15] = hi6526_set_precharge_voltage(CHG_PRG_VCHG_2800);
	ret[16] = hi6526_set_batfet_ctrl(di, CHG_BATFET_EN);
	/* IR compensation voatge clamp,IR compensation resistor setting */
	ret[17] = hi6526_set_bat_comp(di->param_dts.bat_comp);
	ret[18] = hi6526_set_vclamp(di->param_dts.vclamp);
	ret[19] = hi6526_set_otg_current(dev, BOOST_LIM_1000);
	ret[20] = hi6526_set_otg_enable(dev, CHARGE_OTG_DISABLE);

	for (i = 0; i < RET_SIZE_21; i++) {
		if (ret[i])
			return -1;
	}

	return 0;
}

void hi6526_vbat_lv_handle(struct hi6526_device_info *di)
{
	int ret;
	u8 val = 0;

	ret = hi6526_read(di, VBAT_LV_CFG_REG, &val);
	if (ret)
		scharger_inf("%s:hi6526_read fail ret=%d\n", __func__, ret);
	scharger_inf("%s:VBAT_LV_REG is 0x%x\n", __func__, val);
	(void)hi6526_write_mask(di, VBAT_LV_CFG_REG, VBAT_LV_CFG_MASK,
		VBAT_LV_CFG_SHIFT, 1);
}

int hi6526_lock_mutex_init(struct hi6526_device_info *di)
{
	if (di == NULL)
		return -ENOMEM;

	rt_mutex_init(&di->i2c_lock);
	mutex_init(&di->otg_lock);
	mutex_init(&di->fcp_detect_lock);
	mutex_init(&di->adc_conv_lock);
	mutex_init(&di->accp_adapter_reg_lock);
	mutex_init(&di->ibias_calc_lock);
	return 0;
}
