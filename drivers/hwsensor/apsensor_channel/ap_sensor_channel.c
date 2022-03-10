/*
 * motion_channel.c
 *
 * code for ap motion channel
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
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <ap_sensor.h>
#include <ap_sensor_route.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include "securec.h"

static int finger_scp_data_update_ready = 0;
static int fingersense_enabled = 0;
#define MAX_FINGER_SENSE_DATA_CNT   128
#define MAX_SENSOR_NAME_LEN         64
#define REQ_FINGER_SENSE_DATA_CMD   2
struct mutex fing_data_lock;
static int ps_ultrasonic_feature_available = 0;
static int ps_laser_prox_available = 0;

static short finger_data[MAX_FINGER_SENSE_DATA_CNT + 1] = {0};
static char acc_sensor_name[MAX_SENSOR_NAME_LEN] = {0};
#define FINGER_SENSE_MAX_SIZE       (MAX_FINGER_SENSE_DATA_CNT * sizeof(short))

#define MAX_STR_SIZE  20
#define MAX_RECV_CMD_LEN  500

#define CHECK_NULL_ERR(data) \
	do { \
		if (!data) { \
			hwlog_err("err in %s\n", __func__); \
			return -EINVAL; \
		} \
	} while (0)

static int ap_sensor_channel_cmd(unsigned int cmd, unsigned long arg)
{
	return 0;
}

static ssize_t shb_read(struct file *file, char __user *buf, size_t count,
	loff_t *pos)
{
	if (!buf)
		return 0;
	return ap_sensor_route_read(buf, count);
}

static ssize_t shb_write(struct file *file, const char __user *data,
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

static long ap_sensor_channel_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
#if 0
	case MHB_IOCTL_MOTION_START:
	case MHB_IOCTL_MOTION_STOP:
	case MHB_IOCTL_MOTION_ATTR_START:
	case MHB_IOCTL_MOTION_ATTR_STOP:
		break;
#endif
	default:
		hwlog_err("%s unknown cmd : %d\n", __func__, cmd);
		return -ENOTTY;
	}
	return ap_sensor_channel_cmd(cmd, arg);
}

static int ap_sensor_channel_open(struct inode *inode, struct file *file)
{
	hwlog_info("%s ok!\n", __func__);
	return 0;
}

static int ap_sensor_channel_release(struct inode *inode, struct file *file)
{
	hwlog_info("%s ok!\n", __func__);
	return 0;
}

static const struct file_operations shb_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.read = shb_read,
	.write = shb_write,
	.unlocked_ioctl = ap_sensor_channel_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ap_sensor_channel_ioctl,
#endif
	.open = ap_sensor_channel_open,
	.release = ap_sensor_channel_release,
};

static struct miscdevice motionhub_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "apsensorhub",
	.fops = &shb_fops,
};

struct sensor_info_t {
	uint8_t sensor_type;
	char name[16];
};

// here define sensor name info for every phy sensor
static ssize_t sensor_show_ACC_info(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	CHECK_NULL_ERR(buf);
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%s\n", acc_sensor_name);
}

static ssize_t sensor_store_ACC_info(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	CHECK_NULL_ERR(buf);
	if (!buf || (size >= MAX_SENSOR_NAME_LEN))
		return -EINVAL;

	memcpy_s(acc_sensor_name, MAX_SENSOR_NAME_LEN, buf, size);
	hwlog_info("%s set acc name = %s\n", __func__, acc_sensor_name);
	return size;
}

static ssize_t sensor_show_MAG_info(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%s\n", "mag1");
}

static ssize_t sensor_show_GYRO_info(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%s\n", "gyro1");
}

static ssize_t sensor_show_ALS_info(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%s\n", "als1");
}

static ssize_t sensor_show_SAR_info(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%s\n", "sar1");
}

static ssize_t sensor_show_PS_info(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	CHECK_NULL_ERR(buf);

	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%s\n", "ps1");
}

static ssize_t store_fg_sense_enable(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned long val = 0;
	struct ap_sensor_manager_cmd_t fg_cmd;

	if (!buf)
		return -EINVAL;

	if (kstrtoul(buf, 10, &val)) {
		hwlog_info("%s finger enable val %lu invalid", __func__, val);
		return -EINVAL;
	}

	hwlog_info("%s: finger sense enable val %ld\n", __func__, val);
	if ((val != 0) && (val != 1)) {
		hwlog_info("%s finger sense set enable fail, invalid val\n",
			__func__);
		return size;
	}
	if (fingersense_enabled == val) {
		hwlog_info("%s finger sense already current state\n", __func__);
		return size;
	}
	fingersense_enabled = val;
	fg_cmd.sensor_type = SENSOR_TYPE_FINGER_SENSE;
	fg_cmd.action = fingersense_enabled;
	ap_sensor_route_write((char*)&fg_cmd, sizeof(struct ap_sensor_manager_cmd_t));
	return size;
}

static ssize_t show_fg_sensor_enable(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (!buf)
		return -EINVAL;

	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1,
		"%d\n", fingersense_enabled);
}

static ssize_t store_fg_req_data(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct ap_sensor_manager_cmd_t fg_cmd;

	if (!fingersense_enabled) {
		hwlog_info("%s finger sense not enable, dont req data\n", __func__);
		return size;
	}
	finger_scp_data_update_ready = 0;

	hwlog_info("%s: finger sense request data\n", __func__);
	fg_cmd.sensor_type = SENSOR_TYPE_FINGER_SENSE;
	fg_cmd.action = REQ_FINGER_SENSE_DATA_CMD;
	ap_sensor_route_write((char*)&fg_cmd, sizeof(struct ap_sensor_manager_cmd_t));

	return size;
}

static ssize_t store_finger_send_data(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	errno_t rt;
	if (!buf || (size != FINGER_SENSE_MAX_SIZE))
		return -EINVAL;

	mutex_lock(&fing_data_lock);
	rt = memcpy_s((char *)&finger_data[0], FINGER_SENSE_MAX_SIZE, buf, FINGER_SENSE_MAX_SIZE);
	if (rt != EOK)
		hwlog_err("memcpy fg data error\n");
	mutex_unlock(&fing_data_lock);

	finger_scp_data_update_ready = 1;
	return size;
}

static ssize_t show_fg_data_ready(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (!buf)
		return -EINVAL;

	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1,
		"%d\n", finger_scp_data_update_ready);
}

static ssize_t show_fg_latch_data(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	errno_t rt;

	if (!buf)
		return -EINVAL;
	mutex_lock(&fing_data_lock);
	rt = memcpy_s(buf, FINGER_SENSE_MAX_SIZE,
		(char *)&finger_data[0], FINGER_SENSE_MAX_SIZE);
	if (rt != EOK)
		hwlog_err("memcpy fg data error\n");
	mutex_unlock(&fing_data_lock);
	return FINGER_SENSE_MAX_SIZE;
}

static ssize_t show_laser_prox_available(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (!buf)
		return -EINVAL;

	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1,
		"%d\n", ps_laser_prox_available);
}

static ssize_t show_ps_ultrasonic_feature(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (!buf)
		return -EINVAL;

	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1,
		"%d\n", ps_ultrasonic_feature_available);
}

static void hw_get_sensor_info_from_dts(void)
{
	int ret;
	int temp = 0;
	struct device_node *number_node = NULL;

	number_node = of_find_compatible_node(NULL, NULL, "huawei,sensor_info");
	if (number_node == NULL) {
		pr_err("Cannot find acc mode acc_horizontal from dts\n");
		return;
	}

	ret = of_property_read_u32(number_node, "ps_ultrasonic_en", &temp);
	if (!ret) {
		pr_info("%s, ps_ultrasonic is :%d\n", __func__, temp);
		ps_ultrasonic_feature_available = temp;
	} else {
		ps_ultrasonic_feature_available = 0;
		pr_err("%s, Cannot find ps_ultrasonic\n", __func__);
	}

	ret = of_property_read_u32(number_node, "ps_laser_prox_available", &temp);
	if (!ret) {
		pr_info("%s, laser_prox_available is :%d\n", __func__, temp);
		ps_laser_prox_available = temp;
	} else {
		ps_laser_prox_available = 0;
		pr_err("%s, Cannot find laser_prox_available\n", __func__);
	}
}

static DEVICE_ATTR(acc_info, 0660, sensor_show_ACC_info, sensor_store_ACC_info);
static DEVICE_ATTR(mag_info, 0444, sensor_show_MAG_info, NULL);
static DEVICE_ATTR(gyro_info, 0444, sensor_show_GYRO_info, NULL);
static DEVICE_ATTR(als_info, 0444, sensor_show_ALS_info, NULL);
static DEVICE_ATTR(sar_info, 0444, sensor_show_SAR_info, NULL);
static DEVICE_ATTR(ps_info, 0444, sensor_show_PS_info, NULL);
static DEVICE_ATTR(fingersense_send_data, 0220, NULL, store_finger_send_data);
static DEVICE_ATTR(set_fingersense_enable, 0660, show_fg_sensor_enable,
	store_fg_sense_enable);
static DEVICE_ATTR(fingersense_req_data, 0220, NULL, store_fg_req_data);
static DEVICE_ATTR(fingersense_data_ready, 0440, show_fg_data_ready, NULL);
static DEVICE_ATTR(fingersense_latch_data, 0660, show_fg_latch_data, NULL);
static DEVICE_ATTR(ps_ultrasonic_feature, 0660, show_ps_ultrasonic_feature, NULL);
static DEVICE_ATTR(laser_prox_available, 0660, show_laser_prox_available, NULL);

static struct attribute *sensor_attributes[] = {
	&dev_attr_acc_info.attr,
	&dev_attr_mag_info.attr,
	&dev_attr_gyro_info.attr,
	&dev_attr_als_info.attr,
	&dev_attr_sar_info.attr,
	&dev_attr_ps_info.attr,
	&dev_attr_fingersense_send_data.attr,
	&dev_attr_set_fingersense_enable.attr,
	&dev_attr_fingersense_req_data.attr,
	&dev_attr_fingersense_data_ready.attr,
	&dev_attr_fingersense_latch_data.attr,
	&dev_attr_ps_ultrasonic_feature.attr,
	&dev_attr_laser_prox_available.attr,
	NULL
};

static const struct attribute_group sensor_node = {
	.attrs = sensor_attributes,
};

static struct platform_device sensor_input_info = {
	.name = "huawei_sensor",
	.id = -1,
};

static int __init ap_sensorhub_init(void)
{
	int ret;

	ret = misc_register(&motionhub_miscdev);
	if (ret != 0) {
		hwlog_err("cannot register miscdev err=%d\n", ret);
		goto exit1;
	}
	hw_get_sensor_info_from_dts();
	ret = ap_sensor_route_init();
	if (ret != 0) {
		hwlog_err("cannot motion_route_init  err=%d\n", ret);
		goto exit2;
	}

	ret = platform_device_register(&sensor_input_info);
	if (ret) {
		pr_err("%s: register failed, ret:%d\n", __func__, ret);
		goto exit1;
	}

	ret = sysfs_create_group(&sensor_input_info.dev.kobj, &sensor_node);
	if (ret) {
		pr_err("sysfs_create_group error ret =%d\n", ret);
		goto exit1;
	}

	// defalt lsm6ds3
	memcpy_s(acc_sensor_name, MAX_SENSOR_NAME_LEN, "acc_s001_002",
		strlen("acc_s001_002") + 1);
	mutex_init(&fing_data_lock);
	return ret;
exit1:
	return -1;
exit2:
	misc_deregister(&motionhub_miscdev);
	return -1;

}

static void __exit ap_sensorhub_exit(void)
{
	misc_deregister(&motionhub_miscdev);
	ap_sensor_route_destroy();
	hwlog_info("exit %s\n", __func__);
}

late_initcall_sync(ap_sensorhub_init);
module_exit(ap_sensorhub_exit);

MODULE_DESCRIPTION("apsensorhub driver");
MODULE_LICENSE("GPL");
