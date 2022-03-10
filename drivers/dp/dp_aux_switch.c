/*
  * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
  * Description: main file for dp aux driver
  * Author: zhoujiaqiao
  * Create: 2021-03-22
  */

#include "dp_aux_switch.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/pinctrl/consumer.h>

#include <huawei_platform/log/hw_log.h>

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG dp_aux_switch
HWLOG_REGIST();

struct pinctrl *g_pctl = NULL;

static void dp_aux_switch_pinctrl(const char *name)
{
	int ret;
	struct pinctrl_state *state = NULL;

	if (!g_pctl) {
		hwlog_err("%s: pinctrl is null\n", __func__);
		return;
	}

	state = pinctrl_lookup_state(g_pctl, name);
	if (IS_ERR(state)) {
		hwlog_err("%s: set pinctrl state to %s fail ret=%d\n", __func__,
			name, PTR_ERR(state));
		return;
	}

	ret = pinctrl_select_state(g_pctl, state);
	if (ret)
		hwlog_err("%s: set pinctrl state to %s fail ret=%d\n", __func__,
			name, ret);
}

void dp_aux_switch_enable(bool enable)
{
	hwlog_info("%s, %d\n", __func__, enable);

	if(enable)
		dp_aux_switch_pinctrl("aux_enable");
	else
		dp_aux_switch_pinctrl("default");
}
EXPORT_SYMBOL_GPL(dp_aux_switch_enable);

static int dp_aux_switch_probe(struct platform_device *pdev)
{
	g_pctl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(g_pctl)) {
		hwlog_err("failed %d\n", PTR_ERR(g_pctl));
		g_pctl = NULL;
	}
	return 0;
}

static const struct of_device_id dp_aux_switch_match_table[] = {
	{
		.compatible = "huawei,dp_aux_switch",
		.data = NULL,
	},
	{},
};

static struct platform_driver dp_aux_switch_driver = {
	.probe = dp_aux_switch_probe,
	.driver = {
		.name = "dp_aux_switch",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(dp_aux_switch_match_table),
	},
};

static int __init dp_aux_switch_init(void)
{
	return platform_driver_register(&dp_aux_switch_driver);
}

static void __exit dp_aux_switch_exit(void)
{
	platform_driver_unregister(&dp_aux_switch_driver);
}

fs_initcall(dp_aux_switch_init);
module_exit(dp_aux_switch_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei dp aux switch driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
