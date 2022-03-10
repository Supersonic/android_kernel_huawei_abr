// SPDX-License-Identifier: GPL-2.0
/*
 * power_glink.c
 *
 * glink channel for power module
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
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/rpmsg.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/soc/qcom/pmic_glink.h>
#include <chipset_common/hwpower/charger/charger_common_interface.h>
#include <chipset_common/hwpower/common_module/power_dts.h>
#include <chipset_common/hwpower/common_module/power_event_ne.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <chipset_common/hwusb/hw_usb.h>
#include <huawei_platform/hwpower/common_module/power_glink.h>

#define HWLOG_TAG power_glink
HWLOG_REGIST();

#define POWER_GLINK_MSG_NAME              "oem_power"
#define POWER_GLINK_MSG_OWNER_ID          32782
#define POWER_GLINK_MSG_TYPE_REQ_RESP     1
#define POWER_GLINK_MSG_TYPE_NOTIFY       2
#define POWER_GLINK_MSG_OPCODE_GET        0x10000
#define POWER_GLINK_MSG_OPCODE_SET        0x10001
#define POWER_GLINK_MSG_OPCODE_NOTIFY     0x10002

#define POWER_GLINK_MSG_WAIT_MS           1000
#define POWER_GLINK_PROP_DATA_SIZE        16

#define ADSP_INIT_DONE                    1
#define POWER_GLINK_INIT_RETRY_CNT        10
#define POWER_GLINK_INIT_INVALID_DATA     0xFFFFFFFF

/* tx message data for get operation */
struct power_glink_get_tx_msg {
	struct pmic_glink_hdr hdr;
	u32 prop_id;
	u32 data_size;
};

/* rx message data for get operation */
struct power_glink_get_rx_msg {
	struct pmic_glink_hdr hdr;
	u32 prop_id;
	u32 data_buffer[POWER_GLINK_PROP_DATA_SIZE];
	u32 data_size;
};

/* tx message data for set operation */
struct power_glink_set_tx_msg {
	struct pmic_glink_hdr hdr;
	u32 prop_id;
	u32 data_buffer[POWER_GLINK_PROP_DATA_SIZE];
	u32 data_size;
};

/* rx message data for set operation */
struct power_glink_set_rx_msg {
	struct pmic_glink_hdr hdr;
	u32 prop_id;
	u32 ret_code;
};

/* rx message data for notify operation */
struct power_glink_notify_rx_msg {
	struct pmic_glink_hdr hdr;
	u32 notify_id;
	u32 notify_value;
};

struct power_glink_rx_data {
	u32 data_buffer[POWER_GLINK_PROP_DATA_SIZE];
	u32 data_size;
};

struct power_glink_cust_data {
	u32 msg_owner_id;
	u32 msg_type_req_resp;
	u32 msg_type_notify;
	u32 msg_opcode_get;
	u32 msg_opcode_set;
	u32 msg_opcode_notify;
};

struct power_glink_init_data {
	u32 jeita_fv;
	u32 charger_freq_reg;
};

struct power_glink_dev {
	struct device *dev;
	struct pmic_glink_client *client;
	struct mutex send_lock;
	struct completion msg_ack;
	struct work_struct init_work;
	struct power_glink_rx_data rx_data[POWER_GLINK_PROP_ID_END];
	struct power_glink_cust_data cust_data;
	struct power_glink_init_data init_data;
};

static struct power_glink_dev *g_power_glink_dev;

static int power_glink_check_prop_id(u32 prop_id)
{
	if (prop_id >= POWER_GLINK_PROP_ID_END) {
		hwlog_err("invalid prop_id=%u\n", prop_id);
		return -EINVAL;
	}

	return 0;
}

static int power_glink_check_data(u32 *data_buffer, u32 data_size)
{
	if (!data_buffer) {
		hwlog_err("invalid data_buffer\n");
		return -EINVAL;
	}

	if ((data_size == 0) || (data_size > POWER_GLINK_PROP_DATA_SIZE)) {
		hwlog_err("invalid data_size=%u\n", data_size);
		return -EINVAL;
	}

	return 0;
}

