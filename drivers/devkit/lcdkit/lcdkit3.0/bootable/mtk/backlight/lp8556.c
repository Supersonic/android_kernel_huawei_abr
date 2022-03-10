/*
 * lp8556.c
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
#include <platform/mt_gpt.h>
#include <lk_builtin_dtb.h>
#include "lcd_kit_utils.h"
#include "lp8556.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"

#define OFFSET_DEF_VAL (-1)

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

static struct lp8556_backlight_information lp8556_bl_info = {0};
static bool lp8556_init_status = false;
static bool lp8556_checked;
static bool check_status;

static char *lp8556_dts_string[LP8556_RW_REG_MAX] = {
	"lp8556_device_control",
	"lp8556_eprom_cfg0",
	"lp8556_eprom_cfg1",
	"lp8556_eprom_cfg2",
	"lp8556_eprom_cfg3",
	"lp8556_eprom_cfg4",
	"lp8556_eprom_cfg5",
	"lp8556_eprom_cfg6",
	"lp8556_eprom_cfg7",
	"lp8556_eprom_cfg9",
	"lp8556_eprom_cfgA",
	"lp8556_eprom_cfgE",
	"lp8556_eprom_cfg9E",
	"lp8556_led_enable",
	"lp8556_eprom_cfg98",
};

static u8 lp8556_reg_addr[LP8556_RW_REG_MAX] = {
	LP8556_DEVICE_CONTROL,
	LP8556_EPROM_CFG0,
	LP8556_EPROM_CFG1,
	LP8556_EPROM_CFG2,
	LP8556_EPROM_CFG3,
	LP8556_EPROM_CFG4,
	LP8556_EPROM_CFG5,
	LP8556_EPROM_CFG6,
	LP8556_EPROM_CFG7,
	LP8556_EPROM_CFG9,
	LP8556_EPROM_CFGA,
	LP8556_EPROM_CFGE,
	LP8556_EPROM_CFG9E,
	LP8556_LED_ENABLE,
	LP8556_EPROM_CFG98,
};

static int lp8556_2_i2c_read_u8(u8 chip_no, u8 *data_buffer, int addr)
{
	int ret = LP8556_FAIL;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t lp8556_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	lp8556_i2c.id = lp8556_bl_info.lp8556_2_i2c_bus_id;
	lp8556_i2c.addr = chip_no;
	lp8556_i2c.mode = ST_MODE;
	lp8556_i2c.speed = LP8556_I2C_SPEED;

	ret = i2c_write_read(&lp8556_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_read  failed! reg is 0x%x ret: %d\n",
			addr, ret);
	return ret;
}

static int lp8556_i2c_read_u8(u8 chip_no, u8 *data_buffer, int addr)
{
	int ret = LP8556_FAIL;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t lp8556_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	lp8556_i2c.id = lp8556_bl_info.lp8556_i2c_bus_id;
	lp8556_i2c.addr = chip_no;
	lp8556_i2c.mode = ST_MODE;
	lp8556_i2c.speed = LP8556_I2C_SPEED;

	ret = i2c_write_read(&lp8556_i2c, data_buffer, len, len);
	if (ret != 0) {
		LCD_KIT_ERR("i2c_read  failed! reg is 0x%x ret: %d\n",
			addr, ret);
		return ret;
	}
	if (lp8556_bl_info.dual_ic) {
		ret = lp8556_2_i2c_read_u8(chip_no, data_buffer, addr);
		if (ret < 0) {
			LCD_KIT_ERR("lp8556_2_i2c_read_u8  failed! reg is 0x%xret: %d\n",
				addr, ret);
			return ret;
		}
	}
	return ret;
}

static int lp8556_2_i2c_write_u8(u8 chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[LP8556_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = LP8556_WRITE_LEN;
	struct mt_i2c_t lp8556_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	lp8556_i2c.id = lp8556_bl_info.lp8556_2_i2c_bus_id;
	lp8556_i2c.addr = chip_no;
	lp8556_i2c.mode = ST_MODE;
	lp8556_i2c.speed = LP8556_I2C_SPEED;

	ret = i2c_write(&lp8556_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_write  failed! reg is  0x%x ret: %d\n",
			addr, ret);
	return ret;
}


static int lp8556_i2c_write_u8(u8 chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[LP8556_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = LP8556_WRITE_LEN;
	struct mt_i2c_t lp8556_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	lp8556_i2c.id = lp8556_bl_info.lp8556_i2c_bus_id;
	lp8556_i2c.addr = chip_no;
	lp8556_i2c.mode = ST_MODE;
	lp8556_i2c.speed = LP8556_I2C_SPEED;

	ret = i2c_write(&lp8556_i2c, write_data, len);
	if (ret != 0) {
		LCD_KIT_ERR("i2c_write  failed! reg is 0x%x ret: %d\n",
			addr, ret);
		return ret;
	}
	if (lp8556_bl_info.dual_ic) {
		ret = lp8556_2_i2c_write_u8(chip_no, addr, value);
		if(ret < 0){
			LCD_KIT_ERR("lp8556_2_i2c_write_u8  failed! reg is 0x%x ret: %d\n",
				addr, ret);
			return ret;
		}
	}
	return ret;
}

static int lp8556_parse_dts()
{
	int ret;
	int i;

	LCD_KIT_INFO("lp8556_parse_dts +!\n");

	for (i = 0;i < LP8556_RW_REG_MAX;i++ ) {
		ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
			lp8556_bl_info.nodeoffset, lp8556_dts_string[i],
			&lp8556_bl_info.lp8556_reg[i], 0);
		if (ret < 0) {
			lp8556_bl_info.lp8556_reg[i] = 0xffff;
			LCD_KIT_ERR("can not find %s dts\n", lp8556_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				lp8556_dts_string[i], lp8556_bl_info.lp8556_reg[i]);
		}
	}

	return ret;
}

static int lp8556_config_register(void)
{
	int ret = 0;
	int i;

	for(i = 0;i < LP8556_RW_REG_MAX;i++) {
		/*judge reg is valid*/
		if (lp8556_bl_info.lp8556_reg[i] != 0xffff) {
			ret = lp8556_i2c_write_u8(LP8556_SLAV_ADDR,
				(u8)lp8556_reg_addr[i], (u8)lp8556_bl_info.lp8556_reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write lp8556 backlight config register 0x%x failed\n",
					lp8556_reg_addr[i]);
				return ret;
			}
		}
	}
	return ret;
}

