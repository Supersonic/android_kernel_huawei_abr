/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * Description: ufs error recovery header file
 * Author: lida
 * Create: 2021-06-05
 */

#ifndef LINUX_UFS_ERROR_RECOVERY_H
#define LINUX_UFS_ERROR_RECOVERY_H

#include "ufshcd.h"
#include "ufs-vendor-cmd.h"

#ifdef CONFIG_SCSI_UFS_HS_ERROR_RECOVER

struct ufs_error_recovery {
	struct ufs_hba *hba;
	struct work_struct check_link_status;
	struct work_struct recover_hs_work;
	struct mutex check_init_status_mutex;
	struct wakeup_source *recover_wake_lock;
	unsigned int check_link_count;
	bool in_progress;
};

#define SLOW_STATE  2
#define CHECK_LINK_COUNT  10
#define DOORBELL_TIMEOUT_MS (10 * 1000)

void ufs_error_recovery_init(struct ufs_hba *hba);
void check_link_status(struct ufs_hba *hba);
void update_check_link_count(struct ufs_hba *hba);
#else
static inline void update_check_link_count(struct ufs_hba *hba){};
static inline void ufs_error_recovery_init(struct ufs_hba *hba){};
static inline void check_link_status(struct ufs_hba *hba){};
#endif
#endif
