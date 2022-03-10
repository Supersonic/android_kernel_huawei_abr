/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * Description: ufs private driver interface header file
 * Author: zhangjinkai
 * Create: 2021-3-18
 */

#ifndef LINUX_UFS_VENOR_EXTERN_H
#define LINUX_UFS_VENOR_EXTERN_H

#include "ufshcd.h"

#ifdef CONFIG_HUAWEI_UFS_VENDOR_MODE
int __ufshcd_queuecommand_directly(struct ufs_hba *hba,
				struct scsi_cmnd *cmd,
				unsigned int timeout);
int ufshcd_queuecommand_directly(struct ufs_hba *hba, struct scsi_cmnd *cmd,
				unsigned int timeout, bool eh_handle);
#else
static inline  __attribute__((unused)) int
__ufshcd_queuecommand_directly(struct ufs_hba *hba,
				struct scsi_cmnd *cmd,
				unsigned int timeout
				__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) int
ufshcd_queuecommand_directly(struct ufs_hba *hba, struct scsi_cmnd *cmd,
				unsigned int timeout, bool eh_handle
				__attribute__((unused)))
{
	return 0;
}
#endif

#endif
