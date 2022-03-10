/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Team:    Huawei DIVS
 * Date:    2020.07.20
 * Description: xhub logbuff module
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/sysfs.h>
#include <securec.h>
#include <apsensor_channel/ap_sensor_route.h>
#include <apsensor_channel/ap_sensor.h>

#define FLUSH_BUFF_SIZE      2
#define XHUB_LOG_ABNORMAL '2'
#define XHUB_LOG_ON '1'
#define XHUB_LOG_OFF '0'
#define TAG_LOG_FOR_NODE 6
#define LOG_IOCTL_PKG_HEADER 4
#define ERROR_LEVEL 3

static struct semaphore logbuff_sem;
static struct semaphore logon_sem;
static char log_on = XHUB_LOG_OFF;

typedef struct log_to_hal {
	uint8_t sensor_type;
	uint8_t cmd;
	uint8_t subcmd;
	uint8_t reserved;
	uint32_t level;
} log_to_hal_t;

enum {
	SET_LOG_LEVEL = 1,
	SET_LOG_MASK = 2,
};

static ssize_t log_on_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret;

	ret = down_interruptible(&logon_sem);
	if (ret == 0)
		buf[0] = log_on;
	else
		buf[0] = XHUB_LOG_ABNORMAL;
	return 1;
}

static ssize_t log_on_store(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	if (buf[0] == XHUB_LOG_ON || buf[0] == XHUB_LOG_OFF) {
		log_on = buf[0];
		up(&logon_sem);
		pr_info("set logon_sem value %d\n", log_on);
	}
	return count;
}

static DEVICE_ATTR(log_on, S_IWUSR | S_IRUGO, log_on_show, log_on_store);

static ssize_t logbuff_flush_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret;

	ret = down_interruptible(&logbuff_sem);
	if (ret != 0)
		return snprintf_s(buf, FLUSH_BUFF_SIZE, FLUSH_BUFF_SIZE - 1, "1");
	pr_info("get logbuff_sem\n");
	return snprintf_s(buf, FLUSH_BUFF_SIZE, FLUSH_BUFF_SIZE - 1, "0");
}

static ssize_t logbuff_flush_store(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	up(&logbuff_sem);
	return count;
}

static DEVICE_ATTR(logbuff_flush, S_IWUSR | S_IRUGO, logbuff_flush_show, logbuff_flush_store);

static ssize_t set_log_level_store(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	uint32_t log_level;
	log_to_hal_t level_buf;

	log_level = simple_strtol(buf, NULL, 10); /* 10:Decimal */
	if (log_level > ERROR_LEVEL) {
		pr_err("error log level is %d\n", log_level);
		return count;
	}
	level_buf.sensor_type= SENSOR_TYPE_SENSOR_NODE;
	level_buf.cmd = TAG_LOG_FOR_NODE;
	level_buf.subcmd = SET_LOG_LEVEL;
	level_buf.level = log_level;

	pr_info("log level is %d\n", log_level);
	ap_sensor_route_write((char *)&level_buf, sizeof(uint8_t) + LOG_IOCTL_PKG_HEADER);
	return count;
}

static DEVICE_ATTR(set_log_level, S_IWUSR | S_IRUGO, NULL, set_log_level_store);

static ssize_t enable_log_module_store(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	uint32_t log_module_id;
	log_to_hal_t log_module_buf;

	log_module_id = simple_strtol(buf, NULL, 10); /* 10:Decimal */

	log_module_buf.sensor_type= SENSOR_TYPE_SENSOR_NODE;
	log_module_buf.cmd = TAG_LOG_FOR_NODE;
	log_module_buf.subcmd = SET_LOG_MASK;
	log_module_buf.level = log_module_id;

	pr_info("log module id is 0x%x\n", log_module_id);
	ap_sensor_route_write((char *)&log_module_buf, sizeof(int32_t) + LOG_IOCTL_PKG_HEADER);
	return count;
}

static DEVICE_ATTR(enable_log_module, S_IWUSR | S_IRUGO, NULL, enable_log_module_store);


static struct platform_device xhub_logbuff = {
	.name = "huawei_sensorhub_logbuff",
	.id = -1,
};

int logbuff_device_register(void)
{
	int ret;

	ret = platform_device_register(&xhub_logbuff);
	if (ret) {
		pr_err("platform_device_register failed, ret:%d.\n", ret);
		return -1;
	}
	return 0;
}

int creat_log_device_file(void)
{
	int ret;

	ret = device_create_file(&xhub_logbuff.dev,
		&dev_attr_logbuff_flush);
	if (ret) {
		pr_err("create %s file failed, ret:%d.\n", "dev_attr_logbuff_flush", ret);
		return -1;
	}

	ret = device_create_file(&xhub_logbuff.dev,
		&dev_attr_log_on);
	if (ret) {
		device_remove_file(&xhub_logbuff.dev,
		&dev_attr_logbuff_flush);
		pr_err("create %s file failed, ret:%d.\n", "dev_attr_log_on", ret);
		return -1;
	}

	ret = device_create_file(&xhub_logbuff.dev,
		&dev_attr_set_log_level);
	if (ret) {
		pr_err("create %s file failed, ret:%d.\n", "dev_attr_set_log_level", ret);
		return -1;
	}

	ret = device_create_file(&xhub_logbuff.dev,
		&dev_attr_enable_log_module);
	if (ret) {
		pr_err("create %s file failed, ret:%d.\n", "dev_attr_set_log_level", ret);
		return -1;
	}

	return 0;
}

static inline void logbuff_init_success(void)
{
	sema_init(&logbuff_sem, 0);
	sema_init(&logon_sem, 0);
	pr_info("logbuff_init_success done\n");
}

static int xhub_logbuff_init(void)
{
	pr_info("xhub_logbuff_init\n");
	if (logbuff_device_register())
		goto REGISTER_ERR;
	if (creat_log_device_file())
		goto SYSFS_ERR_1;
	logbuff_init_success();
	return 0;
SYSFS_ERR_1:
	platform_device_unregister(&xhub_logbuff);
REGISTER_ERR:
	return -1;
}

static void xhub_logbuff_exit(void)
{
	device_remove_file(&xhub_logbuff.dev, &dev_attr_logbuff_flush);
	platform_device_unregister(&xhub_logbuff);
}

late_initcall_sync(xhub_logbuff_init);
module_exit(xhub_logbuff_exit);

MODULE_LICENSE("GPL");
