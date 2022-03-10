/*
  * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
  * Description: main file for cust attr glink
  * Author: zhoujiaqiao
  * Create: 2021-05-19
  */

#include "cust_attr_glink.h"

#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/soc/qcom/pmic_glink.h>

#include <securec.h>

#include <huawei_platform/log/hw_log.h>

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG cust_attr_glink
HWLOG_REGIST();

#define MSG_OWNER_CUST_ARRT_ID      32788
#define MSG_TYPE_RESP               1

#define CUST_ARRT_REQ_PAYLOAD_SIZE       64
#define CUST_ARRT_RESQ_BUFFER_SIZE       32
#define CUST_ARRT_WAIT_MS                1000
#define CUST_ATTR_CMD_WRITE_RESQ         0x0018

/* ADSP TO AP */
struct cust_attr_req_msg {
	struct pmic_glink_hdr hdr;
	u8 payload[CUST_ARRT_REQ_PAYLOAD_SIZE];
	u32 payload_size;
};

/* AP TO ADSP */
struct cust_attr_resp_msg {
	struct pmic_glink_hdr hdr;
	u32 status;
	u8 buf[CUST_ARRT_RESQ_BUFFER_SIZE];
	u32 buf_size;
};

struct cust_attr_dev {
	struct device *dev;
	struct pmic_glink_client *pgclient;
	struct idr client_idr;
	struct list_head client_list;
	struct mutex client_lock;
};

struct cust_client {
	struct cust_attr_dev *cadev;
	struct cust_client_data data;
	struct list_head c_node;
	struct work_struct client_cb_work;
	u8 msg[CUST_ARRT_REQ_PAYLOAD_SIZE];
};

struct probe_notify_node {
	struct list_head node;
	struct device_node *cadev_node;
	void (*cb)(void *priv);
	void *priv;
};

static LIST_HEAD(probe_notify_list);
static DEFINE_MUTEX(notify_lock);

static int cust_attr_write(struct cust_attr_dev *dev, void *data,
	size_t len)
{
	int rc;

	rc = pmic_glink_write(dev->pgclient, data, len);
	if (rc)
		hwlog_err("Error in sending message: %d\n", rc);

	return rc;
}

static int __cust_attr_send_data(struct cust_attr_dev *dev, const void *data,
	size_t len)
{
	struct cust_attr_resp_msg msg = { { 0 } };
	int ret;

	if (len > sizeof(msg.buf)) {
		hwlog_err("len %zu exceeds msg buf's size: %zu\n",
			len, sizeof(msg.buf));
		return -EINVAL;
	}

	msg.hdr.owner = MSG_OWNER_CUST_ARRT_ID;
	msg.hdr.type = MSG_TYPE_RESP;
	msg.hdr.opcode = CUST_ATTR_CMD_WRITE_RESQ;

	ret = memcpy_s(msg.buf, sizeof(msg.buf), data, len);
	if (ret != 0) {
		hwlog_err("%s:%d:memcpy_s copy failed.\n", __func__, __LINE__);
		msg.status = EINVAL;
	}

	msg.buf_size = len;

	return cust_attr_write(dev, &msg, sizeof(msg));
}

int cust_attr_send_data(struct cust_client *client, const void *data,
	size_t len)
{
	struct cust_attr_dev *cadev = NULL;

	if (!client || !data)
		return -EINVAL;

	cadev = client->cadev;
	return __cust_attr_send_data(cadev, data, len);
}
EXPORT_SYMBOL(cust_attr_send_data);

int cust_attr_send_err(struct cust_client *client)
{
	struct cust_attr_dev *cadev = NULL;
	struct cust_attr_resp_msg msg = { { 0 } };

	if (!client)
		return -EINVAL;

	cadev = client->cadev;
	msg.hdr.owner = MSG_OWNER_CUST_ARRT_ID;
	msg.hdr.type = MSG_TYPE_RESP;
	msg.hdr.opcode = CUST_ATTR_CMD_WRITE_RESQ;
	msg.status = EINVAL;

	return cust_attr_write(cadev, &msg, sizeof(msg));
}
EXPORT_SYMBOL(cust_attr_send_err);

