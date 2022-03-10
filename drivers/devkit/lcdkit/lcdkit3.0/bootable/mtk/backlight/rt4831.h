/*
 * rt4831.h
 *
 * adapt for backlight driver
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

#ifndef __RT4831_H
#define __RT4831_H

#ifndef LCD_KIT_OK
#define LCD_KIT_OK 0
#endif

#ifndef LCD_KIT_FAIL
#define LCD_KIT_FAIL (-1)
#endif

#define RT4831_MODULE_NAME_STR "rt4831"
#define RT4831_MODULE_NAME rt4831
#define RT4831_INVALID_VAL 0xFFFF
#define RT4831_SLAV_ADDR 0x11
#define RT4831_I2C_SPEED 100

#define DTS_COMP_RT4831 "rt,rt4831"
#define RT4831_SUPPORT "rt4831_support"
#define RT4831_I2C_BUS_ID "rt4831_i2c_bus_id"
#define RT4831_HW_LDSEN_GPIO "rt4831_hw_ldsen_gpio"
#define RT4831_HW_EN_GPIO "rt4831_hw_en_gpio"
#define GPIO_RT4831_LDSEN_NAME "rt4831_hw_ldsen"
#define GPIO_RT4831_ENABLE "rt4831_hw_enable"
#define RT4831_HW_EN_DELAY "bl_on_lk_mdelay"
#define RT4831_BL_LEVEL "bl_level"
#define RT4831_HIDDEN_REG_SUPPORT "rt4831_hidden_reg_support"
#define RT4831_EXTEND_REV_SUPPORT "rt4831_extend_rev_support"
#define RT4831_ONLY_BACKLIGHT "rt4831_only_backlight"
#define MAX_STR_LEN 50
#define RT4831_WRITE_LEN 2

/* base reg */
#define PWM_DISABLE 0x00
#define PWM_ENABLE 0x01
#define REG_REVISION 0x01
#define REG_BL_CONFIG_1 0x02
#define REG_BL_CONFIG_2 0x03
#define REG_BL_BRIGHTNESS_LSB 0x04
#define REG_BL_BRIGHTNESS_MSB 0x05
#define REG_AUTO_FREQ_LOW 0x06
#define REG_AUTO_FREQ_HIGH 0x07
#define REG_RT4831_REG_HIDDEN_F0 0xF0
#define REG_RT4831_REG_HIDDEN_B1 0xB1
#define REG_BL_ENABLE 0x08
#define REG_DISPLAY_BIAS_CONFIG_1 0x09
#define REG_DISPLAY_BIAS_CONFIG_2 0x0A
#define REG_DISPLAY_BIAS_CONFIG_3 0x0B
#define REG_LCM_BOOST_BIAS 0x0C
#define REG_VPOS_BIAS 0x0D
#define REG_VNEG_BIAS 0x0E
#define REG_FLAGS 0x0F
#define REG_BL_OPTION_1 0x10
#define REG_BL_OPTION_2 0x11
#define REG_PWM_TO_DIGITAL_LSB 0x12
#define REG_PWM_TO_DIGITAL_MSB 0x13

/* mask mode */
#define MASK_CHANGE_MODE 0xFF

#define RT4831_RAMP_EN 0x02
#define RT4831_RAMP_DIS 0x00

#define RT4831_PWN_CONFIG_HIGH 0x00
#define RT4831_PWN_CONFIG_LOW 0x04

#define MASK_BL_LSB 0x07
#define RT4831_BL_MIN 0
#define RT4831_BL_MAX 2047
#define RT4831_RW_REG_MAX 17
#define MASK_LCM_EN 0xE0

#define MSB 3
#define LSB 0x07
#define OVP_SHUTDOWN_ENABLE 0x10
#define OVP_SHUTDOWN_DISABLE 0x00

#define REG_SET_SECURITYBIT_ADDR 0x50
#define REG_SET_SECURITYBIT_VAL 0x08
#define REG_CLEAR_SECURITYBIT_VAL 0x00

#define VENDOR_ID_RT 0x03
#define DEV_MASK 0x03
#define VENDOR_ID_EXTEND_RT4831 0xE0
#define DEV_MASK_EXTEND_RT4831 0xE0
#define RT4831_EXTEND_REV_VAL 1

