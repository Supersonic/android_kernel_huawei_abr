/*
 * ar_sensor_route.h
 *
 * code for ar_sesnor_route
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

#define HWLOG_TAG hw_ar_route
HWLOG_REGIST();

typedef struct {
	int for_alignment;
	union {
		char effect_addr[sizeof(int)];
		int pkg_length;
	};
	int64_t timestamp;
} t_head;

struct arhub_buffer_pos {
	char *pos;
	unsigned int buffer_size;
};

// Every route item can be used by one reader and one writer.
struct arhub_route_table {
	struct arhub_buffer_pos phead; /*point to the head of buffer*/
	struct arhub_buffer_pos pRead; /*point to the read position of buffer*/
	struct arhub_buffer_pos pWrite; /*point to the write position of buffer*/
	wait_queue_head_t read_wait; /*to block read when no data in buffer*/
	atomic_t data_ready;
	spinlock_t buffer_spin_lock; /*for read write buffer*/
};

int ar_sensor_route_init(void);
void ar_sensor_route_destroy(void);
ssize_t ar_sensor_route_read(char __user *buf, size_t count);
ssize_t ar_sensor_route_write(char *buf, size_t count);
#endif
