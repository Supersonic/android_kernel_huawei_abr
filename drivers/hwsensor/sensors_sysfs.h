/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SENSORS_SYSFS_H
#define _SENSORS_SYSFS_H


/* sensor tag for different node type */
/* cmd */
enum {
    TAG_SENSOR_BEGIN = 0x01,
    TAG_ACCEL = TAG_SENSOR_BEGIN,
    TAG_GYRO,
    TAG_MAG,
    TAG_ALS,
    TAG_PS, /* 5 */
};
/* sensor node type */
/* subcmd PS*/
enum {
    ST_RECEIVER_OFF = 0,
    ST_RECEIVER_ON,
    ALS_GET_RAW_DATA,
};
/* subcmd ALS*/
enum {
    SEND_UNDERSCREEN_CALIDATA,
    ENABLE_RAW_DATA,
    ENABLE_DEBUG_RAW_DATA,
};

#endif /* __LINUX_SENSORS_H_INCLUDED */
