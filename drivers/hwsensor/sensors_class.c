/* Copyright (c) 2013-2014, 2017 The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/rwsem.h>
#include <linux/string.h>
#include <securec.h>
#include <apsensor_channel/ap_sensor_route.h>
#include <apsensor_channel/ap_sensor.h>
#include "sensors_class.h"
#include "xhub_router/xhub_pm.h"
#include "sensors_sysfs_als.h"
#include "sensors_sysfs_ps.h"
#include "sensors_sysfs_hinge.h"
#include <misc/app_info.h>

#define MAX_STR_SIZE 1024
struct class *sensors_class;

#define APPLY_MASK    0x00000001

#define CMD_W_L_MASK 0x00
#define CMD_W_H_MASK 0x10
#define CMD_W_H_L    0x10
#define CMD_MASK    0xF
#define DATA_MASK    0xFFFF0000
#define DATA_AXIS_SHIFT    17
#define DATA_APPLY_SHIFT    16
static struct sensor_cookie all_sensors[];
static int als_after_sale_buf[6];

#define ACC_DEVICE_ID_0 0
#define ACC_DEVICE_ID_1 1
#define ACC_SENSOR_DEFAULT_ID "1"
#define ENABLE_DROP_DEFAULT   "0"

static char *enable_drop = ENABLE_DROP_DEFAULT;
static char *acc_sensor_id = ACC_SENSOR_DEFAULT_ID;

/*
 * CMD_GET_PARAMS(BIT, PARA, DATA) combine high 16 bit and low 16 bit
 * as one params
 */

#define CMD_GET_PARAMS(BIT, PARA, DATA)	\
	((BIT) ?	\
		((DATA) & DATA_MASK)	\
		: ((PARA) \
		| (((DATA) & DATA_MASK) >> 16)))

extern ssize_t als_under_tp_calidata_store(struct device *dev,
                 struct device_attribute *attr, const char *buf, size_t count);
extern ssize_t als_under_tp_calidata_show(struct device *dev,
                 struct device_attribute *attr, char *buf);
extern ssize_t store_send_receiver_req(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t size);
extern ssize_t show_send_receiver_req(struct device *dev,
    struct device_attribute *attr, char *buf);
extern ssize_t store_send_lcdfreq_req(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t size);
extern ssize_t show_send_lcdfreq_req(struct device *dev,
    struct device_attribute *attr, char *buf);
extern ssize_t attr_als_rgb_enable_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count);
extern ssize_t attr_als_rgb_debug_enable_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count);
/*
 * CMD_DO_CAL sensor do calibrate command, when do sensor calibrate must use
 * this.
 * AXIS_X,AXIS_Y,AXIS_Z write axis params to driver like accelerometer
 * magnetometer,gyroscope etc.
 * CMD_W_THRESHOLD_H,CMD_W_THRESHOLD_L,CMD_W_BIAS write theshold and bias
 * params to proximity driver.
 * CMD_W_FACTOR,CMD_W_OFFSET write factor and offset params to light
 * sensor driver.
 * CMD_COMPLETE when one sensor receive calibrate parameters complete, it
 * must use this command to end receive the parameters and send the
 * parameters to sensor.
 */

enum {
	CMD_DO_CAL = 0x0,
	CMD_W_OFFSET_X,
	CMD_W_OFFSET_Y,
	CMD_W_OFFSET_Z,
	CMD_W_THRESHOLD_H,
	CMD_W_THRESHOLD_L,
	CMD_W_BIAS,
	CMD_W_OFFSET,
	CMD_W_FACTOR,
	CMD_W_RANGE,
	CMD_COMPLETE,
	CMD_COUNT
};

