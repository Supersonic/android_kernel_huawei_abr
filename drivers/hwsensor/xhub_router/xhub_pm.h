/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Team:    Huawei DIVS
 * Date:    2020.07.20
 * Description: xhub logbuff module
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
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/completion.h>

#define PM_IOCTL_PKG_HEADER 4

typedef enum {
	/* system status */
	ST_NULL = 0,
	ST_BEGIN,
	ST_POWERON = ST_BEGIN,
	ST_SCREENON, /* 2 */
	ST_SCREENOFF, /* 3 */
	ST_SLEEP, /* 4 */
	ST_WAKEUP, /* 5 */
	ST_POWEROFF,
	ST_END
} sys_status_t;

typedef struct pm_buf_to_hal {
	uint8_t sensor_type;
	uint8_t cmd;
	uint8_t subcmd;
	uint8_t reserved;
	sys_status_t ap_status;
} pm_buf_to_hal_t;

