// SPDX-License-Identifier: GPL-2.0
/*
 * wireless_lightstrap.c
 *
 * wireless lightstrap driver
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

#include <chipset_common/hwpower/accessory/wireless_lightstrap.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <chipset_common/hwpower/common_module/power_sysfs.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_dsm.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_supply_application.h>
#include <chipset_common/hwpower/protocol/wireless_protocol.h>
#include <chipset_common/hwpower/protocol/wireless_protocol_qi.h>
#include <chipset_common/hwpower/wireless_charge/wireless_rx_status.h>
#include <chipset_common/hwpower/wireless_charge/wireless_tx_ic_intf.h>
#include <chipset_common/hwsensor/hall/hall_interface.h>
#include <huawei_platform/power/wireless/wireless_transmitter.h>

#define HWLOG_TAG lightstrap
HWLOG_REGIST();

static struct lightstrap_di *g_lightstrap_di;

static char *g_lightstrap_model_id_table[LIGHTSTRAP_MODEL_ID_END] = {
	[LIGHTSTRAP_VER_ONE_MODEL_ID] = "296",
	[LIGHTSTRAP_VER_TWO_MODEL_ID] = "21Z6",
};

bool lightstrap_online_state(void)
{
	struct lightstrap_di *di = g_lightstrap_di;

	if (!di)
		return false;

	if (di->product_type == LIGHTSTRAP_PRODUCT_TYPE)
		return true;

	if ((wlrx_get_wireless_channel_state() == WIRELESS_CHANNEL_ON) &&
		hall_interface_get_hall_status())
		return true;

	return false;
}

enum wltx_pwr_src lightstrap_specify_pwr_src(void)
{
	return PWR_SRC_5VBST;
}

static bool lightstrap_can_do_reverse_charging(void)
{
	int soc = power_supply_app_get_bat_capacity();

	if (soc > WL_TX_SOC_MIN)
		return true;

	if (!lightstrap_online_state())
		return true;

	if (wlrx_get_wired_channel_state() != WIRED_CHANNEL_OFF)
		return true;

	wireless_tx_set_tx_status(WL_TX_STATUS_TX_CLOSE);
	hwlog_info("lightstrap can not do reverse charging\n");

	return false;
}

void lightstrap_reinit_tx_chip(void)
{
	struct lightstrap_di *di = g_lightstrap_di;

	if (!di)
		return;

	(void)wltx_ic_set_ping_freq(WLTRX_IC_MAIN, di->ping_freq);
	(void)wltx_ic_set_min_fop(WLTRX_IC_MAIN, di->work_freq);
	(void)wltx_ic_set_max_fop(WLTRX_IC_MAIN, di->work_freq);
}

static void lightstrap_send_light_up_msg(void)
{
	unsigned char msg = HWQI_PARA_LIGHTSTRAP_LIGHT_UP_MSG;
	int ret;

	hwlog_info("send_light_up_msg: %02x\n", msg);
	ret = wireless_send_lightstrap_ctrl_msg(WLTRX_IC_MAIN,
		WIRELESS_PROTOCOL_QI, &msg, LIGHTSTRAP_LIGHT_UP_MSG_LEN);
	if (ret)
		hwlog_err("send_light_up_msg fail\n");
	else
		hwlog_info("send_light_up_msg success\n");
}

static void lightstrap_light_up(void)
{
	hwlog_info("lightstrap light up\n");
	wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, true);
	msleep(LIGHTSTRAP_LIGHT_UP_DELAY);
	lightstrap_send_light_up_msg();
}

static void lightstrap_control_work(struct work_struct *work)
{
	struct lightstrap_di *di = container_of(work, struct lightstrap_di,
		control_work.work);

	if (!di)
		return;

	if (!lightstrap_online_state())
		return;

	switch (di->sysfs_val) {
	case LIGHTSTRAP_POWER_OFF:
		hwlog_info("lightstrap power off\n");
		wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, false);
		break;
	case LIGHTSTRAP_POWER_ON:
		hwlog_info("lightstrap power on\n");
		wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, true);
		break;
	case LIGHTSTRAP_LIGHT_UP:
		lightstrap_light_up();
		break;
	default:
		break;
	}
}

static int lightstrap_check_version(struct lightstrap_di *di)
{
	if (di->product_type != LIGHTSTRAP_PRODUCT_TYPE)
		return LIGHTSTRAP_NON;

	switch (di->product_id) {
	case LIGHTSTRAP_VER_ONE_ID:
		return LIGHTSTRAP_VERSION_ONE;
	case LIGHTSTRAP_VER_TWO_ID:
		return LIGHTSTRAP_VERSION_TWO;
	default:
		return LIGHTSTRAP_NON;
	}
}

static void lightstrap_set_model_id(struct lightstrap_di *di)
{
	switch (di->product_id) {
	case LIGHTSTRAP_VER_ONE_ID:
		di->model_id = g_lightstrap_model_id_table[LIGHTSTRAP_VER_ONE_MODEL_ID];
		break;
	case LIGHTSTRAP_VER_TWO_ID:
		di->model_id = g_lightstrap_model_id_table[LIGHTSTRAP_VER_TWO_MODEL_ID];
		break;
	default:
		return;
	}
	hwlog_info("model_id=%s\n", di->model_id);
}

static void lightstrap_report_dmd(enum lightstrap_dmd_type type)
{
	char buff[POWER_DSM_BUF_SIZE_0128] = {0};

	switch (type) {
	case LIGHTSTRAP_ATTACH_DMD:
		snprintf(buff, sizeof(buff), "lightstrap_attach\n");
		power_dsm_report_dmd(POWER_DSM_LIGHTSTRAP, DSM_LIGHTSTRAP_STATUS, buff);
		break;
	case LIGHTSTRAP_DETACH_DMD:
		snprintf(buff, sizeof(buff), "lightstrap_detach\n");
		power_dsm_report_dmd(POWER_DSM_LIGHTSTRAP, DSM_LIGHTSTRAP_STATUS, buff);
		break;
	default:
		break;
	}
}

static void lightstrap_send_on_uevent(struct lightstrap_di *di)
{
	char *envp[LIGHTSTRAP_MAX_RX_SIZE] = {
		"LIGHTSTRAPCASE=ON",
		"RXID=07",
		NULL,
		"SUBMODELID=0",
		NULL,
		NULL
	};

	envp[LIGHTSTRAP_ENVP_OFFSET2] = kzalloc(LIGHTSTRAP_INFO_LEN, GFP_KERNEL);
	if (!envp[LIGHTSTRAP_ENVP_OFFSET2])
		return;

	envp[LIGHTSTRAP_ENVP_OFFSET4] = kzalloc(LIGHTSTRAP_INFO_LEN, GFP_KERNEL);
	if (!envp[LIGHTSTRAP_ENVP_OFFSET4])
		goto alloc_fail;

	snprintf(envp[LIGHTSTRAP_ENVP_OFFSET2], LIGHTSTRAP_INFO_LEN, "MODELID=%s",
		di->model_id);
	snprintf(envp[LIGHTSTRAP_ENVP_OFFSET4], LIGHTSTRAP_INFO_LEN, "PRODUCTID=%02x",
		di->product_id);
	kobject_uevent_env(&di->dev->kobj, KOBJ_CHANGE, envp);
	hwlog_info("lightstrap send case=on uevent\n");
	kfree(envp[LIGHTSTRAP_ENVP_OFFSET4]);

alloc_fail:
	kfree(envp[LIGHTSTRAP_ENVP_OFFSET2]);
}

static void lightstrap_send_off_uevent(struct lightstrap_di *di)
{
	char *envp[LIGHTSTRAP_MAX_RX_SIZE] = {
		"LIGHTSTRAPCASE=OFF",
		"RXID=00",
		NULL,
		"SUBMODELID=0",
		"PRODUCTID=00",
		NULL
	};

	envp[LIGHTSTRAP_ENVP_OFFSET2] = kzalloc(LIGHTSTRAP_INFO_LEN, GFP_KERNEL);
	if (!envp[LIGHTSTRAP_ENVP_OFFSET2])
		return;

	snprintf(envp[LIGHTSTRAP_ENVP_OFFSET2], LIGHTSTRAP_INFO_LEN, "MODELID=%s",
		di->model_id);
	kobject_uevent_env(&di->dev->kobj, KOBJ_CHANGE, envp);
	hwlog_info("lightstrap send case=off uevent\n");
	kfree(envp[LIGHTSTRAP_ENVP_OFFSET2]);
}

static void lightstrap_set_status(struct lightstrap_di *di, unsigned int status)
{
	mutex_lock(&di->lock);
	di->status = status;
	mutex_unlock(&di->lock);
	hwlog_info("set lightstrap status : %d\n", di->status);
}

static void lightstrap_check_work(struct work_struct *work)
{
	struct lightstrap_di *di = container_of(work, struct lightstrap_di,
		check_work.work);

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	if (di->is_opened_by_hall) {
		wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, false);
		di->is_opened_by_hall = false;
		lightstrap_set_status(di, LIGHTSTRAP_STATUS_INIT);
	}
}

static void lightstrap_tx_ping_work(struct work_struct *work)
{
	struct lightstrap_di *di = container_of(work, struct lightstrap_di,
		tx_ping_work.work);

	if (!di) {
		hwlog_err("di is null\n");
		return;
	}

	di->tx_status_ping = false;
	lightstrap_set_status(di, LIGHTSTRAP_STATUS_INIT);
	if (wireless_tx_get_tx_status() == WL_TX_STATUS_PING)
		(void)wltx_ic_set_ping_freq(WLTRX_IC_MAIN, LIGHTSTRAP_RESET_TX_PING_FREQ);
}

static int lightstrap_parse_product_info(struct lightstrap_di *di, void *data)
{
	u8 product_type;

	if (!di || !data)
		return -EINVAL;

	product_type = ((u8 *)data)[0];
	if (product_type != LIGHTSTRAP_PRODUCT_TYPE)
		return -EINVAL;

	di->product_type = product_type;
	di->product_id = ((u8 *)data)[1];
	lightstrap_set_model_id(di);
	hwlog_info("product_type=%02x, product_id=%02x\n", di->product_type,
		di->product_id);

	return 0;
}

static void lightstrap_close_wltx(struct lightstrap_di *di)
{
	if (di->is_opened_by_hall) {
		cancel_delayed_work(&di->check_work);
		di->is_opened_by_hall = false;
	}

	if (di->tx_status_ping) {
		cancel_delayed_work(&di->tx_ping_work);
		di->tx_status_ping = false;
	}

	wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, false);
}

/* handle wlrx disconnect event on status: waiting wlrx end */
static void lightstrap_handle_wlrx_disconnect_on_wwe(struct lightstrap_di *di)
{
	if (!hall_interface_get_hall_status()) {
		lightstrap_set_status(di, LIGHTSTRAP_STATUS_INIT);
		return;
	}

	di->is_opened_by_hall = true;
	wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, true);
	schedule_delayed_work(&di->check_work, msecs_to_jiffies(LIGHTSTRAP_TIMEOUT));
	lightstrap_set_status(di, LIGHTSTRAP_STATUS_WPI);
}

