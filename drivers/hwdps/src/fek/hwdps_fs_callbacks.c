/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: This file contains the function required for operations
 *              about map.
 * Create: 2021-05-21
 */

#include "inc/fek/hwdps_fs_callbacks.h"
#include <linux/cred.h>
#include <linux/dcache.h>
#include <linux/file.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/printk.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <securec.h>
#include <huawei_platform/hwdps/hwdps_ioctl.h>
#include <huawei_platform/hwdps/hwdps_limits.h>
#include <huawei_platform/hwdps/hwdps_defines.h>
#include "inc/base/hwdps_utils.h"
#include "inc/data/hwdps_data.h"
#include "inc/data/hwdps_packages.h"
#include "inc/fek/hwdps_fs_callbacks.h"
#include "inc/policy/hwdps_policy.h"

#define PATH_OF_INSTALLD "/system/bin/installd"
#define PATH_LEN_OF_INSTALLD 20

static hwdps_result_t get_path(const struct dentry *dentry, u8 *path)
{
	u8 *path_tmp = NULL;
	hwdps_result_t ret = HWDPS_SUCCESS;
	u8 *buf = kzalloc(PATH_MAX, GFP_NOFS);

	if (!buf)
		return HWDPS_ERR_GET_PATH;
	path_tmp = dentry_path_raw((struct dentry *)dentry, buf, PATH_MAX);
	if (IS_ERR(path_tmp) || (strlen(path_tmp) >= PATH_MAX))
		goto err;
	if (memcpy_s(path, PATH_MAX, path_tmp, strlen(path_tmp)) != EOK)
		goto err;
	goto out;
err:
	ret = HWDPS_ERR_GET_PATH;
out:
	kzfree(buf);
	return ret;
}
static hwdps_result_t precheck_new_key_data(const struct dentry *dentry,
	uid_t uid, const s8 *fsname)
{
	hwdps_result_t res;
	struct package_hashnode_t *package_node = NULL;
	struct package_info_listnode_t *package_info_node = NULL;
	struct package_info_listnode_t *tmp = NULL;
	u8 *path = NULL;

	hwdps_data_read_lock();
	package_node = get_hwdps_package_hashnode(uid);
	if (!package_node) {
		res = -HWDPS_ERR_NOT_SUPPORTED;
		goto out;
	}
	path = kzalloc(PATH_MAX, GFP_NOFS);
	if (!path) {
		res = -HWDPS_ERR_NO_MEMORY;
		hwdps_pr_err("%s path alloc failed\n", __func__);
		goto out;
	}

	res = get_path(dentry, path);
	if (res != HWDPS_SUCCESS)
		goto out;
	list_for_each_entry_safe(package_info_node, tmp,
		&package_node->pinfo_list, list)
		if (hwdps_evaluate_policies(package_info_node, fsname,
			path)) {
			res = HWDPS_SUCCESS; /* success case */
			goto out;
		}
	hwdps_pr_err("hwdps %s failed\n", __func__);
	res = -HWDPS_ERR_NOT_SUPPORTED; /* policy never matched */
out:
	kzfree(path);
	hwdps_data_read_unlock();
	return res;
}

/* This function is called when a new inode is created. */
static hwdps_result_t handle_check_path(const u8 *fsname,
	const struct dentry *dentry, u32 parent_flags)
{
	uid_t uid;
	const struct cred *cred = NULL;

	if (!fsname || !dentry)
		return HWDPS_ERR_INVALID_ARGS;

	if ((parent_flags & HWDPS_XATTR_ENABLE_FLAG) != 0)
		return HWDPS_SUCCESS;

	cred = get_current_cred();
	if (!cred)
		return HWDPS_ERR_INVALID_ARGS;
	uid = cred->uid.val; /* task uid */
	put_cred(cred);

	return precheck_new_key_data(dentry, uid, fsname);
}

static hwdps_result_t precheck_uid_owner(const encrypt_id *id)
{
	if (id->task_uid == id->uid)
		return HWDPS_SUCCESS;
	hwdps_pr_err("%s uid and task uid error\n", __func__);
	return -HWDPS_ERR_NOT_OWNER;
}


#ifdef CONFIG_HWDPS_ENG_DEBUG
static bool is_pid_high_level(pid_t pid)
{
	s32 i;
	size_t priv_task_len;
	s32 priv_num;
	s8 task_name[TASK_COMM_LEN] = {0};

	/*
	 * lists cmds have high level
	 * echo --> "sh"
	 * adb pull --> "sync svc 44", "sync svc 66"
	 */
	static const s8 * const priv_cmds[] = {
		"cat", "sh", "sync svc"
	};
	get_task_comm(task_name, current);
	priv_num = ARRAY_SIZE(priv_cmds);
	for (i = 0; i < priv_num; ++i) {
		priv_task_len = strlen(priv_cmds[i]);
		if (!strncmp(priv_cmds[i], task_name, priv_task_len))
			return true;
	}
	hwdps_pr_info("task is not high level\n");
	return false;
}
#endif

static bool is_pid_installd(pid_t pid)
{
	return hwdps_utils_exe_check(pid, PATH_OF_INSTALLD,
		PATH_LEN_OF_INSTALLD);
}

static hwdps_result_t handle_hwdps_has_access(const encrypt_id *id, u32 flags)
{
	hwdps_result_t res;

	if (!id) {
		pr_err("%s params error\n", __func__);
		return -HWDPS_ERR_INVALID_ARGS;
	}
	if ((flags & HWDPS_XATTR_ENABLE_FLAG) == 0)
		return -HWDPS_ERR_INVALID_ARGS;

	res = precheck_uid_owner(id);
	if (res != HWDPS_SUCCESS) {
		hwdps_pr_err("precheck_uid_owner error\n");
		goto priv;
	}
	goto out;
priv:
#ifdef CONFIG_HWDPS_ENG_DEBUG
	if (is_pid_high_level(id->pid) || is_pid_installd(id->pid))
#else
	if (is_pid_installd(id->pid))
#endif
		res = HWDPS_SUCCESS;
out:
	return (res > 0) ? -res : res;
}

struct hwdps_fs_callbacks_t g_fs_callbacks = {
	.check_path = handle_check_path,
	.hwdps_has_access = handle_hwdps_has_access,
};

void hwdps_register_fs_callbacks_proxy(void)
{
	hwdps_register_fs_callbacks(&g_fs_callbacks);
}

void hwdps_unregister_fs_callbacks_proxy(void)
{
	hwdps_unregister_fs_callbacks();
}
