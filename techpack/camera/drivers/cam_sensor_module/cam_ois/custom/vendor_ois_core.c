/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor OIS core driver
 */

#include <linux/module.h>
#include <linux/firmware.h>
#include <linux/dma-contiguous.h>
#include <cam_sensor_cmn_header.h>

#include "vendor_ois_core.h"

int ois_write_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
		uint16_t reg, uint16_t value)
{
	struct cam_sensor_i2c_reg_setting  i2c_reg_setting;
	struct cam_sensor_i2c_reg_array i2c_reg_array;
	int32_t rc = 0;

	i2c_reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_setting.size = 1;
	i2c_reg_setting.delay = 0;

	i2c_reg_setting.reg_setting = &i2c_reg_array;

	i2c_reg_setting.reg_setting[0].reg_addr = reg;
	i2c_reg_setting.reg_setting[0].reg_data = value;
	i2c_reg_setting.reg_setting[0].delay = 0;
	i2c_reg_setting.reg_setting[0].data_mask = 0;


	rc = camera_io_dev_write(&(o_ctrl->io_master_info),
		&i2c_reg_setting);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois write failed reg:%x,value:%x,rc:%d", reg, value, rc);

	return rc;
}

int ois_write_addr16_value32(struct cam_ois_ctrl_t *o_ctrl,
		uint16_t reg, uint32_t value)
{
	struct cam_sensor_i2c_reg_setting  i2c_reg_setting;
	struct cam_sensor_i2c_reg_array i2c_reg_array;
	int32_t rc = 0;

	i2c_reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_DWORD;
	i2c_reg_setting.size = 1;
	i2c_reg_setting.delay = 0;

	i2c_reg_setting.reg_setting = &i2c_reg_array;

	i2c_reg_setting.reg_setting[0].reg_addr = reg;
	i2c_reg_setting.reg_setting[0].reg_data = value;
	i2c_reg_setting.reg_setting[0].delay = 0;
	i2c_reg_setting.reg_setting[0].data_mask = 0;


	rc = camera_io_dev_write(&(o_ctrl->io_master_info),
		&i2c_reg_setting);
	if (rc < 0)
		CAM_ERR(CAM_OIS, " ois write failed reg:%x,value:%x,rc:%d", reg, value, rc);

	return rc;
}

