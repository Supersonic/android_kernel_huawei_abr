/* Copyright (c) 2019-2019, Huawei terminal Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/

#ifndef __SGM37603A_H
#define __SGM37603A_H

#include <libfdt.h>
#include <platform/mt_i2c.h>
#include <platform/mt_gpio.h>

#ifndef SGM_SUCC
#define SGM_SUCC 0
#endif

#ifndef SGM_FAIL
#define SGM_FAIL (-1)
#endif


#define SGM37603A_INVALID_VAL 0xFFFF
#define SGM37603A_SLAV_ADDR 0x36
#define SGM37603A_I2C_SPEED 100

#define DTS_COMP_SGM37603A "sgm,sgm37603a"
#define SGM37603A_SUPPORT "sgm37603a_support"
#define SGM37603A_I2C_BUS_ID "sgm37603a_i2c_bus_id"
#define SGM37603A_HW_ENABLE "sgm37603a_hw_enable"
#define SGM37603A_HW_EN_GPIO "sgm37603a_hw_en_gpio"
#define SGM37603A_HW_EN_DELAY "bl_on_lk_mdelay"
#define SGM37603A_BL_LEVEL "bl_level"
#define MAX_STR_LEN 50
#define SGM37603A_MSB_LEN 4
#define SGM37603A_WRITE_LEN 2
#define SGM37603A_BL_DEFAULT_LEVEL 255

/* base reg */
#define SGM37603A_REG_DEV_ID 0x01
#define SGM37603A_REG_SW_RESET 0x01
#define SGM37603A_REG_STATUS 0x0A

/* config reg */
#define SGM37603A_REG_RATIO_LSB 0x1A
#define SGM37603A_REG_RATIO_MSB 0x19
#define SGM37603A_REG_BRIGHTNESS_CONTROL 0x11
#define SGM37603A_REG_LEDS_CONFIG 0x10

#define SGM37603A_REG_LEDS_CONFIG_MASK 0x10
#define SGM37603A_REG_LEDS_CONFIG_DEFAULT 0x10

#define SGM37603A_VENDOR_ID 0x18

#define SGM37603A_RW_REG_MAX 4

/* backlight */
#define SGM37603A_BL_LEN 4
#define SGM37603A_BL_OFFSET 1

struct sgm37603a_backlight_information {
	/* whether support sgm37603a or not */
	unsigned int sgm37603a_support;
	/* which i2c bus controller sgm37603a mount */
	unsigned int sgm37603a_i2c_bus_id;
	unsigned int sgm37603a_hw_en;
	/* sgm37603a hw_en gpio */
	unsigned int sgm37603a_hw_en_gpio;
	unsigned int sgm37603a_reg[SGM37603A_RW_REG_MAX];
	unsigned int bl_on_lk_mdelay;
	unsigned int sgm37603a_level_lsb;
	unsigned int sgm37603a_level_msb;
	unsigned int bl_level;
};
void sgm37603a_set_backlight_status(void);
int sgm37603a_init(void);
#endif
