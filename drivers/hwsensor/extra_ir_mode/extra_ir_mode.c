/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: some functions of extra ir mode
 * Author: huangxinlong
 * Create: 2021-1-14
 * Notes: extra ir
 */
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/rwsem.h>
#include <securec.h>
#include "../sensors_class.h"
#include "sensor.h"

#define ENABLE              1
#define DISABLE             0
#define FAR_MODE            1
#define NEAR_MODE           2
#define DEFAULT_BUF         10

static int ps_switch_mode;

struct extra_ir ps_sensor = {
	.ext_ir_mode_dts = 0,
	.gpio_value_dts = 0,
};

static struct sensors_classdev sensors_proximity_cdev = {
	.name = "ps_sensor",
	.vendor = "huawei",
	.version = 1,
	.handle = SENSORS_PROXIMITY_HANDLE,
	.type = SENSOR_TYPE_PROXIMITY,
	.max_range = "9.0",
	.resolution = "1.0",
	.sensor_power = "3",
	/* in microseconds */
	.min_delay = 0,
	.enabled = 0,
	.delay_msec = DEFAULT_DELAY,
	.sensors_enable = NULL,
	.sensors_poll_delay = NULL,
	.flags = DEFAULT_FLAG,
};

struct sensor_cookie {
	const char *name;
	const struct attribute_group *attrs_group;
	struct device *dev;
	struct sensors_classdev sensor_cdev;
	struct input_dev *input_dev_ps;
};

static struct sensor_cookie *ps_pdata;

static ssize_t ps_switch_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int val = ps_switch_mode;

	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%d\n", val);
}

static ssize_t ps_switch_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t ret;
	unsigned long data = 0;

	ret = kstrtoul(buf, DEFAULT_BUF, &data);
	if (ret)
		return ret;
	ps_switch_mode = data;
	if ((ps_switch_mode == FAR_MODE || ps_switch_mode == NEAR_MODE) &&
		ps_sensor.gpio_value_dts) {
		dev_err(dev, "open ir input=%d\n", ps_switch_mode);
		gpio_direction_output(ps_sensor.gpio_value_dts, ENABLE);
		gpio_set_value(ps_sensor.gpio_value_dts, ENABLE);
		dev_err(dev, "get gpio ir input=%d\n", gpio_get_value(ps_sensor.gpio_value_dts));
	} else if (ps_switch_mode == DISABLE) {
		dev_err(dev, "close ir input=%d\n", ps_switch_mode);
		gpio_direction_output(ps_sensor.gpio_value_dts, DISABLE);
		gpio_set_value(ps_sensor.gpio_value_dts, DISABLE);
		dev_err(dev, "close gpio ir input=%d\n", gpio_get_value(ps_sensor.gpio_value_dts));
	}
	return size;
}

static DEVICE_ATTR(ps_switch_mode, 0664, ps_switch_mode_show, ps_switch_enable_store);

static struct attribute *ps_sensor_attrs[] = {
	&dev_attr_ps_switch_mode.attr,
	NULL,
};

static const struct attribute_group ps_sensor_attrs_grp = {
	.attrs = ps_sensor_attrs,
};

static void hw_get_ps_para_from_dts(void)
{
	int ret, temp;
	struct device_node *number_node = NULL;
	number_node = of_find_compatible_node(NULL, NULL, "huawei,extra_ir");
	if (number_node == NULL) {
		pr_err("Cannot find ext_ir_mode from dts\n");
		return;
	}

	ret = of_property_read_u32(number_node, "ext_ir_mode", &temp);
	if (!ret) {
		pr_info("%s, ext_ir_mode from dts:%d\n", __func__, temp);
		ps_sensor.ext_ir_mode_dts = temp;
	} else {
		pr_err("%s, Cannot find ext_ir_mode from dts\n", __func__);
	}
	ret = of_property_read_u32(number_node, "gpio_value", &temp);
	if (!ret) {
		pr_info("%s, gpio_value from dts:%d\n", __func__, temp);
		ps_sensor.gpio_value_dts = temp;
	} else {
		pr_err("%s, Cannot find ps_support_mode from dts\n", __func__);
	}
}

