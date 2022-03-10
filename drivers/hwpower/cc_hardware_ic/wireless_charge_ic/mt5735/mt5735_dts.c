// SPDX-License-Identifier: GPL-2.0
/*
 * mt5735_dts.c
 *
 * mt5735 dts driver
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

#include "mt5735.h"

#define HWLOG_TAG wireless_mt5735_dts
HWLOG_REGIST();

#define MT5735_CAP_CFG_LEN                3

static void mt5735_parse_tx_fod(const struct device_node *np,
	struct mt5735_dev_info *di)
{
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"tx_ploss_th0", (u32 *)&di->tx_fod.ploss_th0,
		MT5735_TX_PLOSS_TH0_VAL);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"tx_ploss_th1", (u32 *)&di->tx_fod.ploss_th1,
		MT5735_TX_PLOSS_TH1_VAL);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"tx_ploss_th2", (u32 *)&di->tx_fod.ploss_th2,
		MT5735_TX_PLOSS_TH2_VAL);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"tx_ploss_th3", (u32 *)&di->tx_fod.ploss_th3,
		MT5735_TX_PLOSS_TH3_VAL);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"tx_hp_ploss_th0", (u32 *)&di->tx_fod.hp_ploss_th0,
		MT5735_TX_HP_PLOSS_TH0_VAL);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"tx_hp_ploss_th1", (u32 *)&di->tx_fod.hp_ploss_th1,
		MT5735_TX_HP_PLOSS_TH1_VAL);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"tx_ploss_cnt", (u32 *)&di->tx_fod.ploss_cnt,
		MT5735_TX_PLOSS_CNT_VAL);
}

static int mt5735_parse_ldo_cfg(const struct device_node *np,
	struct mt5735_dev_info *di)
{
	if (power_dts_read_u8_array(power_dts_tag(HWLOG_TAG), np,
		"rx_ldo_cfg_5v", di->rx_ldo_cfg_5v, MT5735_RX_LDO_CFG_LEN))
		return -EINVAL;

	if (power_dts_read_u8_array(power_dts_tag(HWLOG_TAG), np,
		"rx_ldo_cfg_9v", di->rx_ldo_cfg_9v, MT5735_RX_LDO_CFG_LEN))
		return -EINVAL;

	if (power_dts_read_u8_array(power_dts_tag(HWLOG_TAG), np,
		"rx_ldo_cfg_12v", di->rx_ldo_cfg_12v, MT5735_RX_LDO_CFG_LEN))
		return -EINVAL;

	if (power_dts_read_u8_array(power_dts_tag(HWLOG_TAG), np,
		"rx_ldo_cfg_sc", di->rx_ldo_cfg_sc, MT5735_RX_LDO_CFG_LEN))
		return -EINVAL;

	return 0;
}

static void mt5735_parse_rx_ask_mod_cfg(const struct device_node *np,
	struct mt5735_dev_info *di)
{
	if (power_dts_read_u8_array(power_dts_tag(HWLOG_TAG), np,
		"rx_volt_cap_cfg", (u8 *)&di->mod_cfg, MT5735_CAP_CFG_LEN)) {
		di->mod_cfg.l_volt = MT5735_ALL_CAP_POSITIVE;
		di->mod_cfg.m_volt = MT5735_BOTH_CAP_NEGATIVE;
		di->mod_cfg.h_volt = MT5735_BOTH_CAP_NEGATIVE;
		di->mod_cfg.fac_l_volt = MT5735_ALL_CAP_POSITIVE;
		di->mod_cfg.fac_m_volt = MT5735_ALL_CAP_NEGATIVE;
		di->mod_cfg.fac_h_volt = MT5735_ALL_CAP_NEGATIVE;
	}
	hwlog_info("[parse_rx_ask_mod_cfg] l_volt=0x%x m_volt=0x%x h_volt=0x%x\t"
		"fac_l_volt=0x%x fac_m_volt=0x%x fac_h_volt=0x%x\n",
		di->mod_cfg.l_volt, di->mod_cfg.m_volt, di->mod_cfg.h_volt,
		di->mod_cfg.fac_l_volt, di->mod_cfg.fac_m_volt, di->mod_cfg.fac_h_volt);
}

int mt5735_parse_dts(const struct device_node *np,
	struct mt5735_dev_info *di)
{
	int ret;

	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"rx_ss_good_lth", (u32 *)&di->rx_ss_good_lth,
		MT5735_RX_SS_MAX);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"gpio_en_valid_val", (u32 *)&di->gpio_en_valid_val,
		WLTRX_IC_EN_ENABLE);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"full_bridge_ith", (u32 *)&di->full_bridge_ith,
		MT5735_TX_FULL_BRIDGE_ITH);
	(void)power_dts_read_u32(power_dts_tag(HWLOG_TAG), np,
		"mtp_check_delay", &di->mtp_check_delay.user,
		WIRELESS_FW_WORK_DELAYED_TIME);
	di->mtp_check_delay.fac = WIRELESS_FW_WORK_DELAYED_TIME;

	mt5735_parse_tx_fod(np, di);

	ret = wlrx_parse_fod_cfg(di->ic_type, np, MT5735_RX_FOD_LEN);
	if (ret) {
		hwlog_err("parse_dts: parse fod para failed\n");
		return ret;
	}

	ret = mt5735_parse_ldo_cfg(np, di);
	if (ret) {
		hwlog_err("parse_dts: parse ldo cfg failed\n");
		return ret;
	}

	mt5735_parse_rx_ask_mod_cfg(np, di);

	return 0;
}
