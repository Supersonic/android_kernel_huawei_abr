/*
 * cam_torch_thermal_protect_classdev.c
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

#include "cam_flash_dev.h"
#include "cam_res_mgr_api.h"
#include "cam_torch_classdev.h"
#include "cam_torch_thermal_protect_classdev.h"

static struct cam_flash_ctrl *g_flash_fctrl;
static enum flash_lock_status g_flash_thermal_protect_status;

static ssize_t hw_flash_thermal_protect_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int rc;
	if (dev == NULL || attr == NULL || buf == NULL) {
		pr_err("%s  NULL input para", __func__);
		return -1;
	}
	rc = scnprintf(buf, PAGE_SIZE, "%d\n", g_flash_thermal_protect_status);
	return rc;
}

static ssize_t hw_flash_thermal_protect_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int op_code = 0;
	int length = 0;

	if (dev == NULL || attr == NULL || buf == NULL) {
		pr_err("%s  NULL input para", __func__);
		return -1;
	}

	length = sscanf(buf, "%d", &op_code);
	if (length != 1) {
		pr_err("%s failed to check param.", __func__);
		return -1;
	}

	if (FLASH_STATUS_UNLOCK == op_code) {
		pr_info("%s disable thermal protect", __func__);
		g_flash_thermal_protect_status = FLASH_STATUS_UNLOCK;
	} else {
		pr_info("%s enable thermal protect", __func__);
		cam_res_mgr_led_trigger_event(g_flash_fctrl->switch_trigger,
			(enum led_brightness)LED_SWITCH_OFF);
		g_flash_thermal_protect_status = FLASH_STATUS_LOCKED;
	}

	return count;
}

int cam_torch_thermal_protect_get_value(void)
{
	return g_flash_thermal_protect_status;
}

static struct device_attribute hw_flash_thermal_protect =
	__ATTR(flash_thermal_protect, 0660,
	hw_flash_thermal_protect_show,
	hw_flash_thermal_protect_store);

int cam_torch_thermal_protect_classdev_register(struct platform_device *pdev,
	void *data)
{
	struct led_classdev *cdev_torch;

	pr_info("enter %s", __func__);
	if (!pdev || !data) {
		pr_err("%s, Null ptr", __func__);
		return -EINVAL;
	}

	cdev_torch = devm_kzalloc(&pdev->dev, sizeof(*cdev_torch), GFP_KERNEL);
	if (!cdev_torch) {
		pr_err("%s, no mem\n", __func__);
		return -ENOMEM;
	}

	cdev_torch->name = kstrdup("torch", GFP_KERNEL);
	cdev_torch->brightness = LED_OFF;
	cdev_torch->brightness_set = NULL;

	if(devm_led_classdev_register(&pdev->dev, cdev_torch)) {
		pr_err("register thermal_protect torch_led fail");
		goto err_create_leds_device;
	}

	if (device_create_file(cdev_torch->dev, &hw_flash_thermal_protect)) {
		pr_err("Failed to create dev file hw_flash_thermal_protect");
		goto err_create_leds_flash_thermal_protect_file;
	}

	g_flash_fctrl = data;

	return 0;

err_create_leds_flash_thermal_protect_file:
	device_remove_file(cdev_torch->dev, &hw_flash_thermal_protect);
err_create_leds_device:
	led_classdev_unregister(cdev_torch);

	return -EINVAL;
}
EXPORT_SYMBOL(cam_torch_thermal_protect_get_value);
EXPORT_SYMBOL(cam_torch_thermal_protect_classdev_register);