static int cust_attr_callback(void *priv, void *data, size_t len)
{
	struct cust_attr_req_msg *msg = data;
	struct cust_attr_dev *dev = priv;
	struct cust_client *client = NULL;
	int ret;

	if (!dev || !msg)
		return -EINVAL;

	hwlog_info("%s, len: %zu owner: %u type: %u opcode %04x\n", __func__,
		len, msg->hdr.owner, msg->hdr.type, msg->hdr.opcode);

	if (msg->hdr.owner != MSG_OWNER_CUST_ARRT_ID) {
		hwlog_err("invalid msg owner: %u\n", msg->hdr.owner);
		return -EINVAL;
	}

	if (msg->payload_size > sizeof(msg->payload)) {
		hwlog_err("len %zu exceeds msg payload size: %zu\n", msg->payload_size,
			sizeof(msg->payload));
		return -EINVAL;
	}

	mutex_lock(&dev->client_lock);
	client = idr_find(&dev->client_idr, msg->hdr.opcode);
	mutex_unlock(&dev->client_lock);
	if (!client) {
		hwlog_err("No client associated with ID %d\n", msg->hdr.opcode);
		return -EINVAL;
	}

	cancel_work_sync(&client->client_cb_work);
	ret = memcpy_s(&client->msg, sizeof(client->msg), msg->payload,
		msg->payload_size);
	if (ret) {
		hwlog_err("%s:%d:memcpy_s failed.\n", __func__, __LINE__);
		return ret;
	}
	schedule_work(&client->client_cb_work);

	return 0;
}

static struct device_node *to_cust_device_node(struct device *client_dev)
{
	int rc;
	struct of_phandle_args pargs;

	rc = of_parse_phandle_with_args(client_dev->of_node,
		"huawei,cust_attr_dev", "#cust-cells", 0, &pargs);
	if (rc) {
		dev_err(client_dev, "Error huawei,cust_attr_dev property: %d\n", rc);
		return ERR_PTR(rc);
	}

	if (pargs.args_count != 1) {
		dev_err(client_dev, "Error in port_index specification\n");
		return ERR_PTR(-EINVAL);
	}

	return pargs.np;
}

static struct cust_attr_dev *to_cust_device(struct device_node *cadev_node)
{
	struct platform_device *cust_pdev = NULL;

	cust_pdev = of_find_device_by_node(cadev_node);
	if (cust_pdev)
		return platform_get_drvdata(cust_pdev);

	return NULL;
}

/*
  * cust_register_notifier()
  * Register to be notified when cust device probes.
  * @client_dev: Client device pointer.
  * @cb:Callback function to execute when cust device probes successfully.
  * @priv: Client's private data which is passed back when cb() is called.
  * Returns: 0 upon success, negative upon errors.
  */
int cust_register_notifier(struct device *client_dev, void (*cb)(void *),
	void *priv)
{
	struct probe_notify_node *notify_node = NULL;
	struct device_node *cadev_node = NULL;
	struct cust_attr_dev *dev = NULL;

	if (!client_dev || !cb || !priv)
		return -EINVAL;

	cadev_node = to_cust_device_node(client_dev);

	mutex_lock(&notify_lock);
	dev = to_cust_device(cadev_node);
	if (dev) {
		of_node_put(cadev_node);
		cb(priv); /* cust device has probed already, notify immediately */
	} else {
		notify_node = kzalloc(sizeof(*notify_node), GFP_KERNEL);
		if (!notify_node) {
			mutex_unlock(&notify_lock);
			return -ENOMEM;
		}

		notify_node->cb = cb;
		notify_node->priv = priv;
		notify_node->cadev_node = cadev_node;

		list_add(&notify_node->node, &probe_notify_list);
	}
	mutex_unlock(&notify_lock);

	return 0;
}
EXPORT_SYMBOL(cust_register_notifier);