static int power_glink_send_message(struct power_glink_dev *l_dev,
	void *data, int len)
{
	int ret;

	mutex_lock(&l_dev->send_lock);
	reinit_completion(&l_dev->msg_ack);

	ret = pmic_glink_write(l_dev->client, data, len);
	if (ret) {
		mutex_unlock(&l_dev->send_lock);
		hwlog_err("send message error\n");
		return ret;
	}

	/* wait for to receive data when message send success */
	ret = wait_for_completion_timeout(&l_dev->msg_ack,
		msecs_to_jiffies(POWER_GLINK_MSG_WAIT_MS));
	if (!ret) {
		mutex_unlock(&l_dev->send_lock);
		hwlog_err("send message timeout\n");
		return -ETIMEDOUT;
	}

	mutex_unlock(&l_dev->send_lock);
	return 0;
}

static int power_glink_get_property(struct power_glink_dev *l_dev,
	u32 prop_id, u32 data_size)
{
	struct power_glink_get_tx_msg msg = { { 0 } };

	/* prepare header */
	msg.hdr.owner = l_dev->cust_data.msg_owner_id;
	msg.hdr.type = l_dev->cust_data.msg_type_req_resp;
	msg.hdr.opcode = l_dev->cust_data.msg_opcode_get;
	/* prepare data */
	msg.prop_id = prop_id;
	msg.data_size = data_size;

	hwlog_info("get_property: prop_id=%u data_size=%u\n", prop_id, data_size);

	return power_glink_send_message(l_dev, &msg, sizeof(msg));
}

static int power_glink_set_property(struct power_glink_dev *l_dev,
	u32 prop_id, u32 *data_buffer, u32 data_size)
{
	struct power_glink_set_tx_msg msg = { { 0 } };
	u32 i;

	/* prepare header */
	msg.hdr.owner = l_dev->cust_data.msg_owner_id;
	msg.hdr.type = l_dev->cust_data.msg_type_req_resp;
	msg.hdr.opcode = l_dev->cust_data.msg_opcode_set;
	/* prepare data */
	msg.prop_id = prop_id;
	for (i = 0; i < data_size; i++)
		msg.data_buffer[i] = data_buffer[i];
	msg.data_size = data_size;

	hwlog_info("set_property: prop_id=%u data_size=%u\n", prop_id, data_size);

	return power_glink_send_message(l_dev, &msg, sizeof(msg));
}

int power_glink_get_property_value(u32 prop_id, u32 *data_buffer, u32 data_size)
{
	struct power_glink_dev *l_dev = g_power_glink_dev;
	int ret;
	u32 i;

	if (!l_dev) {
		hwlog_err("l_dev is null\n");
		return -ENODEV;
	}

	if (power_glink_check_prop_id(prop_id))
		return -EINVAL;

	if (power_glink_check_data(data_buffer, data_size))
		return -EINVAL;

	ret = power_glink_get_property(l_dev, prop_id, data_size);
	if (ret < 0)
		return ret;

	if (data_size != l_dev->rx_data[prop_id].data_size) {
		hwlog_err("invalid rx data_size: prop_id=%u %u!=%u\n",
			prop_id, data_size, l_dev->rx_data[prop_id].data_size);
		return -EINVAL;
	}

	for (i = 0; i < data_size; i++)
		data_buffer[i] = l_dev->rx_data[prop_id].data_buffer[i];

	return 0;
}

int power_glink_set_property_value(u32 prop_id, u32 *data_buffer, u32 data_size)
{
	struct power_glink_dev *l_dev = g_power_glink_dev;

	if (!l_dev) {
		hwlog_err("l_dev is null\n");
		return -ENODEV;
	}

	if (power_glink_check_prop_id(prop_id))
		return -EINVAL;

	if (power_glink_check_data(data_buffer, data_size))
		return -EINVAL;

	return power_glink_set_property(l_dev, prop_id, data_buffer, data_size);
}

