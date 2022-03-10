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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/time.h>

#include "vi5300.h"
#include "vi5300_platform.h"
#include "vi5300_firmware.h"
#include "vi5300_api.h"

#define STATUS_TOF_CONFIDENT 0
#define STATUS_TOF_SEMI_CONFIDENT 6
#define STATUS_TOF_NOT_CONFIDENT 7

#define PILEUP_A (9231000)
#define PILEUP_B (4896)
#define PILEUP_C (1922)
#define PILEUP_D (10)

VI5300_Error VI5300_Chip_PowerON(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	Status = gpio_direction_output(dev->xshut_gpio, 0);
	mdelay(5);
	Status = gpio_direction_output(dev->xshut_gpio, 1);
	mdelay(5);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Chip Power ON Failed Status = %d\n", Status);
		return VI5300_ERROR_POWER_ON;
	}

	return Status;
}

VI5300_Error VI5300_Chip_PowerOFF(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	Status = gpio_direction_output(dev->xshut_gpio, 0);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Chip Power OFF Failed Status = %d\n", Status);
		return VI5300_ERROR_POWER_OFF;
	}

	return Status;
}

void VI5300_Chip_Register_Init(VI5300_DEV dev)
{
	vi5300_write_byte(dev, VI5300_REG_MCU_CFG, 0x00);
	vi5300_write_byte(dev, VI5300_REG_SYS_CFG, 0x0C);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x00);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x01);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x00);
	vi5300_write_byte(dev, VI5300_REG_INTR_MASK, 0x21);
	vi5300_write_byte(dev, VI5300_REG_I2C_IDLE_TIME, 0x0E);
	vi5300_write_byte(dev, VI5300_REG_SPCIAL_PURP, 0x00);
	vi5300_write_byte(dev, VI5300_REG_RCO_AO, 0x80);
	vi5300_write_byte(dev, VI5300_REG_DIGLDO_VREF, 0x30);
	vi5300_write_byte(dev, VI5300_REG_PLLLDO_VREF, 0x00);
	vi5300_write_byte(dev, VI5300_REG_ANALDO_VREF, 0x30);
	vi5300_write_byte(dev, VI5300_REG_PD_RESET, 0x80);
	vi5300_write_byte(dev, VI5300_REG_I2C_STOP_DELAY, 0x80);
	vi5300_write_byte(dev, VI5300_REG_TRIM_MODE, 0x80);
	vi5300_write_byte(dev, VI5300_REG_GPIO_SINGLE, 0x00);
	vi5300_write_byte(dev, VI5300_REG_ANA_TEST_SINGLE, 0x00);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x0E);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x0F);
}


void VI5300_Waiting_For_RCO_Stable(VI5300_DEV dev)
{
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x0F);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x0E);
	mdelay(4);
}

VI5300_Error VI5300_Wait_For_CPU_Ready(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;
	uint8_t stat;
	int retry = 0;

	do {
		mdelay(1);
		vi5300_read_byte(dev, VI5300_REG_DEV_STAT, &stat);
	}while((retry++ < VI5300_MAX_WAIT_RETRY)
		&&(stat & 0x01));
	if(retry >= VI5300_MAX_WAIT_RETRY)
	{
		vi5300_errmsg("CPU Busy stat = %d\n", stat);
		return VI5300_ERROR_CPU_BUSY;
	}

	return Status;
}

