/*
 * bias_bl_utils.h
 *
 * bias_bl_utils head file for hisi paltform i2c device
 *
 * Copyright (c) 2019-2019 Huawei Technologies Co., Ltd.
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
#ifndef _BIAS_BL_UTILS_H_
#define _BIAS_BL_UTILS_H_

#include <platform.h>
#include <debug.h>
#include <sys.h>
#include <module.h>
#include <i2c_ops.h>
#include <boot.h>
#include <dtimage_ops.h>
#include <fdt_ops.h>
#include "libfdt_env.h"

#define FASTBOOT_ERR(msg, ...) do { \
	PRINT_ERROR("[BIAS_BL/E]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define FASTBOOT_INFO(msg, ...) do { \
	PRINT_INFO("[BIAS_BL/I]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_RET_FAIL (-1)
#define LCD_RET_OK    0

enum hisi_i2c_bus_num {
	I2C_BUS_0 = 0,
	I2C_BUS_1 = 1,
	I2C_BUS_2 = 2,
	I2C_BUS_3 = 3,
	I2C_BUS_4 = 4,
	I2C_BUS_5 = 5,
	I2C_BUS_6 = 6,
	I2C_BUS_7 = 7,
	I2C_BUS_INVALID = 64
};

void bias_bl_ops_init(void);
#endif
