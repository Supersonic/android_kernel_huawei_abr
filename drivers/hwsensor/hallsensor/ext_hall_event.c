/*
 * ext_hall_event.c
 *
 * comb_key driver
 *
 * Copyright (c) 2021 Huawei Technologies Co., Ltd.
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
#include <huawei_platform/ap_hall/ext_hall_event.h>

static ATOMIC_NOTIFIER_HEAD(ext_fold_notifier_list);
static struct delayed_work ext_fold_work;
static unsigned long ext_fold_status;

int ext_fold_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&ext_fold_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(ext_fold_register_notifier);

int ext_fold_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&ext_fold_notifier_list, nb);
}
EXPORT_SYMBOL_GPL(ext_fold_unregister_notifier);

int ext_fold_call_notifiers(unsigned long val, void *v)
{
	return atomic_notifier_call_chain(&ext_fold_notifier_list, val, v);
}
EXPORT_SYMBOL_GPL(ext_fold_call_notifiers);

void ext_fold_status_distinguish(unsigned long pressed)
{
	ext_fold_status = pressed;
	pr_info("%s status = %lu\n", __func__, ext_fold_status);
	queue_delayed_work(system_power_efficient_wq, &ext_fold_work, 0);
}
EXPORT_SYMBOL_GPL(ext_fold_status_distinguish);

static void ext_fold_notify_delayed_work(struct work_struct *work)
{
	(void)work;
	ext_fold_call_notifiers(ext_fold_status, NULL);
}

int ext_fold_nb_init(void)
{
	pr_info("init\n");
	INIT_DELAYED_WORK(&ext_fold_work, ext_fold_notify_delayed_work);
	return 0;
}
EXPORT_SYMBOL_GPL(ext_fold_nb_init);

