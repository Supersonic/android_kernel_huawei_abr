/*
 * Universal Flash Storage Host controller driver
 *
 * This code is based on drivers/scsi/ufs/ufs.h
 * Copyright (C) 2011-2013 Samsung India Software Operations
 *
 * Authors:
 *	Santosh Yaraganavi <santosh.sy@samsung.com>
 *	Vinayak Holikatti <h.vinayak@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * See the COPYING file in the top-level directory or visit
 * <http://www.gnu.org/licenses/gpl-2.0.html>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * This program is provided "AS IS" and "WITH ALL FAULTS" and
 * without warranty of any kind. You are solely responsible for
 * determining the appropriateness of using and distributing
 * the program and assume all risks associated with your exercise
 * of rights with respect to the program, including but not limited
 * to infringement of third party rights, the risks and costs of
 * program errors, damage to or loss of data, programs or equipment,
 * and unavailability or interruption of operations. Under no
 * circumstances will the contributor of this Program be liable for
 * any damages of any kind arising from your use or distribution of
 * this program.
 */

#ifndef _UFS_FUNC_H_
#define _UFS_FUNC_H_

enum wb_type {
	WB_DEDICATED     = 0x0,
	WB_SINGLE_SHARED = 0x1,
};

/* QUERY VCMD READ 0xF8 */
enum {
	QUERY_TZ_IDN_CAP_INFO			= 0x8,
	QUERY_TZ_IDN_BLK_INFO			= 0x9,
#ifdef CONFIG_SCSI_UFS_UNISTORE
	QUERY_PWRON_INFO_SYNC 			= 0x20,
	QUERY_OOB_INFO_FETCH 			= 0x21,
#endif
};

/* QUERY VCMD WRITE 0xF9 */
enum {
	QUERY_VENDOR_WRITE_IDN_PGR		= 0x7,
#ifdef CONFIG_SCSI_UFS_UNISTORE
	QUERY_VENDOR_RESET_FTL			= 0x20,
	QUERY_VENDOR_DATA_MOVE			= 0x21,
	QUERY_VENDOR_SLC_MODE_CONFIGURATION	= 0x22,
	QUERY_VENDOR_FS_SYNC_DONE		= 0x23,
	QUERY_RESCUE_BLOCK_INJECT_DATA		= 0x24,
	QUERY_SYNC_READ_VERIFY			= 0x25,
	QUERY_BAD_BLOCK_ERROR_INJECT		= 0x27,
#endif
};

#endif