static void lp8556_enable(void)
{
	int ret;

	if (lp8556_bl_info.lp8556_hw_en) {
		mt_set_gpio_mode(lp8556_bl_info.lp8556_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(lp8556_bl_info.lp8556_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(lp8556_bl_info.lp8556_hw_en_gpio,
			GPIO_OUT_ONE);
		if (lp8556_bl_info.dual_ic) {
			mt_set_gpio_mode(lp8556_bl_info.lp8556_2_hw_en_gpio,
				GPIO_MODE_00);
			mt_set_gpio_dir(lp8556_bl_info.lp8556_2_hw_en_gpio,
				GPIO_DIR_OUT);
			mt_set_gpio_out(lp8556_bl_info.lp8556_2_hw_en_gpio,
				GPIO_OUT_ONE);
		}
		if (lp8556_bl_info.bl_on_lk_mdelay)
			mdelay(lp8556_bl_info.bl_on_lk_mdelay);
	}
	ret = lp8556_config_register();
	if (ret < 0) {
		LCD_KIT_ERR("lp8556 config register failed\n");
		return ;
	}
	lp8556_init_status = true;
}

static void lp8556_disable(void)
{
	if (lp8556_bl_info.lp8556_hw_en) {
		mt_set_gpio_out(lp8556_bl_info.lp8556_hw_en_gpio,
			GPIO_OUT_ZERO);
		if (lp8556_bl_info.dual_ic)
			mt_set_gpio_out(lp8556_bl_info.lp8556_2_hw_en_gpio,
				GPIO_OUT_ZERO);
	}
	lp8556_init_status = false;
}

int lp8556_set_backlight(uint32_t bl_level)
{
	int bl_msb = 0;
	int bl_lsb = 0;
	int ret = 0;

	/*first set backlight, enable lp8556*/
	if (false == lp8556_init_status && bl_level > 0)
	lp8556_enable();

	/*set backlight level*/
	bl_msb = (bl_level >> 8) & 0x0F;
	bl_lsb = bl_level & 0xFF;

	ret = lp8556_i2c_write_u8(LP8556_SLAV_ADDR, lp8556_bl_info.lp8556_level_lsb, bl_lsb);
	if (ret < 0)
		LCD_KIT_ERR("write lp8556 backlight level lsb:0x%x failed\n", bl_lsb);
	ret = lp8556_i2c_write_u8(LP8556_SLAV_ADDR, lp8556_bl_info.lp8556_level_msb, bl_msb);
	if (ret < 0)
		LCD_KIT_ERR("write lp8556 backlight level msb:0x%x failed\n", bl_msb);

	/*if set backlight level 0, disable lp8556*/
	if (true == lp8556_init_status && 0 == bl_level)
		lp8556_disable();

	return ret;
}
int lp8556_en_backlight(uint32_t bl_level)
{
	int ret = 0;

	LCD_KIT_INFO("lp8556_en_backlight: bl_level=%d\n", bl_level);
	/*first set backlight, enable lp8556*/
	if (false == lp8556_init_status && bl_level > 0)
		lp8556_enable();

	/*if set backlight level 0, disable lp8556*/
	if (true == lp8556_init_status && 0 == bl_level)
		lp8556_disable();

	return ret;
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = lp8556_set_backlight,
	.en_backlight = lp8556_en_backlight,
};

void lp8556_set_backlight_status (void)
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
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_LP8556);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find lp8556 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update lp8556 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("lp8556_set_backlight_status OK!\n");
}

