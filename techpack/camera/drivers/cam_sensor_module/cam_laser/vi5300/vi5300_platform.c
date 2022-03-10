/*
 *  vi5300_module.c - Linux kernel modules for VI5300 FlightSense TOF
 *						 sensor
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include "vi5300_platform.h"
#include "vi5300.h"

static int32_t vi5300_i2c_write(VI5300_DEV dev, uint8_t reg, uint8_t *data, uint8_t len)
{
	int ret = 0;
	uint8_t *addr_buf;
	struct i2c_msg msg;
	struct i2c_client *client = dev->client;

	if(!client)
		return -EINVAL;
	addr_buf = kmalloc(len + 1, GFP_KERNEL);
	if (!addr_buf)
		return -ENOMEM;
	addr_buf[0] = reg;
	memcpy(&addr_buf[1], data, len);
	msg.addr = client->addr;
	msg.flags = 0;
	msg.buf = addr_buf;
	msg.len = len+1;

	ret = i2c_transfer(client->adapter, &msg, 1);
	if(ret != 1)
	{
		pr_err("%s: i2c_transfer err:%d, addr:0x%x, reg:0x%x\n",
			__func__, ret, client->addr, reg);
	}
	kfree(addr_buf);
	return ret < 0 ? ret : (ret != 1 ? -EIO : 0);
}

static int32_t vi5300_i2c_read(VI5300_DEV dev, uint8_t reg, uint8_t *data, uint8_t len)
{
	int ret =0;
	struct i2c_msg msg[2];
	struct i2c_client *client = dev->client;

	if(!client)
		return -EINVAL;
	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].buf = &reg;
	msg[0].len = 1;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = data;
	msg[1].len = len;
	ret = i2c_transfer(client->adapter, msg, 2);
	if(ret != 2)
	{
		pr_err("%s: i2c_transfer err:%d, addr:0x%x, reg:0x%x\n",
			__func__, ret, client->addr, reg);
	}

	return ret < 0 ? ret : (ret != 2 ? -EIO : 0);
}

int32_t vi5300_write_byte(VI5300_DEV dev, uint8_t reg, uint8_t data)
{
	return vi5300_i2c_write(dev, reg, &data, 1);
}

int32_t vi5300_read_byte(VI5300_DEV dev, uint8_t reg, uint8_t *data)
{
	return vi5300_i2c_read(dev, reg, data, 1);
}

int32_t vi5300_write_multibytes(VI5300_DEV dev, uint8_t reg, uint8_t *data, int32_t count)
{
	int i = 0;
	int state = 0;
	for (;(i + 1) <= (count / 23); ++i) {
		state = vi5300_i2c_write(dev, reg + i * 23, data + i * 23, 23);
	}
	if (count % 23) {
		state = vi5300_i2c_write(dev, reg + i * 23, data + i * 23, count % 23);
	}
	return state;
}

int32_t vi5300_read_multibytes(VI5300_DEV dev, uint8_t reg, uint8_t *data, int32_t count)
{
	int i = 0;
	int state = 0;
	for (;(i + 1) <= (count / 23); ++i) {
		state = vi5300_i2c_read(dev, reg + i * 23, data + i * 23, 23);
	}
	if (count % 23) {
		state = vi5300_i2c_read(dev, reg + i * 23, data + i * 23, count % 23);
	}
	return state;
}

int32_t vi5300_write_reg_offset(VI5300_DEV dev, uint8_t reg, uint8_t offset, uint8_t data)
{
	return vi5300_write_byte(dev, reg+offset, data);
}

int32_t vi5300_read_reg_offset(VI5300_DEV dev, uint8_t reg, uint8_t offset, uint8_t *data)
{
	return vi5300_read_byte(dev, reg+offset, data);
}
