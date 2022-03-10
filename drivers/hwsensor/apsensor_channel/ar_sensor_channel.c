/*
 * ar_sensor_channel.c
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

#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <ap_sensor.h>
#include <ar_sensor_route.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include "securec.h"

static int ar_sensor_channel_cmd(unsigned int cmd, unsigned long arg)
{
	return 0;
}

static ssize_t ar_sensor_read(struct file *file, char __user *buf, size_t count,
	loff_t *pos)
{
	if (!buf)
		return 0;
	return ar_sensor_route_read(buf, count);
}

static ssize_t ar_sensor_write(struct file *file, const char __user *data,
	size_t len, loff_t *ppos)
{
	char recv_data[256] = {0};

	hwlog_err("write data len = %u\n", len);
	if (!data || (len > 256))
		return -EINVAL;
// here define recv motion data
	if (copy_from_user(recv_data, data, len)) {
		hwlog_err("write data error\n");
		return -EFAULT;
	}
	return len;
}

static long ar_sensor_channel_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	default:
		hwlog_err("%s unknown cmd : %d\n", __func__, cmd);
		return -ENOTTY;
	}
	return ar_sensor_channel_cmd(cmd, arg);
}

static int ar_sensor_channel_open(struct inode *inode, struct file *file)
{
	hwlog_info("%s ok!\n", __func__);
	return 0;
}

static int ar_sensor_channel_release(struct inode *inode, struct file *file)
{
	hwlog_info("%s ok!\n", __func__);
	return 0;
}

static const struct file_operations shb_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.read = ar_sensor_read,
	.write = ar_sensor_write,
	.unlocked_ioctl = ar_sensor_channel_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ar_sensor_channel_ioctl,
#endif
	.open = ar_sensor_channel_open,
	.release = ar_sensor_channel_release,
};

static struct miscdevice arhub_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "arsensorhub",
	.fops = &shb_fops,
};

static int __init ar_sensorhub_init(void)
{
	int ret;

	ret = misc_register(&arhub_miscdev);
	if (ret != 0) {
		hwlog_err("cannot register ar miscdev err=%d\n", ret);
		goto exit1;
	}

	ret = ar_sensor_route_init();
	if (ret != 0) {
		hwlog_err("cannot ar_route_init  err=%d\n", ret);
		goto exit2;
	}

	return ret;
exit1:
	return -1;
exit2:
	misc_deregister(&arhub_miscdev);
	return -1;

}

static void __exit ar_sensorhub_exit(void)
{
	misc_deregister(&arhub_miscdev);
	ar_sensor_route_destroy();
	hwlog_info("exit %s\n", __func__);
}

late_initcall_sync(ar_sensorhub_init);
module_exit(ar_sensorhub_exit);

MODULE_DESCRIPTION("arsensorhub driver");
MODULE_LICENSE("GPL");
