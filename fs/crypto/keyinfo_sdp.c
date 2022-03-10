/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Implementation of 1) get sdp encryption key info;
 *                                2) update sdp context.
 * Create: 2021-05-22
 */

#include "keyinfo_sdp.h"
#include <linux/string.h>
#include <uapi/linux/keyctl.h>
#include <keys/user-type.h>
#include <securec.h>
#include "fscrypt_private.h"
#include "sdp_internal.h"

#ifdef CONFIG_HWDPS
#include <huawei_platform/hwdps/hwdps_fs_hooks.h>
#endif

#define HEX_STR_PER_BYTE 2

static void f2fs_put_crypt_info(struct fscrypt_info *ci)
{
	struct key *key = NULL;

	if (!ci)
		return;

	if (ci->ci_direct_key)
		fscrypt_put_direct_key(ci->ci_direct_key);
	else if (ci->ci_owns_key)
		fscrypt_destroy_prepared_key(&ci->ci_key);

	key = ci->ci_master_key;
	if (key) {
		struct fscrypt_master_key *mk = key->payload.data[0];

		/*
		 * Remove this inode from the list of inodes that were unlocked
		 * with the master key.
		 *
		 * In addition, if we're removing the last inode from a key that
		 * already had its secret removed, invalidate the key so that it
		 * gets removed from ->s_master_keys.
		 */
		spin_lock(&mk->mk_decrypted_inodes_lock);
		list_del(&ci->ci_master_key_link);
		spin_unlock(&mk->mk_decrypted_inodes_lock);
		if (refcount_dec_and_test(&mk->mk_refcount))
			key_invalidate(key);
		key_put(key);
	}
	memzero_explicit(ci, sizeof(*ci));
	kmem_cache_free(fscrypt_info_cachep, ci);
}

#ifdef CONFIG_HWDPS
static int get_dps_crypt_info(struct inode *inode)
{
	int res;
	union fscrypt_context ctx = {0};
	struct fscrypt_info *crypt_info = NULL;

	res = inode->i_sb->s_cop->get_context(inode, &ctx, sizeof(ctx));
	if (res != sizeof(union fscrypt_context)) {
		pr_err("%s: get context failed, res:%d\n", __func__, res);
		return -EINVAL;
	}

	crypt_info = kmem_cache_zalloc(fscrypt_info_cachep, GFP_NOFS);
	if (!crypt_info)
		return -ENOMEM;

	res = fscrypt_policy_from_context(&crypt_info->ci_policy,
		&ctx, sizeof(ctx));
	if (res != 0)
		goto out;

	if (crypt_info->ci_policy.version != FSCRYPT_POLICY_V2) {
		res = -EINVAL;
		goto out;
	}

	crypt_info->ci_hw_enc_flag |= F2FS_XATTR_DPS_ENABLE_FLAG;
	(void)memcpy_s(crypt_info->ci_nonce, sizeof(crypt_info->ci_nonce),
		fscrypt_context_nonce(&ctx), FS_KEY_DERIVATION_NONCE_SIZE);

	if (cmpxchg(&inode->i_crypt_info, NULL, crypt_info) == NULL)
		crypt_info = NULL;

out:
	f2fs_put_crypt_info(crypt_info);
	return res;
}

int hwdps_check_inode(struct inode *inode)
{
	int res;
	u32 flags;
	res = hwdps_check_support(inode, &flags);
	if (res != 0) {
		return res;
	}
	res = hwdps_has_access(inode, flags);
	if (res != 0)
		return res;
	if (!inode->i_crypt_info)
		res = get_dps_crypt_info(inode);

	return res;
}
#endif

int f2fs_inode_get_sdp_encrypt_flags(struct inode *inode, void *fs_data,
	u32 *flag)
{
	int res;

	if (!inode || !flag) /* fs_data can be null, no need to check */
		return -EINVAL;

	res = inode->i_sb->s_cop->get_sdp_encrypt_flags(inode, fs_data, flag);
	if (res != 0)
		pr_err("%s: get flag failed, res:%d!\n", __func__, res);
	return res;
}

