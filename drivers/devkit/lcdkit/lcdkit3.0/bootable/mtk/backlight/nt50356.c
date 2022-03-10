/*
 * nt50356.c
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
#include "lcd_kit_utils.h"
#include "nt50356.h"
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"
#include "lcd_bl_linear_exponential_table.h"

#define OFFSET_DEF_VAL (-1)

static unsigned int check_status;
static char *nt50356_dts_string[NT50356_RW_REG_MAX] = {
	"nt50356_bl_config_1",
	"nt50356_bl_config_2",
	"nt50356_bl_brightness_lsb",
	"nt50356_bl_brightness_msb",
	"nt50356_auto_freq_low",
	"nt50356_auto_freq_high",
	"nt50356_reg_config_e8",
	"nt50356_reg_config_e9",
	"nt50356_reg_config_a9",
	"nt50356_display_bias_config_2",
	"nt50356_display_bias_config_3",
	"nt50356_lcm_boost_bias",
	"nt50356_vpos_bias",
	"nt50356_vneg_bias",
	"nt50356_bl_option_1",
	"nt50356_bl_option_2",
	"nt50356_bl_current_config",
	"nt50356_display_bias_config_1",
	"nt50356_bl_en",
};

static int nt50356_reg_addr[NT50356_RW_REG_MAX] = {
	REG_BL_CONFIG_1,
	REG_BL_CONFIG_2,
	REG_BL_BRIGHTNESS_LSB,
	REG_BL_BRIGHTNESS_MSB,
	REG_AUTO_FREQ_LOW,
	REG_AUTO_FREQ_HIGH,
	NT50356_REG_CONFIG_E8,
	NT50356_REG_CONFIG_E9,
	NT50356_REG_CONFIG_A9,
	REG_DISPLAY_BIAS_CONFIG_2,
	REG_DISPLAY_BIAS_CONFIG_3,
	REG_LCM_BOOST_BIAS,
	REG_VPOS_BIAS,
	REG_VNEG_BIAS,
	REG_BL_OPTION_1,
	REG_BL_OPTION_2,
	REG_BL_CURRENT_CONFIG,
	REG_DISPLAY_BIAS_CONFIG_1,
	REG_BL_ENABLE,
};

static struct nt50356_voltage nt50356_vol_table[] = {
	{ 4000000, NT50356_VOL_400 },
	{ 4050000, NT50356_VOL_405 },
	{ 4100000, NT50356_VOL_410 },
	{ 4150000, NT50356_VOL_415 },
	{ 4200000, NT50356_VOL_420 },
	{ 4250000, NT50356_VOL_425 },
	{ 4300000, NT50356_VOL_430 },
	{ 4350000, NT50356_VOL_435 },
	{ 4400000, NT50356_VOL_440 },
	{ 4450000, NT50356_VOL_445 },
	{ 4500000, NT50356_VOL_450 },
	{ 4550000, NT50356_VOL_455 },
	{ 4600000, NT50356_VOL_460 },
	{ 4650000, NT50356_VOL_465 },
	{ 4700000, NT50356_VOL_470 },
	{ 4750000, NT50356_VOL_475 },
	{ 4800000, NT50356_VOL_480 },
	{ 4850000, NT50356_VOL_485 },
	{ 4900000, NT50356_VOL_490 },
	{ 4950000, NT50356_VOL_495 },
	{ 5000000, NT50356_VOL_500 },
	{ 5050000, NT50356_VOL_505 },
	{ 5100000, NT50356_VOL_510 },
	{ 5150000, NT50356_VOL_515 },
	{ 5200000, NT50356_VOL_520 },
	{ 5250000, NT50356_VOL_525 },
	{ 5300000, NT50356_VOL_530 },
	{ 5350000, NT50356_VOL_535 },
	{ 5400000, NT50356_VOL_540 },
	{ 5450000, NT50356_VOL_545 },
	{ 5500000, NT50356_VOL_550 },
	{ 5550000, NT50356_VOL_555 },
	{ 5600000, NT50356_VOL_560 },
	{ 5650000, NT50356_VOL_565 },
	{ 5700000, NT50356_VOL_570 },
	{ 5750000, NT50356_VOL_575 },
	{ 5800000, NT50356_VOL_580 },
	{ 5850000, NT50356_VOL_585 },
	{ 5900000, NT50356_VOL_590 },
	{ 5950000, NT50356_VOL_595 },
	{ 6000000, NT50356_VOL_600 },
	{ 6050000, NT50356_VOL_605 },
	{ 6400000, NT50356_VOL_640 },
	{ 6450000, NT50356_VOL_645 },
	{ 6500000, NT50356_VOL_650 },
	{ 6550000, NT50356_VOL_655 },
	{ 6600000, NT50356_VOL_660 },
	{ 6650000, NT50356_VOL_665 },
	{ 6700000, NT50356_VOL_670 },
	{ 6750000, NT50356_VOL_675 },
	{ 6800000, NT50356_VOL_680 },
	{ 6850000, NT50356_VOL_685 },
	{ 6900000, NT50356_VOL_690 },
	{ 6950000, NT50356_VOL_695 },
	{ 7000000, NT50356_VOL_700 },
};

static struct nt50356_backlight_information {
	/* whether support nt50356 or not */
	u32 nt50356_support;
	/* which i2c bus controller nt50356 mount */
	int nt50356_i2c_bus_id;
	/* nt50356 hw_ldsen gpio */
	unsigned int nt50356_hw_ldsen_gpio;
	/* nt50356 hw_en gpio */
	unsigned int nt50356_hw_en_gpio;
	unsigned int nt50356_hw_en;
	unsigned int bl_on_lk_mdelay;
	unsigned int bl_level;
	u32 bl_only;
	int nodeoffset;
	void *pfdt;
	int nt50356_reg[NT50356_RW_REG_MAX];
} nt50356_bl_info;

