/*
 * wireless_tx_pen_kb.h
 *
 * wireless tx pen and kb
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

#ifndef _WIRELESS_TX_PEN_KB_H_
#define _WIRELESS_TX_PEN_KB_H_

#define WLTX_GPIO_HIGH                   1
#define WLTX_GPIO_LOW                    0
#define WLTX_PING_TIMEOUT_PEN_DEFAULT    2
#define WLTX_PING_TIMEOUT_KB_DEFAULT     30

enum wireless_gpio_info_index {
	WLTX_GPIO_PEN_KB_BEGIN = 0,
	WLTX_GPIO_PEN = WLTX_GPIO_PEN_KB_BEGIN,
	WLTX_GPIO_KB,
};

enum wireless_tx_pen_kb_acc_info_index {
	WLTX_ACC_INFO_PEN_KB_BEGIN = 0,
	WLTX_ACC_INFO_PEN = WLTX_ACC_INFO_PEN_KB_BEGIN,
	WLTX_ACC_INFO_KB,
	WLTX_ACC_INFO_PEN_KB_END,
};

struct wltx_pen_kb_dev_info {
	struct device *dev;
	struct notifier_block event_nb;
	struct work_struct event_work;
	struct wltx_acc_dev wireless_tx_acc[WLTX_ACC_INFO_PEN_KB_END];
	unsigned int event_type;
	int gpio_tx_pen_en;
	int gpio_tx_kb_en;
	int support_pen_kb_switch;
	int switch_state;
	unsigned int ping_timeout_pen;
	unsigned int ping_timeout_kb;
	bool pen_online;
	bool kb_online;
	bool pen_open_charging;
	bool kb_open_charging;
};

#endif /* _WIRELESS_TX_PEN_KB_H_ */
