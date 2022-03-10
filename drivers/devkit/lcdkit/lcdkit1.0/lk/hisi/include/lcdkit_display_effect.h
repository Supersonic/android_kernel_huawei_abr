/*
 * lcdkit_display_effect.h
 *
 * lcdkit disp effect head file for lcd driver
 *
 * Copyright (c) 2018-2020 Huawei Technologies Co., Ltd.
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

#ifndef HISI_DISPLAY_EFFECT_H
#define HISI_DISPLAY_EFFECT_H

#include <sys.h>
#include <boot.h>
#include <oeminfo_ops.h>
#include <global_ddr_map.h>
#include "hisi_fb.h"

#define LCDKIT_COLOR_INFO_SIZE 8
#define LCDKIT_SERIAL_INFO_SIZE 20
#define Hx83112A 1
#define Hx83112C 2
#define TD4336 3
#define NT36772A 4
#define COLOR_POINT_START_POSITION_TD4336 12
#define COLOR_POINT_LENGTH 10

struct panelid
{
	uint32_t modulesn;
	uint32_t equipid;
	uint32_t modulemanufactdate;
	uint32_t vendorid;
};

struct coloruniformparams
{
	uint32_t c_lmt[3];
	uint32_t mxcc_matrix[3][3];
	uint32_t white_decay_luminace;
};

struct colormeasuredata
{
	uint32_t chroma_coordinates[4][2];
	uint32_t white_luminance;
};

struct lcdbrightnesscoloroeminfo
{
	uint32_t id_flag;
	uint32_t tc_flag;
	struct panelid  panel_id;
	struct coloruniformparams color_params;
	struct colormeasuredata color_mdata;
};
#if defined(FASTBOOT_DISPLAY_LOGO_KIRIN970)
void lcd_bright_rgbw_id_from_oeminfo_process(struct hisi_fb_data_type* phisifd);
#endif

#if defined (FASTBOOT_DISPLAY_LOGO_HI3660) || defined (FASTBOOT_DISPLAY_LOGO_KIRIN970)
int lcdkit_write_gm_to_reserved_mem();
#endif

#endif