int cal_map[] = {
	0,
	offsetof(struct cal_result_t, offset_x),
	offsetof(struct cal_result_t, offset_y),
	offsetof(struct cal_result_t, offset_z),
	offsetof(struct cal_result_t, threshold_h),
	offsetof(struct cal_result_t, threshold_l),
	offsetof(struct cal_result_t, bias),
	offsetof(struct cal_result_t, offset[0]),
	offsetof(struct cal_result_t, offset[1]),
	offsetof(struct cal_result_t, offset[2]),
	offsetof(struct cal_result_t, factor),
	offsetof(struct cal_result_t, range),
};

DECLARE_RWSEM(sensors_list_lock);
LIST_HEAD(sensors_list);

static ssize_t sensors_name_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->name);
}

static ssize_t sensors_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->vendor);
}

static ssize_t sensors_version_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->version);
}

static ssize_t sensors_handle_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->handle);
}

static ssize_t sensors_type_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->type);
}

static ssize_t sensors_max_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->max_delay);
}

static ssize_t sensors_flags_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->flags);
}

static ssize_t sensors_max_range_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->max_range);
}

static ssize_t sensors_resolution_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->resolution);
}

static ssize_t sensors_power_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->sensor_power);
}

static ssize_t sensors_min_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->min_delay);
}

static ssize_t sensors_fifo_event_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n",
			sensors_cdev->fifo_reserved_event_count);
}

static ssize_t sensors_fifo_max_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n",
			sensors_cdev->fifo_max_event_count);
}

static ssize_t sensors_enable_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned long data = 0;

	ret = kstrtoul(buf, 10, &data);
	if (ret)
		return ret;
	if (data > 1) {
		dev_err(dev, "Invalid value of input, input=%ld\n", data);
		return -EINVAL;
	}

	if (sensors_cdev->sensors_enable == NULL) {
		dev_err(dev, "Invalid sensor class enable handle\n");
		return -EINVAL;
	}
	ret = sensors_cdev->sensors_enable(sensors_cdev, data);
	if (ret)
		return ret;

	sensors_cdev->enabled = data;
	return size;
}


static ssize_t sensors_enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n",
			sensors_cdev->enabled);
}

static ssize_t sensors_delay_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned long data = 0;

	ret = kstrtoul(buf, 10, &data);
	if (ret)
		return ret;
	/* The data unit is millisecond, the min_delay unit is microseconds. */
	if ((data * 1000) < sensors_cdev->min_delay) {
		dev_err(dev, "Invalid value of delay, delay=%ld\n", data);
		return -EINVAL;
	}
	if (sensors_cdev->sensors_poll_delay == NULL) {
		dev_err(dev, "Invalid sensor class delay handle\n");
		return size;
	}
	ret = sensors_cdev->sensors_poll_delay(sensors_cdev, data);
	if (ret)
		return ret;

	sensors_cdev->delay_msec = data;
	return size;
}

static ssize_t sensors_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n",
			sensors_cdev->delay_msec);
}

static ssize_t sensors_test_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	int ret;

	if (sensors_cdev->sensors_self_test == NULL) {
		dev_err(dev, "Invalid sensor class self test handle\n");
		return -EINVAL;
	}

	ret = sensors_cdev->sensors_self_test(sensors_cdev);
	if (ret)
		dev_warn(dev, "self test failed.(%d)\n", ret);

	return snprintf(buf, PAGE_SIZE, "%s\n",
			ret ? "fail" : "pass");
}

static ssize_t sensors_max_latency_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	unsigned long latency;
	int ret = -EINVAL;

	ret = kstrtoul(buf, 10, &latency);
	if (ret)
		return ret;

	if (latency > sensors_cdev->max_delay) {
		dev_err(dev, "max_latency(%lu) is greater than max_delay(%u)\n",
				latency, sensors_cdev->max_delay);
		return -EINVAL;
	}

	if (sensors_cdev->sensors_set_latency == NULL) {
		dev_err(dev, "Invalid sensor calss set latency handle\n");
		return -EINVAL;
	}

	/* Disable batching for this sensor */
	if ((latency < sensors_cdev->delay_msec) && (latency != 0)) {
		dev_err(dev, "max_latency is less than delay_msec\n");
		return -EINVAL;
	}

	ret = sensors_cdev->sensors_set_latency(sensors_cdev, latency);
	if (ret)
		return ret;

	sensors_cdev->max_latency = latency;

	return size;
}

