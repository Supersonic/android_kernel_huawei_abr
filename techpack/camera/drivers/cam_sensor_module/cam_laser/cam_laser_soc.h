/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * Description: laser driver
 *
 * Author: lvyali
 *
 * Create: 2021-05-06
 */

#ifndef _CAM_LASER_SOC_H_
#define _CAM_LASER_SOC_H_

#include <linux/input.h>
#include "cam_laser_dev.h"

static const char *laser_name;
static struct input_dev *laser_input_dev;

struct vendor_data {
    struct tof_sensor_chip *data0; /* tof8805 */
    struct stmvl53l1_data *data1; /* stmvl53l1 */
    struct vi5300_data *data2; /* vi5300 */
};

int cam_laser_driver_soc_init(struct cam_laser_ctrl_t *o_ctrl);
int laser_probe (struct i2c_client *client, const struct i2c_device_id *id);
int laser_remove(struct i2c_client *client);

#endif/* _CAM_LASER_SOC_H_ */
