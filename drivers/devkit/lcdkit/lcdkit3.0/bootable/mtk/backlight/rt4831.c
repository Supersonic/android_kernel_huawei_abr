/*
 * rt4831.c
 *
 * adapt for backlight driver
 *
 * Copyright (c) 2018-2019 Huawei Technologies Co., Ltd.
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

#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>
#include <platform/mt_gpio.h>
#include "lcd_kit_utils.h"
#include "rt4831.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"

#define OFFSET_DEF_VAL (-1)

static unsigned int check_status;
static bool rt4831_checked;
static struct rt4831_backlight_information rt4831_bl_info = {0};

int rt4831_set_backlight(uint32_t bl_level);

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = rt4831_set_backlight,
};

static char *rt4831_dts_string[RT4831_RW_REG_MAX] = {
	"rt4831_bl_config_1",
	"rt4831_bl_config_2",
	"rt4831_bl_brightness_lsb",
	"rt4831_bl_brightness_msb",
	"rt4831_auto_freq_low",
	"rt4831_auto_freq_high",
	"rt4831_display_bias_config_2",
	"rt4831_display_bias_config_3",
	"rt4831_lcm_boost_bias",
	"rt4831_vpos_bias",
	"rt4831_vneg_bias",
	"rt4831_bl_option_1",
	"rt4831_bl_option_2",
	"rt4831_display_bias_config_1",
	"rt4831_reg_hidden_f0",
	"rt4831_reg_hidden_b1",
	"rt4831_bl_en",
};

static int rt4831_reg_addr[RT4831_RW_REG_MAX] = {
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
	REG_RT4831_REG_HIDDEN_F0,
	REG_RT4831_REG_HIDDEN_B1,
	REG_BL_ENABLE,
};

static struct rt4831_voltage rt4831_vol_table[] = {
	{ 4000000, RT4831_VOL_400 },
	{ 4050000, RT4831_VOL_405 },
	{ 4100000, RT4831_VOL_410 },
	{ 4150000, RT4831_VOL_415 },
	{ 4200000, RT4831_VOL_420 },
	{ 4250000, RT4831_VOL_425 },
	{ 4300000, RT4831_VOL_430 },
	{ 4350000, RT4831_VOL_435 },
	{ 4400000, RT4831_VOL_440 },
	{ 4450000, RT4831_VOL_445 },
	{ 4500000, RT4831_VOL_450 },
	{ 4550000, RT4831_VOL_455 },
	{ 4600000, RT4831_VOL_460 },
	{ 4650000, RT4831_VOL_465 },
	{ 4700000, RT4831_VOL_470 },
	{ 4750000, RT4831_VOL_475 },
	{ 4800000, RT4831_VOL_480 },
	{ 4850000, RT4831_VOL_485 },
	{ 4900000, RT4831_VOL_490 },
	{ 4950000, RT4831_VOL_495 },
	{ 5000000, RT4831_VOL_500 },
	{ 5050000, RT4831_VOL_505 },
	{ 5100000, RT4831_VOL_510 },
	{ 5150000, RT4831_VOL_515 },
	{ 5200000, RT4831_VOL_520 },
	{ 5250000, RT4831_VOL_525 },
	{ 5300000, RT4831_VOL_530 },
	{ 5350000, RT4831_VOL_535 },
	{ 5400000, RT4831_VOL_540 },
	{ 5450000, RT4831_VOL_545 },
	{ 5500000, RT4831_VOL_550 },
	{ 5550000, RT4831_VOL_555 },
	{ 5600000, RT4831_VOL_560 },
	{ 5650000, RT4831_VOL_565 },
	{ 5700000, RT4831_VOL_570 },
	{ 5750000, RT4831_VOL_575 },
	{ 5800000, RT4831_VOL_580 },
	{ 5850000, RT4831_VOL_585 },
	{ 5900000, RT4831_VOL_590 },
	{ 5950000, RT4831_VOL_595 },
	{ 6000000, RT4831_VOL_600 },
	{ 6050000, RT4831_VOL_605 },
	{ 6400000, RT4831_VOL_640 },
	{ 6450000, RT4831_VOL_645 },
	{ 6500000, RT4831_VOL_650 },
};


static int rt4831_i2c_read_u8(unsigned char chip_no, unsigned char *data_buffer, int addr)
{
	int ret = -1;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t rt4831_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	rt4831_i2c.id = rt4831_bl_info.rt4831_i2c_bus_id;
	rt4831_i2c.addr = chip_no;
	rt4831_i2c.mode = ST_MODE;
	rt4831_i2c.speed =RT4831_I2C_SPEED;

	ret = i2c_write_read(&rt4831_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int rt4831_i2c_write_u8(unsigned char chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[RT4831_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = RT4831_WRITE_LEN;
	struct mt_i2c_t rt4831_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	rt4831_i2c.id = rt4831_bl_info.rt4831_i2c_bus_id;
	rt4831_i2c.addr = chip_no;
	rt4831_i2c.mode = ST_MODE;
	rt4831_i2c.speed = RT4831_I2C_SPEED;

	ret = i2c_write(&rt4831_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int rt4831_i2c_update_bits(unsigned char chip_no,
	int reg, unsigned char mask, unsigned char val)
{
	int ret;
	unsigned char tmp;
	unsigned char orig = 0;

	ret = rt4831_i2c_read_u8(chip_no, &orig, reg);
	if (ret < 0)
		return ret;

	tmp = orig & ~mask;
	tmp |= val & mask;

	if (tmp != orig) {
		ret = rt4831_i2c_write_u8(chip_no, reg, val);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static void rt4831_get_target_voltage(unsigned int *vpos, unsigned int *vneg)
{
	int i;
	int tal_size = sizeof(rt4831_vol_table) / sizeof(struct rt4831_voltage);

	if ((vpos == NULL) || (vneg == NULL)) {
		LCD_KIT_ERR("vpos or vneg is null\n");
		return;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (rt4831_vol_table[i].voltage == power_hdl->lcd_vsp.buf[2]) {
			*vpos = rt4831_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsp voltage, use default voltage\n");
		*vpos = RT4831_VOL_600;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (rt4831_vol_table[i].voltage == power_hdl->lcd_vsn.buf[2]) {
			*vneg = rt4831_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsn voltage, use default voltage\n");
		*vneg = RT4831_VOL_600;
	}

	LCD_KIT_INFO("*vpos_target=0x%x,*vneg_target=0x%x\n", *vpos, *vneg);

	return;
}

static int rt4831_parse_dts(void)
{
	int ret;
	int i;
	unsigned int vpos_target = 0;
	unsigned int vneg_target = 0;

	LCD_KIT_INFO("rt4831_parse_dts +!\n");
	for (i = 0; i < RT4831_RW_REG_MAX; i++) {
		ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
		rt4831_bl_info.nodeoffset, rt4831_dts_string[i],
		&rt4831_bl_info.rt4831_reg[i], 0);
		if (ret < 0)
			LCD_KIT_INFO("can not find %s dts\n", rt4831_dts_string[i]);
		else
			LCD_KIT_INFO("get %s value = 0x%x\n",
				rt4831_dts_string[i], rt4831_bl_info.rt4831_reg[i]);
	}
	if (!rt4831_bl_info.bl_only) {
		rt4831_get_target_voltage(&vpos_target, &vneg_target);
		/* 9  is the position of vsp in rt4831_reg */
		/* 10 is the position of vsn in rt4831_reg */
		if (rt4831_bl_info.rt4831_reg[9] != vpos_target)
			rt4831_bl_info.rt4831_reg[9] = vpos_target;
		if (rt4831_bl_info.rt4831_reg[10] != vneg_target)
			rt4831_bl_info.rt4831_reg[10] = vneg_target;
	}

	return ret;
}

