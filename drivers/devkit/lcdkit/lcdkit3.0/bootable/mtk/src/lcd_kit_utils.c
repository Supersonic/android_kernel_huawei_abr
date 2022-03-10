/*
 * lcd_kit_utils.c
 *
 * lcdkit utils function for lcd driver
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

#include "lcd_kit_utils.h"
#include "lcd_kit_disp.h"
#include "lcd_kit_power.h"
#include "lcd_kit_utils.h"
#include "lcd_kit_panels.h"
#include "mt_gpt.h"
#include <string.h>

#define MAX_DELAY_TIME 200

u32 lcd_kit_get_blmaxnit(struct mtk_panel_info *pinfo)
{
	u32 bl_max_nit;
	u32 lcd_kit_brightness_ddic_info;

	lcd_kit_brightness_ddic_info =
		pinfo->blmaxnit.lcd_kit_brightness_ddic_info;
	if ((pinfo->blmaxnit.get_blmaxnit_type == GET_BLMAXNIT_FROM_DDIC) &&
		(lcd_kit_brightness_ddic_info > BL_MIN) &&
		(lcd_kit_brightness_ddic_info < BL_MAX)) {
		bl_max_nit =
			(lcd_kit_brightness_ddic_info < BL_REG_NOUSE_VALUE) ?
			(lcd_kit_brightness_ddic_info + pinfo->bl_max_nit_min_value) :
			(lcd_kit_brightness_ddic_info + pinfo->bl_max_nit_min_value - 1);
	} else {
		bl_max_nit = pinfo->bl_max_nit;
	}

	return bl_max_nit;
}

char *lcd_kit_get_compatible(uint32_t product_id, uint32_t lcd_id)
{
	uint32_t i;

	for (i = 0; i < ARRAY_SIZE(lcd_kit_map); ++i) {
		if ((lcd_kit_map[i].board_id == product_id) &&
		    (lcd_kit_map[i].gpio_id == lcd_id)) {
			return lcd_kit_map[i].compatible;
		}
	}
	/* use defaut panel */
	return LCD_KIT_DEFAULT_COMPATIBLE;
}

char *lcd_kit_get_lcd_name(uint32_t product_id, uint32_t lcd_id)
{
	uint32_t i;

	for (i = 0; i < ARRAY_SIZE(lcd_kit_map); ++i) {
		if ((lcd_kit_map[i].board_id == product_id) &&
		    (lcd_kit_map[i].gpio_id == lcd_id)) {
			return lcd_kit_map[i].lcd_name;
		}
	}
	/* use defaut panel */
	return LCD_KIT_DEFAULT_PANEL;
}

int lcd_kit_get_tp_color(void)
{
	int ret = LCD_KIT_OK;
	uint8_t read_value = 0;
	int out_len = 0;
	struct mtk_panel_info *panel_info = NULL;

	panel_info = lcm_get_panel_info();
	if (panel_info == NULL) {
		LCD_KIT_ERR("panel_info is NULL\n");
		return LCD_KIT_FAIL;
	}

	if (disp_info->tp_color.support) {
		ret = lcd_kit_dsi_cmds_rx(NULL, &read_value, out_len,
			&disp_info->tp_color.cmds);
		if (ret)
			panel_info->tp_color = 0;
		else
			panel_info->tp_color = read_value;
		LCD_KIT_INFO("tp color = %d\n", panel_info->tp_color);
	} else {
		LCD_KIT_INFO("Not support tp color\n");
	}
	return ret;
}

