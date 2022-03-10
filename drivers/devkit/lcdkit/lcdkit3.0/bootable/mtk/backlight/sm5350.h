/*
 * sm5350.h
 *
 * sm5350 backlight driver
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
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

#ifndef __SM5350_H
#define __SM5350_H

#include <libfdt.h>
#include <platform/mt_i2c.h>
#include <platform/mt_gpio.h>

#define SM5350_INVALID_VAL 0xFFFF
#define SM5350_SLAV_ADDR 0x36
#define SM5350_I2C_SPEED 100
#define SM5350_NAME "sm5350"
#define DTS_COMP_SM5350 "sm,sm5350"
#define SM5350_SUPPORT "sm5350_support"
#define SM5350_I2C_BUS_ID "sm5350_i2c_bus_id"
#define SM5350_HW_ENABLE "sm5350_hw_enable"
#define SM5350_HW_EN_GPIO "sm5350_hw_en_gpio"
#define SM5350_HW_EN_DELAY "bl_on_lk_mdelay"
#define SM5350_BL_LEVEL "bl_level"

#define SM5350_WRITE_LEN 2
#define SM5350_BL_DEFAULT_LEVEL 255
#define SM5350_BL_MAX 2047

/* config reg */
#define SM5350_REG_REVISION 0x00
#define SM5350_REG_RESET 0x01
#define SM5350_REG_HVLED_CURRENT_SINK_OUT_CONFIG 0x10
#define SM5350_REG_CTRL_A_UP_DOWN_RAMP_TIME 0x11
#define SM5350_REG_CTRL_B_UP_DOWN_RAMP_TIME 0x12
#define SM5350_REG_CTRL_RUN_RAMP_TIME 0x13
#define SM5350_REG_CTRL_RUN_RAMP_TIME_CONFIG 0x14
#define SM5350_REG_BL_CONFIG 0x16
#define SM5350_REG_BL_CTRL_A_FULL_SCALE_CURRENT_SETTING 0x17
#define SM5350_REG_BL_CTRL_B_FULL_SCALE_CURRENT_SETTING 0x18
#define SM5350_REG_HVLED_CURRENT_SINK_FEEDBACK_ENABLE 0x19
#define SM5350_REG_BOOST_CTRL 0x1A
#define SM5350_REG_AUTO_FREQUENC_THRESHOLD 0x1B
#define SM5350_REG_PWM_CONFIG 0x1C
#define SM5350_REG_CTRL_A_BRIGHTNESS_LSB 0x20
#define SM5350_REG_CTRL_A_BRIGHTNESS_MSB 0x21
#define SM5350_REG_CTRL_B_BRIGHTNESS_LSB 0x22
#define SM5350_REG_CTRL_B_BRIGHTNESS_MSB 0x23
#define SM5350_REG_CTRL_BANK_ENABLE 0x24
#define SM5350_REG_HVLED_OPEN_FAULT 0xB0
#define SM5350_REG_HVLED_SHORT_FAULT 0xB2
#define SM5350_REG_LED_FAULT_ENABLE 0xB4

#define SM5350_CHIP_ID 0x00
#define SM5350_DEV_MASK 0x0F
#define SM5350_RW_REG_MAX 9

/* backlight */
#define SM5350_BL_LSB_MASK 0x07
#define SM5350_BL_MSB_MASK 0xFF
#define SM5350_BL_LSB_LEN 3

struct sm5350_backlight_information {
	/* whether support sm5350 or not */
	unsigned int sm5350_support;
	/* which i2c bus controller sm5350 mount */
	unsigned int sm5350_i2c_bus_id;
	unsigned int sm5350_hw_en;
	/* sm5350 hw_en gpio */
	unsigned int sm5350_hw_en_gpio;
	unsigned int sm5350_reg[SM5350_RW_REG_MAX];
	unsigned int bl_on_lk_mdelay;
	unsigned int bl_level;
};
void sm5350_set_backlight_status(void);
int sm5350_init(struct mtk_panel_info *pinfo);
#endif
