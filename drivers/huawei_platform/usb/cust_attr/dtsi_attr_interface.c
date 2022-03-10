/*
  * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
  * Description: main file for dtsi attr api, send dtsi seting to adsp
  * Author: zhoujiaqiao
  * Create: 2021-05-19
  */

#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/soc/qcom/pmic_glink.h>

#include <huawei_platform/log/hw_log.h>

#include "cust_attr_glink.h"

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG dtsi_attr_interface
HWLOG_REGIST();

#define DTSI_REQ_PAYLOAD_SIZE    30

enum DTSI_ATTR_CMD_REQ {
	DTSI_CMD_READ_BOOL,
	DTSI_CMD_READ_U32,
	DTSI_CMD_READ_STRING,
};

/* ADSP TO AP */
struct dtsi_attr_req_msg {
	u32 dtsi_cmd;
	u8 compatible[DTSI_REQ_PAYLOAD_SIZE];
	u8 property[DTSI_REQ_PAYLOAD_SIZE];
};

struct dtsi_interface_priv {
	struct device *dev;
	struct cust_client *client;
};

static int dtsi_send_value(struct cust_client *client, int value)
{
	int rc;

	rc = cust_attr_send_data(client, &value, sizeof(value));
	if (rc < 0) {
		hwlog_err("failed to send value: %d\n", rc);
		return rc;
	}

	return rc;
}

static int dtsi_send_string(struct cust_client *client, const char *str)
{
	int rc;

	rc = cust_attr_send_data(client, str, strlen(str));
	if (rc < 0) {
		hwlog_err("failed to send strlen: %d\n", rc);
		return rc;
	}

	return rc;
}

static void dtsi_property_read(struct cust_client *client,
	struct device_node *np, struct dtsi_attr_req_msg *msg)
{
	int ret;
	int value;
	const char *string = NULL;

	switch (msg->dtsi_cmd) {
	case DTSI_CMD_READ_BOOL:
		value = of_property_read_bool(np, msg->property);
		hwlog_info("%s: read bool %d\n", __func__, value);
		dtsi_send_value(client, value);
		break;
	case DTSI_CMD_READ_U32:
		ret = of_property_read_u32(np, msg->property, &value);
		if (ret) {
			hwlog_err("%s: read u32 failed %d\n", __func__, ret);
			goto ERROR;
		}
		hwlog_info("%s: read u32 value %u\n", __func__, value);
		dtsi_send_value(client, value);
		break;
	case DTSI_CMD_READ_STRING:
		ret = of_property_read_string(np, msg->property, &string);
		if (ret) {
			hwlog_err("%s: read string failed %d\n", __func__, ret);
			goto ERROR;
		}
		hwlog_info("%s: read string %s\n", __func__, string);
		dtsi_send_string(client, string);
		break;
	default:
		goto ERROR;
	}
	return;

ERROR:
	cust_attr_send_err(client);
}

static void dtsi_property_get_result(struct cust_client *client,
	struct dtsi_attr_req_msg *msg)
{
	struct device_node *np = NULL;

	hwlog_info("%s: cmd=%d compatible=%s property=%s\n", __func__,
		msg->dtsi_cmd, msg->compatible, msg->property);

	np = of_find_compatible_node(NULL, NULL, msg->compatible);
	if (!np) {
		hwlog_err("%s: not found dts node\n", __func__);
		goto ERROR;
	}

	if (!of_device_is_available(np)) {
		hwlog_err("%s: dts %s not available\n", __func__, np->name);
		goto ERROR;
	}

	dtsi_property_read(client, np, msg);
	return;

ERROR:
	cust_attr_send_err(client);
}

static int dtsi_interface_notify(void *priv, void *data, size_t len)
{
	struct dtsi_interface_priv *dtsi_priv = priv;
	struct dtsi_attr_req_msg *msg = data;

	if (!dtsi_priv || !msg)
		return -EINVAL;

	if (len != sizeof(*msg)) {
		hwlog_err("invalid msg len: %u!=%u\n", len, sizeof(*msg));
		return -EINVAL;
	}

	dtsi_property_get_result(dtsi_priv->client, msg);

	return 0;
}

static void dtsi_interface_register(void *priv)
{
	struct dtsi_interface_priv *dtsi_priv = priv;
	struct cust_client_data cd = {
		.callback = &dtsi_interface_notify,
	};

	cd.cust_id = CUST_ARRT_OPCODE_DTSI;
	cd.priv = dtsi_priv;

	dtsi_priv->client = cust_register_client(dtsi_priv->dev, &cd);
	if (IS_ERR_OR_NULL(dtsi_priv->client))
		hwlog_err("failed to register as client: %d\n",
			PTR_ERR(dtsi_priv->client));
	else
		hwlog_info("%s success\n", __func__);
}

static int dtsi_attr_probe(struct platform_device *pdev)
{
	int rc;
	struct dtsi_interface_priv *dtsi_attr = NULL;
	struct device *dev = &pdev->dev;

	dtsi_attr = devm_kzalloc(dev, sizeof(*dtsi_attr), GFP_KERNEL);
	if (!dtsi_attr)
		return -ENOMEM;

	dtsi_attr->dev = dev;

	rc = cust_register_notifier(dev, dtsi_interface_register, dtsi_attr);
	if (rc) {
		dev_err(dev, "failed to register cust notifier: %d\n", rc);
		return rc;
	}

	platform_set_drvdata(pdev, dtsi_attr);
	return 0;
}

static int dtsi_attr_remove(struct platform_device *pdev)
{
	int rc;
	struct dtsi_interface_priv *dtsi_attr = platform_get_drvdata(pdev);

	rc = cust_deregister_client(dtsi_attr->client);
	if (rc) {
		dev_err(dtsi_attr->dev, "cust client deregister error: %d\n", rc);
		return rc;
	}

	rc = cust_deregister_notifier(dtsi_attr->dev, dtsi_attr);
	if (rc) {
		dev_err(dtsi_attr->dev, "cust notifier deregister error: %d\n", rc);
		return rc;
	}
	return rc;
}

static const struct of_device_id dtsi_attr_match_table[] = {
	{ .compatible = "huawei,dtsi-attr-interface" },
	{},
};

static struct platform_driver dtsi_attr_driver = {
	.driver = {
		.name = "dtsi_attr_interface",
		.of_match_table = dtsi_attr_match_table,
	},
	.probe = dtsi_attr_probe,
	.remove = dtsi_attr_remove,
};
module_platform_driver(dtsi_attr_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei dtsi attr driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");