/* handle wlrx connect event on status: waiting product info */
static void lightstrap_handle_wlrx_connect_on_wpi(struct lightstrap_di *di)
{
	if (di->is_opened_by_hall) {
		cancel_delayed_work(&di->check_work);
		di->is_opened_by_hall = false;
		return;
	}

	if (di->tx_status_ping) {
		cancel_delayed_work(&di->tx_ping_work);
		di->tx_status_ping = false;
	}
}

/* handle hall off event on status: waiting product info */
static void lightstrap_handle_hall_off_on_wpi(struct lightstrap_di *di)
{
	if (di->is_opened_by_hall) {
		cancel_delayed_work(&di->check_work);
		di->is_opened_by_hall = false;
		wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, false);
		return;
	}

	if (di->tx_status_ping) {
		cancel_delayed_work(&di->tx_ping_work);
		di->tx_status_ping = false;
		(void)wltx_ic_set_ping_freq(WLTRX_IC_MAIN, LIGHTSTRAP_RESET_TX_PING_FREQ);
	}
}

/* handle hall approach event on status: initialized */
static void lightstrap_handle_hall_approach_on_init(struct lightstrap_di *di)
{
	int tx_status = wireless_tx_get_tx_status();

	switch (tx_status) {
	case WL_TX_STATUS_PING_SUCC:
	case WL_TX_STATUS_IN_CHARGING:
		break;
	case WL_TX_STATUS_PING:
		di->tx_status_ping = true;
		(void)wltx_ic_set_ping_freq(WLTRX_IC_MAIN, di->ping_freq);
		schedule_delayed_work(&di->tx_ping_work, msecs_to_jiffies(LIGHTSTRAP_TIMEOUT));
		lightstrap_set_status(di, LIGHTSTRAP_STATUS_WPI);
		break;
	default:
		di->is_opened_by_hall = true;
		wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, true);
		schedule_delayed_work(&di->check_work, msecs_to_jiffies(LIGHTSTRAP_TIMEOUT));
		lightstrap_set_status(di, LIGHTSTRAP_STATUS_WPI);
		break;
	}
}

