/*
 * mtk_6730_poweric.c
 *
 * mtk 6730 power ic driver
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

#include "lcm_pmic.h"
#include "lcd_kit_disp.h"
#include "lcd_kit_panel.h"
#include "lcd_kit_bias.h"
#include "lcd_kit_power.h"

extern LCM_DRIVER lcdkit_mtk_common_panel;
#if  defined(MTK_RT5081_PMU_CHARGER_SUPPORT) || defined(MTK_MT6370_PMU)
#define MT6731_VOL_40	0x00 /* 4.0V */
#define MT6731_VOL_41	0x02 /* 4.1V */
#define MT6731_VOL_42	0x04 /* 4.2V */
#define MT6731_VOL_43	0x06 /* 4.3V */
#define MT6731_VOL_44	0x08 /* 4.4V */
#define MT6731_VOL_45	0x0A /* 4.5V */
#define MT6731_VOL_46	0x0C /* 4.6V */
#define MT6731_VOL_47	0x0E /* 4.7V */
#define MT6731_VOL_48	0x10 /* 4.8V */
#define MT6731_VOL_49	0x12 /* 4.9V */
#define MT6731_VOL_50	0x14 /* 5.0V */
#define MT6731_VOL_51	0x16 /* 5.1V */
#define MT6731_VOL_52	0x18 /* 5.2V */
#define MT6731_VOL_53	0x1A /* 5.3V */
#define MT6731_VOL_54	0x1C /* 5.4V */
#define MT6731_VOL_55	0x1E /* 5.5V */
#define MT6731_VOL_56	0x20 /* 5.6V */
#define MT6731_VOL_57	0x22 /* 5.7V */
#define MT6731_VOL_58	0x24 /* 5.8V */
#define MT6731_VOL_59	0x26 /* 5.9V */
#define MT6731_VOL_60	0x28 /* 6.0V */
#define MT6731_VOL_61	0x2A /* 6.1V */
#define MT6731_VOL_615	0x2B /* 6.15V */
#define MT6731_VOL_62	0x2C /* 6.2V */

#define DLEAY_TIME_0MS	0x01
#define DLEAY_TIME_1MS	0x02
#define DLEAY_TIME_4MS	0x03

#define VBOOST_REG_ADDR	0xB2
#define VPOS_REG_ADDR	0xB3
#define VNEG_REG_ADDR	0xB4
#define ENABLE_REG_ADDR	0xB1
#define DISABLE_REG_ADDR	0xB1

#define VPOS_ENABLE_VAL		0x40
#define VNEG_ENABLE_VAL		0x08
#define VPOS_ENABLE_MASK	0x40
#define VNEG_ENABLE_MASK	0x08
#define VPOS_DISABLE_VAL	0x00
#define VNEG_DISABLE_VAL	0x00

#define MAX_VBOOST_CAL	6150000
#define BASE_VBOOST_CAL	200000

struct mtk_mt6731_voltage {
	unsigned int voltage;
	unsigned int value;
};

static struct mtk_mt6731_voltage vol_table[] = {
	{ 4000000, MT6731_VOL_40 },
	{ 4100000, MT6731_VOL_41 },
	{ 4200000, MT6731_VOL_42 },
	{ 4300000, MT6731_VOL_43 },
	{ 4400000, MT6731_VOL_44 },
	{ 4500000, MT6731_VOL_45 },
	{ 4600000, MT6731_VOL_46 },
	{ 4700000, MT6731_VOL_47 },
	{ 4800000, MT6731_VOL_48 },
	{ 4900000, MT6731_VOL_49 },
	{ 5000000, MT6731_VOL_50 },
	{ 5100000, MT6731_VOL_51 },
	{ 5200000, MT6731_VOL_52 },
	{ 5300000, MT6731_VOL_53 },
	{ 5400000, MT6731_VOL_54 },
	{ 5500000, MT6731_VOL_55 },
	{ 5600000, MT6731_VOL_56 },
	{ 5700000, MT6731_VOL_57 },
	{ 5800000, MT6731_VOL_58 },
	{ 5900000, MT6731_VOL_59 },
	{ 6000000, MT6731_VOL_60 },
	{ 6100000, MT6731_VOL_61 },
	{ 6150000, MT6731_VOL_615 },
	{ 6200000, MT6731_VOL_62 },
};

void lcd_kit_get_bias_boost_voltage(unsigned int *vboost_val)
{
	int i;
	int vol_size = ARRAY_SIZE(vol_table);

	*vboost_val += BASE_VBOOST_CAL;
	if (*vboost_val > MAX_VBOOST_CAL)
		*vboost_val = MAX_VBOOST_CAL;
	for (i = 0; i < vol_size; i++) {
		if (vol_table[i].voltage == *vboost_val) {
			*vboost_val = vol_table[i].value;
			break;
		}
	}
	if (i >= vol_size)
		*vboost_val = MT6731_VOL_55;
}

