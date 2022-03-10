/*
 * rt8555.h
 *
 * rt8555 driver for backlight
 *
 * Copyright (c) 2019 Huawei Technologies Co., Ltd.
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

#ifndef __LINUX_RT8555_H
#define __LINUX_RT8555_H

#ifndef RT8555_OK
#define RT8555_OK 0
#endif

#ifndef RT8555_FAIL
#define RT8555_FAIL (-1)
#endif

#define RT8555_I2C_SPEED                    100
#define RT8555_SLAV_ADDR   0x31
#define RT8555_RW_REG_MAX  11
#define RT8555_WRITE_LEN                    2
#define RT8555_BL_DEFAULT_LEVEL             255
#define PARSE_FAILED        0xffff

#define DTS_COMP_RT8555      "realtek,rt8555"
#define RT8555_SUPPORT       "rt8555_support"
#define RT8555_I2C_BUS_ID    "rt8555_i2c_bus_id"
#define RT8555_2_I2C_BUS_ID    "rt8555_2_i2c_bus_id"
#define RT8555_HW_EN_GPIO    "rt8555_hw_en_gpio"
#define RT8555_2_HW_EN_GPIO "rt8555_2_hw_en_gpio"
#define GPIO_RT8555_EN_NAME  "rt8555_hw_enable"

#define RT8555_HW_EN_DELAY	"bl_on_lk_mdelay"
#define RT8555_BL_LEVEL	"bl_level"
#define RT8555_HW_DUAL_IC	"dual_ic"

/* rt8555 reg address */
#define RT8555_CONTROL_MODE_ADDR           0x00
#define RT8555_CURRENT_PROTECTION_ADDR     0x01
#define RT8555_CURRENT_SETTING_ADDR        0x02
#define RT8555_VOLTAGE_SETTING_ADDR        0x03
#define RT8555_BRIGHTNESS_SETTING_ADDR     0x08
#define RT8555_TIME_CONTROL_ADDR           0x09
#define RT8555_MODE_DEVISION_ADDR          0x0A
#define RT8555_COMPENSATION_DUTY_ADDR      0x0B
#define RT8555_CLK_PFM_ENABLE_ADDR         0x0D
#define RT8555_LED_PROTECTION_ADDR         0x0E
#define RT8555_REG_CONFIG_50               0x50

struct rt8555_backlight_info {
	/* whether support rt8555 or not */
	u32 rt8555_support;
	/* which i2c bus controller rt8555 mount */
	int rt8555_i2c_bus_id;
	int rt8555_2_i2c_bus_id;
	u32 rt8555_hw_en;
	/* rt8555 hw_en gpio */
	u32 rt8555_hw_en_gpio;
	u32 rt8555_2_hw_en_gpio;
	/* Dual rt8555 ic */
	u32 dual_ic;
	u32 reg[RT8555_RW_REG_MAX];
	u32 bl_on_lk_mdelay;
	u32 bl_level;
	int nodeoffset;
	void *pfdt;
	int rt8555_level_lsb;
	int rt8555_level_msb;
};

int rt8555_init(struct mtk_panel_info *pinfo);
void rt8555_set_backlight_status (void);

#endif /* _BL_RT8555_H_ */