static void lightstrap_process_product_info_event(struct lightstrap_di *di)
{
	switch (di->status) {
	case LIGHTSTRAP_STATUS_WPI:
		lightstrap_close_wltx(di);
		msleep(LIGHTSTRAP_UEVENT_DELAY);
		break;
	case LIGHTSTRAP_STATUS_INIT:
	case LIGHTSTRAP_STATUS_WWE:
	case LIGHTSTRAP_STATUS_DEV:
	default:
		break;
	}

	if (!di->uevent_flag) {
		lightstrap_send_on_uevent(di);
		lightstrap_report_dmd(LIGHTSTRAP_ATTACH_DMD);
		di->uevent_flag = true;
	}

	lightstrap_set_status(di, LIGHTSTRAP_STATUS_DEV);
}

static void lightstrap_process_wlrx_disconnect_event(struct lightstrap_di *di)
{
	switch (di->status) {
	case LIGHTSTRAP_STATUS_WWE:
		lightstrap_handle_wlrx_disconnect_on_wwe(di);
		break;
	case LIGHTSTRAP_STATUS_INIT:
	case LIGHTSTRAP_STATUS_WPI:
	case LIGHTSTRAP_STATUS_DEV:
	default:
		break;
	}
}

static void lightstrap_process_wlrx_connect_event(struct lightstrap_di *di)
{
	switch (di->status) {
	case LIGHTSTRAP_STATUS_INIT:
		lightstrap_set_status(di, LIGHTSTRAP_STATUS_WWE);
		break;
	case LIGHTSTRAP_STATUS_WWE:
		break;
	case LIGHTSTRAP_STATUS_WPI:
		lightstrap_handle_wlrx_connect_on_wpi(di);
		lightstrap_set_status(di, LIGHTSTRAP_STATUS_WWE);
		break;
	case LIGHTSTRAP_STATUS_DEV:
		break;
	default:
		break;
	}
}

