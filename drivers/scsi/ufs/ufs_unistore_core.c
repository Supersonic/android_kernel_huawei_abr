/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: unistore implement
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

#include "ufs_unistore_internal.h"
#include "ufs_unistore_read.h"
#include "ufs_unistore_write.h"
#include "ufs_vcmd_proc.h"

#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>
#include <asm/unaligned.h>

#include "ufshcd.h"
#include "ufs_func.h"

#define VENDOR_FEATURE_UNISTORE BIT(16)
static bool ufshcd_unistore_is_support(struct ufs_hba *hba, uint32_t feature)
{
	if (hba->manufacturer_id != UFS_VENDOR_HI1861)
		return false;

	dev_info(hba->dev, "%s %x\n", __func__, feature);

	return feature & VENDOR_FEATURE_UNISTORE;
}

static void ufshcd_unistore_enable_init(struct ufs_hba *hba, uint32_t feature)
{
	hba->host->unistore_enable = ufshcd_unistore_is_support(hba, feature);
}

static void ufshcd_vcmd_set_init(struct ufs_hba *hba, uint32_t feature)
{
	hba->host->vcmd_set = 0;
	if (hba->manufacturer_id != UFS_VENDOR_HI1861)
		return;

	/* bit14 ~ bit15 stand vcmd set */
	hba->host->vcmd_set = (feature >> 14) & 0x3;
	dev_info(hba->dev, "%s 0x%x \n", __func__, hba->host->vcmd_set);
}

void ufshcd_vendor_feature_unistore_init(struct ufs_hba *hba, struct ufs_dev_info *dev_info)
{
	ufshcd_unistore_enable_init(hba, dev_info->vendor_feature);
	ufshcd_vcmd_set_init(hba, dev_info->vendor_feature);
}

static int ufshcd_unistore_bad_block_notify_register(
	struct scsi_device *sdev,
	void (*func)(struct Scsi_Host *host,
		struct stor_dev_bad_block_info *bad_block_info))
{
	struct scsi_host_template *hostt = NULL;
	struct Scsi_Host *host = NULL;

	if (!sdev || !func)
		return -EINVAL;

	host = sdev->host;
	if (!host)
		return -EINVAL;

	hostt = host->hostt;
	if (!hostt)
		return -EINVAL;

	hostt->dev_bad_block_notify = func;

	return 0;
}

void ufshcd_unistore_op_register(struct ufs_hba *hba)
{
	struct scsi_host_template *hostt = NULL;

	if (!hba || !hba->host)
		return;

	hostt = hba->host->hostt;
	if (!hostt)
		return;

	hostt->dev_pwron_info_sync = ufshcd_dev_pwron_info_sync;
	hostt->dev_stream_oob_info_fetch = ufshcd_stream_oob_info_fetch;
	hostt->dev_reset_ftl = ufshcd_dev_reset_ftl;
	hostt->dev_read_section = ufshcd_dev_read_section_size;
	hostt->dev_read_lrb_in_use = ufshcd_dev_read_lrb_in_use;
	hostt->dev_read_op_size = ufshcd_dev_read_op_size;
	hostt->dev_config_mapping_partition =
		ufshcd_dev_config_mapping_partition;
	hostt->dev_read_mapping_partition =
		ufshcd_dev_read_mapping_partition;
	hostt->dev_fs_sync_done = ufshcd_dev_fs_sync_done;
	hostt->dev_data_move = ufshcd_dev_data_move;
	hostt->dev_slc_mode_configuration = ufshcd_dev_slc_mode_configuration;
	hostt->dev_sync_read_verify = ufshcd_dev_sync_read_verify;
	hostt->dev_get_bad_block_info = ufshcd_dev_get_bad_block_info;
	hostt->dev_get_program_size = ufshcd_dev_get_program_size;
	hostt->dev_bad_block_notify_register =
		ufshcd_unistore_bad_block_notify_register;
#ifdef CONFIG_MAS_DEBUG_FS
	hostt->dev_rescue_block_inject_data =
		ufshcd_dev_rescue_block_inject_data;
	hostt->dev_bad_block_error_inject = ufshcd_dev_bad_block_error_inject;
#endif
}

static void ufshcd_enable_bad_block_occur(struct ufs_hba *hba)
{
	int err;

	err = ufshcd_enable_ee(hba, MASK_EE_BAD_BLOCK_OCCUR);
	if (err)
		dev_err(hba->dev, "%s: failed to enable exception event %d\n",
			__func__, err);

	return;
}

