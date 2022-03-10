/*
 * nt50356.h
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

#ifndef __NT50356_H
#define __NT50356_H

#ifndef NT50356_OK
#define NT50356_OK 0
#endif

#ifndef NT50356_FAIL
#define NT50356_FAIL (-1)
#endif

#define NT50356_MODULE_NAME_STR             "nt50356"
#define NT50356_MODULE_NAME                  nt50356
#define NT50356_INVALID_VAL                  0xFFFF
#define NT50356_SLAV_ADDR                    0x11
#define NT50356_I2C_SPEED                    100
#define DTS_COMP_NT50356                     "nt,nt50356"
#define NT50356_SUPPORT                      "nt50356_support"
#define NT50356_I2C_BUS_ID                   "nt50356_i2c_bus_id"
#define NT50356_HW_EN_GPIO                   "nt50356_hw_en_gpio"
#define NT50356_HW_EN_DELAY                  "bl_on_lk_mdelay"
#define NT50356_BL_LEVEL                     "bl_level"
#define GPIO_NT50356_EN_NAME                 "nt50356_hw_enable"
#define NT50356_HIDDEN_REG_SUPPORT           "nt50356_hidden_reg_support"
#define NT50356_EXTEND_REV_SUPPORT           "nt50356_extend_rev_support"
#define NT50356_CONFIG1_STRING               "nt50356_bl_config_1"
#define NT50356_BL_EN_STRING                 "nt50356_bl_en"
#define NT50356_ONLY_BACKLIGHT "nt50356_only_backlight"
#define MAX_STR_LEN                          50

/* base reg */
#define REG_REVISION                         0x01
#define REG_BL_CONFIG_1                      0x02
#define REG_BL_CONFIG_2                      0x03
#define REG_BL_BRIGHTNESS_LSB                0x04
#define REG_BL_BRIGHTNESS_MSB                0x05
#define REG_AUTO_FREQ_LOW                    0x06
#define REG_AUTO_FREQ_HIGH                   0x07
#define REG_BL_ENABLE                        0x08
#define REG_DISPLAY_BIAS_CONFIG_1            0x09
#define REG_DISPLAY_BIAS_CONFIG_2            0x0A
#define REG_DISPLAY_BIAS_CONFIG_3            0x0B
#define REG_LCM_BOOST_BIAS                   0x0C
#define REG_VPOS_BIAS                        0x0D
#define REG_VNEG_BIAS                        0x0E
#define REG_FLAGS                            0x0F
#define REG_BL_OPTION_1                      0x10
#define REG_BL_OPTION_2                      0x11
#define REG_BL_CURRENT_CONFIG                0x20
#define NT50356_WRITE_LEN                    2
/* add config reg for nt50356 TSD bug */
#define NT50356_REG_CONFIG_A9                0xA9
#define NT50356_REG_CONFIG_E8                0xE8
#define NT50356_REG_CONFIG_E9                0xE9

#define VENDOR_ID_NT50356                    0x01
#define DEV_MASK                             0x03
#define VENDOR_ID_EXTEND_NT50356             0xE0
#define DEV_MASK_EXTEND_NT50356              0xE0

/* mask mode */
#define MASK_CHANGE_MODE                     0xFF

#define NT50356_RAMP_EN                      0x02
#define NT50356_RAMP_DIS                     0x00

#define NT50356_PWN_CONFIG_HIGH              0x00
#define NT50356_PWN_CONFIG_LOW               0x04

#define MASK_BL_LSB                          0x07
#define NT50356_BL_MIN 0
#define NT50356_BL_MAX 2047
#define NT50356_RW_REG_MAX                   19
#define MASK_LCM_EN                          0xE0

#define MSB                                  3
#define LSB                                  0x07
#define OVP_SHUTDOWN_ENABLE                  0x10
#define OVP_SHUTDOWN_DISABLE                 0x00

#define REG_HIDDEN_ADDR                      0x6A
#define REG_SET_SECURITYBIT_ADDR             0x50
#define REG_SET_SECURITYBIT_VAL              0x08
#define REG_CLEAR_SECURITYBIT_VAL            0x00
#define LCD_KIT_OK                           0
#define CHECK_STATUS_FAIL                    0
#define CHECK_STATUS_OK                      1
#define LCD_BL_LINEAR_EXPONENTIAL_TABLE_NUM  2048