static void lightstrap_process_hall_off_event(struct lightstrap_di *di)
{
	di->uevent_flag = false;
	di->product_type = LIGHTSTRAP_OFF;
	di->product_id = LIGHTSTRAP_OFF;

	switch (di->status) {
	case LIGHTSTRAP_STATUS_INIT:
		break;
	case LIGHTSTRAP_STATUS_WWE:
		break;
	case LIGHTSTRAP_STATUS_WPI:
		lightstrap_handle_hall_off_on_wpi(di);
		break;
	case LIGHTSTRAP_STATUS_DEV:
		lightstrap_send_off_uevent(di);
		lightstrap_report_dmd(LIGHTSTRAP_DETACH_DMD);
		break;
	default:
		break;
	}

	lightstrap_set_status(di, LIGHTSTRAP_STATUS_INIT);
}

static void lightstrap_process_hall_approach_event(struct lightstrap_di *di)
{
	switch (di->status) {
	case LIGHTSTRAP_STATUS_INIT:
		lightstrap_handle_hall_approach_on_init(di);
		break;
	case LIGHTSTRAP_STATUS_WWE:
	case LIGHTSTRAP_STATUS_WPI:
	case LIGHTSTRAP_STATUS_DEV:
	default:
		break;
	}
}

static void lightstrap_init_identify_work(struct work_struct *work)
{
	struct lightstrap_di *di = container_of(work, struct lightstrap_di,
		identify_work.work);
	int i;

	if (!di)
		return;

	for (i = 0; i < di->retry_times; i++) {
		hwlog_info("init identify retry times : %d\n", i);
		if (lightstrap_online_state())
			break;

		if (hall_interface_get_hall_status())
			lightstrap_process_hall_approach_event(di);

		msleep(LIGHTSTRAP_INIT_IDENTIFY_DELAY);
	}
}

static void lightstrap_event_work(struct work_struct *work)
{
	struct lightstrap_di *di = container_of(work, struct lightstrap_di,
		event_work);

	if (!di)
		return;

	switch (di->event_type) {
	case POWER_NE_LIGHTSTRAP_ON:
		lightstrap_process_hall_approach_event(di);
		break;
	case POWER_NE_LIGHTSTRAP_OFF:
		lightstrap_process_hall_off_event(di);
		break;
	case POWER_NE_WIRELESS_CONNECT:
		lightstrap_process_wlrx_connect_event(di);
		break;
	case POWER_NE_WIRELESS_DISCONNECT:
		lightstrap_process_wlrx_disconnect_event(di);
		break;
	case POWER_NE_LIGHTSTRAP_GET_PRODUCT_INFO:
		lightstrap_process_product_info_event(di);
		break;
	case POWER_NE_LIGHTSTRAP_EPT:
		wltx_open_tx(WLTX_CLIENT_LIGHTSTRAP, false);
		break;
	default:
		break;
	}
}

