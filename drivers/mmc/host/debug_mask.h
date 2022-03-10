/*
 *  linux/drivers/mmc/host/debug_mask.h
 *
 *  Copyright (C) 2003 Russell King, All Rights Reserved.
 *  Copyright 2007 Pierre Ossman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _MMC_HOST_DEBUGMASK_H
#define _MMC_HOST_DEBUGMASK_H

extern int mmc_debug_mask;
module_param_named(debug_mask, mmc_debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP);

#define HUAWEI_DBG(fmt, args...) \
	do { \
		if (mmc_debug_mask) \
			printk(fmt, args); \
	} while (0)

static inline void sdhci_request_start_log(struct sdhci_host *host, struct mmc_request *mrq)
{
	if(mmc_debug_mask)
		printk("HUAWEI %s: starting CMD%u arg %08x flags %08x\n",
				mmc_hostname(host->mmc), mrq->cmd->opcode,
				mrq->cmd->arg, mrq->cmd->flags);
}

static inline void sdhci_request_end_log(struct sdhci_host *host, struct mmc_request *mrq)
{
	if(mmc_debug_mask)
		printk("HUAWEI %s: req end (CMD%u): %08x %08x %08x %08x\n",
			mmc_hostname(host->mmc), mrq->cmd->opcode,
			mrq->cmd->resp[0], mrq->cmd->resp[1],
			mrq->cmd->resp[2], mrq->cmd->resp[3]);
}

static inline void sdhci_set_ios_log(struct mmc_host *mmc, struct mmc_ios *ios)
{
	if(mmc_debug_mask)
		printk("%s: clock %uHz busmode %u powermode %u cs %u Vdd %u "
			"width %u timing %u\n",
			mmc_hostname(mmc), ios->clock, ios->bus_mode,
			ios->power_mode, ios->chip_select, ios->vdd,
			ios->bus_width, ios->timing);
}

#endif