static void power_glink_handle_get_message(struct power_glink_dev *l_dev,
	void *data, size_t len)
{
	struct power_glink_get_rx_msg *msg = data;
	u32 prop_id;
	u32 data_size;
	u32 i;

	if (len != sizeof(*msg)) {
		hwlog_err("invalid msg len: %u!=%u\n", len, sizeof(*msg));
		return;
	}

	prop_id = msg->prop_id;
	data_size = msg->data_size;
	memset(&l_dev->rx_data[prop_id], 0, sizeof(struct power_glink_rx_data));
	l_dev->rx_data[prop_id].data_size = data_size;
	for (i = 0; i < data_size; i++)
		l_dev->rx_data[prop_id].data_buffer[i] = msg->data_buffer[i];

	hwlog_info("get_rx_msg: rx_data[%u][0]=%u data_size=%u\n",
		prop_id, l_dev->rx_data[prop_id].data_buffer[0], data_size);

	complete(&l_dev->msg_ack);
}

static void power_glink_handle_set_message(struct power_glink_dev *l_dev,
	void *data, size_t len)
{
	struct power_glink_set_rx_msg *msg = data;

	if (len != sizeof(*msg)) {
		hwlog_err("invalid msg len: %u!=%u\n", len, sizeof(*msg));
		return;
	}

	if (msg->ret_code) {
		hwlog_err("invalid msg ret: %u\n", msg->ret_code);
		return;
	}

	hwlog_info("set_rx_msg: prop_id=%u\n", msg->prop_id);

	complete(&l_dev->msg_ack);
}

static void power_glink_handle_notify_message(struct power_glink_dev *l_dev,
	void *data, size_t len)
{
	struct power_glink_notify_rx_msg *msg = data;

	if (len != sizeof(*msg)) {
		hwlog_err("invalid msg len: %u!=%u\n", len, sizeof(*msg));
		return;
	}

	hwlog_info("notify_rx_msg: notify_id=%u value=%u\n", msg->notify_id, msg->notify_value);
	power_glink_handle_usb_notify_message(msg->notify_id, msg->notify_value);
	power_glink_handle_charge_notify_message(msg->notify_id, msg->notify_value);
}

static int power_glink_msg_callback(void *dev_data, void *data, size_t len)
{
	struct pmic_glink_hdr *hdr = data;
	struct power_glink_dev *l_dev = dev_data;

	if (!l_dev || !hdr)
		return -ENODEV;

	hwlog_info("msg_callback: owner=%u type=%u opcode=%u len=%zu\n",
		hdr->owner, hdr->type, hdr->opcode, len);

	if (hdr->owner != l_dev->cust_data.msg_owner_id) {
		hwlog_err("invalid msg owner: %u!=%u\n", hdr->owner, l_dev->cust_data.msg_owner_id);
		return -EINVAL;
	}

	if (hdr->opcode == l_dev->cust_data.msg_opcode_get)
		power_glink_handle_get_message(l_dev, data, len);
	else if (hdr->opcode == l_dev->cust_data.msg_opcode_set)
		power_glink_handle_set_message(l_dev, data, len);
	else if (hdr->opcode == l_dev->cust_data.msg_opcode_notify)
		power_glink_handle_notify_message(l_dev, data, len);

	return 0;
}

static void power_glink_state_callback(void *dev_data, enum pmic_glink_state state)
{
	struct power_glink_dev *l_dev = dev_data;

	if (!l_dev)
		return;

	hwlog_info("state_callback: state=%d\n", state);
	hwlog_info("owner_id=%u type_req_resp=%u type_notify=%u get=%u set=%u notify=%u\n",
		l_dev->cust_data.msg_owner_id,
		l_dev->cust_data.msg_type_req_resp,
		l_dev->cust_data.msg_type_notify,
		l_dev->cust_data.msg_opcode_get,
		l_dev->cust_data.msg_opcode_set,
		l_dev->cust_data.msg_opcode_notify);
	if (state == PMIC_GLINK_STATE_UP)
		schedule_work(&l_dev->init_work);
}