static ssize_t sensors_max_latency_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE,
		"%u\n", sensors_cdev->max_latency);
}

static ssize_t sensors_flush_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned long data = 0;

	ret = kstrtoul(buf, 10, &data);
	if (ret)
		return ret;
	if (data != 1) {
		dev_err(dev, "Flush: Invalid value of input, input=%ld\n",
				data);
		return -EINVAL;
	}

	if (sensors_cdev->sensors_flush == NULL) {
		dev_err(dev, "Invalid sensor class flush handle\n");
		return -EINVAL;
	}
	ret = sensors_cdev->sensors_flush(sensors_cdev);
	if (ret)
		return ret;

	return size;
}

static ssize_t sensors_flush_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE,
		"Flush handler %s\n",
			(sensors_cdev->sensors_flush == NULL)
				? "not exist" : "exist");
}

static ssize_t sensors_enable_wakeup_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret;
	unsigned long enable;

	if (sensors_cdev->sensors_enable_wakeup == NULL) {
		dev_err(dev, "Invalid sensor class enable_wakeup handle\n");
		return -EINVAL;
	}

	ret = kstrtoul(buf, 10, &enable);
	if (ret)
		return ret;

	enable = enable ? 1 : 0;
	ret = sensors_cdev->sensors_enable_wakeup(sensors_cdev, enable);
	if (ret)
		return ret;

	sensors_cdev->wakeup = enable;

	return size;
}

static ssize_t sensors_enable_wakeup_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->wakeup);
}


static ssize_t sensors_calibrate_show(struct device *dev,
		struct device_attribute *atte, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	if (sensors_cdev->params == NULL) {
		dev_err(dev, "Invalid sensor params\n");
		return -EINVAL;
	}
	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->params);
}

static ssize_t sensors_calibrate_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	long data;
	int axis, apply_now;
	int cmd, bit_h;

	ret = kstrtol(buf, 0, &data);
	if (ret)
		return ret;
	dev_dbg(dev, "data = %lx\n", data);
	cmd = data & CMD_MASK;
	if (cmd == CMD_DO_CAL) {
		if (sensors_cdev->sensors_calibrate == NULL) {
			dev_err(dev, "Invalid calibrate handle\n");
			return -EINVAL;
		}
		/* parse the data to get the axis and apply_now value*/
		apply_now = (int)(data >> DATA_APPLY_SHIFT) & APPLY_MASK;
		axis = (int)data >> DATA_AXIS_SHIFT;
		dev_dbg(dev, "apply_now = %d, axis = %d\n", apply_now, axis);
		ret = sensors_cdev->sensors_calibrate(sensors_cdev,
				axis, apply_now);
		if (ret)
			return ret;
	} else {
		if (sensors_cdev->sensors_write_cal_params == NULL) {
			dev_err(dev,
					"Invalid write_cal_params handle\n");
			return -EINVAL;
		}
		bit_h = (data & CMD_W_H_L) >> 4;
		if (cmd > CMD_DO_CAL && cmd < CMD_COMPLETE) {
			char *p = (char *)(&sensors_cdev->cal_result)
					+ cal_map[cmd];
			*(int *)p = CMD_GET_PARAMS(bit_h, *(int *)p, data);
		} else if (cmd == CMD_COMPLETE) {
			ret = sensors_cdev->sensors_write_cal_params
				(sensors_cdev, &sensors_cdev->cal_result);
		} else {
			dev_err(dev, "Invalid command\n");
			return -EINVAL;
		}
	}
	return size;
}

