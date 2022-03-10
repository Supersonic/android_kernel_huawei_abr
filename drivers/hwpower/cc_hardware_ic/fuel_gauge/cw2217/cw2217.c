// SPDX-License-Identifier: GPL-2.0
/*
 * cw2217.c
 *
 * driver for cw2217 battery fuel gauge
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/workqueue.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/sizes.h>
#include <chipset_common/hwpower/coul/coul_interface.h>
#include <chipset_common/hwpower/coul/coul_calibration.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_delay.h>
#include <chipset_common/hwpower/common_module/power_i2c.h>
#include <chipset_common/hwpower/common_module/power_log.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/battery/battery_model_public.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_devices_info.h>
#include <chipset_common/hwpower/common_module/power_cmdline.h>
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
#include <chipset_common/hwmanufac/dev_detect/dev_detect.h>
#endif

#define HWLOG_TAG cw2217
HWLOG_REGIST();

#define CW2217_REG_CHIP_ID               0x00
#define CW2217_REG_VCELL_H               0x02
#define CW2217_REG_SOC_INT               0x04
#define CW2217_REG_TEMP                  0x06
#define CW2217_REG_MODE_CONFIG           0x08
#define CW2217_REG_GPIO_CONFIG           0x0A
#define CW2217_REG_SOC_ALERT             0x0B
#define CW2217_REG_CURRENT_H             0x0E
#define CW2217_REG_USER_CONF             0xA2
#define CW2217_REG_CYCLE_H               0xA4
#define CW2217_REG_SOH                   0xA6
#define CW2217_REG_IC_STATE              0xA7
#define CW2217_REG_FW_VERSION            0xAB
#define CW2217_REG_BAT_PROFILE           0x10

#define CW2217_CONFIG_MODE_RESTART       0x30
#define CW2217_CONFIG_MODE_ACTIVE        0x00
#define CW2217_CONFIG_MODE_SLEEP         0xF0
#define CW2217_CONFIG_UPDATE_FLG         0x80
#define CW2217_IC_VCHIP_ID               0xA0
#define CW2217_IC_READY_MARK             0x0C
#define CW2217_GPIO_SOC_IRQ_VALUE        0x00

#define CW2217_COMPLEMENT_CODE_U16       0x8000
#define CW2217_QUEUE_DELAYED_WORK_TIME   5000
#define CW2217_QUEUE_START_WORK_TIME     50
#define CW2217_QUEUE_RESUME_WORK_TIME    20

#define CW2217_SLEEP_COUNT               50
#define CW2217_RETRY_COUNT               3
#define CW2217_VOL_MAGIC_PART1           5
#define CW2217_VOL_MAGIC_PART2           16
#define CW2217_UI_FULL                   100
#define CW2217_SOC_MAGIC_BASE            256
#define CW2217_SOC_MAGIC_100             100
#define CW2217_TEMP_MAGIC_PART1          10
#define CW2217_TEMP_MAGIC_PART2          2
#define CW2217_TEMP_MAGIC_PART3          400

#define CW2217_CYCLE_MAGIC               16
#define CW2217_FULL_CAPACITY             100
#define CW2217_TEMP_ABR_LOW              (-400)
#define CW2217_TEMP_ABR_HIGH             800
#define CW2217_NOT_ACTIVE                1
#define CW2217_PROFILE_NOT_READY         2
#define CW2217_PROFILE_NEED_UPDATE       3
#define CW2217_CAPACITY_TH               7
#define CW2217_SIZE_OF_PROFILE           80
#define CW2217_COEFFICIENT_DEFAULT_VAL   1000000
#define CW2217_RESISTANCE_DEFAULT_VAL    1000
#define CW2217_SOH_DEFAULT_VAL           100
#define CW2217_RATED_CAPACITY_VAL        4000
#define CW2217_RSENSE_CORRECT_UP         63
#define CW2217_RSENSE_CORRECT_DOWN       (-192)
#define CW2217_TBATICAL_MIN_A            883000
#define CW2217_TBATICAL_MAX_A            1154000
#define CW2217_LEN_U8_REG                1
#define CW2217_LEN_U16_DAT               2
#define CW2217_FLAG_UPDATE_SOC           90
#define CW2217_ERROR_UI_SOC              25
#define CW2217_ERROR_VOLTAGE             3900

struct cw2217_dev {
	struct i2c_client *client;
	struct device *dev;
	struct workqueue_struct *cwfg_workqueue;
	struct delayed_work battery_delay_work;
	u8 config_profile_info[CW2217_SIZE_OF_PROFILE];
	u32 ic_role;
	int rated_capacity;
	int coefficient;
	int resistance;
	int chip_id;
	int voltage;
	int ic_soc_h;
	int ic_soc_l;
	int ui_soc;
	int temp;
	int curr;
	int cycle;
	int soh;
	int fw_version;
	bool print_error_flag;
};

enum {
	CW2217_IC_TYPE_MAIN = 0,
	CW2217_IC_TYPE_AUX,
	CW2217_IC_TYPE_MAX,
};

static int cw2217_read_byte(struct i2c_client *client, u8 reg, u8 *value)
{
	if (!client) {
		hwlog_err("chip not init\n");
		return -ENODEV;
	}

	return power_i2c_u8_read_byte(client, reg, value);
}

static int cw2217_write_byte(struct i2c_client *client, u8 reg, u8 value)
{
	if (!client) {
		hwlog_err("chip not init\n");
		return -ENODEV;
	}

	return power_i2c_u8_write_byte(client, reg, value);
}

static int cw2217_read_word(struct i2c_client *client, u8 reg, u16 *value)
{
	u8 cmd[CW2217_LEN_U8_REG] = { 0 };
	u8 buf0[CW2217_LEN_U16_DAT] = { 0 };
	u8 buf1[CW2217_LEN_U16_DAT] = { 0 };

	cmd[0] = reg; /* cmd[0]: 8bit register */

	if (!client) {
		hwlog_err("chip not init\n");
		return -ENODEV;
	}

	if (power_i2c_read_block(client, cmd, sizeof(cmd), buf0, sizeof(buf0)))
		return -EIO;
	usleep_range(500, 600); /* delay ranges from 500 us to 600 us */
	if (power_i2c_read_block(client, cmd, sizeof(cmd), buf1, sizeof(buf1)))
		return -EIO;

	if (buf0[0] != buf1[0]) /* high-8bit data not equal can make sure buf1 is right data */
		*value = (buf1[1] | (buf1[0] << 8)); /* buf1 is the right data */
	else
		*value = (buf0[1] | (buf0[0] << 8)); /* buf0 is the right data */

	return 0;
}