/*
  * cust_deregister_notifier()
  * Deregister probe completion notifier.
  * @client_dev: Client device pointer.
  * @priv: Client's private data.
  * Returns: 0 upon success, negative upon errors.
  */
int cust_deregister_notifier(struct device *client_dev, void *priv)
{
	struct device_node *cadev_node = NULL;
	struct probe_notify_node *pos = NULL;
	struct probe_notify_node *tmp = NULL;

	if (!client_dev)
		return -EINVAL;

	cadev_node = to_cust_device_node(client_dev);

	mutex_lock(&notify_lock);
	list_for_each_entry_safe(pos, tmp, &probe_notify_list, node) {
		if (pos->cadev_node == cadev_node && pos->priv == priv) {
			of_node_put(pos->cadev_node);
			list_del(&pos->node);
			kfree(pos);
			break;
		}
	}
	mutex_unlock(&notify_lock);

	of_node_put(cadev_node);

	return 0;
}
EXPORT_SYMBOL(cust_deregister_notifier);

static void client_cb_work(struct work_struct *work)
{
	struct cust_client *client = container_of(work, struct cust_client,
		client_cb_work);

	client->data.callback(client->data.priv, client->msg, sizeof(client->msg));
}

/*
  * cust_register_client()
  * Register to receive PMIC GLINK messages from remote subsystem.
  * @client_dev: Client device pointer.
  * @client_data: Details cust attr client uniquely.
  * Returns: Valid cust client pointer upon success, ERR_PTRs upon errors.
  * Notes: client_data should contain a unique cust_id. It is the same as cust
  *        attr req opcode.
  */
struct cust_client *cust_register_client(struct device *client_dev,
	const struct cust_client_data *client_data)
{
	int rc;
	struct cust_attr_dev *cadev = NULL;
	struct device_node *cadev_node = NULL;
	struct cust_client *caclient = NULL;

	if (!client_dev || !client_data)
		return ERR_PTR(-EINVAL);

	if (!client_data->priv || !client_data->callback)
		return ERR_PTR(-EINVAL);

	cadev_node = to_cust_device_node(client_dev);
	cadev = to_cust_device(cadev_node);
	of_node_put(cadev_node);
	if (!cadev)
		return ERR_PTR(-EPROBE_DEFER);

	caclient = kzalloc(sizeof(*caclient), GFP_KERNEL);
	if (!caclient)
		return ERR_PTR(-ENOMEM);
	caclient->cadev = cadev;
	caclient->data.cust_id = client_data->cust_id;
	caclient->data.priv = client_data->priv;
	caclient->data.callback = client_data->callback;
	INIT_WORK(&caclient->client_cb_work, client_cb_work);

	mutex_lock(&cadev->client_lock);
	rc = idr_alloc(&cadev->client_idr, caclient, caclient->data.cust_id,
		caclient->data.cust_id + 1, GFP_KERNEL);
	if (rc < 0) {
		hwlog_err("Error in allocating idr for client id %d: %d\n",
			caclient->data.cust_id, rc);
		mutex_unlock(&cadev->client_lock);
		kfree(caclient);
		return ERR_PTR(rc);
	}
	list_add(&caclient->c_node, &cadev->client_list);
	mutex_unlock(&cadev->client_lock);

	return caclient;
}
EXPORT_SYMBOL(cust_register_client);

/*
  * cust_register_client()
  * Deregister cust client to stop receiving PMIC GLINK messages.
  * specific to its cust_id from remote subsystem.
  * @client_dev: Client returned by cust_register_client.
  * Returns:0 upon success, negative upon errors.
  */
