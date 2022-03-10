/*
 * Copyright (C) 2013 Huawei Device Co.Ltd
 * License terms: GNU General Public License (GPL) version 2
 *
 */

#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <soc/qcom/subsystem_restart.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include "modem_reset/hw_modem_reset.h"

#define MAX_CMD_LEN 64
#define RESET_LEN 5
int subsystem_restart_requested = 0;

void set_subsystem_restart_requested(int set_value)
{
	subsystem_restart_requested = set_value;
}

int get_subsystem_restart_requested(void)
{
	return subsystem_restart_requested;
}

static ssize_t pil_mss_ctl_read(struct file *fp, char __user *buf,
				size_t count, loff_t *pos)
{
	return 0;
}

static ssize_t pil_mss_ctl_write(struct file *fp, const char __user *buf,
				 size_t count, loff_t *pos)
{
	unsigned char cmd[MAX_CMD_LEN]={0};
	int len = -1;

	if ((count < 1) || (NULL == buf))
	{
		pr_err("%s, %s, Input is invalid\n", __FILE__, __func__);
		return 0;
	}

	len = count > (MAX_CMD_LEN - 1) ? (MAX_CMD_LEN - 1) : count;

	if (copy_from_user(cmd, buf, len))
		return -EFAULT;

	cmd[len] = 0;

	/* lazy */
	if (cmd[len-1] == '\n') {
		cmd[len-1] = 0;
		len--;
	}

	if (!strncmp(cmd, "reset", RESET_LEN)) {
		pr_err("reset modem subsystem requested by huawei\n");
		set_subsystem_restart_requested(1);
		subsystem_restart("modem");
	}

	return count;
}

static int pil_mss_ctl_open(struct inode *ip, struct file *fp)
{
	return 0;
}

static int pil_mss_ctl_release(struct inode *ip, struct file *fp)
{
	return 0;
}

static const struct file_operations pil_mss_ctl_fops = {
	.owner = THIS_MODULE,
	.read = pil_mss_ctl_read,
	.write = pil_mss_ctl_write,
	.open = pil_mss_ctl_open,
	.release = pil_mss_ctl_release,
};

static struct miscdevice pil_mss_ctl_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "pil_modem_ctl",
	.fops = &pil_mss_ctl_fops,
};

static int __init pil_mss_ctl_init(void)
{
	return misc_register(&pil_mss_ctl_dev);
}
module_init(pil_mss_ctl_init);

static void __exit pil_mss_ctl_exit(void)
{
	misc_deregister(&pil_mss_ctl_dev);
}
module_exit(pil_mss_ctl_exit);