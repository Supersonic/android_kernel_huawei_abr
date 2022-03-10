/*
 * lcd_kit_effect.h
 *
 * lcdkit display effect head file for lcd driver
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

#ifndef _LCD_KIT_EFFECT_H_
#define _LCD_KIT_EFFECT_H_

#include <sys.h>
#include <boot.h>
#include <oeminfo_ops.h>
#include <global_ddr_map.h>
#include "hisi_fb.h"
#include "lcd_kit_common.h"

#define LCD_KIT_COLOR_INFO_SIZE  8
#define LCD_KIT_SERIAL_INFO_SIZE 16
#define HASH_LEN 32
/* OEMINFO ID used for gamma data */
#define OEMINFO_GAMMA_DATA       114
/* OEMINFO ID used fir gamma data len */
#define OEMINFO_GAMMA_INDEX      115
#define OEMINFO_GAMMA_LEN        4

/*
 * 1542 equals gamma_r + gamma_g + gamma_b
 * equals (257 + 257 + 257) * sizeof(u16)
 * 1542 equals degamma_r + degamma_g + degamma_b
 * equals (257 + 257 +257) * sizeof(u16)
 */
#define GM_IGM_LEN         (1542 + 1542)
#define GM_LUT_LEN         1542
#define C_LMT_LEN          3
#define MXCC_MATRIX_ROW    3
#define MXCC_MATRIX_COLUMN 3
#define CHROMA_ROW         4
#define CHROMA_COLUMN      2

struct lcd_kit_tplcd_info {
	uint8_t sn_check;
	uint8_t sn_hash[HASH_LEN * 2 + 1];
};

struct panelid {
	uint32_t modulesn;
	uint32_t equipid;
	uint32_t modulemanufactdate;
	uint32_t vendorid;
};

struct coloruniformparams {
	uint32_t c_lmt[C_LMT_LEN];
	uint32_t mxcc_matrix[MXCC_MATRIX_ROW][MXCC_MATRIX_COLUMN];
	uint32_t white_decay_luminace;
};

struct colormeasuredata {
	uint32_t chroma_coordinates[CHROMA_ROW][CHROMA_COLUMN];
	uint32_t white_luminance;
};

struct lcdbrightnesscoloroeminfo {
	uint32_t id_flag;
	uint32_t tc_flag;
	struct panelid  panel_id;
	struct coloruniformparams color_params;
	struct colormeasuredata color_mdata;
};

struct lcd_kit_brightness_color_uniform {
	uint32_t support;
	struct lcd_kit_dsi_panel_cmds modulesn_cmds;
	struct lcd_kit_dsi_panel_cmds equipid_cmds;
	struct lcd_kit_dsi_panel_cmds modulemanufact_cmds;
	struct lcd_kit_dsi_panel_cmds vendorid_cmds;
};

void lcd_bright_rgbw_id_from_oeminfo_process(struct hisi_fb_data_type *hisifd);
int lcd_kit_write_gm_to_reserved_mem(void);
void lcd_kit_bright_rgbw_id_from_oeminfo(struct hisi_fb_data_type *hisifd);
int lcd_kit_write_tplcd_info_to_mem(struct hisi_fb_data_type *hisifd);
#endif
