/*
 * ktd3133.c
 *
 * ktd3133 backlight driver
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

#include "ktd3133.h"
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

static struct ktd3133_backlight_information ktd3133_bl_info = {0};
static bool ktd3133_init_status;
static bool ktd3133_checked;
static unsigned int check_status;
static int ktd3133_reg_ratio_lsb_brightness[BRIGHTNESS_LEVEL] = {
	0x00, 0x04, 0x03, 0x04, 0x04, 0x05, 0x03, 0x02,
	0x03, 0x00, 0x02, 0x00, 0x04, 0x05, 0x05, 0x04,
	0x01, 0x04, 0x07, 0x01, 0x02, 0x02, 0x01, 0x00,
	0x06, 0x03, 0x00, 0x05, 0x01, 0x04, 0x00, 0x03,
	0x05, 0x00, 0x01, 0x03, 0x05, 0x06, 0x07, 0x07,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x06,
	0x05, 0x04, 0x03, 0x01, 0x00, 0x06, 0x05, 0x03,
	0x01, 0x07, 0x05, 0x03, 0x00, 0x06, 0x03, 0x01,
	0x06, 0x03, 0x00, 0x05, 0x02, 0x07, 0x04, 0x01,
	0x06, 0x02, 0x07, 0x04, 0x00, 0x04, 0x01, 0x05,
	0x01, 0x06, 0x02, 0x06, 0x02, 0x06, 0x02, 0x06,
	0x02, 0x05, 0x01, 0x05, 0x01, 0x04, 0x00, 0x04,
	0x07, 0x03, 0x06, 0x02, 0x05, 0x00, 0x04, 0x07,
	0x02, 0x05, 0x01, 0x04, 0x07, 0x02, 0x05, 0x00,
	0x03, 0x06, 0x01, 0x04, 0x07, 0x02, 0x05, 0x00,
	0x03, 0x06, 0x00, 0x03, 0x06, 0x01, 0x03, 0x06,
	0x01, 0x03, 0x06, 0x01, 0x03, 0x06, 0x00, 0x03,
	0x05, 0x00, 0x02, 0x05, 0x07, 0x02, 0x04, 0x06,
	0x01, 0x03, 0x05, 0x00, 0x02, 0x04, 0x07, 0x01,
	0x03, 0x05, 0x07, 0x02, 0x04, 0x06, 0x00, 0x02,
	0x04, 0x07, 0x01, 0x03, 0x05, 0x07, 0x01, 0x03,
	0x05, 0x07, 0x01, 0x03, 0x05, 0x07, 0x01, 0x03,
	0x05, 0x07, 0x01, 0x03, 0x05, 0x06, 0x00, 0x02,
	0x04, 0x06, 0x00, 0x02, 0x03, 0x05, 0x07, 0x01,
	0x02, 0x04, 0x06, 0x00, 0x02, 0x03, 0x05, 0x07,
	0x00, 0x02, 0x04, 0x05, 0x07, 0x01, 0x02, 0x04,
	0x06, 0x07, 0x01, 0x03, 0x04, 0x06, 0x07, 0x01,
	0x03, 0x04, 0x06, 0x07, 0x01, 0x02, 0x04, 0x05,
	0x07, 0x00, 0x02, 0x03, 0x05, 0x06, 0x00, 0x01,
	0x03, 0x04, 0x06, 0x07, 0x01, 0x02, 0x04, 0x05,
	0x06, 0x00, 0x01, 0x03, 0x04, 0x05, 0x07, 0x00,
	0x02, 0x03, 0x04, 0x06, 0x07, 0x00, 0x02, 0x03
};

static int ktd3133_reg_ratio_msb_brightness[BRIGHTNESS_LEVEL] = {
	0x00, 0x2B, 0x3E, 0x4B, 0x55, 0x5D, 0x64, 0x6A,
	0x6F, 0x74, 0x78, 0x7C, 0x7F, 0x82, 0x85, 0x88,
	0x8B, 0x8D, 0x8F, 0x92, 0x94, 0x96, 0x98, 0x9A,
	0x9B, 0x9D, 0x9F, 0xA0, 0xA2, 0xA3, 0xA5, 0xA6,
	0xA7, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB5, 0xB6, 0xB7,
	0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBC, 0xBD, 0xBE,
	0xBF, 0xBF, 0xC0, 0xC1, 0xC2, 0xC2, 0xC3, 0xC4,
	0xC4, 0xC5, 0xC6, 0xC6, 0xC7, 0xC7, 0xC8, 0xC9,
	0xC9, 0xCA, 0xCA, 0xCB, 0xCC, 0xCC, 0xCD, 0xCD,
	0xCE, 0xCE, 0xCF, 0xCF, 0xD0, 0xD0, 0xD1, 0xD1,
	0xD2, 0xD2, 0xD3, 0xD3, 0xD4, 0xD4, 0xD5, 0xD5,
	0xD5, 0xD6, 0xD6, 0xD7, 0xD7, 0xD8, 0xD8, 0xD8,
	0xD9, 0xD9, 0xDA, 0xDA, 0xDA, 0xDB, 0xDB, 0xDC,
	0xDC, 0xDC, 0xDD, 0xDD, 0xDD, 0xDE, 0xDE, 0xDF,
	0xDF, 0xDF, 0xE0, 0xE0, 0xE0, 0xE1, 0xE1, 0xE1,
	0xE2, 0xE2, 0xE2, 0xE3, 0xE3, 0xE3, 0xE4, 0xE4,
	0xE4, 0xE5, 0xE5, 0xE5, 0xE5, 0xE6, 0xE6, 0xE6,
	0xE7, 0xE7, 0xE7, 0xE8, 0xE8, 0xE8, 0xE8, 0xE9,
	0xE9, 0xE9, 0xE9, 0xEA, 0xEA, 0xEA, 0xEB, 0xEB,
	0xEB, 0xEB, 0xEC, 0xEC, 0xEC, 0xEC, 0xED, 0xED,
	0xED, 0xED, 0xEE, 0xEE, 0xEE, 0xEE, 0xEF, 0xEF,
	0xEF, 0xEF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF1, 0xF1,
	0xF1, 0xF1, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF3,
	0xF3, 0xF3, 0xF3, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4,
	0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF6, 0xF6, 0xF6,
	0xF6, 0xF6, 0xF7, 0xF7, 0xF7, 0xF7, 0xF7, 0xF8,
	0xF8, 0xF8, 0xF8, 0xF8, 0xF9, 0xF9, 0xF9, 0xF9,
	0xF9, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB, 0xFB,
	0xFB, 0xFB, 0xFB, 0xFB, 0xFC, 0xFC, 0xFC, 0xFC,
	0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE,
	0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF
};
static char *ktd3133_dts_string[KTD3133_RW_REG_MAX] = {
	"ktd3133_reg_control",
	"ktd3133_reg_lsb",
	"ktd3133_reg_msb",
	"ktd3133_reg_pwm",
	"ktd3133_reg_ramp_on",
	"ktd3133_reg_transit_ramp",
	"ktd3133_reg_mode",
};

static unsigned char ktd3133_reg_addr[KTD3133_RW_REG_MAX] = {
	KTD3133_REG_CONTROL,
	KTD3133_REG_RATIO_LSB,
	KTD3133_REG_RATIO_MSB,
	KTD3133_REG_PWM,
	KTD3133_REG_RAMP_ON,
	KTD3133_REG_TRANS_RAMP,
	KTD3133_REG_MODE,
};

static int ktd3133_i2c_read_u8(unsigned char addr, unsigned char *dataBuffer)
{
	int ret;
	unsigned char len;
	struct mt_i2c_t ktd3133_i2c = {0};
	*dataBuffer = addr;

	ktd3133_i2c.id = ktd3133_bl_info.ktd3133_i2c_bus_id;
	ktd3133_i2c.addr = KTD3133_SLAV_ADDR;
	ktd3133_i2c.mode = ST_MODE;
	ktd3133_i2c.speed = 100;
	len = 1;

	ret = i2c_write_read(&ktd3133_i2c, dataBuffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read  failed! reg is 0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int ktd3133_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[2] = {0};
	unsigned char len;
	struct mt_i2c_t ktd3133_i2c = {0};

	write_data[0] = addr;
	write_data[1] = value;

	ktd3133_i2c.id = ktd3133_bl_info.ktd3133_i2c_bus_id;
	ktd3133_i2c.addr = KTD3133_SLAV_ADDR;
	ktd3133_i2c.mode = ST_MODE;
	ktd3133_i2c.speed = 100;
	len = 2;

	ret = i2c_write(&ktd3133_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write  failed! reg is  0x%x ret: %d\n",
			__func__, addr, ret);
	return ret;
}

static int ktd3133_parse_dts(void)
{
	int ret = 0;
	int i;

	LCD_KIT_INFO("ktd3133_parse_dts +!\n");

	for (i = 0; i < KTD3133_RW_REG_MAX; i++) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3133,
			ktd3133_dts_string[i],
			&ktd3133_bl_info.ktd3133_reg[i], 0);
		if (ret < 0) {
			ktd3133_bl_info.ktd3133_reg[i] = 0xffff;
			LCD_KIT_INFO("can not find %s dts\n",
				ktd3133_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				ktd3133_dts_string[i],
				ktd3133_bl_info.ktd3133_reg[i]);
		}
	}

	return ret;
}

static int ktd3133_config_register(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < KTD3133_RW_REG_MAX; i++) {
		if (ktd3133_bl_info.ktd3133_reg[i] != 0xffff) {
			ret = ktd3133_i2c_write_u8(ktd3133_reg_addr[i],
				(u8)ktd3133_bl_info.ktd3133_reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write ktd3133 backlight config register 0x%x failed\n",
					ktd3133_reg_addr[i]);
				goto exit;
			}
		}
	}
exit:
	return ret;
}

static void ktd3133_enable(void)
{
	int ret;

	if (ktd3133_bl_info.ktd3133_hw_en) {
		mt_set_gpio_mode(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_OUT_ONE);
		if (ktd3133_bl_info.bl_on_lk_mdelay)
			mdelay(ktd3133_bl_info.bl_on_lk_mdelay);
	}
	ret = ktd3133_config_register();
	if (ret < 0) {
		LCD_KIT_ERR("ktd3133 config register failed\n");
		return;
	}
	ktd3133_init_status = true;
}

static void ktd3133_disable(void)
{
	if (ktd3133_bl_info.ktd3133_hw_en)
		mt_set_gpio_out(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_OUT_ZERO);
	ktd3133_init_status = false;
}

static int ktd3133_set_backlight(uint32_t bl_level)
{
	int bl_msb = 0;
	int bl_lsb = 0;
	int ret;

	/* first set backlight, enable ktd3133 */
	if (ktd3133_init_status == false && bl_level > 0)
		ktd3133_enable();

	/* set backlight level */
	ret = ktd3133_i2c_write_u8(KTD3133_REG_RATIO_LSB,
		ktd3133_reg_ratio_lsb_brightness[bl_level]);
	if (ret < 0)
		LCD_KIT_ERR("write ktd3133 backlight level lsb:0x%x failed\n",
			bl_lsb);
	ret = ktd3133_i2c_write_u8(KTD3133_REG_RATIO_MSB,
		ktd3133_reg_ratio_msb_brightness[bl_level]);
	if (ret < 0)
		LCD_KIT_ERR("write ktd3133 backlight level msb:0x%x failed\n",
			bl_msb);
	LCD_KIT_INFO("write ktd3133 backlight level success\n");

	/* if set backlight level 0, disable ktd3133 */
	if (ktd3133_init_status == true && bl_level == 0)
		ktd3133_disable();

	return ret;
}

