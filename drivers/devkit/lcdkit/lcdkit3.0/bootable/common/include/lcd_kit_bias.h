/*
 * lcd_kit_bias.h
 *
 * lcdkit bias function head file for lcd driver
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

#ifndef _LCD_KIT_BIAS_H_
#define _LCD_KIT_BIAS_H_
#include <types.h>
#include <stdarg.h>

#define MAX_BIAS_NUM 10

struct lcd_kit_bias_ops {
	int (*set_bias_voltage)(int vpos, int vneg);
};

/* function declare */
struct lcd_kit_bias_ops *lcd_kit_get_bias_ops(void);
int lcd_kit_bias_register(struct lcd_kit_bias_ops *ops);
int lcd_kit_bias_unregister(struct lcd_kit_bias_ops *ops);
void lcd_kit_dts_set_bias_status(const char *name);
int lcd_kit_bias_recognize_register(int (*func)(void));
int lcd_kit_bias_recognize_call(void);

struct lcd_kit_bias_recognize {
	int used;
	int (*bias_ic_recognize)(void);
};

#endif

