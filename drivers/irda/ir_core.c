/*
 * ir_core.c
 *
 * infrared core driver
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

#include "ir_core.h"
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ktime.h>
#include <linux/uaccess.h>
#include <huawei_platform/log/hw_log.h>

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG ir_core
HWLOG_REGIST();

#ifdef IR_CORE_DEBUG
static struct dentry *g_ir_root;
static unsigned int g_ir_use_count;
#endif /* IR_CORE_DEBUG */

static int ir_core_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	struct ir_device *ir = container_of(file->private_data,
		struct ir_device, misc_dev);

	get_device(ir->parent);
	file->private_data = ir;
	if (ir->ops && ir->ops->open)
		ret = ir->ops->open(ir->parent);

	hwlog_info("%s ret=%d\n", __func__, ret);
	return ret;
}

static int ir_core_close(struct inode *inode, struct file *file)
{
	struct ir_device *ir = (struct ir_device *)file->private_data;

	if (ir->ops && ir->ops->close)
		ir->ops->close(ir->parent);

	put_device(ir->parent);

	hwlog_info("%s\n", __func__);
	return 0;
}

static unsigned int ir_core_calc_duration(const unsigned int *tx_buf,
	size_t count)
{
	int i;
	unsigned int duration = 0;

	for (i = 0; i < count; i++) {
		if ((tx_buf[i] > (IR_MAX_DURATION - duration)) || !tx_buf[i])
			return 0;

		if (tx_buf[i] < IR_MIN_DURATION)
			hwlog_info("%s: tx_buf-%d=%u\n", __func__, i, tx_buf[i]);

		duration += tx_buf[i];
	}

	return duration;
}

#ifdef IR_CORE_DEBUG
static void ir_core_print_tx(struct ir_device *ir, const unsigned int *tx_buf,
	unsigned int count)
{
	int i;
	char str[IR_TRACE_SIZE] = { 0 };

	if (!ir->trace_mode)
		return;

	memset(str, 0, IR_TRACE_SIZE);
	for (i = 0; i < count; i++)
		snprintf(str, IR_TRACE_SIZE - strlen(str), "%s %ld ", str, tx_buf[i]);

	pr_err("%s: %s\n", __func__, str);
}
#else
static void ir_core_print_tx(struct ir_device *ir, const unsigned int *tx_buf,
	unsigned int count)
{
}
#endif /* IR_CORE_DEBUG */

static ssize_t ir_core_transmit(struct ir_device *ir, const unsigned int *tx_buf,
	unsigned int count)
{
	ssize_t ret;
	unsigned int duration;

	duration = ir_core_calc_duration(tx_buf, count);
	if (duration == 0) {
		hwlog_err("%s: ir transmit duration illegal\n", __func__);
		return -EINVAL;
	}
	hwlog_info("%s: ir transmit duration %u\n", __func__, duration);
	ir_core_print_tx(ir, tx_buf, count);

	mutex_lock(&ir->lock);
	ret = ir->ops->tx(ir->parent, tx_buf, count);
	mutex_unlock(&ir->lock);

	if (ret)
		hwlog_err("%s: ir transmit ret=%d\n", __func__, ret);
	return ret;
}

static ssize_t ir_core_write(struct file *file, const char __user *buf,
	size_t n, loff_t *ppos)
{
	ssize_t ret;
	size_t count = n / sizeof(unsigned int);
	unsigned int *tx_buf = NULL;
	struct ir_device *ir = (struct ir_device *)file->private_data;

	if ((n < sizeof(unsigned int)) || (count > IR_BUF_SIZE))
		return -EINVAL;

	if (!ir->ops || !ir->ops->tx)
		return -EFAULT;

	tx_buf = memdup_user(buf, n);
	if (IS_ERR(tx_buf))
		return PTR_ERR(tx_buf);

	ret = ir_core_transmit(ir, tx_buf, count);
	if (!ret)
		ret = n;

	kfree(tx_buf);
	hwlog_info("%s: ret=%d\n", __func__, ret);
	return ret;
}

static int ir_core_set_carrier(struct ir_device *ir, unsigned int val)
{
	hwlog_info("%s: val=%d\n", __func__, val);

	if (val == 0)
		return -EINVAL;

	ir->carrier_freq = val;
	if (ir->ops && ir->ops->tx_carrier)
		return ir->ops->tx_carrier(ir->parent, val);

	return 0;
}

static int ir_core_set_duty_cycle(struct ir_device *ir, unsigned int val)
{
	hwlog_info("%s: val=%d\n", __func__, val);

	if ((val == 0) || (val >= IR_MAX_DUTY_CYCLE))
		return -EINVAL;

	ir->duty_cycle = val;
	if (ir->ops && ir->ops->tx_duty_cycle)
		return ir->ops->tx_duty_cycle(ir->parent, val);

	return 0;
}

