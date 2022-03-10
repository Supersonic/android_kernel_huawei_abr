/*
 * wireless_charge_ictrl.c
 *
 * wireless charge driver, function as buck iout ctrl
 *
 * Copyright (c) 2020-2020 Huawei Technologies Co., Ltd.
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

#include <linux/delay.h>
#include <linux/slab.h>
#include <chipset_common/hwpower/common_module/power_common_macro.h>
#include <chipset_common/hwpower/common_module/power_printk.h>
#include <chipset_common/hwpower/common_module/power_supply_interface.h>
#include <huawei_platform/power/wireless/wireless_charger.h>
#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
#include <huawei_platform/hwpower/common_module/power_glink.h>
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

#define HWLOG_TAG wireless_charge_ictrl
HWLOG_REGIST();

static struct {
	int iin_step;
	int iin_delay;
	int iin_now;
	bool iset_commplete;
} g_wlrx_ictrl;

#if IS_ENABLED(CONFIG_QTI_PMIC_GLINK)
static void wlrx_set_dev_iin(int iin_ma)
{
	u32 data[GLINK_DATA_THREE];
	u32 id = POWER_GLINK_PROP_ID_SET_USB_ICL;

	data[GLINK_DATA_ZERO] = ICL_AGGREGATOR_WLS;
	data[GLINK_DATA_ONE] = AGGREGATOR_VOTE_ENABLE;
	data[GLINK_DATA_TWO] = iin_ma;

	(void)power_glink_set_property_value(id, data, GLINK_DATA_THREE);
}
#else
static void wlrx_set_dev_iin(int iin_ma)
{
	struct power_supply *psy = power_supply_get_by_name("huawei_charge");

	if (!psy) {
		hwlog_err("set_dev_iin: huawei_charge psy is null\n");
		return;
	}

	(void)power_supply_set_int_property_value_with_psy(psy,
		POWER_SUPPLY_PROP_WLRX_ICL, iin_ma);
}
#endif /* IS_ENABLED(CONFIG_QTI_PMIC_GLINK) */

void wlrx_set_iin_prop(int iin_step, int iin_delay)
{
	g_wlrx_ictrl.iin_delay = iin_delay;
	g_wlrx_ictrl.iin_step = iin_step;
	g_wlrx_ictrl.iin_now = 100; /* generally, default iout is 100mA */

	if ((iin_step <= 0) || (iin_delay <= 0))
		wlrx_set_dev_iin(2000); /* default max icl: 2A */
	hwlog_info("[set_iin_prop] delay=%d step=%d\n",
		g_wlrx_ictrl.iin_delay, g_wlrx_ictrl.iin_step);
}

static void wlrx_set_iin_step_by_step(int iin)
{
	int iin_tmp = g_wlrx_ictrl.iin_now;

	g_wlrx_ictrl.iset_commplete = false;
	if (iin > g_wlrx_ictrl.iin_now) {
		do {
			wlrx_set_dev_iin(iin_tmp);
			g_wlrx_ictrl.iin_now = iin_tmp;
			iin_tmp += g_wlrx_ictrl.iin_step;
			if (!g_wlrx_ictrl.iset_commplete)
				msleep(g_wlrx_ictrl.iin_delay);
		} while ((iin_tmp < iin) && !g_wlrx_ictrl.iset_commplete &&
			g_wlrx_ictrl.iin_delay && g_wlrx_ictrl.iin_step);
	}
	if (iin < g_wlrx_ictrl.iin_now) {
		do {
			wlrx_set_dev_iin(iin_tmp);
			g_wlrx_ictrl.iin_now = iin_tmp;
			iin_tmp -= g_wlrx_ictrl.iin_step;
			if (!g_wlrx_ictrl.iset_commplete)
				msleep(g_wlrx_ictrl.iin_delay);
		} while ((iin_tmp > iin) && !g_wlrx_ictrl.iset_commplete &&
			g_wlrx_ictrl.iin_delay && g_wlrx_ictrl.iin_step);
	}
	if (!g_wlrx_ictrl.iset_commplete) {
		g_wlrx_ictrl.iin_now = iin;
		wlrx_set_dev_iin(iin);
		g_wlrx_ictrl.iset_commplete = true;
	}
}

static void wlrx_set_iin(int iin)
{
	if ((iin <= 0) || (g_wlrx_ictrl.iin_delay <= 0) ||
		(g_wlrx_ictrl.iin_step <= 0)) {
		hwlog_err("%s: iin=%d delay=%d step=%d\n",
			__func__, iin, g_wlrx_ictrl.iin_delay, g_wlrx_ictrl.iin_step);
		return;
	}

	if (!g_wlrx_ictrl.iset_commplete) {
		g_wlrx_ictrl.iset_commplete = true;
		/* delay double time for completion */
		msleep(2 * g_wlrx_ictrl.iin_delay);
	}
	wlrx_set_iin_step_by_step(iin);
}

void wlrx_set_rx_iout_limit(int ilim)
{
	wlrx_set_iin(ilim);
}

int wlrx_get_charger_iinlim_regval(void)
{
	return g_wlrx_ictrl.iin_now;
}
