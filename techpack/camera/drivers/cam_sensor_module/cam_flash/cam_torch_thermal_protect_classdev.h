/*
 * cam_torch_thermal_protect_classdev.h
 *
 * Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.
 *
 * fled regulator interface
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

#ifndef _CAM_TORCH_THERMAL_PROTECT_CLASSDEV_H_
#define _CAM_TORCH_THERMAL_PROTECT_CLASSDEV_H_

#include <linux/leds.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/string.h>

enum flash_lock_status {
	FLASH_STATUS_UNLOCK,
	FLASH_STATUS_LOCKED,
};

int cam_torch_thermal_protect_get_value(void);
int cam_torch_thermal_protect_classdev_register(struct platform_device *pdev,
	void *data);

#endif /* _CAM_TORCH_THERMAL_PROTECT_CLASSDEV_H_ */
