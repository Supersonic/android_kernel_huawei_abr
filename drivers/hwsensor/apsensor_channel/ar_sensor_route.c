/*
 * ar_sensor_route.c
 *
 * code for ar sensor channel
 *
 * Copyright (c) 2020- Huawei Technologies Co., Ltd.
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

#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/rtc.h>
#include <linux/time64.h>
#include <ar_sensor_route.h>
#include <linux/vmalloc.h>


#define LENGTH_SIZE sizeof(unsigned int)
#define TIMESTAMP_AR_SIZE (8)

#define HEAD_SIZE (LENGTH_SIZE+TIMESTAMP_AR_SIZE)
#define ROUTE_BUFFER_MAX_SIZE (2048)

static struct arhub_route_table package_route_tbl =
	{{NULL, 0}, {NULL, 0}, {NULL, 0},
	__WAIT_QUEUE_HEAD_INITIALIZER(package_route_tbl.read_wait)};

int ar_sensor_route_init(void)
{
	char *pos = NULL;
	struct arhub_route_table *route_item = &package_route_tbl;
	hwlog_info("%s\n", __func__);
	pos = vzalloc(ROUTE_BUFFER_MAX_SIZE);
	if (!pos)
		return -ENOMEM;
	route_item->phead.pos = pos;
	route_item->pWrite.pos = pos;
	route_item->pRead.pos = pos;
	route_item->phead.buffer_size = ROUTE_BUFFER_MAX_SIZE;
	route_item->pWrite.buffer_size = ROUTE_BUFFER_MAX_SIZE;
	route_item->pRead.buffer_size = 0;
	spin_lock_init(&package_route_tbl.buffer_spin_lock);
	return 0;
}

void ar_sensor_route_destroy(void)
{
	struct arhub_route_table *route_item = &package_route_tbl;
	hwlog_info("%s\n", __func__);
	if (route_item->phead.pos)
		vfree(route_item->phead.pos);

	route_item->phead.pos = NULL;
	route_item->pWrite.pos = NULL;
	route_item->pRead.pos = NULL;
}

static inline bool data_ready(struct arhub_route_table *route_item, struct arhub_buffer_pos *reader)
{
	unsigned long flags = 0;
	spin_lock_irqsave(&route_item->buffer_spin_lock, flags);
	*reader = route_item->pRead;
	spin_unlock_irqrestore(&route_item->buffer_spin_lock, flags);
	return reader->buffer_size > 0;
}

ssize_t ar_sensor_route_read(char __user *buf, size_t count)
{
	struct arhub_route_table *route_item = NULL;
	struct arhub_buffer_pos reader;
	char *buffer_end;
	unsigned int full_pkg_length;
	unsigned int tail_half_len;
	unsigned long flags = 0;

	route_item = &package_route_tbl;
	buffer_end = route_item->phead.pos + route_item->phead.buffer_size;

	// woke up by signal
	if (wait_event_interruptible(route_item->read_wait, data_ready(route_item, &reader)) != 0)
		return 0;

	if (reader.buffer_size > ROUTE_BUFFER_MAX_SIZE) {
		hwlog_err("error reader.buffer_size = %d!\n", (int)reader.buffer_size);
		goto clean_buffer;
	}

	if (buffer_end - reader.pos >= LENGTH_SIZE) {
		full_pkg_length = *((unsigned int *)reader.pos);
		reader.pos += LENGTH_SIZE;
		if (reader.pos == buffer_end)
			reader.pos = route_item->phead.pos;
	} else {
		tail_half_len = buffer_end - reader.pos;
		memcpy(&full_pkg_length, reader.pos, tail_half_len);
		memcpy((char *)&full_pkg_length + tail_half_len, route_item->phead.pos,
			LENGTH_SIZE - tail_half_len);
		reader.pos = route_item->phead.pos + (LENGTH_SIZE - tail_half_len);
	}

	if (full_pkg_length + LENGTH_SIZE > reader.buffer_size || full_pkg_length > count) {
		hwlog_err("full_pkg_length = %u is too large!\n", full_pkg_length);
		goto clean_buffer;
	}

	if (buffer_end - reader.pos >= full_pkg_length) {
		if (0 == copy_to_user(buf, reader.pos, full_pkg_length)) {
			reader.pos += full_pkg_length;
			if (reader.pos == buffer_end)
				reader.pos = route_item->phead.pos;
		} else {
			hwlog_err("copy to user failed\n");
			return 0;
		}
	} else {
		tail_half_len = buffer_end - reader.pos;
		if ((0 == copy_to_user(buf, reader.pos, tail_half_len)) &&
			(0 == copy_to_user(buf + tail_half_len, route_item->phead.pos,
			(full_pkg_length - tail_half_len)))) {
			reader.pos = route_item->phead.pos + (full_pkg_length -tail_half_len);
		} else {
			hwlog_err("copy to user failed\n");
			return 0;
		}
	}
	spin_lock_irqsave(&route_item->buffer_spin_lock, flags);
	route_item->pRead.pos = reader.pos;
	route_item->pRead.buffer_size -= (full_pkg_length + LENGTH_SIZE);
	if ((route_item->pWrite.buffer_size > ROUTE_BUFFER_MAX_SIZE) ||
		(route_item->pWrite.buffer_size +
		(full_pkg_length + LENGTH_SIZE) > ROUTE_BUFFER_MAX_SIZE)) {
		hwlog_err("%s:%d write buffer error buffer_size=%u pkg_len=%u\n",
			__func__, __LINE__, route_item->pWrite.buffer_size, full_pkg_length);
		spin_unlock_irqrestore(&route_item->buffer_spin_lock, flags);
		goto clean_buffer;
	} else {
		route_item->pWrite.buffer_size += (full_pkg_length + LENGTH_SIZE);
	}
	spin_unlock_irqrestore(&route_item->buffer_spin_lock, flags);
	return full_pkg_length;

clean_buffer:
	hwlog_err("now we will clear the receive buffer in port.\n");
	spin_lock_irqsave(&route_item->buffer_spin_lock, flags);
	route_item->pRead.pos = route_item->pWrite.pos;
	route_item->pWrite.buffer_size = ROUTE_BUFFER_MAX_SIZE;
	route_item->pRead.buffer_size = 0;
	spin_unlock_irqrestore(&route_item->buffer_spin_lock, flags);
	return 0;
}

static int64_t getTimestamp(void)
{
	struct timespec ts;
	get_monotonic_boottime(&ts);
	/* timevalToNano */
	return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline void ar_sensor_write_to_fifo(struct arhub_buffer_pos *pwriter,
	char *const buffer_begin, char *const buffer_end, char *buf, int count)
{
	int cur_to_tail_len = buffer_end - pwriter->pos;

	if (cur_to_tail_len >= count) {
		memcpy(pwriter->pos, buf, count);
		pwriter->pos += count;
		if (buffer_end == pwriter->pos) {
			pwriter->pos = buffer_begin;
		}
	} else {
		memcpy(pwriter->pos, buf, cur_to_tail_len);
		memcpy(buffer_begin, buf + cur_to_tail_len, count - cur_to_tail_len);
		pwriter->pos = buffer_begin + (count - cur_to_tail_len);
	}
}

