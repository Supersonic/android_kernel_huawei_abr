/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Hwdps hooks for file system(f2fs).
 * Create: 2021-05-21
 */

#include "huawei_platform/hwdps/hwdps_fs_hooks.h"
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/rwsem.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <log/hiview_hievent.h>
#include <securec.h>
#include <uapi/linux/stat.h>
#include <huawei_platform/hwdps/hwdps_limits.h>
#include <huawei_platform/hwdps/fscrypt_private.h>

#ifdef CONFIG_HWDPS
static DECLARE_RWSEM(g_fs_callbacks_lock);

#define HWDPS_XATTR_NAME "hwdps"
#define HWDPS_KEY_DESC_STANDARD_FLAG 0x42

#define HWDPS_HIVIEW_ID 940016001
#define HWDPS_HIVIEW_CREATE 1000
#define HWDPS_HIVIEW_OPEN 1001
#define HWDPS_HIVIEW_UPDATE 1002
#define HWDPS_HIVIEW_ACCESS 1003
#define HWDPS_HIVIEW_ENCRYPT 1004

static hwdps_result_t default_check_path(const u8 *fsname,
	const struct dentry *dentry, u32 parent_flags)
{
	(void)fsname;
	(void)dentry;
	(void)parent_flags;
	return HWDPS_ERR_NO_FS_CALLBACKS;
}

static hwdps_result_t default_hwdps_has_access(const encrypt_id *id, u32 flags)
{
	(void)id;
	(void)flags;
	return HWDPS_ERR_NO_FS_CALLBACKS;
}

static struct hwdps_fs_callbacks_t g_fs_callbacks = {
	.check_path = default_check_path,
	.hwdps_has_access = default_hwdps_has_access,
};

static hwdps_result_t hwdps_check_path(struct inode *inode,
	const struct dentry *dentry, u32 parent_flags)
{
	hwdps_result_t res;
	const u8 *fsname = NULL;

	if (!inode || !inode->i_sb || !inode->i_sb->s_type)
		return HWDPS_ERR_INVALID_ARGS;

	fsname = inode->i_sb->s_type->name;
	down_read(&g_fs_callbacks_lock);
	res = g_fs_callbacks.check_path(fsname, dentry, parent_flags);
	up_read(&g_fs_callbacks_lock);

	return res;
}

static void hiview_for_hwdps(int type, hwdps_result_t result,
	const encrypt_id *id)
{
	struct hiview_hievent *event = hiview_hievent_create(HWDPS_HIVIEW_ID);

	if (!event) {
		pr_err("hwdps hiview event null");
		return;
	}

	/*lint -e644*/
	pr_info("%s result %d, %d, %u, %u\n", __func__,
		type, result, id->uid, id->task_uid);
	/*lint +e644*/
	hiview_hievent_put_integral(event, "type", type);
	hiview_hievent_put_integral(event, "result", result);
	hiview_hievent_put_integral(event, "task_uid",
		id->task_uid); //lint !e644
	hiview_hievent_put_integral(event, "uid", id->uid); //lint !e644

	hiview_hievent_report(event);
	hiview_hievent_destroy(event);
}

s32 f2fs_set_hwdps_enable_flags(struct inode *inode, void *fs_data)
{
	u32 flags;
	s32 res;

	if (!inode || !inode->i_sb || !inode->i_sb->s_cop ||
		!inode->i_sb->s_cop->get_hwdps_flags ||
		!inode->i_sb->s_cop->set_hwdps_flags) {
		pr_err("%s inode NULL\n", __func__);
		return -EINVAL;
	}

	res = inode->i_sb->s_cop->get_hwdps_flags(inode, fs_data, &flags);
	if (res != 0) {
		pr_err("%s get inode %lu hwdps flags res %d\n",
			__func__, inode->i_ino, res);
		return -EINVAL;
	}

	flags &= (~HWDPS_ENABLE_FLAG);
	flags |= HWDPS_XATTR_ENABLE_FLAG;
	res = inode->i_sb->s_cop->set_hwdps_flags(inode, fs_data, &flags);
	if (res != 0) {
		pr_err("%s set inode %lu hwdps flag res %d\n",
			__func__, inode->i_ino, res);
		return -EINVAL;
	}
	return res;
}

hwdps_result_t hwdps_has_access(struct inode *inode, u32 flags)
{
	hwdps_result_t res;
	encrypt_id id;
	const struct cred *cred = NULL;

	if (!inode)
		return HWDPS_ERR_INVALID_ARGS;

	id.pid = task_tgid_nr(current);
	cred = get_current_cred(); //lint !e666
	if (!cred)
		return HWDPS_ERR_INVALID_ARGS;

	id.task_uid = cred->uid.val;
	put_cred(cred);
	id.uid = inode->i_uid.val;

	down_read(&g_fs_callbacks_lock);
	res = g_fs_callbacks.hwdps_has_access(&id, flags);
	up_read(&g_fs_callbacks_lock);

	if (res != 0)
		hiview_for_hwdps(HWDPS_HIVIEW_ACCESS, res, &id);
	return res;
}