static int lightstrap_event_notifier_call(struct notifier_block *nb,
	unsigned long event, void *data)
{
	struct lightstrap_di *di = container_of(nb, struct lightstrap_di, event_nb);

	if (!di)
		return NOTIFY_OK;

	switch (event) {
	case POWER_NE_LIGHTSTRAP_ON:
		break;
	case POWER_NE_LIGHTSTRAP_OFF:
		break;
	case POWER_NE_WIRELESS_CONNECT:
		break;
	case POWER_NE_WIRELESS_DISCONNECT:
		break;
	case POWER_NE_LIGHTSTRAP_GET_PRODUCT_INFO:
		if (lightstrap_parse_product_info(di, data))
			return NOTIFY_OK;

		break;
	case POWER_NE_LIGHTSTRAP_EPT:
		break;
	default:
		return NOTIFY_OK;
	}

	mutex_lock(&di->lock);
	di->event_type = event;
	mutex_unlock(&di->lock);
	schedule_work(&di->event_work);

	return NOTIFY_OK;
}

static struct wltx_logic_ops g_lightstrap_logic_ops = {
	.type = WLTX_CLIENT_LIGHTSTRAP,
	.can_rev_charge_check = lightstrap_can_do_reverse_charging,
	.need_specify_pwr_src = lightstrap_online_state,
	.specify_pwr_src = lightstrap_specify_pwr_src,
	.reinit_tx_chip = lightstrap_reinit_tx_chip,
};

#ifdef CONFIG_SYSFS
static ssize_t lightstrap_sysfs_show(struct device *dev,
	struct device_attribute *attr, char *buf);
static ssize_t lightstrap_sysfs_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count);

static struct power_sysfs_attr_info lightstrap_sysfs_field_tbl[] = {
	power_sysfs_attr_wo(lightstrap, 0220, LIGHTSTRAP_SYSFS_CONTROL, control),
	power_sysfs_attr_ro(lightstrap, 0440, LIGHTSTRAP_SYSFS_CHECK_VERSION, check_ver),
};

#define LIGHTSTRAP_SYSFS_ATTRS_SIZE ARRAY_SIZE(lightstrap_sysfs_field_tbl)

static struct attribute *lightstrap_sysfs_attrs[LIGHTSTRAP_SYSFS_ATTRS_SIZE + 1];
static const struct attribute_group lightstrap_sysfs_attr_group = {
	.attrs = lightstrap_sysfs_attrs,
};

static ssize_t lightstrap_sysfs_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct power_sysfs_attr_info *info = NULL;
	struct lightstrap_di *di = g_lightstrap_di;

	info = power_sysfs_lookup_attr(attr->attr.name,
		lightstrap_sysfs_field_tbl, LIGHTSTRAP_SYSFS_ATTRS_SIZE);
	if (!info || !di)
		return -EINVAL;

	switch (info->name) {
	case LIGHTSTRAP_SYSFS_CHECK_VERSION:
		return snprintf(buf, PAGE_SIZE, "%u\n", lightstrap_check_version(di));
	default:
		return 0;
	}
}

static ssize_t lightstrap_sysfs_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct power_sysfs_attr_info *info = NULL;
	struct lightstrap_di *di = g_lightstrap_di;
	long val = 0;

	info = power_sysfs_lookup_attr(attr->attr.name,
		lightstrap_sysfs_field_tbl, LIGHTSTRAP_SYSFS_ATTRS_SIZE);
	if (!info || !di)
		return -EINVAL;

	switch (info->name) {
	case LIGHTSTRAP_SYSFS_CONTROL:
		if (kstrtol(buf, POWER_BASE_DEC, &val) < 0)
			return -EINVAL;

		di->sysfs_val = val;
		schedule_delayed_work(&di->control_work, msecs_to_jiffies(0));
		break;
	default:
		break;
	}

	return count;
}

