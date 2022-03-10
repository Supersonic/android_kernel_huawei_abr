/*
 * ktd3136.c
 *
 * ktd3136 backlight driver
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
#include "ktd3136.h"
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_gpt.h>
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static struct ktd3136_backlight_information ktd3136_bl_info = {0};
static unsigned int check_status;
static bool ktd3136_checked;

static char *ktd3136_dts_string[KTD3136_RW_REG_MAX] = {
	"ktd3136_reg_ctrl_a_bl_cfg",
	"ktd3136_reg_ctrl_a_lsb",
	"ktd3136_reg_ctrl_a_msb",
	"ktd3136_reg_pwm_cfg",
	"ktd3136_reg_boost_ctrl",
	"ktd3136_reg_ctrl_a_current_set"
};

static unsigned int ktd3136_reg_addr[KTD3136_RW_REG_MAX] = {
	KTD3136_REG_CTRL_BL_CONFIG,
	KTD3136_REG_CTRL_BRIGHTNESS_LSB,
	KTD3136_REG_CTRL_BRIGHTNESS_MSB,
	KTD3136_REG_CTRL_PWM_CONFIG,
	KTD3136_REG_CTRL_BOOT_CTRL,
	KTD3136_REG_CTRL_CURRENT_SET
};

static int ktd3136_i2c_read_u8(unsigned char addr, unsigned char *data_buffer)
{
	int ret = LCD_KIT_FAIL;
	/* read date default length */
	const unsigned char len = 1;
	struct mt_i2c_t ktd3136_i2c = {0};

	if (data_buffer == NULL) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	ktd3136_i2c.id = ktd3136_bl_info.ktd3136_i2c_bus_id;
	ktd3136_i2c.addr = KTD3136_SLAV_ADDR;
	ktd3136_i2c.mode = ST_MODE;
	ktd3136_i2c.speed = KTD3136_I2C_SPEED;

	ret = i2c_write_read(&ktd3136_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_read failed, reg is 0x%x ret: %d\n",
			addr, ret);

	return ret;
}

static int ktd3136_i2c_write_u8(unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[KTD3136_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = 2;
	struct mt_i2c_t ktd3136_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	ktd3136_i2c.id = ktd3136_bl_info.ktd3136_i2c_bus_id;
	ktd3136_i2c.addr = KTD3136_SLAV_ADDR;
	ktd3136_i2c.mode = ST_MODE;
	ktd3136_i2c.speed = KTD3136_I2C_SPEED;

	ret = i2c_write(&ktd3136_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_write failed, reg is 0x%x ret: %d\n",
			addr, ret);

	return ret;
}

static void ktd3136_parse_dts(void)
{
	int ret;
	int i;

	LCD_KIT_INFO("ktd3136_parse_dts\n");
	for (i = 0; i < KTD3136_RW_REG_MAX; i++) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3136,
			ktd3136_dts_string[i],
			&ktd3136_bl_info.ktd3136_reg[i], 0);
		if (ret < 0) {
			ktd3136_bl_info.ktd3136_reg[i] = KTD3136_INVALID_VAL;
			LCD_KIT_INFO("can not find %s dts\n", ktd3136_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				ktd3136_dts_string[i], ktd3136_bl_info.ktd3136_reg[i]);
		}
	}
}

static int ktd3136_config_register(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < KTD3136_RW_REG_MAX; i++) {
		if (ktd3136_bl_info.ktd3136_reg[i] != KTD3136_INVALID_VAL) {
			ret = ktd3136_i2c_write_u8(ktd3136_reg_addr[i],
				ktd3136_bl_info.ktd3136_reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write ktd3136 reg 0x%x failed\n",
					ktd3136_reg_addr[i]);
				return ret;
			}
		}
	}

	return ret;
}

static int ktd3136_set_backlight(unsigned int bl_level)
{
	unsigned char bl_msb;
	unsigned char bl_lsb;
	int ret;

	bl_level = bl_level * ktd3136_bl_info.bl_level / KTD3136_BL_DEFAULT_LEVEL;

	if (bl_level > KTD3136_BL_MAX)
		bl_level = KTD3136_BL_MAX;

	/* set backlight level */
	bl_msb = (bl_level >> KTD3136_BL_LSB_LEN) & KTD3136_BL_MSB_MASK;
	bl_lsb = bl_level & KTD3136_BL_LSB_MASK;
	ret = ktd3136_i2c_write_u8(KTD3136_REG_CTRL_BRIGHTNESS_LSB, bl_lsb);
	if (ret < 0)
		LCD_KIT_ERR("write ktd3136 backlight lsb:0x%x failed\n", bl_lsb);

	ret = ktd3136_i2c_write_u8(KTD3136_REG_CTRL_BRIGHTNESS_MSB, bl_msb);
	if (ret < 0)
		LCD_KIT_ERR("write ktd3136 backlight msb:0x%x failed\n", bl_msb);

	LCD_KIT_ERR("write ktd3136 backlight %u success\n", bl_level);
	return ret;
}

