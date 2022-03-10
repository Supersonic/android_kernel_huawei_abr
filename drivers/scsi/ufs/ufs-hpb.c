/*
 * ufs-hpb.c
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

#include <linux/slab.h>
#include <linux/of.h>
#include <linux/sysfs.h>
#include <asm/unaligned.h>
#include <linux/async.h>
#include <linux/atomic.h>
#include <linux/blkdev.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/devfreq.h>
#include <linux/bitops.h>
#include <linux/fs.h>
#include <linux/radix-tree.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/list_sort.h>
#include <linux/version.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_host.h>
#include <linux/time.h>
#include "ufs.h"
#include "ufs-hpb.h"
#include "ufshcd.h"
#include "ufshci.h"
#include "ufs_quirks.h"

#define MODULE_NAME "ufs_hpb"
static struct workqueue_struct *ufshpb_wq;

static void ufshpb_fill_read_buffer_cdb(struct ufs_hba *hba,
			u8 *cdb, int rgn_id, int srgn_id)
{
	cdb[HPB_RB_OPCODE_OFFSET] = HPB_READ_BUFFER_OPCODE;
	cdb[HPB_RB_BUFFER_ID_OFFSET] = HPB_READ_BUFFER_ID;
	cdb[HPB_RB_REGION_ID_HIGH_OFFSET] = (unsigned char)GET_BYTE_1(rgn_id);
	cdb[HPB_RB_REGION_ID_LOW_OFFSET] = (unsigned char)GET_BYTE_0(rgn_id);
	cdb[HPB_RB_SUBREGION_ID_HIGH_OFFSET] = (unsigned char)GET_BYTE_1(srgn_id);
	cdb[HPB_RB_SUBREGION_ID_LOW_OFFSET] = (unsigned char)GET_BYTE_0(srgn_id);
	cdb[HPB_RB_AL_LEN3_OFFSET] =
		(unsigned char)GET_BYTE_2(hba->ufs_hpb->host_table_size_per_srgn);
	cdb[HPB_RB_AL_LEN2_OFFSET] =
		(unsigned char)GET_BYTE_1(hba->ufs_hpb->host_table_size_per_srgn);
	cdb[HPB_RB_AL_LEN1_OFFSET] =
		(unsigned char)GET_BYTE_0(hba->ufs_hpb->host_table_size_per_srgn);
	cdb[HPB_RB_CONTROL_OFFSET] = 0;
}

static int ufshpb_read_buffer(struct ufs_hba *hba, int rgn_id,
			int srgn_id, char *srgn_hpb_buffer, int lun)
{
	struct scsi_device *sdev = NULL;
	int err;
	unsigned char cdb[MAX_CDB_SIZE] = {0};
	struct scsi_sense_hdr sshdr;

	ufshpb_fill_read_buffer_cdb(hba, cdb, rgn_id, srgn_id);
	memset(srgn_hpb_buffer, '\0', hba->ufs_hpb->host_table_size_per_srgn);

	__shost_for_each_device(sdev, hba->host) {
		if (sdev->lun == lun)
			break;
	}

	err = scsi_execute_req(sdev, cdb, DMA_FROM_DEVICE,
			       srgn_hpb_buffer,
			       hba->ufs_hpb->host_table_size_per_srgn, &sshdr,
			       5 * HZ, 1, NULL);

	return err;
}

static inline void ufshpb_init_srgn_node_lists(struct srgn_node *srgn_node)
{
	INIT_LIST_HEAD(&srgn_node->updating_srgn_list);
	INIT_LIST_HEAD(&srgn_node->inactive_srgn_list);
}

static int ufshpb_alloc_srgn_node_mem(struct ufshpb_lu_ctrl *ufshpb_lu_ctrl)
{
	int i, j, k;
	u8 *base_addr = NULL;
	struct srgn_node *srgn_node = NULL;
	struct srgn_node *srgn_node_entry = NULL;
	u8 *temp_addr[HPB_ALLOC_MEM_CNT];
	int temp_count = 0;
	struct srgn_node *srgn_node_temp = NULL;

	k = 0;
	for (i = 0; i < HPB_ALLOC_MEM_CNT; i++) {
		base_addr = kzalloc(HPB_MEMSIZE_PER_LOOP, GFP_KERNEL);
		if (!base_addr)
			goto out_mem;
		temp_addr[i] = base_addr;
		temp_count++;
		for (j = 0; j < HPB_SRGN_CNT_PER_MEMSIZE; j++) {
			srgn_node = kzalloc(sizeof(struct srgn_node), GFP_KERNEL);
			if (!srgn_node)
				goto out_mem;
			ufshpb_lu_ctrl->srgn_array[k] = srgn_node;
			k++;
			srgn_node->node_addr =
				(u64 *)(base_addr + j * HPB_HOST_TABLE_SIZE_PER_SRGN);
			ufshpb_init_srgn_node_lists(srgn_node);
			/* add all subregion node to inactive list, default status node empty */
			list_add(&srgn_node->inactive_srgn_list,
				&ufshpb_lu_ctrl->inactive_srgn_list);
		}
	}
	return 0;
out_mem:
	for (k = 0; k < temp_count; k++)
		kfree(temp_addr[k]);
	list_for_each_entry_safe(srgn_node_entry, srgn_node_temp,
			&ufshpb_lu_ctrl->inactive_srgn_list, inactive_srgn_list) {
		list_del_init(&srgn_node_entry->inactive_srgn_list);
		kfree(srgn_node_entry);
	}
	return -ENOMEM;
}

