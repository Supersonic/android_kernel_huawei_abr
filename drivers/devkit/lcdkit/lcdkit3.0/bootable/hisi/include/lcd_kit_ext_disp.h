/*
 * lcd_kit_ext_disp.h
 *
 * lcdkit display function for lcd driver
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
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

#ifndef __LCD_KIT_EXT_DISP_H_
#define __LCD_KIT_EXT_DISP_H_

#include "hisi_fb.h"

#define LCD_EXT_LCD_BIT 4
#define LCD_FOLDER_STR 10
#define LCD_MAX_PANEL_NUM 2
#define DTS_COMP_LCD_PANEL_TYPE "huawei,lcd_panel_type"

enum lcd_actvie_panel_id {
	LCD_MAIN_PANEL,
	LCD_EXT_PANEL,
	LCD_ACTIVE_PANEL_BUTT,
};

enum lcd_product_type {
	LCD_NORMAL_TYPE,
	LCD_FOLDER_TYPE,
	LCD_MULTI_PANEL_BUTT,
};

struct lcd_public_config {
	int product_type;
	unsigned int active_panel_id;
};

extern  struct lcd_public_config g_lcd_public_cfg;
#define LCD_ACTIVE_PANEL \
	((g_lcd_public_cfg.active_panel_id >= LCD_ACTIVE_PANEL_BUTT) ? \
		0 : g_lcd_public_cfg.active_panel_id)

struct lcd_public_config *lcd_kit_get_public_config(void);
#define PUBLIC_CFG lcd_kit_get_public_config()
void lcd_kit_ext_panel_init(void);
void lcd_kit_panel_switch(int panel_id);
void lcd_kit_ext_panel_probe(void);
struct hisi_panel_info *lcd_kit_get_active_panel_info(void);
struct hisi_panel_info *lcd_kit_get_pinfo(void);
#endif
