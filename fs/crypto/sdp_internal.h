/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Internal functions for per-f2fs sdp(sensitive data protection).
 * Create: 2021-04-23
 */

#ifndef SDP_INTERNAL_H
#define SDP_INTERNAL_H

#include <linux/types.h>
#include <uapi/linux/fscrypt.h>

#ifdef CONFIG_SDP_ENCRYPTION

#define FS_ENCRYPTION_CONTEXT_FORMAT_V2 2

#define F2FS_XATTR_SDP_ECE_ENABLE_FLAG 0x01

#define F2FS_XATTR_SDP_ECE_CONFIG_FLAG 0x02

#define F2FS_XATTR_SDP_SECE_ENABLE_FLAG 0x04

#define F2FS_XATTR_SDP_SECE_CONFIG_FLAG 0x08

#define F2FS_XATTR_DPS_ENABLE_FLAG 0x10

#define F2FS_SDP_SETUP_KEY_ENABLE_FLAG 0x40

static inline bool f2fs_inode_is_sdp_encrypted(u32 flag)
{
	return (flag & 0x0f) != 0;
}

static inline bool f2fs_inode_is_config_sdp_ece_encryption(u32 flag)
{
	return (flag & F2FS_XATTR_SDP_ECE_CONFIG_FLAG) != 0;
}

static inline bool f2fs_inode_is_config_sdp_sece_encryption(u32 flag)
{
	return (flag & F2FS_XATTR_SDP_SECE_CONFIG_FLAG) != 0;
}

static inline bool f2fs_inode_is_config_sdp_encryption(u32 flag)
{
	return (flag & F2FS_XATTR_SDP_SECE_CONFIG_FLAG) ||
		(flag & F2FS_XATTR_SDP_ECE_CONFIG_FLAG);
}

static inline bool f2fs_inode_is_enabled_sdp_ece_encryption(u32 flag)
{
	return (flag & F2FS_XATTR_SDP_ECE_ENABLE_FLAG) != 0;
}

static inline bool f2fs_inode_is_enabled_sdp_sece_encryption(u32 flag)
{
	return (flag & F2FS_XATTR_SDP_SECE_ENABLE_FLAG) != 0;
}

static inline bool f2fs_inode_is_enabled_sdp_encryption(u32 flag)
{
	return (flag & F2FS_XATTR_SDP_ECE_ENABLE_FLAG) ||
		(flag & F2FS_XATTR_SDP_SECE_ENABLE_FLAG);
}

static inline bool f2fs_inode_is_enabled_setup_key(u32 flag)
{
	return (flag & F2FS_SDP_SETUP_KEY_ENABLE_FLAG) != 0;
}

static inline bool f2fs_inode_is_enabled_dps(u32 flag)
{
	return (flag & F2FS_XATTR_DPS_ENABLE_FLAG) != 0;
}

static inline u8 f2fs_inode_setup_key_flag(u8 flag)
{
	bool is_enable_setup_key;
	is_enable_setup_key = f2fs_inode_is_sdp_encrypted(flag) ||
		f2fs_inode_is_enabled_dps(flag);
	return is_enable_setup_key ? F2FS_SDP_SETUP_KEY_ENABLE_FLAG : 0;
}

#define ON_LOCK_SCREEN_STATE 0
#define ON_UNLOCK_SCREEN_STATE 1

#define FSCRYPT_CE_CLASS 1
#define FSCRYPT_SDP_ECE_CLASS 2
#define FSCRYPT_SDP_SECE_CLASS 3
#ifdef CONFIG_HWDPS
#define FSCRYPT_DPS_CLASS 4
#endif

#define ENHANCED_CHECK_KEYING 1
#define NO_NEED_TO_CHECK_KEYFING 0
struct fscrypt_sdp_key {
	u32 version;
	u32 sdp_class;
	u32 mode;
	int screen_lock_state; // screen lock or unlock state
	u32 size;
	u8 identifier[FSCRYPT_KEY_IDENTIFIER_SIZE];
} __packed;

struct sdp_fscrypt_context {
	u8 version;
	u8 sdp_class;
	u8 format;
	u8 contents_encryption_mode;
	u8 filenames_encryption_mode;
	u8 flags;
	u8 master_key_descriptor[FSCRYPT_KEY_DESCRIPTOR_SIZE];
} __packed;
#endif
#endif