static int rt4831_config_register(void)
{
	int ret = LCD_KIT_OK;
	int i;
	for (i = 0;i < RT4831_RW_REG_MAX;i++) {
		ret = rt4831_i2c_write_u8(RT4831_SLAV_ADDR, rt4831_reg_addr[i],
				rt4831_bl_info.rt4831_reg[i]);
		if (ret < 0) {
			LCD_KIT_ERR("write rt4831 backlight config register 0x%x failed", rt4831_reg_addr[i]);
			return ret;
		}
	}

	return ret;
}

static int rt4831_device_verify(void)
{
	int ret;
	unsigned char reg_val = 0;

	if (rt4831_bl_info.rt4831_hw_en) {
		mt_set_gpio_mode(rt4831_bl_info.rt4831_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(rt4831_bl_info.rt4831_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(rt4831_bl_info.rt4831_hw_en_gpio,
			GPIO_OUT_ONE);
		if (rt4831_bl_info.bl_on_lk_mdelay)
			mdelay(rt4831_bl_info.bl_on_lk_mdelay);
	}

	/* Read IC revision */
	ret = rt4831_i2c_read_u8(RT4831_SLAV_ADDR, &reg_val, REG_REVISION);
	if (ret < 0) {
		LCD_KIT_ERR("read rt4831 revision failed\n");
		goto error_exit;
	}
	LCD_KIT_INFO("rt4831 reg revision = 0x%x\n", reg_val);
	if ((reg_val & DEV_MASK) != VENDOR_ID_RT) {
		LCD_KIT_INFO("rt4831 check vendor id failed\n");
		ret = LCD_KIT_FAIL;
		goto error_exit;
	}

	return LCD_KIT_OK;
error_exit:
	if (rt4831_bl_info.rt4831_hw_en)
		mt_set_gpio_out(rt4831_bl_info.rt4831_hw_en_gpio,
			GPIO_OUT_ZERO);
	return ret;
}

int rt4831_backlight_ic_check()
{
	int ret = LCD_KIT_OK;

	if (rt4831_checked) {
		LCD_KIT_INFO("rt4831 already check, not again setting\n");
		return ret;
	}

	ret = rt4831_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("rt4831 is not right backlight ic\n");
	} else {
		rt4831_parse_dts();
		ret = rt4831_config_register();
		if (ret < 0)
			LCD_KIT_ERR("rt4831 config register failed\n");
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("rt4831 is right backlight ic\n");
	}
	rt4831_checked = true;
	return ret;
}

int rt4831_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("rt4831 in\n");
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo->bias_bl_ic_checked != 0) {
		LCD_KIT_ERR("bias bl ic checked\n");
		return LCD_KIT_OK;
	}
	rt4831_bl_info.pfdt = get_lk_overlayed_dtb();
	if (rt4831_bl_info.pfdt == NULL) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	rt4831_bl_info.nodeoffset = fdt_node_offset_by_compatible(
		rt4831_bl_info.pfdt, OFFSET_DEF_VAL, DTS_COMP_RT4831);
	if (rt4831_bl_info.nodeoffset < 0) {
		LCD_KIT_INFO("can not find %s node\n", DTS_COMP_RT4831);
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
		rt4831_bl_info.nodeoffset, RT4831_SUPPORT,
		&rt4831_bl_info.rt4831_support, 0);
	if (ret < 0 || !rt4831_bl_info.rt4831_support) {
		LCD_KIT_ERR("get rt4831_support failed!\n");
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
		rt4831_bl_info.nodeoffset, RT4831_I2C_BUS_ID,
		&rt4831_bl_info.rt4831_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts rt4831_i2c_bus_id fail!\n");
		rt4831_bl_info.rt4831_i2c_bus_id = 0;
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
		rt4831_bl_info.nodeoffset, GPIO_RT4831_ENABLE,
		&rt4831_bl_info.rt4831_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts rt4831_hw_enable fail!\n");
		rt4831_bl_info.rt4831_hw_en = 0;
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
		rt4831_bl_info.nodeoffset, RT4831_ONLY_BACKLIGHT,
		&rt4831_bl_info.bl_only, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts only_backlight fail\n");
		rt4831_bl_info.bl_only = 0;
		return LCD_KIT_FAIL;
	}

	if (rt4831_bl_info.rt4831_hw_en) {
		ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
			rt4831_bl_info.nodeoffset, RT4831_HW_EN_GPIO,
			&rt4831_bl_info.rt4831_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts rt4831_hw_en_gpio fail!\n");
			rt4831_bl_info.rt4831_hw_en_gpio = 0;
			return LCD_KIT_FAIL;
		}
		ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
			rt4831_bl_info.nodeoffset, RT4831_HW_EN_DELAY,
			&rt4831_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0)
			LCD_KIT_INFO("parse dts bl_on_lk_mdelay fail!\n");
	}

	ret = lcd_kit_get_dts_u32_default(rt4831_bl_info.pfdt,
		rt4831_bl_info.nodeoffset, RT4831_BL_LEVEL,
		&rt4831_bl_info.bl_level, RT4831_BL_DEFAULT_LEVEL);
	if (ret < 0)
		LCD_KIT_ERR("parse dts rt4831_bl_level fail!\n");

	ret = rt4831_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = 1;
		LCD_KIT_INFO("rt4831 is checked\n");
	}

	LCD_KIT_INFO("rt4831 is support\n");

	return LCD_KIT_OK;
}

