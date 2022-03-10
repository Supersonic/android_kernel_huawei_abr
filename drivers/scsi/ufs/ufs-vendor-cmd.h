/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * Description: ufs command header file
 * Author: zhangjinkai
 * Create: 2021-3-18
 */

#ifndef LINUX_UFS_VENOR_CMD_H
#define LINUX_UFS_VENOR_CMD_H

#include "ufshcd.h"
#include "ufs-qcom.h"

#define UPIU_QUERY_OPCODE_READ_HI1861_FSR 0xF3

struct ufs_query_vcmd {
	enum query_opcode opcode;
	u8 idn;
	u8 index;
	u8 selector;
	u8 query_func;
	u8 lun;
	bool has_data;
	__be32 value;
	__be16 reserved_osf;
	__be32 reserved[2];

	u8 *desc_buf;
	int buf_len;
	struct ufs_query_res *response;
};

enum ufs_query_vendor_write_idn {
	VENDOR_IDN_PGR   = 0x07,
	VENDOR_IDN_TT = 0x08,
};
enum ufs_query_vendor_seg_length {
	VENDOR_PGR_MAX_LENGTH  = 0x108,
	VENDOR_TT_MAX_LENGTH = 0x04,
};

#define QUERY_DESC_TTUNIT_MAX_SIZE 0x04

/* UFSHCD states */
enum {
	UFSHCD_STATE_RESET,
	UFSHCD_STATE_ERROR,
	UFSHCD_STATE_OPERATIONAL,
	UFSHCD_STATE_EH_SCHEDULED_FATAL,
	UFSHCD_STATE_EH_SCHEDULED_NON_FATAL,
};

#define SWAP_ENDIAN_32(x)  ((((x) & 0xFF) << 24) | (((x) & 0xFF00) << 8) \
                            | (((x) >> 8) & 0xFF00) | (((x) >> 24) & 0xFF))

#if defined(CONFIG_HUAWEI_UFS_VENDOR_MODE) || defined(CONFIG_SCSI_MAS_UFS_MQ_DEFAULT)
#ifdef CONFIG_QCOM_SCSI_UFS_DUMP
void ufshcd_dump_scsi_command(struct ufs_hba *hba, unsigned int task_tag);
#endif
int ufshcd_query_vcmd_single(struct ufs_hba *hba, struct ufs_query_vcmd *cmd);
int ufshcd_query_vcmd_retry(struct ufs_hba *hba, struct ufs_query_vcmd *cmd);
int wait_for_ufs_all_complete(struct ufs_hba *hba, int timeout_ms);
void ufshcd_compose_scsi_cmd(struct scsi_cmnd *cmd,
				struct scsi_device *device,
				unsigned char *cdb,
				unsigned char *sense_buffer,
				enum dma_data_direction sc_data_direction,
				struct scatterlist *sglist,
				unsigned int nseg,
				unsigned int sg_len);
int ufshcd_send_vendor_scsi_cmd(struct ufs_hba *hba,
					struct scsi_device *sdp, unsigned char* cdb, void* buf);
int ufshcd_compose_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp);
#else
static inline  __attribute__((unused)) int
ufshcd_query_vcmd_single(struct ufs_hba *hba, struct ufs_query_vcmd *cmd
				__attribute__((unused)))
	return 0;
}
static inline  __attribute__((unused)) int
ufshcd_query_vcmd_retry(struct ufs_hba *hba, struct ufs_query_vcmd *cmd
				__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) int
wait_for_ufs_all_complete(struct ufs_hba *hba, int timeout_ms
				__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) void
ufshcd_compose_scsi_cmd(struct scsi_cmnd *cmd,
				struct scsi_device *device,
				unsigned char *cdb,
				unsigned char *sense_buffer,
				enum dma_data_direction sc_data_direction,
				struct scatterlist *sglist,
				unsigned int nseg,
				unsigned int sg_len
				__attribute__((unused)))
{
	return;
}
static inline  __attribute__((unused)) int
ufshcd_send_vendor_scsi_cmd(struct ufs_hba *hba,
					struct scsi_device *sdp, unsigned char* cdb, void* buf
					__attribute__((unused)))
{
	return 0;
}
static inline  __attribute__((unused)) int
ufshcd_compose_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp
				__attribute__((unused)))
{
	return 0;
}
#endif

static inline  __attribute__((unused)) void
ufshcd_dump_scsi_command(struct ufs_hba *hba, unsigned int task_tag
				__attribute__((unused)))
{
	return;
}

void ufshcd_set_query_req_length(u8 opcode, u8 idn, u32 *data_seg_length);
void ufshcd_set_query_req_desc(u8 opcode, u8 idn,
				struct utp_upiu_req *ptr, u8 *descriptor);
int ufshcd_ioctl_query_vcmd(struct ufs_hba *hba,
			    struct ufs_ioctl_query_data *ioctl_data,
			    void __user *buffer);

#if defined(CONFIG_SCSI_UFS_HI1861_VCMD) && defined(CONFIG_PLATFORM_DIEID)
void hufs_bootdevice_get_dieid(struct ufs_hba *hba);
#else
static inline  __attribute__((unused)) void
hufs_bootdevice_get_dieid(struct ufs_hba *hba
				__attribute__((unused)))
{
	return;
}
#endif

#endif
