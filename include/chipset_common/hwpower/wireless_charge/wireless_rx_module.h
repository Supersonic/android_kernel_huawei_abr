/* SPDX-License-Identifier: GPL-2.0 */
/*
 * wireless_rx_module.h
 *
 * submodue init and deinit for wireless charging
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

#ifndef _WIRELESS_RX_MODULE_H_
#define _WIRELESS_RX_MODULE_H_

#include <chipset_common/hwpower/wireless_charge/wireless_trx_intf.h>

struct device;

#ifdef CONFIG_WIRELESS_CHARGER
int wlrx_evt_init(unsigned int drv_type, struct device *dev);
void wlrx_evt_deinit(unsigned int drv_type);
#else
static inline int wlrx_evt_init(unsigned int drv_type, struct device *dev)
{
	return -ENOTSUPP;
}

static inline void wlrx_evt_deinit(unsigned int drv_type)
{
}
#endif /* CONFIG_WIRELESS_CHARGER */

#endif /* _WIRELESS_RX_MODULE_H_ */