VI5300_Error VI5300_Init_FirmWare(VI5300_DEV dev)
{
	uint8_t  sys_cfg_data = 0;
	uint16_t fw_size = 0;
	uint16_t fw_send = 0;
	uint8_t val;
	VI5300_Error Status = VI5300_ERROR_NONE;

	fw_size = LoadFirmware(dev);
	if(!fw_size)
	{
		vi5300_errmsg("Firmware Load Failed!\n");
		return VI5300_ERROR_FW_FAILURE;
	}

	VI5300_Waiting_For_RCO_Stable(dev);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x08);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x0a);
	vi5300_write_byte(dev, VI5300_REG_MCU_CFG, 0x06);
	vi5300_read_byte(dev, VI5300_REG_SYS_CFG, &sys_cfg_data);
	vi5300_write_byte(dev, VI5300_REG_SYS_CFG, sys_cfg_data | (0x01 << 0));
	vi5300_write_byte(dev, VI5300_REG_CMD, 0x01);
	vi5300_write_byte(dev, VI5300_REG_SIZE, 0x02);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, 0x0);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0x01, 0x0);
	while(fw_size >= 23)
	{
		vi5300_write_reg_offset(dev, VI5300_REG_CMD, 0, VI5300_WRITEFW_CMD);
		vi5300_write_reg_offset(dev, VI5300_REG_SIZE, 0, 0x17);
		vi5300_write_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE, Firmware+fw_send*23, 23);
		udelay(10);
		fw_send += 1;
		fw_size -= 23;
	}
	if(fw_size > 0)
	{
		vi5300_write_reg_offset(dev, VI5300_REG_CMD, 0, VI5300_WRITEFW_CMD);
		vi5300_write_reg_offset(dev, VI5300_REG_SIZE, 0, (uint8_t)fw_size);
		vi5300_write_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE, Firmware+fw_send*23, fw_size);
	}
	vi5300_write_byte(dev, VI5300_REG_SYS_CFG, sys_cfg_data & ~(0x01 << 0));
	vi5300_write_byte(dev, VI5300_REG_MCU_CFG, 0x06);
	vi5300_write_byte(dev, VI5300_REG_PD_RESET, 0xA0);
	vi5300_write_byte(dev, VI5300_REG_PD_RESET, 0x80);
	vi5300_write_byte(dev, VI5300_REG_MCU_CFG, 0x07);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x02);
	vi5300_write_byte(dev, VI5300_REG_PW_CTRL, 0x00);
	mdelay(5);
	vi5300_read_byte(dev, VI5300_REG_SPCIAL_PURP, &val);
	if(val != 0x66)
	{
		vi5300_errmsg("Download Firmware Failed Status = %d\n", Status);
		Status = VI5300_ERROR_FW_FAILURE;
	}

	return Status;
}

void VI5300_Integral_Counts_Write(VI5300_DEV dev, uint32_t inte_counts)
{
	union inte_data {
		uint32_t intecnts;
		uint8_t buf[4];
	} intedata;

	intedata.intecnts = inte_counts;
	VI5300_Waiting_For_RCO_Stable(dev);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, 0x01);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 1, 0x03);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 2, 0x01);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 3, intedata.buf[0]);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 4, intedata.buf[1]);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 5, intedata.buf[2]);
	vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_USER_CFG_CMD);
}

void VI5300_Delay_Count_Write(VI5300_DEV dev, uint16_t delay_count)
{
	union delay_data {
		uint16_t delay;
		uint8_t buf[2];
	} delaydata;

	delaydata.delay = delay_count;
	VI5300_Waiting_For_RCO_Stable(dev);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, 0x01);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 1, 0x02);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 2, 0x04);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 3, delaydata.buf[1]);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 4, delaydata.buf[0]);
	vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_USER_CFG_CMD);
}

void VI5300_Set_Period(VI5300_DEV dev, uint32_t period)
{
	uint32_t inte_time;
	uint32_t fps_time;
	uint32_t delay_time;
	uint16_t delay_counts;
	union inte_data pdata = {0};
	union delay_data {
		uint16_t delay;
		uint8_t buf[2];
	} delaydata;

	VI5300_Waiting_For_RCO_Stable(dev);
	if(period == 0)
	{
		vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, 0x00);
		vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 1, 0x02);
		vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 2, 0x04);
		vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_USER_CFG_CMD);
		msleep(5);
		vi5300_read_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, &delaydata.buf[1]);
		vi5300_read_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 1, &delaydata.buf[0]);
	} else {
		vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, 0x00);
		vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 1, 0x03);
		vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 2, 0x01);
		vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_USER_CFG_CMD);
		msleep(5);
		vi5300_read_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE, pdata.buf, 3);
		inte_time = pdata.intecnts*1463/10;
		fps_time = 1000000000 / period;
		delay_time = fps_time - inte_time -1600000;
		delay_counts = (uint16_t)(delay_time/40900);
		delaydata.delay = delay_counts;
	}
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, 0x01);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 1, 0x02);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 2, 0x04);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 3, delaydata.buf[1]);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 4, delaydata.buf[0]);
	vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_USER_CFG_CMD);
}

