/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_inner.h
 * Description: rdr inner definitions
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#ifndef RDR_INNER_H
#define RDR_INNER_H

#include <linux/version.h>
#include <linux/types.h>
#include <linux/printk.h>

#define PRINT_INFO(args...)    pr_info("dfx "args)
#define PRINT_ERROR(args...)   pr_err("dfx "args)
#define PRINT_DEBUG(args...)   pr_debug("dfx "args)
#define PRINT_START(args...) \
	pr_info(">>>>>dfx enter blackbox %s(): %.4d\n", __func__, __LINE__)
#define PRINT_END(args...)   \
	pr_info("<<<<<dfx exit  blackbox %s(): %.4d\n", __func__, __LINE__)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0))
#define SYS_UNLINK ksys_unlink
#define SYS_OPEN ksys_open
#define SYS_CLOSE ksys_close
#define SYS_RENAME ksys_rename
#define SYS_READ ksys_read
#define SYS_WRITE ksys_write
#define SYS_LSEEK ksys_lseek
#define SYS_ACCESS ksys_access
#define SYS_SYNC ksys_sync
#define SYS_FSYNC(fd) ksys_sync_file_range(fd, 0, LLONG_MAX, SYNC_FILE_RANGE_WRITE)
#define SYS_CHOWN ksys_chown
#define SYS_MKDIR ksys_mkdir
#define SYS_RMDIR ksys_rmdir
#else
#define SYS_UNLINK sys_unlink
#define SYS_OPEN sys_open
#define SYS_CLOSE sys_close
#define SYS_RENAME sys_rename
#define SYS_READ sys_read
#define SYS_WRITE sys_write
#define SYS_LSEEK sys_lseek
#define SYS_ACCESS sys_access
#define SYS_SYNC sys_sync
#define SYS_FSYNC sys_fsync
#define SYS_CHOWN sys_chown
#define SYS_MKDIR sys_mkdir
#define SYS_RMDIR sys_rmdir
#endif

#endif