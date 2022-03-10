/*
 * ps_sensor.c
 *
 * tp substitute approximate sensor driver
 *
 * Copyright (c) 2019-2019 Huawei Technologies Co., Ltd.
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
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

#include "huawei_thp_attr.h"
#include "../sensors_class.h"

#define TP_SENSOR_DRV_NAME        "ps_sensor"
#define TP_SENSOR_NEAR            0
#define TP_SENSOR_FAR             1
#define DEFAULT_VERSION           1
#define TP_SENSOR_ENABLE          1
#define TP_SENSOR_DISABLE         0
#define PROX_EVENT_LEN            12
#define MAX_RANGE_VALUE           "9.0"
#define DEFAULT_RESOLUTION        "1.0"
#define SENSOR_POWER              "3"
#define DELAY_MSEC                100
#define VENDOR_NAME               "huawei"
#define SENSORS_CLASS_NAME        "proximity-tp"
#define PS_INPUT_DEVICE_NAME      "proximity"
#define FIRST_ELEMENT             0
#define SUCCESS_RET               0

static struct sensors_classdev sensors_proximity_cdev = {
	.name = SENSORS_CLASS_NAME,
	.vendor = VENDOR_NAME,
	.version = DEFAULT_VERSION,
	.handle = SENSORS_PROXIMITY_HANDLE,
	.type = SENSOR_TYPE_PROXIMITY,
	.max_range = MAX_RANGE_VALUE,
	.resolution = DEFAULT_RESOLUTION,
	.sensor_power = SENSOR_POWER,
	/* in microseconds */
	.min_delay = 0,
	.enabled = TP_SENSOR_DISABLE,
	.delay_msec = DELAY_MSEC,
	.sensors_enable = NULL,
	.sensors_poll_delay = NULL,
	.flags = 3,
};

struct tp_sensor_data {
	/* sensor class for ps */
	struct device *pdev;
	struct sensors_classdev ps_cdev;
	struct input_dev *input_dev_ps;
	unsigned int enable;
};

static struct tp_sensor_data *tp_pdata;

int thp_prox_event_report(int report_value[], unsigned int len)
{
	int near_far;
	bool params_flag = false;
	bool value_invalid = false;

	params_flag = (!report_value) || (len > PROX_EVENT_LEN) ||
		(len == 0) || (!tp_pdata);
	if (params_flag) {
		pr_err("%s:params are invalid\n", __func__);
		return -EINVAL;
	}

	near_far = report_value[FIRST_ELEMENT];
	value_invalid = (near_far != TP_SENSOR_NEAR) &&
		(near_far != TP_SENSOR_FAR);
	if (value_invalid) {
		pr_err("%s:near_far is invalid\n", __func__);
		return -EINVAL;
	}
	input_report_abs(tp_pdata->input_dev_ps, ABS_DISTANCE, near_far);
	input_sync(tp_pdata->input_dev_ps);
	return SUCCESS_RET;
}

static int tp_ps_set_enable(struct sensors_classdev *sensors_cdev, unsigned int enable)
{
	int ret = -EINVAL;
	bool enable_flag = false;
	bool param_invalid = (enable != TP_SENSOR_ENABLE) &&
		(enable != TP_SENSOR_DISABLE);

	if (param_invalid) {
		pr_err("%s:invalid params, enable=%d\n", __func__, enable);
		return ret;
	}
	enable_flag = (enable == TP_SENSOR_ENABLE) ? true : false;
	ret = thp_set_prox_switch_status(enable_flag);
	if (ret != SUCCESS_RET) {
		pr_err("%s:failed, ret=%d\n", __func__, ret);
		return ret;
	}
	return SUCCESS_RET;
}

static int tp_sensor_class_init(struct tp_sensor_data *pdata)
{
	int err = -EINVAL;

	pr_info("%s:start\n", __func__);
	if (!pdata) {
		pr_err("%s:pdata is null\n", __func__);
		return err;
	}
	pdata->ps_cdev = sensors_proximity_cdev;
	pdata->ps_cdev.sensors_enable = tp_ps_set_enable;
	pdata->ps_cdev.sensors_poll_delay = NULL;

	err = sensors_classdev_register(&pdata->input_dev_ps->dev,
		&pdata->ps_cdev);
	if (err != SUCCESS_RET) {
		pr_err("%s:failed: %d\n", __func__, err);
		return err;
	}
	pr_info("%s:success\n", __func__);
	return SUCCESS_RET;
}