#define CHECK_STATUS_FAIL 0
#define CHECK_STATUS_OK 1

#define RT4831_BL_DEFAULT_LEVEL 255

#define RT4831_VOL_400 (0x00)
#define RT4831_VOL_405 (0x01)
#define RT4831_VOL_410 (0x02)
#define RT4831_VOL_415 (0x03)
#define RT4831_VOL_420 (0x04)
#define RT4831_VOL_425 (0x05)
#define RT4831_VOL_430 (0x06)
#define RT4831_VOL_435 (0x07)
#define RT4831_VOL_440 (0x08)
#define RT4831_VOL_445 (0x09)
#define RT4831_VOL_450 (0x0A)
#define RT4831_VOL_455 (0x0B)
#define RT4831_VOL_460 (0x0C)
#define RT4831_VOL_465 (0x0D)
#define RT4831_VOL_470 (0x0E)
#define RT4831_VOL_475 (0x0F)
#define RT4831_VOL_480 (0x10)
#define RT4831_VOL_485 (0x11)
#define RT4831_VOL_490 (0x12)
#define RT4831_VOL_495 (0x13)
#define RT4831_VOL_500 (0x14)
#define RT4831_VOL_505 (0x15)
#define RT4831_VOL_510 (0x16)
#define RT4831_VOL_515 (0x17)
#define RT4831_VOL_520 (0x18)
#define RT4831_VOL_525 (0x19)
#define RT4831_VOL_530 (0x1A)
#define RT4831_VOL_535 (0x1B)
#define RT4831_VOL_540 (0x1C)
#define RT4831_VOL_545 (0x1D)
#define RT4831_VOL_550 (0x1E)
#define RT4831_VOL_555 (0x1F)
#define RT4831_VOL_560 (0x20)
#define RT4831_VOL_565 (0x21)
#define RT4831_VOL_570 (0x22)
#define RT4831_VOL_575 (0x23)
#define RT4831_VOL_580 (0x24)
#define RT4831_VOL_585 (0x25)
#define RT4831_VOL_590 (0x26)
#define RT4831_VOL_595 (0x27)
#define RT4831_VOL_600 (0x28)
#define RT4831_VOL_605 (0x29)
#define RT4831_VOL_640 (0x30)
#define RT4831_VOL_645 (0x31)
#define RT4831_VOL_650 (0x32)

struct rt4831_voltage {
	unsigned int voltage;
	int value;
};

struct rt4831_backlight_information {
	/* whether support rt4831 or not */
	unsigned int rt4831_support;
	/* which i2c bus controller rt4831 mount */
	unsigned int rt4831_i2c_bus_id;
	/* rt4831 hw_ldsen gpio */
	unsigned int rt4831_hw_ldsen_gpio;
	/* rt4831 hw_en gpio */
	unsigned int rt4831_hw_en;
	unsigned int rt4831_hw_en_gpio;
	unsigned int bl_on_lk_mdelay;
	unsigned int bl_level;
	unsigned int bl_only;
	int nodeoffset;
	void *pfdt;
	unsigned int rt4831_reg[RT4831_RW_REG_MAX];
};

enum rt4831_bl_ovp {
	RT4831_BL_OVP_17 = 0x00,
	RT4831_BL_OVP_21 = 0x20,
	RT4831_BL_OVP_25 = 0x40,
	RT4831_BL_OVP_29 = 0x60,
};

enum rt4831_bled_mode {
	RT4831_BLED_MODE_EXPONETIAL = 0x00,
	RT4831_BLED_MODE_LINEAR = 0x08,
};

enum {
	RT4831_BIAS_BOOST_TIME_0 = 0x00, /* 156ns */
	RT4831_BIAS_BOOST_TIME_1 = 0x10, /* 181ns */
	RT4831_BIAS_BOOST_TIME_2 = 0x20, /* 206ns */
	RT4831_BIAS_BOOST_TIME_3 = 0x30, /* 231ns */
};

int rt4831_init(struct mtk_panel_info *pinfo);
void rt4831_set_backlight_status(void);
#endif /* __RT4831_H */
