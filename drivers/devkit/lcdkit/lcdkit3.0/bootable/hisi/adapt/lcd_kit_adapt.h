/*
 * lcd_kit_adapt.h
 *
 * lcdkit adapt function head file for lcd driver
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

#ifndef _LCD_KIT_ADAPT_H_
#define _LCD_KIT_ADAPT_H_
#include "hisi_fb.h"
#include "libfdt_env.h"

/* print log */
extern u32 g_lcd_kit_msg_level;
#define LCD_KIT_ERR(msg, ...) do { \
	if (g_lcd_kit_msg_level >= MSG_LEVEL_ERROR) \
		PRINT_ERROR("[LCD_KIT/E]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_KIT_WARNING(msg, ...) do { \
	if (g_lcd_kit_msg_level >= MSG_LEVEL_WARNING) \
		PRINT_ERROR("[LCD_KIT/W]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_KIT_INFO(msg, ...) do { \
	if (g_lcd_kit_msg_level >= MSG_LEVEL_INFO) \
		PRINT_INFO("[LCD_KIT/I]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_KIT_DEBUG(msg, ...) do { \
	if (g_lcd_kit_msg_level >= MSG_LEVEL_DEBUG) \
		PRINT_INFO("[LCD_KIT/D]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

/* function declare */
#endif