/* write profile function */
static int cw2217_write_profile(struct i2c_client *client, u8 size,
	struct cw2217_dev *di)
{
	int ret;
	int i;

	for (i = 0; i < size; i++) {
		ret = cw2217_write_byte(client, CW2217_REG_BAT_PROFILE + i,
			di->config_profile_info[i]);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static int cw2217_parse_ocv_table(struct device *dev, struct device_node *np,
	struct cw2217_dev *di)
{
	int i;
	int ret;
	u32 ocv_table_data[CW2217_SIZE_OF_PROFILE] = { 0 };

	ret = power_dts_read_u32_array(power_dts_tag(HWLOG_TAG), np,
		"cw,fg_ocv_table",
		ocv_table_data, CW2217_SIZE_OF_PROFILE);
	if (ret < 0)
		return ret;

	for (i = 0; i < CW2217_SIZE_OF_PROFILE; i++)
		di->config_profile_info[i] = (u8)ocv_table_data[i];

	return 0;
}

static struct device_node *cw2217_get_child_node(struct device *dev)
{
	const char *battery_name = NULL;
	const char *batt_model_name = NULL;
	struct device_node *np = dev->of_node;
	struct device_node *child_node = NULL;
	struct device_node *default_node = NULL;

	batt_model_name = bat_model_name();
	for_each_child_of_node(np, child_node) {
		if (power_dts_read_string(power_dts_tag(HWLOG_TAG),
			child_node, "batt_name", &battery_name)) {
			hwlog_info("childnode without batt_name property");
			continue;
		}
		if (!battery_name)
			continue;
		if (!default_node)
			default_node = child_node;
		hwlog_info("search battery data, battery_name: %s\n", battery_name);
		if (!batt_model_name || !strcmp(battery_name, batt_model_name))
			break;
	}

	if (!child_node) {
		if (default_node) {
			hwlog_info("cannt match childnode, use first\n");
			child_node = default_node;
		} else {
			hwlog_info("cannt find any childnode, use father\n");
			child_node = np;
		}
	}

	return child_node;
}

static int cw2217_parse_dt(struct device *dev, struct cw2217_dev *di)
{
	int ret;
	struct device_node *np = dev->of_node;
	struct device_node *child_node = cw2217_get_child_node(dev);

	if (!child_node)
		return -ENOMEM;

	ret = cw2217_parse_ocv_table(dev, child_node, di);
	if (ret < 0)
		return ret;

	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np, "ic_role",
		&di->ic_role, CW2217_IC_TYPE_MAIN);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), child_node,
		"cw,resistance", (u32 *)&di->resistance, CW2217_RESISTANCE_DEFAULT_VAL);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), child_node,
		"cw,rated_capacity", (u32 *)&di->rated_capacity, CW2217_RATED_CAPACITY_VAL);

	return 0;
}

/*
 * Active function
 * The CONFIG register is used for the host MCU to configure the fuel gauge IC. The default value is 0xF0,
 * SLEEP and RESTART bits are set. To power up the IC, the host MCU needs to write 0x30 to exit shutdown
 * mode, and then write 0x00 to restart the gauge to enter active mode. To reset the IC, the host MCU needs
 * to write 0xF0, 0x30 and 0x00 in sequence to this register to complete the restart procedure. The CW2217B
 * will reload relevant parameters and settings and restart SOC calculation. Note that the SOC may be a
 * different value after reset operation since it is a brand-new calculation based on the latest battery status.
 * CONFIG [3:0] is reserved. Don't do any operation with it.
 */
static int cw2217_enter_active_mode(struct cw2217_dev *di)
{
	int ret;
	u8 reg_val = CW2217_CONFIG_MODE_RESTART;

	ret = cw2217_write_byte(di->client, CW2217_REG_MODE_CONFIG, reg_val);
	if (ret < 0)
		return ret;
	msleep(DT_MSLEEP_20MS);

	reg_val = CW2217_CONFIG_MODE_ACTIVE;
	ret = cw2217_write_byte(di->client, CW2217_REG_MODE_CONFIG, reg_val);
	if (ret < 0)
		return ret;
	msleep(DT_MSLEEP_10MS);

	return 0;
}

/*
 * Sleep function
 * The CONFIG register is used for the host MCU to configure the fuel gauge IC. The default value is 0xF0,
 * SLEEP and RESTART bits are set. To power up the IC, the host MCU needs to write 0x30 to exit shutdown
 * mode, and then write 0x00 to restart the gauge to enter active mode. To reset the IC, the host MCU needs
 * to write 0xF0, 0x30 and 0x00 in sequence to this register to complete the restart procedure. The CW2217B
 * will reload relevant parameters and settings and restart SOC calculation. Note that the SOC may be a
 * different value after reset operation since it is a brand-new calculation based on the latest battery status.
 * CONFIG [3:0] is reserved. Don't do any operation with it.
 */
