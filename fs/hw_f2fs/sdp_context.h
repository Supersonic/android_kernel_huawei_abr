/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Implementation of set/get sdp flag and context.
 * Create: 2021-05-21
 */

#ifndef SDP_CONTEXT_H
#define SDP_CONTEXT_H

#include <linux/fs.h>
#include <linux/f2fs_fs.h>
#include <f2fs.h>
#include "sdp.h"

#ifdef CONFIG_SDP_ENCRYPTION

int f2fs_get_sdp_context(struct inode *inode, void *ctx, size_t len,
	void *fs_data);

int f2fs_set_sdp_context(struct inode *inode, const void *ctx,
	size_t len, void *fs_data);

int f2fs_get_sdp_encrypt_flags(struct inode *inode, void *fs_data,
	u32 *flags);

int f2fs_set_sdp_encrypt_flags(struct inode *inode, void *fs_data,
	u32 *flags);

#endif
#endif