static int lcd_kit_parse_disp_info(const void *fdt, int nodeoffset)
{
	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return LCD_KIT_FAIL;
	}

	/* quickly sleep out */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,quickly-sleep-out-support",
		&disp_info->quickly_sleep_out.support, 0);
	if (disp_info->quickly_sleep_out.support)
		lcd_kit_get_dts_u32_default(fdt, nodeoffset,
			"lcd-kit,quickly-sleep-out-interval",
			&disp_info->quickly_sleep_out.interval, 0);
	/* tp color support */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,tp-color-support",
		&disp_info->tp_color.support, 0);
	if (disp_info->tp_color.support)
		lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset, "lcd-kit,tp-color-cmds",
			"lcd-kit,tp-color-cmds-state",
			&disp_info->tp_color.cmds);
	/* elvdd detect */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,elvdd-detect-support",
		&disp_info->elvdd_detect.support, 0);
	if (disp_info->elvdd_detect.support) {
		lcd_kit_get_dts_u32_default(fdt, nodeoffset,
			"lcd-kit,elvdd-detect-type",
			&disp_info->elvdd_detect.detect_type, 0);
		if (disp_info->elvdd_detect.detect_type ==
			ELVDD_MIPI_CHECK_MODE)
			lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset,
				"lcd-kit,elvdd-detect-cmds",
				"lcd-kit,elvdd-detect-cmds-state",
				&disp_info->elvdd_detect.cmds);
		else
			lcd_kit_get_dts_u32_default(fdt, nodeoffset,
				"lcd-kit,elvdd-detect-gpio",
				&disp_info->elvdd_detect.detect_gpio, 0);
		lcd_kit_get_dts_u32_default(fdt, nodeoffset,
			"lcd-kit,elvdd-detect-value",
			&disp_info->elvdd_detect.exp_value, 0);
		lcd_kit_get_dts_u32_default(fdt, nodeoffset,
			"lcd-kit,elvdd-value-mask",
			&disp_info->elvdd_detect.exp_value_mask, 0);
		lcd_kit_get_dts_u32_default(fdt, nodeoffset,
			"lcd-kit,elvdd-delay",
			&disp_info->elvdd_detect.delay, 0);
	}
	return LCD_KIT_OK;
}

static int lcd_kit_parse_dsc_params(const void *fdt, int nodeoffset,
	struct mtk_panel_info *pinfo)
{
	int ret = LCD_KIT_OK;

	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-ver",
		&pinfo->dsc_params.ver, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-slice-mode",
		&pinfo->dsc_params.slice_mode, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-rgb-swap",
		&pinfo->dsc_params.rgb_swap, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-cfg",
		&pinfo->dsc_params.dsc_cfg, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-rct-on",
		&pinfo->dsc_params.rct_on, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-bit-per-channel",
		&pinfo->dsc_params.bit_per_channel, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-line-buf-depth",
		&pinfo->dsc_params.dsc_line_buf_depth, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-bp-enable",
		&pinfo->dsc_params.bp_enable, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-bit-per-pixel",
		&pinfo->dsc_params.bit_per_pixel, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-pic-height",
		&pinfo->dsc_params.pic_height, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-pic-width",
		&pinfo->dsc_params.pic_width, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-slice-height",
		&pinfo->dsc_params.slice_height, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-slice-width",
		&pinfo->dsc_params.slice_width, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-chunk-size",
		&pinfo->dsc_params.chunk_size, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-xmit-delay",
		&pinfo->dsc_params.xmit_delay, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-dec-delay",
		&pinfo->dsc_params.dec_delay, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-scale-value",
		&pinfo->dsc_params.scale_value, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,dsc-increment-interval",
		&pinfo->dsc_params.increment_interval, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,dsc-decrement-interval",
		&pinfo->dsc_params.decrement_interval, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-line-bpg-offset",
		&pinfo->dsc_params.line_bpg_offset, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-nfl-bpg-offset",
		&pinfo->dsc_params.nfl_bpg_offset, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,dsc-slice-bpg-offset",
		&pinfo->dsc_params.slice_bpg_offset, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-initial-offset",
		&pinfo->dsc_params.initial_offset, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-final-offset",
		&pinfo->dsc_params.final_offset, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-flatness-minqp",
		&pinfo->dsc_params.flatness_minqp, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-flatness-maxqp",
		&pinfo->dsc_params.flatness_maxqp, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-rc-model-size",
		&pinfo->dsc_params.rc_model_size, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-rc-edge-factor",
		&pinfo->dsc_params.rc_edge_factor, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,dsc-rc-quant-incr-limit0",
		&pinfo->dsc_params.rc_quant_incr_limit0, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,dsc-rc-quant-incr-limit1",
		&pinfo->dsc_params.rc_quant_incr_limit1, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,dsc-rc-tgt-offset-hi",
		&pinfo->dsc_params.rc_tgt_offset_hi, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,dsc-rc-tgt-offset-lo",
		&pinfo->dsc_params.rc_tgt_offset_lo, 0);
	return ret;
}