/**
 * rt4831_set_backlight(): Set Backlight working mode
 *
 * @bl_level: value for backlight ,range from 0 to 2047
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */

int rt4831_set_backlight(uint32_t bl_level)
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
	level = bl_level * rt4831_bl_info.bl_level / RT4831_BL_DEFAULT_LEVEL;

	if (level > RT4831_BL_MAX)
		level = RT4831_BL_MAX;

	/* 11-bit brightness code */
	bl_msb = level >> MSB;
	bl_lsb = level & LSB;

	LCD_KIT_INFO("level = %d, bl_msb = %d, bl_lsb = %d", level, bl_msb, bl_lsb);

	ret = rt4831_i2c_update_bits(RT4831_SLAV_ADDR, REG_BL_BRIGHTNESS_LSB, MASK_BL_LSB, bl_lsb);
	if (ret < 0)
		goto i2c_error;

	ret = rt4831_i2c_write_u8(RT4831_SLAV_ADDR, REG_BL_BRIGHTNESS_MSB, bl_msb);
	if (ret < 0)
		goto i2c_error;

	LCD_KIT_INFO("write rt4831 backlight %u success\n", bl_level);
	
	return ret;

i2c_error:
	LCD_KIT_ERR("i2c access fail to register");
	return ret;
}

void rt4831_set_backlight_status(void)
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
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_RT4831);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find rt4831 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update rt4831 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("rt4831_set_backlight_status OK!\n");
}
