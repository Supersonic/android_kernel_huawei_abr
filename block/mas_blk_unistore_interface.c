/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: mas block unistore interface for hmfs
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) "[BLK-IO]" fmt

#include <linux/kernel.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/module.h>
#include <linux/bio.h>
#include <linux/delay.h>
#include <linux/gfp.h>
#include <linux/types.h>
#include "blk.h"

#ifdef CONFIG_MAS_UNISTORE_PRESERVE
int mas_blk_device_pwron_info_sync(struct block_device *bi_bdev,
	struct stor_dev_pwron_info *stor_info, unsigned int rescue_seg_size)
{
	int ret;
	unsigned int i;
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_pwron_info_sync)
		return -EPERM;

	if (!rescue_seg_size) {
		pr_err("%s rescue_seg_size is %u\n", __func__, rescue_seg_size);
		return -EINVAL;
	}

	stor_info->done_info.done = NULL;

	ret = lld->unistore_ops.dev_pwron_info_sync(q, stor_info, rescue_seg_size);
	if (ret)
		return ret;

	for (i = 0; i < BLK_STREAM_MAX_STRAM; i++) {
		if (stor_info->dev_stream_addr[i] != 0)
			stor_info->dev_stream_addr[i] =
				stor_info->dev_stream_addr[i] -
				(unsigned int)(bi_bdev->bd_part->start_sect >> SECTION_SECTOR);
	}
	for (i = 0; i < stor_info->rescue_seg_cnt; i++)
		stor_info->rescue_seg[i] =
			(stor_info->rescue_seg[i] << SECTOR_BYTE) -
			(unsigned int)(bi_bdev->bd_part->start_sect >> SECTION_SECTOR);

	for (i = 0; i < DATA_MOVE_STREAM_NUM; i++) {
		if (stor_info->dm_stream_addr[i] != 0)
			stor_info->dm_stream_addr[i] =
				stor_info->dm_stream_addr[i] -
				(unsigned int)(bi_bdev->bd_part->start_sect >> SECTION_SECTOR);
	}

	return ret;
}

int mas_blk_stream_oob_info_fetch(struct block_device *bi_bdev,
				struct stor_dev_stream_info stream_info,
				unsigned int oob_entry_cnt,
				struct stor_dev_stream_oob_info *oob_info)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_stream_oob_info_fetch)
		return -EPERM;

	stream_info.stream_start_lba += (
		bi_bdev->bd_part->start_sect >> SECTION_SECTOR);

	return lld->unistore_ops.dev_stream_oob_info_fetch(
		q, stream_info, oob_entry_cnt, oob_info);
}

static void mas_blk_clear_section_info(
	struct blk_dev_lld *lld, unsigned char stream)
{
	unsigned long flags;
	struct unistore_section_info *section_info = NULL;

	spin_lock_irqsave(&lld->expected_lba_lock[stream - 1], flags);
#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
	if (lld->mas_sec_size)
		pr_err("%s, old_section: 0x%llx, expected_lba: 0x%llx - 0x%llx ",
			"expected_pu: 0x%llx, current_pu_size: 0x%llx\n",
			__func__, lld->old_section[stream - 1],
			lld->expected_lba[stream - 1] / lld->mas_sec_size,
			lld->expected_lba[stream - 1] % lld->mas_sec_size,
			lld->expected_pu[stream - 1], lld->current_pu_size[stream - 1]);
#endif
	do {
		if (list_empty_careful(&lld->section_list[stream - 1]))
			break;

		section_info = list_first_entry(
				&lld->section_list[stream - 1],
				struct unistore_section_info, section_list);
#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
		if (lld->mas_sec_size)
			pr_err("%s, section_info: 0x%llx\n", __func__,
				section_info->section_start_lba / lld->mas_sec_size);
#endif
		list_del_init(&section_info->section_list);
		kfree(section_info);
	} while (1);

	lld->expected_lba[stream - 1] = 0;
	lld->old_section[stream - 1] = 0;
	lld->expected_pu[stream - 1] = 0;
	lld->current_pu_size[stream - 1] = 0;
	spin_unlock_irqrestore(&lld->expected_lba_lock[stream - 1], flags);
}

