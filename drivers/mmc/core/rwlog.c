/*
 *  linux/drivers/mmc/core/rwlog.c
 *
 *  Copyright (C) 2003 Russell King, All Rights Reserved.
 *  Copyright 2007 Pierre Ossman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  MMC sysfs/driver model support.
 */
#include <linux/debugfs.h>

#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include "rwlog.h"

static struct dentry *dentry_mmclog;
u64 rwlog_enable_flag = 0;   /* 0 : Disable , 1: Enable */
u64 rwlog_index = 0;     /* device index, 0: for emmc */
int mmc_debug_mask = 0;


static int rwlog_enable_set(void *data, u64 val)
{
	rwlog_enable_flag = val;
	return 0;
}
static int rwlog_enable_get(void *data, u64 *val)
{
	*val = rwlog_enable_flag;
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(rwlog_enable_fops,rwlog_enable_get, rwlog_enable_set, "%llu\n");

void rwlog_init(void)
{
	dentry_mmclog = debugfs_create_dir("hw_mmclog", NULL);
	if(dentry_mmclog ) {
		debugfs_create_file("rwlog_enable", S_IFREG|S_IRWXU|S_IRGRP|S_IROTH,
			dentry_mmclog, NULL, &rwlog_enable_fops);
	}
}

void mmc_start_request_rwlog(struct mmc_command *cmd)
{
	if (!cmd)
		return;

	if (1 == rwlog_enable_flag) {
		pr_err("HUAWEI-lifetime: [CMD%d] [0x%x] [FLAGS0x%x]\n", cmd->opcode, cmd->arg, cmd->flags);
	}
}

void mmc_start_cmdq_request_rwlog(struct mmc_request *mrq, u32 req_flags)
{
	if (!mrq || !mrq->data)
		return;

	if (1 == rwlog_enable_flag) {
		pr_err("HUAWEI-lifetime: [CMDQ%c] [0x%x] [%d] [DAT_TAG%d] [PRIO%d]\n",
			!!(req_flags & DIR)?'R':'W',
			mrq->data->blk_addr,
			mrq->data->blocks,
			!!(req_flags & MMC_DATA_DAT_TAG),
			!!(req_flags & MMC_DATA_PRIO));
	}
}

void mmc_start_dcmd_request_rwlog(struct mmc_request *mrq)
{
	if (!mrq || !mrq->cmd)
		return;

	if (1 == rwlog_enable_flag) {
		pr_err("HUAWEI-lifetime: [DCMD%d] [0x%x] [FLAGS0x%x]\n", mrq->cmd->opcode, mrq->cmd->arg, mrq->cmd->flags);
	}
}

