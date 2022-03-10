/*
 * lcdkit_i2c.h
 *
 * lcdkit i2c head file for lcd driver
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

#ifndef _LCDKIT_I2C_H_
#define _LCDKIT_I2C_H_

#include <platform.h>
#include <debug.h>
#include <sys.h>
#include <module.h>
#include <i2c_ops.h>
#include <gpio.h>
#include <boot.h>
#include <dtimage_ops.h>
#include <fdt_ops.h>

#define LCDKIT_DEBUG_ERROR(msg, ...)  PRINT_ERROR("[display E]%s: "msg, __func__, ## __VA_ARGS__)
#define LCDKIT_DEBUG_INFO(msg, ...)  PRINT_INFO("[display I]%s: "msg, __func__, ## __VA_ARGS__)
int lcdkit_bias_ic_i2c_read_u8(unsigned char chip_addr, unsigned char addr, unsigned char *buf);
int lcdkit_bias_ic_i2c_write_u8(unsigned char chip_addr, unsigned char addr, unsigned char val);
int lcdkit_backlight_ic_i2c_read_u8(unsigned char chip_addr, unsigned char addr, unsigned char *buf);
int lcdkit_backlight_ic_i2c_write_u8(unsigned char chip_addr, unsigned char addr, unsigned char val);
#endif