static int ufshpb_rsp_is_invalid(struct ufshpb_rsp_sense_data *rsp_data)
{
	if (be16_to_cpu(rsp_data->sense_data_len) != HPB_SENSE_DATA_LEN ||
		rsp_data->desc_type != HPB_DESC_TYPE ||
		rsp_data->additional_len != HPB_ADDITIONAL_LEN ||
		rsp_data->hpb_opt == HPB_REGION_NONE ||
		rsp_data->act_cnt > HPB_MAX_ACTIVE_NUM ||
		rsp_data->inact_cnt > HPB_MAX_INACTIVE_NUM ||
		(!rsp_data->act_cnt && !rsp_data->inact_cnt) ||
		rsp_data->lun > UFS_UPIU_MAX_GENERAL_LUN)
		return -EINVAL;
	return 0;
}

static inline void ufshpb_init_active_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl)
{
	struct radix_tree_root *tree =
		&(ufshpb_lu_ctrl->active_srgn_tree);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(VERSION, PATCH_L, SUB_L)
	tree->xa_flags = GFP_ATOMIC | __GFP_NOWARN;
	tree->xa_head = NULL;
#else
	tree->gfp_mask = GFP_ATOMIC | __GFP_NOWARN;
	tree->rnode = NULL;
#endif
}

static inline struct srgn_node *ufshpb_get_active_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl, u32 id)
{
	struct radix_tree_root *tree =
		&(ufshpb_lu_ctrl->active_srgn_tree);

	return radix_tree_lookup(tree, id);
}

static inline void ufshpb_add_active_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl,
			struct srgn_node *node)
{
	struct radix_tree_root *tree =
			&(ufshpb_lu_ctrl->active_srgn_tree);

	radix_tree_insert(tree, node->srgn_id, node);
	ufshpb_lu_ctrl->ufshpb_add_node_count++;
}

static inline void ufshpb_del_active_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl,
			struct srgn_node *node)
{
	struct radix_tree_root *tree =
			&(ufshpb_lu_ctrl->active_srgn_tree);

	radix_tree_delete_item(tree, node->srgn_id, node);
}

static inline void ufshpb_init_updating_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl)
{
	INIT_LIST_HEAD(&ufshpb_lu_ctrl->updating_srgn_list);
}

static inline void ufshpb_add_updating_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl,
			struct srgn_node *node)
{
	list_add_tail(&node->updating_srgn_list,
		&ufshpb_lu_ctrl->updating_srgn_list);
}

static inline void ufshpb_del_updating_srgn(
			struct srgn_node *node)
{
	list_del_init(&node->updating_srgn_list);
}

static inline struct srgn_node *ufshpb_get_updating_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl)
{
	struct srgn_node *node = NULL;

	node = list_first_entry_or_null(&ufshpb_lu_ctrl->updating_srgn_list,
					struct srgn_node, updating_srgn_list);

	return node;
}

static inline void ufshpb_init_inactive_srgn(
			struct ufshpb_lu_ctrl *ufshpb_lu_ctrl)
{
	INIT_LIST_HEAD(&ufshpb_lu_ctrl->inactive_srgn_list);
}

static inline void ufshpb_add_inactive_srgn(
			struct ufshpb_lu_ctrl *hpb_lu_ctrl,
			struct srgn_node *node)
{
	list_add(&node->inactive_srgn_list,
		&hpb_lu_ctrl->inactive_srgn_list);
	hpb_lu_ctrl->ufshpb_del_node_count++;
}

static inline struct srgn_node *ufshpb_get_inactive_srgn(
			struct ufshpb_lu_ctrl *hpb_lu_ctrl)
{
	return container_of((&(hpb_lu_ctrl->inactive_srgn_list))->prev,
			 struct srgn_node, inactive_srgn_list);
}

static inline void ufshpb_del_inactive_srgn(
			struct srgn_node *node)
{
	list_del_init(&node->inactive_srgn_list);
}

static void ufshpb_update_srgns(struct ufshpb_lu_ctrl *ufshpb_lu_ctrl)
{
	struct srgn_node *node = NULL;
	struct ufs_hba *hba = NULL;
	int err;
	unsigned long flags;

	hba = ufshpb_lu_ctrl->hba;
	if (hba == NULL)
		return;
	spin_lock_irqsave(&ufshpb_lu_ctrl->rsp_list_lock, flags);
	while ((node = ufshpb_get_updating_srgn(ufshpb_lu_ctrl))) {
		if (hba->ufs_hpb->in_suspend || hba->ufs_hpb->in_shutdown)
			break;
		if (!node->node_addr)
			break;
		ufshpb_del_updating_srgn(node);
		node->status = NODE_NEW;
		spin_unlock_irqrestore(&ufshpb_lu_ctrl->rsp_list_lock, flags);
		err = ufshpb_read_buffer(hba, node->srgn_id,
					 0, (u8 *)node->node_addr, HPB_LUN);
		if (err) {
			hba->ufs_hpb->read_buffer_fail_count++;
			dev_err(hba->dev, "%s, read buffer failed, ret = %d\n",
				__func__, err);
		} else {
			hba->ufs_hpb->read_buffer_succ_cnt++;
#ifdef HPB_DEBUG_PRINT
			dev_err(hba->dev, "%s, node id %x updated\n", __func__,
			       node->srgn_id);
#endif
		}
		spin_lock_irqsave(&ufshpb_lu_ctrl->rsp_list_lock, flags);
	}
	spin_unlock_irqrestore(&ufshpb_lu_ctrl->rsp_list_lock, flags);
}

static void ufshpb_update_work_handler(struct work_struct *work)
{
	struct ufs_hpb_info *ufs_hpb = NULL;
	int i;
	ufs_hpb = container_of(work, struct ufs_hpb_info, hpb_update_work);
	if (ufs_hpb->state != UFSHPB_STATE_OPERATIONAL)
		return;

	for (i = 0; i < MAX_HPB_LUN_NUM; i++)
		if (ufs_hpb->ufshpb_lu_ctrl[i])
			ufshpb_update_srgns(ufs_hpb->ufshpb_lu_ctrl[i]);
}

