/*
 * drivers/display/hisi/backlight/lm36274.c
 *
 * lm36274 driver reffer to lcd
 *
 * Copyright (C) 2012-2015 HUAWEI, Inc.
 * Author: HUAWEI, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>
#include <platform/mt_gpio.h>
#include "lcd_kit_utils.h"
#include "lm36274.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"

#define OFFSET_DEF_VAL (-1)

static unsigned int check_status;
static bool lm36274_checked;
static struct lm36274_backlight_information {
	/* whether support lm36274 or not */
	u32 lm36274_support;
	/* which i2c bus controller lm36274 mount */
	int lm36274_i2c_bus_id;
	/* lm36274 hw_ldsen gpio */
	int lm36274_hw_ldsen_gpio;
	/* lm36274 hw_en gpio */
	u32 lm36274_hw_en;
	u32 lm36274_hw_en_gpio;
	u32 bl_on_lk_mdelay;
	u32 bl_level;
	u32 bl_only;
	int nodeoffset;
	void *pfdt;
	int lm36274_reg[LM36274_RW_REG_MAX];
}lm36274_bl_info;
static struct lm36274_backlight_information lm36274_bl_info = { 0 };

static char *lm36274_dts_string[LM36274_RW_REG_MAX] = {
	"lm36274_bl_config_1",
	"lm36274_bl_config_2",
	"lm36274_bl_brightness_lsb",
	"lm36274_bl_brightness_msb",
	"lm36274_auto_freq_low",
	"lm36274_auto_freq_high",
	"lm36274_display_bias_config_2",
	"lm36274_display_bias_config_3",
	"lm36274_lcm_boost_bias",
	"lm36274_vpos_bias",
	"lm36274_vneg_bias",
	"lm36274_bl_option_1",
	"lm36274_bl_option_2",
	"lm36274_display_bias_config_1",
	"lm36274_bl_en",
};

static u8 lm36274_reg_addr[LM36274_RW_REG_MAX] = {
	REG_BL_CONFIG_1,
	REG_BL_CONFIG_2,
	REG_BL_BRIGHTNESS_LSB,
	REG_BL_BRIGHTNESS_MSB,
	REG_AUTO_FREQ_LOW,
	REG_AUTO_FREQ_HIGH,
	REG_DISPLAY_BIAS_CONFIG_2,
	REG_DISPLAY_BIAS_CONFIG_3,
	REG_LCM_BOOST_BIAS,
	REG_VPOS_BIAS,
	REG_VNEG_BIAS,
	REG_BL_OPTION_1,
	REG_BL_OPTION_2,
	REG_DISPLAY_BIAS_CONFIG_1,
	REG_BL_ENABLE,
};

static struct lm36274_voltage lm36274_vol_table[] = {
	{ 4000000, LM36274_VOL_400 },
	{ 4050000, LM36274_VOL_405 },
	{ 4100000, LM36274_VOL_410 },
	{ 4150000, LM36274_VOL_415 },
	{ 4200000, LM36274_VOL_420 },
	{ 4250000, LM36274_VOL_425 },
	{ 4300000, LM36274_VOL_430 },
	{ 4350000, LM36274_VOL_435 },
	{ 4400000, LM36274_VOL_440 },
	{ 4450000, LM36274_VOL_445 },
	{ 4500000, LM36274_VOL_450 },
	{ 4550000, LM36274_VOL_455 },
	{ 4600000, LM36274_VOL_460 },
	{ 4650000, LM36274_VOL_465 },
	{ 4700000, LM36274_VOL_470 },
	{ 4750000, LM36274_VOL_475 },
	{ 4800000, LM36274_VOL_480 },
	{ 4850000, LM36274_VOL_485 },
	{ 4900000, LM36274_VOL_490 },
	{ 4950000, LM36274_VOL_495 },
	{ 5000000, LM36274_VOL_500 },
	{ 5050000, LM36274_VOL_505 },
	{ 5100000, LM36274_VOL_510 },
	{ 5150000, LM36274_VOL_515 },
	{ 5200000, LM36274_VOL_520 },
	{ 5250000, LM36274_VOL_525 },
	{ 5300000, LM36274_VOL_530 },
	{ 5350000, LM36274_VOL_535 },
	{ 5400000, LM36274_VOL_540 },
	{ 5450000, LM36274_VOL_545 },
	{ 5500000, LM36274_VOL_550 },
	{ 5550000, LM36274_VOL_555 },
	{ 5600000, LM36274_VOL_560 },
	{ 5650000, LM36274_VOL_565 },
	{ 5700000, LM36274_VOL_570 },
	{ 5750000, LM36274_VOL_575 },
	{ 5800000, LM36274_VOL_580 },
	{ 5850000, LM36274_VOL_585 },
	{ 5900000, LM36274_VOL_590 },
	{ 5950000, LM36274_VOL_595 },
	{ 6000000, LM36274_VOL_600 },
	{ 6050000, LM36274_VOL_605 },
	{ 6400000, LM36274_VOL_640 },
	{ 6450000, LM36274_VOL_645 },
	{ 6500000, LM36274_VOL_650 },
};