static int lcd_kit_pinfo_init(const void *fdt, int nodeoffset,
	struct mtk_panel_info *pinfo)
{
	int ret = LCD_KIT_OK;

	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return LCD_KIT_FAIL;
	}
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	/* panel info */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-xres",
		&pinfo->xres, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-yres",
		&pinfo->yres, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-width",
		&pinfo->width, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-height",
		&pinfo->height, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-orientation",
		&pinfo->orientation, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-bpp",
		&pinfo->bpp, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-bgr-fmt",
		&pinfo->bgr_fmt, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-bl-type",
		&pinfo->bl_set_type, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-bl-min",
		&pinfo->bl_min, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-bl-max",
		&pinfo->bl_max, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-cmd-type",
		&pinfo->type, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-output-mode",
		&pinfo->output_mode, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-pxl-clk",
		(u32 *)&pinfo->pxl_clk_rate, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-pxl-clk-div",
		&pinfo->pxl_clk_rate_div, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-data-rate",
		&pinfo->data_rate, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,h-back-porch",
		&pinfo->ldi.h_back_porch, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,h-front-porch",
		&pinfo->ldi.h_front_porch, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,h-pulse-width",
		&pinfo->ldi.h_pulse_width, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,v-back-porch",
		&pinfo->ldi.v_back_porch, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,v-front-porch",
		&pinfo->ldi.v_front_porch, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,v-pulse-width",
		&pinfo->ldi.v_pulse_width, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,mipi-lane-nums",
		&pinfo->mipi.lane_nums, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,mipi-clk-post-adjust",
		&pinfo->mipi.clk_post_adjust, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,mipi-rg-vrefsel-vcm-adjust",
		&pinfo->mipi.rg_vrefsel_vcm_adjust, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,mipi-phy-mode",
		&pinfo->mipi.phy_mode, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,mipi-lp11-flag",
		&pinfo->mipi.lp11_flag, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,mipi-non-continue-enable",
		&pinfo->mipi.non_continue_en, 0);

	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-lcm-type",
		&pinfo->panel_lcm_type, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-ldi-dsi-mode",
		&pinfo->panel_dsi_mode, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-dsi-switch-mode",
		&pinfo->panel_dsi_switch_mode, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-ldi-trans-seq",
		&pinfo->panel_trans_seq, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-ldi-data-padding",
		&pinfo->panel_data_padding, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-ldi-packet-size",
		&pinfo->panel_packtet_size, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-ldi-ps",
		&pinfo->panel_ps, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-vfp-lp",
		&pinfo->ldi.v_front_porch_forlp, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-fbk-div",
		&pinfo->pxl_fbk_div, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,ssc-disable",
		&pinfo->ssc_disable, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,ssc-range",
		&pinfo->ssc_range, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-bl-ic-ctrl-type",
		&pinfo->bl_ic_ctrl_mode, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-bias-ic-ctrl-type",
		&pinfo->bias_ic_ctrl_mode, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-bl-max-nit",
		&pinfo->bl_max_nit, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-bl-boot",
		&pinfo->bl_boot, BL_BOOT_DEF);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-getblmaxnit-type",
		&pinfo->blmaxnit.get_blmaxnit_type, 0);
	if (pinfo->blmaxnit.get_blmaxnit_type == GET_BLMAXNIT_FROM_DDIC)
		lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset,
			"lcd-kit,panel-bl-maxnit-command",
			"lcd-kit,panel-bl-maxnit-command-state",
			&pinfo->blmaxnit.bl_maxnit_cmds);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,mipi-read-gerneric",
		&pinfo->mipi_read_gcs_support, 1);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,dsc-enable",
		&pinfo->dsc_enable, 0);
	if (pinfo->dsc_enable)
		lcd_kit_parse_dsc_params(fdt, nodeoffset, pinfo);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-blmaxnit-min-value",
		&pinfo->bl_max_nit_min_value, BL_NIT);
	return ret;
}

