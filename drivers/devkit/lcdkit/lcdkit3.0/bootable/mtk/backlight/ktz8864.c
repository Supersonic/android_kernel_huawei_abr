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
#include <platform/mt_gpt.h>
#include "lcd_kit_common.h"
#include "lcd_kit_bl.h"
#include "lcd_kit_utils.h"
#include "lcd_bl_linear_exponential_table.h"
#include "ktz8864.h"

#define OFFSET_DEF_VAL (-1)

static unsigned int check_status;
static bool ktz8864_checked;

static int ktz8864_set_backlight(uint32_t bl_level);

static struct backlight_information {
	/* whether support ktz8864 or not */
	int ktz8864_support;
	/* which i2c bus controller ktz8864 mount */
	int ktz8864_i2c_bus_id;
	/* ktz8864 hw_ldsen gpio */
	unsigned int ktz8864_hw_ldsen_gpio;
	/* ktz8864 hw_en gpio */
	unsigned int ktz8864_hw_en;
	unsigned int ktz8864_hw_en_gpio;
	unsigned int bl_on_lk_mdelay;
	unsigned int bl_level;
	u32 bl_only;
	int nodeoffset;
	void *pfdt;
	int ktz8864_reg[KTZ8864_RW_REG_MAX];
}bl_info;
static struct backlight_information bl_info = { 0 };

static char *ktz8864_dts_string[KTZ8864_RW_REG_MAX] = {
	"ktz8864_bl_config_1",
	"ktz8864_bl_config_2",
	"ktz8864_bl_brightness_lsb",
	"ktz8864_bl_brightness_msb",
	"ktz8864_auto_freq_low",
	"ktz8864_auto_freq_high",
	"ktz8864_display_bias_config_2",
	"ktz8864_display_bias_config_3",
	"ktz8864_lcm_boost_bias",
	"ktz8864_vpos_bias",
	"ktz8864_vneg_bias",
	"ktz8864_bl_option_1",
	"ktz8864_bl_option_2",
	"ktz8864_display_bias_config_1",
	"ktz8864_bl_en",
};

static int ktz8864_reg_addr[KTZ8864_RW_REG_MAX] = {
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
	REG_BL_ENABLE,
};

static struct ktz8864_voltage ktz8864_vol_table[] = {
	{ 4000000, KTZ8864_VOL_400 },
	{ 4050000, KTZ8864_VOL_405 },
	{ 4100000, KTZ8864_VOL_410 },
	{ 4150000, KTZ8864_VOL_415 },
	{ 4200000, KTZ8864_VOL_420 },
	{ 4250000, KTZ8864_VOL_425 },
	{ 4300000, KTZ8864_VOL_430 },
	{ 4350000, KTZ8864_VOL_435 },
	{ 4400000, KTZ8864_VOL_440 },
	{ 4450000, KTZ8864_VOL_445 },
	{ 4500000, KTZ8864_VOL_450 },
	{ 4550000, KTZ8864_VOL_455 },
	{ 4600000, KTZ8864_VOL_460 },
	{ 4650000, KTZ8864_VOL_465 },
	{ 4700000, KTZ8864_VOL_470 },
	{ 4750000, KTZ8864_VOL_475 },
	{ 4800000, KTZ8864_VOL_480 },
	{ 4850000, KTZ8864_VOL_485 },
	{ 4900000, KTZ8864_VOL_490 },
	{ 4950000, KTZ8864_VOL_495 },
	{ 5000000, KTZ8864_VOL_500 },
	{ 5050000, KTZ8864_VOL_505 },
	{ 5100000, KTZ8864_VOL_510 },
	{ 5150000, KTZ8864_VOL_515 },
	{ 5200000, KTZ8864_VOL_520 },
	{ 5250000, KTZ8864_VOL_525 },
	{ 5300000, KTZ8864_VOL_530 },
	{ 5350000, KTZ8864_VOL_535 },
	{ 5400000, KTZ8864_VOL_540 },
	{ 5450000, KTZ8864_VOL_545 },
	{ 5500000, KTZ8864_VOL_550 },
	{ 5550000, KTZ8864_VOL_555 },
	{ 5600000, KTZ8864_VOL_560 },
	{ 5650000, KTZ8864_VOL_565 },
	{ 5700000, KTZ8864_VOL_570 },
	{ 5750000, KTZ8864_VOL_575 },
	{ 5800000, KTZ8864_VOL_580 },
	{ 5850000, KTZ8864_VOL_585 },
	{ 5900000, KTZ8864_VOL_590 },
	{ 5950000, KTZ8864_VOL_595 },
	{ 6000000, KTZ8864_VOL_600 },
	{ 6050000, KTZ8864_VOL_605 },
	{ 6400000, KTZ8864_VOL_640 },
	{ 6450000, KTZ8864_VOL_645 },
	{ 6500000, KTZ8864_VOL_650 },
};

