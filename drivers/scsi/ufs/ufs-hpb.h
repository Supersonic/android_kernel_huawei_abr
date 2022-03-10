/*
 * ufs-hpb.h
 *
 * ufs hpb configuration
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020. All rights reserved.
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

#ifndef _UFSHPB_H_
#define _UFSHPB_H_

#include <linux/circ_buf.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include "ufshcd.h"
#include "ufs.h"

#ifdef CONFIG_HUAWEI_UFS_HPB

#define HPB_READ_BUFFER_OPCODE         0xF9
#define HPB_READ_BUFFER_ID             1

#define UFS_BLOCK_SIZE                 4096
#define QUERY_FLAG_IDN_HPB_RESET       0x11
#define HPB_RESET_MAX_COUNT            1000

#define HPB_ENTRY_SIZE                 0x8

#define HPB_SUPPORT_BIT                0x0080
#define SELECTOR_PROTOCOL              0x00
#define SELECTOR_SAMSUNG               0x01
#define HPB_LUN                        0x00
#define HPB_SECTOR_SIZE                0x200

#define HPB_DEVICE_DESC_SIZE           0x59
#define HPB_DEVICE_DESC_SIZE_SAMSUNG   0x5F
#define HPB_DEVICE_DESC_SIZE_MAX       0xFE

#define HPB_UNIT_DESC_SIZE             0x2D
#define HPB_UNIT_DESC_SIZE_MAX         0x2D
#define HPB_UNIT_LU_ENABLE             0x02

#define HPB_GEOMETRY_DESC_SIZE         0x57
#define HPB_GEOMETRY_DESC_SIZE_SAMSUNG 0x59
#define HPB_GEOMETRY_DESC_SIZE_MAX     0x59

#define HPB_SENSE_DATA_LEN             0x12
#define HPB_DESC_TYPE                  0x80
#define HPB_ADDITIONAL_LEN             0x10
#define HPB_MAX_ACTIVE_NUM             0x2
#define HPB_MAX_INACTIVE_NUM           0x2

#define HPB_READ_SIZE_MIN              UFS_BLOCK_SIZE
#define HPB_READ_SIZE_MAX              UFS_BLOCK_SIZE
#define HPB_READ_SIZE_MAX_SAMSUNG      (8 * UFS_BLOCK_SIZE)

#define HPB_READ16_CONTROL_VALUE       0x1
#define HPB_SAMSUNG_BUFID              0x11

#define MAX_HPB_LUN_NUM                0x08
#define HPB_ALLOC_MEM_CNT              0x08
#define HPB_MEMSIZE_PER_LOOP           0x400000
#define HPB_HOST_TABLE_SIZE_PER_SRGN   0x8000
#define HPB_SRGN_CNT_PER_MEMSIZE       0x80
#define HPB_SRGN_CNT                   1024

#define ZERO_BYTE_SHIFT                0
#define ONE_BYTE_SHIFT                 8
#define TWO_BYTE_SHIFT                 16
#define THREE_BYTE_SHIFT               24
#define FOUR_BYTE_SHIFT                32
#define FIVE_BYTE_SHIFT                40
#define SIX_BYTE_SHIFT                 48
#define SEVEN_BYTE_SHIFT               56

#define SHIFT_BYTE_0(num) ((num) << ZERO_BYTE_SHIFT)
#define SHIFT_BYTE_1(num) ((num) << ONE_BYTE_SHIFT)
#define SHIFT_BYTE_2(num) ((num) << TWO_BYTE_SHIFT)
#define SHIFT_BYTE_3(num) ((num) << THREE_BYTE_SHIFT)
#define SHIFT_BYTE_4(num) ((num) << FOUR_BYTE_SHIFT)
#define SHIFT_BYTE_5(num) ((num) << FIVE_BYTE_SHIFT)
#define SHIFT_BYTE_6(num) ((num) << SIX_BYTE_SHIFT)
#define SHIFT_BYTE_7(num) ((num) << SEVEN_BYTE_SHIFT)

#define GET_BYTE_0(num) (((num) >> ZERO_BYTE_SHIFT) & 0xff)
#define GET_BYTE_1(num) (((num) >> ONE_BYTE_SHIFT) & 0xff)
#define GET_BYTE_2(num) (((num) >> TWO_BYTE_SHIFT) & 0xff)
#define GET_BYTE_3(num) (((num) >> THREE_BYTE_SHIFT) & 0xff)
#define GET_BYTE_4(num) (((num) >> FOUR_BYTE_SHIFT) & 0xff)
#define GET_BYTE_5(num) (((num) >> FIVE_BYTE_SHIFT) & 0xff)
#define GET_BYTE_6(num) (((num) >> SIX_BYTE_SHIFT) & 0xff)
#define GET_BYTE_7(num) (((num) >> SEVEN_BYTE_SHIFT) & 0xff)

#define VERSION                       5
#define PATCH_L                       4
#define SUB_L                         61

enum hpb_rsp_operation {
	HPB_REGION_NONE = 0,
	HPB_REGION_UPDATE_REQ,
	HPB_REGION_RESET_REQ,
};

enum norm_read10_compose {
	READ10_RESERVED0_OFFSET = 0x01,
	READ10_LBA1_OFFSET = 0x02,
	READ10_LBA2_OFFSET = 0x03,
	READ10_LBA3_OFFSET = 0x04,
	READ10_LBA4_OFFSET = 0x05,
	READ10_TRANS_LEN_HIGH_OFFSET = 0x07,
	READ10_TRANS_LEN_LOW_OFFSET = 0x08,
};

enum hpb_read16_compose {
	HPB_READ16_OP_OFFSET,
	HPB_READ16_RESERVED0_OFFSET,
	HPB_READ16_LBA1_OFFSET,
	HPB_READ16_LBA2_OFFSET,
	HPB_READ16_LBA3_OFFSET,
	HPB_READ16_LBA4_OFFSET,
	HPB_READ16_PPN1_OFFSET,
	HPB_READ16_PPN2_OFFSET,
	HPB_READ16_PPN3_OFFSET,
	HPB_READ16_PPN4_OFFSET,
	HPB_READ16_PPN5_OFFSET,
	HPB_READ16_PPN6_OFFSET,
	HPB_READ16_PPN7_OFFSET,
	HPB_READ16_PPN8_OFFSET,
	HPB_READ16_TRANS_LEN_OFFSET,
	HPB_READ16_CONTROL_OFFSET,
};

enum hpb_read_buffer_compose {
	HPB_RB_OPCODE_OFFSET = 0,
	HPB_RB_BUFFER_ID_OFFSET,
	HPB_RB_REGION_ID_HIGH_OFFSET,
	HPB_RB_REGION_ID_LOW_OFFSET,
	HPB_RB_SUBREGION_ID_HIGH_OFFSET,
	HPB_RB_SUBREGION_ID_LOW_OFFSET,
	HPB_RB_AL_LEN3_OFFSET,
	HPB_RB_AL_LEN2_OFFSET,
	HPB_RB_AL_LEN1_OFFSET,
	HPB_RB_CONTROL_OFFSET,
};

enum ufshpb_ctrl_mode {
	UFSHPB_HOST_CTRL_MODE = 0,
	UFSHPB_DEVICE_CTRL_MODE = 1,
};

enum ufshpb_state {
	UFSHPB_STATE_NEED_INIT,
	UFSHPB_STATE_OPERATIONAL,
	UFSHPB_STATE_DISABLED,
};

enum srgn_node_status {
	NODE_EMPTY = 0,
	NODE_NEW,
	NODE_UPDATING,
};

struct srgn_node {
	u32 srgn_id;
	u32 status;
	struct list_head updating_srgn_list;
	struct list_head inactive_srgn_list;
	u64 *node_addr;
};

struct ufshpb_rsp_sense_data {
	__be16 sense_data_len;
	u8 desc_type;
	u8 additional_len;
	u8 hpb_opt;
	u8 lun;
	u8 act_cnt;
	u8 inact_cnt;
	__be16 act_rgn_0;
	__be16 act_srgn_0;
	__be16 act_rgn_1;
	__be16 act_srgn_1;
	__be16 inact_rgn_0;
	__be16 inact_rgn_1;
};

struct ufshpb_debug {
	struct device_attribute ufshpb_attr;
};

struct ufshpb_lu_ctrl {
	struct ufs_hba *hba;
	struct srgn_node *srgn_array[HPB_SRGN_CNT];
	spinlock_t rsp_list_lock;
	struct list_head updating_srgn_list;
	struct list_head inactive_srgn_list;
	struct radix_tree_root active_srgn_tree;
	u64 ufshpb_add_node_count;
	u64 ufshpb_del_node_count;
};

struct ufs_hpb_info {
	int state;
	int entry_size;
	int rgn_size;
	int srgn_size;
	int host_table_size_per_srgn;
	int entry_cnt_per_srgn;
	u8 lu_number;
	u32 hpb_logical_block_size;
	u8 hpb_logical_block_count;
	u8 bud0_base_offset;
	u8 bud_config_plength;
	u16 hpb_version;
	u8 hpb_control_mode;
	u32 hpb_extended_ufsfeature_support;
	u16 hpb_device_max_active_rgns;
	u8 hpb_lu_enable[MAX_HPB_LUN_NUM];
	u16 hpb_lu_max_active_rgns[MAX_HPB_LUN_NUM];
	u16 hpb_lu_pinned_rgn_startidx[MAX_HPB_LUN_NUM];
	u16 hpb_lu_pinned_rgn_num[MAX_HPB_LUN_NUM];
	u64 read_total;
	u64 read_hit_count;
	u64 read_buffer_succ_cnt;
	u64 read_buffer_fail_count;
	u64 active_rgn_count;
	u64 inactive_rgn_count;

	struct ufs_hba *hba;
	struct ufshpb_debug debug_state;
	struct delayed_work ufshpb_init_work;
	struct work_struct hpb_update_work;
	struct ufshpb_lu_ctrl *ufshpb_lu_ctrl[MAX_HPB_LUN_NUM];
	bool in_suspend;
	bool in_shutdown;
};

void ufshpb_check_rsp_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp,
			int scsi_status, bool in_irq);
int ufshpb_probe(struct ufs_hba *hba);
void ufshpb_compose_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp);
void ufshpb_suspend(struct ufs_hba *hba, enum ufs_pm_op pm_op);
void ufshpb_shutdown(struct ufs_hba *hba);
#else

static inline void ufshpb_check_rsp_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp,
			int scsi_status, bool in_irq){};
static inline int ufshpb_probe(struct ufs_hba *hba)
{
	return 0;
}
static inline void ufshpb_compose_upiu(struct ufs_hba *hba,
			struct ufshcd_lrb *lrbp){};
static inline void ufshpb_suspend(struct ufs_hba *hba, enum ufs_pm_op pm_op){};
static inline void ufshpb_shutdown(struct ufs_hba *hba){};

#endif

#endif