static void lcd_kit_parse_power(const void *fdt, int nodeoffset)
{
	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return;
	}
	if (power_hdl == NULL) {
		LCD_KIT_ERR("power_hdl is null\n");
		return;
	}
	/* vci */
	if (power_hdl->lcd_vci.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-vci",
			&power_hdl->lcd_vci);
	/* iovcc */
	if (power_hdl->lcd_iovcc.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-iovcc",
			&power_hdl->lcd_iovcc);
	/* vsp */
	if (power_hdl->lcd_vsp.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-vsp",
			&power_hdl->lcd_vsp);
	/* vsn */
	if (power_hdl->lcd_vsn.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-vsn",
			&power_hdl->lcd_vsn);
	/* lcd reset */
	if (power_hdl->lcd_rst.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-reset",
			&power_hdl->lcd_rst);
	/* backlight */
	if (power_hdl->lcd_backlight.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-backlight",
			&power_hdl->lcd_backlight);
	/* TE0 */
	if (power_hdl->lcd_te0.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-te0",
			&power_hdl->lcd_te0);
	/* tp reset */
	if (power_hdl->tp_rst.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,tp-reset",
			&power_hdl->tp_rst);
	/* lcd vdd */
	if (power_hdl->lcd_vdd.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-vdd",
			&power_hdl->lcd_vdd);
	if (power_hdl->lcd_aod.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-aod",
			&power_hdl->lcd_aod);
	if (power_hdl->lcd_poweric.buf == NULL)
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-poweric",
			&power_hdl->lcd_poweric);
}

static void lcd_kit_parse_power_seq(const void *fdt, int nodeoffset)
{
	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return;
	}
	if (power_seq == NULL) {
		LCD_KIT_ERR("power_seq is null\n");
		return;
	}
	/* here 3 means 3 arrays */
	lcd_kit_parse_dts_arrays(fdt, nodeoffset, "lcd-kit,power-on-stage",
		&power_seq->power_on_seq, 3);
	lcd_kit_parse_dts_arrays(fdt, nodeoffset, "lcd-kit,lp-on-stage",
		&power_seq->panel_on_lp_seq, 3);
	lcd_kit_parse_dts_arrays(fdt, nodeoffset, "lcd-kit,hs-on-stage",
		&power_seq->panel_on_hs_seq, 3);
	lcd_kit_parse_dts_arrays(fdt, nodeoffset, "lcd-kit,power-off-stage",
		&power_seq->power_off_seq, 3);
	lcd_kit_parse_dts_arrays(fdt, nodeoffset, "lcd-kit,hs-off-stage",
		&power_seq->panel_off_hs_seq, 3);
	lcd_kit_parse_dts_arrays(fdt, nodeoffset, "lcd-kit,lp-off-stage",
		&power_seq->panel_off_lp_seq, 3);
}

