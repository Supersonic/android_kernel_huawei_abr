// SPDX-License-Identifier: GPL-2.0
/*
 * usb_temp.c
 *
 * usb temperature driver
 *
 * Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.
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

#include <linux/device.h>
#include <linux/errno.h>
#include <linux/iio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <chipset_common/hwpower/common_module/power_thermalzone.h>

#define HWLOG_TAG usb_temp
HWLOG_REGIST();

#define DEFAULT_ADC_CHANNEL        3
#define ADC_SAMPLE_RETRY_MAX       3
#define ADC_VOL_INIT               (-1)

struct usb_temp_info {
	struct device *dev;
	int adc_channel;
	int using_mtcharger_adc;
	struct iio_channel *channel_raw;
};

static void usb_temp_parse_dt(struct usb_temp_info *info,
	struct device_node *node)
{
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), node,
		"uscp_adc", (u32 *)&info->adc_channel,
		DEFAULT_ADC_CHANNEL);
}

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
static int usb_temp_get_raw_data(int adc_channel, long *data, void *dev_data)
{
	struct usb_temp_info *info = dev_data;
	int ret;
	int val = 250; /* default is 25 centigrade */

	if (!data || !info)
		return -EPERM;

	ret = power_supply_get_int_property_value("usb",
		POWER_SUPPLY_PROP_TEMP, &val);
	if (ret)
		hwlog_err("glink usb temp get fail\n");

	*data = val / 10; /* 10: convert temperature unit */
	return 0;
}
#elif defined(CONFIG_IIO)
static int usb_temp_get_raw_data(int adc_channel, long *data, void *dev_data)
{
	int i;
	int ret;
	int t_sample;
	struct usb_temp_info *info = dev_data;

	if (!data || !info || IS_ERR(info->channel_raw))
		return -EPERM;

	for (i = 0; i < ADC_SAMPLE_RETRY_MAX; ++i) {
		t_sample = ADC_VOL_INIT;
		ret = iio_read_channel_processed(info->channel_raw, &t_sample);
		/* trans to mv */
		t_sample = t_sample / POWER_UV_PER_MV;
		if (ret < 0)
			hwlog_err("iio adc read fail\n");
		else
			break;
	}

	*data = t_sample;
	return 0;
}
#else
static int usb_temp_get_raw_data(int adc_channel, long *data, void *dev_data)
{
	struct usb_temp_info *info = dev_data;

	if (!data || !info)
		return -EPERM;

	*data = ADC_VOL_INIT;
	return 0;
}
#endif /* CONFIG_IIO */

static struct power_tz_ops usb_temp_tz_ops = {
	.get_raw_data = usb_temp_get_raw_data,
};

static int usb_temp_probe(struct platform_device *pdev)
{
	struct usb_temp_info *info = NULL;
	struct device_node *node = NULL;
	int ret;

	if (!pdev || !pdev->dev.of_node)
		return -ENODEV;

	node = pdev->dev.of_node;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	usb_temp_parse_dt(info, node);

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
	hwlog_info("get usb temp from glink\n");
#elif defined(CONFIG_IIO)
	info->channel_raw = iio_channel_get(&pdev->dev, "uscp_channel");
	ret = IS_ERR(info->channel_raw);
	if (ret) {
		hwlog_err("fail to usb adc auxadc iio: %d\n", ret);
		goto fail_free_mem;
	}
#endif /* CONFIG_IIO */

	usb_temp_tz_ops.dev_data = info;
	ret = power_tz_ops_register(&usb_temp_tz_ops, "uscp");
	if (ret)
		goto fail_free_mem;

	info->dev = &pdev->dev;
	platform_set_drvdata(pdev, info);

	return 0;

fail_free_mem:
	devm_kfree(&pdev->dev, info);
	return ret;
}

static int usb_temp_remove(struct platform_device *pdev)
{
	struct usb_temp_info *info = platform_get_drvdata(pdev);

	if (!info)
		return -ENODEV;

	platform_set_drvdata(pdev, NULL);
	devm_kfree(&pdev->dev, info);

	return 0;
}

static const struct of_device_id usb_temp_match_table[] = {
	{
		.compatible = "huawei,qcom_usb_temp",
		.data = NULL,
	},
	{},
};

static struct platform_driver usb_temp_driver = {
	.probe = usb_temp_probe,
	.remove = usb_temp_remove,
	.driver = {
		.name = "qcom_usb_temp",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(usb_temp_match_table),
	},
};

static int __init usb_temp_init(void)
{
	return platform_driver_register(&usb_temp_driver);
}

static void __exit usb_temp_exit(void)
{
	platform_driver_unregister(&usb_temp_driver);
}

device_initcall(usb_temp_init);
module_exit(usb_temp_exit);

MODULE_DESCRIPTION("usb temp driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
