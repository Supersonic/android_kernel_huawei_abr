/* SPDX-License-Identifier: GPL-2.0 */
/*
 * wireless_tx_dts.h
 *
 * parse dts head file for wireless reverse charging
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

#ifndef _WIRELESS_TX_DTS_H_
#define _WIRELESS_TX_DTS_H_

struct wltx_dts {
	int pwr_type;
	int high_pwr_soc;
	int vbus_change_type;
};

struct device_node;

#ifdef CONFIG_WIRELESS_CHARGER
struct wltx_dts *wltx_get_dts(void);
int wltx_parse_dts(const struct device_node *np);
void wltx_kfree_dts(void);
#else
static inline struct wltx_dts *wltx_get_dts(void)
{
	return NULL;
}

static inline int wltx_parse_dts(const struct device_node *np)
{
	return -EINVAL;
}

static inline void wltx_kfree_dts(void)
{
}
#endif /* CONFIG_WIRELESS_CHARGER */

#endif /* _WIRELESS_TX_DTS_H_ */
