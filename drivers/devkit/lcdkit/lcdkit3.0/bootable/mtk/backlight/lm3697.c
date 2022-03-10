/*
 * lm3697.c
 *
 * lm3697 backlight driver
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

#include "lm3697.h"
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>
#include <platform/mt_gpio.h>
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"
#include "lcd_kit_utils.h"

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static struct lm3697_backlight_information lm3697_bl_info = {0};
static bool lm3697_init_status;
static bool lm3697_checked;
static unsigned int check_status;

static int lm3697_reg_brightness_lsb_b_brightness[BRIGHTNESS_LEVEL] = {
	0x00, 0x00, 0x07, 0x06, 0x01, 0x00, 0x05, 0x05,
	0x07, 0x06, 0x02, 0x03, 0x03, 0x00, 0x04, 0x07,
	0x01, 0x01, 0x01, 0x00, 0x06, 0x03, 0x00, 0x04,
	0x00, 0x04, 0x06, 0x01, 0x03, 0x05, 0x06, 0x07,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x07, 0x06, 0x05, 0x03, 0x02, 0x00, 0x06, 0x04,
	0x02, 0x00, 0x06, 0x03, 0x01, 0x06, 0x03, 0x01,
	0x06, 0x03, 0x00, 0x04, 0x01, 0x06, 0x02, 0x07,
	0x03, 0x00, 0x04, 0x00, 0x05, 0x01, 0x05, 0x01,
	0x05, 0x01, 0x04, 0x00, 0x04, 0x00, 0x03, 0x07,
	0x03, 0x06, 0x02, 0x05, 0x00, 0x04, 0x07, 0x02,
	0x06, 0x01, 0x04, 0x07, 0x02, 0x05, 0x00, 0x03,
	0x06, 0x01, 0x04, 0x07, 0x02, 0x05, 0x00, 0x02,
	0x05, 0x00, 0x02, 0x05, 0x00, 0x02, 0x05, 0x00,
	0x02, 0x05, 0x07, 0x02, 0x04, 0x07, 0x01, 0x03,
	0x06, 0x00, 0x02, 0x05, 0x07, 0x01, 0x04, 0x06,
	0x00, 0x02, 0x05, 0x07, 0x01, 0x03, 0x05, 0x07,
	0x01, 0x03, 0x06, 0x00, 0x02, 0x04, 0x06, 0x00,
	0x02, 0x04, 0x06, 0x00, 0x01, 0x03, 0x05, 0x07,
	0x01, 0x03, 0x05, 0x07, 0x00, 0x02, 0x04, 0x06,
	0x00, 0x01, 0x03, 0x05, 0x07, 0x00, 0x02, 0x04,
	0x06, 0x07, 0x01, 0x03, 0x04, 0x06, 0x00, 0x01,
	0x03, 0x04, 0x06, 0x00, 0x01, 0x03, 0x04, 0x06,
	0x00, 0x01, 0x03, 0x04, 0x06, 0x07, 0x01, 0x02,
	0x04, 0x05, 0x07, 0x00, 0x02, 0x03, 0x04, 0x06,
	0x07, 0x01, 0x02, 0x04, 0x05, 0x06, 0x00, 0x01,
	0x03, 0x04, 0x05, 0x07, 0x00, 0x01, 0x03, 0x04,
	0x05, 0x07, 0x00, 0x01, 0x03, 0x04, 0x05, 0x06,
	0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x00,
	0x02, 0x03, 0x04, 0x05, 0x07, 0x00, 0x01, 0x02,
	0x03, 0x05, 0x06, 0x07, 0x00, 0x01, 0x02, 0x04,
	0x05, 0x06, 0x07, 0x00, 0x01, 0x02, 0x04, 0x05
};

static int lm3697_reg_brightness_msb_b_brightness[BRIGHTNESS_LEVEL] = {
	0x00, 0x4D, 0x5C, 0x67, 0x70, 0x77, 0x7C, 0x81,
	0x85, 0x89, 0x8D, 0x90, 0x93, 0x96, 0x98, 0x9A,
	0x9D, 0x9F, 0xA1, 0xA3, 0xA4, 0xA6, 0xA8, 0xA9,
	0xAB, 0xAC, 0xAD, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
	0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC,
	0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC1, 0xC2,
	0xC3, 0xC4, 0xC4, 0xC5, 0xC6, 0xC6, 0xC7, 0xC8,
	0xC8, 0xC9, 0xCA, 0xCA, 0xCB, 0xCB, 0xCC, 0xCC,
	0xCD, 0xCE, 0xCE, 0xCF, 0xCF, 0xD0, 0xD0, 0xD1,
	0xD1, 0xD2, 0xD2, 0xD3, 0xD3, 0xD4, 0xD4, 0xD4,
	0xD5, 0xD5, 0xD6, 0xD6, 0xD7, 0xD7, 0xD7, 0xD8,
	0xD8, 0xD9, 0xD9, 0xD9, 0xDA, 0xDA, 0xDB, 0xDB,
	0xDB, 0xDC, 0xDC, 0xDC, 0xDD, 0xDD, 0xDE, 0xDE,
	0xDE, 0xDF, 0xDF, 0xDF, 0xE0, 0xE0, 0xE0, 0xE1,
	0xE1, 0xE1, 0xE1, 0xE2, 0xE2, 0xE2, 0xE3, 0xE3,
	0xE3, 0xE4, 0xE4, 0xE4, 0xE4, 0xE5, 0xE5, 0xE5,
	0xE6, 0xE6, 0xE6, 0xE6, 0xE7, 0xE7, 0xE7, 0xE7,
	0xE8, 0xE8, 0xE8, 0xE9, 0xE9, 0xE9, 0xE9, 0xEA,
	0xEA, 0xEA, 0xEA, 0xEB, 0xEB, 0xEB, 0xEB, 0xEB,
	0xEC, 0xEC, 0xEC, 0xEC, 0xED, 0xED, 0xED, 0xED,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEF, 0xEF, 0xEF,
	0xEF, 0xEF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF1, 0xF1,
	0xF1, 0xF1, 0xF1, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2,
	0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF4, 0xF4,
	0xF4, 0xF4, 0xF4, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
	0xF5, 0xF6, 0xF6, 0xF6, 0xF6, 0xF6, 0xF7, 0xF7,
	0xF7, 0xF7, 0xF7, 0xF7, 0xF8, 0xF8, 0xF8, 0xF8,
	0xF8, 0xF8, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9,
	0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB,
	0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFC, 0xFC, 0xFC,
	0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD,
	0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE
};
static char *lm3697_dts_string[LM3697_RW_REG_MAX] = {
	"lm3697_reg_sink_output_config",
	"lm3697_reg_ramp_on_off_l",
	"lm3697_reg_ramp_on_off_r",
	"lm3697_reg_ramp_time",
	"lm3697_reg_ramp_time_config",
	"lm3697_reg_brightness_config",
	"lm3697_reg_full_scale_current_setting_a",
	"lm3697_reg_full_scale_current_setting_b",
	"lm3697_reg_current_sink_feedback_enable",
	"lm3697_reg_boost_control",
	"lm3697_reg_auto_frequency_threshold",
	"lm3697_reg_pwm_config",
	"lm3697_reg_brightness_lsb_a",
	"lm3697_reg_brightness_msb_a",
	"lm3697_reg_brightness_lsb_b",
	"lm3697_reg_brightness_msb_b",
	"lm3697_reg_bank_enable",
	"lm3697_reg_open_faults",
	"lm3697_reg_short_faults",
	"lm3697_reg_fault_enable",
};

static unsigned char lm3697_reg_addr[LM3697_RW_REG_MAX] = {
	LM3697_REG_SINK_OUTPUT_CONFIG,
	LM3697_REG_RAMP_ON_OFF_L,
	LM3697_REG_RAMP_ON_OFF_R,
	LM3697_REG_RAMP_TIME,
	LM3697_REG_RAMP_TIME_CONFIG,
	LM3697_REG_BRIGHTNESS_CONFIG,
	LM3697_REG_FULL_SCALE_CURRENT_SETTING_A,
	LM3697_REG_FULL_SCALE_CURRENT_SETTING_B,
	LM3697_REG_CURRENT_SINK_FEEDBACK_ENABLE,
	LM3697_REG_BOOST_CONTROL,
	LM3697_REG_AUTO_FREQUENCY_THRESHOLD,
	LM3697_REG_PWM_CONFIG,
	LM3697_REG_BRIGHTNESS_LSB_A,
	LM3697_REG_BRIGHTNESS_MSB_A,
	LM3697_REG_BRIGHTNESS_LSB_B,
	LM3697_REG_BRIGHTNESS_MSB_B,
	LM3697_REG_BANK_ENABLE,
	LM3697_REG_OPEN_FAULTS,
	LM3697_REG_SHORT_FAULTS,
	LM3697_REG_FAULT_ENABLE,
};

static int lm3697_i2c_read_u8(unsigned char addr, unsigned char *dataBuffer)
{
	int ret;
	unsigned char len;
	struct mt_i2c_t lm3697_i2c = {0};
	*dataBuffer = addr;

	lm3697_i2c.id = lm3697_bl_info.lm3697_i2c_bus_id;
	lm3697_i2c.addr = LM3697_SLAV_ADDR;
	lm3697_i2c.mode = ST_MODE;
	lm3697_i2c.speed = 100;
	len = 1;

	ret = i2c_write_read(&lm3697_i2c, dataBuffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read  failed! reg is 0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int lm3697_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[2] = {0};
	unsigned char len;
	struct mt_i2c_t lm3697_i2c = {0};

	write_data[0] = addr;
	write_data[1] = value;

	lm3697_i2c.id = lm3697_bl_info.lm3697_i2c_bus_id;
	lm3697_i2c.addr = LM3697_SLAV_ADDR;
	lm3697_i2c.mode = ST_MODE;
	lm3697_i2c.speed = 100;
	len = 2;

	ret = i2c_write(&lm3697_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write  failed! reg is  0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int lm3697_parse_dts(void)
{
	int ret = 0;
	int i;

	LCD_KIT_INFO("lm3697_parse_dts +!\n");

	for (i = 0; i < LM3697_RW_REG_MAX; i++) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
			lm3697_dts_string[i],
			&lm3697_bl_info.lm3697_reg[i], 0);
		if (ret < 0) {
			lm3697_bl_info.lm3697_reg[i] = 0xffff;
			LCD_KIT_INFO("can not find %s dts\n",
				lm3697_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				lm3697_dts_string[i],
				lm3697_bl_info.lm3697_reg[i]);
		}
	}

	return ret;
}

static int lm3697_config_register(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < LM3697_RW_REG_MAX; i++) {
		if (lm3697_bl_info.lm3697_reg[i] != 0xffff) {
			ret = lm3697_i2c_write_u8(lm3697_reg_addr[i],
				(u8)lm3697_bl_info.lm3697_reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write register 0x%x failed\n",
					lm3697_reg_addr[i]);
				goto exit;
			}
		}
	}
exit:
	return ret;
}

static void lm3697_enable(void)
{
	int ret;

	if (lm3697_bl_info.lm3697_hw_en) {
		mt_set_gpio_mode(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_OUT_ONE);
		if (lm3697_bl_info.bl_on_lk_mdelay)
			mdelay(lm3697_bl_info.bl_on_lk_mdelay);
	}
	ret = lm3697_config_register();
	if (ret < 0) {
		LCD_KIT_ERR("lm3697 config register failed\n");
		return;
	}
	lm3697_init_status = true;
}

static void lm3697_disable(void)
{
	if (lm3697_bl_info.lm3697_hw_en)
		mt_set_gpio_out(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_OUT_ZERO);
	lm3697_init_status = false;
}

static int lm3697_set_backlight(uint32_t bl_level)
{
	int bl_msb = 0;
	int bl_lsb = 0;
	int ret;

	/* first set backlight, enable lm3697 */
	if (lm3697_init_status == false && bl_level > 0)
		lm3697_enable();

	/* set backlight level */
	ret = lm3697_i2c_write_u8(
		(unsigned char)(lm3697_bl_info.lm3697_level_lsb_reg),
		lm3697_reg_brightness_lsb_b_brightness[bl_level]);
	if (ret < 0)
		LCD_KIT_ERR("write lm3697 backlight level lsb:0x%x failed\n",
			bl_lsb);
	ret = lm3697_i2c_write_u8(
		(unsigned char)(lm3697_bl_info.lm3697_level_msb_reg),
		lm3697_reg_brightness_msb_b_brightness[bl_level]);
	if (ret < 0)
		LCD_KIT_ERR("write lm3697 backlight level msb:0x%x failed\n",
		bl_msb);

	/* if set backlight level 0, disable lm3697 */
	if (lm3697_init_status == true && bl_level == 0)
		lm3697_disable();

	return ret;
}