#define UNISTORE_MQ_QUEUE_DEPTH 32
static int ufshcd_unistore_update(struct Scsi_Host *host)
{
	int ret = 0;

	if ((host->mq_reserved_queue_depth != UNISTORE_MQ_QUEUE_DEPTH) ||
		(host->mq_high_prio_queue_depth != UNISTORE_MQ_QUEUE_DEPTH)) {
		host->mq_reserved_queue_depth = UNISTORE_MQ_QUEUE_DEPTH;
		host->mq_high_prio_queue_depth = UNISTORE_MQ_QUEUE_DEPTH;
		host->tag_set.reserved_tags = host->mq_reserved_queue_depth;
		host->tag_set.high_prio_tags = host->mq_high_prio_queue_depth;

		ret = mas_blk_mq_update_unistore_tags(&host->tag_set);
	}

	return ret;
}

static void ufshcd_unistore_set_sec_size(struct ufs_hba *hba)
{
	int ret;
	hba->host->mas_sec_size = 0x9000; /* 144M */

	ret = ufshcd_dev_read_section_size_hba(hba, &(hba->host->mas_sec_size));
	if (ret)
		dev_err(hba->dev, "%s: read sec size ret err %d\n", __func__, ret);

#if defined(CONFIG_DFX_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
	if (!hba->host->mas_sec_size)
		dev_err(hba->dev, "%s: read section size err %u\n",
			__func__, hba->host->mas_sec_size);
#endif
}

static int ufshcd_unistore_set_pu_size(struct ufs_hba *hba)
{
	int ret;

	ret = ufshcd_dev_read_pu_size_hba(hba, &(hba->host->mas_pu_size));
	if (ret || !hba->host->mas_pu_size) {
		dev_err(hba->dev, "%s: read po size ret %d, mas_po_size %u\n",
			__func__, ret, hba->host->mas_pu_size);
		return -EIO;
	}

	return 0;
}

int ufshcd_unistore_init(struct ufs_hba *hba)
{
	int ret;
	struct Scsi_Host *host = hba->host;
	static bool init_flag = false;

	if (!host)
		return -EINVAL;

	if (!host->unistore_enable)
		return 0;

	ret = ufshcd_unistore_update(host);
	if (ret) {
		dev_err(hba->dev, "%s: ufshcd_unistore_update err %d\n",
			__func__, ret);

		return ret;
	}

	ufshcd_enable_bad_block_occur(hba);

	if (!init_flag) {
		ret = ufshcd_unistore_set_pu_size(hba);
		if (ret)
			return ret;

		ufshcd_unistore_set_sec_size(hba);

		ret = ufshcd_dev_data_move_ctl_init();
		if (ret) {
			dev_err(hba->dev, "%s: ufshcd_dev_data_move_init err %d\n",
				__func__, ret);

			return ret;
		}

		init_flag = true;
	}

	return 0;
}

void ufshcd_unistore_done(struct ufs_hba *hba, struct scsi_cmnd *cmd,
	struct ufshcd_lrb *lrbp)
{
	if (!hba || !hba->host || !hba->host->unistore_enable)
		return;

	ufshcd_dev_data_move_done(cmd, lrbp);
}

static bool ufshcd_unistore_is_wlun(u64 lun)
{
	return (lun == UFS_UPIU_REPORT_LUNS_WLUN) ||
		(lun == UFS_UPIU_UFS_DEVICE_WLUN) ||
		(lun == UFS_UPIU_BOOT_WLUN) ||
		(lun == UFS_UPIU_RPMB_WLUN);
}

void ufshcd_unistore_scsi_device_init(struct scsi_device *sdev)
{
	if (!ufshcd_unistore_is_wlun(sdev->lun))
		ufshcd_set_read_buffer_device(sdev);
}

#define CUSTOM_UPIU_BUILD_STREAM_TYPE_MASK	0x1F
#define CUSTOM_UPIU_BUILD_SLC_MODE_OFFSET	4

