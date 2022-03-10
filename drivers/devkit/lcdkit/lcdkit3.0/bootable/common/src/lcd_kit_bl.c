/*
 * lcd_kit_bl.c
 *
 * lcdkit backlight function for lcd driver
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

#include "lcd_kit_bl.h"
#include "lcd_kit_common.h"

static struct lcd_kit_bl_ops *g_bl_ops;
static struct lcd_kit_bl_recognize backlight_recognize[MAX_BACKLIGHT_IC_NUM];
int lcd_kit_bl_register(struct lcd_kit_bl_ops *ops)
{
	if (g_bl_ops) {
		LCD_KIT_ERR("g_bl_ops has already been registered!\n");
		return LCD_KIT_FAIL;
	}
	g_bl_ops = ops;
	return LCD_KIT_OK;
}

int lcd_kit_bl_unregister(struct lcd_kit_bl_ops *ops)
{
	if (g_bl_ops == ops) {
		g_bl_ops = NULL;
		return LCD_KIT_OK;
	}
	LCD_KIT_ERR("g_bl_ops unregister fail!\n");
	return LCD_KIT_FAIL;
}

struct lcd_kit_bl_ops *lcd_kit_get_bl_ops(void)
{
	return g_bl_ops;
}

void lcd_kit_dts_set_backlight_status(const char *name)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("can not get adapt_ops!\n");
		return;
	}
	if (name == NULL) {
		LCD_KIT_ERR("name do not exist!\n");
		return;
	}
#ifdef FASTBOOT_FW_DTB_ENABLE
	if (adapt_ops->fwdtb_change_bl_dts)
		adapt_ops->fwdtb_change_bl_dts(name);
#else
	if (adapt_ops->change_dts_status_by_compatible)
		adapt_ops->change_dts_status_by_compatible(name);
#endif
}

int lcd_kit_backlight_recognize_register(int (*func)(void))
{
	int ret = LCD_KIT_OK;
	int i;

	for (i = 0; i < MAX_BACKLIGHT_IC_NUM; i++) {
		if (backlight_recognize[i].used == 0) {
			backlight_recognize[i].backlight_ic_recognize = func;
			backlight_recognize[i].used = 1;
			break;
		}
	}
	if (i >= MAX_BACKLIGHT_IC_NUM) {
		ret = LCD_KIT_FAIL;
		LCD_KIT_ERR("backlight recognize register not success.\n");
	}
	return ret;
}

int lcd_kit_backlight_recognize_call(void)
{
	int ret = LCD_KIT_OK;
	int i;

	for (i = 0; i < MAX_BACKLIGHT_IC_NUM; i++) {
		if (backlight_recognize[i].used == 1)
			ret = backlight_recognize[i].backlight_ic_recognize();
		if (ret == LCD_KIT_OK) {
			LCD_KIT_INFO("backlight recognize call success.\n");
			break;
		}
	}
	if (i >= MAX_BACKLIGHT_IC_NUM) {
		ret = LCD_KIT_FAIL;
		LCD_KIT_ERR("backlight recognize call fail.\n");
	}
	return ret;
}
