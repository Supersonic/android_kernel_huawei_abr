/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: bu64754 implementation file
 */

#ifndef ACTUATOR_BU64754_H
#define ACTUATOR_BU64754_H
#include "cam_actuator_dev.h"

/* set code */
#define BU64754_POWER_ON_OFF_REG      0x07
#define BU64754_POWER_OFF             0x00
#define BU64754_POWER_ON              0x80
#define BU64754_CHECKSUM              0xF7
#define BU64754_NVM_REG               0x1AE6

/* nvm data rewrite */
#define BU64754_CHECKSUM_VALUE     0x04
#define BU64754_AF_NVM_SIZE        96
#define BU64754_TRY_TIMEOUT        0x2
#define BU64754_NVM_START_REG      0x52
#define BU64754_NVM_WRITE_ADDR_REG 0x50
#define BU64754_NVM_WRITE_DATA_REG 0x51
#define BU64754_NVM_START_ENABLE   0x8000
#define BU64754_NVM_START_DISABLE  0x00
#define BU64754_SHIFT_8BIT         8

/* vcm standby mode */
#define BU64754_SET_POS_REG_DELAY      2

#define BU64754_CHECKSUM_VALUE         0x04

/**
 * @a_ctrl: Actuator ctrl structure
 * @reg:    The value of register
 * @value:  The value of data
 * @ms:     The time of delay
 *
 * This API writes 16 bits data into register
 */
int32_t vcm_write_reg_16bit_data(struct cam_actuator_ctrl_t *a_ctrl, unsigned char reg,
	unsigned short value, unsigned int ms);

/**
 * @a_ctrl:      Actuator ctrl structure
 * @data_buffer: The buffer for storing data
 *
 * This API transfers the data in the buffer into registers
 */

int bu64754_rewrite_data_into_nvm(struct cam_actuator_ctrl_t *a_ctrl,
	const unsigned char* data_buffer);

/**
 * @a_ctrl: Actuator ctrl structure
 *
 * This API judges the checksum
 */
int bu64754_judge_checksum(struct cam_actuator_ctrl_t *a_ctrl);

#endif // ACTUATOR_BU64754_H