void ktd3136_set_backlight_status(void)
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

	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_KTD3136);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find ktd3136 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update ktd3136 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("ktd3136_set_backlight_status OK!\n");
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = ktd3136_set_backlight,
};

static int ktd3136_device_verify(void)
{
	int ret;
	unsigned char chip_id = 0;

	if (ktd3136_bl_info.ktd3136_hw_en) {
		mt_set_gpio_mode(ktd3136_bl_info.ktd3136_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(ktd3136_bl_info.ktd3136_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(ktd3136_bl_info.ktd3136_hw_en_gpio,
			GPIO_OUT_ONE);
		if (ktd3136_bl_info.bl_on_lk_mdelay)
			mdelay(ktd3136_bl_info.bl_on_lk_mdelay);
	}

	ret = ktd3136_i2c_read_u8(KTD3136_REG_REVISION, &chip_id);
	if (ret < 0) {
		LCD_KIT_ERR("read ktd3136 revision failed\n");
		goto error_exit;
	}
	if((chip_id & KTD3136_DEV_MASK) != KTD3136_CHIP_ID) {
		LCD_KIT_ERR("ktd3136 check vendor id failed\n");
		ret = LCD_KIT_FAIL;
		goto error_exit;
	}
	return LCD_KIT_OK;
error_exit:
	if (ktd3136_bl_info.ktd3136_hw_en)
		mt_set_gpio_out(ktd3136_bl_info.ktd3136_hw_en_gpio,
			GPIO_OUT_ZERO);

	return ret;
}

static int ktd3136_backlight_ic_check(void)
{
	int ret = LCD_KIT_OK;

	if (ktd3136_checked) {
		LCD_KIT_INFO("ktd3136 already check, not again setting\n");
		return ret;
	}
	ret = ktd3136_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("ktd3136 is not right backlight ic\n");
	} else {
		ktd3136_parse_dts();
		ret = ktd3136_config_register();
		if (ret < 0){
			LCD_KIT_ERR("ktd3136 config register failed\n");
			return ret;
		}
		check_status = CHECK_STATUS_OK;
		lcd_kit_bl_register(&bl_ops);
		LCD_KIT_ERR("ktd3136 is right backlight ic\n");
	}
	ktd3136_checked = true;

	return ret;
}

int ktd3136_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("ktd3136 enter\n");
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
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3136,
		KTD3136_SUPPORT, &ktd3136_bl_info.ktd3136_support, 0);
	if (ret < 0 || !ktd3136_bl_info.ktd3136_support) {
		LCD_KIT_ERR("not support ktd3136!\n");
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3136,
		KTD3136_I2C_BUS_ID, &ktd3136_bl_info.ktd3136_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts ktd3136_i2c_bus_id fail!\n");
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3136,
		KTD3136_HW_ENABLE, &ktd3136_bl_info.ktd3136_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts ktd3136_hw_enable fail!\n");
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3136,
		KTD3136_BL_LEVEL, &ktd3136_bl_info.bl_level,
		KTD3136_BL_DEFAULT_LEVEL);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts ktd3136_bl_level fail!\n");
		return LCD_KIT_FAIL;
	}
	if (ktd3136_bl_info.ktd3136_hw_en) {
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3136,
			KTD3136_HW_EN_GPIO,
			&ktd3136_bl_info.ktd3136_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts ktd3136_hw_en_gpio fail!\n");
			ktd3136_bl_info.ktd3136_hw_en_gpio = 0;
			return LCD_KIT_FAIL;
		}
		ret = lcd_kit_parse_get_u32_default(DTS_COMP_KTD3136,
			KTD3136_HW_EN_DELAY,
			&ktd3136_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts bl_on_lk_mdelay fail!\n");
			ktd3136_bl_info.bl_on_lk_mdelay = 0;
			return LCD_KIT_FAIL;
		}
	}
	ret = ktd3136_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = CHECK_STATUS_OK;
		LCD_KIT_INFO("ktd3136 is checked succ\n");
	}
	LCD_KIT_INFO("ktd3136 is support\n");

	return LCD_KIT_OK;
}