static int cw2217_enter_sleep_mode(struct cw2217_dev *di)
{
	int ret;
	u8 reg_val = CW2217_CONFIG_MODE_RESTART;

	ret = cw2217_write_byte(di->client, CW2217_REG_MODE_CONFIG, reg_val);
	if (ret < 0)
		return ret;
	msleep(DT_MSLEEP_20MS);

	reg_val = CW2217_CONFIG_MODE_SLEEP;
	ret = cw2217_write_byte(di->client, CW2217_REG_MODE_CONFIG, reg_val);
	if (ret < 0)
		return ret;
	msleep(DT_MSLEEP_10MS);

	return 0;
}

/*
 * The 0x00 register is an UNSIGNED 8bit read-only register. Its value is fixed to 0xA0 in shutdown
 * mode and active mode.
 */
static int cw2217_get_chip_id(int *value, struct cw2217_dev *di)
{
	int ret;
	u8 reg_val = 0;
	int chip_id;

	ret = cw2217_read_byte(di->client, CW2217_REG_CHIP_ID, &reg_val);
	if (ret < 0)
		return ret;

	chip_id = reg_val; /* This value must be 0xA0 */
	hwlog_info("chip_id = %d\n", chip_id);
	*value = chip_id;

	return 0;
}

static int cw2217_update_chip_id(struct cw2217_dev *di)
{
	int ret;
	int chip_id = 0;

	ret = cw2217_get_chip_id(&chip_id, di);
	if (ret < 0)
		return ret;
	di->chip_id = chip_id;

	return 0;
}

static int cw2217_read_chip_id(struct cw2217_dev *di)
{
	int ret = cw2217_update_chip_id(di);

	if (ret < 0) {
		hwlog_err("iic read write error");
		return ret;
	}

	if (di->chip_id != CW2217_IC_VCHIP_ID) {
		hwlog_err("not cw2217B\n");
		return -EPERM;
	}

	return 0;
}

static int cw2217_check_chip(struct cw2217_dev *di)
{
	int retry = CW2217_RETRY_COUNT;
	int ret;

	do {
		ret = cw2217_read_chip_id(di);
		if (!ret)
			return 0;
	} while (retry-- > 0);
	if (ret)
		hwlog_err("chek chip fail\n");

	return ret;
}

/*
 * The VCELL register(0x02 0x03) is an UNSIGNED 14bit read-only register that updates the battery voltage continuously.
 * Battery voltage is measured between the VCELL pin and VSS pin, which is the ground reference. A 14bit
 * sigma-delta A/D converter is used and the voltage resolution is 312.5uV. (0.3125mV is *5/16)
 */
static int cw2217_get_voltage(int *value, struct cw2217_dev *di)
{
	int ret;
	u16 voltage = 0;

	ret = cw2217_read_word(di->client, CW2217_REG_VCELL_H, &voltage);
	if (ret < 0)
		return ret;

	voltage = voltage * CW2217_VOL_MAGIC_PART1 / CW2217_VOL_MAGIC_PART2;
	*value = voltage;

	return 0;
}

int cw2217_update_voltage(struct cw2217_dev *di)
{
	int ret;
	int voltage = 0;

	ret = cw2217_get_voltage(&voltage, di);
	if (ret < 0)
		return ret;
	di->voltage = voltage;

	return 0;
}

/*
 * The SOC register(0x04 0x05) is an UNSIGNED 16bit read-only register that indicates the SOC of the battery. The
 * SOC shows in % format, which means how much percent of the battery's total available capacity is
 * remaining in the battery now. The SOC can intrinsically adjust itself to cater to the change of battery status,
 * including load, temperature and aging etc.
 * The high byte(0x04) contains the SOC in 1% unit which can be directly used if this resolution is good
 * enough for the application. The low byte(0x05) provides more accurate fractional part of the SOC and its
 * LSB is (1/256) %.
 */
static int cw2217_get_capacity(int *value, struct cw2217_dev *di)
{
	int ret;
	u16 reg_val = 0;
	int soc_h;
	int soc_l;
	int ui_soc;
	int remainder;

	ret = cw2217_read_word(di->client, CW2217_REG_SOC_INT, &reg_val);
	if (ret < 0)
		return ret;
	soc_h = (reg_val & 0xFF00) >> 8;
	soc_l = reg_val & 0xFF;
	ui_soc = ((soc_h * CW2217_SOC_MAGIC_BASE + soc_l) * CW2217_SOC_MAGIC_100) /
		(CW2217_UI_FULL * CW2217_SOC_MAGIC_BASE);
	remainder = (((soc_h * CW2217_SOC_MAGIC_BASE + soc_l) * CW2217_SOC_MAGIC_100 *
		CW2217_SOC_MAGIC_100) / (CW2217_UI_FULL * CW2217_SOC_MAGIC_BASE)) % CW2217_SOC_MAGIC_100;
	if (ui_soc >= CW2217_SOC_MAGIC_100) {
		hwlog_info("UI_SOC = %d larger 100\n", ui_soc);
		ui_soc = CW2217_SOC_MAGIC_100;
	}
	di->ic_soc_h = soc_h;
	di->ic_soc_l = soc_l;
	*value = ui_soc;

	return 0;
}

static int cw2217_update_ui_soc(struct cw2217_dev *di)
{
	int ret;
	int ui_soc = 0;

	ret = cw2217_get_capacity(&ui_soc, di);
	if (ret < 0)
		return ret;
	di->ui_soc = ui_soc;

	return 0;
}

static int cw2217_write_reg_capacity(struct cw2217_dev *di, int capacity)
{
	int ret;
	u8 reg_val = (u8)capacity;

	ret = cw2217_write_byte(di->client, CW2217_REG_USER_CONF, reg_val);
	if (ret < 0)
		return ret;

	return 0;
}

static int cw2217_read_reg_capacity(struct cw2217_dev *di)
{
	int ret;
	int ui_soc;
	u8 reg_val = 0;

	ret = cw2217_read_byte(di->client, CW2217_REG_USER_CONF, &reg_val);
	if (ret < 0)
		return ret;
	ui_soc = (int)reg_val;

	return ui_soc;
}

