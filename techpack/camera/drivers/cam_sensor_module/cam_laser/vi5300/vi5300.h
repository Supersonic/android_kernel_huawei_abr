/*
 *  vi5300_module.c - Linux kernel modules for VI5300 FlightSense TOF
 *						 sensor
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#ifndef VI5300_H
#define VI5300_H

#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <cam_laser.h>

#define VI5300_MISC_DEV_NAME	"laser"
#define VI5300_NAME "vi5300"

#define DEVICE_LOAD_CURRENT 18000
typedef int32_t VI5300_Error;
typedef struct vi5300_data* VI5300_DEV;

#define VI5300_CHIP_ADDR 0xD8

#define VI5300_REG_MCU_CFG 0x00
#define VI5300_REG_SYS_CFG 0x01
#define VI5300_REG_DEV_STAT 0x02
#define VI5300_REG_INTR_STAT 0x03
#define VI5300_REG_INTR_MASK 0x04
#define VI5300_REG_I2C_IDLE_TIME 0x05
#define VI5300_REG_DEV_ADDR 0x06
#define VI5300_REG_PW_CTRL 0x07
#define VI5300_REG_SPCIAL_PURP 0x08
#define VI5300_REG_CMD 0x0A
#define VI5300_REG_SIZE 0x0B
#define VI5300_REG_SCRATCH_PAD_BASE 0x0C
#define VI5300_REG_RCO_AO 0x37
#define VI5300_REG_DIGLDO_VREF 0x38
#define VI5300_REG_PLLLDO_VREF 0x39
#define VI5300_REG_ANALDO_VREF 0x3A
#define VI5300_REG_PD_RESET 0x3B
#define VI5300_REG_I2C_STOP_DELAY 0x3C
#define VI5300_REG_TRIM_MODE 0x3D
#define VI5300_REG_GPIO_SINGLE 0x3E
#define VI5300_REG_ANA_TEST_SINGLE 0x3F

#define VI5300_WRITEFW_CMD 0x03
#define VI5300_USER_CFG_CMD 0x09
#define VI5300_XTALK_TRIM_CMD 0x0D
#define VI5300_SINGLE_RANGE_CMD 0x0E
#define VI5300_CONTINOUS_RANGE_CMD 0x0F
#define VI5300_STOP_RANGE_CMD 0x01F

#define VI5300_XTALKR_SUBCMD 0x00
#define VI5300_XTALKW_SUBCMD 0x01
#define VI5300_OTPW_SUBCMD 0x02
#define VI5300_OTPR_SUBCMD 0x03
#define VI5300_XTALK_ADDR 0x00
#define VI5300_XTALK_SIZE 1
#define VI5300_MAX_WAIT_RETRY 5
#define DEFAULT_INTEGRAL_COUNTS 131072
#define DEFAULT_FRAME_COUNTS 30

#define VI5300_ERROR_NONE ((VI5300_Error) 0)
#define VI5300_ERROR_CPU_BUSY ((VI5300_Error) -1)
#define VI5300_ERROR_ENABLE_INTR ((VI5300_Error) -2)
#define VI5300_ERROR_XTALK_CALIB ((VI5300_Error) -3)
#define VI5300_ERROR_OFFSET_CALIB ((VI5300_Error) -4)
#define VI5300_ERROR_XTALK_CONFIG ((VI5300_Error) -5)
#define VI5300_ERROR_SINGLE_CMD ((VI5300_Error) -6)
#define VI5300_ERROR_CONTINUOUS_CMD ((VI5300_Error) -7)
#define VI5300_ERROR_GET_DATA ((VI5300_Error) -8)
#define VI5300_ERROR_STOP_CMD ((VI5300_Error) -9)
#define VI5300_ERROR_IRQ_STATE ((VI5300_Error) -10)
#define VI5300_ERROR_FW_FAILURE ((VI5300_Error) -11)
#define VI5300_ERROR_POWER_ON ((VI5300_Error) -14)
#define VI5300_ERROR_POWER_OFF ((VI5300_Error) -15)

struct VI5300_Measurement_Data {
	int32_t Obj_Range;
	uint32_t AmbientRate;
	uint32_t timeUSec;
	uint32_t RangeStatus;
	uint32_t NumberOfObject;
	uint32_t RangeLimit;
};

struct VI5300_XTALK_Calib_Data {
	int8_t xtalk_cal;
	uint16_t xtalk_peak;
};

struct VI5300_OFFSET_Calib_Data {
	int16_t offset_cal;
};

struct vi5300_data {
 	struct device *dev;
 	const char *dev_name;
	struct VI5300_Measurement_Data Rangedata;
	struct VI5300_XTALK_Calib_Data XtalkData;
	struct VI5300_OFFSET_Calib_Data OffsetData;
 	struct mutex update_lock;
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct miscdevice miscdev;
	struct timespec64 start_ts;
	int irq_gpio;
	int xshut_gpio;
	int irq;
	uint32_t chip_enable;
	uint32_t enable_debug;
	uint8_t intr_state;
	uint8_t fwdl_status;
	int8_t xtalk_config;
	int16_t offset_config;
	const char *program_version;
	uint32_t integral_counts;
	uint32_t period;
	struct mutex work_mutex;

	/* calibration data */
	hwlaser_info_t pinfo;
	hwlaser_RangingData_t udata;
};

union inte_data {
	uint32_t intecnts;
	uint8_t buf[4];
};

enum VI5300_INT_STATUS {
	VI5300_INTR_DISABLED = 0,
	VI5300_INTR_ENABLED,
};

#define vi5300_infomsg(str, args...) \
	 pr_info("%s: " str, __func__, ##args)
#define vi5300_dbgmsg(str, args...) \
	 pr_debug("%s: " str, __func__, ##args)
#define vi5300_errmsg(str, args...) \
	 pr_err("%s: " str, __func__, ##args)


int vi5300_probe(struct i2c_client *client, const struct i2c_device_id *id);
int vi5300_remove(struct i2c_client *client);

#endif
