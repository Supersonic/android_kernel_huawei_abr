/*
 * ir_core.h
 *
 * infrared core driver
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#ifndef _IR_CORE_H_
#define _IR_CORE_H_

#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/debugfs.h>

#define IR_BUF_SIZE         512
#define IR_CIR_SIZE         256
#define IR_TRACE_SIZE       1024
#define IR_MAX_DURATION     50000000 /* us */
#define IR_MIN_DURATION     70 /* us */
#define IR_MAX_DUTY_CYCLE   100
#define IR_DEF_DUTY_CYCLE   33
#define IR_DEF_CARRIER_FREQ 38000
#define IR_DEBUG_ROOT       "irctrl"

#define IR_SET_CARRIER    _IOW('i', 0x00000000, __u32)
#define IR_SET_DUTY_CYCLE _IOW('i', 0x00000001, __u32)

struct ir_ops {
	const char *name;
	int (*open)(struct device *dev);
	void (*close)(struct device *dev);
	int (*tx_carrier)(struct device *dev, u32 carrier);
	int (*tx_duty_cycle)(struct device *dev, u32 duty_cycle);
	int (*tx)(struct device *dev, const unsigned int *buf, u32 n);
};

struct ir_device {
	struct miscdevice misc_dev;
	struct device *parent;
	struct mutex lock;
	const struct ir_ops *ops;
	unsigned int carrier_freq;
	unsigned int duty_cycle;
#ifdef IR_CORE_DEBUG
	unsigned int trace_mode;
	struct dentry *sub;
#endif /* IR_CORE_DEBUG */
};

#ifdef CONFIG_HUAWEI_IR_CORE
struct ir_device *devm_ir_register_device(struct device *parent,
	const struct ir_ops *ops);
#else
static inline struct ir_device *devm_ir_register_device(struct device *parent,
	const struct ir_ops *ops)
{
	return NULL;
}
#endif /* CONFIG_HUAWEI_IR_CORE */

#endif /* _IR_CORE_H_ */
