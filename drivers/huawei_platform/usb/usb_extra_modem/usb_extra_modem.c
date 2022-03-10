/*
 * usb_extra_modem.c
 *
 * file for usb_extra_modem driver
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

#include "usb_extra_modem.h"
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <chipset_common/hwpower/common_module/power_sysfs.h>
#include <huawei_platform/log/hw_log.h>

#define HWLOG_TAG usb_extra_modem
HWLOG_REGIST();

struct uem_dev_info *g_uem_di;

struct uem_dev_info *uem_get_dev_info(void)
{
	if (!g_uem_di)
		hwlog_err("dev_info is null\n");

	return g_uem_di;
}

static int uem_loadswitch_gpio_enable(int gpio_num)
{
	struct uem_dev_info *di = uem_get_dev_info();

	if (!di)
		return -ENODEV;

	switch (gpio_num) {
	case LOADSWITCH_GPIO_Q4:
		if (!gpio_is_valid(di->gpio_q4))
			return -EINVAL;

		hwlog_info("uem loadswitch gpio_q4 open\n");
		gpio_set_value(di->gpio_q4, UEM_LOADSWITCH_GPIO_ENABLE);
		break;
	case LOADSWITCH_GPIO_Q5:
		if (!gpio_is_valid(di->gpio_q5))
			return -EINVAL;

		hwlog_info("uem loadswitch gpio_q5 open\n");
		gpio_set_value(di->gpio_q5, UEM_LOADSWITCH_GPIO_ENABLE);
		break;
	default:
		break;
	}

	return 0;
}

static int uem_loadswitch_gpio_disable(int gpio_num)
{
	struct uem_dev_info *di = uem_get_dev_info();

	if (!di)
		return -ENODEV;

	switch (gpio_num) {
	case LOADSWITCH_GPIO_Q4:
		if (!gpio_is_valid(di->gpio_q4))
			return -EINVAL;

		hwlog_info("uem loadswitch gpio_q4 close\n");
		gpio_set_value(di->gpio_q4, UEM_LOADSWITCH_GPIO_DISABLE);
		break;
	case LOADSWITCH_GPIO_Q5:
		if (!gpio_is_valid(di->gpio_q5))
			return -EINVAL;

		hwlog_info("uem loadswitch gpio_q5 close\n");
		gpio_set_value(di->gpio_q5, UEM_LOADSWITCH_GPIO_DISABLE);
		break;
	default:
		break;
	}

	return 0;
}

static int uem_loadswitch_gpio_init(struct uem_dev_info *di)
{
	int ret;

	di->gpio_q4 = of_get_named_gpio(di->dev->of_node, "gpio_q4", 0);
	di->gpio_q5 = of_get_named_gpio(di->dev->of_node, "gpio_q5", 0);

	if (!gpio_is_valid(di->gpio_q4) || !gpio_is_valid(di->gpio_q5)) {
		hwlog_err("gpio_q4 or gpio_q5 is not valid\n");
		return -EINVAL;
	}

	ret = gpio_request(di->gpio_q4, "gpio_q4");
	if (ret) {
		hwlog_err("gpio q4 request fail\n");
		return -EINVAL;
	}

	ret = gpio_request(di->gpio_q5, "gpio_q5");
	if (ret) {
		hwlog_err("gpio q5 request fail\n");
		goto gpio_request_error;
	}

	ret = gpio_direction_output(di->gpio_q4, UEM_LOADSWITCH_GPIO_DISABLE);
	ret |= gpio_direction_output(di->gpio_q5, UEM_LOADSWITCH_GPIO_DISABLE);
	if (ret) {
		hwlog_err("gpio_q4 or gpio_q5 set output fail\n");
		goto gpio_set_direction_fail;
	}

	hwlog_info("uem loadswitch gpio init succ\n");
	return 0;

gpio_set_direction_fail:
	gpio_free(di->gpio_q5);
gpio_request_error:
	gpio_free(di->gpio_q4);
	return -EINVAL;
}

static ssize_t uem_gpio_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long val = 0;

	if (kstrtol(buf, UEM_BASE_DEC, &val) < 0) {
		hwlog_err("gpio enable val invalid\n");
		return -EINVAL;
	}

	uem_loadswitch_gpio_enable((int)val);
	return count;
}
static DEVICE_ATTR(uem_gpio_enable, 0664, NULL, uem_gpio_enable_store);

static ssize_t uem_gpio_disable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long val = 0;

	if (kstrtol(buf, UEM_BASE_DEC, &val) < 0) {
		hwlog_err("gpio disable val invalid\n");
		return -EINVAL;
	}

	uem_loadswitch_gpio_disable((int)val);
	return count;
}
static DEVICE_ATTR(uem_gpio_disable, 0664, NULL, uem_gpio_disable_store);

static struct attribute *uem_sysfs_attributes[] = {
	&dev_attr_uem_gpio_enable.attr,
	&dev_attr_uem_gpio_disable.attr,
	NULL,
};

static const struct attribute_group uem_attr_group = {
	.attrs = uem_sysfs_attributes,
};

static int uem_probe(struct platform_device *pdev)
{
	struct uem_dev_info *di = NULL;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	g_uem_di = di;
	di->dev = &pdev->dev;
	platform_set_drvdata(pdev, di);

	uem_loadswitch_gpio_init(di);
	power_sysfs_create_group("hw_usb", "usb_extra_modem", &uem_attr_group);

	hwlog_info("probe end\n");
	return 0;
}

static int uem_remove(struct platform_device *pdev)
{
	struct uem_dev_info *di = platform_get_drvdata(pdev);

	if (!di)
		return 0;

	power_sysfs_remove_group(di->dev, &uem_attr_group);
	gpio_free(di->gpio_q4);
	gpio_free(di->gpio_q5);
	platform_set_drvdata(pdev, NULL);
	g_uem_di = NULL;
	kfree(di);

	return 0;
}

static const struct of_device_id uem_match_table[] = {
	{
		.compatible = "huawei,usb_extra_modem",
		.data = NULL,
	},
	{},
};

static struct platform_driver uem_driver = {
	.probe = uem_probe,
	.remove = uem_remove,
	.driver = {
		.name = "huawei,usb_extra_modem",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(uem_match_table),
	},
};

static int __init uem_init(void)
{
	return platform_driver_register(&uem_driver);
}

static void __exit uem_exit(void)
{
	platform_driver_unregister(&uem_driver);
}

module_init(uem_init);
module_exit(uem_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei usb extra modem driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