static void ufshpb_inactive_srgns_by_rsp_data(
			struct ufs_hba *hba,
			struct ufshpb_rsp_sense_data *rsp_data,
			struct ufshcd_lrb *lrbp)
{
	int i;
	unsigned long flags;
	struct ufshpb_lu_ctrl *hpb_lu_ctrl = NULL;
	struct srgn_node *srgn_node = NULL;

	hpb_lu_ctrl = hba->ufs_hpb->ufshpb_lu_ctrl[HPB_LUN];
	if (!hpb_lu_ctrl)
		return;

	if (hba->ufs_hpb->in_suspend || hba->ufs_hpb->in_shutdown)
		return;
	spin_lock_irqsave(&hpb_lu_ctrl->rsp_list_lock, flags);
	for (i = 0; i < rsp_data->inact_cnt; i++) {
		srgn_node = ufshpb_get_active_srgn(hpb_lu_ctrl,
			i == 0 ? (be16_to_cpu(rsp_data->inact_rgn_0)) :
				 (be16_to_cpu(rsp_data->inact_rgn_1)));
		if (srgn_node) {
			srgn_node->status = NODE_EMPTY;
			ufshpb_del_updating_srgn(srgn_node);
			ufshpb_del_active_srgn(hpb_lu_ctrl, srgn_node);
			ufshpb_add_inactive_srgn(hpb_lu_ctrl, srgn_node);
		}
	}
	spin_unlock_irqrestore(&hpb_lu_ctrl->rsp_list_lock, flags);
}

static void ufshpb_active_srgns_by_rsp_data(
			struct ufs_hba *hba,
			struct ufshpb_rsp_sense_data *rsp_data,
			struct ufshcd_lrb *lrbp)
{
	int i;
	int act_rgn_id;
	unsigned long flags;
	struct ufshpb_lu_ctrl *hpb_lu_ctrl = NULL;
	struct srgn_node *srgn_node = NULL;

	hpb_lu_ctrl = hba->ufs_hpb->ufshpb_lu_ctrl[HPB_LUN];
	if (!hpb_lu_ctrl)
		return;

	if (hba->ufs_hpb->in_suspend || hba->ufs_hpb->in_shutdown)
		return;
	spin_lock_irqsave(&hpb_lu_ctrl->rsp_list_lock, flags);
	for (i = 0; i < rsp_data->act_cnt; i++) {
		act_rgn_id = (i == 0 ? (be16_to_cpu(rsp_data->act_rgn_0)) :
				 (be16_to_cpu(rsp_data->act_rgn_1)));
		srgn_node = ufshpb_get_active_srgn(hpb_lu_ctrl, act_rgn_id);
		if (srgn_node) {
			if (srgn_node->status == NODE_UPDATING)
				continue;
			srgn_node->status = NODE_UPDATING;
			ufshpb_add_updating_srgn(hpb_lu_ctrl, srgn_node);
		} else if (!list_empty(&hpb_lu_ctrl->inactive_srgn_list)) {
			srgn_node = ufshpb_get_inactive_srgn(hpb_lu_ctrl);
			srgn_node->srgn_id = act_rgn_id;
			srgn_node->status = NODE_UPDATING;
			ufshpb_add_active_srgn(hpb_lu_ctrl, srgn_node);
			ufshpb_del_inactive_srgn(srgn_node);
			ufshpb_add_updating_srgn(hpb_lu_ctrl, srgn_node);
		} else {
#ifdef HPB_DEBUG_PRINT
			dev_err(hba->dev, "%s: rgnd is full, add_cnt=%lld,del_cnt=%lld.\n",
			       __func__, hpb_lu_ctrl->ufshpb_add_node_count,
			       hpb_lu_ctrl->ufshpb_del_node_count);
#endif
		}
	}
	spin_unlock_irqrestore(&hpb_lu_ctrl->rsp_list_lock, flags);
	queue_work(ufshpb_wq, &hba->ufs_hpb->hpb_update_work);
}

static void ufshpb_rgn_update(struct ufs_hba *hba,
				 struct ufshpb_rsp_sense_data *rsp_data,
				 struct ufshcd_lrb *lrbp)
{
	ufshpb_inactive_srgns_by_rsp_data(hba, rsp_data, lrbp);
	ufshpb_active_srgns_by_rsp_data(hba, rsp_data, lrbp);
}

static inline struct ufshpb_rsp_sense_data *
ufshpb_get_rsp_sense_data(const struct ufshcd_lrb *lrbp)
{
	return (struct ufshpb_rsp_sense_data *)&lrbp->ucd_rsp_ptr->sr.sense_data_len;
}

void ufshpb_check_rsp_upiu(struct ufs_hba *hba,
			struct ufshcd_lrb *lrbp, int scsi_status, bool in_irq)
{
	struct ufshpb_rsp_sense_data *rsp_data = NULL;

	if (!in_irq || scsi_status != SAM_STAT_GOOD)
		return;

	if (hba == NULL || lrbp == NULL || hba->ufs_hpb == NULL)
		return;

	if (lrbp->lun != HPB_LUN)
		return;

	if (hba->ufs_hpb->in_suspend || hba->ufs_hpb->in_shutdown)
		return;

	if (hba->ufs_hpb->state != UFSHPB_STATE_OPERATIONAL)
		return;

	rsp_data = ufshpb_get_rsp_sense_data(lrbp);
	if (ufshpb_rsp_is_invalid(rsp_data))
		return;