int lm36274_set_backlight(uint32_t bl_level);
static int lm36274_i2c_read_u8(u8 chip_no, u8 *data_buffer, int addr)
{
	int ret = -1;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t lm36274_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	lm36274_i2c.id = lm36274_bl_info.lm36274_i2c_bus_id;
	lm36274_i2c.addr = chip_no;
	lm36274_i2c.mode = ST_MODE;
	lm36274_i2c.speed = LM36274_I2C_SPEED;

	ret = i2c_write_read(&lm36274_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int lm36274_i2c_write_u8(u8 chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[LM36274_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = LM36274_WRITE_LEN;
	struct mt_i2c_t lm36274_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	lm36274_i2c.id = lm36274_bl_info.lm36274_i2c_bus_id;
	lm36274_i2c.addr = chip_no;
	lm36274_i2c.mode = ST_MODE;
	lm36274_i2c.speed = LM36274_I2C_SPEED;

	ret = i2c_write(&lm36274_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int lm36274_i2c_update_bits(u8 chip_no, u8 reg, u8 mask, u8 val)
{
	int ret;
	u8 tmp;
	u8 orig = 0;

	ret = lm36274_i2c_read_u8(chip_no, &orig, reg);
	if (ret < 0)
		return ret;

	tmp = orig & ~mask;
	tmp |= val & mask;

	if (tmp != orig) {
		ret = lm36274_i2c_write_u8(chip_no, reg, val);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static void lm36274_get_target_voltage(int *vpos, int *vneg)
{
	int i;
	int tal_size = sizeof(lm36274_vol_table) / sizeof(struct lm36274_voltage);

	if ((vpos == NULL) || (vneg == NULL)) {
		LCD_KIT_ERR("vpos or vneg is null\n");
		return;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (lm36274_vol_table[i].voltage == power_hdl->lcd_vsp.buf[2]) {
			*vpos = lm36274_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsp voltage, use default voltage\n");
		*vpos = LM36274_VOL_600;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (lm36274_vol_table[i].voltage == power_hdl->lcd_vsn.buf[2]) {
			*vneg = lm36274_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsn voltage, use default voltage\n");
		*vneg = LM36274_VOL_600;
	}

	LCD_KIT_INFO("*vpos_target=0x%x,*vneg_target=0x%x\n", *vpos, *vneg);

	return;
}

static int lm36274_parse_dts()
{
	int ret;
	int i;
	int vpos_target = 0;
	int vneg_target = 0;

	LCD_KIT_INFO("lm36274_parse_dts +!\n");
	for (i = 0; i < LM36274_RW_REG_MAX; i++) {
		ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
			lm36274_bl_info.nodeoffset, lm36274_dts_string[i],
			(u32 *)&lm36274_bl_info.lm36274_reg[i], 0);
		if (ret < 0) {
			lm36274_bl_info.lm36274_reg[i] = LM36274_INVALID_VAL;
			LCD_KIT_INFO("can not find %s dts\n", lm36274_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				lm36274_dts_string[i], lm36274_bl_info.lm36274_reg[i]);
		}

	}
	if (lm36274_bl_info.bl_only == 0) {
		lm36274_get_target_voltage(&vpos_target, &vneg_target);
		/* 9  is the position of vsp in lm36274_reg */
		/* 10 is the position of vsn in lm36274_reg */
		if (lm36274_bl_info.lm36274_reg[9] != vpos_target)
			lm36274_bl_info.lm36274_reg[9] = vpos_target;
		if (lm36274_bl_info.lm36274_reg[10] != vneg_target)
			lm36274_bl_info.lm36274_reg[10] = vneg_target;
	}

	return ret;
}

static int lm36274_config_register()
{
	int ret;
	int i;
	for (i = 0; i < LM36274_RW_REG_MAX; i++) {
		ret = lm36274_i2c_write_u8(LM36274_SLAV_ADDR, lm36274_reg_addr[i],
				lm36274_bl_info.lm36274_reg[i]);
		if (ret < 0) {
			LCD_KIT_ERR("write lm36274 backlight config register 0x%x failed", lm36274_reg_addr[i]);
			return ret;
		}
	}

	return ret;
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = lm36274_set_backlight,
};

static int lm36274_device_verify(void)
{
	int ret;
	u32 is_support = 0;
	unsigned char reg_val = 0;

	if (lm36274_bl_info.lm36274_hw_en) {
		mt_set_gpio_mode(lm36274_bl_info.lm36274_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(lm36274_bl_info.lm36274_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(lm36274_bl_info.lm36274_hw_en_gpio,
			GPIO_OUT_ONE);
		if (lm36274_bl_info.bl_on_lk_mdelay)
			mdelay(lm36274_bl_info.bl_on_lk_mdelay);
	}

	/* Read IC revision */
	ret = lm36274_i2c_read_u8(LM36274_SLAV_ADDR, &reg_val, REG_REVISION);
	if (ret < 0) {
		LCD_KIT_ERR("read lm36274 revision failed\n");
		goto error_exit;
	}
	LCD_KIT_INFO("lm36274 reg revision = 0x%x\n", reg_val);
	if ((reg_val & DEV_MASK) != VENDOR_ID_TI) {
		LCD_KIT_INFO("lm36274 check vendor id failed\n");
		ret = LCD_KIT_FAIL;
		goto error_exit;
	}

	ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
		lm36274_bl_info.nodeoffset, LM36274_EXTEND_REV_SUPPORT,
		&is_support, 0);
	if (ret < 0) {
		LCD_KIT_INFO("no lm36274 extend rev compatible \n");
	} else {
		LCD_KIT_INFO("get lm36274 extend rev %d, support\n", is_support);
		if (is_support == LM36274_EXTEND_REV_VAL) {
			/* Read BL_BLCONFIG1, to fix TI and NT same revision problem */
			ret = lm36274_i2c_read_u8(LM36274_SLAV_ADDR, &reg_val, REG_BL_CONFIG_1);
			if (ret < 0) {
				LCD_KIT_ERR("read lm36274 bl config 1 failed\n");
				goto error_exit;
			}
			LCD_KIT_INFO("lm32674 reg bl config1 = 0x%x\n", reg_val);
			if ((reg_val & DEV_MASK_EXTEND_LM32674) == VENDOR_ID_EXTEND_LM32674) {
				LCD_KIT_INFO("lm36274 check vendor id exend failed\n");
				ret = LCD_KIT_FAIL;
				goto error_exit;
			}
		}
	}

	return LCD_KIT_OK;
error_exit:
	if (lm36274_bl_info.lm36274_hw_en)
		mt_set_gpio_out(lm36274_bl_info.lm36274_hw_en_gpio,
			GPIO_OUT_ZERO);
	return ret;
}

static int lm36274_backlight_ic_check(void)
{
	int ret = 0;

	if (lm36274_checked) {
		LCD_KIT_INFO("lm36274 already check, not again setting\n");
		return ret;
	}

	ret = lm36274_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("lm36274 is not right backlight ics\n");
	} else {
		lm36274_parse_dts();
		ret = lm36274_config_register();
		if (ret < 0)
			LCD_KIT_ERR("lm36274 config register failed\n");
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("lm36274 is right backlight ic\n");
	}
	lm36274_checked = true;
	return ret;
}

int lm36274_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("lm36274 init\n");
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo->bias_bl_ic_checked != 0) {
		LCD_KIT_ERR("bias bl ic checked\n");
		return LCD_KIT_OK;
	}
	lm36274_bl_info.pfdt = get_lk_overlayed_dtb();
	if (lm36274_bl_info.pfdt == NULL) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	lm36274_bl_info.nodeoffset = fdt_node_offset_by_compatible(
		lm36274_bl_info.pfdt, OFFSET_DEF_VAL, DTS_COMP_LM36274);
	if (lm36274_bl_info.nodeoffset < 0) {
		LCD_KIT_INFO("can not find %s node\n", DTS_COMP_LM36274);
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
		lm36274_bl_info.nodeoffset, LM36274_SUPPORT,
		&lm36274_bl_info.lm36274_support, 0);
	if (ret < 0 || !lm36274_bl_info.lm36274_support) {
		LCD_KIT_ERR("get lm36274_support failed!\n");
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
		lm36274_bl_info.nodeoffset, LM36274_I2C_BUS_ID,
		(u32 *)&lm36274_bl_info.lm36274_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lm36274_i2c_bus_id fail!\n");
		lm36274_bl_info.lm36274_i2c_bus_id = 0;
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
		lm36274_bl_info.nodeoffset, GPIO_LM36274_EN_NAME,
		&lm36274_bl_info.lm36274_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts lm36274_hw_enable fail!\n");
		lm36274_bl_info.lm36274_hw_en = 0;
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
		lm36274_bl_info.nodeoffset, LM36274_ONLY_BACKLIGHT,
		&lm36274_bl_info.bl_only, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts only_backlight fail\n");
		lm36274_bl_info.bl_only = 0;
	}

	if (lm36274_bl_info.lm36274_hw_en) {
		ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
		lm36274_bl_info.nodeoffset, LM36274_HW_EN_GPIO,
		&lm36274_bl_info.lm36274_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts lm36274_hw_en_gpio fail!\n");
			lm36274_bl_info.lm36274_hw_en_gpio = 0;
			return LCD_KIT_FAIL;
		}
		ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
			lm36274_bl_info.nodeoffset, LM36274_HW_EN_DELAY,
			&lm36274_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0)
			LCD_KIT_INFO("parse dts bl_on_lk_mdelay fail!\n");
	}

	ret = lcd_kit_get_dts_u32_default(lm36274_bl_info.pfdt,
		lm36274_bl_info.nodeoffset, LM36274_BL_LEVEL,
		&lm36274_bl_info.bl_level, LM36274_BL_DEFAULT_LEVEL);
	if (ret < 0)
		LCD_KIT_ERR("parse dts lm36274_bl_level fail!\n");
	ret = lm36274_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = 1;
		LCD_KIT_INFO("lm36274 is checked\n");
	}
	LCD_KIT_INFO("lm36274 is support\n");

	return LCD_KIT_OK;
}

/**
 * lm36274_set_backlight(): Set Backlight working mode
 *
 * @bl_level: value for backlight ,range from 0 to 2047
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */

int lm36274_set_backlight(uint32_t bl_level)
{
	int ret;
	uint32_t level;
	int bl_msb;
	int bl_lsb;
	struct mtk_panel_info *panel_info = NULL;

	panel_info = lcm_get_panel_info();
	if (panel_info == NULL) {
		LCD_KIT_ERR("panel_info is NULL\n");
		return LCD_KIT_FAIL;
	}
	if (panel_info->bl_ic_ctrl_mode == MTK_MIPI_BL_IC_PWM_MODE) {
		LCD_KIT_INFO("lm36274 mipi pwm\n");
		return LCD_KIT_OK;
	}
	/* map bl_level from 10000 to 2048 stage brightness */
	level = bl_level * lm36274_bl_info.bl_level / LM36274_BL_DEFAULT_LEVEL;

	if (level > LM36274_BL_MAX)
		level = LM36274_BL_MAX;

	/* 11-bit brightness code */
	bl_msb = level >> MSB;
	bl_lsb = level & LSB;

	LCD_KIT_INFO("level = %d, bl_msb = %d, bl_lsb = %d", level, bl_msb, bl_lsb);

	ret = lm36274_i2c_update_bits(LM36274_SLAV_ADDR, REG_BL_BRIGHTNESS_LSB, MASK_BL_LSB, bl_lsb);
	if (ret < 0)
		goto i2c_error;

	ret = lm36274_i2c_write_u8(LM36274_SLAV_ADDR, REG_BL_BRIGHTNESS_MSB, bl_msb);
	if (ret < 0)
		goto i2c_error;

	LCD_KIT_INFO("write lm36274 backlight %u success\n", bl_level);
	return ret;

i2c_error:
	LCD_KIT_ERR("i2c access fail to register");
	return ret;
}

void lm36274_set_backlight_status (void)
{
	int ret;
	int offset;
	void *kernel_fdt = NULL;

#ifdef DEVICE_TREE_SUPPORT
	kernel_fdt = get_kernel_fdt();
#endif
	if (kernel_fdt == NULL) {
		LCD_KIT_ERR("kernel_fdt is NULL\n");
		return;
	}
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_LM36274);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find lm36274 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update lm36274 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("lm36274_set_backlight_status OK!\n");
}

