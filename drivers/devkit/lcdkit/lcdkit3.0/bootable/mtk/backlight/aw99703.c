/*
 * aw99703.c
 *
 * aw99703 backlight driver
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
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

#include "lcd_kit_utils.h"
#include "aw99703.h"
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_gpt.h>
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static struct aw99703_backlight_information aw99703_bl_info = {0};
static unsigned int check_status;
static bool aw99703_checked;
static bool aw99703_frist_config = true;

static char *aw99703_dts_string[AW99703_RW_REG_MAX] = {
	"aw99703_reg_pfm_67",
	"aw99703_reg_boot_headroom_23",
	"aw99703_reg_pfm_threshold_25",
	"aw99703_reg_pfm_hys_24",
	"aw99703_reg_led_current",
	"aw99703_reg_boost_conctrol1",
	"aw99703_reg_af_high",
	"aw99703_reg_af_low",
	"aw99703_reg_soft_start_68",
	"aw99703_reg_boot_duty_31",
	"aw99703_reg_flash",
	"aw99703_reg_lsb",
	"aw99703_reg_msb",
	"aw99703_reg_boost_conctrol2",
	"aw99703_reg_mode",
	"aw99703_reg_boot_duty_31_1",
	"aw99703_reg_flash_1",
	"aw99703_reg_soft_start_68_1",
	"aw99703_reg_boost_conctrol2_1"
};

static unsigned int aw99703_reg_addr[AW99703_RW_REG_MAX] = {
	AW99703_REG_PFM_67,
	AW99703_REG_BOOT_HEADROOM_23,
	AW99703_REG_PFM_THRESHOLD_25,
	AW99703_REG_PFM_HYS_24,
	AW99703_REG_LED_CURRENT,
	AW99703_REG_BOOST_CONTROL1,
	AW99703_AUTO_FREQ_HIGH_THRESHOLD,
	AW99703_AUTO_FREQ_LOW_THRESHOLD,
	AW99703_REG_SOFT_START_68,
	AW99703_REG_BOOT_DUTY_31,
	AW99703_FLASH_SETTING,
	AW99703_REG_LED_BRIGHTNESS_LSB,
	AW99703_REG_LED_BRIGHTNESS_MSB,
	AW99703_REG_BOOST_CONTROL2,
	AW99703_REG_MODE,
	AW99703_REG_BOOT_DUTY_31_1,
	AW99703_FLASH_SETTING_1,
	AW99703_REG_SOFT_START_68_1,
	AW99703_REG_BOOST_CONTROL2_1
};

static int aw99703_i2c_read_u8(unsigned char addr, unsigned char *data_buffer)
{
	int ret = LCD_KIT_FAIL;
	/* read date default length */
	const unsigned char len = 1;
	struct mt_i2c_t aw99703_i2c = {0};

	if (data_buffer == NULL) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	aw99703_i2c.id = aw99703_bl_info.aw99703_i2c_bus_id;
	aw99703_i2c.addr = AW99703_SLAV_ADDR;
	aw99703_i2c.mode = ST_MODE;
	aw99703_i2c.speed = AW99703_I2C_SPEED;

	ret = i2c_write_read(&aw99703_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_read failed, reg is 0x%x ret: %d\n",
			addr, ret);

	return ret;
}

static int aw99703_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[AW99703_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = 2;
	struct mt_i2c_t aw99703_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	aw99703_i2c.id = aw99703_bl_info.aw99703_i2c_bus_id;
	aw99703_i2c.addr = AW99703_SLAV_ADDR;
	aw99703_i2c.mode = ST_MODE;
	aw99703_i2c.speed = AW99703_I2C_SPEED;

	ret = i2c_write(&aw99703_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_write failed, reg is 0x%x ret: %d\n",
			addr, ret);

	return ret;
}

static void aw99703_parse_dts(void)
{
	int ret;
	int i;

	LCD_KIT_INFO("aw99703_parse_dts\n");
	for (i = 0; i < AW99703_RW_REG_MAX; i++) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_AW99703,
			aw99703_dts_string[i],
			&aw99703_bl_info.aw99703_reg[i], 0);
		if (ret < 0) {
			aw99703_bl_info.aw99703_reg[i] = AW99703_INVALID_VAL;
			LCD_KIT_INFO("can not find %s dts\n", aw99703_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				aw99703_dts_string[i], aw99703_bl_info.aw99703_reg[i]);
		}
	}
}

static int aw99703_config_register(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < AW99703_RW_REG_MAX; i++) {
		if (aw99703_bl_info.aw99703_reg[i] != AW99703_INVALID_VAL) {
			ret = aw99703_i2c_write_u8(aw99703_reg_addr[i],
				aw99703_bl_info.aw99703_reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write aw99703 reg 0x%x failed\n",
					aw99703_reg_addr[i]);
				return ret;
			}
			LCD_KIT_ERR("write aw99703 reg 0x%x value = 0x%x\n",
					aw99703_reg_addr[i], aw99703_bl_info.aw99703_reg[i]);
			if (aw99703_reg_addr[i] == AW99703_REG_MODE)
					mdelay(AW99703_REG_MODE_DELAY);
		}
	}

	return ret;
}