int f2fs_inode_set_sdp_encryption_flags(struct inode *inode, void *fs_data,
	u32 sdp_enc_flag)
{
	int res;
	u32 flags;

	if ((sdp_enc_flag & (~0x0F)) != 0)
		return -EINVAL;

	res = inode->i_sb->s_cop->get_sdp_encrypt_flags(inode, fs_data, &flags);
	if (res != 0)
		return res;

	flags |= sdp_enc_flag;
	res = inode->i_sb->s_cop->set_sdp_encrypt_flags(inode, fs_data, &flags);
	if (res != 0)
		pr_err("%s: set flag failed, res:%d!\n", __func__, res);
	return res;
}

static int get_context_all(struct inode *inode,
	struct sdp_fscrypt_context *sdp_ctx,
	union fscrypt_context *ctx,
	void *fs_data)
{
	int res;

	res = inode->i_sb->s_cop->get_sdp_context(inode, sdp_ctx,
		sizeof(struct sdp_fscrypt_context), fs_data);
	if (res != sizeof(struct sdp_fscrypt_context)) {
		pr_err("%s: get sdp context failed, res:%d\n", __func__, res);
		return -EINVAL;
	}
	res = inode->i_sb->s_cop->get_context(inode, ctx,
		sizeof(union fscrypt_context));
	if (res != sizeof(union fscrypt_context)) {
		pr_err("%s: get context failed, res:%d\n", __func__, res);
		return -EINVAL;
	}
	return 0;
}
static int check_and_set_sdp_flags(struct inode *inode, void *fs_data,
	u8 sdp_class)
{
	int res;
	u32 flag;

	res = f2fs_inode_get_sdp_encrypt_flags(inode, fs_data, &flag);
	if (res != 0) {
		pr_err("%s: get flags failed, res:%d", __func__, res);
		return res;
	}
	if (f2fs_inode_is_enabled_sdp_ece_encryption(flag) ||
		f2fs_inode_is_enabled_sdp_sece_encryption(flag))
		return res;

	flag = (sdp_class == FSCRYPT_SDP_ECE_CLASS) ?
		F2FS_XATTR_SDP_ECE_ENABLE_FLAG : F2FS_XATTR_SDP_SECE_ENABLE_FLAG;

	res = f2fs_inode_set_sdp_encryption_flags(inode, fs_data, flag);
	if (res != 0) {
		pr_err("%s: set sdp flag:%u failed! need to check!\n",
			__func__, flag);
		return res;
	}
	return res;
}

static int f2fs_get_sdp_ece_crypt_info(struct inode *inode, void *fs_data)
{
	int res;
	union fscrypt_context ctx = {0};
	struct sdp_fscrypt_context sdp_ctx = {0};
	struct fscrypt_info *crypt_info = NULL;

	res = get_context_all(inode, &sdp_ctx, &ctx, fs_data);
	if (res != 0)
		return res;

	crypt_info = kmem_cache_zalloc(fscrypt_info_cachep, GFP_NOFS);
	if (!crypt_info)
		return -ENOMEM;

	res = fscrypt_policy_from_context(&crypt_info->ci_policy,
		&ctx, sizeof(ctx));
	if (res != 0)
		goto out;

	if (sdp_ctx.sdp_class != FSCRYPT_SDP_ECE_CLASS ||
		crypt_info->ci_policy.version != FSCRYPT_POLICY_V2) {
		res = -EINVAL;
		pr_err("%s: sdp class or policy version invalid!", __func__);
		goto out;
	}
	res = f2fs_inode_check_sdp_keyring(sdp_ctx.master_key_descriptor,
		crypt_info->ci_policy.v2.master_key_identifier,
		FSCRYPT_KEY_IDENTIFIER_SIZE, NO_NEED_TO_CHECK_KEYFING);
	if (res != 0) {
		pr_err("%s: f2fs_inode_check_sdp_keyring failed", __func__);
		goto out;
	}
	crypt_info->ci_hw_enc_flag |= F2FS_XATTR_SDP_ECE_ENABLE_FLAG;
	(void)memcpy_s(crypt_info->ci_nonce, sizeof(crypt_info->ci_nonce),
		fscrypt_context_nonce(&ctx), FS_KEY_DERIVATION_NONCE_SIZE);

	if (cmpxchg(&inode->i_crypt_info, NULL, crypt_info) == NULL)
		crypt_info = NULL;

	res = check_and_set_sdp_flags(inode, fs_data, sdp_ctx.sdp_class);
	if (res != 0) {
		fscrypt_put_encryption_info(inode);
		return res;
	}
out:
	f2fs_put_crypt_info(crypt_info);
	return res;
}

