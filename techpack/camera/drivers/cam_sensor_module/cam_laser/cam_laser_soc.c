/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * Description: laser driver
 *
 * Author: lvyali
 *
 * Create: 2021-05-06
 */

#include <linux/of.h>
#include <linux/of_gpio.h>
#include <cam_sensor_cmn_header.h>
#include <cam_sensor_util.h>
#include <cam_sensor_io.h>
#include <cam_req_mgr_util.h>
#include <linux/input.h>
#include <linux/sysfs.h>
#include <linux/errno.h>
#include <sensors_class.h>
#include "cam_soc_util.h"
#include "cam_laser_soc.h"
#include "cam_debug_util.h"
#include "tof8801_driver.h"
#include "stmvl53l1-i2c.h"
#include "vi5300.h"

/**
 * @o_ctrl: ctrl structure
 *
 * This function is called from cam_laser_platform/i2c_driver_probe, it parses
 * the laser dt node.
 */
int cam_laser_driver_soc_init(struct cam_laser_ctrl_t *o_ctrl)
{
	int                             rc = 0;
	struct cam_hw_soc_info         *soc_info = &o_ctrl->soc_info;
	struct device_node             *of_node = NULL;

	if (!soc_info->dev) {
		CAM_ERR(CAM_LASER, "soc_info is not initialized");
		return -EINVAL;
	}

	of_node = soc_info->dev->of_node;
	if (!of_node) {
		CAM_ERR(CAM_LASER, "dev.of_node NULL");
		return -EINVAL;
	}

	rc = of_property_read_string_index(of_node, "compatible", 0,
		(const char **)&soc_info->compatible);
	if (rc) {
		CAM_DBG(CAM_LASER, "No compatible string present for: %s",
			soc_info->dev_name);
		rc = 0;
	}

	rc = cam_soc_util_get_dt_properties(soc_info);
	if (rc)
		return rc;

	return rc;
}

static int input_dev_open(struct input_dev *dev)
{
	struct cam_laser_ctrl_t *o_ctrl = input_get_drvdata(dev);
	int rc = 0;
	uint16_t gpio_enable = 0;
	CAM_INFO(CAM_LASER, "%s\n", __func__);

	gpio_enable = o_ctrl->soc_info.gpio_data->cam_gpio_common_tbl[0].gpio;
	if (gpio_enable && (gpio_get_value(gpio_enable) == 0)) {
		/* enable the chip */
		rc = gpio_direction_output(gpio_enable, 1);
		if (rc) {
			CAM_ERR(CAM_LASER, "Chip enable failed.\n");
			return -EIO;
		}
	}
	return rc;
}

int laser_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;
	struct vendor_data *chip_data = NULL;
	struct cam_laser_ctrl_t *o_ctrl = i2c_get_clientdata(client);
	laser_name = o_ctrl->soc_info.dev_name;
	CAM_INFO(CAM_LASER, "laser_name is %s", laser_name);
	chip_data = kzalloc(sizeof(*chip_data), GFP_KERNEL);

	if (!chip_data) {
		CAM_ERR(CAM_LASER, "kzalloc failedv");
		rc = -ENOMEM;
		goto probe_failure;
	}

	laser_input_dev = devm_input_allocate_device(&client->dev);
	if (laser_input_dev == NULL) {
		CAM_ERR(CAM_LASER, "Error allocating input_dev");
		goto input_dev_alloc_err;
	}

	laser_input_dev->name = laser_name;
	laser_input_dev->id.bustype = BUS_I2C;
	input_set_drvdata(laser_input_dev, o_ctrl);
	laser_input_dev->open = input_dev_open;
	set_bit(EV_ABS, laser_input_dev->evbit);
	input_set_abs_params(laser_input_dev, ABS_DISTANCE, 0, 0xff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT0X, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT0Y, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT1X, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT1Y, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT2X, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT2Y, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT3X, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_HAT3Y, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_WHEEL, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_TILT_Y, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_BRAKE, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_TILT_X, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_TOOL_WIDTH, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_THROTTLE, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_RUDDER, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_MISC, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_VOLUME, 0, 0xffffffff, 0, 0);
	input_set_abs_params(laser_input_dev, ABS_GAS, 0, 0xffffffff, 0, 0);

	rc = input_register_device(laser_input_dev);
	if (rc) {
		CAM_ERR(CAM_LASER, "Error registering input_dev");
		goto input_reg_err;
	}

	if (strcmp(laser_name, "tof8801") == 0) {
		chip_data->data0 = (struct tof_sensor_chip *)(o_ctrl->soc_info.soc_private);
		chip_data->data0->obj_input_dev = laser_input_dev;
		CAM_INFO(CAM_LASER, "tof8801 input_dev");
	} else if (strcmp(laser_name, "stmvl53l1") == 0) {
		chip_data->data1 = (struct stmvl53l1_data *)(o_ctrl->soc_info.soc_private);
		chip_data->data1->input_dev_ps = laser_input_dev;
		CAM_INFO(CAM_LASER, "stmvl53l1 input_dev");
	} else if (strcmp(laser_name, "vi5300") == 0) {
		chip_data->data2 = (struct vi5300_data *)(o_ctrl->soc_info.soc_private);
		chip_data->data2->input_dev = laser_input_dev;
		CAM_INFO(CAM_LASER, "vi5300 input_dev");
	} else {
		CAM_ERR(CAM_LASER, "Error probed laser name");
		return -ENODEV;
	}

	rc = sensors_classdev_register(&laser_input_dev->dev, o_ctrl->class_dev);
	if (rc) {
		CAM_ERR(CAM_LASER, "Error registering sensor class");
		goto input_dev_alloc_err;
	}

	rc = sysfs_create_groups(&laser_input_dev->dev.kobj, o_ctrl->laser_attr_group);
	if (rc) {
		CAM_ERR(CAM_LASER, "Error creating sysfs attribute group");
		goto create_class_sysfs_err;
	}

	return 0;

/***** Failure case(s), unwind and return error *****/
create_class_sysfs_err:
	sensors_classdev_unregister(o_ctrl->class_dev);
input_dev_alloc_err:
input_reg_err:
	input_free_device(laser_input_dev);
	i2c_set_clientdata(client, NULL);
	dev_info(&client->dev, "Probe failed.\n");
probe_failure:

	return rc;
}

int laser_remove(struct i2c_client *client)
{
	struct cam_laser_ctrl_t *o_ctrl = NULL;
	if (!client) {
		CAM_ERR(CAM_LASER, "input client is NULL");
		return -EINVAL;
	}

	o_ctrl = i2c_get_clientdata(client);
	sysfs_remove_groups(&client->dev.kobj, o_ctrl->laser_attr_group);
	sensors_classdev_unregister(o_ctrl->class_dev);
	input_free_device(laser_input_dev);
	i2c_set_clientdata(client, NULL);
	return 0;
}
