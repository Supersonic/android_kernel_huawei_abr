/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: ufs vcmd implement
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "ufs_vcmd_proc.h"

#include <asm/unaligned.h>

/* vcmd request timeout */
#define VCMD_REQ_TIMEOUT 3000 /* 3 seconds */

#define VCMD_RW_BUFFER_SET 0x1

static int ufshcd_rw_buffer_vcmd_retry(struct scsi_device *dev,
	struct ufs_rw_buffer_vcmd *vcmd, int data_direction)
{
	int i;
	int ret = -EINVAL;
	unsigned char cmd[RW_BUFFER_CDB_LEN] = { 0 };

	cmd[RW_BUFFER_OPCODE_OFFSET] = vcmd->opcode;
	cmd[RW_BUFFER_MODE_OFFSET] = RW_BUFFER_MODE;
	cmd[RW_BUFFER_BUFFER_ID_OFFSET] = vcmd->buffer_id;

	for (i = 0; i < RW_BUFFER_1ST_RESERVED_LEN; ++i)
		cmd[RW_BUFFER_1ST_RESERVED_OFFSET + i] = vcmd->reserved_1st[i];

	put_unaligned_be16(vcmd->buffer_len, cmd + RW_BUFFER_LEN_OFFSET);

	for (i = 0; i < RW_BUFFER_2ND_RESERVED_LEN; ++i)
		cmd[RW_BUFFER_2ND_RESERVED_OFFSET + i] = vcmd->reserved_2nd[i];

	for (i = 0; i < vcmd->retries; ++i) {
		ret = scsi_unistore_execute(dev, cmd, data_direction,
			vcmd->buffer, vcmd->buffer_len, VCMD_REQ_TIMEOUT,
			vcmd->retries, vcmd->done);
		if (!ret)
			break;
	}

	return ret;
}

int ufshcd_read_buffer_vcmd_retry(struct scsi_device *dev,
	struct ufs_rw_buffer_vcmd *vcmd)
{
	vcmd->opcode = READ_BUFFER;

	return ufshcd_rw_buffer_vcmd_retry(dev, vcmd, DMA_FROM_DEVICE);
}

int ufshcd_write_buffer_vcmd_retry(struct scsi_device *dev,
	struct ufs_rw_buffer_vcmd *vcmd)
{
	vcmd->opcode = WRITE_BUFFER;

	return ufshcd_rw_buffer_vcmd_retry(dev, vcmd, DMA_TO_DEVICE);
}

bool ufshcd_rw_buffer_is_enabled(struct ufs_hba *hba)
{
	if (!hba || !hba->host || !hba->host->unistore_enable)
		return false;

	return (hba->host->vcmd_set == VCMD_RW_BUFFER_SET);
}