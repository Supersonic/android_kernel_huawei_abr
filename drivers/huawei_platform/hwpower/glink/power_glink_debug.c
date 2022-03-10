// SPDX-License-Identifier: GPL-2.0
/*
 * power_glink_debug.c
 *
 * glink debug for power module
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

#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_debug.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_time.h>
#include <huawei_platform/hwpower/common_module/power_glink.h>

#define HWLOG_TAG power_glink_dbg
HWLOG_REGIST();

struct power_glink_dbg_info {
	u32 prop_id;
	u32 time_sync_timeout;
	struct delayed_work time_sync_work;
};

enum {
	TIME_YEAR,
	TIME_MON,
	TIME_DAY,
	TIME_HOUR,
	TIME_MIN,
	TIME_SEC,
	TIME_MSEC,
	TIME_END,
};

static struct power_glink_dbg_info *g_power_glink_dbg_info;

static ssize_t power_glink_dbg_setprop_store(void *dev_data, const char *buf,
	size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 prop_id = 0;
	u32 data_buffer = 0;

	if (!buf || !info)
		return -EINVAL;

	/* 2: two parameters */
	if (sscanf(buf, "%u %u", &prop_id, &data_buffer) != 2) {
		hwlog_err("unable to parse input:%s\n", buf);
		return -EINVAL;
	}

	power_glink_set_property_value(prop_id, &data_buffer, GLINK_DATA_ONE);

	hwlog_info("dbg_setprop: prop_id=%u data_buf=%u\n", prop_id, data_buffer);
	return size;
}

static ssize_t power_glink_dbg_getprop_store(void *dev_data, const char *buf,
	size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 prop_id = 0;

	if (!buf || !info)
		return -EINVAL;

	if (kstrtou32(buf, 0, &prop_id)) {
		hwlog_err("unable to parse input:%s\n", buf);
		return -EINVAL;
	}

	info->prop_id = prop_id;

	hwlog_info("dbg_getprop: prop_id=%u\n", prop_id);
	return size;
}

static ssize_t power_glink_dbg_getprop_show(void *dev_data, char *buf, size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 prop_id;
	u32 data_buffer = 0;

	if (!info)
		return -EINVAL;

	prop_id = info->prop_id;
	power_glink_get_property_value(prop_id, &data_buffer, GLINK_DATA_ONE);
	info->prop_id = POWER_GLINK_PROP_ID_END;

	return scnprintf(buf, PAGE_SIZE, "dbg_getprop: prop_id=%u data_buf=%u\n",
		prop_id, data_buffer);
}

static ssize_t power_glink_dbg_set_reg_value_store(void *dev_data,
	const char *buf, size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 pmic_index = 0;
	u32 reg = 0;
	u32 value = 0;
	u32 data_buffer[3]; /* [0]: pmic_index, [1]: reg, [2]: value */

	if (!buf || !info)
		return -EINVAL;

	/* 3: three parameters */
	if (sscanf(buf, "%u 0x%x 0x%x", &pmic_index, &reg, &value) != 3) {
		hwlog_err("unable to parse input:%s\n", buf);
		return -EINVAL;
	}

	data_buffer[0] = pmic_index;
	data_buffer[1] = reg;
	data_buffer[2] = value;
	if (power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_REGISTER_VALUE,
		data_buffer, ARRAY_SIZE(data_buffer))) {
		hwlog_err("dbg_set_reg_value fail\n");
		return -EINVAL;
	}

	hwlog_info("dbg_set_reg_value: pmic_index=%u, reg=0x%x, value=0x%x\n",
		pmic_index, reg, value);
	return size;
}

static ssize_t power_glink_dbg_get_reg_value_show(void *dev_data, char *buf,
	size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 data_buffer = 0;

	if (!buf || !info)
		return -EINVAL;

	if (power_glink_get_property_value(POWER_GLINK_PROP_ID_GET_REGISTER_VALUE,
		&data_buffer, GLINK_DATA_ONE)) {
		hwlog_err("dbg_get_reg_value fail\n");
		return -EINVAL;
	}

	return scnprintf(buf, PAGE_SIZE, "%u\n", data_buffer);
}

static ssize_t power_glink_dbg_get_reg_value_store(void *dev_data,
	const char *buf, size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 pmic_index = 0;
	u32 reg = 0;
	u32 data_buffer[2]; /* [0]: pmic_index, [1]: reg */

	if (!buf || !info)
		return -EINVAL;

	/* 2: two parameters */
	if (sscanf(buf, "%u 0x%x", &pmic_index, &reg) != 2) {
		hwlog_err("unable to parse input:%s\n", buf);
		return -EINVAL;
	}

	data_buffer[0] = pmic_index;
	data_buffer[1] = reg;
	if (power_glink_set_property_value(POWER_GLINK_PROP_ID_GET_REGISTER_VALUE,
		data_buffer, ARRAY_SIZE(data_buffer))) {
		hwlog_err("dbg_get_reg_value fail\n");
		return -EINVAL;
	}

	hwlog_info("dbg_get_reg_value: pmic_index=%u, reg=0x%x\n", pmic_index, reg);
	return size;
}