int cust_deregister_client(struct cust_client *client)
{
	struct cust_attr_dev *dev = NULL;
	struct cust_client *pos = NULL;
	struct cust_client *tmp = NULL;

	if (!client || !client->cadev)
		return -ENODEV;

	dev = client->cadev;

	cancel_work_sync(&client->client_cb_work);

	mutex_lock(&dev->client_lock);
	idr_remove(&dev->client_idr, client->data.cust_id);

	list_for_each_entry_safe(pos, tmp, &dev->client_list, c_node) {
		if (pos == client)
			list_del(&pos->c_node);
	}
	mutex_unlock(&dev->client_lock);

	kfree(client);
	return 0;
}
EXPORT_SYMBOL(cust_deregister_client);

static void cust_notify_clients(struct cust_attr_dev *dev)
{
	struct cust_attr_dev *pos_dev = NULL;
	struct probe_notify_node *pos = NULL;
	struct probe_notify_node *tmp = NULL;

	mutex_lock(&notify_lock);
	list_for_each_entry_safe(pos, tmp, &probe_notify_list, node) {
		pos_dev = to_cust_device(pos->cadev_node);
		if (!pos_dev)
			continue;

		if (pos_dev == dev) {
			pos->cb(pos->priv);
			of_node_put(pos->cadev_node);
			list_del(&pos->node);
			kfree(pos);
		}
	}
	mutex_unlock(&notify_lock);
}

static int cust_attr_probe(struct platform_device *pdev)
{
	int rc;
	struct cust_attr_dev *cadev = NULL;
	struct pmic_glink_client_data pgclient_data;
	struct device *dev = &pdev->dev;

	cadev = devm_kzalloc(dev, sizeof(*cadev), GFP_KERNEL);
	if (!cadev)
		return -ENOMEM;

	cadev->dev = dev;

	mutex_init(&cadev->client_lock);
	idr_init(&cadev->client_idr);
	INIT_LIST_HEAD(&cadev->client_list);

	pgclient_data.id = MSG_OWNER_CUST_ARRT_ID;
	pgclient_data.name = "cust_attr";
	pgclient_data.msg_cb = cust_attr_callback;
	pgclient_data.priv = cadev;
	pgclient_data.state_cb = NULL;

	cadev->pgclient = pmic_glink_register_client(dev, &pgclient_data);
	if (IS_ERR(cadev->pgclient)) {
		rc = PTR_ERR(cadev->pgclient);
		if (rc != -EPROBE_DEFER)
			dev_err(dev, "Error in pmic_glink registration: %d\n", rc);
		idr_destroy(&cadev->client_idr);
		return rc;
	}

	platform_set_drvdata(pdev, cadev);
	cust_notify_clients(cadev);

	return 0;
}

static int cust_attr_remove(struct platform_device *pdev)
{
	int rc;
	struct cust_attr_dev *cadev = platform_get_drvdata(pdev);
	struct cust_client *client = NULL;
	struct cust_client *tmp = NULL;
	struct probe_notify_node *npos = NULL;
	struct probe_notify_node *ntmp = NULL;

	idr_destroy(&cadev->client_idr);

	mutex_lock(&notify_lock);
	list_for_each_entry_safe(npos, ntmp, &probe_notify_list, node) {
		of_node_put(npos->cadev_node);
		list_del(&npos->node);
		kfree(npos);
	}
	mutex_unlock(&notify_lock);

	mutex_lock(&cadev->client_lock);
	list_for_each_entry_safe(client, tmp, &cadev->client_list, c_node)
		list_del(&client->c_node);
	mutex_unlock(&cadev->client_lock);

	rc = pmic_glink_unregister_client(cadev->pgclient);
	if (rc < 0)
		dev_err(cadev->dev, "Error in pmic_glink de-registration: %d\n", rc);

	return rc;
}

static const struct of_device_id cust_attr_match_table[] = {
	{ .compatible = "huawei,cust-attr-glink" },
	{},
};

static struct platform_driver cust_attr_driver = {
	.driver = {
		.name = "cust_attr_glink",
		.of_match_table = cust_attr_match_table,
	},
	.probe = cust_attr_probe,
	.remove = cust_attr_remove,
};
module_platform_driver(cust_attr_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("huawei cust attr driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
