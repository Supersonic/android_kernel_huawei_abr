/*
 * lcd_kit_utils.c
 *
 * lcdkit display utils for lcd driver
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
#include "lcd_kit_panels.h"
#include "lcd_kit_power.h"
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN9A0)
#include "dsc/dsc_algorithm_manager.h"
#endif
#include "lcd_kit_ext_disp.h"

#define MAX_TRY_TIMES 100
#define MAX_CMD_LEN   256
#define MAX_CMD_DLEN  256
static char *g_main_panel_dts_path;
static char *g_ext_panel_dts_path;
static char g_main_panel_barcode[LCD_KIT_SNCODE_SIZE];
static char g_ext_panel_barcode[LCD_KIT_SNCODE_SIZE];

static void lcd_kit_orise2x(struct hisi_panel_info *pinfo)
{
	pinfo->ifbc_cmp_dat_rev0 = 1;
	pinfo->ifbc_cmp_dat_rev1 = 0;
	pinfo->ifbc_auto_sel = 0;
}

static void lcd_kit_vesa3x_config(struct hisi_panel_info *pinfo)
{
	/* dsc parameter info */
	pinfo->vesa_dsc.bits_per_component = 8;
	pinfo->vesa_dsc.bits_per_pixel = 8;
	pinfo->vesa_dsc.initial_xmit_delay = 512;
	pinfo->vesa_dsc.first_line_bpg_offset = 12;
	pinfo->vesa_dsc.mux_word_size = 48;
	/* DSC_CTRL */
	pinfo->vesa_dsc.block_pred_enable = 1;
	pinfo->vesa_dsc.linebuf_depth = 9;
	/* RC_PARAM3 */
	pinfo->vesa_dsc.initial_offset = 6144;
	/* FLATNESS_QP_TH */
	pinfo->vesa_dsc.flatness_min_qp = 3;
	pinfo->vesa_dsc.flatness_max_qp = 12;
	/* DSC_PARAM4 */
	pinfo->vesa_dsc.rc_edge_factor = 0x6;
	pinfo->vesa_dsc.rc_model_size = 8192;
	/* DSC_RC_PARAM5: 0x330b0b */
	pinfo->vesa_dsc.rc_tgt_offset_lo = (0x330b0b >> 20) & 0xF;
	pinfo->vesa_dsc.rc_tgt_offset_hi = (0x330b0b >> 16) & 0xF;
	pinfo->vesa_dsc.rc_quant_incr_limit1 = (0x330b0b >> 8) & 0x1F;
	pinfo->vesa_dsc.rc_quant_incr_limit0 = (0x330b0b >> 0) & 0x1F;
	/* DSC_RC_BUF_THRESH0: 0xe1c2a38 */
	pinfo->vesa_dsc.rc_buf_thresh0 = (0xe1c2a38 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh1 = (0xe1c2a38 >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh2 = (0xe1c2a38 >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh3 = (0xe1c2a38 >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH1: 0x46546269 */
	pinfo->vesa_dsc.rc_buf_thresh4 = (0x46546269 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh5 = (0x46546269 >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh6 = (0x46546269 >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh7 = (0x46546269 >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH2: 0x7077797b */
	pinfo->vesa_dsc.rc_buf_thresh8 = (0x7077797b >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh9 = (0x7077797b >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh10 = (0x7077797b >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh11 = (0x7077797b >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH3: 0x7d7e0000 */
	pinfo->vesa_dsc.rc_buf_thresh12 = (0x7d7e0000 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh13 = (0x7d7e0000 >> 16) & 0xFF;
	/* DSC_RC_RANGE_PARAM0: 0x1020100 */
	pinfo->vesa_dsc.range_min_qp0 = (0x1020100 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp0 = (0x1020100 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset0 = (0x1020100 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp1 = (0x1020100 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp1 = (0x1020100 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset1 = (0x1020100 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM1: 0x94009be */
	pinfo->vesa_dsc.range_min_qp2 = (0x94009be >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp2 = (0x94009be >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset2 = (0x94009be >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp3 = (0x94009be >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp3 = (0x94009be >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset3 = (0x94009be >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM2, 0x19fc19fa */
	pinfo->vesa_dsc.range_min_qp4 = (0x19fc19fa >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp4 = (0x19fc19fa >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset4 = (0x19fc19fa >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp5 = (0x19fc19fa >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp5 = (0x19fc19fa >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset5 = (0x19fc19fa >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM3, 0x19f81a38 */
	pinfo->vesa_dsc.range_min_qp6 = (0x19f81a38 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp6 = (0x19f81a38 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset6 = (0x19f81a38 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp7 = (0x19f81a38 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp7 = (0x19f81a38 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset7 = (0x19f81a38 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM4, 0x1a781ab6 */
	pinfo->vesa_dsc.range_min_qp8 = (0x1a781ab6 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp8 = (0x1a781ab6 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset8 = (0x1a781ab6 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp9 = (0x1a781ab6 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp9 = (0x1a781ab6 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset9 = (0x1a781ab6 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM5, 0x2af62b34 */
	pinfo->vesa_dsc.range_min_qp10 = (0x2af62b34 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp10 = (0x2af62b34 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset10 = (0x2af62b34 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp11 = (0x2af62b34 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp11 = (0x2af62b34 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset11 = (0x2af62b34 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM6, 0x2b743b74 */
	pinfo->vesa_dsc.range_min_qp12 = (0x2b743b74 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp12 = (0x2b743b74 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset12 = (0x2b743b74 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp13 = (0x2b743b74 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp13 = (0x2b743b74 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset13 = (0x2b743b74 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM7, 0x6bf40000 */
	pinfo->vesa_dsc.range_min_qp14 = (0x6bf40000 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp14 = (0x6bf40000 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset14 = (0x6bf40000 >> 16) & 0x3F;
}

static void lcd_kit_vesa3_75_single_config(struct hisi_panel_info *pinfo)
{
	/* dsc parameter info */
	pinfo->vesa_dsc.bits_per_component = 10;
	pinfo->vesa_dsc.bits_per_pixel = 8;
	pinfo->vesa_dsc.initial_xmit_delay = 512;
	pinfo->vesa_dsc.first_line_bpg_offset = 12;
	pinfo->vesa_dsc.mux_word_size = 48;
	/* DSC_CTRL */
	pinfo->vesa_dsc.block_pred_enable = 1;
	pinfo->vesa_dsc.linebuf_depth = 11;
	/* RC_PARAM3 */
	pinfo->vesa_dsc.initial_offset = 6144;
	/* FLATNESS_QP_TH */
	pinfo->vesa_dsc.flatness_min_qp = 7;
	pinfo->vesa_dsc.flatness_max_qp = 16;
	/* DSC_PARAM4 */
	pinfo->vesa_dsc.rc_edge_factor = 0x6;
	pinfo->vesa_dsc.rc_model_size = 8192;
	/* DSC_RC_PARAM5: 0x330f0f */
	pinfo->vesa_dsc.rc_tgt_offset_lo = (0x330f0f >> 20) & 0xF;
	pinfo->vesa_dsc.rc_tgt_offset_hi = (0x330f0f >> 16) & 0xF;
	pinfo->vesa_dsc.rc_quant_incr_limit1 = (0x330f0f >> 8) & 0x1F;
	pinfo->vesa_dsc.rc_quant_incr_limit0 = (0x330f0f >> 0) & 0x1F;
	/* DSC_RC_BUF_THRESH0: 0xe1c2a38 */
	pinfo->vesa_dsc.rc_buf_thresh0 = (0xe1c2a38 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh1 = (0xe1c2a38 >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh2 = (0xe1c2a38 >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh3 = (0xe1c2a38 >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH1: 0x46546269 */
	pinfo->vesa_dsc.rc_buf_thresh4 = (0x46546269 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh5 = (0x46546269 >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh6 = (0x46546269 >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh7 = (0x46546269 >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH2: 0x7077797b */
	pinfo->vesa_dsc.rc_buf_thresh8 = (0x7077797b >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh9 = (0x7077797b >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh10 = (0x7077797b >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh11 = (0x7077797b >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH3: 0x7d7e0000 */
	pinfo->vesa_dsc.rc_buf_thresh12 = (0x7d7e0000 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh13 = (0x7d7e0000 >> 16) & 0xFF;
	/* DSC_RC_RANGE_PARAM0: 0x2022200 */
	pinfo->vesa_dsc.range_min_qp0 = (0x2022200 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp0 = (0x2022200 >> 22) & 0x1F;;
	pinfo->vesa_dsc.range_bpg_offset0 = (0x2022200 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp1 = (0x2022200 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp1 = (0x2022200 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset1 = (0x2022200 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM1: 0x2a402abe */
	pinfo->vesa_dsc.range_min_qp2 = (0x2a402abe >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp2 = (0x2a402abe >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset2 = (0x2a402abe >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp3 = (0x2a402abe >> 11) & 0x1F;;
	pinfo->vesa_dsc.range_max_qp3 = (0x2a402abe >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset3 = (0x2a402abe >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM2, 0x3afc3afa */
	pinfo->vesa_dsc.range_min_qp4 = (0x3afc3afa >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp4 = (0x3afc3afa >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset4 = (0x3afc3afa >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp5 = (0x3afc3afa >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp5 = (0x3afc3afa >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset5 = (0x3afc3afa >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM3, 0x3af83b38 */
	pinfo->vesa_dsc.range_min_qp6 = (0x3af83b38 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp6 = (0x3af83b38 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset6 = (0x3af83b38 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp7 = (0x3af83b38 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp7 = (0x3af83b38 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset7 = (0x3af83b38 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM4, 0x3b783bb6 */
	pinfo->vesa_dsc.range_min_qp8 = (0x3b783bb6 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp8 = (0x3b783bb6 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset8 = (0x3b783bb6 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp9 = (0x3b783bb6 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp9 = (0x3b783bb6 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset9 = (0x3b783bb6 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM5, 0x4bf64c34 */
	pinfo->vesa_dsc.range_min_qp10 = (0x4bf64c34 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp10 = (0x4bf64c34 >> 22) & 0x1F;;
	pinfo->vesa_dsc.range_bpg_offset10 = (0x4bf64c34 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp11 = (0x4bf64c34 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp11 = (0x4bf64c34 >> 6) & 0x1F;;
	pinfo->vesa_dsc.range_bpg_offset11 = (0x4bf64c34 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM6, 0x4c745c74 */
	pinfo->vesa_dsc.range_min_qp12 = (0x4c745c74 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp12 = (0x4c745c74 >> 22) & 0x1F;;
	pinfo->vesa_dsc.range_bpg_offset12 = (0x4c745c74 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp13 = (0x4c745c74 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp13 = (0x4c745c74 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset13 = (0x4c745c74 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM7, 0x8cf40000 */
	pinfo->vesa_dsc.range_min_qp14 = (0x8cf40000 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp14 = (0x8cf40000 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset14 = (0x8cf40000 >> 16) & 0x3F;
}

static void lcd_kit_vesa3x_single(struct hisi_panel_info *pinfo)
{
	lcd_kit_vesa3x_config(pinfo);
}

static void lcd_kit_vesa3x_dual(struct hisi_panel_info *pinfo)
{
	lcd_kit_vesa3x_config(pinfo);
}

static void lcd_kit_vesa3_75x_single(struct hisi_panel_info *pinfo)
{
	if (!pinfo) {
		LCD_KIT_ERR("pinfo is null\n");
		return;
	}
	lcd_kit_vesa3_75_single_config(pinfo);
}

static void lcd_kit_vesa3_75x_dual(struct hisi_panel_info *pinfo)
{
	pinfo->vesa_dsc.bits_per_component = 10;
	pinfo->vesa_dsc.linebuf_depth = 11;
	pinfo->vesa_dsc.bits_per_pixel = 8;
	pinfo->vesa_dsc.initial_xmit_delay = 512;
	pinfo->vesa_dsc.first_line_bpg_offset = 12;
	pinfo->vesa_dsc.mux_word_size = 48;
	/* DSC_CTRL */
	pinfo->vesa_dsc.block_pred_enable = 1;
	/* RC_PARAM3 */
	pinfo->vesa_dsc.initial_offset = 6144;
	/* FLATNESS_QP_TH */
	pinfo->vesa_dsc.flatness_min_qp = 7;
	pinfo->vesa_dsc.flatness_max_qp = 16;
	/* DSC_PARAM4 */
	pinfo->vesa_dsc.rc_edge_factor = 0x6;
	pinfo->vesa_dsc.rc_model_size = 8192;
	/* DSC_RC_PARAM5: 0x330f0f */
	pinfo->vesa_dsc.rc_tgt_offset_lo = (0x330f0f >> 20) & 0xF;
	pinfo->vesa_dsc.rc_tgt_offset_hi = (0x330f0f >> 16) & 0xF;
	pinfo->vesa_dsc.rc_quant_incr_limit1 = (0x330f0f >> 8) & 0x1F;
	pinfo->vesa_dsc.rc_quant_incr_limit0 = (0x330f0f >> 0) & 0x1F;
	/* DSC_RC_BUF_THRESH0: 0xe1c2a38 */
	pinfo->vesa_dsc.rc_buf_thresh0 = (0xe1c2a38 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh1 = (0xe1c2a38 >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh2 = (0xe1c2a38 >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh3 = (0xe1c2a38 >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH1: 0x46546269 */
	pinfo->vesa_dsc.rc_buf_thresh4 = (0x46546269 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh5 = (0x46546269 >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh6 = (0x46546269 >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh7 = (0x46546269 >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH2: 0x7077797b */
	pinfo->vesa_dsc.rc_buf_thresh8 = (0x7077797b >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh9 = (0x7077797b >> 16) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh10 = (0x7077797b >> 8) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh11 = (0x7077797b >> 0) & 0xFF;
	/* DSC_RC_BUF_THRESH3: 0x7d7e0000 */
	pinfo->vesa_dsc.rc_buf_thresh12 = (0x7d7e0000 >> 24) & 0xFF;
	pinfo->vesa_dsc.rc_buf_thresh13 = (0x7d7e0000 >> 16) & 0xFF;
	/* DSC_RC_RANGE_PARAM0: 0x2022200 */
	pinfo->vesa_dsc.range_min_qp0 = (0x2022200 >> 27) & 0x1F;
	pinfo->vesa_dsc.range_max_qp0 = (0x2022200 >> 22) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset0 = (0x2022200 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp1 = (0x2022200 >> 11) & 0x1F;
	pinfo->vesa_dsc.range_max_qp1 = (0x2022200 >> 6) & 0x1F;
	pinfo->vesa_dsc.range_bpg_offset1 = (0x2022200 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM1: 0x94009be */
	pinfo->vesa_dsc.range_min_qp2 = 5;
	pinfo->vesa_dsc.range_max_qp2 = 9;
	pinfo->vesa_dsc.range_bpg_offset2 = (0x94009be >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp3 = 5;
	pinfo->vesa_dsc.range_max_qp3 = 10;
	pinfo->vesa_dsc.range_bpg_offset3 = (0x94009be >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM2, 0x19fc19fa */
	pinfo->vesa_dsc.range_min_qp4 = 7;
	pinfo->vesa_dsc.range_max_qp4 = 11;
	pinfo->vesa_dsc.range_bpg_offset4 = (0x19fc19fa >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp5 = 7;
	pinfo->vesa_dsc.range_max_qp5 = 11;
	pinfo->vesa_dsc.range_bpg_offset5 = (0x19fc19fa >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM3, 0x19f81a38 */
	pinfo->vesa_dsc.range_min_qp6 = 7;
	pinfo->vesa_dsc.range_max_qp6 = 11;
	pinfo->vesa_dsc.range_bpg_offset6 = (0x19f81a38 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp7 = 7;
	pinfo->vesa_dsc.range_max_qp7 = 12;
	pinfo->vesa_dsc.range_bpg_offset7 = (0x19f81a38 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM4, 0x1a781ab6 */
	pinfo->vesa_dsc.range_min_qp8 = 7;
	pinfo->vesa_dsc.range_max_qp8 = 13;
	pinfo->vesa_dsc.range_bpg_offset8 = (0x1a781ab6 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp9 = 7;
	pinfo->vesa_dsc.range_max_qp9 = 14;
	pinfo->vesa_dsc.range_bpg_offset9 = (0x1a781ab6 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM5, 0x2af62b34 */
	pinfo->vesa_dsc.range_min_qp10 = 9;
	pinfo->vesa_dsc.range_max_qp10 = 15;
	pinfo->vesa_dsc.range_bpg_offset10 = (0x2af62b34 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp11 = 9;
	pinfo->vesa_dsc.range_max_qp11 = 16;
	pinfo->vesa_dsc.range_bpg_offset11 = (0x2af62b34 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM6, 0x2b743b74 */
	pinfo->vesa_dsc.range_min_qp12 = 9;
	pinfo->vesa_dsc.range_max_qp12 = 17;
	pinfo->vesa_dsc.range_bpg_offset12 = (0x2b743b74 >> 16) & 0x3F;
	pinfo->vesa_dsc.range_min_qp13 = 11;
	pinfo->vesa_dsc.range_max_qp13 = 17;
	pinfo->vesa_dsc.range_bpg_offset13 = (0x2b743b74 >> 0) & 0x3F;
	/* DSC_RC_RANGE_PARAM7, 0x6bf40000 */
	pinfo->vesa_dsc.range_min_qp14 = 17;
	pinfo->vesa_dsc.range_max_qp14 = 19;
	pinfo->vesa_dsc.range_bpg_offset14 = (0x6bf40000 >> 16) & 0x3F;
}

static void lcd_kit_dsc_config(const char *np,
	struct hisi_panel_info *pinfo)
{
	if (!pinfo || !np) {
		LCD_KIT_ERR("pinfo or np is null\n");
		return;
	}
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN9A0)
	struct dsc_algorithm_manager *pt = get_dsc_algorithm_manager_instance();
	struct input_dsc_info panel_input_dsc_info;

	if (!pinfo || !pt || !pt->vesa_dsc_info_calc) {
		LCD_KIT_ERR("pt is null\n");
		return;
	}

	memset(&panel_input_dsc_info, 0, sizeof(struct input_dsc_info));

	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-version",
		&panel_input_dsc_info.dsc_version, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-format",
		&panel_input_dsc_info.format, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-bpp",
		&panel_input_dsc_info.dsc_bpp, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-bpc",
		&panel_input_dsc_info.dsc_bpc, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-blk-pred-en",
		&panel_input_dsc_info.block_pred_enable, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-line-buff-depth",
		&panel_input_dsc_info.linebuf_depth, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-gen-rc-params",
		&panel_input_dsc_info.gen_rc_params, 0);
	panel_input_dsc_info.pic_width = pinfo->xres;
	panel_input_dsc_info.pic_height = pinfo->yres;
	panel_input_dsc_info.slice_width = pinfo->vesa_dsc.slice_width + 1;
	panel_input_dsc_info.slice_height = pinfo->vesa_dsc.slice_height + 1;
	pt->vesa_dsc_info_calc(&panel_input_dsc_info,
		&(pinfo->panel_dsc_info.dsc_info), NULL);
#endif
}

void lcd_kit_compress_config(int mode, struct hisi_panel_info *pinfo)
{
	if (!pinfo) {
		LCD_KIT_ERR("pinfo is null\n");
		return;
	}
	switch (mode) {
	case IFBC_TYPE_ORISE2X:
		lcd_kit_orise2x(pinfo);
		break;
	case IFBC_TYPE_VESA3X_SINGLE:
		if (!disp_info->calc_mode)
			lcd_kit_vesa3x_single(pinfo);
		else
			lcd_kit_dsc_config(disp_info->lcd_name, pinfo);
		break;
	case IFBC_TYPE_VESA3_75X_DUAL:
		lcd_kit_vesa3_75x_dual(pinfo);
		break;
	case IFBC_TYPE_VESA3_75X_SINGLE:
		lcd_kit_vesa3_75x_single(pinfo);
		break;
	case IFBC_TYPE_VESA3X_DUAL:
		if (!disp_info->calc_mode)
			lcd_kit_vesa3x_dual(pinfo);
		else
			lcd_kit_dsc_config(disp_info->lcd_name, pinfo);
		break;
	case IFBC_TYPE_VESA4X_SINGLE_SPR:
	case IFBC_TYPE_VESA4X_DUAL_SPR:
	case IFBC_TYPE_VESA2X_SINGLE_SPR:
	case IFBC_TYPE_VESA2X_DUAL_SPR:
		lcd_kit_dsc_config(disp_info->lcd_name, pinfo);
		break;
	case IFBC_TYPE_NONE:
		break;
	default:
		LCD_KIT_ERR("not support compress mode:%d\n", mode);
		break;
	}
}

int lcd_kit_dsi_fifo_is_empty(uint32_t dsi_base)
{
	uint32_t pkg_status;
	uint32_t phy_status;
	int is_timeout = 1000;

	/* read status register */
	do {
		pkg_status = inp32(dsi_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
		phy_status = inp32(dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
		if ((pkg_status & 0x1) == 0x1 && !(phy_status & 0x2))
			break;
		mdelay(1);
	} while ((is_timeout--) > 0);

	if (!is_timeout) {
		LCD_KIT_ERR("mipi check empty fail:\n");
		LCD_KIT_ERR("MIPIDSI_CMD_PKT_STATUS = 0x%x\n",
			inp32(dsi_base + MIPIDSI_CMD_PKT_STATUS_OFFSET));
		LCD_KIT_ERR("MIPIDSI_PHY_STATUS = 0x%x\n",
			inp32(dsi_base + MIPIDSI_PHY_STATUS_OFFSET));
		LCD_KIT_ERR("MIPIDSI_INT_ST1_OFFSET = 0x%x\n",
			inp32(dsi_base + MIPIDSI_INT_ST1_OFFSET));
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_dsi_fifo_is_full(uint32_t dsi_base)
{
	uint32_t pkg_status;
	uint32_t phy_status;
	int is_timeout = 1000;

	/* read status register */
	do {
		pkg_status = inp32(dsi_base + MIPIDSI_CMD_PKT_STATUS_OFFSET);
		phy_status = inp32(dsi_base + MIPIDSI_PHY_STATUS_OFFSET);
		if ((pkg_status & 0x2) != 0x2 && !(phy_status & 0x2))
			break;
		mdelay(1);
	} while ((is_timeout--) > 0);

	if (!is_timeout) {
		LCD_KIT_ERR("mipi check full fail:\n");
		LCD_KIT_ERR("MIPIDSI_CMD_PKT_STATUS = 0x%x\n",
			inp32(dsi_base + MIPIDSI_CMD_PKT_STATUS_OFFSET));
		LCD_KIT_ERR("MIPIDSI_PHY_STATUS = 0x%x\n",
			inp32(dsi_base + MIPIDSI_PHY_STATUS_OFFSET));
		LCD_KIT_ERR("MIPIDSI_INT_ST1_OFFSET = 0x%x\n",
			inp32(dsi_base + MIPIDSI_INT_ST1_OFFSET));
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

void lcd_kit_read_power_status(struct hisi_fb_data_type *hisifd)
{
	uint32_t status;
	uint32_t try_times = 0;
	uint32_t dsi_base_addr;

	dsi_base_addr = lcd_kit_get_dsi_base_addr(hisifd);
	outp32(dsi_base_addr + MIPIDSI_GEN_HDR_OFFSET, 0x0A06);
	status = inp32(dsi_base_addr + MIPIDSI_CMD_PKT_STATUS_OFFSET);
	while (status & 0x10) {
		udelay(50);
		if ((++try_times) > MAX_TRY_TIMES) {
			try_times = 0;
			LCD_KIT_ERR("Read lcd power status timeout!\n");
			break;
		}

		status = inp32(dsi_base_addr + MIPIDSI_CMD_PKT_STATUS_OFFSET);
	}
	status = inp32(dsi_base_addr + MIPIDSI_GEN_PLD_DATA_OFFSET);
	LCD_KIT_INFO("Fastboot LCD Power State = 0x%x\n", status);
	if (is_dual_mipi_panel(hisifd)) {
		outp32(hisifd->mipi_dsi1_base + MIPIDSI_GEN_HDR_OFFSET, 0x0A06);
		status = inp32(hisifd->mipi_dsi1_base +
			MIPIDSI_CMD_PKT_STATUS_OFFSET);
		try_times = 0;
		while (status & 0x10) {
			udelay(50);
			if ((++try_times) > MAX_TRY_TIMES) {
				try_times = 0;
				LCD_KIT_ERR("Read dsi1 power status timeout\n");
				break;
			}
			status = inp32(hisifd->mipi_dsi1_base +
				MIPIDSI_CMD_PKT_STATUS_OFFSET);
		}
		status = inp32(hisifd->mipi_dsi1_base +
			MIPIDSI_GEN_PLD_DATA_OFFSET);
		LCD_KIT_INFO("Fastboot DSI1 LCD Power State = 0x%x\n", status);
	}
}

int lcd_kit_pwm_set_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level)
{
	return hisi_pwm_set_backlight(hisifd, bl_level);
}

int lcd_kit_sh_blpwm_set_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level)
{
	return hisi_sh_blpwm_set_backlight(hisifd, bl_level);
}

int lcd_kit_get_tp_color(struct hisi_fb_data_type *hisifd)
{
	int ret = LCD_KIT_OK;
	uint8_t read_value = 0;
	struct hisi_panel_info *pinfo = NULL;

	pinfo = hisifd->panel_info;
	if (disp_info->tp_color.support) {
		ret = lcd_kit_dsi_cmds_rx(hisifd, &read_value, 1,
			&disp_info->tp_color.cmds);
		if (ret)
			pinfo->tp_color = 0;
		else
			pinfo->tp_color = read_value;
		LCD_KIT_INFO("tp color = %d\n", pinfo->tp_color);
	} else {
		LCD_KIT_INFO("Not support tp color\n");
	}
	return ret;
}

int lcd_kit_get_elvss_info(struct hisi_fb_data_type *hisifd)
{
	int ret = LCD_KIT_OK;
	uint8_t read_value = 0;
	struct hisi_panel_info *pinfo = NULL;

	if (hisifd == NULL) {
		LCD_KIT_ERR("param hisifd is null, err\n");
		return LCD_KIT_FAIL;
	}
	pinfo = hisifd->panel_info;
	if (pinfo == NULL) {
		LCD_KIT_ERR("param pinfo is null, err\n");
		return LCD_KIT_FAIL;
	}
	pinfo->elvss_dim_val = LCD_KIT_ELVSS_DIM_DEFAULT;

	if (disp_info->elvss.support) {
		ret = lcd_kit_dsi_cmds_rx((void *)hisifd, &read_value, 1,
			&disp_info->elvss.elvss_read_cmds);
		if (ret < 0) {
			LCD_KIT_ERR("elvss_read_cmds send err\n");
			return ret;
		}
		pinfo->elvss_dim_val = read_value;
		LCD_KIT_INFO("read in lcd_kit_get_elvss_info: 0x%x\n",
			pinfo->elvss_dim_val);
		if (disp_info->elvss.set_elvss_lp_support) {
			ret = lcd_kit_dsi_cmds_tx((void *)hisifd,
				&disp_info->elvss.elvss_prepare_cmds);
			if (ret < 0) {
				LCD_KIT_ERR("elvss_prepare_cmds send err\n");
				return ret;
			}
			disp_info->elvss.elvss_write_cmds.cmds[0].payload[1] =
				read_value | LCD_KIT_ELVSS_DIM_ENABLE_MASK;
			ret = lcd_kit_dsi_cmds_tx((void *)hisifd,
				&disp_info->elvss.elvss_write_cmds);
			if (ret < 0) {
				LCD_KIT_ERR("elvss_write_cmds send err\n");
				return ret;
			}
			ret = lcd_kit_dsi_cmds_tx((void *)hisifd,
				&disp_info->elvss.elvss_post_cmds);
			if (ret < 0) {
				LCD_KIT_ERR("elvss_post_cmds send err\n");
				return ret;
			}
		}
	} else {
		LCD_KIT_INFO("Not support elvss dim\n");
	}
	return ret;
}

int lcd_kit_panel_version_init(struct hisi_fb_data_type *hisifd)
{
	int ret;
	struct hisi_panel_info *pinfo = NULL;

	if (hisifd == NULL) {
		LCD_KIT_ERR("hisifd is null\n");
		return LCD_KIT_FAIL;
	}
	pinfo = hisifd->panel_info;
	if (pinfo == NULL) {
		LCD_KIT_ERR("param pinfo is null, err\n");
		return LCD_KIT_FAIL;
	}

	if (disp_info->panel_version.support) {
		if (disp_info->panel_version.enter_cmds.cmds != NULL) {
			ret = lcd_kit_dsi_cmds_tx(hisifd,
				&disp_info->panel_version.enter_cmds);
			if (ret) {
				LCD_KIT_ERR("tx cmd fail\n");
				return LCD_KIT_FAIL;
			}
		}

		ret = lcd_kit_dsi_cmds_rx(hisifd,
			(uint8_t *)disp_info->panel_version.read_value,
			VERSION_VALUE_NUM_MAX,
			&disp_info->panel_version.cmds);
		if (ret) {
			LCD_KIT_ERR("cmd fail\n");
			return LCD_KIT_FAIL;
		}
		if (disp_info->panel_version.exit_cmds.cmds != NULL) {
			ret = lcd_kit_dsi_cmds_tx(hisifd,
				&disp_info->panel_version.exit_cmds);
			if (ret) {
				LCD_KIT_ERR("exit cmd fail\n");
				return LCD_KIT_FAIL;
			}
		}
		ret = lcd_panel_version_compare(hisifd);
		if (ret) {
			LCD_KIT_ERR("panel_version_compare fail\n");
			return LCD_KIT_FAIL;
		}
		return LCD_KIT_OK;
	} else {
		LCD_KIT_INFO("panelVersion fail\n");
	}
	return LCD_KIT_FAIL;
}

int lcd_panel_version_compare(struct hisi_fb_data_type *hisifd)
{
	int i;
	int j;
	char expect_value[VERSION_VALUE_NUM_MAX] = { 0 };
	struct lcd_kit_array_data *arry_data = NULL;
	struct hisi_panel_info *pinfo = NULL;

	if (hisifd == NULL) {
		LCD_KIT_ERR("hisifd is null\n");
		return LCD_KIT_FAIL;
	}
	pinfo = hisifd->panel_info;
	if (pinfo == NULL) {
		LCD_KIT_ERR("param pinfo is null, err\n");
		return LCD_KIT_FAIL;
	}
	arry_data = disp_info->panel_version.value.arry_data;
	if (arry_data == NULL) {
		LCD_KIT_ERR("param arry_data is null, err\n");
		return LCD_KIT_FAIL;
	}
	if (disp_info->panel_version.value_number > VERSION_VALUE_NUM_MAX) {
		LCD_KIT_ERR("Length is greater than array length\n");
		return LCD_KIT_FAIL;
	}
	for (i = 0; i < (int)disp_info->panel_version.version_number; i++) {
		for (j = 0;
			j < (int)disp_info->panel_version.value_number; j++)
			expect_value[j] = arry_data[i].buf[j];
		if (memcmp(disp_info->panel_version.read_value, expect_value,
			disp_info->panel_version.value_number))
			continue;
		if (!strncmp(
			disp_info->panel_version.lcd_version_name[i], "V3",
			strlen(disp_info->panel_version.lcd_version_name[i])))
			pinfo->lcd_panel_version = VER_V3;
		else if (!strncmp(
			disp_info->panel_version.lcd_version_name[i], "V4",
			strlen(disp_info->panel_version.lcd_version_name[i])))
			pinfo->lcd_panel_version = VER_V4;
		else if (!strncmp(
			disp_info->panel_version.lcd_version_name[i], "VN1",
			strlen(disp_info->panel_version.lcd_version_name[i])))
			pinfo->lcd_panel_version = VER_VN1;
		else
			pinfo->lcd_panel_version = VER_VN2;
		LCD_KIT_INFO("Panel version is %d\n", pinfo->lcd_panel_version);
		return LCD_KIT_OK;
	}

	if (i == (int)disp_info->panel_version.version_number) {
		LCD_KIT_INFO("panel_version not find\n");
		return LCD_KIT_FAIL;
	}

	return LCD_KIT_FAIL;
}

uint32_t lcd_kit_get_backlight_type(struct hisi_panel_info *pinfo)
{
	if (pinfo->bl_set_type & BL_SET_BY_PWM)
		return BL_SET_BY_PWM;
	if (pinfo->bl_set_type & BL_SET_BY_BLPWM)
		return BL_SET_BY_BLPWM;
	if (pinfo->bl_set_type & BL_SET_BY_SH_BLPWM)
		return BL_SET_BY_SH_BLPWM;
	if (pinfo->bl_set_type & BL_SET_BY_MIPI)
		return BL_SET_BY_MIPI;
	else
		return BL_SET_BY_NONE;
}

uint32_t lcd_kit_blpwm_set_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level)
{
	int ret = LCD_KIT_OK;
	static bool already_enable = FALSE;

	if (disp_info->rgbw.support) {
		/* 8 means 8 bits */
		disp_info->rgbw.backlight_cmds.cmds->payload[1] =
			(REG61H_VALUE_FOR_RGBW >> 8) & 0x0f;
		disp_info->rgbw.backlight_cmds.cmds->payload[2] =
			(char)(REG61H_VALUE_FOR_RGBW & 0xff);
		ret = lcd_kit_dsi_cmds_tx(hisifd,
			&disp_info->rgbw.backlight_cmds);
		if (ret)
			LCD_KIT_ERR("write backlight_cmds fail\n");
	}
	ret = hisi_blpwm_set_bl(hisifd, bl_level);

	if (power_hdl->lcd_backlight.buf[POWER_MODE] == REGULATOR_MODE) {
		/* enable or disable backlight */
		if (bl_level == 0 && already_enable) {
			lcd_kit_charger_ctrl(LCD_KIT_BL, DISABLE);
			already_enable = FALSE;
		} else if (!already_enable) {
			lcd_kit_charger_ctrl(LCD_KIT_BL, ENABLE);
			already_enable = TRUE;
		}
	}
	return ret;
}

int lcd_kit_set_mipi_backlight(struct hisi_fb_data_type *hisifd,
	uint32_t bl_level)
{
	static bool already_enable = FALSE;
	int ret = 0;

	if (common_ops->set_mipi_backlight)
		common_ops->set_mipi_backlight(hisifd, bl_level);
	if (power_hdl->lcd_backlight.buf[POWER_MODE] == REGULATOR_MODE) {
		if (bl_level == 0 && already_enable) {
			ret = lcd_kit_charger_ctrl(LCD_KIT_BL, DISABLE);
			already_enable = FALSE;
		} else if (!already_enable) {
			ret = lcd_kit_charger_ctrl(LCD_KIT_BL, ENABLE);
			already_enable = TRUE;
		}
	}
	return ret;
}

char *lcd_kit_get_default_compatible(uint32_t product_id, uint32_t lcd_id)
{
	uint32_t i;
	char *product_name = NULL;

	/* get product name */
	for (i = 0; i < ARRAY_SIZE(product_map); ++i) {
		if (product_map[i].board_id == product_id) {
			product_name = product_map[i].product_name;
			LCD_KIT_INFO("found product name:%s\n", product_name);
			break;
		}
	}
	/* if not found product name, use default compatible */
	if (i >= ARRAY_SIZE(product_map)) {
		LCD_KIT_ERR("not found product name, use default compatible\n");
		return LCD_KIT_DEFAULT_COMPATIBLE;
	}
	/* find compatible from default panel */
	for (i = 0; i < ARRAY_SIZE(default_panel_map); ++i) {
		if (!strncmp(product_name, default_panel_map[i].product_name,
			strlen(product_name)) &&
			(lcd_id == default_panel_map[i].gpio_id)) {
			LCD_KIT_INFO("default_panel_map[%d].compatible = %s\n",
				i, default_panel_map[i].compatible);
			return default_panel_map[i].compatible;
		}
	}
	LCD_KIT_ERR("not found compatible from default panel, use default compatible\n");
	return LCD_KIT_DEFAULT_COMPATIBLE;
}

char *lcd_kit_get_normal_compatible(uint32_t product_id, uint32_t lcd_id)
{
	unsigned int i;

	/* find compatible from normal panel */
	for (i = 0; i < ARRAY_SIZE(lcd_kit_map); ++i) {
		if ((lcd_kit_map[i].board_id == product_id) &&
			(lcd_kit_map[i].gpio_id == lcd_id))
			return lcd_kit_map[i].compatible;
	}
	return LCD_KIT_DEFAULT_COMPATIBLE;
}

char *lcd_kit_get_default_lcdname(uint32_t product_id, uint32_t lcd_id)
{
	uint32_t i;
	char *product_name = NULL;

	/* get product name */
	for (i = 0; i < ARRAY_SIZE(product_map); ++i) {
		if (product_map[i].board_id == product_id) {
			product_name = product_map[i].product_name;
			LCD_KIT_INFO("found product name:%s\n", product_name);
			break;
		}
	}
	/* if not found product name, use default compatible */
	if (i >= ARRAY_SIZE(product_map)) {
		LCD_KIT_ERR("not found product name, use default lcd name\n");
		return LCD_KIT_DEFAULT_PANEL;
	}
	/* find lcd name from default panel */
	for (i = 0; i < ARRAY_SIZE(default_panel_map); ++i) {
		if (!strncmp(product_name, default_panel_map[i].product_name,
			strlen(product_name)) &&
			(lcd_id == default_panel_map[i].gpio_id)) {
			LCD_KIT_INFO("default_panel_map[%d].lcd_name = %s\n",
				i, default_panel_map[i].lcd_name);
			return default_panel_map[i].lcd_name;
		}
	}
	LCD_KIT_ERR("not found lcd name from default panel, use default lcd name\n");
	return LCD_KIT_DEFAULT_PANEL;
}

char *lcd_kit_get_normal_lcdname(uint32_t product_id, uint32_t lcd_id)
{
	unsigned int i;

	/* find compatible from normal panel */
	for (i = 0; i < ARRAY_SIZE(lcd_kit_map); ++i) {
		if ((lcd_kit_map[i].board_id == product_id) &&
			(lcd_kit_map[i].gpio_id == lcd_id))
			return lcd_kit_map[i].lcd_name;
	}
	return LCD_KIT_DEFAULT_PANEL;
}


char *lcd_kit_get_compatible(uint32_t product_id, uint32_t lcd_id, int pin_num)
{
	if ((pin_num == LCD_1_PIN && lcd_id == DEF_LCD_1_PIN) ||
		(pin_num == LCD_2_PIN && lcd_id == DEF_LCD_2_PIN)) {
		/* not insert panel, use default panel */
		return lcd_kit_get_default_compatible(product_id, lcd_id);
	} else {
		/* recognize insert panel */
		return lcd_kit_get_normal_compatible(product_id, lcd_id);
	}
}

char *lcd_kit_get_lcd_name(uint32_t product_id, uint32_t lcd_id, int pin_num)
{
	if ((pin_num == LCD_1_PIN && lcd_id == DEF_LCD_1_PIN) ||
		(pin_num == LCD_2_PIN && lcd_id == DEF_LCD_2_PIN)) {
		/* not insert panel, use default panel */
		return lcd_kit_get_default_lcdname(product_id, lcd_id);
	} else {
		/* recognize insert panel */
		return lcd_kit_get_normal_lcdname(product_id, lcd_id);
	}
}

static void lcd_kit_parse_quickly_sleep_out(const char *np)
{
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,quickly-sleep-out-support",
		&disp_info->quickly_sleep_out.support, 0);
	if (disp_info->quickly_sleep_out.support)
		lcd_kit_parse_get_u32_default(np,
		"lcd-kit,quickly-sleep-out-interval",
		&disp_info->quickly_sleep_out.interval, 0);
}

static void lcd_kit_parse_tp_color(const char *np)
{
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,tp-color-support", &disp_info->tp_color.support, 0);
	if (disp_info->tp_color.support)
		lcd_kit_parse_dcs_cmds(np,
		"lcd-kit,tp-color-cmds",
		"lcd-kit,tp-color-cmds-state",
		&disp_info->tp_color.cmds);
}

static void lcd_parse_brightness_color_coo(const char *np)
{
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,brightness-color-uniform-support",
		&disp_info->brightness_color_uniform.support, 0);
	if (disp_info->brightness_color_uniform.support) {
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,module-sn-cmds",
			"lcd-kit,module-sn-cmds-state",
			&disp_info->brightness_color_uniform.modulesn_cmds);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,equip-id-cmds",
			"lcd-kit,equip-id-cmds-state",
			&disp_info->brightness_color_uniform.equipid_cmds);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,module-manufact-cmds",
			"lcd-kit,module-manufact-cmds-state",
			&disp_info->brightness_color_uniform.modulemanufact_cmds);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,vendor-id-cmds",
			"lcd-kit,vendor-id-cmds-state",
			&disp_info->brightness_color_uniform.vendorid_cmds);
	}
}

static void lcd_kit_parse_aod_param(const char *np)
{
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,sensorhub-aod-support",
		&disp_info->aod.support, 0);
	if (disp_info->aod.support) {
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,ramless-aod",
			&disp_info->aod.ramless_aod, 0);
		if (disp_info->aod.ramless_aod) {
			lcd_kit_parse_get_u8_default(np,
				"lcd-kit,aod-no-need-init",
				&disp_info->aod.aod_no_need_init, 0);
			lcd_kit_parse_get_u8_default(np,
				"lcd-kit,aod-need-reset",
				&disp_info->aod.aod_need_reset, 0);
			lcd_kit_parse_get_u32_default(np,
				"lcd-kit,sensorhub-aod-y-start",
				&disp_info->aod.aod_y_start, 0);
			lcd_kit_parse_get_u32_default(np,
				"lcd-kit,sensorhub-aod-y-end",
				&disp_info->aod.aod_y_end, 0);
			lcd_kit_parse_get_u32_default(np,
				"lcd-kit,sensorhub-aod-y-end-one-bit",
				&disp_info->aod.aod_y_end_one_bit, 0);
		}
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,update-core-clk-support",
			&disp_info->aod.update_core_clk_support, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,aod-bl-level",
			&disp_info->aod.bl_level, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,aod-sensorhub-ulps",
			&disp_info->aod.sensorhub_ulps, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,aod-max-te-len",
			&disp_info->aod.max_te_len, 0);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-enter-aod-cmds",
			"lcd-kit,panel-aod-on-cmds-state",
			&(disp_info->aod.aod_on_cmds));
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,sensorhub-aod-ext-support",
			&disp_info->aod.support_ext, 0);
		if (disp_info->aod.support_ext)
			lcd_kit_parse_dcs_cmds(np,
				"lcd-kit,panel-enter-aod-cmds-ext",
				"lcd-kit,panel-aod-on-cmds-ext-state",
				&(disp_info->aod.aod_on_cmds_ext));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-exit-aod-cmds",
			"lcd-kit,panel-aod-off-cmds-state",
			&(disp_info->aod.aod_off_cmds));
		if (!disp_info->aod.bl_level) {
			lcd_kit_parse_dcs_cmds(np,
				"lcd-kit,panel-aod-high-brightness-cmds",
				"lcd-kit,panel-aod-high-brightness-cmds-state",
				&(disp_info->aod.aod_high_brightness_cmds));
			lcd_kit_parse_dcs_cmds(np,
				"lcd-kit,panel-aod-low-brightness-cmds",
				"lcd-kit,panel-aod-low-brightness-cmds-state",
				&(disp_info->aod.aod_low_brightness_cmds));
		} else {
			lcd_kit_parse_dcs_cmds(np,
				"lcd-kit,panel-aod-level1-bl-cmds",
				"lcd-kit,panel-aod-level1-bl-cmds-state",
				&(disp_info->aod.aod_level1_bl_cmds));
			lcd_kit_parse_dcs_cmds(np,
				"lcd-kit,panel-aod-level2-bl-cmds",
				"lcd-kit,panel-aod-level2-bl-cmds-state",
				&(disp_info->aod.aod_level2_bl_cmds));
			lcd_kit_parse_dcs_cmds(np,
				"lcd-kit,panel-aod-level3-bl-cmds",
				"lcd-kit,panel-aod-level3-bl-cmds-state",
				&(disp_info->aod.aod_level3_bl_cmds));
			lcd_kit_parse_dcs_cmds(np,
				"lcd-kit,panel-aod-level4-bl-cmds",
				"lcd-kit,panel-aod-level4-bl-cmds-state",
				&(disp_info->aod.aod_level4_bl_cmds));
		}
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,aod-lcd-detect-type",
			&disp_info->aod.aod_lcd_detect_type, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,aod-exp-ic-reg-vaule",
			&disp_info->aod.aod_exp_ic_reg_vaule, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,aod-ic-reg-vaule-mask",
			&disp_info->aod.aod_ic_reg_vaule_mask, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,aod-pmic-ctrl-by-ap",
			&disp_info->aod.aod_pmic_ctrl_by_ap, 0);
		/* total 6 param */
		if (disp_info->aod.aod_pmic_ctrl_by_ap)
			lcd_kit_parse_arrays(np,
				"lcd-kit,aod-pmic-cfg-tbl",
				&disp_info->aod.aod_pmic_cfg_tbl, 6);
	}
	if (disp_info->aod.ramless_aod) {
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-aod-hbm-enter-cmds",
			"lcd-kit,panel-aod-hbm-enter-cmds-state",
			&(disp_info->aod.aod_hbm_enter_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-aod-hbm-exit-cmds",
			"lcd-kit,panel-aod-hbm-exit-cmds-state",
			&(disp_info->aod.aod_hbm_exit_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-aod-on-one-bit-cmds",
			"lcd-kit,panel-aod-on-one-bit-cmds-state",
			&(disp_info->aod.one_bit_aod_on_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-aod-on-two-bit-cmds",
			"lcd-kit,panel-aod-on-two-bit-cmds-state",
			&(disp_info->aod.two_bit_aod_on_cmds));
	}
}

static void lcd_kit_parse_hbm_elvss(const char *np)
{
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,hbm-elvss-dim-support",
		&disp_info->elvss.support, 0);
	if (disp_info->elvss.support) {
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,default-elvss-dim-close",
			&disp_info->elvss.default_elvss_dim_close, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,hbm-set-elvss-dim-lp",
			&disp_info->elvss.set_elvss_lp_support, 0);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-elvss-prepare-cmds",
			"lcd-kit,panel-hbm-elvss-prepare-cmds-state",
			&(disp_info->elvss.elvss_prepare_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-elvss-read-cmds",
			"lcd-kit,panel-hbm-elvss-read-cmds-state",
			&(disp_info->elvss.elvss_read_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-elvss-write-cmds",
			"lcd-kit,panel-hbm-elvss-write-cmds-state",
			&(disp_info->elvss.elvss_write_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-elvss-post-cmds",
			"lcd-kit,panel-hbm-elvss-post-cmds-state",
			&(disp_info->elvss.elvss_post_cmds));
	}
}

static void lcd_kit_parse_hbm_fp(const char *np)
{
	u32 temp_fp_prepare_support = 0;

	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,hbm-fp-prepare-support", &temp_fp_prepare_support, 0);
	disp_info->pwm.support = (u8)temp_fp_prepare_support;
	if (disp_info->pwm.support) {
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-fp-prepare-cmds",
			"lcd-kit,panel-hbm-fp-prepare-cmds-state",
			&(disp_info->pwm.hbm_fp_prepare_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-fp-post-cmds",
			"lcd-kit,panel-hbm-fp-post-cmds-state",
			&(disp_info->pwm.hbm_fp_post_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-fp-prepare-cmds-vn1",
			"lcd-kit,panel-hbm-fp-prepare-cmds-vn1-state",
			&(disp_info->pwm.hbm_fp_prepare_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-hbm-fp-post-cmds-vn1",
			"lcd-kit,panel-hbm-fp-post-cmds-vn1-state",
			&(disp_info->pwm.hbm_fp_post_cmds));
	}
}

static void lcd_kit_parse_panel_version(const char *np)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;
	char temp_name[VERSION_INFO_LEN_MAX] = {0};
	char *name[VERSION_NUM_MAX] = { NULL };
	char *temp = NULL;
	int i, ret;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("get adapt_ops error\n");
		return;
	}

	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,panel-version-support",
		&disp_info->panel_version.support, 0);
	if (!(disp_info->panel_version.support)) {
		LCD_KIT_INFO("panel_version not support\n");
		return;
	}
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,panel-version-enter-cmds",
		"lcd-kit,panel-version-enter-cmds-state",
		&disp_info->panel_version.enter_cmds);
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,panel-version-cmds",
		"lcd-kit,panel-version-cmds-state",
		&disp_info->panel_version.cmds);
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,panel-version-exit-cmds",
		"lcd-kit,panel-version-exit-cmds-state",
		&disp_info->panel_version.exit_cmds);
	disp_info->panel_version.value_number =
		disp_info->panel_version.cmds.cmds->dlen -
		disp_info->panel_version.cmds.cmds->payload[1];
	lcd_kit_parse_arrays(np, "lcd-kit,panel-version-value",
		&disp_info->panel_version.value,
		disp_info->panel_version.value_number);
	disp_info->panel_version.version_number =
		disp_info->panel_version.value.cnt;
	LCD_KIT_INFO("value_number = %d version_number = %d\n",
		disp_info->panel_version.value_number,
		disp_info->panel_version.version_number);
	if (disp_info->panel_version.version_number <= 0) {
		LCD_KIT_INFO("lcd_version_number <= 0\n");
		return;
	}
	if (adapt_ops->get_string_by_property) {
		adapt_ops->get_string_by_property(np,
			"lcd-kit,panel-fastboot-version",
			temp_name, VERSION_INFO_LEN_MAX);
		name[0] = strtok_s(temp_name, ",", &temp);
		/* copy first target into global variable */
		ret = strncpy_s(disp_info->panel_version.lcd_version_name[0],
				LCD_PANEL_VERSION_SIZE, name[0],
				LCD_PANEL_VERSION_SIZE - 1);
		if (ret != 0) {
			LCD_KIT_ERR("strncpy_s error\n");
			return;
		}
		for (i = 1; (i < (int)disp_info->panel_version.version_number) &&
			(name[i - 1] != NULL); i++) {
			name[i] = strtok_s(NULL, ",", &temp);
			ret = strncpy_s(disp_info->panel_version.lcd_version_name[i],
				LCD_PANEL_VERSION_SIZE, name[i],
				LCD_PANEL_VERSION_SIZE - 1);
			if (ret != 0) {
				LCD_KIT_ERR("strncpy_s error\n");
				return;
			}
			LCD_KIT_INFO("lcd_version_name[%d] = %s\n", i, name[i]);
		}
	}
}

static void lcd_kit_parse_aod_version(const char *np)
{
	struct lcd_kit_adapt_ops *adapt_ops = NULL;

	adapt_ops = lcd_kit_get_adapt_ops();
	if (!adapt_ops) {
		LCD_KIT_ERR("get adapt_ops error\n");
		return;
	}

	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,aod-version-support",
		&disp_info->aod_version.support, 0);
	if (!(disp_info->aod_version.support)) {
		LCD_KIT_INFO("aod_version not support\n");
		return;
	}
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,aod-version-enter-cmds",
		"lcd-kit,aod-version-enter-cmds-state",
		&disp_info->aod_version.enter_cmds);
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,aod-version-cmds",
		"lcd-kit,aod-version-cmds-state",
		&disp_info->aod_version.cmds);
	disp_info->aod_version.value_number =
		disp_info->aod_version.cmds.cmds->dlen;
	lcd_kit_parse_arrays(np, "lcd-kit,aod-version-value",
		&disp_info->aod_version.value,
		disp_info->aod_version.value_number);
	disp_info->aod_version.version_number =
		disp_info->aod_version.value.cnt;
	LCD_KIT_INFO("value_number = %d aod_version_number = %d\n",
		disp_info->aod_version.value_number,
		disp_info->aod_version.version_number);
}

static void lcd_kit_parse_hbm(const char *np)
{
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,hbm-special-bit-ctrl-support",
		&disp_info->hbm.hbm_special_bit_ctrl, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,hbm-fp-support",
		&disp_info->hbm.support, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,aod-hbm-wait-te-times",
		&disp_info->hbm.hbm_wait_te_times, 0);
	if (disp_info->hbm.support) {
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,hbm-enter-cmds",
			"lcd-kit,hbm-enter-cmds-state",
			&disp_info->hbm.enter_cmds);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,hbm-prepare-cmds",
			"lcd-kit,hbm-prepare-cmds-state",
			&disp_info->hbm.hbm_prepare_cmds);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,hbm-cmds",
			"lcd-kit,hbm-cmds-state",
			&disp_info->hbm.hbm_cmds);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,hbm-post-cmds",
			"lcd-kit,hbm-post-cmds-state",
			&disp_info->hbm.hbm_post_cmds);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,enter-dim-cmds",
			"lcd-kit,enter-dim-cmds-state",
			&disp_info->hbm.enter_dim_cmds);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,exit-dim-cmds",
			"lcd-kit,exit-dim-cmds-state",
			&disp_info->hbm.exit_dim_cmds);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,hbm-exit-cmds",
			"lcd-kit,hbm-exit-cmds-state",
			&disp_info->hbm.exit_cmds);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,hbm-fp-enter-cmds",
			"lcd-kit,hbm-fp-enter-cmds-state",
			&disp_info->hbm.fp_enter_cmds);
	}

}

static void lcd_kit_parse_logo_checksum(const char *np)
{
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,logo-checksum-support",
		&disp_info->logo_checksum.support, 0);
	if (disp_info->logo_checksum.support) {
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,checksum-enable-cmds",
			"lcd-kit,checksum-enable-cmds-state",
			&disp_info->logo_checksum.enable_cmds);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,checksum-disable-cmds",
			"lcd-kit,checksum-disable-cmds-state",
			&disp_info->logo_checksum.disable_cmds);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,checksum-cmds",
			"lcd-kit,checksum-cmds-state",
			&disp_info->logo_checksum.checksum_cmds);
		lcd_kit_parse_array(np,
			"lcd-kit,logo-checksum-value",
			&disp_info->logo_checksum.value);
	}
}

static void lcd_kit_parse_elvdd_detect(const char *np)
{
	/* elvdd detect */
	lcd_kit_parse_get_u32_default(np, "lcd-kit,elvdd-detect-support",
		&disp_info->elvdd_detect.support, 0);
	if (disp_info->elvdd_detect.support) {
		lcd_kit_parse_get_u32_default(np, "lcd-kit,elvdd-detect-type",
			&disp_info->elvdd_detect.detect_type, 0);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,elvdd-detect-cmds",
			"lcd-kit,elvdd-detect-cmds-state",
			&disp_info->elvdd_detect.cmds);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,elvdd-detect-gpio",
			&disp_info->elvdd_detect.detect_gpio, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,elvdd-detect-gpio-type",
			&disp_info->elvdd_detect.detect_gpio_type, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,elvdd-detect-value",
			&disp_info->elvdd_detect.exp_value, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,elvdd-value-mask",
			&disp_info->elvdd_detect.exp_value_mask, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,elvdd-delay",
			&disp_info->elvdd_detect.delay, 0);
	}
}

static void lcd_kit_parse_project_id(const char *compatible)
{
	struct lcd_kit_adapt_ops *adpat_ops = NULL;

	adpat_ops = lcd_kit_get_adapt_ops();
	if (!adpat_ops) {
		LCD_KIT_ERR("get adpat_ops error\n");
		return;
	}
	/* project id */
	lcd_kit_parse_get_u32_default(compatible, "lcd-kit,project-id-support",
		&disp_info->project_id.support, 0);
	if (disp_info->project_id.support) {
		lcd_kit_parse_dcs_cmds(compatible, "lcd-kit,project-id-cmds",
			"lcd-kit,project-id-cmds-state",
			&disp_info->project_id.cmds);
		if (adpat_ops->get_string_by_property)
			adpat_ops->get_string_by_property(compatible,
				"lcd-kit,default-project-id",
				disp_info->project_id.default_project_id,
				(PROJECTID_LEN + 1));
	}
}

static void lcd_kit_parse_poweric_adapt(const char *np)
{
	int ret;
	uint32_t gpio_val;

	lcd_kit_parse_get_u32_default(np, "lcd-kit,pnl-on-cmd-bak-support",
		&common_info->panel_cmd_backup.panel_on_support, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,aod-cmd-bak-support",
		&common_info->panel_cmd_backup.aod_support, 0);
	if (!common_info->panel_cmd_backup.panel_on_support &&
		!common_info->panel_cmd_backup.aod_support)
		return;

	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,pnl-cmd-bak-det-gpio",
		&common_info->panel_cmd_backup.detect_gpio, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,pnl-cmd-bak-gpio-exp-val",
		&common_info->panel_cmd_backup.gpio_exp_val, 0xFF);
	ret = lcd_kit_get_gpio_value(
		common_info->panel_cmd_backup.detect_gpio, &gpio_val);
	if (ret != LCD_KIT_OK)
		return;
	if (common_info->panel_cmd_backup.gpio_exp_val != gpio_val) {
		LCD_KIT_INFO("panel_cmd_backup gpio %u\n", gpio_val);
		return;
	}

	if (common_info->panel_cmd_backup.panel_on_support) {
		LCD_KIT_INFO("panel_on_cmd_backup gpio %u\n", gpio_val);
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,panel-on-cmds-backup",
				"lcd-kit,panel-on-cmds-state", &common_info->panel_on_cmds);
	}

	if (disp_info->aod.support &&
		common_info->panel_cmd_backup.aod_support) {
		LCD_KIT_INFO("aod_cmd_backup gpio %u\n", gpio_val);
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-enter-aod-cmds-bak",
			"lcd-kit,panel-aod-on-cmds-state",
			&(disp_info->aod.aod_on_cmds));
		lcd_kit_parse_dcs_cmds(np,
			"lcd-kit,panel-exit-aod-cmds-bak",
			"lcd-kit,panel-aod-off-cmds-state",
			&(disp_info->aod.aod_off_cmds));
	}
}

static void lcd_kit_parse_check_otp(const char *compatible)
{
	lcd_kit_parse_get_u32_default(compatible, "lcd-kit,check-otp-support",
		&disp_info->otp.support, 0);
	if (disp_info->otp.support) {
		lcd_kit_parse_get_u32_default(compatible, "lcd-kit,otp-value",
			&disp_info->otp.otp_value, 0);
		lcd_kit_parse_get_u32_default(compatible, "lcd-kit,otp-mask",
			&disp_info->otp.otp_mask, 0);
		lcd_kit_parse_get_u32_default(compatible, "lcd-kit,osc-value",
			&disp_info->otp.osc_value, 0);
		lcd_kit_parse_get_u32_default(compatible, "lcd-kit,osc-mask",
			&disp_info->otp.osc_mask, 0);
		lcd_kit_parse_get_u32_default(compatible, "lcd-kit,tx-delay",
			&disp_info->otp.tx_delay, 0);
		lcd_kit_parse_dcs_cmds(compatible,
			"lcd-kit,otp-read-cmds",
			"lcd-kit,otp-read-cmds-state",
			&disp_info->otp.read_otp_cmds);
		lcd_kit_parse_dcs_cmds(compatible,
			"lcd-kit,osc-read-cmds",
			"lcd-kit,osc-read-cmds-state",
			&disp_info->otp.read_osc_cmds);
		lcd_kit_parse_dcs_cmds(compatible,
			"lcd-kit,otp-flow-cmds",
			"lcd-kit,otp-flow-cmds-state",
			&disp_info->otp.otp_flow_cmds);
		lcd_kit_parse_dcs_cmds(compatible,
			"lcd-kit,otp-end-cmds",
			"lcd-kit,otp-end-cmds-state",
			&disp_info->otp.otp_end_cmds);
	}
}

static int lcd_kit_parse_disp_info(const char *np)
{
	lcd_kit_parse_quickly_sleep_out(np);
	lcd_kit_parse_tp_color(np);
	lcd_parse_brightness_color_coo(np);
	lcd_kit_parse_aod_param(np);
	lcd_kit_parse_hbm_elvss(np);
	lcd_kit_parse_hbm_fp(np);
	lcd_kit_parse_panel_version(np);
	lcd_kit_parse_aod_version(np);
	lcd_kit_parse_hbm(np);
	lcd_kit_parse_logo_checksum(np);
	lcd_kit_parse_elvdd_detect(np);
	lcd_kit_parse_project_id(np);
	lcd_kit_parse_poweric_adapt(np);
	lcd_kit_parse_check_otp(np);
	return LCD_KIT_OK;
}

static void lcd_kit_vesa_para_parse(const char *np,
	struct hisi_panel_info *pinfo)
{
	if (!np) {
		LCD_KIT_ERR("compatible is null\n");
		return;
	}
	if (!pinfo) {
		LCD_KIT_ERR("pinfo is null\n");
		return;
	}

	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-slice-width",
		&pinfo->vesa_dsc.slice_width, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-slice-height",
		&pinfo->vesa_dsc.slice_height, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-calc-mode",
		&disp_info->calc_mode, 0);
#if !(defined(FASTBOOT_DISPLAY_LOGO_ORLANDO) || defined(FASTBOOT_DISPLAY_LOGO_KIRIN980))
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-native-422",
		&pinfo->vesa_dsc.native_422, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,vesa-dsc-version",
		&pinfo->vesa_dsc.dsc_version, 0);
#endif
}

static void lcd_dsi_tx_lp_mode_cfg(uint32_t dsi_base)
{
	/*
	 * gen short cmd read switch low-power,
	 * include 0-parameter,1-parameter,2-parameter
	 */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x7, 3, 8);
	/* gen long cmd write switch low-power */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x1, 1, 14);
	/*
	 * dcs short cmd write switch high-speed,
	 * include 0-parameter,1-parameter
	 */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x3, 2, 16);
}

static void lcd_dsi_tx_hs_mode_cfg(uint32_t dsi_base)
{
	/*
	 * gen short cmd read switch low-power,
	 * include 0-parameter,1-parameter,2-parameter
	 */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 3, 8);
	/* gen long cmd write switch high-speed */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 1, 14);
	/*
	 * dcs short cmd write switch high-speed,
	 * include 0-parameter,1-parameter
	 */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 2, 16);
}

static void lcd_dsi_rx_lp_mode_cfg(uint32_t dsi_base)
{
	/*
	 * gen short cmd read switch low-power,
	 * include 0-parameter,1-parameter,2-parameter
	 */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x7, 3, 11);
	/* dcs short cmd read switch low-power */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x1, 1, 18);
	/* read packet size cmd switch low-power */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x1, 1, 24);
}

static void lcd_dsi_rx_hs_mode_cfg(uint32_t dsi_base)
{
	/*
	 * gen short cmd read switch high-speed,
	 * include 0-parameter,1-parameter,2-parameter
	 */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 3, 11);
	/* dcs short cmd read switch high-speed */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 1, 18);
	/* read packet size cmd switch high-speed */
	set_reg(dsi_base + MIPIDSI_CMD_MODE_CFG_OFFSET, 0x0, 1, 24);
}

void lcd_kit_set_mipi_link(struct hisi_fb_data_type *hisifd,
	int link_state)
{
	uint32_t dsi_base_addr;
	struct hisi_panel_info *pinfo = NULL;

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		return;
	}
	pinfo = hisifd->panel_info;
	if (!pinfo) {
		LCD_KIT_ERR("panel_info is NULL!\n");
		return;
	}
	if (pinfo->lcd_init_step == LCD_INIT_MIPI_LP_SEND_SEQUENCE) {
		LCD_KIT_INFO("lowpower stage, can not set!\n");
		return;
	}

	dsi_base_addr = lcd_kit_get_dsi_base_addr(hisifd);
	if (is_mipi_cmd_panel(hisifd)) {
		/* wait fifo cmd empty */
		lcd_kit_dsi_fifo_is_empty(dsi_base_addr);
		if (is_dual_mipi_panel(hisifd))
			lcd_kit_dsi_fifo_is_empty(hisifd->mipi_dsi1_base);
		LCD_KIT_INFO("link_state:%d\n", link_state);
		switch (link_state) {
		case LCD_KIT_DSI_LP_MODE:
			lcd_dsi_tx_lp_mode_cfg(dsi_base_addr);
			lcd_dsi_rx_lp_mode_cfg(dsi_base_addr);
			if (is_dual_mipi_panel(hisifd)) {
				lcd_dsi_tx_lp_mode_cfg(hisifd->mipi_dsi1_base);
				lcd_dsi_rx_lp_mode_cfg(hisifd->mipi_dsi1_base);
			}
			break;
		case LCD_KIT_DSI_HS_MODE:
			lcd_dsi_tx_hs_mode_cfg(dsi_base_addr);
			lcd_dsi_rx_hs_mode_cfg(dsi_base_addr);
			if (is_dual_mipi_panel(hisifd)) {
				lcd_dsi_tx_hs_mode_cfg(hisifd->mipi_dsi1_base);
				lcd_dsi_rx_hs_mode_cfg(hisifd->mipi_dsi1_base);
			}
			break;
		default:
			LCD_KIT_ERR("not support mode\n");
			break;
		}
	}
}

#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
static int change_dts_value_hilegacy(char *compatible, char *dts_name, u32 value)
{
	struct fdt_operators *fdt_ops = NULL;
	struct dtb_operators *dtimage_ops = NULL;
	struct fdt_property *property = NULL;
	void *fdt = NULL;
	int ret;
	int offset;
	int len = 0;
	uint32_t temp;

	if (!compatible || !dts_name) {
		LCD_KIT_ERR("null pointer found!\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return LCD_KIT_FAIL;
	}
	dtimage_ops = get_operators(DTIMAGE_MODULE_NAME_STR);
	if (!dtimage_ops) {
		LCD_KIT_ERR("failed to get dtimage operators!\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops->fdt_operate_lock();
	fdt = dtimage_ops->get_dtb_addr();
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_node_offset_by_compatible(fdt, 0, compatible);
	if (ret < 0) {
		LCD_KIT_ERR("Could not find node:%s, change fb dts failed\n",
			compatible);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	offset = ret;
	property = (struct fdt_property *)fdt_ops->fdt_get_property(fdt, offset,
		dts_name, &len);
	if (!property) {
		LCD_KIT_ERR("-----can not find %s\n", dts_name);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	temp = *property->data;
	temp = (uint32_t)fdt32_to_cpu(temp);
	ret = fdt_ops->fdt_setprop_u32(fdt, offset,
		(const char *)dts_name, (value | temp));
	if (ret) {
		LCD_KIT_ERR("Cannot update dts feild:%s errno=%d!\n",
			dts_name, ret);
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	fdt_ops->fdt_operate_unlock();
	return LCD_KIT_OK;
}
#endif

static int change_dts_value_hifastboot(char *compatible, char *dts_name, u32 value)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return LCD_KIT_OK;
#else
	struct fdt_operators *fdt_ops = NULL;
	struct fdt_property *property = NULL;
	void *fdt = NULL;
	int ret;
	int offset;
	int len = 0;
	uint32_t temp;

	if (!compatible || !dts_name) {
		LCD_KIT_ERR("null pointer found!\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return LCD_KIT_FAIL;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_node_offset_by_compatible(fdt, 0, compatible);
	if (ret < 0) {
		LCD_KIT_ERR("Could not find node:%s, change fb dts failed\n",
			compatible);
		return LCD_KIT_FAIL;
	}
	offset = ret;
	property = (struct fdt_property *)fdt_ops->fdt_get_property(fdt, offset,
		dts_name, &len);
	if (!property) {
		LCD_KIT_ERR("-----can not find %s\n", dts_name);
		return LCD_KIT_FAIL;
	}
	temp = *property->data;
	temp = (uint32_t)fdt32_to_cpu(temp);
	ret = fdt_ops->fdt_setprop_u32(fdt, offset,
		(const char *)dts_name, (value | temp));
	if (ret) {
		LCD_KIT_ERR("Cannot update dts feild:%s errno=%d!\n",
			dts_name, ret);
		return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
#endif
}

int lcd_kit_change_dts_value(char *compatible, char *dts_name, u32 value)
{
	int ret;
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	ret = change_dts_value_hilegacy(compatible, dts_name, value);
#else
	ret = change_dts_value_hifastboot(compatible, dts_name, value);
#endif
	return ret;
}

#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
static void set_property_str_hilegacy(const char *path, const char *dts_name,
	const char *str)
{
	struct fdt_operators *fdt_ops = NULL;
	struct dtb_operators *dtimage_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;

	if (!path || !dts_name || !str) {
		LCD_KIT_ERR("pointer is null\n");
		return;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return;
	}
	dtimage_ops = get_operators(DTIMAGE_MODULE_NAME_STR);
	if (!dtimage_ops) {
		LCD_KIT_ERR("failed to get dtimage operators!\n");
		return;
	}
	fdt_ops->fdt_operate_lock();
	fdt = dtimage_ops->get_dtb_addr();
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		fdt_ops->fdt_operate_unlock();
		return;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		fdt_ops->fdt_operate_unlock();
		return;
	}
	ret = fdt_ops->fdt_path_offset(fdt, path);
	if (ret < 0) {
		LCD_KIT_ERR("Could not find panel node, change fb dts failed\n");
		fdt_ops->fdt_operate_unlock();
		return;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, dts_name,
		(const void *)str);
	if (ret)
		LCD_KIT_ERR("Cannot update lcd panel type, ret=%d\n", ret);
	fdt_ops->fdt_operate_unlock();
}
#endif

static void set_property_str_hifastboot(const char *path, const char *dts_name,
	const char *str)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	return;
#else
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;

	if (!path || !dts_name || !str) {
		LCD_KIT_ERR("pointer is null\n");
		return;
	}
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (!fdt_ops) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return;
	}
	fdt = fdt_ops->get_dt_handle(FW_DTB_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return;
	}
	ret = fdt_ops->fdt_path_offset(fdt, path);
	if (ret < 0) {
		LCD_KIT_ERR("Could not find panel node, change fb dts failed\n");
		return;
	}
	offset = ret;
	ret = fdt_ops->fdt_setprop_string(fdt, offset, dts_name,
		(const void *)str);
	if (ret)
		LCD_KIT_ERR("Cannot update lcd panel type, ret=%d\n", ret);
#endif
}

void lcd_kit_set_property_str(const char *path, const char *dts_name, const char *str)
{
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	set_property_str_hilegacy(path, dts_name, str);
#else
	set_property_str_hifastboot(path, dts_name, str);
#endif
}

static void lcd_kit_aod_cmd_init(struct dsi_cmd_desc **pdst_cmds,
	struct lcd_kit_dsi_panel_cmds *psrc_cmds)
{
	uint8_t index;
	uint8_t j;
	struct dsi_cmd_desc *temp_cmds = NULL;

	if ((!psrc_cmds) || (!pdst_cmds)) {
		LCD_KIT_ERR("psrc_cmds is null\n");
		return;
	}

	LCD_KIT_INFO("cmd_cnt is %d\n", psrc_cmds->cmd_cnt);
	if ((psrc_cmds->cmd_cnt == 0) || (psrc_cmds->cmd_cnt > MAX_CMD_LEN)) {
		LCD_KIT_ERR("cmd_cnt is invalid\n");
		return;
	}

	if (!(psrc_cmds->cmds)) {
		LCD_KIT_ERR("psrc_cmds->cmds is null\n");
		return;
	}
	temp_cmds = alloc(psrc_cmds->cmd_cnt * sizeof(struct dsi_cmd_desc));
	if (!temp_cmds) {
		LCD_KIT_ERR("alloc fail\n");
		return;
	}
	for (index = 0; index < psrc_cmds->cmd_cnt; index++) {
		temp_cmds[index].dtype     = psrc_cmds->cmds[index].dtype;
		temp_cmds[index].vc        = psrc_cmds->cmds[index].vc;
		temp_cmds[index].wait      = psrc_cmds->cmds[index].wait;
		temp_cmds[index].waittype  = psrc_cmds->cmds[index].waittype;
		temp_cmds[index].dlen      = psrc_cmds->cmds[index].dlen;
		if (psrc_cmds->cmds[index].dlen <= 0) {
			LCD_KIT_ERR("cmds[%d].dlen is invalid\n", index);
			return;
		}
		temp_cmds[index].payload   = alloc(psrc_cmds->cmds[index].dlen *
			sizeof(char));
		if (!(temp_cmds[index].payload))
			continue;
		for (j = 0; j < (psrc_cmds->cmds[index].dlen); j++)
			temp_cmds[index].payload[j] =
				psrc_cmds->cmds[index].payload[j];
	}
	*pdst_cmds = temp_cmds;
}

/*
 * Initialize related parameters sent to sensorhub
 */
static void lcd_kit_aod_init(struct hisi_panel_info *pinfo)
{
	static uint32_t gpio_vci;
	static uint32_t gpio_te0;
	static uint32_t gpio_reset;
	uint32_t vdd_ctrl_mode = 0;

	if (!pinfo) {
		LCD_KIT_ERR("pinfo is null\n");
		return;
	}

	/* discharge gpio */
	if (power_hdl->lcd_aod.buf != NULL)
		pinfo->dis_gpio = power_hdl->lcd_aod.buf[1];

	/* normal on */
	pinfo->disp_on_cmds_len = common_info->panel_on_cmds.cmd_cnt;
	lcd_kit_aod_cmd_init(&(pinfo->disp_on_cmds),
		&(common_info->panel_on_cmds));

	/* normal off */
	pinfo->disp_off_cmds_len = common_info->panel_off_cmds.cmd_cnt;
	lcd_kit_aod_cmd_init(&(pinfo->disp_off_cmds),
		&(common_info->panel_off_cmds));

	/* aod on */
	pinfo->aod_enter_cmds_len = disp_info->aod.aod_on_cmds.cmd_cnt;
	lcd_kit_aod_cmd_init(&(pinfo->aod_enter_cmds),
		&(disp_info->aod.aod_on_cmds));

	/* aod off */
	pinfo->aod_exit_cmds_len = disp_info->aod.aod_off_cmds.cmd_cnt;
	lcd_kit_aod_cmd_init(&(pinfo->aod_exit_cmds),
		&(disp_info->aod.aod_off_cmds));

	/* gpio reset */
	if ((power_hdl->lcd_rst.buf) != NULL)
		gpio_reset = power_hdl->lcd_rst.buf[POWER_NUMBER];
	static struct gpio_desc gpio_reset_request_cmds[] = {
		{ DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_reset", &gpio_reset, 0 },
	};
	pinfo->gpio_request_cmds = gpio_reset_request_cmds;
	pinfo->gpio_request_cmds_len = ARRAY_SIZE(gpio_reset_request_cmds);

	/* pinctrl normal */
	if ((power_hdl->lcd_te0.buf) != NULL)
		gpio_te0 = power_hdl->lcd_te0.buf[POWER_NUMBER];

	static struct gpio_desc pinctrl_normal_cmds[] = {
		/* reset */
		{ DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_reset", &gpio_reset, FUNCTION0 },
		/* te0 */
		{ DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_te0", &gpio_te0, FUNCTION2 },
	};
	pinfo->pinctrl_normal_cmds_len = ARRAY_SIZE(pinctrl_normal_cmds);
	pinfo->pinctrl_normal_cmds = pinctrl_normal_cmds;

	/* pinctrl lowpower */
	static struct gpio_desc pinctrl_low_cmds[] = {
		/* reset */
		{ DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_reset", &gpio_reset, FUNCTION0 },
		/* te0 */
		{ DTYPE_GPIO_PMX, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_te0", &gpio_te0, FUNCTION0 },
		/* te0 input */
		{ DTYPE_GPIO_INPUT, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_te0", &gpio_te0, 0 },
	};
	pinfo->pinctrl_lowpower_cmds_len = ARRAY_SIZE(pinctrl_low_cmds);
	pinfo->pinctrl_lowpower_cmds = pinctrl_low_cmds;

	/* gpio reset normal */
	static struct gpio_desc gpio_reset_normal_cmds[] = {
		/* reset */
		{ DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		"gpio_lcdkit_reset", &gpio_reset, 1 },
		{ DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		"gpio_lcdkit_reset", &gpio_reset, 0 },
		{ DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 15,
		"gpio_lcdkit_reset", &gpio_reset, 1 },
	};
	pinfo->gpio_normal_cmds1_len = ARRAY_SIZE(gpio_reset_normal_cmds);
	pinfo->gpio_normal_cmds1 = gpio_reset_normal_cmds;

	/* gpio reset low */
	static struct gpio_desc gpio_reset_low_cmds[] = {
		/* reset */
		{ DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 5,
		"gpio_lcdkit_reset", &gpio_reset, 0 },
	};
	pinfo->gpio_lowpower_cmds1_len = ARRAY_SIZE(gpio_reset_low_cmds);
	pinfo->gpio_lowpower_cmds1 = gpio_reset_low_cmds;

	/* gpio reset free */
	static struct gpio_desc gpio_reset_free_cmds[] = {
		/* reset */
		{ DTYPE_GPIO_FREE, WAIT_TYPE_US, 100,
		"gpio_lcdkit_reset", &gpio_reset, 0 },
	};
	pinfo->gpio_lowpower_cmds2_len = ARRAY_SIZE(gpio_reset_free_cmds);
	pinfo->gpio_lowpower_cmds2 = gpio_reset_free_cmds;

	/* aod high brightness */
	pinfo->aod_high_brightness_cmds_len =
		disp_info->aod.aod_high_brightness_cmds.cmd_cnt;
	if (pinfo->aod_high_brightness_cmds_len)
		lcd_kit_aod_cmd_init(&(pinfo->aod_high_brightness_cmds),
			&(disp_info->aod.aod_high_brightness_cmds));

	/* aod low brightness */
	pinfo->aod_low_brightness_cmds_len =
		disp_info->aod.aod_low_brightness_cmds.cmd_cnt;
	if (pinfo->aod_low_brightness_cmds_len)
		lcd_kit_aod_cmd_init(&(pinfo->aod_low_brightness_cmds),
			&(disp_info->aod.aod_low_brightness_cmds));

	/* aod level1 brightness */
	pinfo->aod_bl_level = disp_info->aod.bl_level;
	LCD_KIT_INFO("AOD_BL_level = %d\n", pinfo->aod_bl_level);
	pinfo->aod_bl[0].aod_level_bl_cmds_len =
		disp_info->aod.aod_level1_bl_cmds.cmd_cnt;
	lcd_kit_aod_cmd_init(&(pinfo->aod_bl[0].aod_level_bl_cmds),
		&(disp_info->aod.aod_level1_bl_cmds));
	pinfo->aod_bl[1].aod_level_bl_cmds_len =
		disp_info->aod.aod_level2_bl_cmds.cmd_cnt;
	lcd_kit_aod_cmd_init(&(pinfo->aod_bl[1].aod_level_bl_cmds),
		&(disp_info->aod.aod_level2_bl_cmds));
	pinfo->aod_bl[2].aod_level_bl_cmds_len =
		disp_info->aod.aod_level3_bl_cmds.cmd_cnt;
	lcd_kit_aod_cmd_init(&(pinfo->aod_bl[2].aod_level_bl_cmds),
		&(disp_info->aod.aod_level3_bl_cmds));
	pinfo->aod_bl[3].aod_level_bl_cmds_len =
		disp_info->aod.aod_level4_bl_cmds.cmd_cnt;
	if (pinfo->aod_bl[3].aod_level_bl_cmds_len)
		lcd_kit_aod_cmd_init(&(pinfo->aod_bl[3].aod_level_bl_cmds),
			&(disp_info->aod.aod_level4_bl_cmds));

	/* pwm pulse switch */
	pinfo->hbm_fp_prepare_support = disp_info->pwm.support;
	if (pinfo->hbm_fp_prepare_support) {
		pinfo->hbm_fp_prepare_cmds_len =
			disp_info->pwm.hbm_fp_prepare_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->hbm_fp_prepare_cmds),
			&(disp_info->pwm.hbm_fp_prepare_cmds));
		pinfo->hbm_fp_post_cmds_len =
			disp_info->pwm.hbm_fp_post_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->hbm_fp_post_cmds),
			&(disp_info->pwm.hbm_fp_post_cmds));
		pinfo->hbm_fp_prepare_cmds_vn1_len =
			disp_info->pwm.hbm_fp_prepare_cmds_vn1.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->hbm_fp_prepare_cmds_vn1),
			&(disp_info->pwm.hbm_fp_prepare_cmds_vn1));
		pinfo->hbm_fp_post_cmds_vn1_len =
			disp_info->pwm.hbm_fp_post_cmds_vn1.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->hbm_fp_post_cmds_vn1),
			&(disp_info->pwm.hbm_fp_post_cmds_vn1));
	}

	/* vci enable */
	if (power_hdl->lcd_vci.buf != NULL)
		gpio_vci = power_hdl->lcd_vci.buf[POWER_NUMBER];
	static struct gpio_desc vci_enable_cmds[] = {
		{ DTYPE_GPIO_OUTPUT, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_vci", &gpio_vci, 1 },
	};
	pinfo->gpio_vci_enable_cmds = vci_enable_cmds;
	pinfo->gpio_vci_enable_cmds_len = ARRAY_SIZE(vci_enable_cmds);

	/* vci request */
	static struct gpio_desc vci_request_cmds[] = {
		{ DTYPE_GPIO_REQUEST, WAIT_TYPE_MS, 0,
		"gpio_lcdkit_vci", &gpio_vci, 0 },
	};
	pinfo->gpio_vci_request_cmds = vci_request_cmds;
	pinfo->gpio_vci_request_cmds_len = ARRAY_SIZE(vci_request_cmds);

	/* vdd ctrl mode */
	if ((power_hdl->lcd_vdd.buf) != NULL)
		vdd_ctrl_mode = power_hdl->lcd_vdd.buf[POWER_MODE];
	pinfo->vdd_power_ctrl_mode = vdd_ctrl_mode;
	/* power seq */
	pinfo->power_on_seq.arry_data =
		(struct hisi_array_data *)power_seq->power_on_seq.arry_data;
	pinfo->power_on_seq.cnt = power_seq->power_on_seq.cnt;
	pinfo->power_off_seq.arry_data =
		(struct hisi_array_data *)power_seq->power_off_seq.arry_data;
	pinfo->power_off_seq.cnt = power_seq->power_off_seq.cnt;
	pinfo->panel_on_lp_seq.arry_data =
		(struct hisi_array_data *)power_seq->panel_on_lp_seq.arry_data;
	pinfo->panel_on_lp_seq.cnt = power_seq->panel_on_lp_seq.cnt;
	pinfo->panel_on_hs_seq.arry_data =
		(struct hisi_array_data *)power_seq->panel_on_hs_seq.arry_data;
	pinfo->panel_on_hs_seq.cnt = power_seq->panel_on_hs_seq.cnt;
	pinfo->panel_off_hs_seq.arry_data =
		(struct hisi_array_data *)power_seq->panel_off_hs_seq.arry_data;
	pinfo->panel_off_hs_seq.cnt  = power_seq->panel_off_hs_seq.cnt;
	pinfo->panel_off_lp_seq.arry_data =
		(struct hisi_array_data *)power_seq->panel_off_lp_seq.arry_data;
	pinfo->panel_off_lp_seq.cnt = power_seq->panel_off_lp_seq.cnt;
	/* power config */
	pinfo->lcd_vci.buf   = power_hdl->lcd_vci.buf;
	pinfo->lcd_vci.cnt   = power_hdl->lcd_vci.cnt;
	pinfo->lcd_iovcc.buf = power_hdl->lcd_iovcc.buf;
	pinfo->lcd_iovcc.cnt = power_hdl->lcd_iovcc.cnt;
	pinfo->lcd_vdd.buf   = power_hdl->lcd_vdd.buf;
	pinfo->lcd_vdd.cnt   = power_hdl->lcd_vdd.cnt;
	pinfo->lcd_rst.buf   = power_hdl->lcd_rst.buf;
	pinfo->lcd_rst.cnt   = power_hdl->lcd_rst.cnt;
	pinfo->lcd_aod.buf   = power_hdl->lcd_aod.buf;
	pinfo->lcd_aod.cnt   = power_hdl->lcd_aod.cnt;
	pinfo->lcd_mipi_switch.buf = power_hdl->lcd_mipi_switch.buf;
	pinfo->lcd_mipi_switch.cnt = power_hdl->lcd_mipi_switch.cnt;

	/* elvdd detect */
	pinfo->elvdd_detect.support = disp_info->elvdd_detect.support;
	if (pinfo->elvdd_detect.support) {
		pinfo->elvdd_detect.detect_type =
			disp_info->elvdd_detect.detect_type;
		pinfo->elvdd_detect.detect_gpio =
			disp_info->elvdd_detect.detect_gpio;
		pinfo->elvdd_detect.exp_value =
			disp_info->elvdd_detect.exp_value;
		pinfo->elvdd_detect.exp_value_mask =
			disp_info->elvdd_detect.exp_value_mask;
		lcd_kit_aod_cmd_init(&(pinfo->elvdd_detect.cmds),
			&(disp_info->elvdd_detect.cmds));
		pinfo->elvdd_detect.len =
			disp_info->elvdd_detect.cmds.cmd_cnt;
		pinfo->elvdd_detect.delay =
			disp_info->elvdd_detect.delay;
		LCD_KIT_INFO("pinfo->elvdd_detect.delay = %d\n",
			pinfo->elvdd_detect.delay);
	}
	/* ramless aod */
	pinfo->ramless_aod = disp_info->aod.ramless_aod;
	pinfo->aod_need_reset = disp_info->aod.aod_need_reset;
	pinfo->aod_no_need_init = disp_info->aod.aod_no_need_init;
	if (pinfo->ramless_aod) {
		pinfo->aod_hbm_enter_cmds_len =
			disp_info->aod.aod_hbm_enter_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->aod_hbm_enter_cmds),
			&(disp_info->aod.aod_hbm_enter_cmds));
		pinfo->aod_hbm_exit_cmds_len =
			disp_info->aod.aod_hbm_exit_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->aod_hbm_exit_cmds),
			&(disp_info->aod.aod_hbm_exit_cmds));
		pinfo->one_bit_aod_on_cmds_len =
			disp_info->aod.one_bit_aod_on_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->one_bit_aod_on_cmds),
			&(disp_info->aod.one_bit_aod_on_cmds));
		pinfo->two_bit_aod_on_cmds_len =
			disp_info->aod.two_bit_aod_on_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->two_bit_aod_on_cmds),
			&(disp_info->aod.two_bit_aod_on_cmds));
	}
	/* update_core_clk_support */
	pinfo->update_core_clk_support = disp_info->aod.update_core_clk_support;
	/* 16 means high 16bits of 32bit */
	pinfo->aod_disp_area_y = ((disp_info->aod.aod_y_start & 0xffff) << 16) |
		(disp_info->aod.aod_y_end & 0xffff);
	pinfo->aod_disp_area_y_one_bit = ((disp_info->aod.aod_y_start &
		0xffff) << 16) | (disp_info->aod.aod_y_end_one_bit & 0xffff);

	pinfo->aod_lcd_detect_type = disp_info->aod.aod_lcd_detect_type;
	pinfo->aod_pmic_ctrl_by_ap = disp_info->aod.aod_pmic_ctrl_by_ap;
	pinfo->aod_exp_ic_reg_vaule = disp_info->aod.aod_exp_ic_reg_vaule;
	pinfo->aod_ic_reg_vaule_mask = disp_info->aod.aod_ic_reg_vaule_mask;
	pinfo->sensorhub_ulps = disp_info->aod.sensorhub_ulps;
	pinfo->max_te_len = disp_info->aod.max_te_len;
	if (disp_info->aod.aod_lcd_detect_type == AOD_LCD_DETECT_COMMON_TYPE)
		pinfo->aod_lcd_name = "lcd-aod-common-panel";
	if (disp_info->aod.aod_pmic_ctrl_by_ap) {
		pinfo->aod_pmic_cfg_tbl.arry_data =
			(struct hisi_array_data *)disp_info->aod.aod_pmic_cfg_tbl.arry_data;
		pinfo->aod_pmic_cfg_tbl.cnt = disp_info->aod.aod_pmic_cfg_tbl.cnt;
	}
}

static void lcd_kit_hbm_init(struct hisi_panel_info *pinfo)
{
	if (pinfo == NULL) {
		LCD_KIT_ERR("pinfo is null\n");
		return;
	}

	pinfo->hbm_support = disp_info->hbm.support;
	pinfo->hbm_special_bit_ctrl = disp_info->hbm.hbm_special_bit_ctrl;
	pinfo->hbm_wait_te_times = disp_info->hbm.hbm_wait_te_times;
	LCD_KIT_INFO("pinfo->hbm_support = %d\n", pinfo->hbm_support);
	if (pinfo->hbm_support) {
		pinfo->hbm_prepare_cmds_len =
			disp_info->hbm.hbm_prepare_cmds.cmd_cnt;
		if (pinfo->hbm_prepare_cmds_len)
			lcd_kit_aod_cmd_init(&(pinfo->hbm_prepare_cmds),
				&(disp_info->hbm.hbm_prepare_cmds));
		pinfo->hbm_write_cmds_len = disp_info->hbm.hbm_cmds.cmd_cnt;
		if (pinfo->hbm_write_cmds_len)
			lcd_kit_aod_cmd_init(&(pinfo->hbm_write_cmds),
				&(disp_info->hbm.hbm_cmds));
		pinfo->hbm_post_cmds_len =
			disp_info->hbm.hbm_post_cmds.cmd_cnt;
		if (pinfo->hbm_post_cmds_len)
			lcd_kit_aod_cmd_init(&(pinfo->hbm_post_cmds),
				&(disp_info->hbm.hbm_post_cmds));
		pinfo->hbm_enter_dim_cmds_len =
			disp_info->hbm.enter_dim_cmds.cmd_cnt;
		if (pinfo->hbm_enter_dim_cmds_len)
			lcd_kit_aod_cmd_init(&(pinfo->hbm_enter_dim_cmds),
				&(disp_info->hbm.enter_dim_cmds));
		pinfo->hbm_exit_dim_cmds_len =
			disp_info->hbm.exit_dim_cmds.cmd_cnt;
		if (pinfo->hbm_exit_dim_cmds_len)
			lcd_kit_aod_cmd_init(&(pinfo->hbm_exit_dim_cmds),
				&(disp_info->hbm.exit_dim_cmds));
		pinfo->hbm_fp_write_cmds_len =
			disp_info->hbm.fp_enter_cmds.cmd_cnt;
		if (pinfo->hbm_fp_write_cmds_len)
			lcd_kit_aod_cmd_init(&(pinfo->hbm_fp_write_cmds),
				&(disp_info->hbm.fp_enter_cmds));
	}
	if (disp_info->elvss.default_elvss_dim_close == 1) {
		pinfo->hbm_elvss_support = 0;
		return;
	}
	pinfo->hbm_elvss_support = disp_info->elvss.support;

	if (pinfo->hbm_elvss_support) {
		pinfo->hbm_elvss_prepare_cmds_len =
			disp_info->elvss.elvss_prepare_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->hbm_elvss_prepare_cmds),
			&(disp_info->elvss.elvss_prepare_cmds));
		pinfo->hbm_elvss_write_cmds_len =
			disp_info->elvss.elvss_write_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->hbm_elvss_write_cmds),
			&(disp_info->elvss.elvss_write_cmds));
		pinfo->hbm_elvss_post_cmds_len =
			disp_info->elvss.elvss_post_cmds.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->hbm_elvss_post_cmds),
			&(disp_info->elvss.elvss_post_cmds));
	}
}

static int lcd_kit_get_aod_version(struct hisi_fb_data_type *hisifd,
	int *aod_version)
{
	int ret, i, j;
	char expect_value[AOD_VERSION_VALUE_NUM_MAX] = { 0 };
	struct lcd_kit_array_data *arry_data = NULL;

	if (!disp_info->aod_version.value.arry_data) {
		LCD_KIT_ERR("param arry_data is null, err\n");
		return LCD_KIT_FAIL;
	}
	if (disp_info->aod_version.value_number > AOD_VERSION_VALUE_NUM_MAX) {
		LCD_KIT_ERR("Length is greater than array length\n");
		return LCD_KIT_FAIL;
	}
	/* read register value */
	ret = lcd_kit_dsi_cmds_tx(hisifd,
		&disp_info->aod_version.enter_cmds);
	if (ret) {
		LCD_KIT_ERR("tx enter cmd fail\n");
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_dsi_cmds_rx(hisifd,
		(uint8_t *)disp_info->aod_version.read_value,
		AOD_VERSION_VALUE_NUM_MAX,
		&disp_info->aod_version.cmds);
	LCD_KIT_INFO("aod_version.read_value %x\n",
		disp_info->aod_version.read_value[0]);
	if (ret) {
		LCD_KIT_ERR("tx read cmd fail\n");
		return LCD_KIT_FAIL;
	}
	arry_data = disp_info->aod_version.value.arry_data;
	for (i = 0; i < (int)disp_info->aod_version.version_number; i++) {
		for (j = 0; j < (int)disp_info->aod_version.value_number; j++)
			expect_value[j] = arry_data[i].buf[j];
		if (!memcmp(disp_info->aod_version.read_value, expect_value,
			disp_info->aod_version.value_number))
			*aod_version = AOD_ABNORMAL;
		else
			*aod_version = AOD_NORMAL;
		LCD_KIT_INFO("aod version is %d\n", *aod_version);
		return LCD_KIT_OK;
	}

	LCD_KIT_INFO("lcd_kit_get_aod_version fail\n");
	return LCD_KIT_FAIL;
}

int lcd_kit_aod_extend_cmds_init(struct hisi_fb_data_type *hisifd)
{
	int ret = LCD_KIT_FAIL;
	int aod_version = AOD_NORMAL;
	struct hisi_panel_info *pinfo = NULL;

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		return LCD_KIT_FAIL;
	}
	pinfo = hisifd->panel_info;
	if (!pinfo) {
		LCD_KIT_ERR("param pinfo is null, err\n");
		return LCD_KIT_FAIL;
	}
	LCD_KIT_INFO("+, aod_version.support = %d\n",
		disp_info->aod_version.support);
	if (disp_info->aod_version.support)
		ret = lcd_kit_get_aod_version(hisifd, &aod_version);
	/* aod on ext */
	if ((disp_info->aod.support_ext) && (ret == LCD_KIT_OK) &&
		(aod_version == AOD_ABNORMAL)) {
		LCD_KIT_INFO("use aod extend cmds.\n");
		pinfo->aod_enter_cmds_len =
			disp_info->aod.aod_on_cmds_ext.cmd_cnt;
		lcd_kit_aod_cmd_init(&(pinfo->aod_enter_cmds),
			&(disp_info->aod.aod_on_cmds_ext));
	}
	LCD_KIT_INFO("-\n");
	return LCD_KIT_OK;
}

static void lcd_kit_spr_dsc_parse(const char *np,
	struct hisi_panel_info *pinfo)
{
	struct lcd_kit_array_data spr_lut_table;
	uint8_t mode = pinfo->spr_dsc_mode;

	/* spr and dsc config init */
	if (mode == SPR_DSC_MODE_SPR_ONLY || mode == SPR_DSC_MODE_SPR_AND_DSC) {
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-rgbg2uyvy-8biten",
			&pinfo->spr.spr_rgbg2uyvy_8biten, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-hpartial-mode",
			&pinfo->spr.spr_hpartial_mode, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-partial-mode",
			&pinfo->spr.spr_partial_mode, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-rgbg2uyvy-en",
			&pinfo->spr.spr_rgbg2uyvy_en, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-horzborderdect",
			&pinfo->spr.spr_horzborderdect, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-linebuf-ldmode",
			&pinfo->spr.spr_linebuf_1dmode, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-bordertb-dummymode",
			&pinfo->spr.spr_bordertb_dummymode, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-borderlr-dummymode",
			&pinfo->spr.spr_borderlr_dummymode, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-pattern-mode",
			&pinfo->spr.spr_pattern_mode, 0);
		(void)lcd_kit_parse_get_u8_default(np, "lcd-kit,spr-pattern-en",
			&pinfo->spr.spr_pattern_en, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-subpxl_layout",
			&pinfo->spr.spr_subpxl_layout, 0);
		(void)lcd_kit_parse_get_u8_default(np, "lcd-kit,ck-gt-spr-en",
			&pinfo->spr.ck_gt_spr_en, 0);
		(void)lcd_kit_parse_get_u8_default(np, "lcd-kit,spr-en",
			&pinfo->spr.spr_en, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-coeffsel-even",
			&pinfo->spr.spr_coeffsel_even, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-coeffsel-odd",
			&pinfo->spr.spr_coeffsel_odd, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,pix-panel-arrange-sel",
			&pinfo->spr.pix_panel_arrange_sel, 0);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-r-v0h0-coef",
			pinfo->spr.spr_r_v0h0_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-r-v0h1-coef",
			pinfo->spr.spr_r_v0h1_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-r-v1h0-coef",
			pinfo->spr.spr_r_v1h0_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-r-v1h1-coef",
			pinfo->spr.spr_r_v1h1_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-g-v0h0-coef",
			pinfo->spr.spr_g_v0h0_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-g-v0h1-coef",
			pinfo->spr.spr_g_v0h1_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-g-v1h0-coef",
			pinfo->spr.spr_g_v1h0_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-g-v1h1-coef",
			pinfo->spr.spr_g_v1h1_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-b-v0h0-coef",
			pinfo->spr.spr_b_v0h0_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-b-v0h1-coef",
			pinfo->spr.spr_b_v0h1_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-b-v1h0-coef",
			pinfo->spr.spr_b_v1h0_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-b-v1h1-coef",
			pinfo->spr.spr_b_v1h1_coef, SPR_COLOR_COEF_LEN);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-borderlr-detect-r",
			&pinfo->spr.spr_borderlr_detect_r, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-bordertb-detect-r",
			&pinfo->spr.spr_bordertb_detect_r, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-borderlr-detect-g",
			&pinfo->spr.spr_borderlr_detect_g, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-bordertb-detect-g",
			&pinfo->spr.spr_bordertb_detect_g, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-borderlr-detect-b",
			&pinfo->spr.spr_borderlr_detect_b, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-bordertb-detect-b",
			&pinfo->spr.spr_bordertb_detect_b, 0);
		(void)lcd_kit_parse_get_u64_default(np, "lcd-kit,spr-pix-gain",
			&pinfo->spr.spr_pixgain, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-borderl-position",
			&pinfo->spr.spr_borderl_position, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-borderr-position",
			&pinfo->spr.spr_borderr_position, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-bordert-position",
			&pinfo->spr.spr_bordert_position, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-borderb-position",
			&pinfo->spr.spr_borderb_position, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-r-diff",
			&pinfo->spr.spr_r_diff, 0);
		(void)lcd_kit_parse_get_u64_default(np, "lcd-kit,spr-r-weight",
			&pinfo->spr.spr_r_weight, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-g-diff",
			&pinfo->spr.spr_g_diff, 0);
		(void)lcd_kit_parse_get_u64_default(np, "lcd-kit,spr-g-weight",
			&pinfo->spr.spr_g_weight, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-b-diff",
			&pinfo->spr.spr_b_diff, 0);
		(void)lcd_kit_parse_get_u64_default(np, "lcd-kit,spr-b-weight",
			&pinfo->spr.spr_b_weight, 0);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-rgbg2uyvy-coeff",
			pinfo->spr.spr_rgbg2uyvy_coeff, SPR_CSC_COEF_LEN);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-input-reso",
			&pinfo->spr.input_reso, 0);
		spr_lut_table.buf = NULL;
		spr_lut_table.cnt = 0;
		(void)lcd_kit_parse_array(np, "lcd-kit,spr-lut-table",
			&spr_lut_table);
		pinfo->spr.spr_lut_table = spr_lut_table.buf;
		pinfo->spr.spr_lut_table_len = spr_lut_table.cnt;
	}
	if (mode == SPR_DSC_MODE_DSC_ONLY || mode == SPR_DSC_MODE_SPR_AND_DSC) {
		(void)lcd_kit_parse_get_u8_default(np, "lcd-kit,spr-dsc-enable",
			&pinfo->spr.dsc_enable, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-dsc-slice0-ck-gt-en",
			&pinfo->spr.slice0_ck_gt_en, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-dsc-slice1-ck-gt-en",
			&pinfo->spr.slice1_ck_gt_en, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-rcb-bits",
			&pinfo->spr.rcb_bits, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-dsc-alg-ctrl",
			&pinfo->spr.dsc_alg_ctrl, 0);
		(void)lcd_kit_parse_get_u8_default(np,
			"lcd-kit,spr-bits-per-component",
			&pinfo->spr.bits_per_component, 0);
		(void)lcd_kit_parse_get_u8_default(np, "lcd-kit,spr-dsc-sample",
			&pinfo->spr.dsc_sample, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-bpp-chk",
			&pinfo->spr.bpp_chk, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-pic-reso",
			&pinfo->spr.pic_reso, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-slc-reso",
			&pinfo->spr.slc_reso, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-initial-xmit-delay",
			&pinfo->spr.initial_xmit_delay, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-initial-dec-delay",
			&pinfo->spr.initial_dec_delay, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-initial-scale-value",
			&pinfo->spr.initial_scale_value, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-scale-interval",
			&pinfo->spr.scale_interval, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-first-bpg",
			&pinfo->spr.first_bpg, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-second-bpg",
			&pinfo->spr.second_bpg, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-second-adj",
			&pinfo->spr.second_adj, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-init-finl-ofs",
			&pinfo->spr.init_finl_ofs, 0);
		(void)lcd_kit_parse_get_u32_default(np, "lcd-kit,spr-slc-bpg",
			&pinfo->spr.slc_bpg, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-flat-range",
			&pinfo->spr.flat_range, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-rc-mode-edge",
			&pinfo->spr.rc_mode_edge, 0);
		(void)lcd_kit_parse_get_u32_default(np,
			"lcd-kit,spr-rc-qua-tgt",
			&pinfo->spr.rc_qua_tgt, 0);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-rc-buff-thresh",
			pinfo->spr.rc_buf_thresh, SPR_RC_BUF_THRESH_LEN);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,spr-rc-para",
			pinfo->spr.rc_para, SPR_RC_PARA_LEN);
	}
}

static void lcd_kit_pinfo_init(const char *np,
	struct hisi_panel_info *pinfo)
{
	uint32_t pixel_clk = 0;
	uint32_t dpi0_overlap_size_temp = 0;
	uint32_t dpi1_overlap_size_temp = 0;

	if (!np) {
		LCD_KIT_ERR("compatible is null\n");
		return;
	}
	if (!pinfo) {
		LCD_KIT_ERR("pinfo is null\n");
		return;
	}
	/* panel info */
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-xres",
		&pinfo->xres, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-yres",
		&pinfo->yres, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-width",
		&pinfo->width, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-height",
		&pinfo->height, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-bl-type",
		&pinfo->bl_set_type, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-bl-min",
		&pinfo->bl_min, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-bl-max",
		&pinfo->bl_max, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-bl-boot",
		&pinfo->bl_boot, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,panel-bl-ic-ctrl-type", &pinfo->bl_ic_ctrl_mode, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,blpwm-div",
		&pinfo->blpwm_div, 0);
	lcd_kit_parse_get_u8_default(np, "lcd-kit,bl-work-delay-frame",
		&pinfo->bl_work_delay_frame, 0);
	lcd_kit_parse_get_u8_default(np, "lcd-kit,dpi01-set-change",
		&pinfo->dpi01_exchange_flag, 0);
	lcd_kit_parse_get_u8_default(np, "lcd-kit,pxl-clk-to-pll2",
		&pinfo->pxl_clk_to_pll2, 0);
	lcd_kit_parse_get_u8_default(np, "lcd-kit,pxl-clk-to-pll0",
		&pinfo->pxl_clk_to_pll0, 0);
	lcd_kit_parse_get_u8_default(np, "lcd-kit,panel-mipi-no-round",
		&pinfo->mipi_no_round, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-cmd-type",
		&pinfo->type, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,need-set-dsi1-te-ctrl",
		&pinfo->need_set_dsi1_te_ctrl, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,product-type",
		&pinfo->product_type, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-normal-logo-index",
		&pinfo->normal_logo_index, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-charge-logo-index",
		&pinfo->charge_logo_index, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-low-power-logo-index",
		&pinfo->low_power_logo_index, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-ifbc-type",
		&pinfo->ifbc_type, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-pxl-clk",
		&pixel_clk, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-pxl-clk-div",
		&pinfo->pxl_clk_rate_div, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,h-back-porch",
		&pinfo->ldi.h_back_porch, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,h-front-porch",
		&pinfo->ldi.h_front_porch, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,h-pulse-width",
		&pinfo->ldi.h_pulse_width, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,v-back-porch",
		&pinfo->ldi.v_back_porch, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,v-front-porch",
		&pinfo->ldi.v_front_porch, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,v-pulse-width",
		&pinfo->ldi.v_pulse_width, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-lane-nums",
		&pinfo->mipi.lane_nums, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-dsi-te-type",
		&pinfo->mipi.dsi_te_type, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-color-mode",
		&pinfo->mipi.color_mode, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-vc",
		&pinfo->mipi.vc, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-dsi-bit-clk",
		&pinfo->mipi.dsi_bit_clk, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-max-tx-esc-clk",
		&pinfo->mipi.max_tx_esc_clk, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-burst-mode",
		&pinfo->mipi.burst_mode, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-non-continue-enable",
		&pinfo->mipi.non_continue_en, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-clk-post-adjust",
		&pinfo->mipi.clk_post_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-rg-vrefsel-vcm-adjust",
		&pinfo->mipi.rg_vrefsel_vcm_adjust, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,need-adjust-dsi-vcm",
		&pinfo->need_adjust_dsi_vcm, 0);
	if (pinfo->need_adjust_dsi_vcm) {
		lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-rg-lptx-sri-adjust",
			&pinfo->mipi.rg_lptx_sri_adjust, 0);
		lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-rg-vrefsel-lptx-adjust",
			&pinfo->mipi.rg_vrefsel_lptx_adjust, 0);
	}
	lcd_kit_parse_get_u32_default(np, "lcd-kit,support-de-emphasis",
		&pinfo->mipi.support_de_emphasis, 0);
	if (pinfo->mipi.support_de_emphasis) {
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,de-emphasis-reg",
			pinfo->mipi.de_emphasis_reg, DE_EMPHASIS_REG_NUM);
		(void)lcd_kit_parse_u32_array(np, "lcd-kit,de-emphasis-value",
			pinfo->mipi.de_emphasis_value, DE_EMPHASIS_REG_NUM);
	}
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-phy-mode",
		&pinfo->mipi.phy_mode, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,mipi-lp11-flag",
		&pinfo->mipi.lp11_flag, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,ldi-dpi0-overlap-size",
		&dpi0_overlap_size_temp, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,ldi-dpi1-overlap-size",
		&dpi1_overlap_size_temp, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-data-t-lpx-adjust",
		(uint32_t *)(&pinfo->mipi.data_t_lpx_adjust), 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-clk-t-lpx-adjust",
		(uint32_t *)(&pinfo->mipi.clk_t_lpx_adjust), 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-clk-t-hs-trail-adjust",
		(uint32_t *)&pinfo->mipi.clk_t_hs_trial_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-data-t-hs-trail-adjust",
		(uint32_t *)&pinfo->mipi.data_t_hs_trial_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-data-t-hs-zero-adjust",
		(uint32_t *)&pinfo->mipi.data_t_hs_zero_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-data-t-hs-prepare-adjust",
		(uint32_t *)&pinfo->mipi.data_t_hs_prepare_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-data-lane-lp2hs-time-adjust",
		(uint32_t *)&pinfo->mipi.data_lane_lp2hs_time_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-clk-t-hs-prepare-adjust",
		(uint32_t *)&pinfo->mipi.clk_t_hs_prepare_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-clk-t-hs-exit-adjust",
		(uint32_t *)&pinfo->mipi.clk_t_hs_exit_adjust, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,mipi-cphy-need-adjust-flag",
		(uint32_t *)&pinfo->mipi.mipi_cphy_adjust.need_adjust_cphy_para, 0);
	if (pinfo->mipi.mipi_cphy_adjust.need_adjust_cphy_para) {
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-cphy-data-t-post-adjust",
			(uint32_t *)&pinfo->mipi.mipi_cphy_adjust.cphy_data_t_post_adjust, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-cphy-data-t-pre-adjust",
			(uint32_t *)&pinfo->mipi.mipi_cphy_adjust.cphy_data_t_prepare_adjust, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-cphy-data-t-pre-begin-adjust",
			(uint32_t *)&pinfo->mipi.mipi_cphy_adjust.cphy_data_t_prebegin_adjust, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-cphy-data-t-lpx-adjust",
			(uint32_t *)&pinfo->mipi.mipi_cphy_adjust.cphy_data_t_lpx_adjust, 0);
	}
	lcd_kit_parse_get_u32_default(np, "lcd-kit,cascade-ic-support",
		&pinfo->mipi.cascadeic_support, 0);

	/* lk enter or exit ulps */
	lcd_kit_parse_get_u32_default(np, "lcd-kit,lk-enter-exit-ulps",
		&pinfo->lk_enter_exit_ulps, 0);

	/* spr and dsc config init */
	lcd_kit_parse_get_u8_default(np,
		"lcd-kit,spr-dsc-mode",
		&pinfo->spr_dsc_mode, 0);
	lcd_kit_parse_get_u32_default(np,
		"lcd-kit,dummy-pixel",
		&pinfo->dummy_pixel_num, 0);
	if (pinfo->spr_dsc_mode)
		lcd_kit_spr_dsc_parse(np, pinfo);
	lcd_kit_parse_get_u8_default(np,
		"lcd-kit,min-phy-timing-flag",
		&pinfo->mipi.mininum_phy_timing_flag, 0);
	lcd_kit_parse_get_u8_default(np,
		"lcd-kit,mipi-dsi-timing-support",
		&pinfo->mipi.dsi_timing_support, 0);
	if (pinfo->mipi.dsi_timing_support) {
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-h-sync-area",
			&pinfo->mipi.hsa, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-h-back-porch",
			&pinfo->mipi.hbp, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-h-line-time",
			&pinfo->mipi.hline_time, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-dpi-h-size",
			&pinfo->mipi.dpi_hsize, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-v-sync-area",
			&pinfo->mipi.vsa, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-v-back-porch",
			&pinfo->mipi.vbp, 0);
		lcd_kit_parse_get_u32_default(np,
			"lcd-kit,mipi-v-front-porch",
			&pinfo->mipi.vfp, 0);
	}
	pinfo->ldi.dpi0_overlap_size = (uint8_t)dpi0_overlap_size_temp;
	pinfo->ldi.dpi1_overlap_size = (uint8_t)dpi1_overlap_size_temp;
	pinfo->orientation = 1;
	pinfo->bpp = 0;
	pinfo->bgr_fmt = 0;
	pinfo->ldi.hsync_plr = 0;
	pinfo->ldi.vsync_plr = 0;
	pinfo->ldi.pixelclk_plr = 0;
	pinfo->ldi.data_en_plr = 0;
	pinfo->pxl_clk_rate = pixel_clk * 1000000UL;
	pinfo->lcd_type = LCD_KIT;
	pinfo->lcd_name = disp_info->lcd_name;
	pinfo->mipi.dsi_bit_clk_upt = pinfo->mipi.dsi_bit_clk;
	pinfo->mipi.max_tx_esc_clk = pinfo->mipi.max_tx_esc_clk * 1000000;
	/* aod init */
	pinfo->aod_support = disp_info->aod.support;
	pinfo->aod_bl_level = disp_info->aod.bl_level;
	pinfo->lcd_uninit_step_support = 1;
	if (pinfo->aod_support) {
		lcd_kit_hbm_init(pinfo);
		lcd_kit_aod_init(pinfo);
	}
}

int lcd_kit_parse_dt(const char *np)
{
	int ret;

	if (!np) {
		LCD_KIT_ERR("compatible is null\n");
		return LCD_KIT_FAIL;
	}
	/* parse display info */
	ret = lcd_kit_parse_disp_info(np);
	if (ret)
		LCD_KIT_ERR("parse disp info fail!\n");
	return ret;
}

static void lcd_kit_fps_info_init(const char *np, struct hisi_panel_info *pinfo)
{
	if (!np) {
		LCD_KIT_ERR("compatible is null\n");
		return;
	}
	if (!pinfo) {
		LCD_KIT_ERR("pinfo is null\n");
		return;
	}
	lcd_kit_parse_get_u32_default(np, "lcd-kit,fps-support",
		&disp_info->fps.support, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,panel-dsc-switch",
		&pinfo->dynamic_dsc_en, FPS_CONFIG_EN);
	if (disp_info->fps.support != FPS_CONFIG_EN)
		return;
	lcd_kit_parse_get_u32_default(np, "lcd-kit,fps-ifbc-type",
		&disp_info->fps.fps_ifbc_type, 0);
	if (pinfo->ifbc_type == IFBC_TYPE_NONE &&
		disp_info->fps.fps_ifbc_type != IFBC_TYPE_NONE) {
		pinfo->dynamic_dsc_support = FPS_CONFIG_EN;
		pinfo->dynamic_dsc_ifbc_type = disp_info->fps.fps_ifbc_type;
	}
}

int lcd_kit_utils_init(struct hisi_panel_info *pinfo)
{
	int ret = LCD_KIT_OK;

	if (!pinfo) {
		LCD_KIT_ERR("pinfo is null\n");
		return LCD_KIT_FAIL;
	}
	/* parse panel dts */
	lcd_kit_parse_dt(disp_info->lcd_name);
	/* high fps func dts */
	lcd_kit_fps_info_init(disp_info->lcd_name, pinfo);
	/* pinfo init */
	lcd_kit_pinfo_init(disp_info->lcd_name, pinfo);
	/* parse vesa parameters */
	lcd_kit_vesa_para_parse(disp_info->lcd_name, pinfo);
	/* config compress setting */
	lcd_kit_compress_config(pinfo->ifbc_type, pinfo);
	if (disp_info->dynamic_gamma_support) {
		ret = lcd_kit_write_gm_to_reserved_mem();
		if (ret < LCD_KIT_OK)
			LCD_KIT_ERR("Write gamma to shared memory failed!\n");
	}
	return ret;
}

void lcd_logo_checksum_set(struct hisi_fb_data_type *hisifd)
{
	int ret;

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		return;
	}
	if (disp_info->logo_checksum.support) {
		ret = lcd_kit_dsi_cmds_tx(hisifd,
			&disp_info->logo_checksum.enable_cmds);
		if (ret) {
			LCD_KIT_ERR("enable checksum fail\n");
			return;
		}
		LCD_KIT_INFO("enable logo checksum\n");
	}
}

void lcd_logo_checksum_check(struct hisi_fb_data_type *hisifd)
{
	int ret;
	int checksum_result = 0;
	int i;
	uint8_t read_value[LCD_KIT_LOGO_CHECKSUM_SIZE] = {0};

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		return;
	}
	if (disp_info->logo_checksum.support) {
		ret = lcd_kit_dsi_cmds_rx(hisifd, read_value,
			LCD_KIT_LOGO_CHECKSUM_SIZE,
			&disp_info->logo_checksum.checksum_cmds);
		if (ret) {
			LCD_KIT_ERR("checksum_cmds fail\n");
			return;
		}
		if (disp_info->logo_checksum.value.buf == NULL) {
			LCD_KIT_ERR("logo checksum value is Null!\n");
			return;
		}
		if (disp_info->logo_checksum.value.cnt >= LCD_KIT_LOGO_CHECKSUM_SIZE) {
			LCD_KIT_ERR("logo checksum value cnt is too big!\n");
			return;
		}
		for (i = 0; i < disp_info->logo_checksum.value.cnt; i++) {
			if (read_value[i] !=
				disp_info->logo_checksum.value.buf[i]) {
				LCD_KIT_INFO("read[%d] = 0x%x,buf[%d] = 0x%x\n",
					i, read_value[i], i,
					disp_info->logo_checksum.value.buf[i]);
				checksum_result++;
			}
		}

		if (checksum_result) {
			LCD_KIT_INFO("logo checksum_result:%d\n",
				checksum_result);
			ret = lcd_kit_change_dts_value(
				"huawei,lcd_panel_type",
				"fastboot_record_bit", BIT(1));
			if (ret) {
				LCD_KIT_ERR("change BIT(1) fail\n");
				return;
			}
		}

		ret = lcd_kit_dsi_cmds_tx(hisifd,
			&disp_info->logo_checksum.disable_cmds);
		if (ret) {
			LCD_KIT_ERR("disable checksum fail\n");
			return;
		}
	}
}

static bool isalnum(char ch)
{
	if (isalpha(ch) || isdigit(ch))
		return true;
	return false;
}

static int lcd_kit_check_project_id(void)
{
	int i;

	for (i = 0; i < PROJECTID_LEN - 1; i++) {
		if (!isalnum((disp_info->project_id.id)[i]))
			return LCD_KIT_FAIL;
	}
	return LCD_KIT_OK;
}

int lcd_kit_read_project_id(struct hisi_fb_data_type *hisifd)
{
	int ret;

	if (disp_info->project_id.support == 0)
		return LCD_KIT_OK;
	ret = memset_s(disp_info->project_id.id, PROJECTID_LEN + 1, 0, PROJECTID_LEN + 1);
	if (ret != EOK) {
		LCD_KIT_ERR("memset_s fail\n");
		return LCD_KIT_FAIL;
	}
	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		return LCD_KIT_FAIL;
	}
	ret = lcd_kit_dsi_cmds_rx(hisifd, (uint8_t *)disp_info->project_id.id,
		(PROJECTID_LEN + 1), &disp_info->project_id.cmds);
	if ((ret == LCD_KIT_OK) && (lcd_kit_check_project_id() == LCD_KIT_OK)) {
		LCD_KIT_INFO("project id is %s\n", disp_info->project_id.id);
		lcd_kit_set_property_str(DTS_LCD_PANEL_TYPE, "project_id",
			disp_info->project_id.id);
		return LCD_KIT_OK;
	}
	LCD_KIT_INFO("disp_info->project_id.default_project_id = %s\n",
		disp_info->project_id.default_project_id);
	lcd_kit_set_property_str(DTS_LCD_PANEL_TYPE, "project_id",
		disp_info->project_id.default_project_id);
	return LCD_KIT_OK;
}
#ifdef FASTBOOT_FW_DTB_ENABLE
static int lcd_kit_set_main_barcode_callback(void)
{
	lcd_kit_set_dts_data(g_main_panel_dts_path,
		"lcd-kit,barcode-length",
		LCD_KIT_SNCODE_SIZE);
	lcd_kit_set_dts_array_data(g_main_panel_dts_path,
		"lcd-kit,barcode-value",
		g_main_panel_barcode,
		LCD_KIT_SNCODE_SIZE);
	return LCD_KIT_OK;
}
static int lcd_kit_set_ext_barcode_callback(void)
{
	lcd_kit_set_dts_data(g_ext_panel_dts_path,
		"lcd-kit,barcode-length",
		LCD_KIT_SNCODE_SIZE);
	lcd_kit_set_dts_array_data(g_ext_panel_dts_path,
		"lcd-kit,barcode-value",
		g_ext_panel_barcode,
		LCD_KIT_SNCODE_SIZE);
	return LCD_KIT_OK;
}
#endif
static void lcd_kit_set_dts_main_barcode(struct hisi_fb_data_type *hisifd,
	uint8_t *barcode, uint32_t barcode_len)
{
	int ret;
	struct hisi_panel_info *pinfo = NULL;

	if (hisifd == NULL) {
		LCD_KIT_ERR("hisifd is null\n");
		return;
	}
	pinfo = hisifd->panel_info;
	if ((pinfo == NULL) || (pinfo->disp_cnnt_type != LCD_MAIN_PANEL)) {
		LCD_KIT_INFO("no need save sn code\n");
		return;
	}
	ret = memset_s(g_main_panel_barcode, LCD_KIT_SNCODE_SIZE,
		0, LCD_KIT_SNCODE_SIZE);
	if (ret != 0) {
		LCD_KIT_ERR("memset_s error\n");
		return;
	}
	ret = memcpy_s(g_main_panel_barcode, LCD_KIT_SNCODE_SIZE,
		barcode, barcode_len);
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return;
	}
	g_main_panel_dts_path = pinfo->lcd_name;
#ifdef FASTBOOT_FW_DTB_ENABLE
	if (lcd_fdt_ops)
		lcd_fdt_ops->register_update_dts_func(lcd_kit_set_main_barcode_callback);
#endif
}
static void lcd_kit_set_dts_ext_barcode(struct hisi_fb_data_type *hisifd,
	uint8_t *barcode, uint32_t barcode_len)
{
	int ret;
	struct hisi_panel_info *pinfo = NULL;

	if (hisifd == NULL) {
		LCD_KIT_ERR("hisifd is null\n");
		return;
	}

	pinfo = hisifd->panel_info;
	if ((pinfo == NULL) || (pinfo->disp_cnnt_type != LCD_EXT_PANEL)) {
		LCD_KIT_INFO("no need save sn code\n");
		return;
	}
	ret = memset_s(g_ext_panel_barcode, LCD_KIT_SNCODE_SIZE,
		0, LCD_KIT_SNCODE_SIZE);
	if (ret != 0) {
		LCD_KIT_ERR("memset_s error\n");
		return;
	}
	ret = memcpy_s(g_ext_panel_barcode, LCD_KIT_SNCODE_SIZE,
		barcode, barcode_len);
	if (ret != 0) {
		LCD_KIT_ERR("memcpy_s error\n");
		return;
	}
	g_ext_panel_dts_path = pinfo->lcd_name;
#ifdef FASTBOOT_FW_DTB_ENABLE
	if (lcd_fdt_ops)
		lcd_fdt_ops->register_update_dts_func(lcd_kit_set_ext_barcode_callback);
#endif
}
static int lcd_kit_set_dts_barcode(struct hisi_fb_data_type *hisifd,
	const char *np)
{
	int ret;
	uint8_t read_value[LCD_KIT_SNCODE_SIZE + 1] = {0};

	if (!hisifd) {
		LCD_KIT_ERR("hisifd is null\n");
		return LCD_KIT_FAIL;
	}
	lcd_kit_parse_dcs_cmds(np, "lcd-kit,barcode-2d-cmds",
		"lcd-kit,barcode-2d-cmds-state",
		&disp_info->sn_code.cmds);
	ret = lcd_kit_dsi_cmds_rx(hisifd, (uint8_t *)read_value,
		LCD_KIT_SNCODE_SIZE,
		&disp_info->sn_code.cmds);
	if (ret != 0) {
		LCD_KIT_ERR("get sn_code error!\n");
		return LCD_KIT_FAIL;
	}
	lcd_kit_set_dts_main_barcode(hisifd, read_value, LCD_KIT_SNCODE_SIZE);
	lcd_kit_set_dts_ext_barcode(hisifd, read_value, LCD_KIT_SNCODE_SIZE);
	return LCD_KIT_OK;
}

int lcd_kit_parse_sn_check(struct hisi_fb_data_type *hisifd, const char *np)
{
	struct hisi_panel_info *pinfo = NULL;
	uint32_t oeminfo_support = 0;
	uint32_t barcode_support = 0;

	pinfo = hisifd->panel_info;
	if (pinfo && (pinfo->product_type == LCD_FOLDER_TYPE)) {
		lcd_kit_parse_get_u32_default(np, "lcd-kit,oem-info-support",
			&oeminfo_support, 0);
		lcd_kit_parse_get_u32_default(np, "lcd-kit,oem-barcode-2d-support",
			&barcode_support, 0);
		if (oeminfo_support && barcode_support)
			lcd_kit_set_dts_barcode(hisifd, np);
	}

	/* sn check on support */
	lcd_kit_parse_get_u32_default(np, "lcd-kit,sn-code-support",
		&disp_info->sn_code.sn_support, 0);
	lcd_kit_parse_get_u32_default(np, "lcd-kit,sn-code-check",
		&disp_info->sn_code.sn_check_support, 0);
	if (disp_info->sn_code.sn_support &&
		disp_info->sn_code.sn_check_support) {
		lcd_kit_parse_dcs_cmds(np, "lcd-kit,barcode-2d-cmds",
			"lcd-kit,barcode-2d-cmds-state",
			&disp_info->sn_code.cmds);

		if (lcd_kit_write_tplcd_info_to_mem(hisifd) < 0) {
			LCD_KIT_INFO("write tplcd info to mem error!\n");
			return LCD_KIT_FAIL;
		}
	}
	return LCD_KIT_OK;
}
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
static int lcd_add_status_disabled_hilegacy(void)
{
	struct fdt_operators *fdt_ops = NULL;
	struct dtb_operators *dtimage_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;
	unsigned int i;

	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		LCD_KIT_ERR("can not get fdt_ops!\n");
		return LCD_KIT_FAIL;
	}
	dtimage_ops = get_operators(DTIMAGE_MODULE_NAME_STR);
	if (dtimage_ops == NULL) {
		LCD_KIT_ERR("can not get dtimage operators!\n");
		return LCD_KIT_FAIL;
	}
	fdt_ops->fdt_operate_lock();
	fdt = dtimage_ops->get_dtb_addr();
	if (fdt == NULL) {
		HISI_FB_ERR("failed to get fdt addr!\n");
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		HISI_FB_ERR("fdt_open_into failed!\n");
		fdt_ops->fdt_operate_unlock();
		return LCD_KIT_FAIL;
	}
	for (i = 0; i < ARRAY_SIZE(lcd_kit_map); ++i) {
		if (!strcmp(disp_info->lcd_name, lcd_kit_map[i].lcd_name)) {
			LCD_KIT_INFO("lcd name %s\n", disp_info->lcd_name);
			continue;
		}
		ret = fdt_ops->fdt_path_offset(fdt, lcd_kit_map[i].lcd_name);
		offset = ret;
		ret = fdt_ops->fdt_setprop_string(fdt, offset,
			(const char *)"status", (const void *)"disabled");
		if (ret != 0)
			LCD_KIT_INFO("Cannot set lcd status errno=%d\n", ret);
	}
	fdt_ops->fdt_operate_unlock();
	return LCD_KIT_OK;
}
#else
static int lcd_add_status_disabled_hifastboot(void)
{
	struct fdt_operators *fdt_ops = NULL;
	void *fdt = NULL;
	int ret;
	int offset;
	unsigned int i;

	LCD_KIT_INFO("+\n");
	fdt_ops = get_operators(FDT_MODULE_NAME_STR);
	if (fdt_ops == NULL) {
		LCD_KIT_ERR("fdt_ops is null\n");
		return LCD_KIT_FAIL;
	}
	fdt = fdt_ops->get_dt_handle(UPDATE_DTS_TYPE);
	if (!fdt) {
		LCD_KIT_ERR("failed to get fdt addr!\n");
		return LCD_KIT_FAIL;
	}
	ret = fdt_ops->fdt_open_into(fdt, fdt, DTS_SPACE_LEN);
	if (ret < 0) {
		LCD_KIT_ERR("fdt_open_into failed!\n");
		return LCD_KIT_FAIL;
	}
	for (i = 0; i < ARRAY_SIZE(lcd_kit_map); ++i) {
		if (!strcmp(disp_info->lcd_name, lcd_kit_map[i].lcd_name)) {
			LCD_KIT_INFO("lcd name %s\n", disp_info->lcd_name);
			continue;
		}
		ret = fdt_ops->fdt_path_offset(fdt, lcd_kit_map[i].lcd_name);
		offset = ret;
		ret = fdt_ops->fdt_setprop_string(fdt, offset,
			(const char *)"status", (const void *)"disabled");
		if (ret != 0)
			LCD_KIT_INFO("Cannot set lcd status errno=%d\n", ret);
	}
	LCD_KIT_INFO("-\n");
	return LCD_KIT_OK;
}
#endif

int lcd_add_status_disabled(void)
{
	int ret;
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN980) || defined(FASTBOOT_DISPLAY_LOGO_ORLANDO)
	ret = lcd_add_status_disabled_hilegacy();
#else
	ret = lcd_add_status_disabled_hifastboot();
#endif
	return ret;
}

void lcd_kit_check_otp(struct hisi_fb_data_type *hisifd)
{
	uint8_t value = 0;
	int ret;

	if (!disp_info->otp.support)
		return;
	ret = lcd_kit_dsi_cmds_rx(hisifd, &value,
		1, &disp_info->otp.read_osc_cmds);
	if (ret != LCD_KIT_OK) {
		LCD_KIT_ERR("read osc err\n");
		return;
	}
	if ((value & disp_info->otp.osc_mask) != disp_info->otp.osc_value) {
		LCD_KIT_INFO("osc = %d, exit", value);
		return;
	}
	ret = lcd_kit_dsi_cmds_rx(hisifd, &value,
		1, &disp_info->otp.read_otp_cmds);
	if (ret != LCD_KIT_OK) {
		LCD_KIT_ERR("read otp err\n");
		return;
	}
	if ((value & disp_info->otp.otp_mask) != disp_info->otp.otp_value) {
		LCD_KIT_INFO("otp times = %d, exit", value);
		return;
	}
	ret = lcd_kit_dsi_cmds_tx(hisifd,
		&disp_info->otp.otp_flow_cmds);
	if (ret) {
		LCD_KIT_ERR("otp flow fail\n");
		return;
	}
	lcd_kit_delay(disp_info->otp.tx_delay, LCD_KIT_WAIT_MS);
	ret = lcd_kit_dsi_cmds_tx(hisifd,
		&disp_info->otp.otp_end_cmds);
	if (ret) {
		LCD_KIT_ERR("otp end fail\n");
		return;
	}
	LCD_KIT_INFO("otp success\n");
}
