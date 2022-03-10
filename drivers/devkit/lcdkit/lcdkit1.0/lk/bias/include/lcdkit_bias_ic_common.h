/*
 * lcdkit_bias_ic_common.h
 *
 * lcdkit bias head file for lcd driver
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

#ifndef _LCDKIT_BIAS_IC_COMMON_H_
#define _LCDKIT_BIAS_IC_COMMON_H_
#include "lcdkit_i2c.h"
#define LCD_BIAS_IC_NAME_LEN  128

#define BIAS_IC_READ_INHIBITION	   0x01
#define BIAS_IC_HAVE_E2PROM		   0x02
#define BIAS_IC_CONFIG_NEED_ENABLE	0x04
#define BIAS_IC_RESUME_NEED_CONFIG	0x08

struct lcd_bias_voltage_info
{
	char			name[LCD_BIAS_IC_NAME_LEN];
	unsigned char   i2c_addr;
	unsigned char   i2c_num;
	unsigned char   qup_id;
	unsigned char   ic_type;
	unsigned char   vpos_reg;
	unsigned char   vneg_reg;
	unsigned char   vpos_val;
	unsigned char   vneg_val;
	unsigned char   vpos_mask;
	unsigned char   vneg_mask;
	unsigned char   check_reg;
	unsigned char   check_val;
	unsigned char   check_mask;
	unsigned char   state_reg;
	unsigned char   state_val;
	unsigned char   state_mask;
	unsigned char   write_reg;
	unsigned char   write_val;
	unsigned char   write_mask;
	int			 delay;
};

struct lcd_bias_ic
{
	struct lcd_bias_voltage_info **lcd_bias_ic_list;
	int num_of_lcd_bias_ic_list;
};

void lcdkit_bias_ic_init(struct lcd_bias_voltage_info **pinfo, int len);
struct lcd_bias_voltage_info *get_lcd_bias_ic_info(void);
#endif