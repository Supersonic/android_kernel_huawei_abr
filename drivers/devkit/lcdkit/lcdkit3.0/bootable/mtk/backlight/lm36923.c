/* Copyright (c) 2019-2019, Huawei terminal Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/

#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>
#include <platform/mt_gpio.h>
#include "lm36923.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"
#include "lcd_kit_utils.h"

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static struct lm36923_backlight_information lm36923_bl_info = {0};
static bool lm36923_init_status = false;
static unsigned int check_status;
static bool lm36923_checked;

static char *lm36923_dts_string[LM36923_RW_REG_MAX] = {
	"lm36923_reg_brightness_conctrol",
	"lm36923_reg_lsb",
	"lm36923_reg_msb",
	"lm36923_reg_leds_config"
};

static unsigned int lm36923_reg_addr[LM36923_RW_REG_MAX] = {
	LM36923_REG_BRIGHTNESS_CONTROL,
	LM36923_REG_RATIO_LSB,
	LM36923_REG_RATIO_MSB,
	LM36923_REG_LEDS_CONFIG
};

static int lm36923_i2c_read_u8(unsigned char addr, unsigned char *data_buffer)
{
	int ret = LM_FAIL;
	/* read date default length */
	const unsigned char len = 1;
	struct mt_i2c_t lm36923_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	lm36923_i2c.id = lm36923_bl_info.lm36923_i2c_bus_id;
	lm36923_i2c.addr = LM36923_SLAV_ADDR;
	lm36923_i2c.mode = ST_MODE;
	lm36923_i2c.speed = LM36923_I2C_SPEED;

	ret = i2c_write_read(&lm36923_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int lm36923_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[LM36923_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = 2;
	struct mt_i2c_t lm36923_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	lm36923_i2c.id = lm36923_bl_info.lm36923_i2c_bus_id;
	lm36923_i2c.addr = LM36923_SLAV_ADDR;
	lm36923_i2c.mode = ST_MODE;
	lm36923_i2c.speed = LM36923_I2C_SPEED;

	ret = i2c_write(&lm36923_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int lm36923_parse_dts(void)
{
	int ret;
	int i;

	LCD_KIT_INFO("lm36923_parse_dts +!\n");
	for (i = 0; i < LM36923_RW_REG_MAX; i++) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM36923,
			lm36923_dts_string[i],
			&lm36923_bl_info.lm36923_reg[i], 0);
		if (ret < 0) {
			lm36923_bl_info.lm36923_reg[i] = LM36923_INVALID_VAL;
			LCD_KIT_INFO("can not find %s dts\n", lm36923_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				lm36923_dts_string[i], lm36923_bl_info.lm36923_reg[i]);
		}
	}

	return ret;
}

static int lm36923_config_register(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < LM36923_RW_REG_MAX; i++) {
		if (lm36923_bl_info.lm36923_reg[i] != LM36923_INVALID_VAL) {
			ret = lm36923_i2c_write_u8(lm36923_reg_addr[i],
				(u8)lm36923_bl_info.lm36923_reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write lm36923 backlight config 0x%x failed\n",
					lm36923_reg_addr[i]);
				return ret;
			}
		}
	}

	return ret;
}

static void lm36923_enable(void)
{
	int ret;

	if (lm36923_bl_info.lm36923_hw_en) {
		mt_set_gpio_mode(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_OUT_ONE);
		if (lm36923_bl_info.bl_on_lk_mdelay)
			mdelay(lm36923_bl_info.bl_on_lk_mdelay);
	}
	ret = lm36923_config_register();
	if (ret < 0) {
		LCD_KIT_ERR("lm36923 config register failed\n");
		return;
	}
	lm36923_init_status = true;
}

static void lm36923_disable(void)
{
	if (lm36923_bl_info.lm36923_hw_en)
		mt_set_gpio_out(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_OUT_ZERO);

	lm36923_init_status = false;
}

static int lm36923_set_backlight(unsigned int bl_level)
{
	int bl_msb;
	int bl_lsb;
	int ret;

	if (bl_level > LM36923_BL_DEFAULT_LEVEL)
		bl_level = LM36923_BL_DEFAULT_LEVEL;

	/* first set backlight, enable lm36923 */
	if ((lm36923_init_status == false) && (bl_level > 0))
		lm36923_enable();

	bl_level = bl_level * lm36923_bl_info.bl_level / LM36923_BL_DEFAULT_LEVEL;

	/* set backlight level */
	bl_msb = (bl_level >> LM36923_MSB_LEN) & LM36923_MSB_MASK;
	bl_lsb = bl_level & LM36923_LSB_MASK;
	ret = lm36923_i2c_write_u8(LM36923_REG_RATIO_LSB, bl_lsb);
	if (ret < 0)
		LCD_KIT_ERR("write lm36923 backlight level lsb:0x%x failed\n", bl_lsb);

	ret = lm36923_i2c_write_u8(LM36923_REG_RATIO_MSB, bl_msb);
	if (ret < 0)
		LCD_KIT_ERR("write lm36923 backlight level msb:0x%x failed\n", bl_msb);

	LCD_KIT_INFO("write lm36923 backlight %u success\n", bl_level);

	/* if set backlight level 0, disable lm36923 */
	if ((lm36923_init_status == true) && (bl_level == 0))
		lm36923_disable();

	return ret;
}