ssize_t ar_sensor_route_write(char *buf, size_t count)
{
	struct arhub_route_table *route_item;
	struct arhub_buffer_pos writer;
	char *buffer_begin = NULL;
	char *buffer_end = NULL;
	t_head header;
	unsigned long flags = 0;
	int i = 0;

	route_item = &package_route_tbl;
	header.timestamp = getTimestamp();
	for (i=0; i < count; i++)
		hwlog_debug("ar_route_write buf[%d]=%d.\n",i,buf[i]);
	spin_lock_irqsave(&route_item->buffer_spin_lock, flags);
	writer = route_item->pWrite;

	if (writer.buffer_size < count + HEAD_SIZE) {
		spin_unlock_irqrestore(&route_item->buffer_spin_lock, flags);
		hwlog_info("ar writer.buffer_size too short %d %d.\n", writer.buffer_size, count);
		return 0;
	}

	buffer_begin = route_item->phead.pos;
	buffer_end = route_item->phead.pos + route_item->phead.buffer_size;
	header.pkg_length = count + sizeof(int64_t);
	ar_sensor_write_to_fifo(&writer, buffer_begin, buffer_end, header.effect_addr, HEAD_SIZE);
	ar_sensor_write_to_fifo(&writer, buffer_begin, buffer_end, buf, count);

	route_item->pWrite.pos = writer.pos;
	route_item->pWrite.buffer_size -= (count + HEAD_SIZE);
	route_item->pRead.buffer_size += (count + HEAD_SIZE);
	spin_unlock_irqrestore(&route_item->buffer_spin_lock, flags);
	atomic_set(&route_item->data_ready, 1);
	wake_up_interruptible(&route_item->read_wait);
	hwlog_info("ar_sensor_route_write route_item->pRead.buffer_size=%d.\n",route_item->pRead.buffer_size);
	return (count + HEAD_SIZE);
}