void lm3697_set_backlight_status(void)
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

	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_LM3697);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find lm3697 node,change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update lm3697 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("lm3697_set_backlight_status OK!\n");
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = lm3697_set_backlight,
};

static int lm3697_device_verify(void)
{
	int ret;
	unsigned char vendor_id = 0;

	if (lm3697_bl_info.lm3697_hw_en) {
		mt_set_gpio_mode(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_OUT_ONE);
		if (lm3697_bl_info.bl_on_lk_mdelay)
			mdelay(lm3697_bl_info.bl_on_lk_mdelay);
	}

	ret = lm3697_i2c_read_u8(LM3697_REG_DEV_ID, &vendor_id);
	if (ret < 0) {
		LCD_KIT_INFO("no lm3697 device, read vendor id failed\n");
		goto error_exit;
	}
	if (vendor_id != LM3697_VENDOR_ID) {
		LCD_KIT_INFO("no lm3697 device, vendor id is not right\n");
		goto error_exit;
	}
	if (lm3697_bl_info.lm3697_hw_en)
		mt_set_gpio_out(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_OUT_ZERO);

	return 0;
error_exit:
	if (lm3697_bl_info.lm3697_hw_en)
		mt_set_gpio_out(lm3697_bl_info.lm3697_hw_en_gpio,
			GPIO_OUT_ZERO);
	return LCD_KIT_FAIL;
}