#define LINEAR_EXPONENTIAL_MASK              0x08

#define NT50356_BL_DEFAULT_LEVEL             255
#define NT50356_EXTEND_REV_VAL               1

#define NT50356_VOL_400 (0x00)
#define NT50356_VOL_405 (0x01)
#define NT50356_VOL_410 (0x02)
#define NT50356_VOL_415 (0x03)
#define NT50356_VOL_420 (0x04)
#define NT50356_VOL_425 (0x05)
#define NT50356_VOL_430 (0x06)
#define NT50356_VOL_435 (0x07)
#define NT50356_VOL_440 (0x08)
#define NT50356_VOL_445 (0x09)
#define NT50356_VOL_450 (0x0A)
#define NT50356_VOL_455 (0x0B)
#define NT50356_VOL_460 (0x0C)
#define NT50356_VOL_465 (0x0D)
#define NT50356_VOL_470 (0x0E)
#define NT50356_VOL_475 (0x0F)
#define NT50356_VOL_480 (0x10)
#define NT50356_VOL_485 (0x11)
#define NT50356_VOL_490 (0x12)
#define NT50356_VOL_495 (0x13)
#define NT50356_VOL_500 (0x14)
#define NT50356_VOL_505 (0x15)
#define NT50356_VOL_510 (0x16)
#define NT50356_VOL_515 (0x17)
#define NT50356_VOL_520 (0x18)
#define NT50356_VOL_525 (0x19)
#define NT50356_VOL_530 (0x1A)
#define NT50356_VOL_535 (0x1B)
#define NT50356_VOL_540 (0x1C)
#define NT50356_VOL_545 (0x1D)
#define NT50356_VOL_550 (0x1E)
#define NT50356_VOL_555 (0x1F)
#define NT50356_VOL_560 (0x20)
#define NT50356_VOL_565 (0x21)
#define NT50356_VOL_570 (0x22)
#define NT50356_VOL_575 (0x23)
#define NT50356_VOL_580 (0x24)
#define NT50356_VOL_585 (0x25)
#define NT50356_VOL_590 (0x26)
#define NT50356_VOL_595 (0x27)
#define NT50356_VOL_600 (0x28)
#define NT50356_VOL_605 (0x29)
#define NT50356_VOL_640 (0x30)
#define NT50356_VOL_645 (0x31)
#define NT50356_VOL_650 (0x32)
#define NT50356_VOL_655 (0x33)
#define NT50356_VOL_660 (0x34)
#define NT50356_VOL_665 (0x35)
#define NT50356_VOL_670 (0x36)
#define NT50356_VOL_675 (0x37)
#define NT50356_VOL_680 (0x38)
#define NT50356_VOL_685 (0x39)
#define NT50356_VOL_690 (0x3A)
#define NT50356_VOL_695 (0x3B)
#define NT50356_VOL_700 (0x3C)


struct nt50356_voltage {
	unsigned int voltage;
	int value;
};

enum nt50356_bl_ovp {
	NT50356_BL_OVP_17 = 0x00,
	NT50356_BL_OVP_21 = 0x20,
	NT50356_BL_OVP_25 = 0x40,
	NT50356_BL_OVP_29 = 0x60,
};

enum nt50356_bled_mode {
	NT50356_BLED_MODE_EXPONETIAL = 0x00,
	NT50356_BLED_MODE_LINEAR = 0x08,
};

enum {
	NT50356_BIAS_BOOST_TIME_0 = 0x00, /* 156ns */
	NT50356_BIAS_BOOST_TIME_1 = 0x10, /* 181ns */
	NT50356_BIAS_BOOST_TIME_2 = 0x20, /* 206ns */
	NT50356_BIAS_BOOST_TIME_3 = 0x30, /* 231ns */
};

int nt50356_init(struct mtk_panel_info *pinfo);
void nt50356_set_backlight_status (void);
#endif /* __NT50356_H */