static int lp8556_backlight_ic_check(void)
{
	int ret = 0;

	if (lp8556_checked) {
		LCD_KIT_INFO("lp8556 already check, not again setting\n");
		return ret;
	}
	lp8556_parse_dts();
	lp8556_enable();
	ret = lp8556_i2c_read_u8(LP8556_SLAV_ADDR,
		(u8 *)&lp8556_bl_info.lp8556_reg[0], lp8556_reg_addr[0]);
	if (ret < 0) {
		LCD_KIT_ERR("lp8556 not used\n");
		return ret;
	}
	lp8556_disable();
	lcd_kit_bl_register(&bl_ops);
	check_status = CHECK_STATUS_OK;
	LCD_KIT_INFO("lp8556 is right backlight ic\n");
	lp8556_checked = true;
	return ret;
}

int lp8556_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("lp8556 init\n");
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo->bias_bl_ic_checked != 0) {
		LCD_KIT_ERR("bias bl ic checked\n");
		return LCD_KIT_OK;
	}

	lp8556_bl_info.pfdt = get_lk_overlayed_dtb();
	if (lp8556_bl_info.pfdt == NULL) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	lp8556_bl_info.nodeoffset = fdt_node_offset_by_compatible(
		lp8556_bl_info.pfdt, OFFSET_DEF_VAL, DTS_COMP_LP8556);
	if (lp8556_bl_info.nodeoffset < 0) {
		LCD_KIT_INFO("can not find %s node\n", DTS_COMP_LP8556);
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
		lp8556_bl_info.nodeoffset, LP8556_SUPPORT,
		&lp8556_bl_info.lp8556_support, 0);
	if (ret < 0 || !lp8556_bl_info.lp8556_support) {
		LCD_KIT_ERR("get lp8556_support failed!\n");
		goto exit;
	}

	ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
		lp8556_bl_info.nodeoffset, LP8556_HW_DUAL_IC,
		&lp8556_bl_info.dual_ic, 0);
	if (ret < 0)
		LCD_KIT_ERR("parse dts dual_ic fail!\n");

	ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
		lp8556_bl_info.nodeoffset, LP8556_I2C_BUS_ID,
		&lp8556_bl_info.lp8556_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts lp8556_i2c_bus_id fail!\n");
		lp8556_bl_info.lp8556_i2c_bus_id = 0;
		goto exit;
	}
	if (lp8556_bl_info.dual_ic) {
		ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
			lp8556_bl_info.nodeoffset, LP8556_2_I2C_BUS_ID,
			&lp8556_bl_info.lp8556_2_i2c_bus_id, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts lp8556_2_i2c_bus_id fail!\n");
			lp8556_bl_info.lp8556_2_i2c_bus_id = 0;
			goto exit;
		}
	}
	ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
		lp8556_bl_info.nodeoffset, GPIO_LP8556_EN_NAME,
		&lp8556_bl_info.lp8556_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts lp8556_hw_enable fail!\n");
		lp8556_bl_info.lp8556_hw_en = 0;
		return LCD_KIT_FAIL;
	}

	if (lp8556_bl_info.lp8556_hw_en) {
		ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
			lp8556_bl_info.nodeoffset, LP8556_HW_EN_GPIO,
			&lp8556_bl_info.lp8556_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts lp8556_hw_en_gpio fail!\n");
			lp8556_bl_info.lp8556_hw_en_gpio = 0;
			goto exit;
		}
		if (lp8556_bl_info.dual_ic) {
			ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
				lp8556_bl_info.nodeoffset, LP8556_2_HW_EN_GPIO,
				&lp8556_bl_info.lp8556_2_hw_en_gpio, 0);
			if (ret < 0) {
				LCD_KIT_ERR("parse dts lp8556_2_hw_en_gpio fail!\n");
				lp8556_bl_info.lp8556_2_hw_en_gpio = 0;
				goto exit;
			}
		}
		ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
			lp8556_bl_info.nodeoffset, LP8556_HW_EN_DELAY,
			&lp8556_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0)
			LCD_KIT_INFO("parse dts bl_on_lk_mdelay fail!\n");
	}

	ret = lcd_kit_get_dts_u32_default(lp8556_bl_info.pfdt,
		lp8556_bl_info.nodeoffset, LP8556_BL_LEVEL,
		&lp8556_bl_info.bl_level, LP8556_BL_DEFAULT_LEVEL);
	if (ret < 0)
		LCD_KIT_ERR("parse dts lp8556_bl_level fail!\n");

	ret = lp8556_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = 1;
		LCD_KIT_INFO("lp8556 is checked\n");
	}
	LCD_KIT_INFO("[%s]:lp8556 is support\n", __FUNCTION__);

exit:
	return ret;
}

