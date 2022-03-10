// SPDX-License-Identifier: GPL-2.0
/*
 * D2DP kobject interface implementation
 */

#include <linux/mutex.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <uapi/linux/in.h>

#include "d2d.h"
#include "kobject.h"
#include "protocol.h"

static struct kset *d2dp_kset;

#define to_d2dp_obj(x) container_of(x, struct d2d_protocol, kobj)

struct d2dp_attribute {
	struct attribute attr;
	ssize_t (*show)(struct d2d_protocol *proto, struct d2dp_attribute *attr,
			char *buf);
	ssize_t (*store)(struct d2d_protocol *proto,
			 struct d2dp_attribute *attr, const char *buf,
			 size_t count);
};
#define to_d2dp_attr(x) container_of(x, struct d2dp_attribute, attr)

static ssize_t d2dp_attr_show(struct kobject *kobj,
			      struct attribute *attr,
			      char *buf)
{
	struct d2dp_attribute *attribute;
	struct d2d_protocol *proto;

	attribute = to_d2dp_attr(attr);
	proto = to_d2dp_obj(kobj);

	if (!attribute->show)
		return -EIO;

	return attribute->show(proto, attribute, buf);
}

static ssize_t d2dp_attr_store(struct kobject *kobj,
			       struct attribute *attr,
			       const char *buf, size_t len)
{
	struct d2dp_attribute *attribute;
	struct d2d_protocol *proto;

	attribute = to_d2dp_attr(attr);
	proto = to_d2dp_obj(kobj);

	if (!attribute->store)
		return -EIO;

	return attribute->store(proto, attribute, buf, len);
}

static const struct sysfs_ops d2dp_sysfs_ops = {
	.show  = d2dp_attr_show,
	.store = d2dp_attr_store,
};

static void d2dp_release(struct kobject *kobj)
{
	d2d_protocol_destroy(to_d2dp_obj(kobj));
}

static ssize_t tx_packet_id_show(struct d2d_protocol *proto,
				 struct d2dp_attribute *attr,
				 char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%llu\n", proto->tx_packet_id);
}

static ssize_t rx_packet_id_show(struct d2d_protocol *proto,
				 struct d2dp_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%llu\n", proto->rx_packet_id);
}

static struct d2dp_attribute tx_packet_id_attribute = __ATTR_RO(tx_packet_id);
static struct d2dp_attribute rx_packet_id_attribute = __ATTR_RO(rx_packet_id);

static struct attribute *d2dp_default_attrs[] = {
	&tx_packet_id_attribute.attr,
	&rx_packet_id_attribute.attr,
	NULL,
};

static struct kobj_type d2dp_ktype = {
	.sysfs_ops     = &d2dp_sysfs_ops,
	.release       = d2dp_release,
	.default_attrs = d2dp_default_attrs,
};

static u64 d2dp_kobj_id = 0;
static DEFINE_MUTEX(d2dp_kobj_mtx);

int d2dp_register_kobj(struct d2d_protocol *proto)
{
	int ret = 0;

	mutex_lock(&d2dp_kobj_mtx);

	proto->kobj.kset = d2dp_kset;
	ret = kobject_init_and_add(&proto->kobj, &d2dp_ktype, NULL,
				   "%llu", d2dp_kobj_id);
	if (ret) {
		pr_err("kobj init: %d\n", ret);
		goto unlock;
	}

	d2dp_kobj_id++;
	kobject_uevent(&proto->kobj, KOBJ_ADD);

unlock:
	mutex_unlock(&d2dp_kobj_mtx);
	return ret;
}

int d2dp_kobject_init(void)
{
	d2dp_kset = kset_create_and_add("d2dp", NULL, kernel_kobj);
	if (!d2dp_kset)
		return -ENOMEM;

	return 0;
}

void d2dp_kobject_deinit(void)
{
	kset_unregister(d2dp_kset);
	d2dp_kset = NULL;
}
