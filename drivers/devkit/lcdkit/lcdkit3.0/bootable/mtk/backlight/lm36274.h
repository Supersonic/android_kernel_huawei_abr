/*
* Simple driver for Texas Instruments LM3630 LED Flash driver chip
* Copyright (C) 2012 Texas Instruments
*
*/

#ifndef __LM36274_H
#define __LM36274_H

#ifndef LCD_KIT_OK
#define LCD_KIT_OK 0
#endif

#ifndef LCD_KIT_FAIL
#define LCD_KIT_FAIL (-1)
#endif

#define LM36274_MODULE_NAME_STR     "lm36274"
#define LM36274_MODULE_NAME          lm36274
#define LM36274_INVALID_VAL          0xFFFF
#define LM36274_SLAV_ADDR            0x11
#define LM36274_I2C_SPEED            100

#define DTS_COMP_LM36274             "ti,lm36274"
#define LM36274_SUPPORT              "lm36274_support"
#define LM36274_I2C_BUS_ID           "lm36274_i2c_bus_id"
#define LM36274_HW_LDSEN_GPIO        "lm36274_hw_ldsen_gpio"
#define LM36274_HW_EN_GPIO           "lm36274_hw_en_gpio"
#define GPIO_LM36274_LDSEN_NAME      "lm36274_hw_ldsen"
#define GPIO_LM36274_EN_NAME         "lm36274_hw_enable"
#define LM36274_HW_EN_DELAY          "bl_on_lk_mdelay"
#define LM36274_BL_LEVEL "bl_level"
#define LM36274_EXTEND_REV_SUPPORT   "lm36274_extend_rev_support"
#define LM36274_ONLY_BACKLIGHT "lm36274_only_backlight"
#define MAX_STR_LEN	                 50
#define LM36274_WRITE_LEN            2

/* base reg */
#define PWM_DISABLE                  0x00
#define PWM_ENABLE                   0x01
#define REG_REVISION                 0x01
#define REG_BL_CONFIG_1              0x02
#define REG_BL_CONFIG_2              0x03
#define REG_BL_BRIGHTNESS_LSB        0x04
#define REG_BL_BRIGHTNESS_MSB        0x05
#define REG_AUTO_FREQ_LOW            0x06
#define REG_AUTO_FREQ_HIGH           0x07
#define REG_BL_ENABLE                0x08
#define REG_DISPLAY_BIAS_CONFIG_1    0x09
#define REG_DISPLAY_BIAS_CONFIG_2    0x0A
#define REG_DISPLAY_BIAS_CONFIG_3    0x0B
#define REG_LCM_BOOST_BIAS           0x0C
#define REG_VPOS_BIAS                0x0D
#define REG_VNEG_BIAS                0x0E
#define REG_FLAGS                    0x0F
#define REG_BL_OPTION_1              0x10
#define REG_BL_OPTION_2              0x11
#define REG_PWM_TO_DIGITAL_LSB       0x12
#define REG_PWM_TO_DIGITAL_MSB       0x13

// mask mode
#define MASK_CHANGE_MODE             0xFF

#define LM36274_RAMP_EN              0x02
#define LM36274_RAMP_DIS             0x00

#define LM36274_PWN_CONFIG_HIGH      0x00
#define LM36274_PWN_CONFIG_LOW       0x04

#define MASK_BL_LSB                  0x07
#define LM36274_BL_MIN 0
#define LM36274_BL_MAX 2047
#define LM36274_RW_REG_MAX           15
#define MASK_LCM_EN                  0xE0

#define MSB                          3
#define LSB                          0x07
#define OVP_SHUTDOWN_ENABLE          0x10
#define OVP_SHUTDOWN_DISABLE         0x00

#define REG_HIDDEN_ADDR              0x6A
#define REG_SET_SECURITYBIT_ADDR     0x50
#define REG_SET_SECURITYBIT_VAL      0x08
#define REG_CLEAR_SECURITYBIT_VAL    0x00

#define VENDOR_ID_TI                 0x01
#define DEV_MASK                     0x03
#define VENDOR_ID_EXTEND_LM32674     0xE0
#define DEV_MASK_EXTEND_LM32674      0xE0
#define LM36274_EXTEND_REV_VAL       1

#define CHECK_STATUS_FAIL            0
#define CHECK_STATUS_OK              1

#define LM36274_BL_DEFAULT_LEVEL     255

#define LM36274_VOL_400 (0x00)
#define LM36274_VOL_405 (0x01)
#define LM36274_VOL_410 (0x02)
#define LM36274_VOL_415 (0x03)
#define LM36274_VOL_420 (0x04)
#define LM36274_VOL_425 (0x05)
#define LM36274_VOL_430 (0x06)
#define LM36274_VOL_435 (0x07)
#define LM36274_VOL_440 (0x08)
#define LM36274_VOL_445 (0x09)
#define LM36274_VOL_450 (0x0A)
#define LM36274_VOL_455 (0x0B)
#define LM36274_VOL_460 (0x0C)
#define LM36274_VOL_465 (0x0D)
#define LM36274_VOL_470 (0x0E)
#define LM36274_VOL_475 (0x0F)
#define LM36274_VOL_480 (0x10)
#define LM36274_VOL_485 (0x11)
#define LM36274_VOL_490 (0x12)
#define LM36274_VOL_495 (0x13)
#define LM36274_VOL_500 (0x14)
#define LM36274_VOL_505 (0x15)
#define LM36274_VOL_510 (0x16)
#define LM36274_VOL_515 (0x17)
#define LM36274_VOL_520 (0x18)
#define LM36274_VOL_525 (0x19)
#define LM36274_VOL_530 (0x1A)
#define LM36274_VOL_535 (0x1B)
#define LM36274_VOL_540 (0x1C)
#define LM36274_VOL_545 (0x1D)
#define LM36274_VOL_550 (0x1E)
#define LM36274_VOL_555 (0x1F)
#define LM36274_VOL_560 (0x20)
#define LM36274_VOL_565 (0x21)
#define LM36274_VOL_570 (0x22)
#define LM36274_VOL_575 (0x23)
#define LM36274_VOL_580 (0x24)
#define LM36274_VOL_585 (0x25)
#define LM36274_VOL_590 (0x26)
#define LM36274_VOL_595 (0x27)
#define LM36274_VOL_600 (0x28)
#define LM36274_VOL_605 (0x29)
#define LM36274_VOL_640 (0x30)
#define LM36274_VOL_645 (0x31)
#define LM36274_VOL_650 (0x32)


struct lm36274_voltage {
	u32 voltage;
	int value;
};

enum lm36274_bl_ovp {
	LM36274_BL_OVP_17 = 0x00,
	LM36274_BL_OVP_21 = 0x20,
	LM36274_BL_OVP_25 = 0x40,
	LM36274_BL_OVP_29 = 0x60,
};

enum {
	LM36274_BIAS_BOOST_TIME_0 = 0x00, /* 156ns */
	LM36274_BIAS_BOOST_TIME_1 = 0x10, /* 181ns */
	LM36274_BIAS_BOOST_TIME_2 = 0x20, /* 206ns */
	LM36274_BIAS_BOOST_TIME_3 = 0x30, /* 231ns */
};

enum lm36274_bled_mode {
	LM36274_BLED_MODE_EXPONETIAL = 0x00,
	LM36274_BLED_MODE_LINEAR = 0x08,
};

void lm36274_set_backlight_status (void);
int lm36274_init(struct mtk_panel_info *pinfo);
#endif /* __LM36274_H */
