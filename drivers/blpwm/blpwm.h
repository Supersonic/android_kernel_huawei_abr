/*
 * blpwm.h
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

#ifndef _BLPWM_H_
#define _BLPWM_H_

#include <linux/mutex.h>
#include <linux/soc/qcom/pmic_glink.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/completion.h>

#define BLPWM_GLINK_MSG_NAME                    "b2"
#define BLPWM_GLINK_MSG_OWNER_ID                32787
#define BLPWM_GLINK_MSG_TYPE_REQ_RESP           1
#define BLPWM_GLINK_MSG_OPCODE_SET              0x0057
#define BLPWM_GLINK_NOWAIT_MSG_OPCODE_SET       0x0058
#define BLPWM_GLINK_PLAN_DATA_SIZE              16
#define BLPWM_GLINK_MSG_WAIT_MS                 500
#define BLPWM_GLINK_DEBUG_PLA_SIZE              6
#define NV_NUMBER                               449
#define DEBUG_BUFFER_SIZE                       10
#define NV_VALID_SIZE                           15
#define APP_DUTY_MIN                            0
#define APP_DUTY_MAX                            100
#define COVER_INFO_NOT_READ                     0
#define COVER_INFO_ALREADY_READ                 1
#define INIT_DELAY                              10000

#define IOCTL_BLPWM_AT1_CMD                    _IO('Q', 0x01)
#define IOCTL_BLPWM_AT2_CMD                    _IO('Q', 0x02)
#define IOCTL_BLPWM_AT3_CMD                    _IO('Q', 0x03)
#define IOCTL_BLPWM_AT4_CMD                    _IO('Q', 0x04)
#define IOCTL_BLPWM_RESET_CMD                  _IO('Q', 0x05)
#define IOCTL_BLPWM_DUTY                       _IO('Q', 0x06)
#define IOCTL_BLPWM_USAGE                      _IO('Q', 0x08)
#define IOCTL_BLPWM_USAGE_STOP                 _IO('Q', 0x09)
#define IOCTL_COVER_COLOR_CMD                  _IO('Q', 0x0A)
#define IOCTL_COVER_LOOKUP_CMD                 _IO('Q', 0x0E)

/* COVER INFO */
#define SUPPORT_WITH_COLOR             3
#define ABNORMAL_WITH_COLOR            2
#define UNSUPPORT_WITH_NOCOLOR         1
#define SUPPORT_WITH_NOCOLOR           0

struct blpwm_dev {
	struct device *dev;
	struct pmic_glink_client *client;
	struct mutex send_lock;
	struct completion msg_ack;
	u32 color;
	u32 support;
	u32 duty_cycle_value;
	u32 usage;
	u32 usage_stop;
	bool status;
	struct delayed_work blpwm_init_work;
	struct dentry *root;
};

struct blpwm_glink_config_adsp_req {
	struct pmic_glink_hdr hdr;
	u32 plan;
	u32 duty;
	u32 color;
	u32 usage;
};

struct blpwm_glink_config_adsp_rsp {
	struct pmic_glink_hdr hdr;
	u32 result;
	u32 blpwm_support;
};

enum blpwm_color {
	COLOR_ID_MIN = 0,
	NOCOLOR = COLOR_ID_MIN,
	DARKBLUE,
	LIGHTBLUE,
	COLOR_ID_MAX = 5,
};

enum blpwm_cfg_cmd {
	BLPEM_CFG_CMD_MIN = 1,
	DBC_PLAN1 = BLPEM_CFG_CMD_MIN,
	DBC_PLAN2,
	DBC_PLAN3,
	DBC_PLAN4,
	DBC_RESET,
	ADC_CMD,
	USAGE_CMD,
	DUTY_CMD,
	USAGE_STOP_CMD,
	BLPWM_DARK_CMD,
	BLPEM_CFG_CMD_MAX,
};

#endif /* _BLPWM_H_ */