	hba->ufs_hpb->active_rgn_count += rsp_data->act_cnt;
	hba->ufs_hpb->inactive_rgn_count += rsp_data->inact_cnt;

#ifdef HPB_DEBUG_PRINT
	pr_err("%s, hpb operation %x,lun %x,lrbp->lun %x\n", __func__,
			rsp_data->hpb_opt, rsp_data->lun, lrbp->lun);
	pr_err("%s,act_cnt %x, rgn_0 %x,srgn_0 %x,rgn_1 "
			"%x,srgn_1 %x\n",
			__func__, rsp_data->act_cnt,
			(be16_to_cpu(rsp_data->act_rgn_0)),
			(be16_to_cpu(rsp_data->act_srgn_0)),
			(be16_to_cpu(rsp_data->act_rgn_1)),
			(be16_to_cpu(rsp_data->act_srgn_1)));
	pr_err("%s,inact_cnt %x,rgn_0 %x, rgn_1 %x\n", __func__,
			rsp_data->inact_cnt, (be16_to_cpu(rsp_data->inact_rgn_0)),
			(be16_to_cpu(rsp_data->inact_rgn_1)));
#endif

	switch (rsp_data->hpb_opt) {
	case HPB_REGION_UPDATE_REQ:
		ufshpb_rgn_update(hba, rsp_data, lrbp);
		break;
	case HPB_REGION_RESET_REQ:
		break;
	default:
		dev_err(hba->dev, "hpb operation is not available : %d", rsp_data->hpb_opt);
		return;
	}
}

static void ufshpb_ppn_prep(struct ufs_hba *hba, struct ufshcd_lrb *lrbp,
			    u64 ppn)
{
	unsigned char cmd[MAX_CDB_SIZE] = {0};
	unsigned int transfer_len;

	transfer_len = SHIFT_BYTE_1(lrbp->cmd->cmnd[READ10_TRANS_LEN_HIGH_OFFSET]) |
		lrbp->cmd->cmnd[READ10_TRANS_LEN_LOW_OFFSET];

	cmd[HPB_READ16_OP_OFFSET] = READ_16;
	cmd[HPB_READ16_LBA1_OFFSET] = lrbp->cmd->cmnd[READ10_LBA1_OFFSET];
	cmd[HPB_READ16_LBA2_OFFSET] = lrbp->cmd->cmnd[READ10_LBA2_OFFSET];
	cmd[HPB_READ16_LBA3_OFFSET] = lrbp->cmd->cmnd[READ10_LBA3_OFFSET];
	cmd[HPB_READ16_LBA4_OFFSET] = lrbp->cmd->cmnd[READ10_LBA4_OFFSET];
	cmd[HPB_READ16_PPN1_OFFSET] = GET_BYTE_7(ppn);
	cmd[HPB_READ16_PPN2_OFFSET] = GET_BYTE_6(ppn);
	cmd[HPB_READ16_PPN3_OFFSET] = GET_BYTE_5(ppn);
	cmd[HPB_READ16_PPN4_OFFSET] = GET_BYTE_4(ppn);
	cmd[HPB_READ16_PPN5_OFFSET] = GET_BYTE_3(ppn);
	cmd[HPB_READ16_PPN6_OFFSET] = GET_BYTE_2(ppn);
	cmd[HPB_READ16_PPN7_OFFSET] = GET_BYTE_1(ppn);
	cmd[HPB_READ16_PPN8_OFFSET] = GET_BYTE_0(ppn);
	cmd[HPB_READ16_TRANS_LEN_OFFSET] = GET_BYTE_0(transfer_len);
	cmd[HPB_READ16_CONTROL_OFFSET] = HPB_READ16_CONTROL_VALUE;

	if (hba->manufacturer_id == UFS_VENDOR_SAMSUNG) {
		cmd[HPB_READ16_RESERVED0_OFFSET] = lrbp->cmd->cmnd[READ10_RESERVED0_OFFSET];
		cmd[HPB_READ16_TRANS_LEN_OFFSET] = HPB_SAMSUNG_BUFID;
		cmd[HPB_READ16_CONTROL_OFFSET] = GET_BYTE_0(transfer_len);
		lrbp->cmd->cmd_len = MAX_CDB_SIZE;
	}

	memcpy(lrbp->cmd->cmnd, cmd, MAX_CDB_SIZE);
	memcpy(lrbp->ucd_req_ptr->sc.cdb, cmd, MAX_CDB_SIZE);
}

static int ufshpb_get_device_desc(struct ufs_hba *hba, u8 *device_desc_buf)
{
	int ret;
	int size = HPB_DEVICE_DESC_SIZE;
	int selector = SELECTOR_PROTOCOL;

	if (hba->manufacturer_id == UFS_VENDOR_SAMSUNG) {
		size = HPB_DEVICE_DESC_SIZE_SAMSUNG;
		selector = SELECTOR_SAMSUNG;
	}
	ret = ufshcd_query_descriptor_retry(hba, UPIU_QUERY_OPCODE_READ_DESC,
				    QUERY_DESC_IDN_DEVICE, 0,
				    selector,
				    device_desc_buf, &size);
	if (ret)
		pr_err("%s: read device desc error, ret = %d\n", __func__, ret);
	return ret;
}

static int ufshpb_get_geometry_desc(struct ufs_hba *hba, u8 *geo_desc_buf)
{
	int ret;
	int size = HPB_GEOMETRY_DESC_SIZE;
	int selector = SELECTOR_PROTOCOL;

	if (hba->manufacturer_id == UFS_VENDOR_SAMSUNG) {
		size = HPB_GEOMETRY_DESC_SIZE_SAMSUNG;
		selector = SELECTOR_SAMSUNG;
	}
	ret = ufshcd_query_descriptor_retry(hba, UPIU_QUERY_OPCODE_READ_DESC,
				    QUERY_DESC_IDN_GEOMETRY, 0,
				    selector,
				    geo_desc_buf, &size);
	if (ret)
		pr_err("%s: read geometry desc error, ret = %d\n", __func__, ret);
	return ret;
}

