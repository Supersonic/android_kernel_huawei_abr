/*
 * ktd3133.h
 *
 * ktd3133 backlight driver head file
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

#ifndef __LINUX_KTD3133_H
#define __LINUX_KTD3133_H

#define KTD3133_SLAV_ADDR	0x35

#define DTS_COMP_KTD3133	"ktd,ktd3133"
#define KTD3133_SUPPORT		"ktd3133_support"
#define KTD3133_I2C_BUS_ID	"ktd3133_i2c_bus_id"
#define KTD3133_HW_ENABLE	"ktd3133_hw_enable"
#define KTD3133_HW_EN_GPIO	"ktd3133_hw_en_gpio"
#define KTD3133_HW_EN_DELAY	"bl_on_lk_mdelay"
#define MAX_STR_LEN	50

/* base reg */
#define KTD3133_REG_DEV_ID	0x00
#define KTD3133_REG_SW_RESET	0x01
#define KTD3133_REG_STATUS	0x0A

/* config reg */
#define KTD3133_REG_MODE	0x02
#define KTD3133_REG_CONTROL	0x03
#define KTD3133_REG_RATIO_LSB	0x04
#define KTD3133_REG_RATIO_MSB	0x05
#define KTD3133_REG_PWM		0x06
#define KTD3133_REG_RAMP_ON	0x07
#define KTD3133_REG_TRANS_RAMP	0x08

#define KTD3133_VENDOR_ID	0x18

#define KTD3133_RW_REG_MAX	7

#define BRIGHTNESS_LEVEL	256

struct ktd3133_backlight_information {
	/* whether support ktd3133 or not */
	unsigned int ktd3133_support;
	/* which i2c bus controller ktd3133 mount */
	unsigned int ktd3133_i2c_bus_id;
	unsigned int ktd3133_hw_en;
	/* ktd3133 hw_en gpio */
	unsigned int ktd3133_hw_en_gpio;
	unsigned int ktd3133_reg[KTD3133_RW_REG_MAX];
	unsigned int bl_on_lk_mdelay;
	unsigned int ktd3133_level_lsb;
	unsigned int ktd3133_level_msb;
};

int ktd3133_set_backlight_init(unsigned int bl_level);
void ktd3133_set_backlight_status(void);
int ktd3133_init(void);
#endif /* __LINUX_KTD3133_H */
