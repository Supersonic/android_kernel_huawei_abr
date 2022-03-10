/*
 * tps65132.h
 *
 * tps65132 bias driver head file
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

#ifndef __TPS65132_H
#define __TPS65132_H

#include "lcd_kit_common.h"

enum {
	TPS65132_VOL_40_V   =  0x00, /* 4.0V */
	TPS65132_VOL_41_V   =  0x01, /* 4.1V */
	TPS65132_VOL_42_V   =  0x02, /* 4.2V */
	TPS65132_VOL_43_V   =  0x03, /* 4.3V */
	TPS65132_VOL_44_V   =  0x04, /* 4.4V */
	TPS65132_VOL_45_V   =  0x05, /* 4.5V */
	TPS65132_VOL_46_V   =  0x06, /* 4.6V */
	TPS65132_VOL_47_V   =  0x07, /* 4.7V */
	TPS65132_VOL_48_V   =  0x08, /* 4.8V */
	TPS65132_VOL_49_V   =  0x09, /* 4.9V */
	TPS65132_VOL_50_V   =  0x0A, /* 5.0V */
	TPS65132_VOL_51_V   =  0x0B, /* 5.1V */
	TPS65132_VOL_52_V   =  0x0C, /* 5.2V */
	TPS65132_VOL_53_V   =  0x0D, /* 5.3V */
	TPS65132_VOL_54_V   =  0x0E, /* 5.4V */
	TPS65132_VOL_55_V   =  0x0F, /* 5.5V */
	TPS65132_VOL_56_V   =  0x10, /* 5.6V */
	TPS65132_VOL_57_V   =  0x11, /* 5.7V */
	TPS65132_VOL_58_V   =  0x12, /* 5.8V */
	TPS65132_VOL_59_V   =  0x13, /* 5.9V */
	TPS65132_VOL_60_V   =  0x14, /* 6.0V */
	TPS65132_VOL_MAX_V
};

struct tps65132_voltage {
	int voltage;
	int value;
};

#define TPS65132_MODULE_NAME_STR	"tps65132"
#define TPS65132_MODULE_NAME	tps65132
#define DTS_COMP_TPS65132	"ti,tps65132"
#define TPS65132_SUPPORT	"tps65132_support"
#define DTS_TPS65132		"ti,tps65132"
#define TPS65132_I2C_BUS_ID	"tps65132_i2c_bus_id"
#define TPS65132_REG_VPOS	0x00
#define TPS65132_REG_VNEG	0x01
#define TPS65132_REG_APP_DIS	0x03
#define TPS65132_REG_CTL	0xFF

#define TPS65132_REG_VOL_MASK	0x1F
#define TPS65312_APPS_BIT	(1 << 6)
#define TPS65132_DISP_BIT	(1 << 1)
#define TPS65132_DISN_BIT	(1 << 0)
#define TPS65132_WED_BIT	(1 << 7)

void tps65132_set_voltage(int vpos, int vneg);
void tps65132_set_bias_status(void);
int tps65132_init(void);
#endif