static long ir_core_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;
	unsigned int val = 0;
	unsigned int __user *argp = (unsigned int __user *)(arg);
	struct ir_device *ir = (struct ir_device *)file->private_data;

	if (_IOC_DIR(cmd) & _IOC_WRITE) {
		ret = get_user(val, argp);
		if (ret)
			return ret;
	}

	mutex_lock(&ir->lock);
	switch (cmd) {
	case IR_SET_CARRIER:
		ret = ir_core_set_carrier(ir, val);
		break;
	case IR_SET_DUTY_CYCLE:
		ret = ir_core_set_duty_cycle(ir, val);
		break;
	default:
		ret = -ENOTTY;
		break;
	}
	mutex_unlock(&ir->lock);

	return ret;
}

static const struct file_operations ir_misc_fops = {
	.owner = THIS_MODULE,
	.open = ir_core_open,
	.release = ir_core_close,
	.write = ir_core_write,
	.unlocked_ioctl = ir_core_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ir_core_ioctl,
#endif /* CONFIG_COMPAT */
};

#ifdef IR_CORE_DEBUG
static int ir_str_to_uint_array(char *src, unsigned int *dest, unsigned int dl)
{
	unsigned int index = 0;
	char *sub = NULL;
	char *cur = NULL;

	cur = src;
	sub = strsep(&cur, " ");
	while (sub) {
		if (*sub != '\0') {
			/* 10: data is decimal */
			if (kstrtouint(sub, 10, &dest[index]))
				return -EINVAL;

			index++;
			if (index >= dl)
				return index;
		}
		sub = strsep(&cur, " ");
	}

	return index;
}

static int ir_debug_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	struct ir_device *ir = inode->i_private;

	get_device(ir->parent);
	file->private_data = ir;
	if (ir->ops && ir->ops->open)
		ret = ir->ops->open(ir->parent);

	hwlog_info("%s ret=%d\n", __func__, ret);
	return ret;
}

static int ir_debug_close(struct inode *inode, struct file *file)
{
	struct ir_device *ir = (struct ir_device *)file->private_data;

	if (ir->ops && ir->ops->close)
		ir->ops->close(ir->parent);

	put_device(ir->parent);

	hwlog_info("%s\n", __func__);
	return 0;
}

static ssize_t ir_debug_write(struct file *file, const char __user *buf,
	size_t n, loff_t *ppos)
{
	ssize_t ret;
	int count;
	char tx_char_buf[IR_BUF_SIZE] = { 0 };
	unsigned int tx_buf[IR_CIR_SIZE] = { 0 };
	struct ir_device *ir = (struct ir_device *)file->private_data;

	if (n >= IR_BUF_SIZE)
		return -EINVAL;

	if (!ir->ops || !ir->ops->tx)
		return -EFAULT;

	if (copy_from_user(tx_char_buf, buf, n)) {
		hwlog_err("%s: memdup error\n", __func__);
		return -EFAULT;
	}
	tx_char_buf[n] = '\0';

	count = ir_str_to_uint_array(tx_char_buf, tx_buf, IR_CIR_SIZE);
	if (count <= 0) {
		hwlog_err("%s: format error %d\n", __func__, count);
		return -EINVAL;
	}

	ret = ir_core_transmit(ir, tx_buf, count);
	if (!ret)
		ret = n;

	hwlog_info("%s: ret=%d\n", __func__, ret);
	return ret;
}

static const struct file_operations ir_debug_fops = {
	.open = ir_debug_open,
	.release = ir_debug_close,
	.write = ir_debug_write,
};

static int ir_debug_carrier_freq_set(void *data, u64 val)
{
	int ret;
	struct ir_device *ir = data;

	mutex_lock(&ir->lock);
	ret = ir_core_set_carrier(data, val);
	mutex_unlock(&ir->lock);

	return ret;
}