/*
 * The TEMP register is an UNSIGNED 8bit read only register.
 * It reports the real-time battery temperature
 * measured at TS pin. The scope is from -40 to 87.5 degrees Celsius,
 * LSB is 0.5 degree Celsius. TEMP(C) = - 40 + Value(0x06 Reg) / 2
 */
static int cw2217_get_temp(int *value, struct cw2217_dev *di)
{
	int ret;
	u8 reg_val = 0;
	int temp;

	ret = cw2217_read_byte(di->client, CW2217_REG_TEMP, &reg_val);
	if (ret < 0)
		return ret;

	temp = (int)reg_val * CW2217_TEMP_MAGIC_PART1 / CW2217_TEMP_MAGIC_PART2 - CW2217_TEMP_MAGIC_PART3;
	*value = temp;

	return 0;
}

static int cw2217_update_temp(struct cw2217_dev *di)
{
	int ret;
	int temp = 0;

	ret = cw2217_get_temp(&temp, di);
	if (ret < 0)
		return ret;
	di->temp = temp;

	return 0;
}

/* get complement code function, unsigned short must be U16 */
static long cw2217_get_complement_code(unsigned short raw_code)
{
	long complement_code;
	int dir;

	if ((raw_code & CW2217_COMPLEMENT_CODE_U16) != 0) {
		dir = -1;
		raw_code =  (~raw_code) + 1;
	} else {
		dir = 1;
	}
	complement_code = (long)raw_code * dir;

	return complement_code;
}

/*
 * CURRENT is a SIGNED 16bit register(0x0E 0x0F) that reports current A/D converter result of the voltage across the
 * current sense resistor, 10mohm typical. The result is stored as a two's complement value to show positive
 * and negative current. Voltages outside the minimum and maximum register values are reported as the
 * minimum or maximum value.
 * The register value should be divided by the sense resistance to convert to amperes. The value of the
 * sense resistor determines the resolution and the full-scale range of the current readings. The LSB of 0x0F
 * is (52.4/32768)uV.
 * The default value is 0x0000, stands for 0mA. 0x7FFF stands for the maximum charging current and 0x8001 stands for
 * the maximum discharging current.
 */
static int cw2217_get_current(int *value, struct cw2217_dev *di)
{
	int ret;
	s64 curr;
	u16 current_reg = 0;

	ret = cw2217_read_word(di->client, CW2217_REG_CURRENT_H, &current_reg);
	if (ret < 0)
		return ret;

	curr = cw2217_get_complement_code(current_reg);
	curr = curr * 1600 / di->resistance; /* I(A) = 0.0016(uV) * value / resistance(mR) */
	curr = curr * di->coefficient / CW2217_COEFFICIENT_DEFAULT_VAL;
	*value = curr;

	return 0;
}

static int cw2217_update_current(struct cw2217_dev *di)
{
	int ret;
	int curr = 0;

	ret = cw2217_get_current(&curr, di);
	if (ret < 0)
		return ret;
	di->curr = curr;

	return 0;
}

/*
 * CYCLECNT is an UNSIGNED 16bit register(0xA4 0xA5) that counts cycle life of the battery. The LSB of 0xA5 stands
 * for 1/16 cycle. This register will be clear after enters shutdown mode
 */
static int cw2217_get_cycle_count(int *value, struct cw2217_dev *di)
{
	int ret;
	u16 cycle = 0;

	ret = cw2217_read_word(di->client, CW2217_REG_CYCLE_H, &cycle);
	if (ret < 0)
		return ret;
	*value = cycle / CW2217_CYCLE_MAGIC;

	return 0;
}

static int cw2217_update_cycle_count(struct cw2217_dev *di)
{
	int ret;
	int cycle_count = 0;

	ret = cw2217_get_cycle_count(&cycle_count, di);
	if (ret < 0)
		return ret;
	di->cycle = cycle_count;

	return 0;
}

/*
 * SOH (State of Health) is an UNSIGNED 8bit register(0xA6) that represents the level of battery aging by tracking
 * battery internal impedance increment. When the device enters active mode, this register refresh to 0x64
 * by default. Its range is 0x00 to 0x64, indicating 0 to 100%. This register will be clear after enters shutdown
 * mode.
 */
static int cw2217_get_soh(int *value, struct cw2217_dev *di)
{
	int ret;
	u8 reg_val = 0;
	int soh;

	ret = cw2217_read_byte(di->client, CW2217_REG_SOH, &reg_val);
	if (ret < 0)
		return ret;

	soh = reg_val;
	*value = soh;

	return 0;
}

static int cw2217_update_soh(struct cw2217_dev *di)
{
	int ret;
	int soh = 0;

	ret = cw2217_get_soh(&soh, di);
	if (ret < 0)
		return ret;
	di->soh = soh;

	return 0;
}

/*
 * FW_VERSION register reports the firmware (FW) running in the chip. It is fixed to 0x00 when the chip is
 * in shutdown mode. When in active mode, Bit [7:6] are fixed to '01', which stand for the CW2217B and Bit
 * [5:0] stand for the FW version running in the chip. Note that the FW version is subject to update and contact
 * sales office for confirmation when necessary.
 */
static int cw2217_get_fw_version(int *value, struct cw2217_dev *di)
{
	int ret;
	u8 reg_val = 0;
	int fw_version;

	ret = cw2217_read_byte(di->client, CW2217_REG_FW_VERSION, &reg_val);
	if (ret < 0)
		return ret;

	fw_version = reg_val;
	*value = fw_version;

	return 0;
}

static int cw2217_update_fw_version(struct cw2217_dev *di)
{
	int ret;
	int fw_version = 0;

	ret = cw2217_get_fw_version(&fw_version, di);
	if (ret < 0)
		return ret;
	di->fw_version = fw_version;

	return 0;
}