int ois_read_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
		uint16_t reg, uint16_t *value)
{
	int32_t rc = 0;

	rc = camera_io_dev_read(&(o_ctrl->io_master_info), (uint32_t)reg, (uint32_t *)value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois read failed reg:%x,rc:%d", reg, rc);

	return rc;
}

int ois_read_addr16_value32(struct cam_ois_ctrl_t *o_ctrl,
		uint16_t reg, uint32_t *value)
{
	int32_t rc = 0;

	rc = camera_io_dev_read(&(o_ctrl->io_master_info), (uint32_t)reg, (uint32_t *)value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_DWORD);
	if (rc < 0)
		CAM_ERR(CAM_OIS, " ois read failed reg:%x,rc:%d", reg, rc);

	return rc;
}

int ois_i2c_block_write_reg(struct cam_ois_ctrl_t *o_ctrl,
		uint16_t reg, uint16_t *wr_buf, int32_t length, int32_t flag)
{
	uint32_t                           pkt_size;
	uint16_t                           *ptr = NULL;
	int32_t                            rc = 0, cnt;
	struct cam_sensor_i2c_reg_setting  i2c_reg_setting;
	void                              *vaddr = NULL;

	i2c_reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_setting.size = length;
	i2c_reg_setting.delay = 0;
	pkt_size = (sizeof(struct cam_sensor_i2c_reg_array) * length);
	vaddr = vmalloc(pkt_size);
	if (!vaddr) {
		CAM_ERR(CAM_OIS,
			"Failed in allocating i2c_array: pkt_size: %u", pkt_size);
		return -ENOMEM;
	}

	i2c_reg_setting.reg_setting = (struct cam_sensor_i2c_reg_array *) (
		vaddr);

	for (cnt = 0, ptr = wr_buf; cnt < length; cnt++, ptr++) {
		i2c_reg_setting.reg_setting[cnt].reg_addr = reg;
		i2c_reg_setting.reg_setting[cnt].reg_data = *ptr;
		i2c_reg_setting.reg_setting[cnt].delay = 0;
		i2c_reg_setting.reg_setting[cnt].data_mask = 0;
	}

	/* we use busrt write MSM_CCI_I2C_WRITE_BURST */
	rc = camera_io_dev_write_continuous(&(o_ctrl->io_master_info),
		&i2c_reg_setting, flag);
	if (rc < 0)
		CAM_ERR(CAM_OIS, "ois block write failed %d", rc);

	vfree(vaddr);
	vaddr = NULL;

	return rc;
}

int ois_i2c_block_write_addr8_value8(struct cam_ois_ctrl_t *o_ctrl, uint8_t addr,
		uint8_t *data, uint32_t num_byte, int32_t flag)
{
	struct cam_sensor_i2c_reg_setting i2c_reg_setting;
	int ret = -1;
	uint32_t cnt;

	if (o_ctrl == NULL || data == NULL) {
		CAM_ERR(CAM_OIS, "Invalid Args o_ctrl: %pK, data: %pK",
			o_ctrl, data);
		return -EINVAL;
	}

	i2c_reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_setting.size = num_byte;
	i2c_reg_setting.delay = 0;
	i2c_reg_setting.reg_setting =
	    (struct cam_sensor_i2c_reg_array *)
	    kzalloc(sizeof(struct cam_sensor_i2c_reg_array) * num_byte,
		    GFP_KERNEL);
	if (i2c_reg_setting.reg_setting == NULL) {
		CAM_ERR(CAM_OIS, "kzalloc failed");
		return -EINVAL;
	}
	i2c_reg_setting.reg_setting[0].reg_addr = addr;
	i2c_reg_setting.reg_setting[0].reg_data = data[0];
	i2c_reg_setting.reg_setting[0].delay = 0;
	i2c_reg_setting.reg_setting[0].data_mask = 0;
	for (cnt = 1; cnt < num_byte; cnt++) {
		i2c_reg_setting.reg_setting[cnt].reg_addr = 0;
		i2c_reg_setting.reg_setting[cnt].reg_data = data[cnt];
		i2c_reg_setting.reg_setting[cnt].delay = 0;
		i2c_reg_setting.reg_setting[cnt].data_mask = 0;
	}
	ret = camera_io_dev_write_continuous(&(o_ctrl->io_master_info),
					   &i2c_reg_setting, flag);
	if (ret < 0) {
		CAM_ERR(CAM_OIS, "cci write_continuous failed %d", ret);
		kfree(i2c_reg_setting.reg_setting);
		return ret;
	}
	kfree(i2c_reg_setting.reg_setting);
	return ret;
}

int ois_i2c_block_write_reg_value8(struct cam_ois_ctrl_t *o_ctrl,
		uint16_t reg, uint8_t *wr_buf, int16_t length, int32_t flag)
{
	uint32_t                           pkt_size;
	uint8_t                           *ptr = NULL;
	int32_t                            rc = 0, cnt;
	struct cam_sensor_i2c_reg_setting  i2c_reg_setting;
	void                              *vaddr = NULL;

	i2c_reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_setting.size = length;
	i2c_reg_setting.delay = 0;
	pkt_size = (sizeof(struct cam_sensor_i2c_reg_array) * length);
	vaddr = vmalloc(pkt_size);
	if (!vaddr) {
		CAM_ERR(CAM_OIS,
			"Failed in allocating i2c_array: pkt_size: %u", pkt_size);
		return -ENOMEM;
	}

	i2c_reg_setting.reg_setting = (struct cam_sensor_i2c_reg_array *) (
		vaddr);

	for (cnt = 0, ptr = wr_buf; cnt < length; cnt++, ptr++) {
		i2c_reg_setting.reg_setting[cnt].reg_addr = reg;
		i2c_reg_setting.reg_setting[cnt].reg_data = *ptr;
		i2c_reg_setting.reg_setting[cnt].delay = 0;
		i2c_reg_setting.reg_setting[cnt].data_mask = 0;
	}

	/* we use busrt write MSM_CCI_I2C_WRITE_BURST */
	rc = camera_io_dev_write_continuous(&(o_ctrl->io_master_info),
		&i2c_reg_setting, flag);
	if (rc < 0)
		CAM_ERR(CAM_OIS, " ois block write reg:%x failed, rc:%d", reg, rc);

	vfree(vaddr);
	vaddr = NULL;

	return rc;
}

int ois_i2c_block_read_reg(struct cam_ois_ctrl_t *o_ctrl,
		uint16_t reg, uint32_t *wr_buf, uint16_t length,
		enum camera_sensor_i2c_type addr_type,
		enum camera_sensor_i2c_type data_type)
{
	int32_t rc = 0;
	int i;
	uint8_t *data = (uint8_t *)wr_buf;
	uint16_t *data_short = NULL;
	rc = camera_io_dev_read_seq(&(o_ctrl->io_master_info), (uint32_t)reg, data,
		addr_type, data_type, (int32_t)length);
	if (rc < 0) {
		CAM_ERR(CAM_OIS, " ois read failed reg:%x,rc:%d", reg, rc);
	} else {
		if (data_type == CAMERA_SENSOR_I2C_TYPE_BYTE) {
			return rc;
		} else if (data_type == CAMERA_SENSOR_I2C_TYPE_WORD) {
			data_short = (uint16_t *)data;
			for (i = 0; i < length; i = i + 2)
				data_short[i / 2] = data[i] << 8 | data[i + 1];
		} else if (data_type == CAMERA_SENSOR_I2C_TYPE_DWORD) {
			for (i = 0; i < length; i = i + 4)
				wr_buf[i / 4] = data[i] << 24 | data[i + 1] << 16 |
					data[i + 2] << 8 | data[i + 3];
		} else {
			CAM_ERR(CAM_OIS, "data type invalid");
		}
	}

	return rc;
}