s32 hwdps_check_support(struct inode *inode, uint32_t *flags)
{
	s32 err = 0;

	if (!inode || !flags)
		return -EINVAL;

	if (!S_ISREG(inode->i_mode))
		return -EOPNOTSUPP;
	/*
	 * The inode->i_crypt_info->ci_hw_enc_flag keeps sync with the
	 * flags in xattr_header. And it can not be changed once the
	 * file is opened.
	 */
	if (!inode->i_crypt_info)
		err = inode->i_sb->s_cop->get_hwdps_flags(inode, NULL, flags);
	else
		*flags = (u32)(inode->i_crypt_info->ci_hw_enc_flag);
	if (err != 0)
		pr_err("hwdps ino %lu get flags err %d\n", inode->i_ino, err);
	else if ((*flags & HWDPS_XATTR_ENABLE_FLAG) == 0)
		err = -EOPNOTSUPP;

	return err;
}

static s32 hwdps_dir_inherit_flags(struct inode *inode,
	void *fs_data, u32 parent_flags)
{
	if (!S_ISDIR(inode->i_mode))
		return 0;

	if ((parent_flags & HWDPS_XATTR_ENABLE_FLAG) != 0)
		return f2fs_set_hwdps_enable_flags(inode, fs_data);

	return 0;
}

static s32 hwdps_get_parent_dps_flag(struct inode *parent, u32 *flags,
	void *fs_data)
{
	s32 err;

	if (!parent || !flags)
		return -ENODATA;

	err = parent->i_sb->s_cop->get_hwdps_flags(parent, fs_data, flags);
	if (err != 0)
		*flags = 0;
	else
		pr_debug("hwdps ino %lu, parent has dps flag\n", parent->i_ino);

	return err;
}

static s32 check_params(const struct inode *parent,
	const struct inode *inode, const struct dentry *dentry,
	const void *fs_data)
{
	if (!dentry || !inode || !fs_data || !parent)
		return -EAGAIN;
	if (!parent->i_crypt_info) {
		pr_err("hwdps parent ci is null error\n");
		return -ENOKEY;
	}

	return HWDPS_SUCCESS;
}

/*
 * Code is mainly copied from fscrypt_inherit_context
 *
 * funcs except hwdps_create_fek must not return EAGAIN
 *
 * Return:
 *  o 0: SUCC
 *  o other errno: the file is not supported by policy
 */
s32 hwdps_inherit_context(struct inode *parent, struct inode *inode,
	const struct dentry *dentry, void *fs_data, struct page *page)
{
	s32 err;
	u32 parent_flags = 0;

	/* no need to judge page because it can be null */
	err = check_params(parent, inode, dentry, fs_data);
	if (err != HWDPS_SUCCESS)
		return err;

	err = hwdps_get_parent_dps_flag(parent, &parent_flags, page);
	if (err != HWDPS_SUCCESS)
		return err;

	if (!S_ISREG(inode->i_mode))
		return hwdps_dir_inherit_flags(inode, fs_data, parent_flags);

	err = hwdps_check_path(inode, dentry, parent_flags);
	if (err == -HWDPS_ERR_NOT_SUPPORTED) {
		pr_info_once("hwdps ino %lu not protected\n", inode->i_ino);
		return HWDPS_SUCCESS;
	}
	if (err != HWDPS_SUCCESS) {
		pr_err("hwdps ino %lu check path err %d\n", inode->i_ino, err);
		return err;
	}
	err = f2fs_set_hwdps_enable_flags(inode, fs_data);
	if (err != 0)
		pr_err("hwdps ino %lu set hwdps enable flags err %d\n",
			inode->i_ino, err);
	return err;
}

void hwdps_register_fs_callbacks(struct hwdps_fs_callbacks_t *callbacks)
{
	down_write(&g_fs_callbacks_lock);
	if (callbacks) {
		if (callbacks->check_path)
			g_fs_callbacks.check_path = callbacks->check_path;
		if (callbacks->hwdps_has_access)
			g_fs_callbacks.hwdps_has_access = callbacks->hwdps_has_access;
	}
	up_write(&g_fs_callbacks_lock);
}
EXPORT_SYMBOL(hwdps_register_fs_callbacks); //lint !e580

void hwdps_unregister_fs_callbacks(void)
{
	down_write(&g_fs_callbacks_lock);
	g_fs_callbacks.check_path = default_check_path;
	g_fs_callbacks.hwdps_has_access = default_hwdps_has_access;
	up_write(&g_fs_callbacks_lock);
}
EXPORT_SYMBOL(hwdps_unregister_fs_callbacks); //lint !e580

#endif
