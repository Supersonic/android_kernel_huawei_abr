/*
 * lcd_kit_ext_display.c
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
#include "lcd_kit_ext_disp.h"
#include "lcd_kit_disp.h"
#include <boardid.h>
#include <oeminfo_ops.h>
#include "hisi_fb.h"
#include "lcd_kit_effect.h"
#include "lcd_kit_power.h"
#include "lcd_kit_utils.h"

struct lcd_public_config g_lcd_public_cfg = {
	LCD_NORMAL_TYPE,
	LCD_MAIN_PANEL,
};

struct lcd_public_config *lcd_kit_get_public_config(void)
{
	return &g_lcd_public_cfg;
}

struct hisi_panel_info *lcd_kit_get_active_panel_info(void)
{
	if ((PUBLIC_CFG->product_type != LCD_FOLDER_TYPE)
		|| (PUBLIC_CFG->active_panel_id >= LCD_ACTIVE_PANEL_BUTT)) {
		HISI_FB_ERR("lcd folder type err %d.\n",
			PUBLIC_CFG->active_panel_id);
		return NULL;
	}
	return lcd_kit_get_pinfo();
}

void lcd_kit_panel_switch(int panel_id)
{
	if (PUBLIC_CFG->product_type != LCD_FOLDER_TYPE) {
		LCD_KIT_ERR("lcd folder type is %d.\n",
			PUBLIC_CFG->product_type);
		return;
	}

	if (panel_id >= LCD_ACTIVE_PANEL_BUTT) {
		LCD_KIT_ERR("lcd folder type err %d.\n", panel_id);
		return;
	}

	LCD_KIT_INFO("lcd folder type from %d change to %d.\n",
		PUBLIC_CFG->active_panel_id, panel_id);
	PUBLIC_CFG->active_panel_id = panel_id;
}

extern void *lcd_fdt;
extern struct fdt_operators *lcd_fdt_ops;

void lcd_ext_fdt_open(void)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return;
#else
	lcd_fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (lcd_fdt_ops == NULL) {
		LCD_KIT_ERR("can't get fdt operators\n");
		return;
	}
	lcd_fdt = lcd_fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (lcd_fdt == NULL) {
		LCD_KIT_ERR("fdt is null!!!\n");
		return;
	}
#endif
}

void lcd_kit_ext_panel_probe(void)
{
	struct hisi_panel_info *pinfo = NULL;

	LCD_KIT_INFO(" enter\n");
	if (PUBLIC_CFG->product_type != LCD_FOLDER_TYPE) {
		LCD_KIT_INFO("multi_panel_type %d\n",
			PUBLIC_CFG->product_type);
		return;
	}
	lcd_kit_panel_switch(LCD_EXT_PANEL);
	/* init lcd panel info */
	pinfo = lcd_kit_get_pinfo();
	memset_s(pinfo, sizeof(struct hisi_panel_info),
		0, sizeof(struct hisi_panel_info));
	pinfo->disp_cnnt_type = LCD_EXT_PANEL;
#if !defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) && !defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	lcd_ext_fdt_open();
#endif
	/* common init */
	if (common_ops->common_init)
		common_ops->common_init(disp_info->lcd_name);
	/* utils init */
	lcd_kit_utils_init(pinfo);
	/* panel init */
	lcd_kit_panel_init();
	/* power init */
	lcd_kit_power_init();

	LCD_KIT_INFO(" exit\n");
}

void lcd_kit_ext_panel_init(void)
{
	int pin_num = 0;
	struct lcd_type_operators *lcd_type_ops = NULL;

	lcd_type_ops = get_operators(LCD_TYPE_MODULE_NAME_STR);
	if (!lcd_type_ops) {
		LCD_KIT_ERR("failed to get lcd type operator!\n");
		return;
	}
	LCD_KIT_INFO("lcd_type = %d\n", lcd_type_ops->get_lcd_type());
	if (lcd_type_ops->get_lcd_type() == LCD_KIT) {
		LCD_KIT_INFO("lcd type is LCD_KIT\n");
		(void)get_dts_value(DTS_COMP_LCD_PANEL_TYPE,
			"product_type", &PUBLIC_CFG->product_type);
		LCD_KIT_INFO("product type is %d.\n", PUBLIC_CFG->product_type);
		if (PUBLIC_CFG->product_type != LCD_FOLDER_TYPE)
			return;
		lcd_kit_panel_switch(LCD_EXT_PANEL);
		/* init lcd id */
		disp_info->lcd_id = lcd_type_ops->get_ext_lcd_id(&pin_num);
		disp_info->lcd_id = (LCD_EXT_PANEL << LCD_EXT_LCD_BIT) |
			disp_info->lcd_id;
		disp_info->product_id = lcd_type_ops->get_product_id();
		LCD_KIT_INFO("disp_info->lcd_id = %d, disp_info->product_id = %d\n",
			disp_info->lcd_id, disp_info->product_id);
		disp_info->compatible = lcd_kit_get_compatible(disp_info->product_id,
			disp_info->lcd_id, pin_num);
		disp_info->lcd_name = lcd_kit_get_lcd_name(disp_info->product_id,
			disp_info->lcd_id, pin_num);
		lcd_type_ops->set_ext_lcd_panel_type(disp_info->compatible);
		/* adapt init */
		lcd_kit_adapt_init();
		LCD_KIT_INFO("disp_info->lcd_id = %d, disp_info->product_id = %d\n",
			disp_info->lcd_id, disp_info->product_id);
		LCD_KIT_INFO("disp_info->lcd_name = %s, disp_info->compatible = %s\n",
			disp_info->lcd_name, disp_info->compatible);
	} else {
		LCD_KIT_INFO("lcd type is not LCD_KIT\n");
	}
}