static struct device *lightstrap_sysfs_create_group(void)
{
	power_sysfs_init_attrs(lightstrap_sysfs_attrs,
		lightstrap_sysfs_field_tbl, LIGHTSTRAP_SYSFS_ATTRS_SIZE);

	return power_sysfs_create_group("hw_power", "lightstrap",
		&lightstrap_sysfs_attr_group);
}

static void lightstrap_sysfs_remove_group(struct device *dev)
{
	power_sysfs_remove_group(dev, &lightstrap_sysfs_attr_group);
}
#else
static inline struct device *lightstrap_sysfs_create_group(void)
{
	return NULL;
}

static inline void lightstrap_sysfs_remove_group(struct device *dev)
{
}
#endif /* CONFIG_SYSFS */

static void lightstrap_parse_dts(struct device_node *np,
	struct lightstrap_di *di)
{
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np, "tx_ping_freq",
		&di->ping_freq, LIGHTSTRAP_PING_FREQ_DEFAULT);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np, "tx_work_freq",
		&di->work_freq, LIGHTSTRAP_WORK_FREQ_DEFAULT);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np, "retry_times",
		&di->retry_times, LIGHTSTRAP_RETRY_TIMES_DEFAULT);
}

static int lightstrap_probe(struct platform_device *pdev)
{
	int ret;
	struct lightstrap_di *di = NULL;
	struct device_node *np = NULL;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	g_lightstrap_di = di;
	di->dev = &pdev->dev;
	np = di->dev->of_node;

	lightstrap_parse_dts(np, di);
	INIT_WORK(&di->event_work, lightstrap_event_work);
	INIT_DELAYED_WORK(&di->check_work, lightstrap_check_work);
	INIT_DELAYED_WORK(&di->tx_ping_work, lightstrap_tx_ping_work);
	INIT_DELAYED_WORK(&di->control_work, lightstrap_control_work);
	INIT_DELAYED_WORK(&di->identify_work, lightstrap_init_identify_work);
	mutex_init(&di->lock);
	platform_set_drvdata(pdev, di);
	di->event_nb.notifier_call = lightstrap_event_notifier_call;
	ret = power_event_bnc_register(POWER_BNT_LIGHTSTRAP, &di->event_nb);
	if (ret)
		goto lightstrap_notifier_regist_fail;

	ret = power_event_bnc_register(POWER_BNT_CONNECT, &di->event_nb);
	if (ret)
		goto connect_notifier_regist_fail;

	ret = wireless_tx_logic_ops_register(&g_lightstrap_logic_ops);
	if (ret)
		goto logic_ops_regist_fail;

	di->dev = lightstrap_sysfs_create_group();
	lightstrap_set_status(di, LIGHTSTRAP_STATUS_INIT);
	schedule_delayed_work(&di->identify_work, msecs_to_jiffies(0));

	return 0;

logic_ops_regist_fail:
	(void)power_event_bnc_unregister(POWER_BNT_CONNECT, &di->event_nb);
connect_notifier_regist_fail:
	(void)power_event_bnc_unregister(POWER_BNT_LIGHTSTRAP, &di->event_nb);
lightstrap_notifier_regist_fail:
	platform_set_drvdata(pdev, NULL);
	mutex_destroy(&di->lock);
	kfree(di);
	g_lightstrap_di = NULL;

	return ret;
}

static int lightstrap_remove(struct platform_device *pdev)
{
	struct lightstrap_di *di = platform_get_drvdata(pdev);

	if (!di)
		return 0;

	lightstrap_sysfs_remove_group(di->dev);
	power_event_bnc_unregister(POWER_BNT_LIGHTSTRAP, &di->event_nb);
	platform_set_drvdata(pdev, NULL);
	mutex_destroy(&di->lock);
	kfree(di);
	g_lightstrap_di = NULL;

	return 0;
}

static const struct of_device_id lightstrap_match_table[] = {
	{
		.compatible = "huawei,lightstrap",
		.data = NULL,
	},
	{},
};

static struct platform_driver lightstrap_driver = {
	.probe = lightstrap_probe,
	.remove = lightstrap_remove,
	.driver = {
		.name = "huawei,lightstrap",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(lightstrap_match_table),
	},
};

static int __init lightstrap_init(void)
{
	return platform_driver_register(&lightstrap_driver);
}

static void __exit lightstrap_exit(void)
{
	platform_driver_unregister(&lightstrap_driver);
}

device_initcall_sync(lightstrap_init);
module_exit(lightstrap_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("wireless lightstrap module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
