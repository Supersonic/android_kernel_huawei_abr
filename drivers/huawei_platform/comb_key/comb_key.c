/*
 * comb_key.c
 *
 * comb_key driver
 *
 * Copyright (c) 2020 Huawei Technologies Co., Ltd.
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <securec.h>
#include <huawei_platform/log/hw_log.h>
#include <huawei_platform/comb_key/comb_key_event.h>
#include <huawei_platform/comb_key/power_key_event.h>
#include <huawei_platform/comb_key/volume_key_event.h>

#define MAX_STR_SIZE                                1024
static unsigned long comb_key_status;
static long volume_key_status = -1;
static long power_key_status = -1;
static DEFINE_MUTEX(comb_key_mutex);
static ATOMIC_NOTIFIER_HEAD(comb_key_notifier_list);

static struct platform_device comb_key_info = {
	.name = "comb_key",
	.id = -1,
};

int comb_key_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&comb_key_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(comb_key_register_notifier);

int comb_key_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister( &comb_key_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(comb_key_unregister_notifier);

int comb_key_call_notifiers(unsigned long val, void *v)
{
	return atomic_notifier_call_chain(&comb_key_notifier_list, val, v);
}
EXPORT_SYMBOL_GPL(comb_key_notifier_list);

static void power_volume_comb_status_distinguish(void)
{
	static unsigned long last_comb_key_status = COMB_KEY_PRESS_RELEASE;

	if ((power_key_status == POWER_KEY_PRESS_DOWN) &&
		(volume_key_status == VOLUME_DOWN_KEY_PRESS_DOWN)) {
		comb_key_status = COMB_KEY_PRESS_DOWN;
	} else {
		comb_key_status = COMB_KEY_PRESS_RELEASE;
	}
	if (comb_key_status == last_comb_key_status)
		return;
	pr_info("%s changed to %d\n", __func__, comb_key_status);
	last_comb_key_status = comb_key_status;
	comb_key_call_notifiers(comb_key_status, NULL);
}

static int power_key_notifier(struct notifier_block *nb,
	unsigned long foo, void *bar)
{
	(void)bar;
	if ((foo == POWER_KEY_PRESS_RELEASE) ||
		(foo == POWER_KEY_PRESS_DOWN)) {
		pr_info("%s recv power key status = %lu\n", __func__, foo);
		power_key_status = (long)foo;
		power_volume_comb_status_distinguish();
	}
	return 0;
}

static struct notifier_block power_key_notify = {
	.notifier_call = power_key_notifier,
	.priority = -1,
};

static int volume_key_notifier(struct notifier_block *nb,
	unsigned long foo, void *bar)
{
	(void)bar;
	mutex_lock(&comb_key_mutex);

	if ((foo == VOLUME_DOWN_KEY_PRESS_RELEASE) ||
		(foo == VOLUME_DOWN_KEY_PRESS_DOWN)) {
		pr_info("%s recv volume key status = %lu\n", __func__, foo);
		volume_key_status = (long)foo;
		power_volume_comb_status_distinguish();
	}

	mutex_unlock(&comb_key_mutex);
	return 0;
}

static struct notifier_block volume_key_notify = {
	.notifier_call = volume_key_notifier,
	.priority = -1,
};

static ssize_t show_comb_key_status(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (!buf)
		return 0;
	pr_info("%s = %d\n", __func__, comb_key_status);
	return snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%d\n",
		comb_key_status);
}

DEVICE_ATTR(comb_key_status, 0440, show_comb_key_status, NULL);

static struct attribute *comb_key_attributes[] = {
	&dev_attr_comb_key_status.attr,
	NULL,
};

static const struct attribute_group comb_key_attr_group_node = {
	.attrs = comb_key_attributes,
};

static void comb_key_info_register(void)
{
	int ret;

	ret = platform_device_register(&comb_key_info);
	if (ret) {
		pr_info("%s: register failed, ret:%d\n", __func__, ret);
		return;
	}

	ret = sysfs_create_group(&comb_key_info.dev.kobj,
		&comb_key_attr_group_node);
	if (ret) {
		pr_err("sysfs_create_group error ret =%d\n", ret);
		platform_device_unregister(&comb_key_info);;
	}
	pr_info("%s success\n", __func__);
}

static int __init comb_key_init(void)
{
	pr_info("init\n");

	volume_key_register_notifier(&volume_key_notify);

	power_key_register_notifier(&power_key_notify);

	comb_key_info_register();
	return 0;
}

static void __exit comb_key_exit(void)
{
	pr_info("init\n");
}

module_init(comb_key_init);
module_exit(comb_key_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Comb Key driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
