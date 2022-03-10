/*
 * lcd_bl.c
 *
 * lcd bl function for lcd driver
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

#include "lcdkit_i2c.h"
#include "lcd_bl.h"
#include "lcd_bl_linear_exponential_table.h"

#define LCD_BACKLIGHT_FAIL (-1)
#define LCD_BACKLIGHT_OK 0
#define I2C_READ_CMD 0
#define CHECK_REG 0x02
#define CHECK_VALUE 0xe0

static struct backlight_ic_info *g_lcd_backlight_chip = NULL;

int lckdit_backlight_ic_get_i2c_num(unsigned char *buf)
{
	if(NULL == g_lcd_backlight_chip)
	{
		return -1;
	}
	*buf = g_lcd_backlight_chip->i2c_num;
	return 0;
}

int lcdkit_backlight_ic_get_i2c_qup_id(unsigned char *buf)
{
	if(NULL == g_lcd_backlight_chip)
	{
		return -1;
	}
	*buf = g_lcd_backlight_chip->qup_id;
	return 0;
}

static int lcdkit_backlight_ic_i2c_update_u8(unsigned char chip_addr, unsigned char reg, unsigned char mask, unsigned char val)
{
	int ret;
	unsigned char orig_val = 0;
	unsigned char value;

	ret = lcdkit_backlight_ic_i2c_read_u8(chip_addr, reg, &orig_val);
	if(ret < 0)
	{
		LCDKIT_DEBUG_ERROR("lcdkit_i2c_update_u8 read failed\n");
		return ret;
	}

	value = orig_val & (~mask);

	value |= (val & mask);

	if(value != orig_val)
	{
		ret = lcdkit_backlight_ic_i2c_write_u8(chip_addr, reg, value);
		if(ret < 0)
		{
			LCDKIT_DEBUG_ERROR("lcdkit_i2c_update_u8 write failed\n");
			return ret;
		}
	}
	LCDKIT_DEBUG_INFO("lcdkit_i2c_update_u8 addr is 0x%x, val is 0x%x\n", reg, value);
	return ret;
}

static int lcdkit_backlight_ic_second_check(struct backlight_ic_info *pinfo)
{
	int ret;
	unsigned char read_val;
	unsigned char cmd_mask;

	if (pinfo == NULL)
		return LCD_BACKLIGHT_FAIL;

	if (pinfo->second_check == 0) {
		LCDKIT_DEBUG_INFO("no need second check\n");
		return LCD_BACKLIGHT_OK;
	}

	switch (pinfo->second_check_cmd.ops_type) {
	case I2C_READ_CMD:
		ret = lcdkit_backlight_ic_i2c_read_u8(pinfo->i2c_addr,
			pinfo->second_check_cmd.cmd_reg, &read_val);
		break;
	default:
		ret = LCD_BACKLIGHT_FAIL;
		break;
	}
	if (ret < 0)
		return ret;

	if (pinfo->second_check_cmd.cmd_mask != 0) {
		if ((pinfo->second_check_cmd.cmd_mask & read_val) ==
			pinfo->second_check_cmd.cmd_val) {
			LCDKIT_DEBUG_INFO("mask 0x%x, read ok, ret is %d\n",
				pinfo->second_check_cmd.cmd_mask, ret);
		} else {
			cmd_mask = pinfo->second_check_cmd.cmd_mask;
			LCDKIT_DEBUG_INFO("reg value is not expect value!\n");
			if (((cmd_mask & read_val) != CHECK_VALUE) &&
				!strcmp(pinfo->name, "hw_lm36274") &&
				(pinfo->second_check_cmd.cmd_reg == CHECK_REG))
				ret = LCD_BACKLIGHT_OK;
			else
				ret = LCD_BACKLIGHT_FAIL;
		}
	}

	return ret;
}

static int lcdkit_backlight_ic_check(struct backlight_ic_info *pinfo)
{
	int ret;
	unsigned char readval = 0;
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_check\n");
	if(pinfo == NULL)
	{
		return -1;
	}
	g_lcd_backlight_chip = pinfo;
	switch (pinfo->check_cmd.ops_type) {
	case 0:
		ret = lcdkit_backlight_ic_i2c_read_u8(pinfo->i2c_addr,
			pinfo->check_cmd.cmd_reg, &readval);
		break;
	case 1:
		ret = lcdkit_backlight_ic_i2c_write_u8(pinfo->i2c_addr,
			pinfo->check_cmd.cmd_reg, pinfo->check_cmd.cmd_val);
		break;
	case 2:
		ret = lcdkit_backlight_ic_i2c_update_u8(pinfo->i2c_addr,
			pinfo->check_cmd.cmd_reg, pinfo->check_cmd.cmd_mask,
			pinfo->check_cmd.cmd_val);
		break;
	default:
		ret = -1;
		break;
	}

	if(ret < 0)
		return ret;
	if (pinfo->check_cmd.ops_type == 0) {
		if (pinfo->check_cmd.cmd_mask != 0x00) {
			if ((pinfo->check_cmd.cmd_mask & readval) == pinfo->check_cmd.cmd_val) {
				LCDKIT_DEBUG_INFO("mask is 0x%x, read i2c reg value ok, ret is %d\n", pinfo->check_cmd.cmd_mask, ret);
				ret = lcdkit_backlight_ic_second_check(pinfo);
				return ret;
			} else {
				LCDKIT_DEBUG_INFO("reg value is not expect value!\n");
				return -1;
			}
		}
		if (readval != pinfo->check_cmd.cmd_val) {
			return -1;
		}
	}
	return 0;
}

static int lcdkit_backlight_ic_set_hidden_reg(struct backlight_ic_info *pblinfo)
{
	int ret;
	unsigned char readval = 0;

	LCDKIT_DEBUG_INFO("lcdkit_backlight_ic_set_hidden_reg\n");
	if(pblinfo == NULL)
	{
		return -1;
	}

	ret = lcdkit_backlight_ic_i2c_write_u8(pblinfo->i2c_addr, pblinfo->security_reg_enable_cmd.cmd_reg, pblinfo->security_reg_enable_cmd.cmd_val);
	if(ret < 0)
	{
		LCDKIT_DEBUG_ERROR("lcdkit backlight ic set security reg enable failed\n");
		goto out;
	}
	ret = lcdkit_backlight_ic_i2c_read_u8(pblinfo->i2c_addr, pblinfo->hidden_reg_cmd.cmd_reg, &readval);
	if(ret < 0)
	{
		LCDKIT_DEBUG_ERROR("lcdkit backlight ic read hidden reg failed\n");
		goto out;
	}

	if(pblinfo->hidden_reg_val_mask != (readval&pblinfo->hidden_reg_val_mask))
	{
		readval |= pblinfo->hidden_reg_val_mask;
		ret = lcdkit_backlight_ic_i2c_write_u8(pblinfo->i2c_addr, pblinfo->hidden_reg_cmd.cmd_reg, readval);
		if(ret < 0)
			LCDKIT_DEBUG_ERROR("lcdkit backlight ic set hidden reg failed\n");
	}
out:
	ret = lcdkit_backlight_ic_i2c_write_u8(pblinfo->i2c_addr, pblinfo->security_reg_disable_cmd.cmd_reg, pblinfo->security_reg_disable_cmd.cmd_val);
	if(ret < 0)
		LCDKIT_DEBUG_ERROR("lcdkit backlight ic set security reg disable failed\n");

	return ret;
}

int lcdkit_backlight_ic_inital(struct backlight_ic_info *pinfo)
{
	int ret = 0;
	int i;
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_inital\n");
	if(pinfo == NULL)
	{
		return -1;
	}

	if (pinfo->ic_hidden_reg_support)
	{
		ret = lcdkit_backlight_ic_set_hidden_reg(pinfo);
		if(ret < 0)
			LCDKIT_DEBUG_ERROR("lcdkit backlight is set hidden reg failed\n");
	}

	for (i=0; i<pinfo->num_of_init_cmds; i++)
	{
		switch (pinfo->init_cmds[i].ops_type) {
		case 0:
			ret = lcdkit_backlight_ic_i2c_read_u8(pinfo->i2c_addr,
				pinfo->init_cmds[i].cmd_reg, &(pinfo->init_cmds[i].cmd_val));
			break;
		case 1:
			ret = lcdkit_backlight_ic_i2c_write_u8(pinfo->i2c_addr,
				pinfo->init_cmds[i].cmd_reg, pinfo->init_cmds[i].cmd_val);
			break;
		case 2:
			ret = lcdkit_backlight_ic_i2c_update_u8(pinfo->i2c_addr,
				pinfo->init_cmds[i].cmd_reg, pinfo->init_cmds[i].cmd_mask,
				pinfo->init_cmds[i].cmd_val);
			break;
		default:
			break;
		}
		if (ret < 0)
		{
			LCDKIT_DEBUG_ERROR("operation chip_addr 0x%x  reg 0x%x failed!\n",
				pinfo->i2c_addr,pinfo->init_cmds[i].cmd_reg);
			return ret;
		}
	}

	return ret;
}

int lcdkit_backlight_ic_set_brightness(struct backlight_ic_info *pinfo, unsigned int level)
{
	unsigned char level_lsb;
	unsigned char level_msb;
	int ret;

	if(pinfo == NULL)
	{
		return -1;
	}
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_set_brightness\n");

	level_lsb = level & pinfo->bl_lsb_reg_cmd.cmd_mask;
	level_msb = (level >> pinfo->bl_lsb_reg_cmd.val_bits)&(pinfo->bl_msb_reg_cmd.cmd_mask);
	if(pinfo->bl_lsb_reg_cmd.val_bits != 0)
	{
		ret = lcdkit_backlight_ic_i2c_write_u8(pinfo->i2c_addr, pinfo->bl_lsb_reg_cmd.cmd_reg, level_lsb);
		if(ret < 0)
		{
			LCDKIT_DEBUG_ERROR("set backlight ic brightness failed!\n");
			return ret;
		}
	}
	ret = lcdkit_backlight_ic_i2c_write_u8(pinfo->i2c_addr, pinfo->bl_msb_reg_cmd.cmd_reg, level_msb);
	if(ret < 0)
		LCDKIT_DEBUG_ERROR("set backlight ic brightness failed!\n");

	return ret;
}

int lcdkit_backlight_ic_enable_brightness(struct backlight_ic_info *pinfo)
{
	int ret;
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_enable_brightness\n");
	if(pinfo == NULL)
	{
		return -1;
	}

	ret = lcdkit_backlight_ic_i2c_update_u8(pinfo->i2c_addr, pinfo->bl_enable_cmd.cmd_reg, pinfo->bl_enable_cmd.cmd_mask, pinfo->bl_enable_cmd.cmd_val);
	if(ret < 0)
		LCDKIT_DEBUG_ERROR("enable backlight ic brightness failed!\n");

	return ret;
}

int lcdkit_backlight_ic_disable_brightness(struct backlight_ic_info *pinfo)
{
	int ret;
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_disable_brightness\n");
	if(pinfo == NULL)
	{
		return -1;
	}

	ret = lcdkit_backlight_ic_i2c_update_u8(pinfo->i2c_addr, pinfo->bl_disable_cmd.cmd_reg, pinfo->bl_disable_cmd.cmd_mask, pinfo->bl_disable_cmd.cmd_val);
	if(ret < 0)
		LCDKIT_DEBUG_ERROR("disable backlight ic brightness failed!\n");

	return ret;
}
int lcdkit_backlight_ic_disable_device(struct backlight_ic_info *pinfo)
{
	int ret;
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_disable_device\n");
	if(pinfo == NULL)
	{
		return -1;
	}
	ret = lcdkit_backlight_ic_i2c_update_u8(pinfo->i2c_addr, pinfo->disable_dev_cmd.cmd_reg, pinfo->disable_dev_cmd.cmd_mask, pinfo->disable_dev_cmd.cmd_val);
	if(ret < 0)
		LCDKIT_DEBUG_ERROR("disable backlight ic device failed!\n");
	return ret;
}

int lcdkit_backlight_ic_bias(struct backlight_ic_info *pinfo, bool enable)
{
	int ret;
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_bias enable is %d\n",enable);
	if(pinfo == NULL)
	{
		return -1;
	}
	if(enable)
	{
		ret = lcdkit_backlight_ic_i2c_update_u8(pinfo->i2c_addr, pinfo->bias_enable_cmd.cmd_reg, pinfo->bias_enable_cmd.cmd_mask, pinfo->bias_enable_cmd.cmd_val);
		if(ret < 0)
		{
			LCDKIT_DEBUG_ERROR("disable backlight ic enable bias failed!\n");
			return ret;
		}
	}
	else
	{
		ret = lcdkit_backlight_ic_i2c_update_u8(pinfo->i2c_addr, pinfo->bias_disable_cmd.cmd_reg, pinfo->bias_disable_cmd.cmd_mask, pinfo->bias_disable_cmd.cmd_val);
		if(ret < 0)
		{
			LCDKIT_DEBUG_ERROR("disable backlight ic enable bias failed!\n");
			return ret;
		}
	}
	return 0;
}

void lcdkit_backlight_ic_select(struct backlight_ic_info **pinfo, int len)
{
	int i;
	int ret;
	LCDKIT_DEBUG_INFO("lcd_backlight_ic_select\n");

	if(NULL == pinfo)
	{
		LCDKIT_DEBUG_ERROR("lcd_backlight_ic_select pointer is null\n");
		return;
	}

	if(len <= 0)
	{
		LCDKIT_DEBUG_ERROR("lcd_backlight_ic_select no config backlight ic\n");
		return;
	}

	for (i = 0; i < len; ++i)
	{
		ret = lcdkit_backlight_ic_check(pinfo[i]);
		if(ret == 0)
		{
			LCDKIT_DEBUG_INFO("lcd_backlight_ic_select  backlight ic index is %d\n",i);
			break;
		}
	}

	if(i == len)
	{
		LCDKIT_DEBUG_ERROR("lcd_backlight_ic_select no lcd backlight ic is found!\n");
		g_lcd_backlight_chip = NULL;
	}

	return;
}

struct backlight_ic_info *get_lcd_backlight_ic_info(void)
{
	return g_lcd_backlight_chip;
}

int lcdkit_backlight_common_set(int bl_level)
{
	struct backlight_ic_info *tmp = NULL;
	int bl_ctrl_mod = -1;
	unsigned int backlight_level;

	tmp =  get_lcd_backlight_ic_info();
	if(tmp != NULL)
	{
		if(tmp->ic_exponential_ctrl == BL_IC_EXPONENTIAL_MODE)
		{
			backlight_level = linear_exponential_table[bl_level*tmp->bl_level/255];
		}
		else
		{
			backlight_level = bl_level*tmp->bl_level/255;
		}
		LCDKIT_DEBUG_INFO("lcdkit_backlight_common_set backlight level is %d\n",backlight_level);
		bl_ctrl_mod = tmp->bl_ctrl_mod;
		switch (bl_ctrl_mod) {
		case BL_REG_ONLY_MODE:
		case BL_MUL_RAMP_MODE:
		case BL_RAMP_MUL_MODE:
			lcdkit_backlight_ic_set_brightness(tmp,backlight_level);
			break;
		case BL_PWM_ONLY_MODE:
		default:
			break;
		}
	}

	return bl_ctrl_mod;
}
