/* SPDX-License-Identifier: GPL-2.0 */
/*
 * wireless_rx_platform.h
 *
 * platform interface for wireless charging
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

#ifndef _WIRELESS_RX_PLATFORM_
#define _WIRELESS_RX_PLATFORM_

#include <huawei_platform/power/wireless/wireless_charge_ictrl.h>
#include <linux/types.h>

bool wlrx_bst_rst_completed(void);

static inline int wlrx_buck_get_iin_regval(void)
{
	return wlrx_get_charger_iinlim_regval();
}

#endif /* _WIRELESS_RX_PLATFORM_ */