static int cw2217_update_data(struct cw2217_dev *di)
{
	int ret = 0;

	ret += cw2217_update_voltage(di);
	ret += cw2217_update_ui_soc(di);
	ret += cw2217_update_temp(di);
	ret += cw2217_update_current(di);
	ret += cw2217_update_cycle_count(di);
	ret += cw2217_update_soh(di);
	hwlog_info("vol = %d  current = %d cap = %d temp = %d\n",
		di->voltage, di->curr, di->ui_soc, di->temp);

	return ret;
}

static int cw2217_init_data(struct cw2217_dev *di)
{
	int ret = 0;

	ret += cw2217_update_chip_id(di);
	ret += cw2217_update_voltage(di);
	ret += cw2217_update_ui_soc(di);
	ret += cw2217_update_temp(di);
	ret += cw2217_update_current(di);
	ret += cw2217_update_cycle_count(di);
	ret += cw2217_update_soh(di);
	ret += cw2217_update_fw_version(di);
	hwlog_info("chip_id = %d vol = %d  cur = %d cap = %d temp = %d  fw_version = %d\n",
		di->chip_id, di->voltage, di->curr,
		di->ui_soc, di->temp, di->fw_version);

	return ret;
}

/* CW2217 update profile function, Often called during initialization */
static int cw2217_config_start_ic(struct cw2217_dev *di)
{
	int ret;
	u8 reg_val;
	int count = 0;

	ret = cw2217_enter_sleep_mode(di);
	if (ret < 0)
		return ret;

	/* update new battery info */
	ret = cw2217_write_profile(di->client, CW2217_SIZE_OF_PROFILE, di);
	if (ret < 0)
		return ret;

	/* set update flag and soc interrupt value */
	reg_val = CW2217_CONFIG_UPDATE_FLG | CW2217_GPIO_SOC_IRQ_VALUE;
	ret = cw2217_write_byte(di->client, CW2217_REG_SOC_ALERT, reg_val);
	if (ret < 0)
		return ret;

	/* close all interruptes */
	reg_val = 0;
	ret = cw2217_write_byte(di->client, CW2217_REG_GPIO_CONFIG, reg_val);
	if (ret < 0)
		return ret;

	ret = cw2217_enter_active_mode(di);
	if (ret < 0)
		return ret;

	while (true) {
		msleep(DT_MSLEEP_100MS);
		cw2217_read_byte(di->client, CW2217_REG_IC_STATE, &reg_val);
		if ((reg_val & CW2217_IC_READY_MARK) == CW2217_IC_READY_MARK)
			break;
		count++;
		if (count >= CW2217_SLEEP_COUNT) {
			cw2217_enter_sleep_mode(di);
			return -EPERM;
		}
	}

	return 0;
}

/*
 * Get the cw2217 running state
 * Determine whether the profile needs to be updated
 */
static int cw2217_get_state(struct cw2217_dev *di)
{
	int ret;
	u8 reg_val = 0;
	int i;
	int reg_profile;

	ret = cw2217_read_byte(di->client, CW2217_REG_MODE_CONFIG, &reg_val);
	if (ret < 0)
		return ret;
	if (reg_val != CW2217_CONFIG_MODE_ACTIVE)
		return CW2217_NOT_ACTIVE;

	ret = cw2217_read_byte(di->client, CW2217_REG_SOC_ALERT, &reg_val);
	if (ret < 0)
		return ret;
	if ((reg_val & CW2217_CONFIG_UPDATE_FLG) == 0x00)
		return CW2217_PROFILE_NOT_READY;

	if (power_cmdline_is_erecovery_mode() || power_cmdline_is_recovery_mode()) {
		hwlog_info("recovery_mode not update profile\n");
		return 0;
	}

	for (i = 0; i < CW2217_SIZE_OF_PROFILE; i++) {
		ret = cw2217_read_byte(di->client, (CW2217_REG_BAT_PROFILE + i),
			&reg_val);
		if (ret < 0)
			return ret;
		reg_profile = CW2217_REG_BAT_PROFILE + i;
		hwlog_info("0x%2x = 0x%2x\n", reg_profile, reg_val);
		if (di->config_profile_info[i] != reg_val)
			break;
	}
	if (i < CW2217_SIZE_OF_PROFILE)
		return CW2217_PROFILE_NEED_UPDATE;

	return 0;
}

static void cw2217_init_calibration_para(struct cw2217_dev *di)
{
	int curr_gain = 0;

	/* main battery gauge use aux calibration data for compatible */
	if (di->ic_role == CW2217_IC_TYPE_MAIN)
		coul_cali_get_para(COUL_CALI_MODE_AUX, COUL_CALI_PARA_CUR_A, &curr_gain);
	else
		coul_cali_get_para(COUL_CALI_MODE_MAIN, COUL_CALI_PARA_CUR_A, &curr_gain);

	if (curr_gain) {
		if (curr_gain < CW2217_TBATICAL_MIN_A)
			curr_gain = CW2217_TBATICAL_MIN_A;
		else if (curr_gain > CW2217_TBATICAL_MAX_A)
			curr_gain = CW2217_TBATICAL_MAX_A;
		di->coefficient = curr_gain;
	} else {
		di->coefficient = CW2217_COEFFICIENT_DEFAULT_VAL;
	}
	hwlog_info("coefficient = %u\n", di->coefficient);
}

static int cw2217_calibrate_rsense(unsigned int coefficient)
{
	long value;
	int round_mark = 500000; /* 500000: used for rounding */

	if (coefficient == CW2217_COEFFICIENT_DEFAULT_VAL) {
		hwlog_info("Calibration is not required\n");
		return -EPERM;
	}
	value = coefficient - CW2217_COEFFICIENT_DEFAULT_VAL;
	value = 1024 * value; /* multiply by 1024 to avoid floating point arithmetic */
	if (value < 0)
		round_mark = 0 - round_mark;

	value = (value + round_mark) / CW2217_COEFFICIENT_DEFAULT_VAL;
	if ((value > CW2217_RSENSE_CORRECT_UP) || (value < CW2217_RSENSE_CORRECT_DOWN) || (value == 0)) {
		hwlog_info("Calibration is error\n");
		return -EPERM;
	} else if (value < 0) {
		value = 256 + value; /* to get a positive number */
	}

	return value;
}