static int ufshpb_get_unit_desc(struct ufs_hba *hba, u8 lun, u8 *unit_desc_buf)
{
	int ret;
	int size = HPB_UNIT_DESC_SIZE;
	int selector = SELECTOR_PROTOCOL;
	if (hba->manufacturer_id == UFS_VENDOR_SAMSUNG)
		selector = SELECTOR_SAMSUNG;

	ret = ufshcd_query_descriptor_retry(hba, UPIU_QUERY_OPCODE_READ_DESC,
				    QUERY_DESC_IDN_UNIT, lun,
				    selector,
				    unit_desc_buf, &size);
	if (ret)
		pr_err("%s: read geometry desc error,ret = %d\n", __func__, ret);
	return ret;
}

static int ufshpb_update_info_from_geo_desc(struct ufs_hba *hba)
{
	int ret;
	u8 *geo_desc_buf = NULL;

	geo_desc_buf = kzalloc(HPB_GEOMETRY_DESC_SIZE_MAX, GFP_KERNEL);
	if (!geo_desc_buf)
		return -ENOMEM;
	ret = ufshpb_get_geometry_desc(hba, geo_desc_buf);
	if (ret)
		goto out;
	hba->ufs_hpb->rgn_size =
		(1 << geo_desc_buf[GEOMETRY_DESC_PARAM_HPB_REGION_SIZE]) *
		HPB_SECTOR_SIZE;
	hba->ufs_hpb->srgn_size =
		(1 << geo_desc_buf[GEOMETRY_DESC_PARAM_HPB_SUBREGION_SIZE]) *
		HPB_SECTOR_SIZE;
	hba->ufs_hpb->lu_number = geo_desc_buf[GEOMETRY_DESC_PARAM_HPB_LU_NUMBER];
	hba->ufs_hpb->hpb_device_max_active_rgns =
		cpu_to_be16(*((u16 *)(&geo_desc_buf[GEOMETRY_DESC_PARAM_HPB_MAX_ACT_REGIONS])));

out:
	kfree(geo_desc_buf);
	return ret;
}

static int ufshpb_update_info_from_unit_desc(struct ufs_hba *hba)
{
	int ret;
	u8 *unit_desc_buf = NULL;

	unit_desc_buf = kzalloc(HPB_UNIT_DESC_SIZE_MAX, GFP_KERNEL);
	if (!unit_desc_buf)
		return -ENOMEM;
	ret = ufshpb_get_unit_desc(hba, HPB_LUN, unit_desc_buf);
	if (ret)
		goto out;

	hba->ufs_hpb->hpb_lu_enable[HPB_LUN] =
		unit_desc_buf[UNIT_DESC_PARAM_LU_ENABLE];
	hba->ufs_hpb->hpb_logical_block_size =
		(1 << unit_desc_buf[UNIT_DESC_PARAM_LOGICAL_BLK_SIZE]);
	hba->ufs_hpb->hpb_logical_block_count = cpu_to_be64(
		*((u64 *)(&unit_desc_buf[UNIT_DESC_PARAM_LOGICAL_BLK_COUNT])));
	hba->ufs_hpb->hpb_lu_max_active_rgns[HPB_LUN] = cpu_to_be16(*((
		u16 *)(&unit_desc_buf[UNIT_DESC_PARAM_MAX_ACTIVE_HPB_REGIONS])));
	hba->ufs_hpb->hpb_lu_pinned_rgn_startidx[HPB_LUN] = cpu_to_be16(*(
		(u16 *)(&unit_desc_buf[UNIT_DESC_PARAM_PINNED_REGION_STARTIDX])));
	hba->ufs_hpb->hpb_lu_pinned_rgn_num[HPB_LUN] = cpu_to_be16(
		*((u16 *)(&unit_desc_buf[UNIT_DESC_PARAM_NUM_PINNED_REGIONS])));
	pr_err("%s: lu enable %x\n", __func__,
		hba->ufs_hpb->hpb_lu_enable[HPB_LUN]);
out:
	kfree(unit_desc_buf);
	return ret;
}

static int ufshpb_update_info_from_dev_desc(struct ufs_hba *hba)
{
	int ret;
	u8 *device_desc_buf = NULL;

	device_desc_buf = kzalloc(HPB_DEVICE_DESC_SIZE_MAX, GFP_KERNEL);
	if (!device_desc_buf)
		return -ENOMEM;

	ret = ufshpb_get_device_desc(hba, device_desc_buf);
	if (ret)
		goto out;

	hba->ufs_hpb->hpb_version =
		cpu_to_be16(*((u16 *)(&device_desc_buf[DEVICE_DESC_PARAM_HPB_VERSION])));
	hba->ufs_hpb->hpb_extended_ufsfeature_support =
		cpu_to_be32(*((u32 *)(&device_desc_buf[DEVICE_DESC_PARAM_EXT_UFS_FEATURE_SUP])));
	hba->ufs_hpb->bud0_base_offset = device_desc_buf[DEVICE_DESC_PARAM_UD_OFFSET];
	hba->ufs_hpb->bud_config_plength = device_desc_buf[DEVICE_DESC_PARAM_UD_LEN];
	hba->ufs_hpb->hpb_control_mode =
		device_desc_buf[DEVICE_DESC_PARAM_HPB_CONTROL];
	pr_err("%s: hpb version %x device desc feature support %x\n",
		__func__, hba->ufs_hpb->hpb_version,
		hba->ufs_hpb->hpb_extended_ufsfeature_support);

out:
	kfree(device_desc_buf);
	return ret;
}

