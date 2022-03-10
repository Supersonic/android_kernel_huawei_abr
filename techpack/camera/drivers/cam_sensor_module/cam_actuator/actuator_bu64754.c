/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: bu64754 implementation file
 */

#include "actuator_bu64754.h"
#include <cam_sensor_cmn_header.h>
#include "debug_adapter.h"

int32_t vcm_write_reg_16bit_data(struct cam_actuator_ctrl_t *a_ctrl, unsigned char reg,
	unsigned short value, unsigned int ms)
{
	int rc = 0;
	struct cam_sensor_i2c_reg_setting i2c_reg_settings = {0};
	struct cam_sensor_i2c_reg_array   i2c_reg_array    = {0};

	if (!a_ctrl) {
		CAM_ERR(CAM_ACTUATOR, "Invalid Args");
		return -EINVAL;
	}

	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.size      = 1;

	i2c_reg_array.reg_addr = reg;
	i2c_reg_array.reg_data = value;
	i2c_reg_settings.delay = ms;
	i2c_reg_settings.reg_setting = &i2c_reg_array;

	rc = camera_io_dev_write(&(a_ctrl->io_master_info), &i2c_reg_settings);
	if (rc)
		CAM_ERR(CAM_ACTUATOR, "bu64754 i2c write failed rc %d", rc);

	return rc;
}

int bu64754_rewrite_data_into_nvm(struct cam_actuator_ctrl_t *a_ctrl,
	const unsigned char* data_buffer)
{
	int timeout = BU64754_TRY_TIMEOUT;
	int rc = 0;
	unsigned int checksum;
	int once = 1;
	unsigned short nvm_data;
	unsigned int i;
	unsigned int j;

	if (!a_ctrl || !data_buffer) {
		CAM_ERR(CAM_ACTUATOR, "Invalid Args");
		return -EINVAL;
	}

	do {
		checksum = 0;
		rc = vcm_write_reg_16bit_data(a_ctrl, BU64754_NVM_START_REG, BU64754_NVM_START_ENABLE, 0);
		if (rc < 0)
			CAM_ERR(CAM_ACTUATOR, "bu74654 write failed reg:%x,rc:%d", BU64754_NVM_START_REG, rc);

		for (i = 0, j = 0; i < BU64754_AF_NVM_SIZE; i += 2, j++) {
			rc = vcm_write_reg_16bit_data(a_ctrl, BU64754_NVM_WRITE_ADDR_REG, j, 0);
			if (rc < 0)
				CAM_ERR(CAM_ACTUATOR, "bu74654 write failed reg:%x,rc:%d", BU64754_NVM_WRITE_ADDR_REG, rc);
			nvm_data = (data_buffer[i] << BU64754_SHIFT_8BIT) | (data_buffer[i + 1]);
			if (once) {
				CAM_INFO(CAM_ACTUATOR, "nvm_data = 0x%x", nvm_data);
				once--;
			}
			rc = vcm_write_reg_16bit_data(a_ctrl, BU64754_NVM_WRITE_DATA_REG, nvm_data, 0);
			if (rc < 0)
				CAM_ERR(CAM_ACTUATOR, "bu74654 write failed reg:%x,rc:%d", BU64754_NVM_WRITE_DATA_REG, rc);
		}

		rc = vcm_write_reg_16bit_data(a_ctrl, BU64754_NVM_START_REG, BU64754_NVM_START_DISABLE, 0);
		if (rc < 0)
			CAM_ERR(CAM_ACTUATOR, "bu74654 write failed reg:%x,rc:%d", BU64754_NVM_START_REG, rc);
		rc = vcm_write_reg_16bit_data(a_ctrl, BU64754_POWER_ON_OFF_REG, BU64754_POWER_OFF, 0);
		if (rc < 0)
			CAM_ERR(CAM_ACTUATOR, "bu74654 write failed reg:%x,rc:%d", BU64754_POWER_ON_OFF_REG, rc);
		rc = vcm_write_reg_16bit_data(a_ctrl, BU64754_POWER_ON_OFF_REG, BU64754_POWER_ON, 2);
		if (rc < 0)
			CAM_ERR(CAM_ACTUATOR, "bu74654 write failed reg:%x,rc:%d", BU64754_POWER_ON_OFF_REG, rc);

		rc = camera_io_dev_read(&(a_ctrl->io_master_info), BU64754_CHECKSUM, &checksum,
			CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_WORD);
		if (rc < 0)
			CAM_ERR(CAM_ACTUATOR, "bu74654 read failed reg:%x,rc:%d", BU64754_CHECKSUM, rc);
	} while ((checksum != BU64754_CHECKSUM_VALUE) && (--timeout));

	if (checksum != BU64754_CHECKSUM_VALUE) {
		CAM_ERR(CAM_ACTUATOR, "bu64754_rewrite_into_nvm try %d times, but failed", BU64754_TRY_TIMEOUT);
		return -EINVAL;
	}
	return rc;
}

int bu64754_judge_checksum(struct cam_actuator_ctrl_t *a_ctrl)
{
	unsigned int checksum = 0;
	if (!a_ctrl) {
		CAM_ERR(CAM_ACTUATOR, "Invalid Args");
		return -EINVAL;
	}

	if (camera_io_dev_read(&(a_ctrl->io_master_info), BU64754_CHECKSUM, &checksum,
			CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_WORD) < 0)
		CAM_ERR(CAM_ACTUATOR, "bu74654 checksum read failed reg:%x", BU64754_CHECKSUM);

	if (checksum != BU64754_CHECKSUM_VALUE) {
		CAM_DBG(CAM_ACTUATOR, "checksum is 0x%x", checksum);
		return -EINVAL;
	}

	return 0;
}
