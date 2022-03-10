// SPDX-License-Identifier: GPL-2.0
/*
 * buck_charge.c
 *
 * buck charge driver
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

#include <chipset_common/hwpower/buck_charge/buck_charge.h>
#include <chipset_common/hwpower/charger/charger_common_interface.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/hardware_monitor/ffc_control.h>

#define HWLOG_TAG buck_charge
HWLOG_REGIST();

static struct buck_charge_dev *g_buck_charge_dev;

static void buck_charge_ffc_update_iterm(int iterm)
{
	u32 value = (iterm >= 0) ? iterm : -iterm;

	charge_set_buck_iterm(value);
	hwlog_info("buck charge set iterm=%d\n", iterm);
}

static int buck_charge_ffc_get_incr_term_volt(struct buck_charge_dev *di)
{
	static int cnt = 0;
	int ffc_vterm = ffc_get_buck_vterm();

	if (di->ffc_vterm_flag & FFC_VETRM_END_FLAG) {
		buck_charge_ffc_update_iterm(ffc_get_buck_iterm());
		return 0;
	}

	if (ffc_vterm) {
		cnt = 0;
		di->ffc_vterm_flag |= FFC_VTERM_START_FLAG;
		return ffc_vterm;
	}

	if (di->ffc_vterm_flag & FFC_VTERM_START_FLAG) {
		cnt++;
		if (cnt >= FFC_CHARGE_EXIT_TIMES) {
			di->ffc_vterm_flag |= FFC_VETRM_END_FLAG;
			cnt = 0;
		}
	}

	return 0;
}

static void buck_charge_monitor_work(struct work_struct *work)
{
	int increase_volt;
	struct buck_charge_dev *l_dev = container_of(work,
		struct buck_charge_dev, buck_charge_work.work);

	if (!l_dev)
		return;

	hwlog_info("%s enter\n", __func__);

	increase_volt = buck_charge_ffc_get_incr_term_volt(l_dev);
	charge_set_buck_fv_delta(increase_volt);
	hwlog_info("%s increase_volt=%d\n", __func__, increase_volt);

	charge_update_buck_iin_thermal();

	schedule_delayed_work(&l_dev->buck_charge_work,
		msecs_to_jiffies(BUCK_CHARGE_WORK_TIMEOUT));
	hwlog_info("%s end\n", __func__);
}

static void buck_charge_stop_monitor_work(struct work_struct *work)
{
	struct buck_charge_dev *l_dev = container_of(work,
		struct buck_charge_dev, stop_charge_work);

	if (!l_dev)
		return;

	hwlog_info("%s enter\n", __func__);

	cancel_delayed_work_sync(&l_dev->buck_charge_work);
	l_dev->ffc_vterm_flag = 0;
	charge_set_buck_fv_delta(0);
	charge_set_buck_iterm(0);
	hwlog_info("%s end\n", __func__);
}

static int buck_charge_event_notifier_call(struct notifier_block *nb,
	unsigned long event, void *data)
{
	struct buck_charge_dev *l_dev = g_buck_charge_dev;

	if (!l_dev)
		return NOTIFY_OK;

	switch (event) {
	case POWER_NE_CHARGING_START:
		schedule_delayed_work(&l_dev->buck_charge_work, msecs_to_jiffies(0));
		break;
	case POWER_NE_CHARGING_STOP:
		schedule_work(&l_dev->stop_charge_work);
		break;
	default:
		break;
	}

	return NOTIFY_OK;
}

static int buck_charge_probe(struct platform_device *pdev)
{
	int ret;
	struct buck_charge_dev *l_dev = NULL;

	l_dev = kzalloc(sizeof(*l_dev), GFP_KERNEL);
	if (!l_dev)
		return -ENOMEM;

	INIT_DELAYED_WORK(&l_dev->buck_charge_work, buck_charge_monitor_work);
	INIT_WORK(&l_dev->stop_charge_work, buck_charge_stop_monitor_work);

	l_dev->event_nb.notifier_call = buck_charge_event_notifier_call;
	ret = power_event_bnc_register(POWER_BNT_CHARGING, &l_dev->event_nb);
	if (ret)
		goto fail_free_mem;

	g_buck_charge_dev = l_dev;
	platform_set_drvdata(pdev, l_dev);
	return 0;

fail_free_mem:
	kfree(l_dev);
	g_buck_charge_dev = NULL;
	return ret;
}

static int buck_charge_remove(struct platform_device *pdev)
{
	struct buck_charge_dev *l_dev = platform_get_drvdata(pdev);

	if (!l_dev)
		return -ENODEV;

	cancel_delayed_work(&l_dev->buck_charge_work);
	power_event_bnc_unregister(POWER_BNT_CHARGING, &l_dev->event_nb);
	platform_set_drvdata(pdev, NULL);
	kfree(l_dev);
	g_buck_charge_dev = NULL;
	return 0;
}

static const struct of_device_id buck_charge_match_table[] = {
	{
		.compatible = "huawei,buck_charge",
		.data = NULL,
	},
	{},
};

static struct platform_driver buck_charge_driver = {
	.probe = buck_charge_probe,
	.remove = buck_charge_remove,
	.driver = {
		.name = "huawei,buck_charge",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(buck_charge_match_table),
	},
};

static int __init buck_charge_init(void)
{
	return platform_driver_register(&buck_charge_driver);
}

static void __exit buck_charge_exit(void)
{
	platform_driver_unregister(&buck_charge_driver);
}

device_initcall_sync(buck_charge_init);
module_exit(buck_charge_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("buck charge driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