VI5300_Error VI5300_Single_Measure(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	Status = VI5300_Wait_For_CPU_Ready(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("CPU Abnormal Single Measure!Status = %d\n", Status);
		Status = VI5300_ERROR_SINGLE_CMD;
	}
	VI5300_Waiting_For_RCO_Stable(dev);
	Status = vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_SINGLE_RANGE_CMD);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Single measure Failed Status = %d\n", Status);
		return VI5300_ERROR_SINGLE_CMD;
	}

	return Status;
}
VI5300_Error VI5300_Start_Continuous_Measure(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	Status = VI5300_Wait_For_CPU_Ready(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("CPU Abnormal Continuous Measure!Status = %d\n", Status);
		Status = VI5300_ERROR_CONTINUOUS_CMD;
	}
	VI5300_Waiting_For_RCO_Stable(dev);
	Status = vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_CONTINOUS_RANGE_CMD);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Continuous Measure Failed Status = %d\n", Status);
		Status = VI5300_ERROR_CONTINUOUS_CMD;
	}

	return Status;
}

VI5300_Error VI5300_Stop_Continuous_Measure(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	Status = vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_STOP_RANGE_CMD);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Stop Measure Failed Status = %d\n", Status);
		return VI5300_ERROR_STOP_CMD;
	}

	return Status;
}

VI5300_Error VI5300_Get_Measure_Data(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;
	uint8_t buf[32];
	int16_t millimeter;
	int16_t nearlimit;
	int16_t farlimit;
	uint16_t noise_level;
	uint32_t peak1;
	uint32_t peak2;
	uint32_t integral_times;
	uint32_t confidence;
	uint16_t noise_tmp;
	uint32_t lower;
	uint32_t upper;
	int32_t bias;
	uint32_t peak_tmp;

	struct timespec64 end_ts = {0},ts_sub = {0};

	Status |= vi5300_read_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE, buf, 16);
	Status |= vi5300_read_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE + 0x10, buf + 16, 16);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Get Range Data Failed Status = %d\n", Status);
		return VI5300_ERROR_GET_DATA;
	}
	millimeter = *((int16_t *)(buf + 12));
	nearlimit = *((int16_t *)(buf + 18));
	farlimit = *((int16_t *)(buf + 15));
	noise_level = *((uint16_t *)(buf + 26));
	integral_times = *((uint32_t *)(buf + 22));
	peak1 = *((uint32_t *)(buf + 28));
	peak2 = *((uint32_t *)(buf + 8));
	integral_times = integral_times &0x00ffffff;
	if(peak2 > 65536)
		peak_tmp = peak2 * 256 / integral_times * 256;
	else
		peak_tmp = peak2 *65536 / integral_times;
	peak_tmp = peak_tmp >> 12;
	bias = (int32_t)(PILEUP_A / (PILEUP_B - peak_tmp * PILEUP_D) - PILEUP_C) / PILEUP_D;
	if(bias < 0)
		bias = 0;
	millimeter = millimeter + (int16_t)bias;
	millimeter = millimeter - dev->offset_config;
	nearlimit = nearlimit + (int16_t)bias - dev->offset_config;
	farlimit = farlimit + (int16_t)bias - dev->offset_config;

	noise_tmp = noise_level  / 8;
	if(noise_tmp < 45) {
		lower = 228 * noise_level / 8 + 1260;
		upper = 269 * noise_level / 8 + 2284;
	} else if(noise_tmp < 94) {
		lower = 252 * noise_level / 8 + 172;
		upper = 300 * noise_level / 8 + 901;
	} else if(noise_tmp <154) {
		lower = 190 * noise_level / 8 + 6066;
		upper = 227 * noise_level / 8 + 7703;
	} else if(noise_tmp <246) {
		lower = 209 * noise_level / 8 + 3043;
		upper = 237 * noise_level / 8 + 6121;
	} else if(noise_tmp <721){
		lower = 197 * noise_level / 8 + 6006;
		upper = 216 * noise_level / 8 + 11300;
	} else if(noise_tmp <1320){
		lower = 201 * noise_level / 8 + 3177;
		upper = 226 * noise_level / 8 + 4547;
	} else if(noise_tmp <2797){
		lower = 191 * noise_level / 8 + 16874;
		upper = 217 * noise_level / 8 + 16340;
	} else if(noise_tmp <3581){
		lower = 188 * noise_level / 8 + 23925;
		upper = 218 * noise_level / 8 + 14012;
	} else {
		lower = 180 * noise_level / 8 + 53524;
		upper = 204 * noise_level / 8 + 62305;
	}
	if(peak1 < lower) {
		confidence = 0;
	} else if(peak1 > upper) {
		confidence = 100;
	} else {
		confidence = 100 * (peak1 - lower) / (upper - lower);
	}

	ktime_get_real_ts64(&end_ts);
	ts_sub = timespec64_sub(end_ts, dev->start_ts);
	dev->Rangedata.timeUSec = ts_sub.tv_sec * 1000000 + ts_sub.tv_nsec / 1000;
	dev->Rangedata.Obj_Range = millimeter << 16;
	dev->Rangedata.NumberOfObject = 1 << 6;
	dev->Rangedata.RangeLimit = nearlimit << 16 | (farlimit & 0xFFFF);
	dev->Rangedata.AmbientRate = ((uint32_t)noise_level) * 300 / (peak2 * 8);
	if(confidence >= 80)
		dev->Rangedata.RangeStatus = STATUS_TOF_CONFIDENT;
	else if(confidence >= 20)
		dev->Rangedata.RangeStatus = STATUS_TOF_SEMI_CONFIDENT;
	else
		dev->Rangedata.RangeStatus = STATUS_TOF_NOT_CONFIDENT;
	if(millimeter < 50 && peak1 < 800000)
		dev->Rangedata.RangeStatus = STATUS_TOF_NOT_CONFIDENT;
	if(dev->enable_debug)
	{
		vi5300_errmsg("timeUSec:%d, Obj_Range:%d, RangeStatus:%d, NumberOfObject:%d, AmbientRate:%d, nearlimit:%d, farlimit:%d\n",
					dev->Rangedata.timeUSec,
					dev->Rangedata.Obj_Range >> 16,
					dev->Rangedata.RangeStatus,
					dev->Rangedata.NumberOfObject >> 6,
					dev->Rangedata.AmbientRate,
					nearlimit, farlimit);
	}
	return Status;
}