static int ufshpb_update_basic_info(struct ufs_hba *hba)
{
	int ret;

	ret = ufshpb_update_info_from_geo_desc(hba);
	if (ret) {
		pr_err("%s, update geo info failed! ret = %d\n", __func__, ret);
		return ret;
	}
	ret = ufshpb_update_info_from_unit_desc(hba);
	if (ret) {
		pr_err("%s, update unit info failed! ret = %d\n", __func__, ret);
		return ret;
	}
	ret = ufshpb_update_info_from_dev_desc(hba);
	if (ret) {
		pr_err("%s, update dev info failed! ret = %d\n", __func__, ret);
		return ret;
	}
	hba->ufs_hpb->entry_size = HPB_ENTRY_SIZE;
	hba->ufs_hpb->entry_cnt_per_srgn =
		hba->ufs_hpb->srgn_size / UFS_BLOCK_SIZE;
	hba->ufs_hpb->host_table_size_per_srgn =
		hba->ufs_hpb->entry_cnt_per_srgn * hba->ufs_hpb->entry_size;

	return ret;
}

static int ufshpb_init_per_lun(struct ufs_hba *hba, int lun)
{
	struct ufshpb_lu_ctrl *hpb_lu_ctrl = NULL;
	unsigned int ret;

	ret = ufshpb_update_basic_info(hba);
	if (ret)
		return ret;

	hpb_lu_ctrl =
		kzalloc(sizeof(struct ufshpb_lu_ctrl), GFP_KERNEL);
	if (!hpb_lu_ctrl) {
		pr_err("%s: hpb kzalloc lun control error\n", __func__);
		return -ENOMEM;
	}

	hba->ufs_hpb->ufshpb_lu_ctrl[lun] = hpb_lu_ctrl;
	hpb_lu_ctrl->hba = hba;
	spin_lock_init(&hpb_lu_ctrl->rsp_list_lock);
	ufshpb_init_active_srgn(hpb_lu_ctrl);
	ufshpb_init_updating_srgn(hpb_lu_ctrl);
	ufshpb_init_inactive_srgn(hpb_lu_ctrl);
	ret = ufshpb_alloc_srgn_node_mem(hpb_lu_ctrl);
	if (ret) {
		kfree(hpb_lu_ctrl);
		return ret;
	}
	return ret;
}

static int ufshcd_query_flag_common(struct ufs_hba *hba,
	enum query_opcode opcode, enum flag_idn idn, bool *flag_res)
{
	if (hba == NULL || flag_res == NULL)
		return -EINVAL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(VERSION, PATCH_L, SUB_L)
	return ufshcd_query_flag(hba, opcode, idn, 0, flag_res);
#else
	return ufshcd_query_flag(hba, opcode, idn, flag_res);
#endif
}


static int ufshpb_reset_process(struct ufs_hba *hba)
{
	int ret;
	int retry_count = 0;
	bool hpb_after_reset = true;

	ret = ufshcd_query_flag_common(hba, UPIU_QUERY_OPCODE_SET_FLAG,
				QUERY_FLAG_IDN_HPB_RESET, NULL);
	if (ret) {
		dev_err(hba->dev, "%s, HPB reset error, ret = %d\n", __func__, ret);
		return ret;
	}

	while (hpb_after_reset) {
		retry_count++;
		ret = ufshcd_query_flag_common(hba, UPIU_QUERY_OPCODE_READ_FLAG,
					QUERY_FLAG_IDN_HPB_RESET, &hpb_after_reset);
		if (ret) {
			dev_err(hba->dev, "query hpb reset status failed! ret = %d\n", ret);
			return ret;
		}

		if (retry_count >= HPB_RESET_MAX_COUNT)
			return -EINVAL;
	}
	dev_info(hba->dev, "HPB reset is success!\n");
	return 0;
}

static void ufshpb_basic_print(struct ufs_hba *hba)
{
	if (hba->ufs_hpb == NULL)
		return;

	pr_info("bud0 base offset %lx bud config plength %lx\n",
			hba->ufs_hpb->bud0_base_offset,
			hba->ufs_hpb->bud_config_plength);
	pr_info("hpb version %lx\n", hba->ufs_hpb->hpb_version);
	pr_info("hpb lu number %lx hpb rgn size %lx\n",
			hba->ufs_hpb->lu_number,
			hba->ufs_hpb->rgn_size);
	pr_info("hpb srgn size %lx hpb device max active rgns %lx\n",
			hba->ufs_hpb->srgn_size,
			hba->ufs_hpb->hpb_device_max_active_rgns);
	pr_info("hpb lu enable %lx, hpb logical block size %lx, hpb logical block count %lx\n",
			hba->ufs_hpb->hpb_lu_enable[HPB_LUN],
			hba->ufs_hpb->hpb_logical_block_size,
			hba->ufs_hpb->hpb_logical_block_count);
	pr_info("hpb lu configured max active rgns %lx, pinned rgn startidx %lx, pinned rgn num %lx\n",
			hba->ufs_hpb->hpb_lu_max_active_rgns[HPB_LUN],
			hba->ufs_hpb->hpb_lu_pinned_rgn_startidx[HPB_LUN],
			hba->ufs_hpb->hpb_lu_pinned_rgn_num[HPB_LUN]);
	pr_info("hpb control mode %d\n", hba->ufs_hpb->hpb_control_mode);
	pr_info("host table size per srgn %x, one node entry number %x\n",
			hba->ufs_hpb->host_table_size_per_srgn,
			hba->ufs_hpb->entry_cnt_per_srgn);
}