int mas_blk_device_reset_ftl(struct block_device *bi_bdev)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);
	struct stor_dev_reset_ftl reset_ftl_info = {0};
	int ret;
	unsigned char stream;


	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_reset_ftl)
		return -EPERM;

	ret = lld->unistore_ops.dev_reset_ftl(q, &reset_ftl_info);

	for (stream = 0; stream < BLK_ORDER_STREAM_NUM; stream++)
		mas_blk_clear_section_info(lld, stream + 1); /* ORDER STREAM */

	return ret;
}

int mas_blk_device_close_section(struct block_device *bi_bdev,
	struct stor_dev_reset_ftl *reset_ftl_info)
{
	int ret;
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_reset_ftl)
		return -EPERM;

	ret = lld->unistore_ops.dev_reset_ftl(q, reset_ftl_info);
	if (ret) {
		pr_err("%s, close section failed: %u\n", __func__, ret);
		return ret;
	}

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
	pr_err("%s, op_type: %u, stream_type: %u, stream_id: %u\n", __func__,
		reset_ftl_info->op_type, reset_ftl_info->stream_type, reset_ftl_info->stream_id);
#endif
	/*
	 * op_type: 0:reset ftl, 1:close section
	 * stream_type: 0:normal, 1: datamove
	 */
	if ((reset_ftl_info->op_type == 1) &&
		(reset_ftl_info->stream_type == 0) &&
		mas_blk_is_order_stream(reset_ftl_info->stream_id))
		mas_blk_clear_section_info(lld, reset_ftl_info->stream_id);

	return ret;
}

int mas_blk_device_read_section(
	struct block_device *bi_bdev, unsigned int *section_size)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_read_section)
		return -EPERM;

	return lld->unistore_ops.dev_read_section(q, section_size);
}

int mas_blk_device_config_mapping_partition(
	struct block_device *bi_bdev,
	struct stor_dev_mapping_partition *mapping_info)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_config_mapping_partition)
		return -EPERM;

	return lld->unistore_ops.dev_config_mapping_partition(q, mapping_info);
}

int mas_blk_device_read_mapping_partition(
	struct block_device *bi_bdev,
	struct stor_dev_mapping_partition *mapping_info)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_read_mapping_partition)
		return -EPERM;

	return lld->unistore_ops.dev_read_mapping_partition(q, mapping_info);
}

int mas_blk_device_read_op_size(struct block_device *bi_bdev,
	int *op_size)
{
	int ret;
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_read_op_size)
		return -EPERM;

	ret = lld->unistore_ops.dev_read_op_size(q, op_size);
	if (unlikely(ret))
		pr_err("%s, read op size failed, ret = %d\n", __func__, ret);

	return ret;
}

int mas_blk_fs_sync_done(struct block_device *bi_bdev)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_fs_sync_done)
		return -EPERM;

	return lld->unistore_ops.dev_fs_sync_done(q);
}

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
int mas_blk_rescue_block_inject_data(struct block_device *bi_bdev, sector_t sect)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);
	unsigned int lba = sect + (bi_bdev->bd_part->start_sect >> SECTION_SECTOR);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_rescue_block_inject_data)
		return -EPERM;

	return lld->unistore_ops.dev_rescue_block_inject_data(q, lba);
}

int mas_blk_bad_block_error_inject(struct block_device *bi_bdev,
	unsigned char bad_slc_cnt, unsigned char bad_tlc_cnt)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_bad_block_err_inject)
		return -EPERM;

	return lld->unistore_ops.dev_bad_block_err_inject(q, bad_slc_cnt, bad_tlc_cnt);
}
#endif

int mas_blk_data_move(struct block_device *bi_bdev,
	struct stor_dev_data_move_info *data_move_info)
{
	unsigned int i;
	int ret;
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);
	struct stor_dev_data_move_source_addr *source_addr = NULL;
	sector_t start_sect = bi_bdev->bd_part->start_sect >> SECTION_SECTOR;

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!data_move_info)
		return -EFAULT;
	if ((data_move_info->source_addr_num > DATA_MOVE_MAX_NUM) ||
			(data_move_info->source_inode_num > DATA_MOVE_MAX_NUM))
		return -EINVAL;

	data_move_info->dest_4k_lba += start_sect;
	data_move_info->done_info.start_addr = start_sect;

	for (i = 0; i < data_move_info->source_addr_num; i++) {
		source_addr = (data_move_info->source_addr) + i;
		source_addr->data_move_source_addr += start_sect;
	}

	if (!lld || !lld->unistore_ops.dev_data_move)
		return -EPERM;

	ret = lld->unistore_ops.dev_data_move(q, data_move_info);

	data_move_info->verify_info.next_to_be_verify_4k_lba -= start_sect;
	data_move_info->verify_info.next_available_write_4k_lba -= start_sect;

	return ret;
}