static int aw99703_set_backlight(unsigned int bl_level)
{
	int bl_msb;
	int bl_lsb;
	int ret;

	bl_level = bl_level * aw99703_bl_info.bl_level / AW99703_BL_DEFAULT_LEVEL;

	if (bl_level > AW99703_BL_MAX)
		bl_level = AW99703_BL_MAX;

	if (aw99703_frist_config) {
		aw99703_frist_config = false;
		ret = aw99703_config_register();
		if (ret < 0) {
			LCD_KIT_ERR("aw99703 config register failed\n");
			return ret;
		}
	}

	/* set backlight level */
	bl_msb = (bl_level >> AW99703_BL_LSB_LEN) & AW99703_BL_MSB_MASK;
	bl_lsb = bl_level & AW99703_BL_LSB_MASK;
	ret = aw99703_i2c_write_u8(AW99703_REG_LED_BRIGHTNESS_LSB, bl_lsb);
	if (ret < 0)
		LCD_KIT_ERR("write aw99703 backlight lsb:0x%x failed\n", bl_lsb);

	ret = aw99703_i2c_write_u8(AW99703_REG_LED_BRIGHTNESS_MSB, bl_msb);
	if (ret < 0)
		LCD_KIT_ERR("write aw99703 backlight msb:0x%x failed\n", bl_msb);

	LCD_KIT_INFO("write aw99703 backlight %u success\n", bl_level);
	return ret;
}

void aw99703_set_backlight_status(void)
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

	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_AW99703);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find aw99703 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update aw99703 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("aw99703_set_backlight_status OK!\n");
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = aw99703_set_backlight,
};

static int aw99703_device_verify(void)
{
	int ret;
	unsigned char chip_id = 0;

	if (aw99703_bl_info.aw99703_hw_en) {
		mt_set_gpio_mode(aw99703_bl_info.aw99703_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(aw99703_bl_info.aw99703_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(aw99703_bl_info.aw99703_hw_en_gpio,
			GPIO_OUT_ONE);
		if (aw99703_bl_info.bl_on_lk_mdelay)
			mdelay(aw99703_bl_info.bl_on_lk_mdelay);
	}

	ret = aw99703_i2c_read_u8(AW99703_REG_DEV_ID, &chip_id);
	if (ret < 0) {
		LCD_KIT_ERR("read aw99703 revision failed\n");
		goto error_exit;
	}

	if(chip_id != AW99703_CHIP_ID) {
		LCD_KIT_ERR("aw99703 check vendor id failed\n");
		ret = LCD_KIT_FAIL;
		goto error_exit;
	}
	return LCD_KIT_OK;
error_exit:
	if (aw99703_bl_info.aw99703_hw_en)
		mt_set_gpio_out(aw99703_bl_info.aw99703_hw_en_gpio,
			GPIO_OUT_ZERO);

	return ret;
}

static int aw99703_backlight_ic_check(void)
{
	int ret = LCD_KIT_OK;

	if (aw99703_checked) {
		LCD_KIT_INFO("aw99703 already check, not again setting\n");
		return ret;
	}
	ret = aw99703_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("aw99703 is not right backlight ics\n");
	} else {
		aw99703_parse_dts();
		check_status = CHECK_STATUS_OK;
		lcd_kit_bl_register(&bl_ops);
		LCD_KIT_INFO("aw99703 is right backlight ic\n");
	}
	aw99703_checked = true;

	return ret;
}

int aw99703_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("aw99703 enter\n");
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}

	if (pinfo->bias_bl_ic_checked != 0) {
		LCD_KIT_ERR("bl ic is checked\n");
		return LCD_KIT_OK;
	}

	if (check_status == CHECK_STATUS_OK) {
		LCD_KIT_ERR("bl ic is checked succ\n");
		return LCD_KIT_OK;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_AW99703,
		AW99703_SUPPORT, &aw99703_bl_info.aw99703_support, 0);
	if (ret < 0 || !aw99703_bl_info.aw99703_support) {
		LCD_KIT_ERR("not support aw99703!\n");
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_AW99703,
		AW99703_I2C_BUS_ID, &aw99703_bl_info.aw99703_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts aw99703_i2c_bus_id fail!\n");
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_AW99703,
		AW99703_HW_ENABLE, &aw99703_bl_info.aw99703_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts aw99703_hw_enable fail!\n");
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_AW99703,
		AW99703_BL_LEVEL, &aw99703_bl_info.bl_level,
		AW99703_BL_DEFAULT_LEVEL);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts aw99703_bl_level fail!\n");
		return LCD_KIT_FAIL;
	}
	if (aw99703_bl_info.aw99703_hw_en) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_AW99703,
			AW99703_HW_EN_GPIO,
			&aw99703_bl_info.aw99703_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts aw99703_hw_en_gpio fail!\n");
			aw99703_bl_info.aw99703_hw_en_gpio = 0;
			return LCD_KIT_FAIL;
		}
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_AW99703,
			AW99703_HW_EN_DELAY,
			&aw99703_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts bl_on_lk_mdelay fail!\n");
			aw99703_bl_info.bl_on_lk_mdelay = 0;
			return LCD_KIT_FAIL;
		}
	}
	ret = aw99703_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = CHECK_STATUS_OK;
		LCD_KIT_INFO("aw99703 is checked succ\n");
	}
	LCD_KIT_INFO("aw99703 is support\n");

	return LCD_KIT_OK;
}