static int tp_sensor_input_init(struct tp_sensor_data *pdata)
{
	int ret = -EINVAL;

	pr_info("%s:start\n", __func__);
	if (!pdata) {
		pr_err("%s:pdata is null\n", __func__);
		return ret;
	}
	pdata->input_dev_ps = input_allocate_device();
	if (!pdata->input_dev_ps) {
		pr_err("%s:input_allocate_device failed\n", __func__);
		return ret;
	}
	set_bit(EV_ABS, pdata->input_dev_ps->evbit);
	input_set_abs_params(pdata->input_dev_ps, ABS_DISTANCE, TP_SENSOR_NEAR,
		TP_SENSOR_FAR, 0, 0);
	pdata->input_dev_ps->name = PS_INPUT_DEVICE_NAME;
	ret = input_register_device(pdata->input_dev_ps);
	if (ret != SUCCESS_RET) {
		ret = -ENOMEM;
		pr_err("%s:input_register_device failed\n", __func__);
		input_free_device(pdata->input_dev_ps);
		return ret;
	}
	pr_info("%s:success\n", __func__);
	return SUCCESS_RET;
}

static int tp_sensor_probe(struct platform_device *pdev)
{
	struct tp_sensor_data *pdata = NULL;
	int ret;

	pr_info("%s:start\n", __func__);
	if (!pdev) {
		pr_err("%s:pdev is null\n", __func__);
		return -EINVAL;
	}
	pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;
	pdata->enable = TP_SENSOR_DISABLE;
	ret = tp_sensor_input_init(pdata);
	if (ret != SUCCESS_RET) {
		devm_kfree(&pdev->dev, pdata);
		pr_err("%s:tp_sensor_input_init failed\n", __func__);
		return ret;
	}
	ret = tp_sensor_class_init(pdata);
	if (ret != SUCCESS_RET) {
		input_unregister_device(pdata->input_dev_ps);
		input_free_device(pdata->input_dev_ps);
		devm_kfree(&pdev->dev, pdata);
		pr_err("%s:tp_sensor_class_init failed\n", __func__);
		return ret;
	}
	tp_pdata = pdata;
	pr_info("%s:success\n", __func__);
	return SUCCESS_RET;
}

static int tp_sensor_remove(struct platform_device *pdev)
{
	struct tp_sensor_data *pdata = NULL;

	pr_info("%s:start\n", __func__);
	if (!pdev) {
		pr_err("%s:pdev is null\n", __func__);
		return -EINVAL;
	}
	pdata = platform_get_drvdata(pdev);
	if (!pdata) {
		pr_err("%s:pdata is null\n", __func__);
		return -EINVAL;
	}
	sensors_classdev_unregister(&pdata->ps_cdev);
	input_unregister_device(pdata->input_dev_ps);
	input_free_device(pdata->input_dev_ps);
	devm_kfree(&pdev->dev, pdata);
	pr_info("%s:success\n", __func__);
	return SUCCESS_RET;
}

static const struct of_device_id tp_sensor_match_table[] = {
	{ .compatible = "huawei,ps_sensor", },
	{},
};
MODULE_DEVICE_TABLE(of, tp_sensor_match_table);

static struct platform_driver tp_sensor_driver = {
	.driver = {
		.name   = TP_SENSOR_DRV_NAME,
		.owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(tp_sensor_match_table),
	},
	.probe  = tp_sensor_probe,
	.remove = tp_sensor_remove,
};

static int __init tp_sensor_init(void)
{
	int rc;

	pr_info("%s:start\n", __func__);
	rc = platform_driver_register(&tp_sensor_driver);
	if (rc != SUCCESS_RET) {
		pr_err("%s:add driver failed, rc=%d\n", __func__, rc);
		return rc;
	}
	pr_info("%s:add driver success\n", __func__);
	return SUCCESS_RET;
}

static void __exit tp_sensor_exit(void)
{
	platform_driver_unregister(&tp_sensor_driver);
}

module_init(tp_sensor_init);
module_exit(tp_sensor_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("TP proximity sensor driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
