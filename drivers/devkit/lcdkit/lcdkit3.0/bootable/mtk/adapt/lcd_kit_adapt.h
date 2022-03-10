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

#include <stdio.h>
#include "lcd_kit_common.h"

#define DTS_COMP_LCD_PANEL_TYPE	"huawei,lcd_panel_type"
#define DTS_LCD_PANEL_TYPE	"/huawei,lcd_panel"
#define LCD_TYPE_NAME_MAX	30
#define MIPI_MODE_LP		1
#define MIPI_MODE_HS		0
#define MAX_LCD_NAME_LEN	20

/* print log */
extern u32 g_lcd_kit_msg_level;

enum iovcc_ctrl_mode {
	IOVCC_CTRL_NONE,
	IOVCC_CTRL_REGULATOR,
	IOVCC_CTRL_GPIO,
};

enum mtk_lcd_type {
	LCDKIT,
	LCD_KIT,
	UNKNOWN_LCD,
};

struct lcd_type_info {
	enum mtk_lcd_type lcd_type;
	unsigned char lcd_name[MAX_LCD_NAME_LEN];
};

#define LCD_KIT_ERR(msg, ...) do { \
	if (g_lcd_kit_msg_level >= MSG_LEVEL_ERROR) \
		dprintf(CRITICAL, "[LCD_KIT/E]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_KIT_WARNING(msg, ...) do { \
	if (g_lcd_kit_msg_level >= MSG_LEVEL_WARNING) \
		dprintf(SPEW, "[LCD_KIT/W]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_KIT_INFO(msg, ...) do { \
		if (g_lcd_kit_msg_level >= MSG_LEVEL_INFO) \
			dprintf(SPEW, "[LCD_KIT/I]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_KIT_DEBUG(msg, ...) do { \
	if (g_lcd_kit_msg_level >= MSG_LEVEL_DEBUG) \
		dprintf(SPEW, "[LCD_KIT/D]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

uint32_t lcdkit_get_lcd_id(void);
int lcd_kit_get_product_id(void);
void lcdkit_set_lcd_panel_type(char *type);
void lcdkit_set_lcd_panel_status(char *lcd_name);
int lcd_kit_get_lcd_type(void);
void lcd_kit_set_lcd_name_to_no_lcd(void);
void lcd_kit_get_lcdname(void);
int lcd_kit_get_detect_type(void);
void lcdkit_set_lcd_ddic_max_brightness(unsigned int bl_val);
/* function declare */
#endif