int mas_blk_slc_mode_configuration(struct block_device *bi_bdev, int *status)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_slc_mode_configuration)
		return -EPERM;

	return lld->unistore_ops.dev_slc_mode_configuration(q, status);
}

int mas_blk_sync_read_verify(struct block_device *bi_bdev,
	struct stor_dev_sync_read_verify_info *verify_info)
{
	int ret;
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);
	sector_t start_sect = bi_bdev->bd_part->start_sect >> SECTION_SECTOR;

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (verify_info->cp_verify_l4k)
		verify_info->cp_verify_l4k += start_sect;

	if (verify_info->cp_open_l4k)
		verify_info->cp_open_l4k += start_sect;

	if (verify_info->cp_cache_l4k)
		verify_info->cp_cache_l4k += start_sect;

	if (!lld || !lld->unistore_ops.dev_sync_read_verify)
		return -EPERM;

	ret = lld->unistore_ops.dev_sync_read_verify(q, verify_info);

	if (verify_info->verify_info.next_to_be_verify_4k_lba)
		verify_info->verify_info.next_to_be_verify_4k_lba -= start_sect;
	if (verify_info->verify_info.next_available_write_4k_lba)
		verify_info->verify_info.next_available_write_4k_lba -= start_sect;

	return ret;
}

int mas_blk_get_bad_block_info(struct block_device *bi_bdev,
	struct stor_dev_bad_block_info *bad_block_info)
{
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_get_bad_block_info)
		return -EPERM;

	return lld->unistore_ops.dev_get_bad_block_info(q, bad_block_info);
}

int mas_blk_get_program_size(struct block_device *bi_bdev,
			struct stor_dev_program_size *program_size)
{
	int ret;
	struct request_queue *q = bdev_get_queue(bi_bdev);
	struct blk_dev_lld *lld = mas_blk_get_lld(q);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	if (!lld || !lld->unistore_ops.dev_get_program_size)
		return -EPERM;

	ret = lld->unistore_ops.dev_get_program_size(q, program_size);
	if (ret)
		pr_err("%s, ret=%d\n", __func__, ret);

	pr_err("%s, tlc_size=%x, slc_size=%x\n", __func__,
		program_size->tlc_program_size, program_size->slc_program_size);

	return ret;
}


static int mas_blk_set_streamid(struct hd_struct *p, struct blkdev_cmd *cmd)
{
	struct blkdev_set_stream stream;

	if (!cmd->cust_argp)
		return -EFAULT;

	if (copy_from_user(&stream, cmd->cust_argp, sizeof(struct blkdev_set_stream))) {
		pr_err("%s copy_from_user failed\n", __func__);
		return -EFAULT;
	}

	if (stream.stream_id >= BLK_STREAM_MAX_STRAM) {
		pr_err("%s, set wrong stream id:%u\n", __func__, stream.stream_id);
		return -EFAULT;
	}

	p->mas_hd.default_stream_id = stream.stream_id;
	return 0;
}

void mas_blk_fsync_barrier(struct block_device *bdev)
{
	unsigned long flags = 0;
	struct request_queue *q = NULL;
	struct blk_dev_lld *lld = NULL;

	if (unlikely(!bdev))
		return;

	q = bdev_get_queue(bdev);
	if (!blk_queue_query_unistore_enable(q))
		return;

	lld = mas_blk_get_lld(q);
	spin_lock_irqsave(&lld->fsync_ind_lock, flags);
	if (lld && !lld->fsync_ind)
		lld->fsync_ind = true;
	spin_unlock_irqrestore(&lld->fsync_ind_lock, flags);
}

