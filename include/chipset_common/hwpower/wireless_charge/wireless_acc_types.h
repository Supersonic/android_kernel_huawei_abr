/* SPDX-License-Identifier: GPL-2.0 */
/*
 * wireless_acc_types.h
 *
 * accessory(tx,cable,adapter etc.) types for wireless charging
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

#ifndef _WIRELESS_ACC_TYPES_H_
#define _WIRELESS_ACC_TYPES_H_

#define WLACC_ADP_FAC_BASE              0x20 /* tx for factory */
#define WLACC_ADP_FAC_MAX               0x3F
#define WLACC_ADP_CAR_BASE              0x40 /* tx for car */
#define WLACC_ADP_CAR_MAX               0x5F
#define WLACC_ADP_PWR_BANK_BASE         0x60 /* tx for power bank */
#define WLACC_ADP_PWR_BANK_MAX          0x7F

enum wlacc_adp_type {
	WLACC_ADP_UNKOWN   = 0x00,
	WLACC_ADP_SDP      = 0x01,
	WLACC_ADP_CDP      = 0x02,
	WLACC_ADP_NON_STD  = 0x03,
	WLACC_ADP_DCP      = 0x04,
	WLACC_ADP_FCP      = 0x05,
	WLACC_ADP_SCP      = 0x06,
	WLACC_ADP_PD       = 0x07,
	WLACC_ADP_QC       = 0x08,
	WLACC_ADP_OTG_A    = 0x09, /* rvs charging powered by battery */
	WLACC_ADP_OTG_B    = 0x0A, /* rvs charging powered by adaptor */
	WLACC_ADP_ERR      = 0xFF,
};

enum wlacc_tx_type {
	WLACC_TX_UNKNOWN = 0,
	WLACC_TX_CP85,
	WLACC_TX_CP60,
	WLACC_TX_CP61,
	WLACC_TX_CP62_LX,
	WLACC_TX_CP62_XW,
	WLACC_TX_CP62R,
	WLACC_TX_CP39S,
	WLACC_TX_CP39S_HK,
	WLACC_TX_CK30_LX,
	WLACC_TX_CK30_LP,
	WLACC_TX_ERROR = 0xFF,
};

static inline bool wlacc_is_err_tx(int type)
{
	return type == WLACC_ADP_ERR;
}

static inline bool wlacc_is_fac_tx(int type)
{
	return (type >= WLACC_ADP_FAC_BASE) && (type <= WLACC_ADP_FAC_MAX);
}

static inline bool wlacc_is_car_tx(int type)
{
	return (type >= WLACC_ADP_CAR_BASE) && (type <= WLACC_ADP_CAR_MAX);
}

static inline bool wlacc_is_rvs_tx(int type)
{
	return (type == WLACC_ADP_OTG_A) || (type == WLACC_ADP_OTG_B);
}

static inline bool wlacc_is_qc_adp(int type)
{
	return type == WLACC_ADP_QC;
}

#endif /* _WIRELESS_ACC_TYPES_H_ */
