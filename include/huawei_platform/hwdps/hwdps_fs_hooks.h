/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: This file define function about fs hooks.
 * Create: 2021-05-21
 */

#ifndef HWDPS_FS_HOOKS_H
#define HWDPS_FS_HOOKS_H

#include <linux/fs.h>
#include <linux/types.h>
#include <huawei_platform/hwdps/hwdps_defines.h>
#include <huawei_platform/hwdps/hwdps_error.h>
#include <huawei_platform/hwdps/hwdps_ioctl.h>

#ifdef CONFIG_HWDPS

/* filesystem callback functions */
struct hwdps_fs_callbacks_t {
	hwdps_result_t (*check_path)(const u8 *fsname,
		const struct dentry *dentry, u32 parent_flags);

	hwdps_result_t (*hwdps_has_access)(const encrypt_id *id, u32 flags);
};

/*
 * Description: Access a file encryption key. Note: memory for the output
 *              parameters inode will be check inside the
 *              function.
 * Input: inode: the inode requesting check uid
 * Return: HWDPS_SUCCESS: successfully protected by hwdps
 *         -HWDPS_ERR_NOT_SUPPORTED:  not protected by hwdps
 *         -HWDPS_ERR_NO_FS_CALLBACKS: should wait
 *         other negative value: error
 */
hwdps_result_t hwdps_has_access(struct inode *inode, u32 flags);


/*
 * Description: Loads filesystem callback functions.
 * Input: struct hwdps_fs_callbacks_t *callbacks: callback functions to be
 *        registered
 */
void hwdps_register_fs_callbacks(struct hwdps_fs_callbacks_t *callbacks);

/*
 * Description: Unloads filesystem callback functions.
 */
void hwdps_unregister_fs_callbacks(void);

/*
 * Description: Check the context of a file inode whether support the hwdps.
 * Input: inode: the inode requesting fek struct
 * Return: 0: successfully support hwdps
 *         -EOPNOTSUPP: not support hwdps
 */
s32 hwdps_check_support(struct inode *inode, u32 *flags);


/*
 * Description: Inherit the context from parent inode context.
 * Input: parent: the parent inode struct
 * Input: inode: the inode requesting fek struct
 * Input: dentry: the dentry of this inode
 * Input: fs_data: the page data buffer
 * Return: 0: successfully inherit the context
 *         -ENOKEY: no file key
 */
s32 hwdps_inherit_context(struct inode *parent, struct inode *inode,
	const struct dentry *dentry, void *fs_data, struct page *dpage);


s32 f2fs_set_hwdps_enable_flags(struct inode *inode, void *fs_data);

#endif /* CONFIG_HWDPS */
#endif