static int cw2217_calculate_crc_value(struct cw2217_dev *di)
{
	unsigned char crcvalue = 0x55; /* 0x55: crc odd check initial value */
	int i;
	int j;
	unsigned char temp_profile_one;

	for (i = 0; i < CW2217_SIZE_OF_PROFILE - 1; i++) {
		temp_profile_one = di->config_profile_info[i];
		crcvalue = crcvalue ^ temp_profile_one;
		for (j = 0; j < 8; j++) { /* 8: one byte */
			if ((crcvalue & 0x80)) {
				crcvalue <<= 1;
				crcvalue ^= 0x07;
			} else {
				crcvalue <<= 1;
			}
		}
	}

	return crcvalue;
}

static void cw2217_calibrate_profile(struct cw2217_dev *di)
{
	int ret;
	unsigned char byte7_value;
	unsigned char byte80_value;

	cw2217_init_calibration_para(di);

	ret = cw2217_calibrate_rsense(di->coefficient);
	if (ret < 0)
		return;
	byte7_value = ret;
	di->config_profile_info[6] = byte7_value; /* 6: the seventh element of the array needs to be updated */
	byte80_value = cw2217_calculate_crc_value(di);
	di->config_profile_info[79] = byte80_value; /* 79: the eighth element of the array needs to be updated */
	hwlog_info("calibration success\n");
}

/* init function, often called during initialization */
static int cw2217_config(struct cw2217_dev *di)
{
	int ret;
	int i;

	for (i = 0; i < CW2217_SIZE_OF_PROFILE; i++)
		hwlog_info("config_profile_info[%d] = 0x%x\n", i, di->config_profile_info[i]);
	cw2217_calibrate_profile(di);

	ret = cw2217_get_state(di);
	if (ret < 0) {
		hwlog_err("iic read write error");
		return ret;
	}

	if (ret != 0) {
		ret = cw2217_config_start_ic(di);
		if (ret < 0)
			return ret;
	}

	hwlog_info("init success\n");
	return 0;
}

static void cw2217_capacity_error_detect(struct cw2217_dev *di)
{
	int i;
	int ret;
	u8 reg_val = 0;
	int reg_profile;

	if (di->ui_soc > CW2217_FLAG_UPDATE_SOC && !di->print_error_flag) {
		di->print_error_flag = true;
	} else if (di->ui_soc < CW2217_ERROR_UI_SOC && di->voltage > CW2217_ERROR_VOLTAGE &&
		di->print_error_flag) {
		for (i = 0; i < CW2217_SIZE_OF_PROFILE; i++) {
			ret = cw2217_read_byte(di->client, (CW2217_REG_BAT_PROFILE + i), &reg_val);
			if (ret < 0)
				return;
			reg_profile = CW2217_REG_BAT_PROFILE + i;
			hwlog_info("reg[0x%2x] = 0x%2x", reg_profile, reg_val);
		}
		di->print_error_flag = false;
	}
}

static void cw2217_bat_work(struct work_struct *work)
{
	struct delayed_work *delay_work = NULL;
	struct cw2217_dev *di = NULL;

	delay_work = container_of(work, struct delayed_work, work);
	if (!delay_work)
		return;
	di = container_of(delay_work, struct cw2217_dev, battery_delay_work);
	if (!di)
		return;

	cw2217_update_data(di);
	cw2217_capacity_error_detect(di);
	queue_delayed_work(di->cwfg_workqueue, &di->battery_delay_work,
		msecs_to_jiffies(CW2217_QUEUE_DELAYED_WORK_TIME));
}

static int cw2217_get_log_head(char *buffer, int size, void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di || !buffer)
		return -EPERM;

	if (di->ic_role == CW2217_IC_TYPE_MAIN)
		snprintf(buffer, size,
			" soc    soc_h  soc_l  voltage  current  temp   cycle  soh  ");
	else
		snprintf(buffer, size,
			" soc1    soc_h1  soc_l1  voltage1  current1  temp1   cycle1  soh1  ");

	return 0;
}

static int cw2217_dump_log_data(char *buffer, int size, void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di || !buffer)
		return -EPERM;

	if (di->ic_role == CW2217_IC_TYPE_MAIN)
		snprintf(buffer, size, "%-7d%-7d%-7d%-9d%-9d%-7d%-7d%-4d   ",
			di->ui_soc, di->ic_soc_h, di->ic_soc_l,
			di->voltage, di->curr, di->temp, di->cycle, di->soh);
	else
		snprintf(buffer, size, "%-8d%-8d%-8d%-10d%-10d%-8d%-8d%-7d   ",
			di->ui_soc, di->ic_soc_h, di->ic_soc_l,
			di->voltage, di->curr, di->temp, di->cycle, di->soh);

	return 0;
}

static struct power_log_ops cw2217_log_ops = {
	.dev_name = "cw2217",
	.dump_log_head = cw2217_get_log_head,
	.dump_log_content = cw2217_dump_log_data,
};

static struct power_log_ops cw2217_aux_log_ops = {
	.dev_name = "cw2217_aux",
	.dump_log_head = cw2217_get_log_head,
	.dump_log_content = cw2217_dump_log_data,
};

static int cw2217_is_ready(void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	return 1;
}

static int cw2217_is_battery_exist(void *dev_data)
{
	int ret;
	int temp = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_temp(&temp, di);
	if (ret < 0)
		return ret;

	if ((temp <= CW2217_TEMP_ABR_LOW) || (temp >= CW2217_TEMP_ABR_HIGH))
		return 0;

	return 1; /* battery is existing */
}