static int is_need_check_keyring(struct inode *inode, void *fs_data, int *enforce)
{
	u32 flag;
	int res;

	*enforce = NO_NEED_TO_CHECK_KEYFING;
	res = f2fs_inode_get_sdp_encrypt_flags(inode, fs_data, &flag);
	if (res != 0) {
		pr_err("%s: get flags failed, res:%d", __func__, res);
		return res;
	}
	if (f2fs_inode_is_enabled_sdp_sece_encryption(flag))
		*enforce = ENHANCED_CHECK_KEYING;
	return res;
}

static int f2fs_get_sdp_sece_crypt_info(struct inode *inode, void *fs_data)
{
	int res;
	int enforce;
	union fscrypt_context ctx = {0};
	struct sdp_fscrypt_context sdp_ctx = {0};
	struct fscrypt_info *crypt_info = NULL;

	res = get_context_all(inode, &sdp_ctx, &ctx, fs_data);
	if (res != 0)
		return res;

	crypt_info = kmem_cache_zalloc(fscrypt_info_cachep, GFP_NOFS);
	if (!crypt_info)
		return -ENOMEM;

	res = fscrypt_policy_from_context(&crypt_info->ci_policy,
		&ctx, sizeof(ctx));
	if (res != 0)
		goto out;

	if (sdp_ctx.sdp_class != FSCRYPT_SDP_SECE_CLASS ||
		crypt_info->ci_policy.version != FSCRYPT_POLICY_V2) {
		res = -EINVAL;
		pr_err("%s: sdp class or policy version invalid!", __func__);
		goto out;
	}
	if (is_need_check_keyring(inode, fs_data, &enforce) != 0) {
		res = -EINVAL;
		goto out;
	}

	res = f2fs_inode_check_sdp_keyring(sdp_ctx.master_key_descriptor,
		crypt_info->ci_policy.v2.master_key_identifier,
		FSCRYPT_KEY_IDENTIFIER_SIZE, enforce);
	if (res != 0) {
		pr_err("%s: f2fs_inode_check_sdp_keyring failed", __func__);
		goto out;
	}

	crypt_info->ci_hw_enc_flag |= F2FS_XATTR_SDP_SECE_ENABLE_FLAG;
	(void)memcpy_s(crypt_info->ci_nonce, sizeof(crypt_info->ci_nonce),
		fscrypt_context_nonce(&ctx), FS_KEY_DERIVATION_NONCE_SIZE);

	if (cmpxchg(&inode->i_crypt_info, NULL, crypt_info) == NULL)
		crypt_info = NULL;

	res = check_and_set_sdp_flags(inode, fs_data, sdp_ctx.sdp_class);
	if (res != 0) {
		fscrypt_put_encryption_info(inode);
		return res;
	}
out:
	f2fs_put_crypt_info(crypt_info);
	return res;
}

int f2fs_change_to_sdp_crypto(struct inode *inode, void *fs_data,
	int sdp_class)
{
	int res;
	u32 flag;

	if (!inode) /* fs_data is NULL so not need to jduge */
		return -EINVAL;

	res = f2fs_inode_get_sdp_encrypt_flags(inode, fs_data, &flag);
	if (res != 0) {
		pr_err("%s: get sdp flag failed, res:%d", __func__, res);
		return res;
	}

	if (f2fs_inode_is_config_sdp_ece_encryption(flag)) {
		res = f2fs_inode_set_sdp_encryption_flags(inode,
			fs_data, F2FS_XATTR_SDP_ECE_ENABLE_FLAG);
	} else {
		res = f2fs_inode_set_sdp_encryption_flags(inode,
			fs_data, F2FS_XATTR_SDP_SECE_ENABLE_FLAG);
	}
	if (res != 0) {
		pr_err("%s: change context failed, res:%d\n", __func__, res);
		return res;
	}
	if (!inode->i_crypt_info) {
		pr_err("%s: crypt_info is null!\n", __func__);
		return -EINVAL;
	}
	fscrypt_put_encryption_info(inode);
	if (sdp_class == FSCRYPT_SDP_ECE_CLASS) {
		res = f2fs_get_sdp_ece_crypt_info(inode, fs_data);
	} else {
		res = f2fs_get_sdp_sece_crypt_info(inode, fs_data);
	}
	return res;
}