void lm36923_set_backlight_status (void)
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
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_LM36923);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find lm36923 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update lm36923 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("lm36923_set_backlight_status OK!\n");
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = lm36923_set_backlight,
};

static int lm36923_device_verify(void)
{
	int ret;
	unsigned char vendor_id = 0;
	unsigned char led_config = 0;
	if (lm36923_bl_info.lm36923_hw_en) {
		mt_set_gpio_mode(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_OUT_ONE);
		if (lm36923_bl_info.bl_on_lk_mdelay)
			mdelay(lm36923_bl_info.bl_on_lk_mdelay);
	}

	ret = lm36923_i2c_read_u8(LM36923_REG_DEV_ID, &vendor_id);
	if (ret < 0) {
		LCD_KIT_ERR("no lm36923 device, read vendor id failed\n");
		goto error_exit;
	}

	ret = lm36923_i2c_read_u8(LM36923_REG_LEDS_CONFIG, &led_config);
	if (ret < 0) {
		LCD_KIT_ERR("no lm36923 device, read led_config failed\n");
		goto error_exit;
	}

	if ((led_config & LM36923_REG_LEDS_CONFIG_MASK) !=
		LM36923_REG_LEDS_CONFIG_DEFAULT) {
		ret = LM_FAIL;
		LCD_KIT_ERR("no lm36923 device, led config is %x\n", led_config);
		goto error_exit;
	}

error_exit:
	if (lm36923_bl_info.lm36923_hw_en)
		mt_set_gpio_out(lm36923_bl_info.lm36923_hw_en_gpio,
			GPIO_OUT_ZERO);

	return ret;
}


int lm36923_backlight_ic_recognize(void)
{
	int ret = LM_SUCC;

	if (lm36923_checked) {
		LCD_KIT_INFO("lm36923 already check, not again setting\n");
		return ret;
	}
	ret = lm36923_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("lm36923 is not right backlight ics\n");
	} else {
		lm36923_parse_dts();
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("lm36923 is right backlight ic\n");
	}
	lm36923_checked = true;
	return ret;
}

int lm36923_init(void)
{
	int ret;

	LCD_KIT_INFO("lm36923 in %s\n", __func__);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM36923,
		LM36923_SUPPORT, &lm36923_bl_info.lm36923_support, 0);
	if (ret < 0 || !lm36923_bl_info.lm36923_support) {
		LCD_KIT_ERR("not support lm36923!\n");
		return LM_SUCC;
	}
	/* register bl ops */
	lcd_kit_backlight_recognize_register(lm36923_backlight_ic_recognize);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM36923,
		LM36923_I2C_BUS_ID, &lm36923_bl_info.lm36923_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lm36923_i2c_bus_id fail!\n");
		lm36923_bl_info.lm36923_i2c_bus_id = 0;
		return LM_SUCC;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM36923,
		LM36923_HW_ENABLE, &lm36923_bl_info.lm36923_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lm36923_hw_enable fail!\n");
		lm36923_bl_info.lm36923_hw_en = 0;
		return LM_SUCC;
	}
	if (lm36923_bl_info.lm36923_hw_en) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM36923,
			LM36923_HW_EN_GPIO,
			&lm36923_bl_info.lm36923_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts lm36923_hw_en_gpio fail!\n");
			lm36923_bl_info.lm36923_hw_en_gpio = 0;
			return LM_SUCC;
		}
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM36923,
			LM36923_HW_EN_DELAY,
			&lm36923_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts bl_on_lk_mdelay fail!\n");
			lm36923_bl_info.bl_on_lk_mdelay = 0;
			return LM_SUCC;
		}
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_LM36923,
		LM36923_BL_LEVEL, &lm36923_bl_info.bl_level,
		LM36923_BL_DEFAULT_LEVEL);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lm36923_bl_level fail!\n");
		return LM_SUCC;
	}
	LCD_KIT_INFO("[%s]:lm36923 is support\n", __FUNCTION__);

	return LM_SUCC;
}