struct stor_pwron_info_to_user {
	unsigned int dev_stream_addr[BLK_STREAM_MAX_STRAM];
	unsigned int dm_stream_addr[DATA_MOVE_STREAM_NUM];
	unsigned char io_slc_mode_status;
};

/*lint -e644*/
static int mas_blk_get_open_ptr(struct block_device *bdev, struct blkdev_cmd *cmd)
{
	int err;
	int i;
	struct stor_dev_pwron_info stor_info;
	struct stor_pwron_info_to_user stor_info_to_user;

	struct request_queue *q = bdev_get_queue(bdev);

	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	stor_info.rescue_seg = vmalloc(sizeof(unsigned int) * MAX_RESCUE_SEG_CNT);
	if (unlikely(!stor_info.rescue_seg)) {
		pr_err("%s: alloc mem failed\n", __func__);
		return -ENOMEM;
	}

	err = mas_blk_device_pwron_info_sync(bdev, &stor_info,
			sizeof(unsigned int) * MAX_RESCUE_SEG_CNT);
	if (unlikely(err)) {
		pr_err("%s, blk_device_pwron_info_sync failed, err = %d\n", __func__, err);
		vfree(stor_info.rescue_seg);
		return err;
	}
	vfree(stor_info.rescue_seg);

	memset(&stor_info_to_user, 0, sizeof(struct stor_pwron_info_to_user));
	for (i = 0; i < BLK_STREAM_MAX_STRAM; i++) {
		stor_info_to_user.dev_stream_addr[i] = stor_info.dev_stream_addr[i];
		pr_err("%s, dev_stream_addr[%d] = 0x%x\n",
				__func__, i, stor_info_to_user.dev_stream_addr[i]);
	}

	for (i = 0; i < DATA_MOVE_STREAM_NUM; i++) {
		stor_info_to_user.dm_stream_addr[i] = stor_info.dm_stream_addr[i];
		pr_err("%s, dm_stream_addr[%d] = 0x%x\n",
				__func__, i, stor_info_to_user.dm_stream_addr[i]);
	}

	stor_info_to_user.io_slc_mode_status = stor_info.io_slc_mode_status;
	pr_err("%s, io_slc_mode_status = 0x%x\n",
				__func__, stor_info_to_user.io_slc_mode_status);

	if (!cmd->cust_argp)
		return -EFAULT;

	err = copy_to_user(cmd->cust_argp, &stor_info_to_user,
				sizeof(struct stor_pwron_info_to_user));
	if (unlikely(err))
		pr_err("%s, copy_to_user failed, err = %d\n", __func__, err);

	return err;
}
/*lint -e644*/

static int mas_blk_get_segments_per_section(
	struct block_device *bdev, struct blkdev_cmd *cmd)
{
	int err;
	unsigned int section_size = 0;
	unsigned int segments_per_section;

	err = mas_blk_device_read_section(bdev, &section_size);
	if (err) {
		pr_err("%s, blk_device_read_section failed, err = %d\n",
							__func__, err);
		return err;
	}

	segments_per_section = section_size >> SECTOR_BYTE;

	if (!cmd->cust_argp)
		return -EFAULT;

	err = copy_to_user(cmd->cust_argp, &segments_per_section, sizeof(unsigned int));
	if (unlikely(err))
		pr_err("%s, copy_to_user failed, err = %d\n", __func__, err);

	return err;
}

static int mas_blk_get_mapping_pos(struct block_device *bdev,
	struct blkdev_cmd *cmd)
{
	int ret;
	struct stor_dev_mapping_partition mapping_info;

	memset(&mapping_info, 0, sizeof(struct stor_dev_mapping_partition));
	ret = mas_blk_device_read_mapping_partition(bdev, &mapping_info);
	if (ret) {
		pr_err("%s, blk_device_config_mapping_partition failed %d\n",
			__func__, ret);

		return ret;
	}

	if (!cmd->cust_argp)
		return -EFAULT;

	ret = copy_to_user(cmd->cust_argp, &mapping_info,
		sizeof(struct stor_dev_mapping_partition));
	if (unlikely(ret))
		pr_err("%s, copy_from_user failed %d\n", __func__, ret);

	return ret;
}