int nt50356_set_backlight(uint32_t bl_level);
static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = nt50356_set_backlight,
};

static int nt50356_i2c_read_u8(u8 chip_no, u8 *data_buffer, int addr)
{
	int ret = NT50356_FAIL;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t nt50356_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	nt50356_i2c.id = nt50356_bl_info.nt50356_i2c_bus_id;
	nt50356_i2c.addr = chip_no;
	nt50356_i2c.mode = ST_MODE;
	nt50356_i2c.speed = NT50356_I2C_SPEED;

	ret = i2c_write_read(&nt50356_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int nt50356_i2c_write_u8(u8 chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[NT50356_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = NT50356_WRITE_LEN;
	struct mt_i2c_t nt50356_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	nt50356_i2c.id = nt50356_bl_info.nt50356_i2c_bus_id;
	nt50356_i2c.addr = chip_no;
	nt50356_i2c.mode = ST_MODE;
	nt50356_i2c.speed = NT50356_I2C_SPEED;

	ret = i2c_write(&nt50356_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int nt50356_i2c_update_bits(u8 chip_no, int reg, u8 mask, u8 val)
{
	int ret;
	unsigned char tmp;
	unsigned char orig = 0;

	ret = nt50356_i2c_read_u8(chip_no, &orig, reg);
	if (ret < 0)
		return ret;
	tmp = orig & ~mask;
	tmp |= val & mask;
	if (tmp != orig) {
		ret = nt50356_i2c_write_u8(chip_no, reg, val);
		if (ret < 0)
			return ret;
	}
	return ret;
}

static void nt50356_get_target_voltage(int *vpos, int *vneg)
{
	int i;
	int tal_size = sizeof(nt50356_vol_table) / sizeof(struct nt50356_voltage);

	if ((vpos == NULL) || (vneg == NULL)) {
		LCD_KIT_ERR("vpos or vneg is null\n");
		return;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (nt50356_vol_table[i].voltage == power_hdl->lcd_vsp.buf[2]) {
			*vpos = nt50356_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsp voltage, use default voltage\n");
		*vpos = NT50356_VOL_600;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (nt50356_vol_table[i].voltage == power_hdl->lcd_vsn.buf[2]) {
			*vneg = nt50356_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsn voltage, use default voltage\n");
		*vneg = NT50356_VOL_600;
	}

	LCD_KIT_INFO("*vpos_target=0x%x,*vneg_target=0x%x\n", *vpos, *vneg);

	return;
}


static int nt50356_parse_dts(void)
{
	int ret;
	int i;
	int vpos_target = 0;
	int vneg_target = 0;

	for (i = 0; i < NT50356_RW_REG_MAX; i++) {
		ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
			nt50356_bl_info.nodeoffset, nt50356_dts_string[i],
			(u32 *)&nt50356_bl_info.nt50356_reg[i], 0);
		if (ret < 0)
			LCD_KIT_INFO("get nt50356 dts config failed\n");
		else
			LCD_KIT_INFO("get %s value = 0x%x\n",
				nt50356_dts_string[i], nt50356_bl_info.nt50356_reg[i]);
	}
	if (nt50356_bl_info.bl_only == 0) {
		nt50356_get_target_voltage(&vpos_target, &vneg_target);
		/* 12 is the position of vsp in nt50356_reg */
		/* 13 is the position of vsn in nt50356_reg */
		if (nt50356_bl_info.nt50356_reg[12] != vpos_target)
			nt50356_bl_info.nt50356_reg[12] = vpos_target;
		if (nt50356_bl_info.nt50356_reg[13] != vneg_target)
			nt50356_bl_info.nt50356_reg[13] = vneg_target;
	}

	return ret;
}

static int nt50356_config_register(void)
{
	int ret;
	int i;

	for (i = 0; i < NT50356_RW_REG_MAX; i++) {
		ret = nt50356_i2c_write_u8(NT50356_SLAV_ADDR,
			nt50356_reg_addr[i], nt50356_bl_info.nt50356_reg[i]);
		if (ret < 0) {
			LCD_KIT_ERR("write ktz bl config reg 0x%x failed",
				nt50356_reg_addr[i]);
			return ret;
		}
	}
	return ret;
}

void nt50356_set_backlight_status (void)
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
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_NT50356);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find nt50356 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update nt50356 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("nt50356_set_backlight_status OK!\n");
}

static int nt50356_device_verify(void)
{
	int ret;
	u32 is_support = 0;
	unsigned char reg_val = 0;

	if (nt50356_bl_info.nt50356_hw_en) {
		mt_set_gpio_mode(nt50356_bl_info.nt50356_hw_en,
			GPIO_MODE_00);
		mt_set_gpio_dir(nt50356_bl_info.nt50356_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(nt50356_bl_info.nt50356_hw_en_gpio,
			GPIO_OUT_ONE);
		if (nt50356_bl_info.bl_on_lk_mdelay)
			mdelay(nt50356_bl_info.bl_on_lk_mdelay);
	}

	/* Read IC revision */
	ret = nt50356_i2c_read_u8(NT50356_SLAV_ADDR, &reg_val, REG_REVISION);
	if (ret < 0) {
		LCD_KIT_ERR("read nt50356 revision failed\n");
		goto error_exit;
	}
	LCD_KIT_INFO("nt50356 reg revision = 0x%x\n", reg_val);
	if ((reg_val & DEV_MASK) != VENDOR_ID_NT50356) {
		LCD_KIT_INFO("nt50356 check vendor id failed\n");
		ret = NT50356_FAIL;
		goto error_exit;
	}

	ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
		nt50356_bl_info.nodeoffset, NT50356_EXTEND_REV_SUPPORT,
		&is_support, 0);
	if (ret < 0) {
		LCD_KIT_INFO("no nt50356 extend rev compatible \n");
	} else {
		LCD_KIT_INFO("get nt50356 extend rev %d, support\n", is_support);
		if (is_support == NT50356_EXTEND_REV_VAL) {
			/* Read BL_BLCONFIG1, to fix TI and NT same revision problem */
			ret = nt50356_i2c_read_u8(NT50356_SLAV_ADDR, &reg_val, REG_BL_CONFIG_1);
			if (ret < 0) {
				LCD_KIT_ERR("read nt50356 bl config 1 failed\n");
				goto error_exit;
			}
			LCD_KIT_INFO("nt50356 reg bl config1 = 0x%x\n", reg_val);
			if ((reg_val & DEV_MASK_EXTEND_NT50356) != VENDOR_ID_EXTEND_NT50356) {
				LCD_KIT_INFO("nt50356 check vendor id exend failed\n");
				ret = NT50356_FAIL;
				goto error_exit;
			}
		}
	}

	return LCD_KIT_OK;
error_exit:
	if (nt50356_bl_info.nt50356_hw_en)
		mt_set_gpio_out(nt50356_bl_info.nt50356_hw_en_gpio,
			GPIO_OUT_ZERO);
	return ret;
}

static int nt50356_backlight_ic_check(void)
{
	int ret;

	ret = nt50356_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("nt50356 is not right backlight ic\n");
	} else {
		nt50356_parse_dts();
		ret = nt50356_config_register();
		if (ret < 0)
			LCD_KIT_ERR("nt50356 config register failed\n");
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("nt50356 is right backlight ic\n");
	}
	return ret;
}

int nt50356_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("nt50356 init\n");
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo->bias_bl_ic_checked != 0) {
		LCD_KIT_ERR("bias bl ic checked\n");
		return LCD_KIT_OK;
	}

	memset(&nt50356_bl_info, 0, sizeof(struct nt50356_backlight_information));
	nt50356_bl_info.pfdt = get_lk_overlayed_dtb();
	if (nt50356_bl_info.pfdt == NULL) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	nt50356_bl_info.nodeoffset = fdt_node_offset_by_compatible(
		nt50356_bl_info.pfdt, OFFSET_DEF_VAL, DTS_COMP_NT50356);
	if (nt50356_bl_info.nodeoffset < 0) {
		LCD_KIT_INFO("can not find %s node\n", DTS_COMP_NT50356);
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
		nt50356_bl_info.nodeoffset, NT50356_SUPPORT,
		&nt50356_bl_info.nt50356_support, 0);
	if (ret < 0 || !nt50356_bl_info.nt50356_support) {
		LCD_KIT_ERR("get nt50356_support failed!\n");
		goto exit;
	}

	ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
		nt50356_bl_info.nodeoffset, NT50356_I2C_BUS_ID,
		(u32 *)&nt50356_bl_info.nt50356_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts nt50356_i2c_bus_id fail!\n");
		nt50356_bl_info.nt50356_i2c_bus_id = 0;
		goto exit;
	}

	ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
		nt50356_bl_info.nodeoffset, GPIO_NT50356_EN_NAME,
		&nt50356_bl_info.nt50356_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts nt50356_hw_enable fail!\n");
		nt50356_bl_info.nt50356_hw_en = 0;
		return LCD_KIT_FAIL;
	}

	ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
		nt50356_bl_info.nodeoffset, NT50356_ONLY_BACKLIGHT,
		&nt50356_bl_info.bl_only, 0);
	if (ret < 0) {
		LCD_KIT_ERR(" parse dts only_backlight fail\n");
		nt50356_bl_info.bl_only = 0;
	}

	if (nt50356_bl_info.nt50356_hw_en) {
		ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
			nt50356_bl_info.nodeoffset, NT50356_HW_EN_GPIO,
			&nt50356_bl_info.nt50356_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts nt50356_hw_en_gpio fail!\n");
			nt50356_bl_info.nt50356_hw_en_gpio = 0;
			goto exit;
		}
		ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
			nt50356_bl_info.nodeoffset, NT50356_HW_EN_DELAY,
			&nt50356_bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0)
			LCD_KIT_INFO("parse dts bl_on_lk_mdelay fail!\n");
	}

	ret = lcd_kit_get_dts_u32_default(nt50356_bl_info.pfdt,
		nt50356_bl_info.nodeoffset, NT50356_BL_LEVEL,
		&nt50356_bl_info.bl_level, NT50356_BL_DEFAULT_LEVEL);
	if (ret < 0)
		LCD_KIT_ERR("parse dts nt50356_bl_level fail!\n");

	ret = nt50356_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = 1;
		LCD_KIT_INFO("nt50356 is checked\n");
	}
	LCD_KIT_INFO("[%s]:nt50356 is support\n", __FUNCTION__);