static DEVICE_ATTR(name, 0664, sensors_name_show, NULL);
static DEVICE_ATTR(vendor, 0444, sensors_vendor_show, NULL);
static DEVICE_ATTR(version, 0444, sensors_version_show, NULL);
static DEVICE_ATTR(handle, 0444, sensors_handle_show, NULL);
static DEVICE_ATTR(type, 0444, sensors_type_show, NULL);
static DEVICE_ATTR(max_range, 0444, sensors_max_range_show, NULL);
static DEVICE_ATTR(resolution, 0444, sensors_resolution_show, NULL);
static DEVICE_ATTR(sensor_power, 0444, sensors_power_show, NULL);
static DEVICE_ATTR(min_delay, 0444, sensors_min_delay_show, NULL);
static DEVICE_ATTR(fifo_reserved_event_count, 0444, sensors_fifo_event_show,
	NULL);
static DEVICE_ATTR(fifo_max_event_count, 0444, sensors_fifo_max_show, NULL);
static DEVICE_ATTR(max_delay, 0444, sensors_max_delay_show, NULL);
static DEVICE_ATTR(flags, 0444, sensors_flags_show, NULL);
static DEVICE_ATTR(enable, 0664, sensors_enable_show, sensors_enable_store);
static DEVICE_ATTR(enable_wakeup, 0664, sensors_enable_wakeup_show,
	sensors_enable_wakeup_store);
static DEVICE_ATTR(poll_delay, 0664, sensors_delay_show, sensors_delay_store);
static DEVICE_ATTR(self_test, 0440, sensors_test_show, NULL);
static DEVICE_ATTR(max_latency, 0660, sensors_max_latency_show,
	sensors_max_latency_store);
static DEVICE_ATTR(flush, 0660, sensors_flush_show, sensors_flush_store);
static DEVICE_ATTR(calibrate, 0664, sensors_calibrate_show,
	sensors_calibrate_store);

static struct attribute *sensors_class_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_vendor.attr,
	&dev_attr_version.attr,
	&dev_attr_handle.attr,
	&dev_attr_type.attr,
	&dev_attr_max_range.attr,
	&dev_attr_resolution.attr,
	&dev_attr_sensor_power.attr,
	&dev_attr_min_delay.attr,
	&dev_attr_fifo_reserved_event_count.attr,
	&dev_attr_fifo_max_event_count.attr,
	&dev_attr_max_delay.attr,
	&dev_attr_flags.attr,
	&dev_attr_enable.attr,
	&dev_attr_enable_wakeup.attr,
	&dev_attr_poll_delay.attr,
	&dev_attr_self_test.attr,
	&dev_attr_max_latency.attr,
	&dev_attr_flush.attr,
	&dev_attr_calibrate.attr,
	NULL,
};
ATTRIBUTE_GROUPS(sensors_class);

/**
 * sensors_classdev_register - register a new object of sensors_classdev class.
 * @parent: The device to register.
 * @sensors_cdev: the sensors_classdev structure for this device.
*/
int sensors_classdev_register(struct device *parent,
				struct sensors_classdev *sensors_cdev)
{
	sensors_cdev->dev = device_create(sensors_class, parent, 0,
				      sensors_cdev, "%s", sensors_cdev->name);
	if (IS_ERR(sensors_cdev->dev))
		return PTR_ERR(sensors_cdev->dev);

	down_write(&sensors_list_lock);
	list_add_tail(&sensors_cdev->node, &sensors_list);
	up_write(&sensors_list_lock);

	pr_debug("Registered sensors device: %s\n",
			sensors_cdev->name);
	return 0;
}
EXPORT_SYMBOL(sensors_classdev_register);

/**
 * sensors_classdev_unregister - unregister a object of sensors class.
 * @sensors_cdev: the sensor device to unregister
 * Unregister a previously registered via sensors_classdev_register object.
*/
void sensors_classdev_unregister(struct sensors_classdev *sensors_cdev)
{
	device_unregister(sensors_cdev->dev);
	down_write(&sensors_list_lock);
	list_del(&sensors_cdev->node);
	up_write(&sensors_list_lock);
}
EXPORT_SYMBOL(sensors_classdev_unregister);

