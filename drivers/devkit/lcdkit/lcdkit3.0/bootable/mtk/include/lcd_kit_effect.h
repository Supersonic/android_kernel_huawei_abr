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

#define LCD_KIT_COLOR_INFO_SIZE  8
#define LCD_KIT_SERIAL_INFO_SIZE 16

#define C_LMT_LEN          3
#define MXCC_MATRIX_ROW    3
#define MXCC_MATRIX_COLUMN 3
#define CHROMA_ROW         4
#define CHROMA_COLUMN      2

struct colormeasuredata {
	uint32_t chroma_coordinates[CHROMA_ROW][CHROMA_COLUMN];
	uint32_t white_luminance;
};

struct coloruniformparams {
	uint32_t c_lmt[C_LMT_LEN];
	uint32_t mxcc_matrix[MXCC_MATRIX_ROW][MXCC_MATRIX_COLUMN];
	uint32_t white_decay_luminace;
};

struct panelid {
	uint32_t modulesn;
	uint32_t equipid;
	uint32_t modulemanufactdate;
	uint32_t vendorid;
};

struct lcdbrightnesscoloroeminfo {
	uint32_t id_flag;
	uint32_t tc_flag;
	struct panelid  panel_id;
	struct coloruniformparams color_params;
	struct colormeasuredata color_mdata;
};

#endif