static ssize_t ufshpb_debug_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct ufs_hba *hba = dev_get_drvdata(dev);
	if (hba->ufs_hpb->state == UFSHPB_STATE_OPERATIONAL) {
		ufshpb_basic_print(hba);
		return snprintf(buf, PAGE_SIZE,
			"%s\nhit/total=%lld/%lld(%d%%)\nread buffer count %d\n\
			hpb_active_count:%d\nufsread_buffer_fail_count:%d\nadd_node:%d\ndel_count:%d\n",
			"ufshpb_turn_on",
			hba->ufs_hpb->read_hit_count,
			hba->ufs_hpb->read_total,
			hba->ufs_hpb->read_hit_count * 100 / hba->ufs_hpb->read_total,
			hba->ufs_hpb->read_buffer_succ_cnt,
			hba->ufs_hpb->active_rgn_count,
			hba->ufs_hpb->read_buffer_fail_count,
			hba->ufs_hpb->ufshpb_lu_ctrl[HPB_LUN]->ufshpb_add_node_count,
			hba->ufs_hpb->ufshpb_lu_ctrl[HPB_LUN]->ufshpb_del_node_count);
	} else {
		return snprintf(buf, PAGE_SIZE, "%s\n", "ufshpb_turn_off");
	}
}

static ssize_t ufshpb_debug_store(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t count)
{
	struct ufs_hba *hba = dev_get_drvdata(dev);

	if (sysfs_streq(buf, "ufshpb_turn_off")) {
		hba->ufs_hpb->state = UFSHPB_STATE_DISABLED;
	} else if (sysfs_streq(buf, "ufshpb_turn_on")) {
		hba->ufs_hpb->state = UFSHPB_STATE_OPERATIONAL;
	} else {
		dev_err(hba->dev, "%s: invalid input debug parameter.\n",
			__func__);
		return -EINVAL;
	}
	return count;
}

static void ufshpb_debug_init(struct ufs_hba *hba)
{
	hba->ufs_hpb->debug_state.ufshpb_attr.show =
		ufshpb_debug_show;
	hba->ufs_hpb->debug_state.ufshpb_attr.store =
		ufshpb_debug_store;
	sysfs_attr_init(&hba->ufs_hpb->debug_state.ufshpb_attr.attr);
	hba->ufs_hpb->debug_state.ufshpb_attr.attr.name = "ufshpb_debug";
	hba->ufs_hpb->debug_state.ufshpb_attr.attr.mode =
		0640; /* 0640 node attribute mode */
	if (device_create_file(hba->dev, &hba->ufs_hpb->debug_state.ufshpb_attr))
		dev_err(hba->dev, "Failed to create sysfs for ufshpb\n");
}

static int ufshpb_init(struct ufs_hba *hba)
{
	unsigned int lun;
	unsigned int ret = 0;

	pm_runtime_get_sync(hba->dev);

	if (hba->ufs_hpb->state != UFSHPB_STATE_NEED_INIT) {
		dev_err(hba->dev, "ufs hpb has been initialized!\n");
		goto out;
	}

	if (hba->manufacturer_id != UFS_VENDOR_SAMSUNG) {
		ret = ufshpb_reset_process(hba);
		if (ret)
			goto out;
	}

	for (lun = 0; lun < MAX_HPB_LUN_NUM; lun++) {
		hba->ufs_hpb->ufshpb_lu_ctrl[lun] = NULL;
		if (lun == HPB_LUN) {
			ret = ufshpb_init_per_lun(hba, lun);
			if (ret)
				goto out;
		}
	}

	ufshpb_wq = alloc_workqueue("ufshpb-wq", WQ_UNBOUND | WQ_MEM_RECLAIM, 0);
	INIT_WORK(&hba->ufs_hpb->hpb_update_work, ufshpb_update_work_handler);
	hba->ufs_hpb->state = UFSHPB_STATE_OPERATIONAL;
	dev_info(hba->dev, "hpb init success!\n");
	pm_runtime_put_sync(hba->dev);
	return ret;
out:
	kfree(hba->ufs_hpb);
	hba->ufs_hpb = NULL;
	pm_runtime_put_sync(hba->dev);
	return ret;
}

static bool ufshpb_cap_support(struct ufs_hba *hba)
{
	int ret;
	u64 ufs_hpb_support;
	u8 *device_desc_buf = NULL;
	u64 support_offset = DEVICE_DESC_PARAM_EXT_UFS_FEATURE_SUP;

	device_desc_buf = kzalloc(HPB_DEVICE_DESC_SIZE_MAX, GFP_KERNEL);
	if (!device_desc_buf)
		return false;

	ret = ufshpb_get_device_desc(hba, device_desc_buf);
	if (ret) {
		dev_err(hba->dev, "%s: read ufs hpb support err, don't use hpb\n",
		       __func__);
		kfree(device_desc_buf);
		return false;
	}

	ufs_hpb_support =
		cpu_to_be32(*((u32 *)(&device_desc_buf[support_offset])));
	pr_info("%s: hpb support %llx\n", __func__, ufs_hpb_support);

	kfree(device_desc_buf);
	return ufs_hpb_support & HPB_SUPPORT_BIT;
}

static bool ufshpb_lun_support(struct ufs_hba *hba)
{
	bool ret = true;
	int err;
	u8 *unit_desc_buf = NULL;

	unit_desc_buf = kzalloc(HPB_UNIT_DESC_SIZE_MAX, GFP_KERNEL);
	if (!unit_desc_buf)
		return false;

	err = ufshpb_get_unit_desc(hba, HPB_LUN, unit_desc_buf);
	if (err) {
		ret = false;
		goto out;
	}
	if (unit_desc_buf[UNIT_DESC_PARAM_LU_ENABLE] != HPB_UNIT_LU_ENABLE) {
		ret = false;
		goto out;
	}
out:
	kfree(unit_desc_buf);
	return ret;
}