void ktd3133_set_backlight_status(void)
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
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_KTD3133);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find ktd3133 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update ktd3133 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("ktd3133_set_backlight_status OK!\n");
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = ktd3133_set_backlight,
};

static int ktd3133_device_verify(void)
{
	int ret;
	unsigned char vendor_id = 0;

	if (ktd3133_bl_info.ktd3133_hw_en) {
		mt_set_gpio_mode(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_OUT_ONE);
		if (ktd3133_bl_info.bl_on_lk_mdelay)
			mdelay(ktd3133_bl_info.bl_on_lk_mdelay);
	}

	ret = ktd3133_i2c_read_u8(KTD3133_REG_DEV_ID, &vendor_id);
	if (ret < 0) {
		LCD_KIT_INFO("no ktd3133 device, read vendor id failed\n");
		goto error_exit;
	}
	if (vendor_id != KTD3133_VENDOR_ID) {
		LCD_KIT_INFO("no ktd3133 device, vendor id is not right\n");
		goto error_exit;
	}
	if (ktd3133_bl_info.ktd3133_hw_en)
		mt_set_gpio_out(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_OUT_ZERO);
	return 0;
error_exit:
	if (ktd3133_bl_info.ktd3133_hw_en)
		mt_set_gpio_out(ktd3133_bl_info.ktd3133_hw_en_gpio,
			GPIO_OUT_ZERO);

	return LCD_KIT_FAIL;
}