static int extra_ir_class_init(struct sensor_cookie *pdata)
{
	int err = -EINVAL;

	pr_info("%s: start\n", __func__);
	if (!pdata) {
		pr_err("%s: pdata is null\n", __func__);
		return err;
	}
	pdata->sensor_cdev = sensors_proximity_cdev;
	err = sensors_classdev_register(&pdata->input_dev_ps->dev, &pdata->sensor_cdev);
	if (err != 0) {
		pr_err("%s: sensors_classdev_register failed, %d\n", __func__, err);
		return err;
	}

	pr_info("%s: success", __func__);
	return 0;
}

static int extra_ir_input_init(struct sensor_cookie *pdata)
{
	int ret = -EINVAL;

	pr_info("%s: start\n", __func__);
	if (!pdata) {
		pr_err("%s: pdata is null\n", __func__);
		return ret;
	}
	pdata->input_dev_ps = input_allocate_device();
	if (!pdata->input_dev_ps) {
		ret = -1;
		pr_err("%s: input_allocate_device failed", __func__);
		return ret;
	}
	pdata->input_dev_ps->name = "ps";
	ret = input_register_device(pdata->input_dev_ps);
	if (ret != 0) {
		ret = -ENOMEM;
		pr_err("%s: input_register_device failed", __func__);
		input_free_device(pdata->input_dev_ps);
		return ret;
	}
	pr_info("%s: success", __func__);
	return 0;
}

static int extra_ir_probe(struct platform_device *pdev)
{
	int ret;
	struct sensor_cookie *pdata = NULL;

	pr_info("%s: start", __func__);
	if (!pdev) {
		pr_err("%s: pdev is null\n", __func__);
		return -EINVAL;
	}
	pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;
	pdata->attrs_group = &ps_sensor_attrs_grp;

	ret = extra_ir_input_init(pdata);
	if (ret != 0) {
		devm_kfree(&pdev->dev, pdata);
		pr_err("%s: extra_ir_input_init failed\n", __func__);
		return ret;
	}

	ret = extra_ir_class_init(pdata);
	if (ret != 0) {
		input_unregister_device(pdata->input_dev_ps);
		input_free_device(pdata->input_dev_ps);
		devm_kfree(&pdev->dev, pdata);
		pr_err("%s: extra_ir_class_init failed\n", __func__);
		return ret;
	}

	ps_pdata = pdata;
	if ((ps_pdata->attrs_group) &&
		(sysfs_create_group(&ps_pdata->sensor_cdev.dev->kobj, ps_pdata->attrs_group))) {
		pr_err("create files failed in psensor");
	}

	pr_info("%s: success\n", __func__);
	return 0;
}

static int extra_ir_remove(struct platform_device *pdev)
{
	struct sensor_cookie *pdata = NULL;

	pr_info("%s: start\n", __func__);
	if (!pdev) {
		pr_err("%s: pdev is null\n", __func__);
		return -EINVAL;
	}
	pdata = platform_get_drvdata(pdev);
	if (!pdata) {
		pr_err("%s: pdata is null\n", __func__);
		return -EINVAL;
	}

	sensors_classdev_unregister(&pdata->sensor_cdev);
	input_unregister_device(pdata->input_dev_ps);
	input_free_device(pdata->input_dev_ps);
	devm_kfree(&pdev->dev, pdata);
	pr_info("%s: success\n", __func__);
	return 0;
}

static const struct of_device_id ps_sensor_match_table[] = {
	{ .compatible = "huawei,extra_ir", },
	{},
};
MODULE_DEVICE_TABLE(of, ps_sensor_match_table);

static struct platform_driver ps_sensor_driver = {
	.driver = {
		.name = "extra_ir",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ps_sensor_match_table),
	},
	.probe = extra_ir_probe,
	.remove = extra_ir_remove,
};

static int __init extra_ir_init(void)
{
	int rc;
	pr_info("%s: start\n", __func__);
	hw_get_ps_para_from_dts();
	if (ps_sensor.ext_ir_mode_dts == 1) {
		rc = platform_driver_register(&ps_sensor_driver);
		if (rc != 0) {
			pr_err("%s: add driver failed, rc=%d\n", __func__, rc);
			return rc;
		}
	}

	pr_info("%s: add driver success\n", __func__);
	return 0;
}

static void __exit extra_ir_exit(void)
{
	if (ps_sensor.ext_ir_mode_dts == 1) {
		platform_driver_unregister(&ps_sensor_driver);
	}
	pr_info("%s end\n", __func__);
}

module_init(extra_ir_init);
module_exit(extra_ir_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("extra ir driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
