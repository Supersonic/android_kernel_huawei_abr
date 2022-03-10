// SPDX-License-Identifier: GPL-2.0
/*
 * power_proxy.c
 *
 * charger or battery proxy
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <chipset_common/hwpower/battery/battery_capacity_public.h>
#include <chipset_common/hwpower/common_module/power_debug.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_supply.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <huawei_platform/power/huawei_charger.h>
#include <huawei_platform/power/huawei_charger_adaptor.h>

#define HWLOG_TAG power_proxy
HWLOG_REGIST();

struct power_proxy_device {
	struct device *dev;
	struct notifier_block event_nb;
	int battery_proxy;
	int event_proxy;
};

struct power_proxy_device *g_power_proxy_dev;

int power_proxy_get_filter_sum(int base)
{
	int capacity_raw = 0;
	int capacity = 0;

	if (!g_power_proxy_dev || !g_power_proxy_dev->battery_proxy) {
		power_supply_get_int_property_value("bms", POWER_SUPPLY_PROP_CAPACITY_RAW, &capacity_raw);
		power_supply_get_int_property_value("bms", POWER_SUPPLY_PROP_CAPACITY, &capacity);
		if ((base != 0) && ((capacity_raw / base) != capacity))
			return capacity * base;

		return capacity_raw;
	}

	return bat_ui_capacity_get_filter_sum(base);
}

void power_proxy_sync_filter_soc(int rep_soc, int round_soc, int base)
{
	if (!g_power_proxy_dev || !g_power_proxy_dev->battery_proxy) {
		power_supply_set_int_property_value("bms", POWER_SUPPLY_PROP_SYNC_FILTER, rep_soc);
		return;
	}

	bat_ui_capacity_sync_filter(rep_soc, round_soc, base);
}

void power_proxy_cancle_capacity_work(void)
{
	if (!g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return;

	bat_ui_capacity_cancle_work();
}

void power_proxy_restart_capacity_work(void)
{
	if (!g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return;

	bat_ui_capacity_restart_work();
}

int power_proxy_get_bat_voltage(int *val)
{
	if (!val || !g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return -EPERM;

	if (power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_VOLTAGE_NOW, val))
		*val = POWER_SUPPLY_DEFAULT_VOLTAGE_NOW;

	return 0;
}

int power_proxy_get_bat_current(int *val)
{
	if (!val || !g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return -EPERM;

	if (power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_CURRENT_NOW, val))
		*val = POWER_SUPPLY_DEFAULT_CURRENT_NOW;

	return 0;
}

int power_proxy_get_bat_current_avg(int *val)
{
	if (!val || !g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return -EPERM;

	if (power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_CURRENT_AVG, val))
		*val = POWER_SUPPLY_DEFAULT_CURRENT_NOW;

	return 0;
}

int power_proxy_get_bat_temperature(int *val)
{
	if (!val || !g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return -EPERM;

	if (power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_TEMP, val))
		*val = POWER_SUPPLY_DEFAULT_TEMP;

	return 0;
}

int power_proxy_get_capacity(int *val)
{
	if (!val || !g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return -EPERM;

	if (power_supply_get_int_property_value("assist_battery",
		POWER_SUPPLY_PROP_CAPACITY, val))
		*val = POWER_SUPPLY_DEFAULT_CAPACITY;

	return 0;
}

int power_proxy_get_ui_capacity(int *val)
{
	if (!val || !g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return -EPERM;

	if (power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_CAPACITY, val))
		*val = POWER_SUPPLY_DEFAULT_CAPACITY;

	return 0;
}

int power_proxy_is_battery_exit(int *val)
{
	if (!val || !g_power_proxy_dev || !g_power_proxy_dev->battery_proxy)
		return -EPERM;

	if (power_supply_get_int_property_value("battery",
		POWER_SUPPLY_PROP_PRESENT, val))
		*val = 0;

	return 0;
}

static ssize_t power_proxy_bat_voltage_show(void *dev_data,
	char *buf, size_t size)
{
	int value;

	if (power_proxy_get_bat_voltage(&value))
		return scnprintf(buf, size, "%s\n", "fail");

	return scnprintf(buf, size, "%d\n", value);
}

static ssize_t power_proxy_bat_current_show(void *dev_data,
	char *buf, size_t size)
{
	int value;

	if (power_proxy_get_bat_current(&value))
		return scnprintf(buf, size, "%s\n", "fail");

	return scnprintf(buf, size, "%d\n", value);
}

static ssize_t power_proxy_bat_current_avg_show(void *dev_data,
	char *buf, size_t size)
{
	int value;

	if (power_proxy_get_bat_current_avg(&value))
		return scnprintf(buf, size, "%s\n", "fail");

	return scnprintf(buf, size, "%d\n", value);
}

static ssize_t power_proxy_bat_temperature_show(void *dev_data,
	char *buf, size_t size)
{
	int value;

	if (power_proxy_get_bat_temperature(&value))
		return scnprintf(buf, size, "%s\n", "fail");

	return scnprintf(buf, size, "%d\n", value);
}

static ssize_t power_proxy_capacity_show(void *dev_data,
	char *buf, size_t size)
{
	int value;

	if (power_proxy_get_capacity(&value))
		return scnprintf(buf, size, "%s\n", "fail");

	return scnprintf(buf, size, "%d\n", value);
}

static ssize_t power_proxy_ui_capacity_show(void *dev_data,
	char *buf, size_t size)
{
	int value;

	if (power_proxy_get_ui_capacity(&value))
		return scnprintf(buf, size, "%s\n", "fail");

	return scnprintf(buf, size, "%d\n", value);
}

static void power_proxy_debug_register(struct power_proxy_device *di)
{
	power_dbg_ops_register("power_proxy", "bat_voltage", (void *)di,
		power_proxy_bat_voltage_show, NULL);
	power_dbg_ops_register("power_proxy", "bat_current", (void *)di,
		power_proxy_bat_current_show, NULL);
	power_dbg_ops_register("power_proxy", "bat_current_avg", (void *)di,
		power_proxy_bat_current_avg_show, NULL);
	power_dbg_ops_register("power_proxy", "bat_temperature", (void *)di,
		power_proxy_bat_temperature_show, NULL);
	power_dbg_ops_register("power_proxy", "capacity", (void *)di,
		power_proxy_capacity_show, NULL);
	power_dbg_ops_register("power_proxy", "ui_capacity", (void *)di,
		power_proxy_ui_capacity_show, NULL);
}

static void power_proxy_parse_dts(struct power_proxy_device *di)
{
	struct device_node *np = di->dev->of_node;

	if (!np)
		return;

	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"battery_proxy", &di->battery_proxy, 0);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"event_proxy", &di->event_proxy, 0);
}

static int power_proxy_event_notifier_call(struct notifier_block *nb,
	unsigned long event, void *data)
{
	switch (event) {
	case POWER_NE_THIRDPLAT_CHARGING_NORMAL:
	case POWER_NE_THIRDPLAT_CHARGING_START:
		power_event_bnc_notify(POWER_BNT_CHARGING, POWER_NE_CHARGING_START, NULL);
		break;
	case POWER_NE_THIRDPLAT_CHARGING_STOP:
		power_event_bnc_notify(POWER_BNT_CHARGING, POWER_NE_CHARGING_STOP, NULL);
		break;
	case POWER_NE_THIRDPLAT_CHARGING_ERROR:
		power_event_bnc_notify(POWER_BNT_CHARGING, POWER_NE_CHARGING_SUSPEND, NULL);
		break;
	default:
		break;
	}

	hwlog_info("power proxy event=%lu\n", event);
	return NOTIFY_OK;
}

static int power_proxy_notifier_register(struct power_proxy_device *di)
{
	if (!di->event_proxy)
		return 0;

	di->event_nb.notifier_call = power_proxy_event_notifier_call;
	return power_event_bnc_register(POWER_BNT_THIRDPLAT_CHARGING, &di->event_nb);
}

static void power_proxy_notifier_unregister(struct power_proxy_device *di)
{
	if (!di->event_proxy)
		return;

	power_event_bnc_unregister(POWER_BNT_THIRDPLAT_CHARGING, &di->event_nb);
}

static int power_proxy_probe(struct platform_device *pdev)
{
	int ret;
	struct power_proxy_device *di = NULL;

	if (!pdev || !pdev->dev.of_node)
		return -ENODEV;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	di->dev = &pdev->dev;
	g_power_proxy_dev = di;
	power_proxy_parse_dts(di);
	power_proxy_debug_register(di);

	ret = power_proxy_notifier_register(di);
	if (ret)
		goto fail_free_mem;

	return 0;

fail_free_mem:
	kfree(di);
	g_power_proxy_dev = NULL;
	return -ENOMEM;
}

static int power_proxy_remove(struct platform_device *pdev)
{
	struct power_proxy_device *di = platform_get_drvdata(pdev);

	if (!di)
		return -ENODEV;

	power_proxy_notifier_unregister(di);
	kfree(di);
	g_power_proxy_dev = NULL;
	return 0;
}

static const struct of_device_id power_proxy_match_table[] = {
	{
		.compatible = "huawei,power_proxy",
		.data = NULL,
	},
	{},
};

static struct platform_driver power_proxy_driver = {
	.probe = power_proxy_probe,
	.remove = power_proxy_remove,
	.driver = {
		.name = "huawei,power_proxy",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(power_proxy_match_table),
	},
};

static int __init power_proxy_init(void)
{
	return platform_driver_register(&power_proxy_driver);
}

static void __exit power_proxy_exit(void)
{
	platform_driver_unregister(&power_proxy_driver);
}

fs_initcall_sync(power_proxy_init);
module_exit(power_proxy_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Huawei power proxy");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
