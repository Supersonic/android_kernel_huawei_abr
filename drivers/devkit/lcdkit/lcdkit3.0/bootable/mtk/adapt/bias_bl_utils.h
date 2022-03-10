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

#include <debug.h>
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#include <fdt_op.h>
#endif
#include <platform/mt_i2c.h>
#include <platform/mt_gpt.h>
#include <fdt_op.h>
#include <string.h>
#include <lk_builtin_dtb.h>

#define FASTBOOT_ERR(msg, ...) do { \
	dprintf(CRITICAL, "[BIAS_BL/E]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define FASTBOOT_INFO(msg, ...) do { \
	dprintf(SPEW, "[BIAS_BL/I]%s: "msg, __func__, ## __VA_ARGS__); \
} while (0)

#define LCD_RET_FAIL (-1)
#define LCD_RET_OK    0
#define INVALID_VALUE 64

enum mtk_i2c_bus_num {
	I2C_BUS_0 = 0,
	I2C_BUS_1 = 1,
	I2C_BUS_2 = 2,
	I2C_BUS_3 = 3,
	I2C_BUS_4 = 4,
	I2C_BUS_5 = 5,
	I2C_BUS_6 = 6,
	I2C_BUS_7 = 7,
	I2C_BUS_8 = 8,
	I2C_BUS_9 = 9,
	I2C_BUS_INVALID = 64
};

void bias_bl_ops_init(void);
#endif
