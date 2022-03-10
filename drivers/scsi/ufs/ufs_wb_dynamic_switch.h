/*
* ufs_wb_dynamic_switch.h
* ufs Write Boster feature
*
* Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/
#ifndef _UFS_WB_DYNAMIC_SWITCH_H_
#define _UFS_WB_DYNAMIC_SWITCH_H_

#include <linux/types.h>
#include <linux/bootdevice.h>

#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
#include <scsi/scsi_cmnd.h>
#endif

struct ufs_hba;
#ifdef CONFIG_HUAWEI_UFS_DYNAMIC_SWITCH_WB

#define WB_T_BUFFER_THRESHOLD 0x3
#define VERSION                5
#define PATCH_L                4
#define SUB_L                  61

extern struct device_attribute *ufs_host_attrs[];

enum {
	MASK_EE_WRITEBOOSTER_EVENT = (1 << 5),
};

#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
#define MONITOR_WB_US			10000
#define OPEN_WB_SLOT_MAX		6
#define CLOSE_WB_SLOT_MAX		4
#define COUNT_CLOSE_WB_MAX		10
#define FLUSH_LOW_LIMIT			7
#define FLUSH_UP_LIMIT			8
#define MAX_PNM_LEN			18


struct ufs_wb_runtime_switch {
	bool wb_opt_state;
	bool disable;
	ktime_t last_time;
	unsigned short avg_slot;
	unsigned long slot_cnt;
	unsigned long req_cnt;
	unsigned long low_speed_cnt;
	unsigned int curr_bw;
	unsigned int high_bw_threshold;
	unsigned long total_write;
	bool flush_is_open; /* auto flush in h8 */
	bool is_samsung_v5;
};

void ufshcd_wb_runtime_config(struct scsi_cmnd *cmd, struct ufs_hba *hba);
void ufshcd_wb_runtime_switch_suspend(struct ufs_hba *hba);
#endif

struct ufs_wb_switch_info {
	struct ufs_hba *hba;
	u32  wb_shared_alloc_units;
	bool wb_work_sched;
	bool wb_permanent_disabled;
	bool wb_exception_enabled;
	struct delayed_work wb_toggle_work;
#ifdef CONFIG_HUAWEI_UFS_WB_RUNTIME
	struct ufs_wb_runtime_switch wb_runtime;
	struct work_struct ondemad_wb_work; /* workstruct for ondemand open/close wb */
#endif
};

bool ufshcd_wb_is_permanent_disabled(struct ufs_hba *hba);
void ufshcd_wb_exception_event_handler(struct ufs_hba *hba);
void ufshcd_wb_toggle_cancel(struct ufs_hba *hba);
int ufs_dynamic_switching_wb_config(struct ufs_hba *hba);
int ufshcd_enable_wb_exception_event(struct ufs_hba *hba);
void ufs_dynamic_switching_wb_remove(struct ufs_hba *hba);
#else /* CONFIG_HUAWEI_UFS_DYNAMIC_SWITCH_WB */
static inline  __attribute__((unused)) bool
ufshcd_wb_is_permanent_disabled(struct ufs_hba *hba
	__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) void
ufshcd_wb_exception_event_handler(struct ufs_hba *hba
		__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) void
ufshcd_wb_toggle_cancel(struct ufs_hba *hba
		__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) int
ufs_dynamic_switching_wb_config(struct ufs_hba *hba
		__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) int
ufshcd_enable_wb_exception_event(struct ufs_hba *hba
		__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) int
ufs_dynamic_switching_wb_remove(struct ufs_hba *hba
		__attribute__((unused)))
{
	return 0;
}
#endif /* CONFIG_HUAWEI_UFS_DYNAMIC_SWITCH_WB */
#endif