exit:
	return ret;
}
/*
 * nt50356_set_backlight_reg(): Set Backlight working mode
 *
 * @bl_level: value for backlight ,range from 0 to 2047
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
int nt50356_set_backlight(uint32_t bl_level)
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
		LCD_KIT_INFO("nt50356 mipi pwm\n");
		return LCD_KIT_OK;
	}
	/* map bl_level from 10000 to 2048 stage brightness */
	level = bl_level * nt50356_bl_info.bl_level / NT50356_BL_DEFAULT_LEVEL;

	if (level > NT50356_BL_MAX)
		level = NT50356_BL_MAX;

	/* 11-bit brightness code */
	bl_msb = level >> MSB;
	bl_lsb = level & LSB;

	LCD_KIT_INFO("level = %d, bl_msb = %d, bl_lsb = %d", level, bl_msb,
		bl_lsb);

	ret = nt50356_i2c_update_bits(NT50356_SLAV_ADDR, REG_BL_BRIGHTNESS_LSB,
		MASK_BL_LSB, bl_lsb);
	if (ret < 0) {
		goto i2c_error;
	}

	ret = nt50356_i2c_write_u8(NT50356_SLAV_ADDR, REG_BL_BRIGHTNESS_MSB,
		bl_msb);
	if (ret < 0) {
		goto i2c_error;
	}

	return ret;

i2c_error:
	LCD_KIT_ERR("i2c access fail to register");
	return ret;

}