static int mas_blk_set_mapping_pos(struct block_device *bdev,
	struct blkdev_cmd *cmd)
{
	int ret;
	struct stor_dev_mapping_partition mapping_info;

	if (!cmd->cust_argp)
		return -EFAULT;

	ret = copy_from_user(&mapping_info, cmd->cust_argp,
		sizeof(struct stor_dev_mapping_partition));
	if (unlikely(ret)) {
		pr_err("%s, copy_from_user failed, err = %d\n",
			__func__, ret);

		return ret;
	}

	ret = mas_blk_device_config_mapping_partition(bdev, &mapping_info);
	if (ret)
		pr_err("%s, blk_device_config_mapping_partition failed %d\n",
			__func__, ret);

	return ret;
}

int mas_blk_close_section(struct block_device *bdev,
	struct blkdev_cmd *cmd)
{
	int ret;
	struct stor_dev_reset_ftl reset_ftl_info;

	if (!cmd->cust_argp)
		return -EFAULT;

	ret = copy_from_user(&reset_ftl_info, cmd->cust_argp,
		sizeof(struct stor_dev_reset_ftl));
	if (unlikely(ret)) {
		pr_err("%s, copy_from_user failed, err = %d\n",
			__func__, ret);

		return ret;
	}

	ret = mas_blk_device_close_section(bdev, &reset_ftl_info);
	if (ret)
		pr_err("%s, blk_device_reset_ftl failed, err = %d\n",
			__func__, ret);

	return ret;
}

static void mas_blk_add_section_list_tail(struct blk_dev_lld *lld,
	sector_t section_start_lba, unsigned char stream_type, int flash_mode)
{
	unsigned long flags = 0;
	struct unistore_section_info *section_info = NULL;
	struct unistore_section_info *tail_section_info = NULL;

	section_info = kmalloc(sizeof(struct unistore_section_info), GFP_NOIO);
	if (unlikely(!section_info)) {
		section_info = kmalloc(sizeof(struct unistore_section_info), GFP_NOIO | __GFP_NOFAIL);
		if (unlikely(!section_info)) {
			pr_err("%s, kmalloc fail\n", __func__);
			return;
		}
	}

	section_info->section_start_lba = section_start_lba;
	section_info->slc_mode = (flash_mode == 1) ? true : false;
	section_info->section_id = section_start_lba / lld->mas_sec_size;
	section_info->rcv_io_complete_flag = false;
	section_info->next_section_start_lba = 0;
	section_info->next_section_id = 0;
	section_info->section_insert_time = ktime_get();

	spin_lock_irqsave(&lld->expected_lba_lock[stream_type - 1], flags);
	if (list_empty_careful(&lld->section_list[stream_type - 1])) {
		mas_blk_update_expected_info(lld, section_start_lba, stream_type);
	} else {
		tail_section_info = list_last_entry(
			&lld->section_list[stream_type - 1],
			struct unistore_section_info, section_list);
		if (tail_section_info->section_id == section_info->section_id) {
			pr_err("%s section add the same 0x%llx\n", __func__, section_info->section_id);
			kfree(section_info);
			goto out;
		}
		tail_section_info->next_section_start_lba = section_info->section_start_lba;
		tail_section_info->next_section_id = section_info->section_id;
	}

	list_add_tail(&section_info->section_list, &lld->section_list[stream_type - 1]);
out:
	spin_unlock_irqrestore(&lld->expected_lba_lock[stream_type - 1], flags);
}