VI5300_Error VI5300_Get_Interrupt_State(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;
	uint8_t stat;

	Status = vi5300_read_byte(dev, VI5300_REG_INTR_STAT, &stat);
	if(!(stat & 0x01))
	{
		vi5300_errmsg("Get Interrupt State Failed Status = %d\n", Status);
		return VI5300_ERROR_IRQ_STATE;
	}

	return Status;
}

VI5300_Error VI5300_Interrupt_Enable(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;
	int loop = 0;
	uint8_t enable = 0;

	do
	{
		vi5300_read_byte(dev, VI5300_REG_INTR_MASK, &enable);
		enable |=  0x01;
		vi5300_write_byte(dev, VI5300_REG_INTR_MASK, enable);
		vi5300_read_byte(dev, VI5300_REG_INTR_MASK, &enable);
		loop++;
	} while((loop < VI5300_MAX_WAIT_RETRY)
		&& (!(enable & 0x01)));
	if(loop >= VI5300_MAX_WAIT_RETRY)
	{
		vi5300_errmsg("Enable interrupt Failed Status = %d\n", Status);
		return VI5300_ERROR_ENABLE_INTR;
	}
	return Status;
}

VI5300_Error VI5300_Chip_Init(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	VI5300_Chip_Register_Init(dev);
	Status = VI5300_Wait_For_CPU_Ready(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Internal CPU busy!\n");
		return Status;
	}
	Status = VI5300_Interrupt_Enable(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Clear Interrupt Mask failed!\n");
		return Status;
	}
	Status = VI5300_Init_FirmWare(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Download Firmware Failed!\n");
		return Status;
	}

	return Status;
}

VI5300_Error VI5300_Start_XTalk_Calibration(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	Status = VI5300_Wait_For_CPU_Ready(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("CPU Abnormal XTALK Calibrating Status = %d\n", Status);
		return VI5300_ERROR_XTALK_CALIB;
	}
	VI5300_Waiting_For_RCO_Stable(dev);
	vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_XTALK_TRIM_CMD);

	return Status;
}