static int cw2217_read_battery_soc(void *dev_data)
{
	int ret;
	int ui_soc = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_capacity(&ui_soc, di);
	if (ret < 0)
		return ret;

	return ui_soc;
}

static int cw2217_read_battery_vol(void *dev_data)
{
	int ret;
	int voltage = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_voltage(&voltage, di);
	if (ret < 0)
		return ret;

	return voltage;
}

static int cw2217_read_battery_current(void *dev_data)
{
	int ret;
	int curr = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_current(&curr, di);
	if (ret < 0)
		return ret;

	return curr;
}

static int cw2217_read_battery_avg_current(void *dev_data)
{
	int ret;
	int avg_curr = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_current(&avg_curr, di);
	if (ret < 0)
		return ret;

	return avg_curr;
}

static int cw2217_read_battery_temperature(void *dev_data)
{
	int ret;
	int temp = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_temp(&temp, di);
	if (ret < 0)
		return ret;

	return temp;
}

static int cw2217_read_battery_fcc(void *dev_data)
{
	int ret;
	int soh = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_soh(&soh, di);
	if (ret < 0)
		return ret;

	return di->rated_capacity * soh / CW2217_SOH_DEFAULT_VAL;
}

static int cw2217_read_battery_cycle(void *dev_data)
{
	int ret;
	int cycle = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_cycle_count(&cycle, di);
	if (ret < 0)
		return ret;

	return cycle;
}

static int cw2217_set_battery_low_voltage(int val, void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di)
		return -EPERM;

	return 0;
}

static int cw2217_set_last_capacity(int capacity, void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di || (capacity > CW2217_FULL_CAPACITY) || (capacity < 0))
		return 0;

	return cw2217_write_reg_capacity(di, capacity);
}

static int cw2217_get_last_capacity(void *dev_data)
{
	int ret;
	int last_cap = 0;
	int cap = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return last_cap;

	last_cap = cw2217_read_reg_capacity(di);
	ret = cw2217_get_capacity(&cap, di);
	if (ret < 0)
		return ret;

	if ((last_cap <= 0) || (cap <= 0))
		return cap;

	if (abs(last_cap - cap) >= CW2217_CAPACITY_TH)
		return cap;

	/* reset last capacity */
	ret = cw2217_write_reg_capacity(di, 0);
	if (ret < 0)
		return ret;

	return last_cap;
}

static int cw2217_read_battery_rm(void *dev_data)
{
	int ret;
	int fcc;
	int capacity = 0;
	struct cw2217_dev *di = dev_data;

	if (!di)
		return 0;

	ret = cw2217_get_capacity(&capacity, di);
	if (ret < 0)
		return ret;

	ret = cw2217_read_battery_fcc(di);
	if (ret < 0)
		return ret;
	fcc = ret;

	return capacity * fcc / CW2217_FULL_CAPACITY;
}

static int cw2217_set_vterm_dec(int vterm, void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di)
		return -EPERM;

	return -EPERM;
}

static const char *cw2217_get_coul_model(void *dev_data)
{
	return "cw22xx";
}

static struct coul_interface_ops cw2217_ops = {
	.type_name = "main",
	.is_coul_ready = cw2217_is_ready,
	.is_battery_exist = cw2217_is_battery_exist,
	.get_battery_capacity = cw2217_read_battery_soc,
	.get_battery_voltage = cw2217_read_battery_vol,
	.get_battery_current = cw2217_read_battery_current,
	.get_battery_avg_current = cw2217_read_battery_avg_current,
	.get_battery_temperature = cw2217_read_battery_temperature,
	.get_battery_fcc = cw2217_read_battery_fcc,
	.get_battery_cycle = cw2217_read_battery_cycle,
	.set_battery_low_voltage = cw2217_set_battery_low_voltage,
	.set_battery_last_capacity = cw2217_set_last_capacity,
	.get_battery_last_capacity = cw2217_get_last_capacity,
	.get_battery_rm = cw2217_read_battery_rm,
	.set_vterm_dec = cw2217_set_vterm_dec,
	.get_coul_model = cw2217_get_coul_model,
};

static struct coul_interface_ops cw2217_aux_ops = {
	.type_name = "aux",
	.is_coul_ready = cw2217_is_ready,
	.is_battery_exist = cw2217_is_battery_exist,
	.get_battery_capacity = cw2217_read_battery_soc,
	.get_battery_voltage = cw2217_read_battery_vol,
	.get_battery_current = cw2217_read_battery_current,
	.get_battery_avg_current = cw2217_read_battery_avg_current,
	.get_battery_temperature = cw2217_read_battery_temperature,
	.get_battery_fcc = cw2217_read_battery_fcc,
	.get_battery_cycle = cw2217_read_battery_cycle,
	.set_battery_low_voltage = cw2217_set_battery_low_voltage,
	.set_battery_last_capacity = cw2217_set_last_capacity,
	.get_battery_last_capacity = cw2217_get_last_capacity,
	.get_battery_rm = cw2217_read_battery_rm,
	.set_vterm_dec = cw2217_set_vterm_dec,
	.get_coul_model = cw2217_get_coul_model,
};

static int cw2217_get_calibration_curr(int *val, void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di || !val) {
		hwlog_err("di or val is null\n");
		return -EPERM;
	}

	return cw2217_get_current(val, di);
}

static int cw2217_get_calibration_vol(int *val, void *dev_data)
{
	int ret;
	struct cw2217_dev *di = dev_data;

	if (!di || !val) {
		hwlog_err("di or val is null\n");
		return -EPERM;
	}

	ret = cw2217_get_voltage(val, di);
	if (ret < 0)
		return ret;

	*val *= POWER_UV_PER_MV;
	return 0;
}

static int cw2217_set_current_gain(unsigned int val, void *dev_data)
{
	struct cw2217_dev *di = dev_data;

	if (!di || !val) {
		hwlog_err("di or val is null\n");
		return -EPERM;
	}

	di->coefficient = val;
	return 0;
}

