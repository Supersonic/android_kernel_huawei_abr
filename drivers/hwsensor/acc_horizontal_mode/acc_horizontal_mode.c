/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: some functions of acc horizontal mode
 * Author: huangxinlong
 * Create: 2021-1-20
 * Notes: horizontal mode
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

struct acc_mode acc_sensor_mode = {
	.horizontal_mode = 0,
};

static struct sensors_classdev acc_sensor_cdev = {
	.name = "acc_sensor",
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
	struct input_dev *input_dev_acc;
};

static struct sensor_cookie *acc_pdata;

static ssize_t acc_horizontal_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int val = acc_sensor_mode.horizontal_mode;

	return snprintf_s(buf, PAGE_SIZE, PAGE_SIZE - 1, "%d\n", val);
}

static DEVICE_ATTR(acc_horizontal_mode, 0664, acc_horizontal_mode_show, NULL);

static struct attribute *acc_sensor_attrs[] = {
	&dev_attr_acc_horizontal_mode.attr,
	NULL,
};

static const struct attribute_group acc_sensor_attrs_grp = {
	.attrs = acc_sensor_attrs,
};

static void hw_get_acc_para_from_dts(void)
{
	int ret, temp;
	struct device_node *number_node = NULL;
	number_node = of_find_compatible_node(NULL, NULL, "huawei,acc_horizontal");
	if (number_node == NULL) {
		pr_err("Cannot find acc mode acc_horizontal from dts\n");
		return;
	}

	ret = of_property_read_u32(number_node, "horizontal_mode", &temp);
	if (!ret) {
		pr_info("%s, horizontal_mode from dts:%d\n", __func__, temp);
		acc_sensor_mode.horizontal_mode = temp;
	} else {
		pr_err("%s, Cannot find horizontal_mode from dts\n", __func__);
	}
}

static int acc_mode_class_init(struct sensor_cookie *pdata)
{
	int err = -EINVAL;

	pr_info("%s: start\n", __func__);
	if (!pdata) {
		pr_err("%s: pdata is null\n", __func__);
		return err;
	}
	pdata->sensor_cdev = acc_sensor_cdev;
	err = sensors_classdev_register(&pdata->input_dev_acc->dev, &pdata->sensor_cdev);
	if (err != 0) {
		pr_err("%s: sensors_classdev_register failed, %d\n", __func__, err);
		return err;
	}

	pr_info("%s: success", __func__);
	return 0;
}

static int acc_mode_input_init(struct sensor_cookie *pdata)
{
	int ret = -EINVAL;

	pr_info("%s: start\n", __func__);
	if (!pdata) {
		pr_err("%s: pdata is null\n", __func__);
		return ret;
	}
	pdata->input_dev_acc = input_allocate_device();
	if (!pdata->input_dev_acc) {
		ret = -1;
		pr_err("%s: input_allocate_device failed", __func__);
		return ret;
	}
	pdata->input_dev_acc->name = "acc";
	ret = input_register_device(pdata->input_dev_acc);
	if (ret != 0) {
		ret = -ENOMEM;
		pr_err("%s: input_register_device failed", __func__);
		input_free_device(pdata->input_dev_acc);
		return ret;
	}
	pr_info("%s: success", __func__);
	return 0;
}

static int acc_mode_probe(struct platform_device *pdev)
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
	pdata->attrs_group = &acc_sensor_attrs_grp;

	ret = acc_mode_input_init(pdata);
	if (ret != 0) {
		devm_kfree(&pdev->dev, pdata);
		pr_err("%s: acc_mode_input_init failed\n", __func__);
		return ret;
	}

	ret = acc_mode_class_init(pdata);
	if (ret != 0) {
		input_unregister_device(pdata->input_dev_acc);
		input_free_device(pdata->input_dev_acc);
		devm_kfree(&pdev->dev, pdata);
		pr_err("%s: acc_mode_class_init failed\n", __func__);
		return ret;
	}

	acc_pdata = pdata;
	if ((acc_pdata->attrs_group) &&
		(sysfs_create_group(&acc_pdata->sensor_cdev.dev->kobj, acc_pdata->attrs_group))) {
		pr_err("create files failed in accsensor");
	}

	pr_info("%s: success\n", __func__);
	return 0;
}

static int acc_mode_remove(struct platform_device *pdev)
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
	input_unregister_device(pdata->input_dev_acc);
	input_free_device(pdata->input_dev_acc);
	devm_kfree(&pdev->dev, pdata);
	pr_info("%s: success\n", __func__);
	return 0;
}

static const struct of_device_id acc_sensor_match_table[] = {
	{ .compatible = "huawei,acc_horizontal", },
	{ },
};
MODULE_DEVICE_TABLE(of, acc_sensor_match_table);

static struct platform_driver acc_sensor_driver = {
	.driver = {
		.name = "acc_horizontal",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(acc_sensor_match_table),
	},
	.probe = acc_mode_probe,
	.remove = acc_mode_remove,
};

static int __init acc_mode_init(void)
{
	int rc;
	pr_info("%s: start\n", __func__);
	hw_get_acc_para_from_dts();
	if (acc_sensor_mode.horizontal_mode == 1) {
		rc = platform_driver_register(&acc_sensor_driver);
		if (rc != 0) {
			pr_err("%s: add driver failed, rc=%d\n", __func__, rc);
			return rc;
		}
	}

	pr_info("%s: add driver success\n", __func__);
	return 0;
}

static void __exit acc_mode_exit(void)
{
	if (acc_sensor_mode.horizontal_mode == 1)
		platform_driver_unregister(&acc_sensor_driver);
}

module_init(acc_mode_init);
module_exit(acc_mode_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("acc horizontal mode driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
