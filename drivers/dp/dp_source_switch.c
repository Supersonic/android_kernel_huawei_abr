/*
 * dp_source_switch.c
 *
 * factory test for DP module
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

#include "dp_source_switch.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/regmap.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/of.h>

#include <huawei_platform/log/hw_log.h>

#define HWLOG_TAG dp_source_switch
HWLOG_REGIST();

#ifndef unused
#define unused(x)    (void)(x)
#endif

struct dp_source_priv {
	struct class *class;
	bool parse_dts_flag;
	enum dp_source_mode source_mode;
};

struct dp_device_type {
	const char *name;
	struct device *dev;
	int index;
};

static struct dp_device_type dp_source = {
	.name = "source",
	.index = 0,
};

static struct dp_source_priv g_dp_priv;

static void dp_source_mode_parse_dts(void);

__attribute__ ((weak)) int dp_display_switch_source(void)
{
	hwlog_info("%s: use weak\n", __func__);
	return 0;
}

int get_current_dp_source_mode(void)
{
	if (!g_dp_priv.parse_dts_flag)
		dp_source_mode_parse_dts();

	return g_dp_priv.source_mode;
}
EXPORT_SYMBOL_GPL(get_current_dp_source_mode);

static void dp_source_mode_parse_dts(void)
{
	struct device_node *np = NULL;
	int value = 0;
	int ret;

	hwlog_info("%s: enter\n", __func__);
	if (g_dp_priv.parse_dts_flag) {
		hwlog_info("%s: source mode has already parsed\n", __func__);
		return;
	}
	g_dp_priv.parse_dts_flag = true;

	np = of_find_compatible_node(NULL, NULL, "huawei,dp_source_switch");
	if (!np) {
		hwlog_info("%s: not found dts node\n", __func__);
		/* If no node find will setting the defalut mode as SAME_SOURCE */
		g_dp_priv.source_mode = SAME_SOURCE;
		return;
	}

	if (!of_device_is_available(np)) {
		hwlog_err("%s: dts %s not available\n", __func__, np->name);
		g_dp_priv.source_mode = SAME_SOURCE;
		return;
	}

	ret = of_property_read_u32(np, "dp_default_source_mode", &value);
	if (ret) {
		hwlog_info("%s: get default source_mode failed %d\n",
			__func__, ret);
		g_dp_priv.source_mode = SAME_SOURCE;
		return;
	}

	if (!value)
		g_dp_priv.source_mode = DIFF_SOURCE;
	else
		g_dp_priv.source_mode = SAME_SOURCE;

	hwlog_info("%s: get source mode %d success\n", __func__,
		g_dp_priv.source_mode);
}

static ssize_t dp_source_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int ret;

	unused(dev);
	unused(attr);
	if (!buf) {
		hwlog_err("%s: buf is null\n", __func__);
		return -EINVAL;
	}

	ret = scnprintf(buf, PAGE_SIZE, "%d\n", g_dp_priv.source_mode);
	return ret;
}

static ssize_t dp_source_mode_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int last_mode;
	int ret;

	unused(dev);
	unused(attr);
	if (!buf) {
		hwlog_err("%s: buf is null\n", __func__);
		return -EINVAL;
	}

	last_mode = g_dp_priv.source_mode;
	ret = kstrtoint(buf, 0, (int *)(&g_dp_priv.source_mode));
	if (ret) {
		hwlog_err("%s: store error\n", __func__);
		return -EINVAL;
	}

	if ((g_dp_priv.source_mode == SAME_SOURCE) ||
		(g_dp_priv.source_mode == DIFF_SOURCE)) {
		if (last_mode == g_dp_priv.source_mode) {
			hwlog_info("%s: sync framework source mode state %d\n",
				__func__, g_dp_priv.source_mode);
			return count;
		}

		hwlog_info("%s: store %d success\n", __func__, g_dp_priv.source_mode);
		dp_display_switch_source();
	} else {
		hwlog_err("%s: invalid source_mode %d\n", __func__,
			g_dp_priv.source_mode);
	}

	return count;
}

static struct device_attribute dev_source_mode =
	__ATTR(source_mode, 0644, dp_source_mode_show, dp_source_mode_store);

static int dp_source_mode_create_file(void)
{
	int ret;

	ret = device_create_file(dp_source.dev, &dev_source_mode);
	if (ret)
		hwlog_err("%s: dp_source create failed\n", __func__);

	return ret;
}


#define dp_device_destroy(priv, type) \
do { \
	if (!IS_ERR_OR_NULL(type.dev)) { \
		device_destroy(priv.class, type.dev->devt); \
		type.dev = NULL; \
	} \
} while (0)

#define dp_remove_file(priv, type, attr) \
do { \
	if (!IS_ERR_OR_NULL(type.dev)) { \
		device_remove_file(type.dev, &attr); \
		device_destroy(priv.class, type.dev->devt); \
		type.dev = NULL; \
	} \
} while (0)

static int __init dp_source_mode_init(void)
{
	struct class *dp_class = NULL;
	int ret;

	hwlog_info("%s: enter\n", __func__);
	memset(&g_dp_priv, 0, sizeof(g_dp_priv));

	g_dp_priv.parse_dts_flag = false;
	g_dp_priv.source_mode = SAME_SOURCE;
	dp_source_mode_parse_dts();

	dp_class = class_create(THIS_MODULE, "dp");
	if (IS_ERR(dp_class)) {
		hwlog_err("%s: create dp source class failed\n", __func__);
		return IS_ERR(dp_class);
	}
	g_dp_priv.class = dp_class;

	dp_source.dev = device_create(g_dp_priv.class, NULL, 0, NULL, dp_source.name);
	if (IS_ERR_OR_NULL(dp_source.dev)) {
		hwlog_err("%s: some device create failed\n", __func__);
		ret = -EINVAL;
		goto err_out;
	}

	ret = dp_source_mode_create_file();
	if (ret < 0)
		goto err_out;

	hwlog_info("%s: init success\n", __func__);
	return 0;

err_out:
	dp_device_destroy(g_dp_priv, dp_source);

	class_destroy(g_dp_priv.class);
	g_dp_priv.class = NULL;

	hwlog_err("%s: init failed %d\n", __func__, ret);
	return ret;
}

static void __exit dp_source_mode_exit(void)
{
	hwlog_info("%s enter\n", __func__);
	dp_remove_file(g_dp_priv, dp_source, dev_source_mode);

	class_destroy(g_dp_priv.class);
	g_dp_priv.class = NULL;
}

module_init(dp_source_mode_init);
module_exit(dp_source_mode_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Huawei dp source driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
