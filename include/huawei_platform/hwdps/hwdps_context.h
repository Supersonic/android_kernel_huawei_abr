/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Implementation of set/get hwdps flag and context.
 * Create: 2021-05-21
 */

#ifndef HWDPS_CONTEXT_H
#define HWDPS_CONTEXT_H

#ifdef CONFIG_HWDPS
#include <linux/fs.h>

s32 f2fs_get_hwdps_flags(struct inode *inode, void *fs_data, u32 *flags);

s32 f2fs_set_hwdps_flags(struct inode *inode, void *fs_data, u32 *flags);

#endif
#endif
