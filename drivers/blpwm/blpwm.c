/*
 * blpwm.c
 *
 * blpwm module for qcom
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

#include "blpwm.h"
#include <linux/module.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/rpmsg.h>
#include <linux/debugfs.h>
#ifdef CONFIG_HW_NVE
#include <linux/mtd/hw_nve_interface.h>
#endif
#include <huawei_platform/log/hw_log.h>

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG blpwm
HWLOG_REGIST();

static struct blpwm_dev *g_blpwm_dev;

static void blpwm_store_duty(uintptr_t arg)
{
	if (!arg) {
		hwlog_err("%s: input arg is null\n", __func__);
		return;
	}

	if (copy_from_user(&g_blpwm_dev->duty_cycle_value, (void __user *)arg,
		sizeof(int))) {
		hwlog_err("blpwm %s: copy from user fail\n", __func__);
		return;
	}

	hwlog_info("%s: g_blpwm_dev->duty is: %d\n", __func__, g_blpwm_dev->duty_cycle_value);
}

static void blpwm_store_useage(uintptr_t arg)
{
	if (!arg) {
		hwlog_err("%s: input arg is null\n", __func__);
		return;
	}

	if (copy_from_user(&g_blpwm_dev->usage, (void __user *)arg, sizeof(int))) {
		hwlog_err("%s: copy from user fail\n", __func__);
		return;
	}

	hwlog_info("%s: g_blpwm_dev->usage is %d\n", __func__, g_blpwm_dev->usage);
}

static void blpwm_store_useage_stop(uintptr_t arg)
{
	if (!arg) {
		hwlog_err("%s: input arg is null\n", __func__);
		return;
	}

	if (copy_from_user(&g_blpwm_dev->usage_stop, (void __user *)arg, sizeof(int))) {
		hwlog_err("%s: copy from user fail\n", __func__);
		return;
	}

	hwlog_info("%s: g_blpwm_dev->usage_stop is %d\n", __func__,
		g_blpwm_dev->usage_stop);
}

static int blpwm_nv_color_read(u32 *pcolor)
{
	struct hw_nve_info_user nv_info = {0};

	if (!pcolor) {
		hwlog_err("pcoloris null\n");
		return -EINVAL;
	}

	nv_info.nv_operation = NV_READ;
	nv_info.nv_number = NV_NUMBER;
	nv_info.valid_size = NV_VALID_SIZE;
	strncpy(nv_info.nv_name, "BLPWM", sizeof(nv_info.nv_name) - 1);
	nv_info.nv_name[sizeof(nv_info.nv_name) - 1] = '\0';
	memset(nv_info.nv_data, 0, sizeof(nv_info.nv_data));

	if (hw_nve_direct_access(&nv_info)) {
		hwlog_err("nv %s read fail\n", nv_info.nv_name);
		return -EINVAL;
	}
	memcpy(pcolor, nv_info.nv_data, sizeof(u32));

	hwlog_info("%s: color read from nv is: %d", __func__, *pcolor);
	return 0;
}

static int blpwm_nv_color_write(u32 color)
{
	struct hw_nve_info_user nv_info = {0};

	nv_info.nv_operation = NV_WRITE;
	nv_info.nv_number  = NV_NUMBER;
	nv_info.valid_size = NV_VALID_SIZE;
	strncpy(nv_info.nv_name, "BLPWM", sizeof(nv_info.nv_name) - 1);
	nv_info.nv_name[sizeof(nv_info.nv_name) - 1] = '\0';
	memcpy(nv_info.nv_data, &color, sizeof(color));

	if (hw_nve_direct_access(&nv_info)) {
		hwlog_err("nv %s write fail\n", nv_info.nv_name);
		return -EINVAL;
	}

	hwlog_info("%s write color:%d", __func__, color);
	return 0;
}

static int blpwm_glink_send_message(struct blpwm_dev *b_dev, void *data, int len)
{
	int ret;

	mutex_lock(&b_dev->send_lock);
	reinit_completion(&b_dev->msg_ack);

	ret = pmic_glink_write(b_dev->client, data, len);
	if (ret) {
		mutex_unlock(&b_dev->send_lock);
		hwlog_err("send message error\n");
		return ret;
	}

	ret = wait_for_completion_timeout(&b_dev->msg_ack,
		msecs_to_jiffies(BLPWM_GLINK_MSG_WAIT_MS));
	if (!ret) {
		mutex_unlock(&b_dev->send_lock);
		hwlog_err("send message timeout\n");
		return -ETIMEDOUT;
	}

	hwlog_info("blpwm glink send message succ\n");
	mutex_unlock(&b_dev->send_lock);

	return ret;
}

static int blpwm_glink_send_message_no_wait(struct blpwm_dev *b_dev, void *data, int len)
{
	int ret;

	mutex_lock(&b_dev->send_lock);

	ret = pmic_glink_write(b_dev->client, data, len);
	if (ret) {
		mutex_unlock(&b_dev->send_lock);
		hwlog_err("send no wait message error\n");
		return ret;
	}

	hwlog_info("blpwm glink no wait send message succ\n");
	mutex_unlock(&b_dev->send_lock);

	return ret;
}

static int blpwm_glink_config_adsp(struct blpwm_dev *b_dev, enum blpwm_cfg_cmd sub_cmd)
{
	struct blpwm_glink_config_adsp_req msg = { {0} };

	msg.hdr.owner = BLPWM_GLINK_MSG_OWNER_ID;
	msg.hdr.type = BLPWM_GLINK_MSG_TYPE_REQ_RESP;
	msg.hdr.opcode = BLPWM_GLINK_MSG_OPCODE_SET;
	msg.plan = sub_cmd;
	msg.duty = b_dev->duty_cycle_value;
	msg.color = b_dev->color;
	msg.usage = b_dev->usage;

	hwlog_info(" %s:plan is %u\n", __func__, msg.plan);
	if (msg.plan == DUTY_CMD || msg.plan == USAGE_CMD || msg.plan == USAGE_STOP_CMD) {
		msg.hdr.opcode = BLPWM_GLINK_NOWAIT_MSG_OPCODE_SET;
		return blpwm_glink_send_message_no_wait(b_dev, &msg, sizeof(msg));
	} else {
		return blpwm_glink_send_message(b_dev, &msg, sizeof(msg));
	}
}

static void blpwm_glink_handle_config_adsp(struct blpwm_dev *b_dev, void *data, size_t len)
{
	struct blpwm_glink_config_adsp_rsp *msg = data;

	if (len != sizeof(*msg)) {
		hwlog_err("invalid msg len: %u!=%u\n", len, sizeof(*msg));
		return;
	}

	if (msg->result) {
		hwlog_err("invalid msg ret: %u\n", msg->result);
		return;
	}

	g_blpwm_dev->support = msg->blpwm_support;
	hwlog_info("set plan result is %u, blpwm_support is %u\n",
		 msg->result, g_blpwm_dev->support);

	complete(&b_dev->msg_ack);
}

static int blpwm_glink_msg_callback(void *dev_data, void *data, size_t len)
{
	struct pmic_glink_hdr *hdr = data;
	struct blpwm_dev *b_dev = dev_data;

	if (!b_dev || !hdr)
		return -ENODEV;

	hwlog_info("msg_callback: owner=%u type=%u opcode=%u len=%zu\n",
		hdr->owner, hdr->type, hdr->opcode, len);

	if (hdr->owner != BLPWM_GLINK_MSG_OWNER_ID) {
		hwlog_err("invalid msg owner: %u\n", hdr->owner);
		return -EINVAL;
	}

	if (hdr->opcode == BLPWM_GLINK_MSG_OPCODE_SET)
		blpwm_glink_handle_config_adsp(b_dev, data, len);

	return 0;
}

static void blpwm_init_work(struct work_struct *work)
{
	blpwm_nv_color_read(&g_blpwm_dev->color);
	blpwm_glink_config_adsp(g_blpwm_dev, ADC_CMD);
}

static ssize_t blpwm_hal_get_info(struct file *file, char __user *buf, size_t count,
	loff_t *pos)
{
	char ret_buf[DEBUG_BUFFER_SIZE] = {0};

	if (!buf) {
		hwlog_err("buf is null\n");
		return -EINVAL;
	}

	ret_buf[0] = g_blpwm_dev->color;
	ret_buf[1] = g_blpwm_dev->support;

	if (copy_to_user(buf, ret_buf, sizeof(ret_buf))) {
		hwlog_err("%s: copy_to_user failed\n", __func__);
		return -EINVAL;
	}

	hwlog_info("%s: color is %d, support is %d\n",
		__func__, ret_buf[0], ret_buf[1]);

	return 0;
}

static void blpwm_set_duty(void)
{
	if (g_blpwm_dev->duty_cycle_value == APP_DUTY_MIN) {
		blpwm_glink_config_adsp(g_blpwm_dev, BLPWM_DARK_CMD);
		return;
	}

	g_blpwm_dev->duty_cycle_value = APP_DUTY_MAX - g_blpwm_dev->duty_cycle_value;
	blpwm_glink_config_adsp(g_blpwm_dev, DUTY_CMD);
}

static long blpwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	int ret = 0;
	void __user *data = (void __user *)(uintptr_t)arg;

	if (!g_blpwm_dev)
		return -EFAULT;

	switch (cmd) {
	case IOCTL_BLPWM_AT1_CMD:
		blpwm_glink_config_adsp(g_blpwm_dev, DBC_PLAN1);
		break;
	case IOCTL_BLPWM_AT2_CMD:
		blpwm_glink_config_adsp(g_blpwm_dev, DBC_PLAN2);
		break;
	case IOCTL_BLPWM_AT3_CMD:
		blpwm_glink_config_adsp(g_blpwm_dev, DBC_PLAN3);
		break;
	case IOCTL_BLPWM_AT4_CMD:
		blpwm_glink_config_adsp(g_blpwm_dev, DBC_PLAN4);
		break;
	case IOCTL_BLPWM_RESET_CMD:
		blpwm_glink_config_adsp(g_blpwm_dev, DBC_RESET);
		break;
	case IOCTL_COVER_COLOR_CMD:
		g_blpwm_dev->color = (u32)(arg);
		blpwm_nv_color_write(g_blpwm_dev->color);
		break;
	case IOCTL_BLPWM_DUTY:
		blpwm_store_duty((uintptr_t)data);
		blpwm_set_duty();
		break;
	case IOCTL_BLPWM_USAGE:
		blpwm_store_useage((uintptr_t)data);
		blpwm_glink_config_adsp(g_blpwm_dev, USAGE_CMD);
		break;
	case IOCTL_BLPWM_USAGE_STOP:
		blpwm_store_useage_stop((uintptr_t)data);
		blpwm_glink_config_adsp(g_blpwm_dev, USAGE_STOP_CMD);
		break;
	case IOCTL_COVER_LOOKUP_CMD:
		blpwm_nv_color_read(&g_blpwm_dev->color);
		blpwm_glink_config_adsp(g_blpwm_dev, ADC_CMD);
		copy_to_user((u32 *)(uintptr_t)arg, &g_blpwm_dev->support,
			sizeof(g_blpwm_dev->support));
		break;
	default:
		hwlog_err("unsupport cmd\n");
		ret = -EFAULT;
		break;
	}

	return (long)ret;
}

static ssize_t blpwm_enable_debug(struct file *file, const char __user *buf,
	size_t size, loff_t *ppos)
{
	struct blpwm_dev *b_dev = g_blpwm_dev;
	int value;
	int ret;

	hwlog_info("%s enter\n", __func__);

	if (!buf)
		return -EINVAL;

	ret = kstrtoint(buf, DEBUG_BUFFER_SIZE, &value);
	if (ret) {
		hwlog_err("%s: covert to int type failed\n", __func__);
		return ret;
	}

	if (value >= BLPEM_CFG_CMD_MIN && value <= BLPEM_CFG_CMD_MAX) {
		hwlog_info("value is %d\n", value);
		blpwm_glink_config_adsp(b_dev, value);
	} else {
		hwlog_info("invlaid parameters\n");
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations blpwm_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = blpwm_hal_get_info,
	.write = blpwm_enable_debug,
	.unlocked_ioctl = blpwm_ioctl,
};

static struct miscdevice blpwm_misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "blpwm",
	.fops = &blpwm_fops,
};

static void blpwm_create_debugfs(struct blpwm_dev *b_dev)
{
	b_dev->root = debugfs_create_dir("blpwm", NULL);
	if (!b_dev->root) {
		hwlog_err("%s: create debugfs root fail\n", __func__);
		return;
	}

	debugfs_create_file("plan", 0220, b_dev->root, b_dev, &blpwm_fops);
}

static int blpwm_probe(struct platform_device *pdev)
{
	struct blpwm_dev *b_dev = NULL;
	struct pmic_glink_client_data client_data = { 0 };
	int ret = -EINVAL;

	if (!pdev || !pdev->dev.of_node)
		return -ENODEV;

	b_dev = devm_kzalloc(&pdev->dev, sizeof(*b_dev), GFP_KERNEL);
	if (!b_dev)
		return -ENOMEM;

	b_dev->dev = &pdev->dev;

	mutex_init(&b_dev->send_lock);
	init_completion(&b_dev->msg_ack);

	client_data.id = BLPWM_GLINK_MSG_OWNER_ID;
	client_data.name = BLPWM_GLINK_MSG_NAME;
	client_data.msg_cb = blpwm_glink_msg_callback;
	client_data.priv = b_dev;

	b_dev->client = pmic_glink_register_client(b_dev->dev, &client_data);
	if (IS_ERR(b_dev->client)) {
		ret = PTR_ERR(b_dev->client);
		if (ret != -EPROBE_DEFER)
			hwlog_err("blpwm glink register fail, ret is %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, b_dev);
	g_blpwm_dev = b_dev;

	ret = misc_register(&blpwm_misc_dev);
	if (ret) {
		hwlog_err("%s: misc register failed %d", __func__, ret);
		return ret;
	}

	blpwm_create_debugfs(b_dev);
	INIT_DELAYED_WORK(&b_dev->blpwm_init_work, blpwm_init_work);
	schedule_delayed_work(&b_dev->blpwm_init_work, msecs_to_jiffies(INIT_DELAY));

	return 0;
}

static int blpwm_remove(struct platform_device *pdev)
{
	struct blpwm_dev *b_dev = platform_get_drvdata(pdev);

	if (!b_dev)
		return -ENODEV;

	mutex_destroy(&b_dev->send_lock);
	pmic_glink_unregister_client(b_dev->client);
	devm_kfree(&pdev->dev, g_blpwm_dev);
	g_blpwm_dev = NULL;
	return 0;
}

static const struct of_device_id blpwm_of_match[] = {
	{ .compatible = "huawei,blpwm", },
	{},
};
MODULE_DEVICE_TABLE(of, blpwm_of_match);

static struct platform_driver blpwm_driver = {
	.driver = {
		.name = "blpwm",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(blpwm_of_match),
	},
	.probe = blpwm_probe,
	.remove = blpwm_remove,
};

static int __init blpwm_init(void)
{
	return platform_driver_register(&blpwm_driver);
}

static void __exit blpwm_exit(void)
{
	platform_driver_unregister(&blpwm_driver);
}

device_initcall(blpwm_init);
module_exit(blpwm_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("blpwm driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