void mas_blk_insert_section_list(struct block_device *bdev,
	unsigned int start_blkaddr, int stream_type, int flash_mode)
{
	sector_t section_start_lba;
	struct hd_struct *p = NULL;
	struct request_queue *q = NULL;
	struct blk_dev_lld *lld = NULL;

	if (!bdev)
		return;

	if (!mas_blk_is_order_stream(stream_type))
		return;

	q = bdev_get_queue(bdev);
	if (!blk_queue_query_unistore_enable(q))
		return;

	lld = mas_blk_get_lld(q);
	if (!lld) {
		pr_err("%s - lld is null\n", __func__);
		return;
	}

	if (!lld->mas_sec_size || !lld->mas_pu_size) {
		pr_err("%s - err, section size: %u, pu size: %u\n",
			__func__, lld->mas_sec_size, lld->mas_pu_size);
		return;
	}

	rcu_read_lock();
	p = __disk_get_part(bdev->bd_disk, bdev->bd_partno);
	if (!p) {
		pr_err("%s - disk get part failed\n", __func__);
		rcu_read_unlock();
		return;
	}
	section_start_lba = start_blkaddr + (p->start_sect >> 3);
	rcu_read_unlock();

	if (section_start_lba % (lld->mas_sec_size)) {
		pr_err("%s - stream_type %d section_start_lba is err\n", __func__, stream_type);
		return;
	}

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
	if (mas_blk_unistore_debug_en())
		pr_err("%s, start_blkaddr = %u,"
			"section_start_lba = 0x%llx - 0x%llx, stream_type = %d\n",
			__func__, start_blkaddr, section_start_lba / (lld->mas_sec_size),
			section_start_lba % (lld->mas_sec_size), stream_type);
#endif

	mas_blk_add_section_list_tail(lld, section_start_lba, stream_type, flash_mode);
}

#define MAX_4K_SECTION_CAP 0x708000
static int mas_blk_set_max_meta_mapping_pos(struct block_device *bdev)
{
	int ret;
	int i;
	struct stor_dev_mapping_partition mapping_info;
	unsigned int sec_num = 0;
	unsigned int sec_size = 0;
	unsigned int total_size = 0;

	struct request_queue *q = bdev_get_queue(bdev);
	if (!blk_queue_query_unistore_enable(q))
		return -EFAULT;

	ret = mas_blk_device_read_section(bdev, &sec_size);
	if (unlikely(ret)) {
		pr_err("%s, read sec size failed %d\n", __func__, ret);
		return ret;
	}

	ret = mas_blk_device_read_mapping_partition(bdev, &mapping_info);
	if (unlikely(ret)) {
		pr_err("%s, read_mapping_partition failed %d\n", __func__, ret);
		return ret;
	}

	if (!sec_size) {
		pr_err("%s, get sec_size err\n", __func__);
		return -EFAULT;
	}

	for (i = 0; i < PARTITION_TYPE_MAX; i++)
		total_size += mapping_info.partion_size[i];

	sec_num = MAX_4K_SECTION_CAP / sec_size;
	if ((total_size <= sec_num) || !sec_num) {
		pr_err("%s, get sec_num err total_size %u sec_num %u\n",
			__func__, total_size, sec_num);
		return -EFAULT;
	}
	pr_err("%s sec_num %u\n", __func__, sec_num);

	/* first partition */
	mapping_info.partion_start[PARTITION_TYPE_META0] = 0;
	mapping_info.partion_size[PARTITION_TYPE_META0] = sec_num;

	mapping_info.partion_start[PARTITION_TYPE_USER0] = sec_num;
	mapping_info.partion_size[PARTITION_TYPE_USER0] = total_size - sec_num;

	/* second partition */
	mapping_info.partion_start[PARTITION_TYPE_META1] = 0;
	mapping_info.partion_size[PARTITION_TYPE_META1] = 0;

	mapping_info.partion_start[PARTITION_TYPE_USER1] = 0;
	mapping_info.partion_size[PARTITION_TYPE_USER1] = 0;

	ret = mas_blk_device_config_mapping_partition(bdev, &mapping_info);
	if (ret)
		pr_err("%s, config mapping partition failed %d\n", __func__, ret);

	return ret;
}

int mas_blk_read_op_size(struct block_device *bdev,
	struct blkdev_cmd *cmd)
{
	int err;
	int op_size = 0;

	err = mas_blk_device_read_op_size(bdev, &op_size);
	if (unlikely(err))
		return err;

	if (!cmd->cust_argp)
		return -EFAULT;

	err = copy_to_user(cmd->cust_argp, &op_size, sizeof(int));
	if (unlikely(err))
		pr_err("%s, copy_to_user failed, err = %d\n", __func__, err);

	return err;
}

static int mas_blk_add_section(struct block_device *bdev,
	struct blkdev_cmd *cmd)
{
	struct stor_dev_section_info section_info;

	if (!cmd->cust_argp)
		return -EFAULT;

	if (copy_from_user(&section_info, cmd->cust_argp, sizeof(struct stor_dev_section_info))) {
		pr_err("%s copy_from_user failed\n", __func__);
		return -EFAULT;
	}