static s8 hex_to_char(u8 hex)
{
	s8 c;
	u8 lower = hex & 0x0f;

	if (lower >= 0x0a)
		c = ('a' + lower - 0x0a);
	else
		c = '0' + lower;
	return c;
}

static void bytes_to_hexstring(const u8 *bytes, u32 len, s8 *str, u32 str_len)
{
	s32 i;

	if (str_len <= HEX_STR_PER_BYTE * len)
		return;
	for (i = 0; i < len; i++) {
		str[HEX_STR_PER_BYTE * i] = hex_to_char((bytes[i] & 0xf0) >> 4);
		str[HEX_STR_PER_BYTE * i + 1] = hex_to_char(bytes[i] & 0x0f);
	}
	str[HEX_STR_PER_BYTE * len] = '\0';
}

static struct key *fscrypt_request_key(const u8 *descriptor, const u8 *prefix,
	int prefix_size)
{
	u8 *full_key_descriptor = NULL;
	struct key *keyring_key = NULL;
	int full_key_len = prefix_size + (FSCRYPT_KEY_DESCRIPTOR_SIZE * 2) + 1;

	full_key_descriptor = kmalloc(full_key_len, GFP_NOFS);
	if (!full_key_descriptor)
		return (struct key *)ERR_PTR(-ENOMEM);

	(void)memcpy_s(full_key_descriptor, full_key_len, prefix, prefix_size);
	bytes_to_hexstring(descriptor, FSCRYPT_KEY_DESCRIPTOR_SIZE,
		full_key_descriptor + prefix_size, full_key_len - prefix_size);

	keyring_key = request_key(&key_type_logon, full_key_descriptor, NULL);
	kfree(full_key_descriptor);

	return keyring_key;
}

static int check_screen_lock_state(int enforce,
	const struct fscrypt_sdp_key *master_sdp_key)
{
	if (!master_sdp_key)
		return -EINVAL;

	if (master_sdp_key->sdp_class != FSCRYPT_SDP_SECE_CLASS &&
		master_sdp_key->sdp_class != FSCRYPT_SDP_ECE_CLASS) {
		pr_err("%s: sdp class incorrect:%d\n",
			__func__, master_sdp_key->sdp_class);
		return -EINVAL;
	}

	if (master_sdp_key->sdp_class == FSCRYPT_SDP_SECE_CLASS) {
		if (enforce == ENHANCED_CHECK_KEYING) {
			if (master_sdp_key->screen_lock_state == ON_LOCK_SCREEN_STATE) {
				return -EINVAL;
			}
		}
	}
	return 0;
}

int f2fs_inode_check_sdp_keyring(const u8 *descriptor,
	u8 *key_identifier, int key_identifier_len, int enforce)
{
	int res = -EKEYREVOKED;
	struct key *keyring_key = NULL;
	const struct user_key_payload *payload = NULL;
	struct fscrypt_sdp_key *master_sdp_key = NULL;

	if (!descriptor)
		return -EINVAL;

	keyring_key = fscrypt_request_key(descriptor,
		FSCRYPT_KEY_DESC_PREFIX, FSCRYPT_KEY_DESC_PREFIX_SIZE);
	if (IS_ERR(keyring_key))
		return PTR_ERR(keyring_key);

	down_read(&keyring_key->sem);
	if (keyring_key->type != &key_type_logon) {
		pr_err("%s: key type must be logon\n", __func__);
		goto out;
	}

	payload = user_key_payload_locked(keyring_key);
	if (!payload)
		goto out;

	if (payload->datalen != sizeof(struct fscrypt_sdp_key)) {
		pr_err("%s: sdp full key size incorrect:%d\n",
			__func__, payload->datalen);
		goto out;
	}

	master_sdp_key = (struct fscrypt_sdp_key *)payload->data;
	if (check_screen_lock_state(enforce, master_sdp_key) != 0)
		goto out;

	if (key_identifier) {
		if (key_identifier_len !=
			sizeof(master_sdp_key->identifier)) {
			res = -EINVAL;
			pr_err("%s: key_identifier_len incorrect:%d\n",
				key_identifier_len);
			goto out;
		}
		(void)memcpy_s(key_identifier, key_identifier_len,
			master_sdp_key->identifier, key_identifier_len);
	}
	res = 0;
out:
	up_read(&keyring_key->sem);
	key_put(keyring_key);
	return res;
}

