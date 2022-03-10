/*
 * lcd_kit_bl.h
 *
 * lcdkit backlight function head file for lcd driver
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

#ifndef _LCD_KIT_BL_H_
#define _LCD_KIT_BL_H_
#include <types.h>
#include <stdarg.h>

#define MAX_BACKLIGHT_IC_NUM 10

struct lcd_kit_bl_ops {
	int (*set_backlight)(unsigned int level);
	int (*en_backlight)(unsigned int level);
};

struct lcd_kit_bl_recognize {
	int used;
	int (*backlight_ic_recognize)(void);
};

/* function declare  */
struct lcd_kit_bl_ops *lcd_kit_get_bl_ops(void);
int lcd_kit_bl_register(struct lcd_kit_bl_ops *ops);
int lcd_kit_bl_unregister(struct lcd_kit_bl_ops *ops);
int lcd_kit_backlight_recognize_call(void);
void lcd_kit_dts_set_backlight_status(const char *name);
int lcd_kit_backlight_recognize_register(int (*func)(void));
#endif
