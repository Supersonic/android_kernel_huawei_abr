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

#include "sgm37603a.h"
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"
#include "lcd_kit_utils.h"

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static struct sgm37603a_backlight_information sgm37603a_bl_info = {0};
static bool sgm37603a_init_status = false;
static unsigned int check_status;
static bool sgm37603a_checked;

static char *sgm37603a_dts_string[SGM37603A_RW_REG_MAX] = {
	"sgm37603a_reg_brightness_conctrol",
	"sgm37603a_reg_lsb",
	"sgm37603a_reg_msb",
	"sgm37603a_reg_leds_config"
};

static unsigned int sgm37603a_reg_addr[SGM37603A_RW_REG_MAX] = {
	SGM37603A_REG_BRIGHTNESS_CONTROL,
	SGM37603A_REG_RATIO_LSB,
	SGM37603A_REG_RATIO_MSB,
	SGM37603A_REG_LEDS_CONFIG
};

static int sgm37603a_i2c_read_u8(unsigned char addr, unsigned char *data_buffer)
{
	int ret = SGM_FAIL;
	/* read date default length */
	const unsigned char len = 1;
	struct mt_i2c_t sgm37603a_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	sgm37603a_i2c.id = sgm37603a_bl_info.sgm37603a_i2c_bus_id;
	sgm37603a_i2c.addr = SGM37603A_SLAV_ADDR;
	sgm37603a_i2c.mode = ST_MODE;
	sgm37603a_i2c.speed = SGM37603A_I2C_SPEED;

	ret = i2c_write_read(&sgm37603a_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int sgm37603a_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[SGM37603A_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = 2;
	struct mt_i2c_t sgm37603a_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	sgm37603a_i2c.id = sgm37603a_bl_info.sgm37603a_i2c_bus_id;
	sgm37603a_i2c.addr = SGM37603A_SLAV_ADDR;
	sgm37603a_i2c.mode = ST_MODE;
	sgm37603a_i2c.speed = SGM37603A_I2C_SPEED;

	ret = i2c_write(&sgm37603a_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int sgm37603a_parse_dts(void)
{
	int ret;
	int i;

	LCD_KIT_INFO("sgm37603a_parse_dts +!\n");
	for (i = 0; i < SGM37603A_RW_REG_MAX; i++) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_SGM37603A,
			sgm37603a_dts_string[i],
			&sgm37603a_bl_info.sgm37603a_reg[i], 0);
		if (ret < 0) {
			sgm37603a_bl_info.sgm37603a_reg[i] = SGM37603A_INVALID_VAL;
			LCD_KIT_INFO("can not find %s dts\n", sgm37603a_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				sgm37603a_dts_string[i], sgm37603a_bl_info.sgm37603a_reg[i]);
		}
	}

	return ret;
}

static int sgm37603a_config_register(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < SGM37603A_RW_REG_MAX; i++) {
		if (sgm37603a_bl_info.sgm37603a_reg[i] != SGM37603A_INVALID_VAL) {
			ret = sgm37603a_i2c_write_u8(sgm37603a_reg_addr[i],
				(u8)sgm37603a_bl_info.sgm37603a_reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write sgm37603a backlight config 0x%x failed\n",
					sgm37603a_reg_addr[i]);
				return ret;
			}
		}
	}

	return ret;
}

static void sgm37603a_enable(void)
{
	int ret;

	if (sgm37603a_bl_info.sgm37603a_hw_en) {
		mt_set_gpio_mode(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_OUT_ONE);
		if (sgm37603a_bl_info.bl_on_lk_mdelay)
			mdelay(sgm37603a_bl_info.bl_on_lk_mdelay);
	}
	ret = sgm37603a_config_register();
	if (ret < 0) {
		LCD_KIT_ERR("sgm37603a config register failed\n");
		return;
	}
	sgm37603a_init_status = true;
}

static void sgm37603a_disable(void)
{
	if (sgm37603a_bl_info.sgm37603a_hw_en)
		mt_set_gpio_out(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_OUT_ZERO);

	sgm37603a_init_status = false;
}

static int sgm37603a_set_backlight(unsigned int bl_level)
{
	int bl_msb;
	int bl_lsb;
	int ret;

	if (bl_level > SGM37603A_BL_DEFAULT_LEVEL)
		bl_level = SGM37603A_BL_DEFAULT_LEVEL;

	/* first set backlight, enable sgm37603a */
	if ((sgm37603a_init_status == false) && (bl_level > 0))
		sgm37603a_enable();

	bl_level = bl_level * sgm37603a_bl_info.bl_level / SGM37603A_BL_DEFAULT_LEVEL;

	/* set backlight level */
	bl_msb = (bl_level >> SGM37603A_MSB_LEN) & 0xFF;
	bl_lsb = bl_level & 0x0F;
	ret = sgm37603a_i2c_write_u8(SGM37603A_REG_RATIO_LSB, bl_lsb);
	if (ret < 0)
		LCD_KIT_ERR("write sgm37603a backlight level lsb:0x%x failed\n", bl_lsb);

	ret = sgm37603a_i2c_write_u8(SGM37603A_REG_RATIO_MSB, bl_msb);
	if (ret < 0)
		LCD_KIT_ERR("write sgm37603a backlight level msb:0x%x failed\n", bl_msb);

	LCD_KIT_INFO("write sgm37603a backlight %u success\n", bl_level);

	/* if set backlight level 0, disable sgm37603a */
	if ((sgm37603a_init_status == true) && (bl_level == 0))
		sgm37603a_disable();

	return ret;
}

