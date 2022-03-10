// SPDX-License-Identifier: GPL-2.0
/*
 * wireless_rx_plim.c
 *
 * power limit for wireless charging
 *
 * Copyright (c) 2020-2021 Huawei Technologies Co., Ltd.
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

#include <chipset_common/hwpower/wireless_charge/wireless_rx_plim.h>
#include <chipset_common/hwpower/wireless_charge/wireless_rx_pctrl.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <linux/slab.h>

#define HWLOG_TAG wireless_rx_plim
HWLOG_REGIST();

struct wlrx_plim  {
	int src_id;
	const char *src_name;
	bool need_rst; /* reset para when recharging */
	int vtx; /* mV */
	int vrx; /* mV */
	int irx; /* mA */
};

static struct wlrx_plim const g_plim_tbl[WLRX_PLIM_SRC_END] = {
	{ WLRX_PLIM_SRC_OTG,        "otg",        false, 5000,  5500,  1000 },
	{ WLRX_PLIM_SRC_RPP,        "rpp",        true,  12000, 12000, 1300 },
	{ WLRX_PLIM_SRC_FAN,        "fan",        true,  9000,  9900,  1250 },
	{ WLRX_PLIM_SRC_VOUT_ERR,   "vout_err",   true,  9000,  9900,  1250 },
	{ WLRX_PLIM_SRC_TX_BST_ERR, "tx_bst_err", true,  5000,  5500,  1000 },
	{ WLRX_PLIM_SRC_KB,         "keyboard",   true,  9000,  9900,  1100 },
	{ WLRX_PLIM_SRC_THERMAL,    "thermal",    false, 9000,  9900,  1250 },
};

struct wlrx_plim_dev {
	const struct wlrx_plim *tbl;
	unsigned long cur_src;
};

static struct wlrx_plim_dev *g_rx_plim_di[WLTRX_DRV_MAX];

static struct wlrx_plim_dev *wlrx_plim_get_di(unsigned int drv_type)
{
	if (!wltrx_is_drv_type_valid(drv_type)) {
		hwlog_err("get_di: drv_type=%u err\n", drv_type);
		return NULL;
	}

	return g_rx_plim_di[drv_type];
}

unsigned int wlrx_plim_get_src(unsigned int drv_type)
{
	struct wlrx_plim_dev *di = wlrx_plim_get_di(drv_type);

	if (!di)
		return 0;

	return (unsigned int)di->cur_src;
}

void wlrx_plim_set_src(unsigned int drv_type, unsigned int src_id)
{
	struct wlrx_plim_dev *di = wlrx_plim_get_di(drv_type);

	if (!di || (src_id < WLRX_PLIM_SRC_BEGIN) || (src_id >= WLRX_PLIM_SRC_END))
		return;

	if (test_bit(src_id, &di->cur_src))
		return;
	set_bit(src_id, &di->cur_src);
	if (src_id != di->tbl[src_id].src_id)
		return;
	hwlog_info("[set_src] %s\n", di->tbl[src_id].src_name);
}

void wlrx_plim_clear_src(unsigned int drv_type, unsigned int src_id)
{
	struct wlrx_plim_dev *di = wlrx_plim_get_di(drv_type);

	if (!di || (src_id < WLRX_PLIM_SRC_BEGIN) || (src_id >= WLRX_PLIM_SRC_END))
		return;

	if (!test_bit(src_id, &di->cur_src))
		return;
	clear_bit(src_id, &di->cur_src);
	if (src_id != di->tbl[src_id].src_id)
		return;
	hwlog_info("[clear_src] %s\n", di->tbl[src_id].src_name);
}

void wlrx_plim_reset_src(unsigned int drv_type)
{
	int i;
	struct wlrx_plim_dev *di = wlrx_plim_get_di(drv_type);

	if (!di)
		return;

	for (i = WLRX_PLIM_SRC_BEGIN; i < WLRX_PLIM_SRC_END; i++) {
		if (di->tbl[i].need_rst)
			clear_bit(i, &di->cur_src);
	}
}

void wlrx_plim_update_pctrl(unsigned int drv_type, struct wlrx_pctrl *pctrl)
{
	int i;
	struct wlrx_plim_dev *di = wlrx_plim_get_di(drv_type);

	if (!di || !pctrl)
		return;

	for (i = WLRX_PLIM_SRC_BEGIN; i < WLRX_PLIM_SRC_END; i++) {
		if (!test_bit(i, &di->cur_src))
			continue;
		pctrl->vtx = min(pctrl->vtx, di->tbl[i].vtx);
		pctrl->vrx = min(pctrl->vrx, di->tbl[i].vrx);
		pctrl->irx = min(pctrl->irx, di->tbl[i].irx);
	}
}

static void wlrx_plim_kfree_dev(struct wlrx_plim_dev *di)
{
	if (!di)
		return;

	kfree(di);
}

int wlrx_plim_init(unsigned int drv_type, const struct device_node *np)
{
	struct wlrx_plim_dev *di = NULL;

	if (!np || !wltrx_is_drv_type_valid(drv_type))
		return -ENODEV;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	di->tbl = &g_plim_tbl[0];
	g_rx_plim_di[drv_type] = di;
	return 0;
}

void wlrx_plim_deinit(unsigned int drv_type)
{
	if (!wltrx_is_drv_type_valid(drv_type))
		return;

	wlrx_plim_kfree_dev(g_rx_plim_di[drv_type]);
	g_rx_plim_di[drv_type] = NULL;
}
