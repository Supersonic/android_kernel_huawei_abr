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

#ifndef _SENSORS_SYSFS_PS_H
#define _SENSORS_SYSFS_PS_H

#define PS_IOCTL_PKG_HEADER 4
#define PS_NODE_INFO_CNT    3

/* freq value */
#define ST_LCDFREQ_30HZ     30
#define ST_LCDFREQ_60HZ     60
#define ST_LCDFREQ_90HZ     90
#define ST_LCDFREQ_120HZ    120

typedef struct ps_node_buf_to_hal_t {
    uint8_t sensor_type;
    uint8_t cmd;
    uint8_t subcmd;
    uint8_t reserved;
    uint32_t data[PS_NODE_INFO_CNT];
} ps_node_buf_to_hal_t;

/* sensor node type */

/* node payload */
enum {
    NODE_INFO_TYPE,
    NODE_INFO_VALUE,
    NODE_OTHER_INFO
};


/* node info type */
enum {
    SUBCOM_RECV,
    SUNCOM_LCDFREQ
};

/* recv value */
enum {
    ST_RECEIVER_OFF = 0,
    ST_RECEIVER_ON
};

ssize_t store_send_lcdfreq_req(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count);
ssize_t show_send_lcdfreq_req(struct device *dev,
    struct device_attribute *attr, char *buf);
ssize_t store_send_receiver_req(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count);
ssize_t show_send_receiver_req(struct device *dev,
    struct device_attribute *attr, char *buf);

#endif		/* __LINUX_SENSORS_H_INCLUDED */