void sgm37603a_set_backlight_status(void)
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

	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_SGM37603A);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find sgm37603a node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update sgm37603a status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("sgm37603a_set_backlight_status OK!\n");
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = sgm37603a_set_backlight,
};

static int sgm37603a_device_verify(void)
{
	int ret;
	unsigned char vendor_id = 0;
	unsigned char led_config = 0;
	if (sgm37603a_bl_info.sgm37603a_hw_en) {
		mt_set_gpio_mode(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_OUT_ONE);
		if (sgm37603a_bl_info.bl_on_lk_mdelay)
			mdelay(sgm37603a_bl_info.bl_on_lk_mdelay);
	}

	ret = sgm37603a_i2c_read_u8(SGM37603A_REG_DEV_ID, &vendor_id);
	if (ret < 0) {
		LCD_KIT_ERR("no sgm37603a device, read vendor id failed\n");
		goto error_exit;
	}

	ret = sgm37603a_i2c_read_u8(SGM37603A_REG_LEDS_CONFIG, &led_config);
	if (ret < 0) {
		LCD_KIT_ERR("no sgm37603a device, read led_config failed\n");
		goto error_exit;
	}

	if ((led_config & SGM37603A_REG_LEDS_CONFIG_MASK) !=
		SGM37603A_REG_LEDS_CONFIG_DEFAULT) {
		ret = SGM_FAIL;
		LCD_KIT_ERR("no sgm37603a device, led config is %x\n", led_config);
		goto error_exit;
	}

error_exit:
	if (sgm37603a_bl_info.sgm37603a_hw_en)
		mt_set_gpio_out(sgm37603a_bl_info.sgm37603a_hw_en_gpio,
			GPIO_OUT_ZERO);

	return ret;
}


int sgm37603a_backlight_ic_recognize(void)
{
	int ret = SGM_SUCC;

	if (sgm37603a_checked) {
		LCD_KIT_INFO("sgm37603a already check, not again setting\n");
		return ret;
	}
	ret = sgm37603a_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("sgm37603a is not right backlight ics\n");
	} else {
		sgm37603a_parse_dts();
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("sgm37603a is right backlight ic\n");
	}
	sgm37603a_checked = true;
	return ret;
}

int sgm37603a_init(void)
{
	int ret;

	LCD_KIT_INFO("sgm37603a in %s\n", __func__);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_SGM37603A,
		SGM37603A_SUPPORT, &sgm37603a_bl_info.sgm37603a_support, 0);
	if (ret < 0 || !sgm37603a_bl_info.sgm37603a_support) {
		LCD_KIT_ERR("not support sgm37603a!\n");
		return SGM_SUCC;
	}
	/* register bl ops */
	lcd_kit_backlight_recognize_register(sgm37603a_backlight_ic_recognize);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_SGM37603A,
		SGM37603A_I2C_BUS_ID, &sgm37603a_bl_info.sgm37603a_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts sgm37603a_i2c_bus_id fail!\n");
		sgm37603a_bl_info.sgm37603a_i2c_bus_id = 0;
		return SGM_SUCC;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_SGM37603A,
		SGM37603A_HW_ENABLE, &sgm37603a_bl_info.sgm37603a_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts sgm37603a_hw_enable fail!\n");
		sgm37603a_bl_info.sgm37603a_hw_en = 0;
		return SGM_SUCC;
	}
	if (sgm37603a_bl_info.sgm37603a_hw_en) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_SGM37603A,
			SGM37603A_HW_EN_GPIO,
			&sgm37603a_bl_info.sgm37603a_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts sgm37603a_hw_en_gpio fail!\n");
			sgm37603a_bl_info.sgm37603a_hw_en_gpio = 0;
			return SGM_SUCC;
		}
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_SGM37603A,
			SGM37603A_HW_EN_DELAY,
			&sgm37603a_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts bl_on_lk_mdelay fail!\n");
			sgm37603a_bl_info.bl_on_lk_mdelay = 0;
			return SGM_SUCC;
		}
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_SGM37603A,
		SGM37603A_BL_LEVEL, &sgm37603a_bl_info.bl_level,
		SGM37603A_BL_DEFAULT_LEVEL);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts sgm37603a_bl_level fail!\n");
		return SGM_SUCC;
	}
	LCD_KIT_INFO("[%s]:sgm37603a is support\n", __FUNCTION__);

	return SGM_SUCC;
}