static char rgb_data[MAX_STR_SIZE] = {'\0'};

static ssize_t attr_als_rgb_data_under_tp_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%s\n", rgb_data);
}

static ssize_t attr_als_rgb_data_under_tp_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    if (buf == NULL) {
        pr_err("%s:buf is invalid",__func__);
    }
    memcpy_s(rgb_data, sizeof(rgb_data), buf, count);
    pr_info("%s:%s",__func__, rgb_data);
    return count;
}

static ssize_t attr_als_calibrate_under_tp_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%s\n", rgb_data);
}


static void get_als_calibrate_after_sale_info_from_dts(void)
{
	struct device_node *als_after_sale_node = NULL;
    int index;

    char als_sensor_name[APP_INFO_VALUE_LENTH] = {0};
    int ret = app_info_proc_get("L-Sensor", als_sensor_name);
    if (ret) {
        hwlog_err("%s, read L-Sensor type failed : %d!\n", __func__, ret);
    } else {
        hwlog_info("%s, read L-Sensor type : %s\n", __func__, als_sensor_name);
        index = 0;
        while(als_sensor_name[index] != ' ' && als_sensor_name[index] != '\0') {
            index++;
        }
        als_sensor_name[index] = '\0';
    }

	als_after_sale_node = of_find_compatible_node(NULL, NULL, als_sensor_name);
	hwlog_info("calibrate_after_sale find name 1\n");
	if (als_after_sale_node == NULL) {
		hwlog_err("find ar node failed\n");
	}

	if (of_property_read_s32(als_after_sale_node, "als_after_sale_product", &als_after_sale_buf[0] ) != 0) {
		hwlog_err("%s, read support num failed!\n", __func__);
	}
	if (of_property_read_s32(als_after_sale_node, "als_after_sale_xl", &als_after_sale_buf[1] ) != 0) {
		hwlog_err("%s, read support num failed!\n", __func__);
	}
	if (of_property_read_s32(als_after_sale_node, "als_after_sale_yl", &als_after_sale_buf[2] ) != 0) {
		hwlog_err("%s, read support num failed!\n", __func__);
	}
	if (of_property_read_s32(als_after_sale_node, "als_after_sale_xr", &als_after_sale_buf[3] ) != 0) {
		hwlog_err("%s, read support num failed!\n", __func__);
	}
	if (of_property_read_s32(als_after_sale_node, "als_after_sale_yr", &als_after_sale_buf[4] ) != 0) {
		hwlog_err("%s, read support num failed!\n", __func__);
	}
	if (of_property_read_s32(als_after_sale_node, "als_after_sale_gamma", &als_after_sale_buf[5] ) != 0) {
		hwlog_err("%s, read support num failed!\n", __func__);
	}

}


static ssize_t attr_als_calibrate_after_sale_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
	get_als_calibrate_after_sale_info_from_dts();
    return snprintf_s(buf, 30, 30 - 1, "%d, %d, %d, %d, %d, %d \n", als_after_sale_buf[0],als_after_sale_buf[1],als_after_sale_buf[2],als_after_sale_buf[3],als_after_sale_buf[4],als_after_sale_buf[5]);
}


static DEVICE_ATTR(als_calibrate_under_tp, 0440,
    attr_als_calibrate_under_tp_show, NULL);

static DEVICE_ATTR(als_rgb_data_under_tp, 0660,
    attr_als_rgb_data_under_tp_show, attr_als_rgb_data_under_tp_store);

static ssize_t attr_als_under_tp_calidata_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return als_under_tp_calidata_store(dev, attr, buf, count);
}

 /* return underscreen als to node file */
static ssize_t attr_als_under_tp_calidata_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    return als_under_tp_calidata_show(dev, attr, buf);
}
static DEVICE_ATTR(set_als_under_tp_calidata, 0660,
    attr_als_under_tp_calidata_show, attr_als_under_tp_calidata_store);