VI5300_Error VI5300_Start_Offset_Calibration(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;
	uint8_t buf[32];
	int16_t mm;
	uint32_t peak;
	uint32_t inte_t;
	int32_t bias;
	uint32_t peak_t;
	int16_t total = 0;
	int16_t offset = 0;
	int cnt = 0;
	uint8_t stat;

	Status = VI5300_Start_Continuous_Measure(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Offset Calibtration Start Failed!\n");
		return VI5300_ERROR_OFFSET_CALIB;
	}
	while(1)
	{
		mdelay(35);
		Status = vi5300_read_byte(dev, VI5300_REG_INTR_STAT, &stat);
		if(Status == VI5300_ERROR_NONE)
		{
			if((stat & 0x01) == 0x01)
			{
				Status |= vi5300_read_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE, buf, 16);
				Status |= vi5300_read_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE + 0x10, buf + 16, 16);
				if(Status != VI5300_ERROR_NONE)
				{
					vi5300_errmsg("Get Range Data Failed Status = %d\n", Status);
					break;
				}
				mm = *((int16_t *)(buf + 12));
				inte_t = *((uint32_t *)(buf + 22));
				peak = *((uint32_t *)(buf + 8));
				inte_t = inte_t &0x00ffffff;
				if(peak > 65536)
					peak_t = peak * 256 / inte_t * 256;
				else
					peak_t = peak *65536 / inte_t;
				peak_t = peak_t >> 12;
				bias = (int32_t)(PILEUP_A / (PILEUP_B - peak_t * PILEUP_D) - PILEUP_C) / PILEUP_D;
				if(bias < 0)
					bias = 0;
				mm = mm + (int16_t)bias;
				total += mm;
				++cnt;
			}else
				continue;
		}else {
			vi5300_errmsg("Can't Get irq State!Status = %d\n", Status);
			break;
		}
		if(cnt >= 30)
			break;
	}
	Status = VI5300_Stop_Continuous_Measure(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("Offset Calibtration Stop Failed!\n");
		return VI5300_ERROR_OFFSET_CALIB;
	}
	offset = total / 30;
	dev->OffsetData.offset_cal = offset - 50;

	return Status;
}


VI5300_Error VI5300_Get_XTalk_Parameter(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;
	uint8_t val;
	uint8_t cg_buf[5];

	Status = vi5300_read_byte(dev, VI5300_REG_SPCIAL_PURP, &val);
	if(Status == VI5300_ERROR_NONE && val == 0xaa)
	{
		Status = vi5300_read_multibytes(dev, VI5300_REG_SCRATCH_PAD_BASE, cg_buf, 5);
		if(Status != VI5300_ERROR_NONE)
		{
			vi5300_errmsg("Get XTALK parameter Failed Status = %d\n", Status);
			return VI5300_ERROR_XTALK_CALIB;
		}
		dev->XtalkData.xtalk_cal = *((int8_t *)(cg_buf + 0));
		dev->XtalkData.xtalk_peak = *((uint16_t *)(cg_buf + 1));
	} else {
		vi5300_errmsg("XTALK Calibration Failed Status = %d, val = 0x%02x\n", Status, val);
		return VI5300_ERROR_XTALK_CALIB;
	}

	return Status;
}

VI5300_Error VI5300_Config_XTalk_Parameter(VI5300_DEV dev)
{
	VI5300_Error Status = VI5300_ERROR_NONE;

	Status = VI5300_Wait_For_CPU_Ready(dev);
	if(Status != VI5300_ERROR_NONE)
	{
		vi5300_errmsg("CPU Abnormal Configing XTALK Failed Status = %d\n", Status);
		return VI5300_ERROR_XTALK_CONFIG;
	}
	VI5300_Waiting_For_RCO_Stable(dev);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0, VI5300_XTALKW_SUBCMD);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0x01, VI5300_XTALK_SIZE);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0x02, VI5300_XTALK_ADDR);
	vi5300_write_reg_offset(dev, VI5300_REG_SCRATCH_PAD_BASE, 0x03, *((uint8_t *)(&dev->xtalk_config)));
	vi5300_write_byte(dev, VI5300_REG_CMD, VI5300_USER_CFG_CMD);

	return Status;
}

