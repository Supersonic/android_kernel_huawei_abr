/*
 * dev_perf_ctrl.c
 *
 * interfaces defined for perf_ctrl
 *
 * Copyright (c) 2021 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/perf_ctrl.h>

int perf_ctrl_set_memlat_immutable_flag(void __user *uarg)
{
	unsigned int flag;

	if (uarg == NULL)
		return -EINVAL;

	if (copy_from_user(&flag, uarg, sizeof(unsigned int)))
		return -EFAULT;

	memlat_set_immutable_flag(flag);

	return 0;
}