static int cw2217_set_voltage_gain(unsigned int val, void *dev_data)
{
	return 0;
}

static int cw2217_enable_cali_mode(int enable, void *dev_data)
{
	return 0;
}

static struct coul_cali_ops cw2217_cali_ops = {
	.dev_name = "aux",
	.get_current = cw2217_get_calibration_curr,
	.get_voltage = cw2217_get_calibration_vol,
	.set_current_gain = cw2217_set_current_gain,
	.set_voltage_gain = cw2217_set_voltage_gain,
	.set_cali_mode = cw2217_enable_cali_mode,
};

/* main battery gauge use aux calibration data for compatible */
static struct coul_cali_ops cw2217_aux_cali_ops = {
	.dev_name = "main",
	.get_current = cw2217_get_calibration_curr,
	.get_voltage = cw2217_get_calibration_vol,
	.set_current_gain = cw2217_set_current_gain,
	.set_voltage_gain = cw2217_set_voltage_gain,
	.set_cali_mode = cw2217_enable_cali_mode,
};

static int cw2217_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	int loop = 0;
	struct cw2217_dev *di = NULL;
	struct power_devices_info_data *power_dev_info = NULL;

	di = devm_kzalloc(&client->dev, sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	ret = cw2217_parse_dt(&client->dev, di);
	if (ret) {
		hwlog_err("parse dts fail\n");
		return ret;
	}

	i2c_set_clientdata(client, di);
	di->dev = &client->dev;
	di->client = client;

	ret = cw2217_check_chip(di);
	if (ret)
		return ret;

	ret = cw2217_config(di);
	while ((loop++ < CW2217_RETRY_COUNT) && (ret != 0)) {
		msleep(DT_MSLEEP_200MS);
		ret = cw2217_config(di);
	}
	if (ret) {
		hwlog_err("init config fail\n");
		return ret;
	}

	ret = cw2217_init_data(di);
	if (ret) {
		hwlog_err("init data fail\n");
		return ret;
	}

#ifdef CONFIG_HUAWEI_HW_DEV_DCT
	if (di->ic_role == CW2217_IC_TYPE_MAIN)
		set_hw_dev_flag(DEV_I2C_GAUGE_IC);
	else
		set_hw_dev_flag(DEV_I2C_GAUGE_IC_AUX);
#endif /* CONFIG_HUAWEI_HW_DEV_DCT */

	di->cwfg_workqueue = create_singlethread_workqueue("cwfg_gauge");
	INIT_DELAYED_WORK(&di->battery_delay_work, cw2217_bat_work);
	queue_delayed_work(di->cwfg_workqueue, &di->battery_delay_work,
		msecs_to_jiffies(CW2217_QUEUE_START_WORK_TIME));

	if (di->ic_role == CW2217_IC_TYPE_MAIN) {
		cw2217_log_ops.dev_data = (void *)di;
		power_log_ops_register(&cw2217_log_ops);
		cw2217_ops.dev_data = (void *)di;
		coul_interface_ops_register(&cw2217_ops);
		cw2217_cali_ops.dev_data = (void *)di;
		coul_cali_ops_register(&cw2217_cali_ops);
	} else {
		cw2217_aux_log_ops.dev_data = (void *)di;
		power_log_ops_register(&cw2217_aux_log_ops);
		cw2217_aux_ops.dev_data = (void *)di;
		coul_interface_ops_register(&cw2217_aux_ops);
		cw2217_aux_cali_ops.dev_data = (void *)di;
		coul_cali_ops_register(&cw2217_aux_cali_ops);
	}

	power_dev_info = power_devices_info_register();
	if (power_dev_info) {
		power_dev_info->dev_name = di->dev->driver->name;
		power_dev_info->dev_id = di->chip_id;
		power_dev_info->ver_id = 0;
	}

	return 0;
}

static int cw2217_remove(struct i2c_client *client)
{
	return 0;
}

#ifdef CONFIG_PM
static int cw2217_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cw2217_dev *di = NULL;

	if (!client)
		return 0;

	di = i2c_get_clientdata(client);
	if (!di)
		return 0;

	cancel_delayed_work_sync(&di->battery_delay_work);
	return 0;
}

static int cw2217_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cw2217_dev *di = NULL;

	if (!client)
		return 0;

	di = i2c_get_clientdata(client);
	if (!di)
		return 0;

	queue_delayed_work(di->cwfg_workqueue, &di->battery_delay_work,
		msecs_to_jiffies(CW2217_QUEUE_RESUME_WORK_TIME));
	return 0;
}

static const struct dev_pm_ops cw2217_pm_ops = {
	.suspend = cw2217_suspend,
	.resume = cw2217_resume,
};
#define CW2217_PM_OPS (&cw2217_pm_ops)
#else
#define CW2217_PM_OPS (NULL)
#endif /* CONFIG_PM */

static const struct i2c_device_id cw2217_id_table[] = {
	{ "cw2217", 0 },
	{}
};

static const struct of_device_id cw2217_match_table[] = {
	{ .compatible = "cellwise,cw2217", },
	{},
};

static struct i2c_driver cw2217_driver = {
	.probe = cw2217_probe,
	.remove = cw2217_remove,
	.id_table = cw2217_id_table,
	.driver = {
		.owner = THIS_MODULE,
		.name = "cw2217",
		.of_match_table = cw2217_match_table,
		.pm = CW2217_PM_OPS,
	},
};

static int __init cw2217_init(void)
{
	return i2c_add_driver(&cw2217_driver);
}

static void __exit cw2217_exit(void)
{
	i2c_del_driver(&cw2217_driver);
}

module_init(cw2217_init);
module_exit(cw2217_exit);

MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
MODULE_DESCRIPTION("cw2217 Fuel Gauge Driver");
MODULE_LICENSE("GPL v2");
