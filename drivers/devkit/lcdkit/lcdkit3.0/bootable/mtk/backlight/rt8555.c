/*
 * rt8555.c
 *
 * rt8555 driver for backlight
 *
 * Copyright (c) 2019 Huawei Technologies Co., Ltd.
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
#include "lcd_kit_utils.h"
#include "rt8555.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"

#define OFFSET_DEF_VAL (-1)

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1
static bool rt8555_checked;
static bool g_init_status;
static bool check_status;
static struct rt8555_backlight_info g_bl_info;

static char *g_dts_string[RT8555_RW_REG_MAX] = {
	"rt8555_control_mode",       /* register 0x00 */
	"rt8555_current_protection", /* register 0x01 */
	"rt8555_current_setting",    /* register 0x02 */
	"rt8555_voltage_setting",    /* register 0x03 */
	"rt8555_brightness_setting", /* register 0x08 */
	"rt8555_time_control",       /* register 0x09 */
	"rt8555_mode_devision",      /* register 0x0A */
	"rt8555_compensation_duty",  /* register 0x0B */
	"rt8555_clk_pfm_enable",     /* register 0x0D */
	"rt8555_led_protection",     /* register 0x0E */
	"rt8555_reg_config_50",      /* register 0x50 */
};
static unsigned int g_reg_addr[RT8555_RW_REG_MAX] = {
	RT8555_CONTROL_MODE_ADDR,
	RT8555_CURRENT_PROTECTION_ADDR,
	RT8555_CURRENT_SETTING_ADDR,
	RT8555_VOLTAGE_SETTING_ADDR,
	RT8555_BRIGHTNESS_SETTING_ADDR,
	RT8555_TIME_CONTROL_ADDR,
	RT8555_MODE_DEVISION_ADDR,
	RT8555_COMPENSATION_DUTY_ADDR,
	RT8555_CLK_PFM_ENABLE_ADDR,
	RT8555_LED_PROTECTION_ADDR,
	RT8555_REG_CONFIG_50,
};

static int rt8555_2_i2c_read_u8(u8 chip_no, u8 *data_buffer, int addr)
{
	int ret = RT8555_FAIL;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t rt8555_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	rt8555_i2c.id = g_bl_info.rt8555_2_i2c_bus_id;
	rt8555_i2c.addr = chip_no;
	rt8555_i2c.mode = ST_MODE;
	rt8555_i2c.speed = RT8555_I2C_SPEED;

	ret = i2c_write_read(&rt8555_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_read  failed! reg is 0x%x ret: %d\n",
			addr, ret);
	return ret;
}

static int rt8555_i2c_read_u8(u8 chip_no, u8 *data_buffer, int addr)
{
	int ret = RT8555_FAIL;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t rt8555_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	rt8555_i2c.id = g_bl_info.rt8555_i2c_bus_id;
	rt8555_i2c.addr = chip_no;
	rt8555_i2c.mode = ST_MODE;
	rt8555_i2c.speed = RT8555_I2C_SPEED;

	ret = i2c_write_read(&rt8555_i2c, data_buffer, len, len);
	if (ret != 0) {
		LCD_KIT_ERR("i2c_read  failed! reg is 0x%x ret: %d\n",
			addr, ret);
		return ret;
	}
	if (g_bl_info.dual_ic) {
		ret = rt8555_2_i2c_read_u8(chip_no, data_buffer, addr);
		if (ret < 0) {
			LCD_KIT_ERR("rt8555_2_i2c_read_u8  failed! reg is 0x%xret: %d\n",
				addr, ret);
			return ret;
		}
	}
	return ret;
}

