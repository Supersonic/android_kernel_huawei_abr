/*
 * ktd3136.h
 *
 * ktd3136 backlight driver
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

#ifndef __KTD3136_H
#define __KTD3136_H

#include <libfdt.h>
#include <platform/mt_i2c.h>
#include <platform/mt_gpio.h>

#define KTD3136_INVALID_VAL 0xFFFF
#define KTD3136_SLAV_ADDR 0x36
#define KTD3136_I2C_SPEED 100
#define KTD3136_NAME "ktd3136"
#define DTS_COMP_KTD3136 "ktd,ktd3136"
#define KTD3136_SUPPORT "ktd3136_support"
#define KTD3136_I2C_BUS_ID "ktd3136_i2c_bus_id"
#define KTD3136_HW_ENABLE "ktd3136_hw_enable"
#define KTD3136_HW_EN_GPIO "ktd3136_hw_en_gpio"
#define KTD3136_HW_EN_DELAY "bl_on_lk_mdelay"
#define KTD3136_BL_LEVEL "bl_level"

#define KTD3136_WRITE_LEN 2
#define KTD3136_BL_DEFAULT_LEVEL 255
#define KTD3136_BL_MAX 2047

/* config reg */
#define KTD3136_REG_REVISION 0x00
#define KTD3136_REG_RESET 0x01
#define KTD3136_REG_CTRL_CURRENT_SET 0x02
#define KTD3136_REG_CTRL_BL_CONFIG 0x03
#define KTD3136_REG_CTRL_BRIGHTNESS_LSB 0x04
#define KTD3136_REG_CTRL_BRIGHTNESS_MSB 0x05
#define KTD3136_REG_CTRL_PWM_CONFIG 0x06
#define KTD3136_REG_CTRL_BOOT_CTRL 0x08

#define KTD3136_CHIP_ID 0x18
#define KTD3136_DEV_MASK 0x38
#define KTD3136_RW_REG_MAX 6

/* backlight */
#define KTD3136_BL_LSB_MASK 0x07
#define KTD3136_BL_MSB_MASK 0xFF
#define KTD3136_BL_LSB_LEN 3

struct ktd3136_backlight_information {
	/* whether support KTD3136 or not */
	unsigned int ktd3136_support;
	/* which i2c bus controller ktd3136 mount */
	unsigned int ktd3136_i2c_bus_id;
	unsigned int ktd3136_hw_en;
	/* KTD3136 hw_en gpio */
	unsigned int ktd3136_hw_en_gpio;
	unsigned int ktd3136_reg[KTD3136_RW_REG_MAX];
	unsigned int bl_on_lk_mdelay;
	unsigned int bl_level;
};
void ktd3136_set_backlight_status(void);
int ktd3136_init(struct mtk_panel_info *pinfo);
#endif
