/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor OIS core driver
 */

#ifndef _VENDOR_OIS_CORE_H_
#define _VENDOR_OIS_CORE_H_

#include <linux/cma.h>
#include "cam_ois_dev.h"

int ois_write_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint16_t value);

int ois_read_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint16_t *value);

int ois_i2c_block_write_reg(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint16_t *wr_buf, int32_t length, int32_t flag);

int ois_i2c_block_read_addr8_value8(struct cam_ois_ctrl_t *o_ctrl, uint8_t addr,
	uint8_t *data, uint32_t num_byte);

int ois_i2c_block_write_addr8_value8(struct cam_ois_ctrl_t *o_ctrl, uint8_t addr,
	uint8_t *data, uint32_t num_byte, int32_t flag);

int ois_i2c_block_write_reg_value8(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint8_t *wr_buf, int16_t length, int32_t flag);

int ois_read_addr16_value32(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint32_t *value);

int ois_write_addr16_value32(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint32_t value);

int ois_i2c_block_read_reg(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint32_t *wr_buf, uint16_t length,
	enum camera_sensor_i2c_type addr_type,
	enum camera_sensor_i2c_type data_type);

#endif
/* _VENDOR_OIS_CORE_H_ */