static int f2fs_get_sdp_crypt_info(struct inode *inode, void *fs_data)
{
	int res;
	u32 flag;
	struct fscrypt_info *ci_info = NULL;

	if (!inode) /* fs_data can be null so not need to check */
		return -EINVAL;

	ci_info = inode->i_crypt_info;
	res = f2fs_inode_get_sdp_encrypt_flags(inode, fs_data, &flag);
	if (res != 0)
		return res;

	if (ci_info && f2fs_inode_is_enabled_sdp_encryption(flag)) {
		return f2fs_inode_check_sdp_keyring(
			ci_info->ci_policy.v2.master_key_identifier, NULL, 0,
			ENHANCED_CHECK_KEYING);
	}
	if (f2fs_inode_is_config_sdp_ece_encryption(flag))
		return f2fs_get_sdp_ece_crypt_info(inode, fs_data);

	return f2fs_get_sdp_sece_crypt_info(inode, fs_data);
}

int f2fs_get_crypt_keyinfo(struct inode *inode, void *fs_data, int *flag)
{
	int res;
	u32 sdpflag;
#ifdef CONFIG_HWDPS
	struct fscrypt_info *ci_info = NULL;
#endif

	if (!inode)
		return -EINVAL;
	if (!flag) /* fs_data can be null so not check it */
		return -EINVAL;
	/* 0 for getting original ce crypt info, otherwise be 1 */
	*flag = 0;
#ifdef CONFIG_HWDPS
	ci_info = inode->i_crypt_info;
	if (!ci_info ||
		(ci_info->ci_hw_enc_flag & F2FS_XATTR_DPS_ENABLE_FLAG) != 0) {
		res = hwdps_check_inode(inode);
		if (res == -EOPNOTSUPP) {
			goto get_sdp_encryption_info;
		} else if (res != 0) {
			return -EACCES;
		} else {
			*flag = 1; /* enabled */
			return 0;
		}
	}
get_sdp_encryption_info:
#endif
	if (!S_ISREG(inode->i_mode))
		return 0;

	down_write(&inode->i_sdp_sem);
	res = inode->i_sb->s_cop->get_sdp_encrypt_flags(inode,
		fs_data, &sdpflag);
	if (res != 0) {
		pr_err("%s: get_sdp_encrypt_flags failed, res:%d",
			__func__, res);
		goto unlock;
	}

	/* get sdp crypt info */
	if (f2fs_inode_is_sdp_encrypted(sdpflag)) {
		*flag = 1; /* enabled */
		res = f2fs_get_sdp_crypt_info(inode, fs_data);
		if (res != 0)
			pr_err("%s: inode:%lu, get keyinfo failed res:%d\n",
				__func__, inode->i_ino, res);
	}
unlock:
	up_write(&inode->i_sdp_sem);
	return res;
}

int f2fs_is_file_sdp_encrypted(struct inode *inode)
{
	u32 flag;
	/* if the inode is not null, i_sb, s_cop is not null */
	if (!inode)
		return 0; /* In this condition 0 means not enable sdp */

	if (inode->i_sb->s_cop->get_sdp_encrypt_flags(inode, NULL, &flag))
		return 0;
	return f2fs_inode_is_sdp_encrypted(flag);
}
