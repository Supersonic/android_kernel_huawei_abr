/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * Description: laser driver
 *
 * Author: lvyali
 *
 * Create: 2021-05-06
 */

#ifndef _CAM_LASER_DEV_H_
#define _CAM_LASER_DEV_H_

#include <linux/i2c.h>
#include <linux/gpio.h>
#include <media/v4l2-event.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-ioctl.h>
#include <media/cam_sensor.h>
#include <cam_sensor_i2c.h>
#include <cam_sensor_spi.h>
#include <cam_sensor_io.h>
#include <cam_cci_dev.h>
#include <cam_req_mgr_util.h>
#include <cam_req_mgr_interface.h>
#include <cam_mem_mgr.h>
#include <cam_subdev.h>
#include <sensors_class.h>
#include "cam_soc_util.h"
#include "cam_context.h"

#define DEVICE_LOAD_CURRENT	18000 /* VDD average power consumption, units: ua*/
#define PROXIMITY_MIN_DELAY				30000 //in microseconds
#define PROXIMITY_FIFO_RESERVED_COUNT	0
#define PROXIMITY_FIFO_MAX_COUNT		0

#define DEFINE_MSM_MUTEX(mutexname) \
	static struct mutex mutexname = __MUTEX_INITIALIZER(mutexname)

enum cam_laser_state {
	CAM_LASER_INIT,
	CAM_LASER_ACQUIRE,
	CAM_LASER_CONFIG,
	CAM_LASER_START,
};

/**
 * struct cam_laser_registered_driver_t - registered driver info
 * @platform_driver      :   flag indicates if platform driver is registered
 * @i2c_driver           :   flag indicates if i2c driver is registered
 *
 */
struct cam_laser_registered_driver_t {
	bool platform_driver;
	bool i2c_driver;
};

/**
 * struct cam_laser_i2c_info_t - I2C info
 * @slave_addr      :   slave address
 * @i2c_freq_mode   :   i2c frequency mode
 *
 */
struct cam_laser_i2c_info_t {
	uint16_t slave_addr;
	uint8_t i2c_freq_mode;
};

/**
 * struct cam_laser_soc_private - laser soc private data structure
 * @laser_name      :   laser name
 * @i2c_info        :   i2c info structure
 * @power_info      :   laser power info
 *
 */
struct cam_laser_soc_private {
	const char *laser_name;
	struct cam_laser_i2c_info_t i2c_info;
	struct cam_sensor_power_ctrl_t power_info;
};

/**
 * struct cam_laser_intf_params - bridge interface params
 * @device_hdl   : Device Handle
 * @session_hdl  : Session Handle
 * @ops          : KMD operations
 * @crm_cb       : Callback API pointers
 */
struct cam_laser_intf_params {
	int32_t device_hdl;
	int32_t session_hdl;
	int32_t link_hdl;
	struct cam_req_mgr_kmd_ops ops;
	struct cam_req_mgr_crm_cb *crm_cb;
};

/**
 * struct cam_laser_ctrl_t - laser ctrl private data
 * @device_name     :   laser device_name
 * @pdev            :   platform device
 * @laser_mutex       :   laser mutex
 * @soc_info        :   laser soc related info
 * @io_master_info  :   Information about the communication master
 * @cci_i2c_master  :   I2C structure
 * @v4l2_dev_str    :   V4L2 device structure
 * @bridge_intf     :   bridge interface params
 * @i2c_init_data   :   laser i2c init settings
 * @i2c_mode_data   :   laser i2c mode settings
 * @i2c_calib_data  :   laser i2c calib settings
 * @laser_device_type :   laser device type
 * @cam_laser_state :   laser_device_state
 * @laser_fw_flag     :   flag for firmware download
 * @is_laser_calib    :   flag for Calibration data
 * @opcode          :   laser opcode
 * @device_name     :   Device name
 *
 */
struct cam_laser_ctrl_t {
	char device_name[CAM_CTX_DEV_NAME_MAX_LENGTH];
	struct platform_device *pdev;
	struct sensors_classdev *class_dev;
	struct mutex laser_mutex;
	struct cam_hw_soc_info soc_info;
	struct camera_io_master io_master_info;
	struct i2c_settings_array i2c_init_data;
	struct i2c_settings_array i2c_calib_data;
	struct i2c_settings_array i2c_mode_data;
	enum msm_camera_device_type_t laser_device_type;
	enum cam_laser_state cam_laser_state;
	char laser_name[32];
	uint8_t laser_fw_flag;
	uint8_t is_laser_calib;
	uint32_t open_cnt;
	int reg;
	const struct attribute_group **laser_attr_group;
};

typedef struct _tag_laser_module_intf_t {
	char *laser_name;
	int (*data_init)(struct i2c_client *client,
		const struct i2c_device_id *id);
	int (*data_remove)(struct i2c_client *client);
} laser_module_intf_t;

int cam_laser_driver_init(void);
void cam_laser_driver_exit(void);
#endif /*_CAM_LASER_DEV_H_ */
