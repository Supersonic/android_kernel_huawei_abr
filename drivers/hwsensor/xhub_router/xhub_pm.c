/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Team:    Huawei DIVS
 * Date:    2021.07.20
 * Description: xhub pm module
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

#include "xhub_pm.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/sysfs.h>
#include <securec.h>
#include <linux/fb.h>
#include <../apsensor_channel/ap_sensor_route.h>
#include <../apsensor_channel/ap_sensor.h>
#include <linux/hrtimer.h>
#include "lcd_kit_drm_panel.h"
#include <linux/device.h>
#include <linux/backlight.h>
#include <linux/completion.h>

static int xhub_drm_notifier_call(struct notifier_block *nb,
	unsigned long event, void *data)
{
	struct drm_panel_notifier *blank_event = data;
	int *blank = NULL;

	pm_buf_to_hal_t pm_buf;

	pm_buf.sensor_type = SENSOR_TYPE_PM;
	pm_buf.cmd = 0;
	pm_buf.subcmd = 0;

	if (!blank_event || !blank_event->data) {
		pr_err("l_dev or blank_event is null\n");
		return NOTIFY_DONE;
	}

	blank = blank_event->data;

	if ((event == DRM_PANEL_EVENT_BLANK) && (*blank == DRM_PANEL_BLANK_UNBLANK)) {
		pm_buf.ap_status = ST_SCREENON;
		pr_info("xhub_drm_notifier screen on\n");
		ap_sensor_route_write((char *)&pm_buf, (sizeof(pm_buf.ap_status) + PM_IOCTL_PKG_HEADER));
	} else if ((event == DRM_PANEL_EVENT_BLANK) && (*blank == DRM_PANEL_BLANK_POWERDOWN)) {
		pm_buf.ap_status = ST_SCREENOFF;
		pr_info("xhub_drm_notifier screen off\n");
		ap_sensor_route_write((char *)&pm_buf, (sizeof(pm_buf.ap_status) + PM_IOCTL_PKG_HEADER));
	}

	return NOTIFY_DONE;
}



static struct notifier_block xhub_drm_notify = {
	.notifier_call = xhub_drm_notifier_call,
};

static int xhub_panel_register(void)
{
	(void)lcd_kit_drm_notifier_register(0, &xhub_drm_notify);
	return 0;
}

static void xhub_panel_exit(void)
{
        pr_info("xhub_panel_exit\n");
	return;
}

late_initcall_sync(xhub_panel_register);
module_exit(xhub_panel_exit);

MODULE_LICENSE("GPL");
