/*
  * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
  * Description: header file for cust attr glink
  * Author: zhoujiaqiao
  * Create: 2021-05-19
  */

#ifndef _CUST_ATTR_H_
#define _CUST_ATTR_H_

#include <linux/types.h>
#include <linux/device.h>

struct cust_client;

enum CUST_ARRT_OPCODE_REQ {
	CUST_ARRT_OPCODE_DTSI = 0x0100,
};

struct cust_client_data {
	u16 cust_id;
	void *priv;
	int (*callback)(void *priv, void *data, size_t len);
};

#ifdef CONFIG_CUST_ATTR_GLINK
int cust_attr_send_data(struct cust_client *client, const void *data,
	size_t len);
int cust_attr_send_err(struct cust_client *client);
int cust_register_notifier(struct device *client_dev,
	void (*cb)(void *), void *priv);
int cust_deregister_notifier(struct device *client_dev, void *priv);
struct cust_client *cust_register_client(struct device *client_dev,
	const struct cust_client_data *client_data);
int cust_deregister_client(struct cust_client *client);
#else
static inline int cust_attr_send_data(struct cust_client *client,
	const void *data, size_t len)
{
	return -ENODEV;
}

static inline int cust_attr_send_err(struct cust_client *client)
{
	return -ENODEV;
}

static inline int cust_register_notifier(struct device *client_dev,
	void (*cb)(void *), void *priv);
{
	return -ENODEV;
}

static inline int cust_deregister_notifier(struct device *client_dev,
	void *priv)
{
	return -ENODEV;
}

static inline struct cust_client *cust_register_client(
	struct device *client_dev, const struct cust_client_data *client_data)
{
	return ERR_PTR(-ENODEV);
}

static inline cust_deregister_client(struct cust_client *client)
{
	return -ENODEV;
}
#endif /* CONFIG_CUST_ATTR_GLINK */

#endif /* _CUST_ATTR_H_ */
