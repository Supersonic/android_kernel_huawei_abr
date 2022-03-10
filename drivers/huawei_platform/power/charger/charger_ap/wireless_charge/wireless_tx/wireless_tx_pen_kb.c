/*
 * wireless_tx_pen_kb.c
 *
 * wireless tx pen and kb
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <huawei_platform/power/wireless/wireless_transmitter_aux.h>
#include <huawei_platform/power/wireless/wireless_tx_pen_kb.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_gpio.h>
#include <chipset_common/hwpower/wireless_charge/wireless_accessory.h>

#define HWLOG_TAG wireless_tx_pen_kb
HWLOG_REGIST();

static struct wltx_pen_kb_dev_info *g_wltx_pen_kb_di;

static bool support_pen_kb_switch(void)
{
	if (!g_wltx_pen_kb_di)
		return false;
	return g_wltx_pen_kb_di->support_pen_kb_switch;
}

static void wltx_set_pen_gpio_state(void)
{
	struct wltx_pen_kb_dev_info *di = g_wltx_pen_kb_di;

	if (!di) {
		hwlog_err("di null\n");
		return;
	}

	di->switch_state = WLTX_GPIO_PEN;
	gpio_set_value(di->gpio_tx_kb_en, WLTX_GPIO_LOW);
	gpio_set_value(di->gpio_tx_pen_en, WLTX_GPIO_HIGH);
	di->pen_open_charging = true;
	di->kb_open_charging = false;
}

static void wltx_set_kb_gpio_state(void)
{
	struct wltx_pen_kb_dev_info *di = g_wltx_pen_kb_di;

	if (!di) {
		hwlog_err("di null\n");
		return;
	}

	di->switch_state = WLTX_GPIO_KB;
	gpio_set_value(di->gpio_tx_pen_en, WLTX_GPIO_LOW);
	gpio_set_value(di->gpio_tx_kb_en, WLTX_GPIO_HIGH);
	di->pen_open_charging = false;
	di->kb_open_charging = true;
}

static int wltx_get_pen_kb_dev_no(void)
{
	if (!g_wltx_pen_kb_di)
		return -EINVAL;
	return g_wltx_pen_kb_di->switch_state ? ACC_DEV_NO_KB : ACC_DEV_NO_PEN;
}

static int wltx_get_ping_timeout(void)
{
	if (!g_wltx_pen_kb_di)
		return -EINVAL;
	if (g_wltx_pen_kb_di->pen_open_charging)
		return g_wltx_pen_kb_di->ping_timeout_pen;
	return g_wltx_pen_kb_di->ping_timeout_kb;
}

static struct wltx_acc_dev *wltx_get_acc_info(int dev_no)
{
	int acc_info_index;

	if (!g_wltx_pen_kb_di)
		return NULL;

	acc_info_index = (dev_no == ACC_DEV_NO_PEN) ? WLTX_ACC_INFO_PEN : WLTX_ACC_INFO_KB;
	return &g_wltx_pen_kb_di->wireless_tx_acc[acc_info_index];
}

static void wltx_set_acc_info(struct wltx_acc_dev *acc_info)
{
	struct wltx_pen_kb_dev_info *di = g_wltx_pen_kb_di;
	int acc_info_index;

	if (!di || !acc_info) {
		hwlog_err("set_acc_info: di or acc_info null\n");
		return;
	}

	acc_info_index = di->switch_state ? WLTX_ACC_INFO_KB : WLTX_ACC_INFO_PEN;
	di->wireless_tx_acc[acc_info_index].dev_state = acc_info->dev_state;
	di->wireless_tx_acc[acc_info_index].dev_no = acc_info->dev_no;
	di->wireless_tx_acc[acc_info_index].dev_submodel_id = acc_info->dev_submodel_id;
	di->wireless_tx_acc[acc_info_index].dev_version = acc_info->dev_version;
	di->wireless_tx_acc[acc_info_index].dev_business = acc_info->dev_business;
	di->wireless_tx_acc[acc_info_index].dev_info_cnt = acc_info->dev_info_cnt;
	memcpy(di->wireless_tx_acc[acc_info_index].dev_mac,
		acc_info->dev_mac, WL_TX_ACC_DEV_MAC_LEN);
	memcpy(di->wireless_tx_acc[acc_info_index].dev_model_id,
		acc_info->dev_model_id, WL_TX_ACC_DEV_MODELID_LEN);
}

static struct wltx_aux_logic_ops g_pen_kb_logic_ops = {
	.support_switch = support_pen_kb_switch,
	.get_dev_no = wltx_get_pen_kb_dev_no,
	.set_pen_gpio = wltx_set_pen_gpio_state,
	.set_kb_gpio = wltx_set_kb_gpio_state,
	.get_ping_timeout = wltx_get_ping_timeout,
	.get_acc_info = wltx_get_acc_info,
	.set_acc_info = wltx_set_acc_info,
};

static void wltx_acc_dev_init(struct wltx_acc_dev *acc_dev, int dev_no)
{
	acc_dev->dev_no = dev_no;
	acc_dev->dev_info_cnt = WL_TX_ACC_DEV_INFO_CNT;
}

static void wltx_pen_kb_hall_away_from(struct wltx_pen_kb_dev_info *di, int dev_no, int state)
{
	wltx_aux_report_acc_info(dev_no, WL_ACC_DEV_STATE_OFFLINE);
	if (!(di->pen_online && di->kb_online)) {
		if (state == WLTX_GPIO_PEN)
			wltx_aux_open_tx(WLTX_AUX_OPEN_BY_PEN, false);
		else
			wltx_aux_open_tx(WLTX_AUX_OPEN_BY_KB, false);
	}
	if (state == WLTX_GPIO_PEN)
		di->pen_online = false;
	else
		di->kb_online = false;
}

static void pen_kb_event_work(struct work_struct *work)
{
	struct wltx_pen_kb_dev_info *di = container_of(work, struct wltx_pen_kb_dev_info,
		event_work);

	if (!di)
		return;

	switch (di->event_type) {
	case POWER_NE_WLTX_AUX_PEN_HALL_APPROACH:
		hwlog_info("POWER_NE_WLTX_AUX_PEN_HALL_APPROACH\n");
		wltx_set_pen_gpio_state();
		di->pen_online = true;
		wltx_aux_set_pen_approaching(true);
		wltx_aux_open_tx(WLTX_AUX_OPEN_BY_PEN, true);
		break;
	case POWER_NE_WLTX_AUX_PEN_HALL_AWAY_FROM:
		hwlog_info("POWER_NE_WLTX_AUX_PEN_HALL_AWAY_FROM\n");
		wltx_aux_set_pen_approaching(false);
		wltx_pen_kb_hall_away_from(di, ACC_DEV_NO_PEN, WLTX_GPIO_PEN);
		break;
	case POWER_NE_WLTX_AUX_KB_HALL_APPROACH:
		hwlog_info("POWER_NE_WLTX_AUX_KB_HALL_APPROACH\n");
		wltx_set_kb_gpio_state();
		di->kb_online = true;
		wltx_aux_open_tx(WLTX_AUX_OPEN_BY_KB, true);
		break;
	case POWER_NE_WLTX_AUX_KB_HALL_AWAY_FROM:
		hwlog_info("POWER_NE_WLTX_AUX_KB_HALL_AWAY_FROM\n");
		wltx_pen_kb_hall_away_from(di, ACC_DEV_NO_KB, WLTX_GPIO_KB);
		break;
	default:
		hwlog_err("has no this event_type %u\n", di->event_type);
		break;
	}
}

static int event_notifier_call(struct notifier_block *nb, unsigned long event, void *data)
{
	struct wltx_pen_kb_dev_info *di = container_of(nb, struct wltx_pen_kb_dev_info, event_nb);

	if (!di)
		return NOTIFY_OK;

	switch (event) {
	case POWER_NE_WLTX_AUX_PEN_HALL_APPROACH:
		wltx_acc_dev_init(&di->wireless_tx_acc[WLTX_ACC_INFO_PEN], ACC_DEV_NO_PEN);
		wltx_aux_report_acc_info(ACC_DEV_NO_PEN, WL_ACC_DEV_STATE_PINGING);
		break;
	case POWER_NE_WLTX_AUX_KB_HALL_APPROACH:
		wltx_acc_dev_init(&di->wireless_tx_acc[WLTX_ACC_INFO_KB], ACC_DEV_NO_KB);
		wltx_aux_report_acc_info(ACC_DEV_NO_KB, WL_ACC_DEV_STATE_PINGING);
		break;
	case POWER_NE_WLTX_AUX_KB_HALL_AWAY_FROM:
	case POWER_NE_WLTX_AUX_PEN_HALL_AWAY_FROM:
		break;
	default:
		return NOTIFY_OK;
	}

	di->event_type = event;
	schedule_work(&di->event_work);

	return NOTIFY_OK;
}

static void wltx_pen_kb_parse_dts(struct device_node *np, struct wltx_pen_kb_dev_info *di)
{
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"support_pen_kb_switch", &di->support_pen_kb_switch, 0);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"ping_timeout_pen", &di->ping_timeout_pen, WLTX_PING_TIMEOUT_PEN_DEFAULT);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"ping_timeout_kb", &di->ping_timeout_kb, WLTX_PING_TIMEOUT_KB_DEFAULT);
}

static int wltx_pen_kb_gpio_init(struct device_node *np, struct wltx_pen_kb_dev_info *di)
{
	if (power_gpio_config_output(np,
		"gpio_tx_pen_en", "gpio_tx_pen_en", &di->gpio_tx_pen_en, 0))
		return -EINVAL;
	if (power_gpio_config_output(np,
		"gpio_tx_kb_en", "gpio_tx_kb_en", &di->gpio_tx_kb_en, 0))
		return -EINVAL;
	return 0;
}

static int pen_kb_probe(struct platform_device *pdev)
{
	int ret;
	struct wltx_pen_kb_dev_info *di = NULL;
	struct device_node *np = NULL;

	di = devm_kzalloc(&pdev->dev, sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	di->dev = &pdev->dev;
	np = di->dev->of_node;
	g_wltx_pen_kb_di = di;

	wltx_pen_kb_parse_dts(np, di);
	ret = wltx_pen_kb_gpio_init(np, di);
	if (ret)
		goto pen_kb_gpio_init_fail;

	INIT_WORK(&di->event_work, pen_kb_event_work);
	platform_set_drvdata(pdev, di);
	di->event_nb.notifier_call = event_notifier_call;
	ret = power_event_bnc_register(POWER_BNT_WLTX_AUX, &di->event_nb);
	if (ret)
		goto notifier_regist_fail;
	ret = wltx_aux_logic_ops_register(&g_pen_kb_logic_ops);
	if (ret)
		goto logic_ops_regist_fail;

	hwlog_info("probe ok\n");
	return 0;

logic_ops_regist_fail:
	(void)power_event_bnc_unregister(POWER_BNT_WLTX_AUX, &di->event_nb);
notifier_regist_fail:
	platform_set_drvdata(pdev, NULL);
	gpio_free(di->gpio_tx_kb_en);
	gpio_free(di->gpio_tx_pen_en);
pen_kb_gpio_init_fail:
	devm_kfree(&pdev->dev, di);
	g_wltx_pen_kb_di = NULL;

	return ret;
}

static int pen_kb_remove(struct platform_device *pdev)
{
	struct wltx_pen_kb_dev_info *di = platform_get_drvdata(pdev);

	if (!di)
		return -ENODEV;

	power_event_bnc_unregister(POWER_BNT_WLTX_AUX, &di->event_nb);
	platform_set_drvdata(pdev, NULL);
	kfree(di);
	g_wltx_pen_kb_di = NULL;

	return 0;
}

static const struct of_device_id pen_kb_match_table[] = {
	{
		.compatible = "huawei, pen_kb",
		.data = NULL,
	},
	{},
};

static struct platform_driver pen_kb_driver = {
	.probe = pen_kb_probe,
	.remove = pen_kb_remove,
	.driver = {
		.name = "huawei, pen_kb",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(pen_kb_match_table),
	},
};

static int __init pen_kb_init(void)
{
	return platform_driver_register(&pen_kb_driver);
}

static void __exit pen_kb_exit(void)
{
	platform_driver_unregister(&pen_kb_driver);
}

device_initcall_sync(pen_kb_init);
module_exit(pen_kb_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("wireless pen kb module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");