void lcd_kit_get_bias_vpos_voltage(unsigned int *vpos_val)
{
	int i;
	int vol_size = ARRAY_SIZE(vol_table);

	for (i = 0; i < vol_size; i++) {
		if (vol_table[i].voltage == *vpos_val) {
			*vpos_val = vol_table[i].value;
			break;
		}
	}
	if (i >= vol_size)
		*vpos_val = MT6731_VOL_55;
}

void lcd_kit_get_bias_vneg_voltage(unsigned int *vneg_val)
{
	int i;
	int vol_size = ARRAY_SIZE(vol_table);

	for (i = 0; i < vol_size; i++) {
		if (vol_table[i].voltage == *vneg_val) {
			*vneg_val = vol_table[i].value;
			break;
		}
	}
	if (i >= vol_size)
		*vneg_val = MT6731_VOL_55;
}

int lcd_kit_display_bias_vsp_enable(struct regulate_bias_desc vspconfig)
{
	int ret;
	unsigned int vboost = vspconfig.max_uV;
	unsigned int vpos = vspconfig.max_uV;
	unsigned int delay_time = vspconfig.wait;

	lcd_kit_get_bias_boost_voltage(&vboost);
	lcd_kit_get_bias_vpos_voltage(&vpos);

	if (delay_time == 0)
		delay_time = DLEAY_TIME_0MS;
	else if ((delay_time >= 1) && (delay_time < 4))
		delay_time = DLEAY_TIME_1MS;
	else
		delay_time = DLEAY_TIME_4MS;

	/* set voltage with min & max  6 means 6 bits */
	ret = PMU_REG_MASK(VBOOST_REG_ADDR,
		(unsigned char)(((vboost) | (delay_time << 6)) & 0xFF), 0xFF);
	if (ret < 0) {
		LCD_KIT_ERR("set vboost reg 0x%0x fail\n", VBOOST_REG_ADDR);
		ret = LCD_KIT_FAIL;
		return ret;
	}

	ret = PMU_REG_MASK(VPOS_REG_ADDR, (unsigned char)(vpos & 0xFF), 0x3F);
	if (ret < 0) {
		LCD_KIT_ERR("set vpos reg 0x%0x fail\n", VPOS_REG_ADDR);
		ret = LCD_KIT_FAIL;
	}
	return ret;
}

int lcd_kit_display_bias_vsn_enable(struct regulate_bias_desc vsnconfig)
{
	int ret;
	unsigned int vneg = vsnconfig.max_uV;

	lcd_kit_get_bias_vneg_voltage(&vneg);
	ret = PMU_REG_MASK(VNEG_REG_ADDR, (unsigned char)(vneg & 0xFF), 0x3F);
	if (ret < 0) {
		LCD_KIT_ERR("set vneg reg 0x%0x fail\n", VNEG_REG_ADDR);
		ret = LCD_KIT_FAIL;
		return ret;
	}

	ret = PMU_REG_MASK(ENABLE_REG_ADDR,
		(unsigned char)(VPOS_ENABLE_VAL | VNEG_ENABLE_VAL),
		(unsigned char)(VPOS_ENABLE_MASK | VNEG_ENABLE_MASK));
	if (ret < 0) {
		LCD_KIT_ERR("set vsp vsn enable reg 0x%0x fail\n",
			ENABLE_REG_ADDR);
		ret = LCD_KIT_FAIL;
		return ret;
	}

	return ret;
}

int lcd_kit_display_bias_vsp_disable(struct regulate_bias_desc vsnconfig)
{
	int ret;

	(void)vsnconfig;
	ret = PMU_REG_MASK(ENABLE_REG_ADDR, VPOS_DISABLE_VAL,
		VPOS_ENABLE_MASK);
	if (ret < 0)
		LCD_KIT_ERR("set vsp disable reg 0x%0x fail\n",
			ENABLE_REG_ADDR);
	return ret;
}

int lcd_kit_display_bias_vsn_disable(struct regulate_bias_desc vsnconfig)
{
	int ret;

	(void)vsnconfig;
	ret = PMU_REG_MASK(ENABLE_REG_ADDR, VNEG_DISABLE_VAL,
		VNEG_ENABLE_MASK);
	if (ret < 0)
		LCD_KIT_ERR("set vsp disable reg 0x%0x fail\n",
			ENABLE_REG_ADDR);
	return ret;
}

static struct lcd_kit_mtk_regulate_ops mtk_regulate_ops = {
	.reguate_init = NULL,
	.reguate_vsp_set_voltage = NULL,
	.reguate_vsp_on_delay = NULL,
	.reguate_vsp_enable = lcd_kit_display_bias_vsp_enable,
	.reguate_vsp_disable = lcd_kit_display_bias_vsp_disable,
	.reguate_vsn_set_voltage = NULL,
	.reguate_vsn_on_delay = NULL,
	.reguate_vsn_enable = lcd_kit_display_bias_vsn_enable,
	.reguate_vsn_disable = lcd_kit_display_bias_vsn_disable,
};

struct lcd_kit_mtk_regulate_ops *lcd_kit_mtk_regulate_register(void)
{
	return &mtk_regulate_ops;
}
#endif