static int rt8555_2_i2c_write_u8(u8 chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[RT8555_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = RT8555_WRITE_LEN;
	struct mt_i2c_t rt8555_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	rt8555_i2c.id = g_bl_info.rt8555_2_i2c_bus_id;
	rt8555_i2c.addr = chip_no;
	rt8555_i2c.mode = ST_MODE;
	rt8555_i2c.speed = RT8555_I2C_SPEED;

	ret = i2c_write(&rt8555_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("i2c_write  failed! reg is  0x%x ret: %d\n",
			addr, ret);
	return ret;
}

static int rt8555_i2c_write_u8(u8 chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[RT8555_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = RT8555_WRITE_LEN;
	struct mt_i2c_t rt8555_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	rt8555_i2c.id = g_bl_info.rt8555_i2c_bus_id;
	rt8555_i2c.addr = chip_no;
	rt8555_i2c.mode = ST_MODE;
	rt8555_i2c.speed = RT8555_I2C_SPEED;

	ret = i2c_write(&rt8555_i2c, write_data, len);
	if (ret != 0) {
		LCD_KIT_ERR("i2c_write  failed! reg is  0x%x ret: %d\n",
			addr, ret);
		return ret;
	}
	if (g_bl_info.dual_ic) {
		ret = rt8555_2_i2c_write_u8(chip_no, addr, value);
		if(ret < 0){
			LCD_KIT_ERR("rt8555_2_i2c_write_u8  failed! reg is  0x%x ret: %d\n",
				addr, ret);
			return ret;
		}
	}
	return ret;
}

static int rt8555_parse_dts(void)
{
	int ret;
	int i;

	LCD_KIT_INFO("rt8555_parse_dts +!\n");

	for (i = 0;i < RT8555_RW_REG_MAX;i++ ) {
		ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
			g_bl_info.nodeoffset, g_dts_string[i],
			&g_bl_info.reg[i], 0);
		if (ret < 0) {
			g_bl_info.reg[i] = 0xffff;
			LCD_KIT_ERR("can not find %s dts\n", g_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				g_dts_string[i],g_bl_info.reg[i]);
		}
	}
	return ret;
}

static int rt8555_config_register(void)
{
	int i;
	int ret = 0;

	for (i = 0; i < RT8555_RW_REG_MAX; i++) {
		if (g_bl_info.reg[i] != PARSE_FAILED) {
			ret = rt8555_i2c_write_u8(RT8555_SLAV_ADDR,
				g_reg_addr[i], (u8)g_bl_info.reg[i]);
			if (ret < 0) {
				LCD_KIT_ERR("write reg 0x%x failed\n",
					g_reg_addr[i]);
				goto exit;
			}
		}
	}
exit:
	return ret;
}

static void rt8555_enable(void)
{
	int ret;

	if (g_bl_info.rt8555_hw_en) {
		mt_set_gpio_mode(g_bl_info.rt8555_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(g_bl_info.rt8555_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(g_bl_info.rt8555_hw_en_gpio,
			GPIO_OUT_ONE);
		if (g_bl_info.dual_ic) {
			mt_set_gpio_mode(g_bl_info.rt8555_2_hw_en_gpio,
				GPIO_MODE_00);
			mt_set_gpio_dir(g_bl_info.rt8555_2_hw_en_gpio,
				GPIO_DIR_OUT);
			mt_set_gpio_out(g_bl_info.rt8555_2_hw_en_gpio,
				GPIO_OUT_ONE);
		}
		if (g_bl_info.bl_on_lk_mdelay)
			mdelay(g_bl_info.bl_on_lk_mdelay);
	}
	ret = rt8555_config_register();
	if (ret < 0) {
		LCD_KIT_ERR("rt8555 config register failed\n");
		return ;
	}
	g_init_status = true;
}

static void rt8555_disable(void)
{
	if (g_bl_info.rt8555_hw_en) {
		mt_set_gpio_out(g_bl_info.rt8555_hw_en_gpio,
			GPIO_OUT_ZERO);
		if (g_bl_info.dual_ic)
			mt_set_gpio_out(g_bl_info.rt8555_2_hw_en_gpio,
				GPIO_OUT_ZERO);
	}
	g_init_status = false;
}

static int rt8555_set_backlight(uint32_t bl_level)
{
	int ret;
	int bl_msb;
	int bl_lsb;

	/* first set backlight, enable rt8555 */
	if ((g_init_status == false) && (bl_level > 0))
	rt8555_enable();
	/*
	 * This is a rt8555 IC bug, when bl level is 0 or 0x0FF,
	 * bl level must add 1, or flickering
	 */
	if ((bl_level != 0) && ((bl_level & 0xF) == 0))
		bl_level += 1;
	if ((bl_level & 0xFF) == 0xFF)
		bl_level -= 1;
	/* set backlight level rt8555 backlight level 10bits */
	bl_msb = (bl_level >> 8) & 0x03; /* 2 msb bits */
	bl_lsb = bl_level & 0xFF; /* 8 lsb bits */

	ret = rt8555_i2c_write_u8(RT8555_SLAV_ADDR,
		g_bl_info.rt8555_level_lsb, bl_lsb);
	if (ret < 0)
		LCD_KIT_ERR("write bl level lsb:0x%x failed\n", bl_lsb);
	ret = rt8555_i2c_write_u8(RT8555_SLAV_ADDR,
		g_bl_info.rt8555_level_msb, bl_msb);
	if (ret < 0)
		LCD_KIT_ERR("write bl level msb:0x%x failed\n", bl_msb);
	/* if set backlight level 0, disable rt8555 */
	if ((g_init_status == true) && (bl_level == 0))
		rt8555_disable();

	return ret;
}

static int rt8555_en_backlight(uint32_t bl_level)
{
	int ret = 0;

	LCD_KIT_INFO("rt8555_en_backlight: bl_level=%d\n", bl_level);
	/* first set backlight, enable rt8555 */
	if ((g_init_status == false) && (bl_level > 0))
		rt8555_enable();

	/* if set backlight level 0, disable rt8555 */
	if ((g_init_status == true) && (bl_level == 0))
		rt8555_disable();
	return ret;
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = rt8555_set_backlight,
	.en_backlight = rt8555_en_backlight,
};

void rt8555_set_backlight_status (void)
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
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_RT8555);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find rt8555 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update rt8555 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("rt8555_set_backlight_status OK!\n");
}

static int rt8555_backlight_ic_check(void)
{
	int ret = 0;

	if (rt8555_checked) {
		LCD_KIT_INFO("rt8555 already check, not again setting\n");
		return ret;
	}
	rt8555_parse_dts();
	rt8555_enable();
	/* Only testing rt8555 used */
	ret = rt8555_i2c_read_u8(RT8555_SLAV_ADDR,
		(u8 *)&g_bl_info.reg[0], g_reg_addr[0]);
	if (ret < 0) {
		LCD_KIT_ERR("rt8555 not used\n");
		return ret;
	}
	rt8555_disable();
	lcd_kit_bl_register(&bl_ops);
	check_status = CHECK_STATUS_OK;
	LCD_KIT_INFO("rt8555 is right backlight ic\n");
	rt8555_checked = true;
	return ret;
}

int rt8555_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("rt8555_init \n");
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo->bias_bl_ic_checked != 0) {
		LCD_KIT_INFO("bias bl ic checked\n");
		return LCD_KIT_OK;
	}

	g_bl_info.pfdt = get_lk_overlayed_dtb();
	if (g_bl_info.pfdt == NULL) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	g_bl_info.nodeoffset = fdt_node_offset_by_compatible(
		g_bl_info.pfdt, OFFSET_DEF_VAL, DTS_COMP_RT8555);
	if (g_bl_info.nodeoffset < 0) {
		LCD_KIT_ERR("can not find %s node\n", DTS_COMP_RT8555);
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
		g_bl_info.nodeoffset, RT8555_SUPPORT,
		&g_bl_info.rt8555_support, 0);
	if (ret < 0 || !g_bl_info.rt8555_support) {
		LCD_KIT_ERR("get rt8555_support failed!\n");
		goto exit;
	}

	ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
		g_bl_info.nodeoffset, RT8555_I2C_BUS_ID,
		(u32 *)&g_bl_info.rt8555_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts rt8555_i2c_bus_id fail!\n");
		g_bl_info.rt8555_i2c_bus_id = 0;
		goto exit;
	}

	ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
		g_bl_info.nodeoffset, RT8555_HW_DUAL_IC,
		&g_bl_info.dual_ic, 0);
	if (ret < 0)
		LCD_KIT_INFO("parse dts dual_ic fail!\n");

	if (g_bl_info.dual_ic) {
		ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
			g_bl_info.nodeoffset, RT8555_2_I2C_BUS_ID,
			(u32 *)&g_bl_info.rt8555_2_i2c_bus_id, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts rt8555_2_i2c_bus_id fail!\n");
			g_bl_info.rt8555_2_i2c_bus_id = 0;
			goto exit;
		}
	}
	ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
		g_bl_info.nodeoffset, GPIO_RT8555_EN_NAME,
		&g_bl_info.rt8555_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts rt8555_hw_enable fail!\n");
		g_bl_info.rt8555_hw_en = 0;
		return LCD_KIT_FAIL;
	}

	if (g_bl_info.rt8555_hw_en) {
		ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
			g_bl_info.nodeoffset, RT8555_HW_EN_GPIO,
			&g_bl_info.rt8555_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts rt8555_hw_en_gpio fail!\n");
			g_bl_info.rt8555_hw_en_gpio = 0;
			goto exit;
		}
		if (g_bl_info.dual_ic) {
			ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
				g_bl_info.nodeoffset, RT8555_2_HW_EN_GPIO,
				&g_bl_info.rt8555_2_hw_en_gpio, 0);
			if (ret < 0) {
				LCD_KIT_ERR("parse dts rt8555_2_hw_en_gpio fail!\n");
				g_bl_info.rt8555_2_hw_en_gpio = 0;
				goto exit;
			}
		}
		ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
			g_bl_info.nodeoffset, RT8555_HW_EN_DELAY,
			&g_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0)
			LCD_KIT_INFO("parse dts bl_on_lk_mdelay fail!\n");
	}

	ret = lcd_kit_get_dts_u32_default(g_bl_info.pfdt,
		g_bl_info.nodeoffset, RT8555_BL_LEVEL,
		&g_bl_info.bl_level, RT8555_BL_DEFAULT_LEVEL);
	if (ret < 0)
		LCD_KIT_INFO("parse dts rt8555_bl_level fail!\n");

	ret = rt8555_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = 1;
		LCD_KIT_INFO("rt8555 is checked\n");
	}
	LCD_KIT_INFO("[%s]:rt8555 is support\n", __FUNCTION__);

exit:
	return ret;
}

