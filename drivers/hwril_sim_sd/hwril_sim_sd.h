/*
 * hwril_sim_sd.h
 *
 * generic netlink for hwril-sim-sd
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

#ifndef __HWRIL_SIM_SD_H
#define __HWRIL_SIM_SD_H
#include <linux/types.h>

#define HWRIL_SIM_SD_GENL_FAMILY "hwril-sim-sd"

#define GET_NMC_DETECT_RESULT 0
#define SIM_DETECT_RESULT_NOTIFY 1

typedef int (*sim_nmc_handler)(int cmd, int param);

enum {
	HWRIL_SIM_SD_CMD_UNSPEC,
	HWRIL_SIM_SD_CMD_GET_CARD_TYPE,
	HWRIL_SIM_SD_CMD_SIM_DET_RET_NOTIFY,
	HWRIL_SIM_SD_CMD_NMC_DET_CARD_TYPE_NOTIFY,

	__HWRIL_SIM_SD_CMD_MAX,
};

#define HWRIL_SIM_SD_CMD_MAX (__HWRIL_SIM_SD_CMD_MAX - 1)

enum {
	HWRIL_SIM_SD_ATTR_UNSPEC,
	HWRIL_SIM_SD_ATTR_SIM_DET_RESULT,
	HWRIL_SIM_SD_ATTR_NMC_DET_CARD_TYPE,
	HWRIL_SIM_SD_ATTR_PAD,
	__HWRIL_SIM_SD_ATTR_MAX,
};

#define HWRIL_SIM_SD_ATTR_MAX (__HWRIL_SIM_SD_ATTR_MAX - 1)

int hwril_sim_sd_nmc_detect_result_notify(u32 card_type);
int register_sim_sd_handler(sim_nmc_handler cb);

#endif /* __HWRIL_SIM_SD_H */