static void ufshcd_unistore_set_dfx_time(struct utp_upiu_req *ucd_req_ptr)
{
	u32 curr_time;

	if (ucd_req_ptr->sc.cdb[0] == READ_10) {
		curr_time = (u32)ktime_to_ms(ktime_get());

		/* CMD UPIU Byte 5,6,7,9 for time */
		ucd_req_ptr->header.dword_1 |= UPIU_HEADER_DWORD(0, (u8)curr_time,
			(u8)(curr_time >> 8), (u8)(curr_time >> 16));
		ucd_req_ptr->header.dword_2 |=
			UPIU_HEADER_DWORD(0, (u8)(curr_time >> 24), 0, 0);
	}
}

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
static void ufshcd_unistore_stub_disorder(
	struct utp_upiu_req *ucd_req_ptr, struct request *req)
{
	sector_t new_lba;

	if ((mas_blk_recovery_debug_on() == RESET_DEBUG_DISORDER) &&
		req->mas_req.stream_type && req->q) {
		new_lba = mas_blk_get_stub_disorder_lba(req);
		if (!new_lba)
			return;

		pr_err("%s, reset_debug_disorder, make_nr:%u, stream:%u\n", __func__,
			req->mas_req.make_req_nr, req->mas_req.stream_type);

		/* stub WRITE_10 lba, byte 2 ~ byte 5 */
		ucd_req_ptr->sc.cdb[2] = (u8)((new_lba >> 24) & 0xFF);
		ucd_req_ptr->sc.cdb[3] = (u8)((new_lba >> 16) & 0xFF);
		ucd_req_ptr->sc.cdb[4] = (u8)((new_lba >> 8) & 0xFF);
		ucd_req_ptr->sc.cdb[5] = (u8)(new_lba & 0xFF);

		mas_blk_recovery_debug_off();
	}
}
#endif

int ufshcd_custom_upiu_unistore(
	struct utp_upiu_req *ucd_req_ptr, struct request *req,
	struct scsi_cmnd *scmd, struct ufs_hba *hba)
{
	unsigned char wo_nr = 0;
	unsigned char pre_order_cnt = 0;
	unsigned char stream_type;

	if (unlikely(!req))
		return 0;

	if (ucd_req_ptr->sc.cdb[0] == WRITE_10) {
		/* Add inode number */
		ucd_req_ptr->sc.cdb[11] =
			(unsigned char)(req->mas_req.data_ino);
		ucd_req_ptr->sc.cdb[10] =
			(unsigned char)(req->mas_req.data_ino >> 8);
		ucd_req_ptr->header.dword_1 &= 0xFF0000FF;
		ucd_req_ptr->header.dword_1 |=
			__cpu_to_be32(((req->mas_req.data_ino >> 8) &
				0x00FFFF00));

		/* Add file offset */
		ucd_req_ptr->sc.cdb[15] =
			(unsigned char)(req->mas_req.data_idx);
		ucd_req_ptr->sc.cdb[14] =
			(unsigned char)(req->mas_req.data_idx >> 8);
		ucd_req_ptr->sc.cdb[13] =
			(unsigned char)(req->mas_req.data_idx >> 16);
		ucd_req_ptr->sc.cdb[12] =
			(unsigned char)(req->mas_req.data_idx >> 24);

		/* Add stream type */
		ucd_req_ptr->sc.cdb[6] = req->mas_req.stream_type;
		if (ucd_req_ptr->sc.cdb[6])
			ucd_req_ptr->sc.cdb[6] = (req->mas_req.stream_type |
			((unsigned char)req->mas_req.slc_mode <<
			CUSTOM_UPIU_BUILD_SLC_MODE_OFFSET)) &
			CUSTOM_UPIU_BUILD_STREAM_TYPE_MASK;

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
		ufshcd_unistore_stub_disorder(ucd_req_ptr, req);
#endif
		/* Add CP Tag flag 0x20 */
		if (req->mas_req.cp_tag)
			ucd_req_ptr->sc.cdb[6] |= 0x20;
	}

	ufshcd_unistore_set_dfx_time(ucd_req_ptr);

	if (scsi_is_order_cmd(scmd)) {
		stream_type = (scmd->cmnd[0] == WRITE_10) ?
			req->mas_req.stream_type : STREAM_TYPE_RPMB;
		mas_blk_req_get_order_nr_unistore(req, stream_type, &wo_nr,
			&pre_order_cnt, true);

		/* CMD UPIU Byte 7 for Command Order */
		ucd_req_ptr->header.dword_1 |=
			UPIU_HEADER_DWORD(0, 0, 0, wo_nr);

		/* CMD UPIU Byte 9 for Pre Command Order Cnt */
		ucd_req_ptr->header.dword_2 |=
			UPIU_HEADER_DWORD(0, pre_order_cnt, 0, 0);
	}
	return 0;
}