static void ufshpb_init_handler(struct work_struct *work)
{
	struct ufs_hba *hba = NULL;
	int err;
	struct delayed_work *dwork = to_delayed_work(work);

	hba = ((struct ufs_hpb_info *)(container_of(dwork, struct ufs_hpb_info,
						    ufshpb_init_work)))->hba;

	if (hba->ufs_hpb && (hba->ufs_hpb->state == UFSHPB_STATE_NEED_INIT)) {
		dev_err(hba->dev, "ufshpb init start!\n");
		err = ufshpb_init(hba);
		if (err)
			pr_err("ufshpb init no need\n");
	}
}

int ufshpb_probe(struct ufs_hba *hba)
{
	if (!(ufshpb_cap_support(hba)) || !(ufshpb_lun_support(hba))) {
		dev_err(hba->dev, "%s: ufs not support hpb!\n", __func__);
		return -1;
	}

	hba->ufs_hpb = kzalloc(sizeof(struct ufs_hpb_info), GFP_KERNEL);
	if (!hba->ufs_hpb) {
		dev_err(hba->dev, "%s: alloc ufs_hpb_info failed!\n", __func__);
		return -EINVAL;
	}
	ufshpb_debug_init(hba);
	hba->ufs_hpb->state = UFSHPB_STATE_NEED_INIT;
	hba->ufs_hpb->hba = hba;
	INIT_DELAYED_WORK(&hba->ufs_hpb->ufshpb_init_work, ufshpb_init_handler);
	schedule_delayed_work(&hba->ufs_hpb->ufshpb_init_work,
			      msecs_to_jiffies(0));
	return 0;
}

static bool ufshpb_is_valid_read(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	unsigned int sector_cnt;
	unsigned int transfer_len;
	int hpb_read_size_min = HPB_READ_SIZE_MIN;
	int hpb_read_size_max = HPB_READ_SIZE_MAX;

	if (!hba->ufs_hpb)
		return false;

	if (hba->ufs_hpb->state != UFSHPB_STATE_OPERATIONAL)
		return false;

	if ((lrbp->cmd->request == NULL) || (lrbp->lun != HPB_LUN))
		return false;

	if (lrbp->cmd->cmnd[0] != READ_10)
		return false;

	if (hba->manufacturer_id == UFS_VENDOR_SAMSUNG)
		hpb_read_size_max = HPB_READ_SIZE_MAX_SAMSUNG;

	sector_cnt = blk_rq_sectors(lrbp->cmd->request);
	transfer_len = sector_cnt * HPB_SECTOR_SIZE;
	if (transfer_len < hpb_read_size_min || transfer_len > hpb_read_size_max) {
#ifdef HPB_HISI_DEBUG_PRINT
		pr_err("%s: hpb don't support read length exceed %d-%d\n",
			__func__, hpb_read_size_min, hpb_read_size_max);
#endif
		return false;
	}
	return true;
}

static void ufshpb_upiu_prep(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	struct ufshpb_lu_ctrl *hpb_lu_ctrl = NULL;
	u64 lba;
	u64 ppn;
	u64 ppn_offset, srgn_id;
	unsigned long long sector;
	struct srgn_node *srgn_node = NULL;
	unsigned long flags;

	if (!ufshpb_is_valid_read(hba, lrbp))
		return;

	sector = blk_rq_pos(lrbp->cmd->request);
	hba->ufs_hpb->read_total++;
	lba = sector * HPB_SECTOR_SIZE / UFS_BLOCK_SIZE;
	hpb_lu_ctrl = hba->ufs_hpb->ufshpb_lu_ctrl[lrbp->lun];
	if (!hpb_lu_ctrl)
		return;
	srgn_id = lba / hba->ufs_hpb->entry_cnt_per_srgn;
	ppn_offset = lba % hba->ufs_hpb->entry_cnt_per_srgn;

	spin_lock_irqsave(&hpb_lu_ctrl->rsp_list_lock, flags);
	srgn_node = ufshpb_get_active_srgn(hpb_lu_ctrl, srgn_id);
	spin_unlock_irqrestore(&hpb_lu_ctrl->rsp_list_lock, flags);

	if (srgn_node) {
		if (srgn_node->status != NODE_NEW)
			return;
		ppn = *(srgn_node->node_addr + ppn_offset);
		ufshpb_ppn_prep(hba, lrbp, ppn);
		hba->ufs_hpb->read_hit_count++;
	}
}

void ufshpb_compose_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	if (hba == NULL) {
		dev_err(hba->dev, "%s: hba is NULL\n", __func__);
		return;
	}
	if (lrbp == NULL) {
		dev_err(hba->dev, "%s: lrbp is NULL\n", __func__);
		return;
	}

	if (hba->ufs_hpb && (hba->ufs_hpb->state == UFSHPB_STATE_OPERATIONAL))
		ufshpb_upiu_prep(hba, lrbp);
}

static void ufshpb_suspend_wait(struct ufs_hba *hba)
{
	if (hba->ufs_hpb && (hba->ufs_hpb->state == UFSHPB_STATE_OPERATIONAL))
		cancel_work_sync(&hba->ufs_hpb->hpb_update_work);
}

void ufshpb_suspend(struct ufs_hba *hba, enum ufs_pm_op pm_op)
{
	if (hba == NULL)
		return;
	if (hba->ufs_hpb == NULL)
		return;
	if (!ufshcd_is_runtime_pm(pm_op)) {
		hba->ufs_hpb->in_suspend = true;
		ufshpb_suspend_wait(hba);
	}
}

void ufshpb_shutdown(struct ufs_hba *hba)
{
	if (hba == NULL)
		return;
	if (hba->ufs_hpb == NULL)
		return;
	hba->ufs_hpb->in_shutdown = true;
	ufshpb_suspend_wait(hba);
}

MODULE_AUTHOR("HUAWEI");
MODULE_DESCRIPTION("HPB FEATURE");
MODULE_LICENSE("GPL");
MODULE_VERSION("V1.0");
