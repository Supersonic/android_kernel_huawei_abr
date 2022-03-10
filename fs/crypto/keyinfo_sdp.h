/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Implementation of 1) get sdp encryption key info;
 *                                2) update sdp context.
 * Create: 2021-05-22
 */

#ifndef KEYINFO_SDP_H
#define KEYINFO_SDP_H
#include <linux/fs.h>

#ifdef CONFIG_SDP_ENCRYPTION

int f2fs_change_to_sdp_crypto(struct inode *inode, void *fs_data,
	int sdp_class);

int f2fs_inode_check_sdp_keyring(const u8 *descriptor, u8 *key_identifier,
	int key_identifier_len, int enforce);

int f2fs_inode_get_sdp_encrypt_flags(struct inode *inode, void *fs_data,
	u32 *flag);

int f2fs_inode_set_sdp_encryption_flags(struct inode *inode, void *fs_data,
	u32 sdp_enc_flag);

#endif
#endif