/* files create for ps sensor */
static ssize_t attr_ps_send_recever_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return store_send_receiver_req(dev, attr, buf, count);
}

static ssize_t attr_ps_send_recever_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    return show_send_receiver_req(dev, attr, buf);
}

static ssize_t attr_ps_send_lcdfreq_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    return store_send_lcdfreq_req(dev, attr, buf, count);
}

static ssize_t attr_ps_send_lcdfreq_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    return show_send_lcdfreq_req(dev, attr, buf);
}


static DEVICE_ATTR(send_receiver_req, 0660, attr_ps_send_recever_show, attr_ps_send_recever_store);
static DEVICE_ATTR(send_lcdfreq_req, 0660, attr_ps_send_lcdfreq_show, attr_ps_send_lcdfreq_store);

/* files create for every sensor */
static struct attribute *proximity_attributes[] = {
    &dev_attr_send_receiver_req.attr,
    &dev_attr_send_lcdfreq_req.attr,
    NULL,
};
static const struct attribute_group proximity_attr_group = {
    .attrs = proximity_attributes,
};

static DEVICE_ATTR(als_calibrate_after_sale, 0440,
    attr_als_calibrate_after_sale_show, NULL);
static DEVICE_ATTR(als_rgb_enable, 0220,
    NULL, attr_als_rgb_enable_store);
static DEVICE_ATTR(als_rgb_debug_enable, 0220,
    NULL, attr_als_rgb_debug_enable_store);

static struct attribute *als_sensor_attrs[] = {
    &dev_attr_als_rgb_data_under_tp.attr,
    &dev_attr_set_als_under_tp_calidata.attr,
    &dev_attr_als_calibrate_under_tp.attr,
    &dev_attr_als_calibrate_after_sale.attr,
    &dev_attr_als_rgb_enable.attr,
    &dev_attr_als_rgb_debug_enable.attr,
    NULL,
};

static const struct attribute_group als_sensor_attrs_grp = {
    .attrs = als_sensor_attrs,
};

void report_fold_status_when_poweroff_charging(int status)
{
	int i = 2;         // hinge_sensor
	static int pre_status = -1;
	char *fold_on[] = {"FOLDSTAUS=0", NULL};
	char *fold_off[] = {"FOLDSTAUS=1", NULL};

	if (status == pre_status)
		return;
	pre_status = status;
	if (status == 0)
		kobject_uevent_env(&all_sensors[i].dev->kobj, KOBJ_CHANGE,
			fold_on);
	else
		kobject_uevent_env(&all_sensors[i].dev->kobj, KOBJ_CHANGE,
			fold_off);
}

static ssize_t show_hinge_status_info(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (!buf)
		return 0;
	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%d\n",
		get_fold_hinge_status());
}

static DEVICE_ATTR(hinge_status_info, 0664, show_hinge_status_info, NULL);
static struct attribute *hinge_sensor_attrs[] = {
	&dev_attr_hinge_status_info.attr,
	NULL,
};

static const struct attribute_group hinge_sensor_attrs_grp = {
	.attrs = hinge_sensor_attrs,
};

/* acc sensor id begin */
static ssize_t attr_acc_get_sensor_id_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	memcpy_s(acc_sensor_id, count, buf, count);
	return count;
}

static ssize_t attr_acc_get_sensor_id_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1,
		"%s", acc_sensor_id);
}

static DEVICE_ATTR(get_sensor_id, 0644,
	attr_acc_get_sensor_id_show, attr_acc_get_sensor_id_store);

static struct attribute *acc_sensor_attrs[] = {
	&dev_attr_get_sensor_id.attr,
	NULL,
};

static const struct attribute_group acc_sensor_attrs_grp = {
	.attrs = acc_sensor_attrs,
};