int lm3697_backlight_ic_recognize(void)
{
	int ret = 0;

	if (lm3697_checked) {
		LCD_KIT_INFO("lm3697 already check ,not again setting\n");
		return ret;
	}
	ret = lm3697_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("lm3697 is not right backlight ics\n");
	} else {
		lm3697_parse_dts();
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("lm3697 is right backlight ic\n");
	}
	lm3697_checked = true;
	return ret;
}

int lm3697_init(void)
{
	int ret;

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
		LM3697_SUPPORT, &lm3697_bl_info.lm3697_support, 0);
	if (ret < 0 || !lm3697_bl_info.lm3697_support) {
		LCD_KIT_ERR("not support lm3697!\n");
		return 0;
	}

	/* register bl ops */
	lcd_kit_backlight_recognize_register(lm3697_backlight_ic_recognize);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
		LM3697_I2C_BUS_ID, &lm3697_bl_info.lm3697_i2c_bus_id, 0);
	if (ret < 0) {
		lm3697_bl_info.lm3697_i2c_bus_id = 0;
		return 0;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
		LM3697_HW_ENABLE, &lm3697_bl_info.lm3697_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lm3697_hw_enable fail!\n");
		lm3697_bl_info.lm3697_hw_en = 0;
		return 0;
	}
	if (lm3697_bl_info.lm3697_hw_en) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
			LM3697_HW_EN_GPIO,
			&lm3697_bl_info.lm3697_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts lm3697_hw_en_gpio fail!\n");
			lm3697_bl_info.lm3697_hw_en_gpio = 0;
			return 0;
		}
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
			LM3697_HW_EN_DELAY,
			&lm3697_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts bl_on_lk_mdelay fail!\n");
			lm3697_bl_info.bl_on_lk_mdelay = 0;
			return 0;
		}
	}

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
		LM3697_BACKLIGHT_CONFIG_LSB_REG_ADDR,
		&lm3697_bl_info.lm3697_level_lsb_reg, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lm3697_level_lsb_reg_addr fail!\n");
		lm3697_bl_info.lm3697_level_lsb_reg =
			LM3697_REG_BRIGHTNESS_LSB_B;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM3697,
		LM3697_BACKLIGHT_CONFIG_MSB_REG_ADDR,
		&lm3697_bl_info.lm3697_level_msb_reg, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lm3697_level_msb_reg_addr fail!\n");
		lm3697_bl_info.lm3697_level_msb_reg =
			LM3697_REG_BRIGHTNESS_MSB_B;
	}

	LCD_KIT_INFO("[%s]:lm3697 is support!\n", __func__);
	return 0;
}