static void lcd_kit_parse_common_info(const void *fdt, int nodeoffset)
{
	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return;
	}
	if (common_info == NULL) {
		LCD_KIT_ERR("common_info is null\n");
		return;
	}
	/* panel cmds */
	lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset, "lcd-kit,panel-on-cmds",
		"lcd-kit,panel-on-cmds-state", &common_info->panel_on_cmds);
	lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset, "lcd-kit,panel-off-cmds",
		"lcd-kit,panel-off-cmds-state", &common_info->panel_off_cmds);
	/* backlight */
	lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset, "lcd-kit,backlight-cmds",
	"lcd-kit,backlight-cmds-state", &common_info->backlight.bl_cmd);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,backlight-order",
		&common_info->backlight.order, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-bl-max",
		&common_info->backlight.bl_max, 0);
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,panel-bl-min",
		&common_info->backlight.bl_min, 0);
	/* check backlight short/open */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,check-bl-support",
		&common_info->check_bl_support, 0);
	/* check reg on */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,check-reg-on-support",
		&common_info->check_reg_on.support, 0);
	if (common_info->check_reg_on.support) {
		lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset, "lcd-kit,check-reg-on-cmds",
			"lcd-kit,check-reg-on-cmds-state",
			&common_info->check_reg_on.cmds);
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,check-reg-on-value",
			&common_info->check_reg_on.value);
		lcd_kit_get_dts_u32_default(fdt, nodeoffset,
			"lcd-kit,check-reg-on-support-dsm-report",
			&common_info->check_reg_on.support_dsm_report, 0);
	}
	/* check reg off */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,check-reg-off-support",
		&common_info->check_reg_off.support, 0);
	if (common_info->check_reg_off.support) {
		lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset, "lcd-kit,check-reg-off-cmds",
			"lcd-kit,check-reg-off-cmds-state",
			&common_info->check_reg_off.cmds);
		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,check-reg-off-value",
			&common_info->check_reg_off.value);
		lcd_kit_get_dts_u32_default(fdt, nodeoffset,
			"lcd-kit,check-reg-off-support-dsm-report",
			&common_info->check_reg_off.support_dsm_report, 0);
	}
}

static void lcd_kit_parse_effect(const void *fdt, int nodeoffset)
{
	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return;
	}
	if (common_info == NULL) {
		LCD_KIT_ERR("common_info is null\n");
		return;
	}
	/* effect on support */
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,effect-on-support",
		&common_info->effect_on.support, 0);
	if (common_info->effect_on.support)
		lcd_kit_parse_dts_dcs_cmds(fdt, nodeoffset, "lcd-kit,effect-on-cmds",
			"lcd-kit,effect-on-cmds-state",
			&common_info->effect_on.cmds);
}

static void lcd_kit_parse_btb_check(const void *fdt, int nodeoffset)
{
	lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,lcd-btb-support",
		&common_info->btb_support, 0);

	if (common_info->btb_support) {
		lcd_kit_get_dts_u32_default(fdt, nodeoffset, "lcd-kit,lcd-btb-check-type",
			&common_info->btb_check_type, 0);

		lcd_kit_parse_dts_array(fdt, nodeoffset, "lcd-kit,lcd-btb-gpio",
			&common_info->lcd_btb_gpio);
	}
}

static void lcd_kit_common_info_init(const void *fdt, int nodeoffset)
{
	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is null\n");
		return;
	}
	lcd_kit_parse_common_info(fdt, nodeoffset);
	lcd_kit_parse_power_seq(fdt, nodeoffset);
	lcd_kit_parse_power(fdt, nodeoffset);
	lcd_kit_parse_effect(fdt, nodeoffset);
	/* btb check */
	lcd_kit_parse_btb_check(fdt, nodeoffset);
}

void lcd_kit_disp_on_check_delay(void)
{
	unsigned long delta_time;
	unsigned int delay_margin;
	unsigned int max_delay_margin = MAX_DELAY_TIME;

	if (disp_info == NULL) {
		LCD_KIT_INFO("disp_info is NULL\n");
		return;
	}
	delta_time = get_timer(disp_info->quickly_sleep_out.panel_on_record);
	if (delta_time >= disp_info->quickly_sleep_out.interval) {
		LCD_KIT_INFO("%lu > %d, no need delay\n",
			delta_time,
			disp_info->quickly_sleep_out.interval);
		disp_info->quickly_sleep_out.panel_on_tag = false;
		return;
	}
	delay_margin = disp_info->quickly_sleep_out.interval -
		delta_time;
	if (delay_margin > max_delay_margin) {
		LCD_KIT_INFO("something maybe error");
		disp_info->quickly_sleep_out.panel_on_tag = false;
		return;
	}
	mdelay(delay_margin);
	disp_info->quickly_sleep_out.panel_on_tag = false;
}

