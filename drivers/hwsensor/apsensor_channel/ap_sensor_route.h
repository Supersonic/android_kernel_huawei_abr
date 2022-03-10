/*
 * motion_route.h
 *
 * code for motion_route
 *
 * Copyright (c) 2020 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#ifndef __LINUX_MOTION_ROUTE_H__
#define __LINUX_MOTION_ROUTE_H__
#include <linux/version.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>

#define HWLOG_TAG hw_motion_route
HWLOG_REGIST();

#define MAX_MOTION_DATA_PARA_LEN      6

#pragma pack (1)
struct ap_sensor_manager_cmd_t {
	uint8_t sensor_type;
	uint8_t action;
	uint8_t reserved1;
	uint8_t reserved2;
	int32_t data[3];
};

struct ap_sensor_data_t {
    uint8_t sensor_type;
    uint8_t action;
    uint8_t reserved1;
    uint8_t reserved2;
    int32_t data_len;
};

struct custom_sensor_data_t {
    struct ap_sensor_data_t sensor_data_head;
    int sensor_data[16];
};

struct motion_data_paras_t {
    struct ap_sensor_data_t motion_head;
    int motion_data_para[MAX_MOTION_DATA_PARA_LEN];
};

struct extend_step_para_t {
       uint8_t motion_type;
       uint32_t begin_time;
       uint16_t record_count;
       uint16_t capacity;
       uint32_t total_step_count;
       uint32_t total_floor_ascend;
       uint32_t total_calorie;
       uint32_t total_distance;
       uint16_t step_pace;
       uint16_t step_length;
       uint16_t speed;
       uint16_t touchdown_ratio;
       uint16_t reserved1;
       uint16_t reserved2; // 36
       int action_record[120];
};

struct ext_step_counter_data_t {
    struct ap_sensor_data_t step_head;
    struct extend_step_para_t step_data;
};

#pragma pack()

typedef struct {
	int for_alignment;
	union {
		char effect_addr[sizeof(int)];
		int pkg_length;
	};
	int64_t timestamp;
} t_head;

struct inputhub_buffer_pos {
	char *pos;
	unsigned int buffer_size;
};

// Every route item can be used by one reader and one writer.
struct inputhub_route_table {
	struct inputhub_buffer_pos phead; /*point to the head of buffer*/
	struct inputhub_buffer_pos pRead; /*point to the read position of buffer*/
	struct inputhub_buffer_pos pWrite; /*point to the write position of buffer*/
	wait_queue_head_t read_wait; /*to block read when no data in buffer*/
	atomic_t data_ready;
	spinlock_t buffer_spin_lock; /*for read write buffer*/
};

int ap_sensor_route_init(void);
void ap_sensor_route_destroy(void);
ssize_t ap_sensor_route_read(char __user *buf, size_t count);
ssize_t ap_sensor_route_write(char *buf, size_t count);
#endif