void acc_get_sensors_id_from_dts(struct device_node *dn)
{
	if (dn == NULL)
		return;
	if (of_property_read_string(dn,
		"acc_sensor_id", (const char **)&acc_sensor_id)) {
		acc_sensor_id = ACC_SENSOR_DEFAULT_ID;
		pr_err("cannot find acc from dts, so only have one id\n");
	}
	pr_info("acc_sensor_id: %s\n", acc_sensor_id);
}

static int get_sensorhub_info_from_dts(void)
{
	struct device_node *sensorhub_node = NULL;

	sensorhub_node = of_find_compatible_node(NULL, NULL,
						"huawei,sensorhub");
	if (!sensorhub_node) {
		pr_err("%s, can't find node sensorhub\n", __func__);
		return -1;
	}

	acc_get_sensors_id_from_dts(sensorhub_node);

	return 0;
}

/* acc sensor_id end */

/* enable drop begin */
static ssize_t attr_enable_drop_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	memcpy_s(enable_drop, count, buf, count);
	return count;
}
static ssize_t attr_enable_drop_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1,
		"%s", enable_drop);
}
static DEVICE_ATTR(get_enable_drop_status, 0644,
	attr_enable_drop_show, attr_enable_drop_store);

static struct attribute *enable_drop_attrs[] = {
	&dev_attr_get_enable_drop_status.attr,
	NULL,
};

static const struct attribute_group drop_sensor_attrs_grp = {
	.attrs = enable_drop_attrs,
};

void get_enable_drop_status_from_dts(struct device_node *dn)
{
	if (dn == NULL)
		return;
	if (of_property_read_string(dn,
		"enable_drop", (const char **)&enable_drop)) {
		enable_drop = ENABLE_DROP_DEFAULT;
		pr_err("cannot find enable_drop from dts, so only have one id\n");
	}
	pr_info("enable_drop: %s\n", enable_drop);
}

static int get_enable_drop_info_from_dts(void)
{
	struct device_node *sensorhub_node = NULL;

	sensorhub_node = of_find_compatible_node(NULL, NULL,
						"huawei,sensorhub");
	if (!sensorhub_node) {
		pr_err("%s, can't find node sensorhub\n", __func__);
		return -1;
	}

	get_enable_drop_status_from_dts(sensorhub_node);

	return 0;
}

/* enable drop end */


// if add sensor type to all_sensors, Must add to last
static struct sensor_cookie all_sensors[] = {
	{
		.name = "ps_sensor",
		.attrs_group = &proximity_attr_group,
	},
	{
		.name = "als_sensor",
		.attrs_group = &als_sensor_attrs_grp,
	},
	{
		.name = "hinge_sensor",
		.attrs_group = &hinge_sensor_attrs_grp,
	},
	{
		.name = "acc_sensor",
		.attrs_group = &acc_sensor_attrs_grp,
	},
	{
		.name = "drop_sensor",
		.attrs_group = &drop_sensor_attrs_grp,
	}
};

static int sensors_register(void)
{
    int i;

    get_sensorhub_info_from_dts();
    get_enable_drop_info_from_dts();

    for (i = 0; i < sizeof(all_sensors) / sizeof(all_sensors[0]); ++i) {
        all_sensors[i].dev = device_create(sensors_class, NULL, 0,
            &all_sensors[i], all_sensors[i].name);
        if (!all_sensors[i].dev) {
            pr_err("[%s] Failed", __func__);
            return -1;
        } else {
            if (all_sensors[i].attrs_group &&
                sysfs_create_group(&all_sensors[i].dev->kobj,
                    all_sensors[i].attrs_group))
            pr_err("create files failed in %s\n", __func__);
        }
    }
    return 0;
}

static int __init sensors_init(void)
{
    sensors_class = class_create(THIS_MODULE, "sensors");
    if (IS_ERR(sensors_class))
        return PTR_ERR(sensors_class);
    sensors_class->dev_groups = sensors_class_groups;
    sensors_register();
    return 0;
}

static void __exit sensors_exit(void)
{
	class_destroy(sensors_class);
}

subsys_initcall(sensors_init);
module_exit(sensors_exit);