void lcd_kit_disp_on_record_time(void)
{
	if (disp_info == NULL) {
		LCD_KIT_INFO("disp_info is NULL\n");
		return;
	}
	disp_info->quickly_sleep_out.panel_on_record = get_timer(0);
	LCD_KIT_INFO("display on at %lu mil seconds\n",
		disp_info->quickly_sleep_out.panel_on_record);
	disp_info->quickly_sleep_out.panel_on_tag = true;
}

#ifdef MTK_ROUND_CORNER_SUPPORT
void lcd_kit_parse_round_corner_info(const void *fdt, int nodeoffset,
	struct mtk_panel_info *pinfo)
{
	int ret;
	struct lcd_kit_array_data array_data;
	int i;
	unsigned char *tmp = NULL;

	if ((fdt == NULL) || (pinfo == NULL)) {
		LCD_KIT_ERR("fdt or pinfo is null\n");
		return;
	}
	ret = lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-round-corner-enable",
		&pinfo->round_corner.round_corner_en, 0);
	if (ret == LCD_KIT_FAIL) {
		LCD_KIT_ERR("parse round-corner-enable fail\n");
		return;
	}
	if (pinfo->round_corner.round_corner_en == 0)
		return;
	ret = lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-round-corner-full-content",
		&pinfo->round_corner.full_content, 0);
	if (ret == LCD_KIT_FAIL) {
		LCD_KIT_ERR("parse round-corner-full-content fail\n");
		return;
	}
	ret = lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-round-corner-h",
		&pinfo->round_corner.h, 0);
	if (ret == LCD_KIT_FAIL) {
		LCD_KIT_ERR("parse round-corner-h fail\n");
		return;
	}
	ret = lcd_kit_get_dts_u32_default(fdt, nodeoffset,
		"lcd-kit,panel-round-corner-h-bot",
		&pinfo->round_corner.h_bot, 0);
	if (ret == LCD_KIT_FAIL) {
		LCD_KIT_ERR("parse round-corner-h-bot fail\n");
		return;
	}
	array_data.buf = NULL;
	array_data.cnt = 0;
	ret = lcd_kit_parse_dts_array(fdt, nodeoffset,
		"lcd-kit,panel-round-corner-top-rc-pattern",
		&array_data);
	if (ret == LCD_KIT_FAIL) {
		LCD_KIT_ERR("parse round-corner-top-rc-pattern fail\n");
		return;
	}
	if (array_data.cnt == 0) {
		if (array_data.buf != NULL)
			free(array_data.buf);
		return;
	}
	pinfo->round_corner.lt_addr = (void *)malloc(array_data.cnt);
	if (pinfo->round_corner.lt_addr == NULL) {
		LCD_KIT_ERR("alloc buf fail\n");
		if (array_data.buf != NULL)
			free(array_data.buf);
		return;
	}
	memset(pinfo->round_corner.lt_addr, 0, array_data.cnt);
	tmp = (unsigned char *)pinfo->round_corner.lt_addr;
	for (i = 0; i < array_data.cnt; i++)
		*(tmp + i) = (unsigned char)(array_data.buf[i]);
	pinfo->round_corner.tp_size = array_data.cnt;
}
#endif

int lcd_kit_utils_init(struct mtk_panel_info *pinfo, const void *fdt, int nodeoffset)
{
	int ret = LCD_KIT_OK;

	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is NULL!\n");
		return LCD_KIT_FAIL;
	}
	if (fdt == NULL) {
		LCD_KIT_ERR("fdt is NULL!\n");
		return LCD_KIT_FAIL;
	}
	/* common init */
	lcd_kit_common_info_init(fdt, nodeoffset);
	/* pinfo init */
	lcd_kit_pinfo_init(fdt, nodeoffset, pinfo);
	/* parse panel dts */
	lcd_kit_parse_disp_info(fdt, nodeoffset);
#ifdef MTK_ROUND_CORNER_SUPPORT
	/* parse hw round coener dts */
	lcd_kit_parse_round_corner_info(fdt, nodeoffset, pinfo);
#endif
	return ret;
}