static void power_glink_parse_dts(struct power_glink_dev *l_dev)
{
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"msg_owner_id", &l_dev->cust_data.msg_owner_id,
		POWER_GLINK_MSG_OWNER_ID);
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"msg_type_req_resp", &l_dev->cust_data.msg_type_req_resp,
		POWER_GLINK_MSG_TYPE_REQ_RESP);
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"msg_type_notify", &l_dev->cust_data.msg_type_notify,
		POWER_GLINK_MSG_TYPE_NOTIFY);
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"msg_opcode_get", &l_dev->cust_data.msg_opcode_get,
		POWER_GLINK_MSG_OPCODE_GET);
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"msg_opcode_set", &l_dev->cust_data.msg_opcode_set,
		POWER_GLINK_MSG_OPCODE_SET);
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"msg_opcode_notify", &l_dev->cust_data.msg_opcode_notify,
		POWER_GLINK_MSG_OPCODE_NOTIFY);
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"jeita_fv", &l_dev->init_data.jeita_fv,
		POWER_GLINK_INIT_INVALID_DATA);
	power_dts_read_u32(power_dts_tag(HWLOG_TAG), l_dev->dev->of_node,
		"charger_freq_reg", &l_dev->init_data.charger_freq_reg,
		POWER_GLINK_INIT_INVALID_DATA);
}

static void power_glink_init_work(struct work_struct *work)
{
	struct power_glink_dev *l_dev = g_power_glink_dev;
	u32 prop_id = POWER_GLINK_PROP_ID_GET_ADSP_INIT_STATUS;
	u32 status = 0;
	int data_status = -EINVAL;
	u32 i = 0;

	if (!l_dev) {
		hwlog_err("l_dev is null\n");
		return;
	}

	do {
		msleep(500); /* wait 500ms for adsp init done */
		power_glink_get_property_value(prop_id, &status, 1); /* 1:valid buff size */
		hwlog_info("adsp init status=%u\n", status);
		if (status == ADSP_INIT_DONE) {
			data_status = power_glink_set_property_value(POWER_GLINK_PROP_ID_SET_INIT_DATA,
				(u32 *)&l_dev->init_data, sizeof(struct power_glink_init_data) / sizeof(u32));
		}
		i++;
	} while ((data_status != 0) && (i < POWER_GLINK_INIT_RETRY_CNT));
}

static int power_glink_probe(struct platform_device *pdev)
{
	struct power_glink_dev *l_dev = NULL;
	struct pmic_glink_client_data client_data = { 0 };
	int ret = -EINVAL;

	if (!pdev || !pdev->dev.of_node)
		return -ENODEV;

	l_dev = devm_kzalloc(&pdev->dev, sizeof(*l_dev), GFP_KERNEL);
	if (!l_dev)
		return -ENOMEM;

	l_dev->dev = &pdev->dev;
	power_glink_parse_dts(l_dev);

	mutex_init(&l_dev->send_lock);
	init_completion(&l_dev->msg_ack);
	INIT_WORK(&l_dev->init_work, power_glink_init_work);

	client_data.id = l_dev->cust_data.msg_owner_id;
	client_data.name = POWER_GLINK_MSG_NAME;
	client_data.msg_cb = power_glink_msg_callback;
	client_data.priv = l_dev;
	client_data.state_cb = power_glink_state_callback;
	l_dev->client = pmic_glink_register_client(l_dev->dev, &client_data);
	if (IS_ERR(l_dev->client)) {
		hwlog_err("glink register fail\n");
		ret = -EPROBE_DEFER;
		goto fail_free_mem;
	}

	schedule_work(&l_dev->init_work);
	platform_set_drvdata(pdev, l_dev);
	g_power_glink_dev = l_dev;
	hwlog_info("probe ok\n");

	return 0;

fail_free_mem:
	mutex_destroy(&l_dev->send_lock);
	return ret;
}

static int power_glink_remove(struct platform_device *pdev)
{
	struct power_glink_dev *l_dev = platform_get_drvdata(pdev);

	if (!l_dev)
		return -ENODEV;

	mutex_destroy(&l_dev->send_lock);
	pmic_glink_unregister_client(l_dev->client);
	g_power_glink_dev = NULL;
	return 0;
}

static const struct of_device_id power_glink_match_table[] = {
	{
		.compatible = "huawei,power_glink",
		.data = NULL,
	},
	{},
};

static struct platform_driver power_glink_driver = {
	.probe = power_glink_probe,
	.remove = power_glink_remove,
	.driver = {
		.name = "huawei,power_glink",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(power_glink_match_table),
	},
};

int __init power_glink_init(void)
{
	return platform_driver_register(&power_glink_driver);
}

void __exit power_glink_exit(void)
{
	platform_driver_unregister(&power_glink_driver);
}

module_init(power_glink_init);
module_exit(power_glink_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("power glink module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