static int ir_debug_carrier_freq_get(void *data, u64 *val)
{
	struct ir_device *ir = data;

	mutex_lock(&ir->lock);
	*val = ir->carrier_freq;
	mutex_unlock(&ir->lock);
	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(ir_carrier_freq_fops,
	ir_debug_carrier_freq_get, ir_debug_carrier_freq_set,
	"%llu\n");


static int ir_debug_duty_cycle_set(void *data, u64 val)
{
	int ret;
	struct ir_device *ir = data;

	mutex_lock(&ir->lock);
	ret = ir_core_set_duty_cycle(data, val);
	mutex_unlock(&ir->lock);

	hwlog_info("%s misc_dev=%p ir_device=%p\n", __func__,
		&ir->misc_dev, ir);
	return ret;
}

static int ir_debug_duty_cycle_get(void *data, u64 *val)
{
	struct ir_device *ir = data;

	mutex_lock(&ir->lock);
	*val = ir->duty_cycle;
	mutex_unlock(&ir->lock);
	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(ir_duty_cycle_fops,
	ir_debug_duty_cycle_get, ir_debug_duty_cycle_set,
	"%llu\n");

static int ir_debug_trace_mode_set(void *data, u64 val)
{
	struct ir_device *ir = data;

	hwlog_info("%s: set trace mode to %llu\n", __func__, val);
	ir->trace_mode = val;
	return 0;
}

static int ir_debug_trace_mode_get(void *data, u64 *val)
{
	struct ir_device *ir = data;

	*val = ir->trace_mode;
	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(ir_core_trace_fops,
	ir_debug_trace_mode_get, ir_debug_trace_mode_set, "%llu\n");

static void ir_create_debugfs(const char *name, struct ir_device *ir)
{
	struct dentry *root = NULL;
	struct dentry *sub = NULL;

	if (!g_ir_root) {
		root = debugfs_create_dir(IR_DEBUG_ROOT, NULL);
		if (!root) {
			hwlog_err("%s: create debugfs root node fail\n", __func__);
			return;
		}
		g_ir_root = root;
	}

	sub = debugfs_create_dir(name, g_ir_root);
	if (!sub) {
		hwlog_err("%s: create debugfs sub node fail\n", __func__);
		return;
	}
	ir->sub = sub;
	g_ir_use_count++;

	debugfs_create_file("data", 0200, sub, ir, &ir_debug_fops);
	debugfs_create_file("freq", 0600, sub, ir, &ir_carrier_freq_fops);
	debugfs_create_file("duty", 0600, sub, ir, &ir_duty_cycle_fops);
	debugfs_create_file("trace_mode", 0600, sub, ir, &ir_core_trace_fops);
	hwlog_info("%s\n", __func__);
}

static void ir_destroy_debugfs(struct ir_device *ir)
{
	if (!ir->sub || !g_ir_root)
		return;
	g_ir_use_count--;

	if (g_ir_use_count)
		debugfs_remove_recursive(ir->sub);
	else
		debugfs_remove_recursive(g_ir_root);
}
#else
static void ir_create_debugfs(const char *name, struct ir_device *ir)
{
}

static void ir_destroy_debugfs(struct ir_device *ir)
{
}
#endif /* IR_CORE_DEBUG */

static struct ir_device *ir_register_device(const char *name,
	const struct ir_ops *ops)
{
	int ret;
	struct ir_device *ir = NULL;

	ir = kzalloc(sizeof(*ir), GFP_KERNEL);
	if (!ir)
		return NULL;

	ir->misc_dev.minor = MISC_DYNAMIC_MINOR;
	ir->misc_dev.name = "irctrl";
	ir->misc_dev.fops = &ir_misc_fops;
	mutex_init(&ir->lock);
	ir->ops = ops;
	ir->carrier_freq = IR_DEF_CARRIER_FREQ;
	ir->duty_cycle = IR_DEF_DUTY_CYCLE;

	ret = misc_register(&ir->misc_dev);
	if (ret) {
		hwlog_err("%s: misc register failed %d\n", __func__, ret);
		goto misc_regisetr_fail;
	}

	ir_create_debugfs(name, ir);
	hwlog_info("%s: ir register %s\n", __func__, ops->name);
	return ir;

misc_regisetr_fail:
	mutex_destroy(&ir->lock);
	kfree(ir);
	return NULL;
}

static void ir_unregister_device(struct ir_device *ir)
{
	ir_destroy_debugfs(ir);
	misc_deregister(&ir->misc_dev);
	mutex_destroy(&ir->lock);
	kfree(ir);
}

static void devm_ir_release(struct device *dev, void *res)
{
	struct ir_device *ir = *(struct ir_device **)res;

	if (!ir)
		return;

	ir_unregister_device(ir);
}

struct ir_device *devm_ir_register_device(struct device *parent,
	const struct ir_ops *ops)
{
	struct ir_device **ptr = NULL;
	struct ir_device *ir = NULL;

	if (!parent || !ops)
		return NULL;

	ptr = devres_alloc(devm_ir_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return NULL;

	ir = ir_register_device(dev_name(parent), ops);
	if (!ir) {
		devres_free(ptr);
		hwlog_err("%s: ir register fail\n", __func__);
		return NULL;
	}

	ir->parent = parent;
	*ptr = ir;
	devres_add(parent, ptr);
	hwlog_info("%s: ir register %s\n", __func__, ops->name);
	return ir;
}
