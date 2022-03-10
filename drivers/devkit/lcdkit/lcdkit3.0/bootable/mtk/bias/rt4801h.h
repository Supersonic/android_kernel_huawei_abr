/*
 * rt4801h.h
 *
 * rt4801h bias driver head file
 *
 * Copyright (c) 2019-2019 Huawei Technologies Co., Ltd.
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

#ifndef __RT4801H_H_
#define __RT4801H_H_
#include "lcd_kit_common.h"

#define LCD_BIAS_VOL_40 4000000
#define LCD_BIAS_VOL_41 4100000
#define LCD_BIAS_VOL_42 4200000
#define LCD_BIAS_VOL_43 4300000
#define LCD_BIAS_VOL_44 4400000
#define LCD_BIAS_VOL_45 4500000
#define LCD_BIAS_VOL_46 4600000
#define LCD_BIAS_VOL_47 4700000
#define LCD_BIAS_VOL_48 4800000
#define LCD_BIAS_VOL_49 4900000
#define LCD_BIAS_VOL_50 5000000
#define LCD_BIAS_VOL_51 5100000
#define LCD_BIAS_VOL_52 5200000
#define LCD_BIAS_VOL_53 5300000
#define LCD_BIAS_VOL_54 5400000
#define LCD_BIAS_VOL_55 5500000
#define LCD_BIAS_VOL_56 5600000
#define LCD_BIAS_VOL_57 5700000
#define LCD_BIAS_VOL_58 5800000
#define LCD_BIAS_VOL_59 5900000
#define LCD_BIAS_VOL_60 6000000

enum {
	RT4801H_VOL_40_V = 0x00,
	RT4801H_VOL_41_V = 0x01,
	RT4801H_VOL_42_V = 0x02,
	RT4801H_VOL_43_V = 0x03,
	RT4801H_VOL_44_V = 0x04,
	RT4801H_VOL_45_V = 0x05,
	RT4801H_VOL_46_V = 0x06,
	RT4801H_VOL_47_V = 0x07,
	RT4801H_VOL_48_V = 0x08,
	RT4801H_VOL_49_V = 0x09,
	RT4801H_VOL_50_V = 0x0A,
	RT4801H_VOL_51_V = 0x0B,
	RT4801H_VOL_52_V = 0x0C,
	RT4801H_VOL_53_V = 0x0D,
	RT4801H_VOL_54_V = 0x0E,
	RT4801H_VOL_55_V = 0x0F,
	RT4801H_VOL_56_V = 0x10,
	RT4801H_VOL_57_V = 0x11,
	RT4801H_VOL_58_V = 0x12,
	RT4801H_VOL_59_V = 0x13,
	RT4801H_VOL_60_V = 0x14,
	RT4801H_VOL_MAX_V
};

struct rt4801h_voltage {
	unsigned int voltage;
	unsigned char value;
};

#define DTS_COMP_RT4801H "rt,rt4801h"
#define RT4801H_SUPPORT "rt4801h_support"
#define DTS_RT4801H "rt,rt4801h"
#define RT4801H_I2C_BUS_ID "rt4801h_i2c_bus_id"
#define RT4801H_REG_VPOS 0x00
#define RT4801H_REG_VNEG 0x01
#define RT4801H_REG_APP_DIS 0x03
#define RT4801H_REG_VOL_MASK 0x1F
#define RT4801H_APPS_BIT (1 << 6)
#define RT4801H_DISP_BIT (1 << 1)
#define RT4801H_DISN_BIT (1 << 0)

int rt4801h_init(void);
void rt4801h_set_bias_status(void);
#endif