int ktz8864_set_backlight(uint32_t bl_level);

static int ktz8864_i2c_read_u8(u8 chip_no, u8 *data_buffer, int addr)
{
	int ret = FAIL;
	/* read date default length */
	unsigned char len = 1;
	struct mt_i2c_t ktz8864_i2c = {0};

	if (!data_buffer) {
		LCD_KIT_ERR("data buffer is NULL");
		return ret;
	}

	*data_buffer = addr;
	ktz8864_i2c.id = bl_info.ktz8864_i2c_bus_id;
	ktz8864_i2c.addr = chip_no;
	ktz8864_i2c.mode = ST_MODE;
	ktz8864_i2c.speed = KTZ8864_I2C_SPEED;

	ret = i2c_write_read(&ktz8864_i2c, data_buffer, len, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_read failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int ktz8864_i2c_write_u8(u8 chip_no, unsigned char addr, unsigned char value)
{
	int ret;
	unsigned char write_data[KTZ8864_WRITE_LEN] = {0};
	/* write date default length */
	unsigned char len = KTZ8864_WRITE_LEN;
	struct mt_i2c_t ktz8864_i2c = {0};

	/* data0: address, data1: write value */
	write_data[0] = addr;
	write_data[1] = value;

	ktz8864_i2c.id = bl_info.ktz8864_i2c_bus_id;
	ktz8864_i2c.addr = chip_no;
	ktz8864_i2c.mode = ST_MODE;
	ktz8864_i2c.speed = KTZ8864_I2C_SPEED;

	ret = i2c_write(&ktz8864_i2c, write_data, len);
	if (ret != 0)
		LCD_KIT_ERR("%s: i2c_write failed, reg is 0x%x ret: %d\n",
			__func__, addr, ret);

	return ret;
}

static int ktz8864_i2c_update_bits(u8 chip_no, int reg, u8 mask, u8 val)
{
	int ret;
	u8 tmp;
	u8 orig = 0;

	ret = ktz8864_i2c_read_u8(chip_no, &orig, reg);
	if (ret < 0)
		return ret;

	tmp = orig & ~mask;
	tmp |= val & mask;

	if (tmp != orig) {
		ret = ktz8864_i2c_write_u8(chip_no, reg, val);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static void ktz8864_get_target_voltage(int *vpos, int *vneg)
{
	int i;
	int tal_size = sizeof(ktz8864_vol_table) / sizeof(struct ktz8864_voltage);

	if ((vpos == NULL) || (vneg == NULL)) {
		LCD_KIT_ERR("vpos or vneg is null\n");
		return;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (ktz8864_vol_table[i].voltage == power_hdl->lcd_vsp.buf[2]) {
			*vpos = ktz8864_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsp voltage, use default voltage\n");
		*vpos = KTZ8864_VOL_600;
	}

	for (i = 0; i < tal_size; i++) {
		/* set bias voltage buf[2] means bias value */
		if (ktz8864_vol_table[i].voltage == power_hdl->lcd_vsn.buf[2]) {
			*vneg = ktz8864_vol_table[i].value;
			break;
		}
	}

	if (i >= tal_size) {
		LCD_KIT_INFO("not found vsn voltage, use default voltage\n");
		*vneg = KTZ8864_VOL_600;
	}

	LCD_KIT_INFO("*vpos_target=0x%x,*vneg_target=0x%x\n", *vpos, *vneg);

	return;
}

static int ktz8864_parse_dts(void)
{
	int ret;
	int i;
	int vpos_target = 0;
	int vneg_target = 0;

	LCD_KIT_INFO("ktz8864_parse_dts +!\n");
	for (i = 0; i < KTZ8864_RW_REG_MAX; i++) {
		ret = lcd_kit_get_dts_u32_default(bl_info.pfdt, bl_info.nodeoffset,
			ktz8864_dts_string[i],
			(u32 *)&bl_info.ktz8864_reg[i], 0);
		if (ret < 0) {
			bl_info.ktz8864_reg[i] = KTZ8864_INVALID_VAL;
			LCD_KIT_INFO("can not find %s dts\n", ktz8864_dts_string[i]);
		} else {
			LCD_KIT_INFO("get %s value = 0x%x\n",
				ktz8864_dts_string[i], bl_info.ktz8864_reg[i]);
		}
	}

	if(bl_info.bl_only == 0) {
		ktz8864_get_target_voltage(&vpos_target, &vneg_target);
		/* 9  is the position of vsp in ktz8864_reg */
		/* 10 is the position of vsn in ktz8864_reg */
		if (bl_info.ktz8864_reg[9] != vpos_target)
			bl_info.ktz8864_reg[9] = vpos_target;
		if (bl_info.ktz8864_reg[10] != vneg_target)
			bl_info.ktz8864_reg[10] = vneg_target;
	}

	return ret;
}

static int ktz8864_config_register(void)
{
	int ret;
	int i;

	for (i = 0; i < KTZ8864_RW_REG_MAX; i++) {
		ret = ktz8864_i2c_write_u8(KTZ8864_SLAV_ADDR,
				ktz8864_reg_addr[i], bl_info.ktz8864_reg[i]);
		LCD_KIT_INFO("ktz8864_config_register 0x%x value = 0x%x\n",
				ktz8864_reg_addr[i], bl_info.ktz8864_reg[i]);
		if (ret < 0) {
			LCD_KIT_ERR("write ktz backlight config register 0x%x failed",
					ktz8864_reg_addr[i]);
			return ret;
		}
	}
	return ret;
}

static struct lcd_kit_bl_ops bl_ops = {
	.set_backlight = ktz8864_set_backlight,
};

/*
 * ktz8864_set_backlight(): Set Backlight working mode
 *
 * @bl_level: value for backlight ,range from 0 to 2047
 *
 * A value of zero will be returned on success, a negative errno will
 * be returned in error cases.
 */
static int ktz8864_set_backlight(uint32_t bl_level)
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
		LCD_KIT_INFO("ktz8864 mipi pwm\n");
		return LCD_KIT_OK;
	}
	/* map bl_level from 10000 to 2048 stage brightness */
	level = bl_level * bl_info.bl_level / KTZ8864_BL_DEFAULT_LEVEL;

	if (level > KTZ8864_BL_MAX)
		level = KTZ8864_BL_MAX;

	/* 11-bit brightness code */
	bl_msb = level >> MSB;
	bl_lsb = level & LSB;

	ret = ktz8864_i2c_update_bits(KTZ8864_SLAV_ADDR, REG_BL_BRIGHTNESS_LSB,
			MASK_BL_LSB, bl_lsb);
	if (ret < 0)
		goto i2c_error;

	ret = ktz8864_i2c_write_u8(KTZ8864_SLAV_ADDR, REG_BL_BRIGHTNESS_MSB,
			bl_msb);
	if (ret < 0)
		goto i2c_error;

	LCD_KIT_INFO("write ktz8864 backlight %u success\n", bl_level);

	return ret;

i2c_error:
	LCD_KIT_ERR("%s:i2c access fail to register", __func__);
	return ret;
}

void ktz8864_set_backlight_status (void)
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
	offset = fdt_node_offset_by_compatible(kernel_fdt, 0, DTS_COMP_KTZ8864);
	if (offset < 0) {
		LCD_KIT_ERR("Could not find ktz8864 node, change dts failed\n");
		return;
	}

	if (check_status == CHECK_STATUS_OK)
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"okay");
	else
		ret = fdt_setprop_string(kernel_fdt, offset, (const char *)"status",
			"disabled");
	if (ret) {
		LCD_KIT_ERR("Cannot update ktz8864 status errno=%d\n", ret);
		return;
	}

	LCD_KIT_INFO("ktz8864_set_backlight_status OK!\n");
}

static int ktz8864_device_verify(void)
{
	int ret;
	unsigned char reg_val = 0;

	if (bl_info.ktz8864_hw_en) {
		mt_set_gpio_mode(bl_info.ktz8864_hw_en_gpio,
			GPIO_MODE_00);
		mt_set_gpio_dir(bl_info.ktz8864_hw_en_gpio,
			GPIO_DIR_OUT);
		mt_set_gpio_out(bl_info.ktz8864_hw_en_gpio,
			GPIO_OUT_ONE);
		if (bl_info.bl_on_lk_mdelay)
			mdelay(bl_info.bl_on_lk_mdelay);
	}

	/* Read IC revision */
	ret = ktz8864_i2c_read_u8(KTZ8864_SLAV_ADDR, &reg_val, REG_REVISION);
	if (ret < 0) {
		LCD_KIT_ERR("read ktz8864 revision failed\n");
		goto error_exit;
	}
	LCD_KIT_INFO("ktz8864 reg revision = 0x%x\n", reg_val);
	if ((reg_val & DEV_MASK) != VENDOR_ID_KTZ) {
		LCD_KIT_INFO("ktz8864 check vendor id failed\n");
		ret = FAIL;
		goto error_exit;
	}

	return LCD_KIT_OK;
error_exit:
	if (bl_info.ktz8864_hw_en)
		mt_set_gpio_out(bl_info.ktz8864_hw_en_gpio,
			GPIO_OUT_ZERO);
	return ret;
}

int ktz8864_backlight_ic_check(void)
{
	int ret = SUCC;

	if (ktz8864_checked) {
		LCD_KIT_INFO("ktz8864 already check, not again setting\n");
		return ret;
	}

	ret = ktz8864_device_verify();
	if (ret < 0) {
		check_status = CHECK_STATUS_FAIL;
		LCD_KIT_ERR("ktz8864 is not right backlight ic\n");
	} else {
		ktz8864_parse_dts();
		ret = ktz8864_config_register();
		if (ret < 0)
			LCD_KIT_ERR("ktz8864 config register failed\n");
		lcd_kit_bl_register(&bl_ops);
		check_status = CHECK_STATUS_OK;
		LCD_KIT_INFO("ktz8864 is right backlight ic\n");
	}
	ktz8864_checked = true;
	return ret;
}

int ktz8864_init(struct mtk_panel_info *pinfo)
{
	int ret;

	LCD_KIT_INFO("ktz8864 init\n");
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo->bias_bl_ic_checked != 0) {
		LCD_KIT_ERR("bias bl ic checked\n");
		return LCD_KIT_OK;
	}
	bl_info.pfdt = get_lk_overlayed_dtb();
	if (bl_info.pfdt == NULL) {
		LCD_KIT_ERR("pfdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	bl_info.nodeoffset = fdt_node_offset_by_compatible(bl_info.pfdt,
		OFFSET_DEF_VAL, DTS_COMP_KTZ8864);
	if (bl_info.nodeoffset < 0) {
		LCD_KIT_INFO("can not find %s node\n", DTS_COMP_KTZ8864);
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_get_dts_u32_default(bl_info.pfdt, bl_info.nodeoffset,
		KTZ8864_I2C_BUS_ID, (u32 *)&bl_info.ktz8864_i2c_bus_id, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts ktz8864_i2c_bus_id fail!\n");
		bl_info.ktz8864_i2c_bus_id = 0;
		return SUCC;
	}

	ret = lcd_kit_get_dts_u32_default(bl_info.pfdt, bl_info.nodeoffset,
		KTZ8864_EN_NAME, &bl_info.ktz8864_hw_en, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts ktz8864_hw_enable fail!\n");
		bl_info.ktz8864_hw_en = 0;
		return SUCC;
	}

	ret = lcd_kit_get_dts_u32_default(bl_info.pfdt, bl_info.nodeoffset,
		KTZ8864_ONLY_BACKLIGHT, &bl_info.bl_only, 0);
	if (ret < 0) {
		LCD_KIT_ERR("parse dts only_backlight fail\n");
		bl_info.bl_only = 0;
	}

	if (bl_info.ktz8864_hw_en) {
		ret = lcd_kit_get_dts_u32_default(bl_info.pfdt, bl_info.nodeoffset,
			KTZ8864_HW_EN_GPIO,
			&bl_info.ktz8864_hw_en_gpio, 0);
		if (ret < 0) {
			LCD_KIT_ERR("parse dts ktz8864_hw_en_gpio fail!\n");
			bl_info.ktz8864_hw_en_gpio= 0;
			return SUCC;
		}
		ret = lcd_kit_get_dts_u32_default(bl_info.pfdt, bl_info.nodeoffset,
			KTZ8864_HW_EN_DELAY,
			&bl_info.bl_on_lk_mdelay, 0);
		if (ret < 0)
			LCD_KIT_INFO("parse dts bl_on_lk_mdelay fail!\n");
	}

	ret = lcd_kit_get_dts_u32_default(bl_info.pfdt, bl_info.nodeoffset,
		KTZ8864_BL_LEVEL, &bl_info.bl_level, KTZ8864_BL_DEFAULT_LEVEL);
	if (ret < 0)
		LCD_KIT_ERR("parse dts ktz8864_bl_level fail!\n");

	ret = ktz8864_backlight_ic_check();
	if (ret == LCD_KIT_OK) {
		pinfo->bias_bl_ic_checked = 1;
		LCD_KIT_INFO("ktz8864 is checked\n");
	}
	LCD_KIT_INFO("ktz8864 is support\n");

	return SUCC;
}