static ssize_t power_glink_dbg_set_sync_timeout_show(void *dev_data, char *buf,
	size_t size)
{
	struct power_glink_dbg_info *info = dev_data;

	if (!buf || !info)
		return -EINVAL;

	return scnprintf(buf, PAGE_SIZE, "%u\n", info->time_sync_timeout);
}

static ssize_t power_glink_dbg_set_sync_timeout_store(void *dev_data,
	const char *buf, size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 value = 0;

	if (!buf || !info)
		return -EINVAL;

	/* 1: one parameters */
	if (sscanf(buf, "%u", &value) != 1) {
		hwlog_err("unable to parse input:%s\n", buf);
		return -EINVAL;
	}

	info->time_sync_timeout = value;
	hwlog_info("dbg_set_sync_timeout: %us\n", value);
	return size;
}

static ssize_t power_glink_dbg_set_sync_work_store(void *dev_data,
	const char *buf, size_t size)
{
	struct power_glink_dbg_info *info = dev_data;
	u32 value = 0;

	if (!buf || !info)
		return -EINVAL;

	/* 1: one parameters */
	if (sscanf(buf, "%u", &value) != 1) {
		hwlog_err("unable to parse input:%s\n", buf);
		return -EINVAL;
	}

	if (value)
		schedule_delayed_work(&info->time_sync_work, msecs_to_jiffies(0));
	else
		cancel_delayed_work(&info->time_sync_work);

	hwlog_info("dbg_set_sync_work: %s\n", value ? "start" : "stop");
	return size;
}

static void power_glink_time_sync_work(struct work_struct *work)
{
	struct power_glink_dbg_info *info = g_power_glink_dbg_info;
	struct timeval tv = { 0 };
	struct rtc_time tm = { 0 };
	u32 time[TIME_END] = { 0 };
	u32 id = POWER_GLINK_PROP_ID_SET_TIME_SYNC;

	if (!info)
		return;

	power_get_timeofday(&tv);
	power_get_local_time(&tv, &tm);

	time[TIME_YEAR] = tm.tm_year + 1900; /* year since 1900 */
	time[TIME_MON] = tm.tm_mon + 1; /* month add 1 */
	time[TIME_DAY] = tm.tm_mday;
	time[TIME_HOUR] = tm.tm_hour;
	time[TIME_MIN] = tm.tm_min;
	time[TIME_SEC] = tm.tm_sec;
	time[TIME_MSEC] = tv.tv_usec / POWER_US_PER_MS;

	power_glink_set_property_value(id, time, TIME_END);

	schedule_delayed_work(&info->time_sync_work,
		msecs_to_jiffies(info->time_sync_timeout * POWER_MS_PER_S));
}

static int __init power_glink_dbg_init(void)
{
	struct power_glink_dbg_info *info = NULL;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->time_sync_timeout = 5; /* timeout:5s */
	INIT_DELAYED_WORK(&info->time_sync_work, power_glink_time_sync_work);

	g_power_glink_dbg_info = info;
	info->prop_id = POWER_GLINK_PROP_ID_END;

	power_dbg_ops_register("power_glink", "setprop", (void *)info,
		NULL, power_glink_dbg_setprop_store);
	power_dbg_ops_register("power_glink", "getprop", (void *)info,
		power_glink_dbg_getprop_show,
		power_glink_dbg_getprop_store);
	power_dbg_ops_register("power_glink", "set_reg_value", (void *)info,
		NULL, power_glink_dbg_set_reg_value_store);
	power_dbg_ops_register("power_glink", "get_reg_value", (void *)info,
		power_glink_dbg_get_reg_value_show,
		power_glink_dbg_get_reg_value_store);
	power_dbg_ops_register("power_glink", "set_sync_timeout", (void *)info,
		power_glink_dbg_set_sync_timeout_show,
		power_glink_dbg_set_sync_timeout_store);
	power_dbg_ops_register("power_glink", "set_sync_work", (void *)info,
		NULL, power_glink_dbg_set_sync_work_store);
	return 0;
}

static void __exit power_glink_dbg_exit(void)
{
	if (!g_power_glink_dbg_info)
		return;

	cancel_delayed_work(&g_power_glink_dbg_info->time_sync_work);
	kfree(g_power_glink_dbg_info);
	g_power_glink_dbg_info = NULL;
}

module_init(power_glink_dbg_init);
module_exit(power_glink_dbg_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("debug for power glink module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