	mas_blk_insert_section_list(bdev, section_info.section_start_lba,
		section_info.stream_type, section_info.flash_mode);

	return 0;
}

void mas_blk_clear_section_list(struct block_device *bdev,
	unsigned char stream_type)
{
	struct request_queue *q = NULL;
	struct blk_dev_lld *lld = NULL;

	if (!bdev)
		return;

	if (!mas_blk_is_order_stream(stream_type))
		return;

	q = bdev_get_queue(bdev);
	if (!blk_queue_query_unistore_enable(q))
		return;

	lld = mas_blk_get_lld(q);
	if (!lld) {
		pr_err("%s lld is null\n", __func__);
		return;
	}

	mas_blk_clear_section_info(lld, stream_type);
}

int mas_blk_clear_section(struct block_device *bdev,
	struct blkdev_cmd *cmd)
{
	struct blkdev_set_stream stream;

	if (!cmd->cust_argp)
		return -EFAULT;

	if (copy_from_user(&stream, cmd->cust_argp, sizeof(struct blkdev_set_stream))) {
		pr_err("%s copy_from_user failed\n", __func__);
		return -EFAULT;
	}

	mas_blk_clear_section_list(bdev, stream.stream_id);

	return 0;
}
#endif /* CONFIG_MAS_UNISTORE_PRESERVE */

static int mas_blk_get_device_unistore_enabled(
	struct block_device *bdev, struct blkdev_cmd *cmd)
{
	int err;
	uint8_t enabled = 0;
	struct request_queue *q = bdev_get_queue(bdev);

	if (blk_queue_query_unistore_enable(q))
		enabled = 1;

	if (!cmd->cust_argp)
		return -EFAULT;

	err = copy_to_user(cmd->cust_argp, &enabled, sizeof(uint8_t));
	if (unlikely(err))
		pr_err("%s, copy_to_user failed, err = %d\n", __func__, err);

	return err;
}

int mas_blk_cust_ioctl(struct block_device *bdev, struct blkdev_cmd __user *arg)
{
	struct blkdev_cmd cmd;

	if (!arg)
		return -EFAULT;

	if (copy_from_user(&cmd, arg, sizeof(struct blkdev_cmd))) {
		pr_err("%s copy_from_user failed\n", __func__);
		return -EFAULT;
	}

	switch (cmd.cmd) {
#ifdef CONFIG_MAS_UNISTORE_PRESERVE
	case CUST_BLKDEV_SET_STREAM_ID:
		return mas_blk_set_streamid(bdev->bd_part, &cmd);
	case CUST_BLKDEV_GET_OPEN_PTR:
		return mas_blk_get_open_ptr(bdev, &cmd);
	case CUST_BLKDEV_GET_SEGS_PER_SEC:
		return mas_blk_get_segments_per_section(bdev, &cmd);
	case CUST_BLKDEV_RESET_FTL:
		return mas_blk_device_reset_ftl(bdev);
	case CUST_BLKDEV_SET_MAPPING_POS:
		return mas_blk_set_mapping_pos(bdev, &cmd);
	case CUST_BLKDEV_GET_OP_SIZE:
		return mas_blk_read_op_size(bdev, &cmd);
	case CUST_BLKDEV_GET_MAPPING_POS:
		return mas_blk_get_mapping_pos(bdev, &cmd);
	case CUST_BLKDEV_CLOSE_SECTION:
		return mas_blk_close_section(bdev, &cmd);
	case CUST_BLKDEV_FS_SYNC_DONE:
		return mas_blk_fs_sync_done(bdev);
	case CUST_BLKDEV_SET_MAX_META_MAPPING_POS:
		return mas_blk_set_max_meta_mapping_pos(bdev);
	case CUST_BLKDEV_ADD_SECTION:
		return mas_blk_add_section(bdev, &cmd);
	case CUST_BLKDEV_CLEAR_SECTION:
		return mas_blk_clear_section(bdev, &cmd);
#endif
	case CUST_BLKDEV_GET_UNISTORE_ENABLE:
		return mas_blk_get_device_unistore_enabled(bdev, &cmd);
	default:
		break;
	}
	return 0;
}