int ktd3133_backlight_ic_recognize(void)
{
	int ret = 0;

	if (ktd3133_checked) {
		LCD_KIT_INFO("ktd3133 already check ,not again setting\n");
		return ret;
	}
	ret = ktd3133_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("ktd3133 is not right backlight ics\n");
	} else {
		ktd3133_parse_dts();
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("ktd3133 is right backlight ic\n");
	}
	ktd3133_checked = true;
	return ret;
}

int ktd3133_init(void)
{
	int ret;

	LCD_KIT_INFO("ktd3133 in %s\n", __func__);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3133,
		KTD3133_SUPPORT,
		&ktd3133_bl_info.ktd3133_support, 0);
	if ((ret < 0) || (!ktd3133_bl_info.ktd3133_support)) {
		LCD_KIT_ERR("not support ktd3133!\n");
		return LCD_KIT_OK;
	}

	/* register bl ops */
	lcd_kit_backlight_recognize_register(ktd3133_backlight_ic_recognize);
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3133,
		KTD3133_I2C_BUS_ID,
		&ktd3133_bl_info.ktd3133_i2c_bus_id, 0);
	if (ret < 0) {
		ktd3133_bl_info.ktd3133_i2c_bus_id = 0;
		return LCD_KIT_OK;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3133,
		KTD3133_HW_ENABLE,
		&ktd3133_bl_info.ktd3133_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts ktd3133_hw_enable fail!\n");
		ktd3133_bl_info.ktd3133_hw_en = 0;
		return LCD_KIT_OK;
	}
	if (ktd3133_bl_info.ktd3133_hw_en) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3133,
			KTD3133_HW_EN_GPIO,
			&ktd3133_bl_info.ktd3133_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts ktd3133_hw_en_gpio fail!\n");
			ktd3133_bl_info.ktd3133_hw_en_gpio = 0;
			return LCD_KIT_OK;
		}
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3133,
			KTD3133_HW_EN_DELAY,
			&ktd3133_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts bl_on_lk_mdelay fail!\n");
			ktd3133_bl_info.bl_on_lk_mdelay = 0;
			return LCD_KIT_OK;
		}
	}
	LCD_KIT_INFO("[%s]:ktd3133 is support!\n", __func__);
	LCD_KIT_INFO("ktd3133 out %s\n", __func__);
	return LCD_KIT_OK;
}
