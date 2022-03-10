/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * Description: ufs command function
 * Author: zhangjinkai
 * Create: 2021-3-18
 */
#include <linux/version.h>
#include "ufs-vendor-cmd.h"
#include "ufs-vendor-mode.h"
#include "ufs-vendor-extern.h"
#include "ufs-qcom.h"
#include "ufshcd.h"

#define VCMD_REQ_RETRIES 5
/* vcmd request timeout */
#define VCMD_REQ_TIMEOUT 3000 /* 3 seconds */
#define VCMD_RW_BUFFER_SET 0x1
#define HI1861_FSR_INFO_SIZE 4096

#if defined(CONFIG_SCSI_UFS_HI1861_VCMD) && defined(CONFIG_PLATFORM_DIEID)
static u8 *ufs_hixxxx_dieid;
static int is_fsr_read_failed;
#define UFS_CONTROLLER_DIEID_SIZE 32
#define UFS_FLASH_DIE_ID_SIZE 128
#define UFS_DIEID_NUM_SIZE 4
#define UFS_DIEID_NUM_SIZE_THOR920 1
#define UFS_DCIP_CRACK_EVER_SIZE 1
#define UFS_DCIP_CRACK_NOW_SIZE 1
#define UFS_NAND_CHIP_VER_SIZE 8
#define UFS_DIEID_TOTAL_SIZE 175
#define UFS_DIEID_BUFFER_SIZE 800
#define UFS_DIEID_CHIP_VER_OFFSET 4
#define UFS_DIEID_CONTROLLER_OFFSET 12
#define UFS_DIEID_FLASH_OFFSET 44
#define UFS_DCIP_CRACK_NOW_OFFSET 173
#define UFS_DCIP_CRACK_EVER_OFFSET 174
#define UFS_FLASH_VENDOR_T 0x98
#define UFS_FLASH_VENDOR_M 0x2c
#define UFS_FLASH_VENDOR_H 0xad
#define UFS_FLASH_VENDOR_Y 0x9b
#define UFS_FLASH_TWO_DIE 2
#define UFS_FLASH_THREE_DIE 3
#define UFS_FLASH_FOUR_DIE 4
#define HI1861_FSR_INFO_SIZE 4096
#define UFS_HIXXXX_PRODUCT_NAME                 "SS6100GBCV100"
#define HIXXXX_PROD_LEN                         (13)
#define UFS_PRODUCT_NAME_THOR920                "THR920"
#define UFS_PRODUCT_NAME_THOR925                "THR925"
#define UFS_PRODUCT_NAME_SS6100                 "SS6100"
#define UFS_PRODUCT_NAME_LEN                    (6)
char ufs_product_name[UFS_PRODUCT_NAME_LEN + 1];
#endif

#ifdef CONFIG_QCOM_SCSI_UFS_DUMP
void ufshcd_dump_scsi_command(struct ufs_hba *hba, unsigned int task_tag)
{
	if (unlikely(ufs_dump)) {
		switch ((int)hba->lrb[task_tag].cmd->cmnd[0]) {
		case 0x28:
			printk(KERN_DEBUG "ufs read10 addr:%d, length:%d\n",
				(int)hba->lrb[task_tag].cmd->cmnd[2] * 0x1000000 +
				(int)hba->lrb[task_tag].cmd->cmnd[3] * 0x10000 +
				(int)hba->lrb[task_tag].cmd->cmnd[4] * 0x100 +
				(int)hba->lrb[task_tag].cmd->cmnd[5] * 0x1,
				(int)hba->lrb[task_tag].cmd->cmnd[7] * 0x100 +
				(int)hba->lrb[task_tag].cmd->cmnd[8] * 0x1);
			break;
		case 0x2A:
			printk(KERN_DEBUG "ufs write10 addr:%d, length:%d\n",
				(int)hba->lrb[task_tag].cmd->cmnd[2] * 0x1000000 +
				(int)hba->lrb[task_tag].cmd->cmnd[3] * 0x10000 +
				(int)hba->lrb[task_tag].cmd->cmnd[4] * 0x100 +
				(int)hba->lrb[task_tag].cmd->cmnd[5] * 0x1,
				(int)hba->lrb[task_tag].cmd->cmnd[7] * 0x100 +
				(int)hba->lrb[task_tag].cmd->cmnd[8] * 0x1);
			break;
		case 0x35:
			printk(KERN_DEBUG "ufs sync10 addr:%d, block num:%d\n",
				(int)hba->lrb[task_tag].cmd->cmnd[2] * 0x1000000 +
				(int)hba->lrb[task_tag].cmd->cmnd[3] * 0x10000 +
				(int)hba->lrb[task_tag].cmd->cmnd[4] * 0x100 +
				(int)hba->lrb[task_tag].cmd->cmnd[5] * 0x1,
				(int)hba->lrb[task_tag].cmd->cmnd[7] * 0x100 +
				(int)hba->lrb[task_tag].cmd->cmnd[8] * 0x1);
			break;
		case 0x42:
			printk(KERN_DEBUG "ufs unmap(discard)\n");
			break;
		default:
			printk(KERN_DEBUG "ufs cmd (0x%x)\n",
				hba->lrb[task_tag].cmd->cmnd[0]);
			break;
		}
	}
}
#endif

#ifdef CONFIG_HUAWEI_UFS_VENDOR_MODE
int wait_for_ufs_all_complete(struct ufs_hba *hba, int timeout_ms)
{
	if (hba == NULL)
		return -EINVAL;
	while (timeout_ms-- > 0) {
		if (!hba->outstanding_reqs)
			return 0;
		/* wait 1 ms */
		udelay(1000);
	}
	dev_err(hba->dev, "%s: outstanding req : 0x%lx\n", __func__,
			hba->outstanding_reqs);
	return -ETIMEDOUT;
}

/**
 * ufshcd_compose_upiu - form UFS Protocol Information Unit(UPIU)
 * @hba - per adapter instance
 * @lrb - pointer to local reference block
 */
int ufshcd_compose_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	u32 upiu_flags = 0;
	int ret = 0;

	switch (lrbp->command_type) {
	case UTP_CMD_TYPE_SCSI:
		if (likely(lrbp->cmd)) {
			ufshcd_prepare_req_desc_hdr(lrbp, &upiu_flags,
					lrbp->cmd->sc_data_direction);
			ufshcd_prepare_utp_scsi_cmd_upiu(lrbp, upiu_flags);
		} else {
			ret = -EINVAL;
		}
		break;
	case UTP_CMD_TYPE_DEV_MANAGE:
		ufshcd_prepare_req_desc_hdr(lrbp, &upiu_flags, DMA_NONE);
		if (hba->dev_cmd.type == DEV_CMD_TYPE_QUERY)
			ufshcd_prepare_utp_query_req_upiu(
					hba, lrbp, upiu_flags);
		else if (hba->dev_cmd.type == DEV_CMD_TYPE_NOP)
			ufshcd_prepare_utp_nop_upiu(lrbp);
		else
			ret = -EINVAL;
		break;
	case UTP_CMD_TYPE_UFS:
		/* For UFS native command implementation */
		ret = -ENOTSUPP;
		dev_err(hba->dev, "%s: UFS native command are not supported\n",
			__func__);
		break;
	default:
		ret = -ENOTSUPP;
		dev_err(hba->dev, "%s: unknown command type: 0x%x\n",
				__func__, lrbp->command_type);
		break;
	} /* end of switch */

	return ret;
}
EXPORT_SYMBOL(ufshcd_compose_upiu);

/**
 * ufshcd_compose_scsi_cmd - filling scsi_cmnd with already info
 * @cmd: command from SCSI Midlayer
 * @done: call back function
 *
 * Until now, we do not filling all the infomation for scsi_cmnd, we just use
 * this for direct send cmd to ufs driver, skip block layer and scsi layer
 *
 * Returns 0 for success, non-zero in case of failure
 */
void ufshcd_compose_scsi_cmd(struct scsi_cmnd *cmd,
				    struct scsi_device *device,
				    unsigned char *cdb,
				    unsigned char *sense_buffer,
				    enum dma_data_direction sc_data_direction,
				    struct scatterlist *sglist,
				    unsigned int nseg,
				    unsigned int sg_len)
{
	cmd->device = device;
	cmd->cmnd = cdb;
	cmd->cmd_len = COMMAND_SIZE(cdb[0]);
#ifdef CONFIG_HUAWEI_UFS_VENDOR_MODE
	if(((cdb[0]>>5)&7) >= UFS_IOCTL_VENDOR_CMD)
		cmd->cmd_len = UFS_IOCTL_VENDOR_CDB_LEN;
#endif
	cmd->sense_buffer = sense_buffer;
	cmd->sc_data_direction = sc_data_direction;

	cmd->sdb.table.sgl = sglist;
	cmd->sdb.table.nents = nseg;
	cmd->sdb.length = sg_len;
}

int ufshcd_send_vendor_scsi_cmd(struct ufs_hba *hba,
		struct scsi_device *sdp, unsigned char* cdb, void* buf)
{
	int ret;
	unsigned char *dma_buf;
	struct scatterlist sglist;
	struct scsi_cmnd cmnd;

	dma_buf = kzalloc((size_t)PAGE_SIZE, GFP_ATOMIC);
	if (!dma_buf) {
		ret = -ENOMEM;
		goto out;
	}
	sg_init_one(&sglist, dma_buf, (unsigned int)PAGE_SIZE);

	memset(&cmnd, 0, sizeof(cmnd));

	ufshcd_compose_scsi_cmd(&cmnd, sdp, cdb, NULL, DMA_FROM_DEVICE,
				&sglist, 1, (unsigned int)PAGE_SIZE);

	ret = ufshcd_queuecommand_directly(hba, &cmnd, (unsigned int)1000, false);
	if(buf)
		memcpy(buf, dma_buf, PAGE_SIZE);
	if (ret)
		pr_err("%s: failed with err %d\n", __func__, ret);

	kfree(dma_buf);
out:
	return ret;
}
#endif

void ufshcd_set_query_req_length(u8 opcode, u8 idn, u32 *data_seg_length)
{
	if (opcode != UPIU_QUERY_OPCODE_VENDOR_WRITE)
		return;
	if (idn == VENDOR_IDN_TT)
		*data_seg_length = VENDOR_TT_MAX_LENGTH;

	return;
}
void ufshcd_set_query_req_desc(u8 opcode, u8 idn, struct utp_upiu_req *ptr, u8 *descriptor)
{
	if (opcode != UPIU_QUERY_OPCODE_VENDOR_WRITE)
		return;
	if (idn == VENDOR_IDN_TT)
		memcpy(ptr, descriptor, VENDOR_TT_MAX_LENGTH);

	return;
}
void ufshcd_init_query_vcmd(struct ufs_hba *hba,
	struct ufs_query_req **request, struct ufs_query_res **response,
	struct ufs_query_vcmd *cmd)
{
	ufshcd_init_query(hba, request, response, cmd->opcode, cmd->idn,
		cmd->index, cmd->selector);

	hba->dev_cmd.query.descriptor = cmd->desc_buf;
	(*request)->query_func = cmd->query_func;
	(*request)->upiu_req.value = cmd->value;
	(*request)->upiu_req.reserved_osf = cmd->reserved_osf;
	(*request)->upiu_req.reserved[0] = cmd->reserved[0];
	(*request)->upiu_req.reserved[1] = cmd->reserved[1];
	(*request)->upiu_req.length = cpu_to_be16(cmd->buf_len);
}

static int __ufshcd_query_vcmd(struct ufs_hba *hba,
	struct ufs_query_vcmd *cmd)
{
	struct ufs_query_req *request = NULL;
	struct ufs_query_res *response = NULL;
	int err;

	ufshcd_hold(hba, false);
	mutex_lock(&hba->dev_cmd.lock);

	ufshcd_init_query_vcmd(hba, &request, &response, cmd);

	err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, VCMD_REQ_TIMEOUT);
	if (err) {
		dev_err(hba->dev,
			"%s: opcode 0x%.2x for idn 0x%x failed, err = %d\n",
			__func__, cmd->opcode, cmd->idn, err);
		goto out_unlock;
	}

	hba->dev_cmd.query.descriptor = NULL;
	cmd->buf_len = be16_to_cpu(response->upiu_res.length);

	if (cmd->response)
		memcpy(cmd->response, response, sizeof(struct ufs_query_res));

out_unlock:
	mutex_unlock(&hba->dev_cmd.lock);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0))
	ufshcd_release(hba);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0))
	ufshcd_release(hba, false);
#else
	ufshcd_release(hba);
#endif
	return err;
}

int ufshcd_query_vcmd_single(struct ufs_hba *hba,
	struct ufs_query_vcmd *cmd)
{
	return __ufshcd_query_vcmd(hba, cmd);
}

int ufshcd_query_vcmd_retry(struct ufs_hba *hba,
	struct ufs_query_vcmd *cmd)
{
	int err = 0;
	int retries;

	for (retries = VCMD_REQ_RETRIES; retries > 0; retries--) {
		err = __ufshcd_query_vcmd(hba, cmd);
		if (!err || err == -EINVAL)
			break;
	}

	return err;
}

static int
ufshcd_ioctl_query_read_fsr(struct ufs_hba *hba,
	struct ufs_ioctl_query_data *ioctl_data, u8 *buffer, u16 *buffer_len)
{
	int err;
	struct ufs_query_vcmd cmd = { 0 };

	cmd.buf_len = *buffer_len;
	cmd.desc_buf = buffer;
	cmd.opcode = ioctl_data->opcode;
	cmd.idn = ioctl_data->idn;
	cmd.query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;

	err = ufshcd_query_vcmd_retry(hba, &cmd);
	if (err) {
		dev_err(hba->dev, "%s:  ret %d", __func__, err);

		return err;
	}

	*buffer_len = (u16)((cmd.buf_len < (int)HI1861_FSR_INFO_SIZE) ?
			cmd.buf_len : HI1861_FSR_INFO_SIZE);

	return 0;
}

static int
ufshcd_ioctl_read_fsr(struct ufs_hba *hba,
	struct ufs_ioctl_query_data *ioctl_data, void __user *buffer)
{
	int ret;
	u16 buffer_len = HI1861_FSR_INFO_SIZE;
	u8 *read_buffer = kzalloc(HI1861_FSR_INFO_SIZE, GFP_KERNEL);

	if (!read_buffer) {
		dev_err(hba->dev, "%s: alloc fail\n", __func__);
		return -ENOMEM;
	}

	ret = ufshcd_ioctl_query_read_fsr(hba, ioctl_data,
			read_buffer, &buffer_len);
	if (ret) {
		dev_err(hba->dev, "%s:  ret %d", __func__, ret);

		goto out;
	}

	ioctl_data->buf_size = buffer_len;
	ret = copy_to_user(buffer + sizeof(struct ufs_ioctl_query_data),
			   read_buffer, ioctl_data->buf_size);
	if (ret)
		dev_err(hba->dev, "%s: copy back to user err %d\n", __func__,
			ret);

out:
	kfree(read_buffer);

	return ret;
}

int ufshcd_ioctl_query_vcmd(struct ufs_hba *hba,
			    struct ufs_ioctl_query_data *ioctl_data,
			    void __user *buffer)
{
	int err;

	if (hba->manufacturer_id != UFS_VENDOR_HI1861)
		return -EINVAL;

	switch (ioctl_data->opcode) {
	case UPIU_QUERY_OPCODE_READ_HI1861_FSR:
		err = ufshcd_ioctl_read_fsr(hba, ioctl_data, buffer);
		break;
	default:
		err = -ENOIOCTLCMD;
		break;
	}

	return err;
}

#ifdef CONFIG_SCSI_UFS_HI1861_VCMD
static int ufshcd_read_fsr_info(struct ufs_hba *hba,
				u8 desc_id,
				int desc_index,
				u8 *param_read_buf,
				u32 param_size)
{
	int ret;
	struct ufs_query_vcmd cmd = { 0 };
	cmd.buf_len = param_size;
	/* allocate memory to hold full descriptor */
	cmd.desc_buf = kmalloc(cmd.buf_len, GFP_KERNEL);
	if (!cmd.desc_buf)
		return -ENOMEM;
	memset(cmd.desc_buf, 0, cmd.buf_len);
	cmd.opcode = UPIU_QUERY_OPCODE_READ_HI1861_FSR;
	cmd.idn = desc_id;
	cmd.index = desc_index;
	cmd.query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
	ret = ufshcd_query_vcmd_retry(hba, &cmd);
	if (ret) {
		dev_err(hba->dev, "%s: Failed reading FSR. desc_id %d "
				  "buff_len %d ret %d",
			__func__, desc_id, cmd.buf_len, ret);
		goto out;
	}
	memcpy(param_read_buf, cmd.desc_buf, cmd.buf_len);
out:
	kfree(cmd.desc_buf);
	return ret;
}

#ifdef CONFIG_PLATFORM_DIEID
static void ufshcd_ufs_set_dieid(struct ufs_hba *hba)
{
	/* allocate memory to hold full descriptor */
	u8 *fbuf = NULL;
	int ret = 0;

	if (hba->manufacturer_id != UFS_VENDOR_HI1861)
		return;

	if (ufs_hixxxx_dieid == NULL)
		ufs_hixxxx_dieid = kmalloc(UFS_DIEID_TOTAL_SIZE, GFP_KERNEL);
	if (!ufs_hixxxx_dieid)
		return;

	memset(ufs_hixxxx_dieid, 0, UFS_DIEID_TOTAL_SIZE);

	fbuf = kmalloc(HI1861_FSR_INFO_SIZE, GFP_KERNEL);
	if (!fbuf) {
		kfree(ufs_hixxxx_dieid);
		ufs_hixxxx_dieid = NULL;
		return;
	}

	memset(fbuf, 0, HI1861_FSR_INFO_SIZE);

	ret = ufshcd_read_fsr_info(hba, 0, 0, fbuf, HI1861_FSR_INFO_SIZE);
	if (ret) {
		is_fsr_read_failed = 1;
		dev_err(hba->dev, "[%s]READ FSR FAILED\n", __func__);
		goto out;
	}

	/* get ufs product name */
	ret = snprintf(ufs_product_name, UFS_PRODUCT_NAME_LEN, hba->dev_info.model_name);
	if (ret <= 0) {
		dev_err(hba->dev, "[%s]copy ufs product name fail\n", __func__);
		goto out;
	}

	ret = strncmp(UFS_HIXXXX_PRODUCT_NAME, hba->dev_info.model_name, HIXXXX_PROD_LEN);
	if (ret != 0) {
		/* after hi1861 ver. */
		memcpy(ufs_hixxxx_dieid, fbuf + 16, UFS_DIEID_NUM_SIZE_THOR920);
		memcpy(ufs_hixxxx_dieid + UFS_DIEID_CHIP_VER_OFFSET,
			fbuf + 36, UFS_NAND_CHIP_VER_SIZE);
		memcpy(ufs_hixxxx_dieid + UFS_DIEID_CONTROLLER_OFFSET,
			fbuf + 256, UFS_CONTROLLER_DIEID_SIZE);
		memcpy(ufs_hixxxx_dieid + UFS_DIEID_FLASH_OFFSET,
			fbuf + 448, UFS_FLASH_DIE_ID_SIZE);
		memcpy(ufs_hixxxx_dieid + UFS_DCIP_CRACK_NOW_OFFSET,
			fbuf + 440, UFS_DCIP_CRACK_NOW_SIZE);
		memcpy(ufs_hixxxx_dieid + UFS_DCIP_CRACK_EVER_OFFSET,
			fbuf + 441, UFS_DCIP_CRACK_EVER_SIZE);
	} else {
		/* hi1861 ver. */
		memcpy(ufs_hixxxx_dieid, fbuf + 12, UFS_DIEID_NUM_SIZE);
		memcpy(ufs_hixxxx_dieid + UFS_DIEID_CHIP_VER_OFFSET,
			fbuf + 28, UFS_NAND_CHIP_VER_SIZE);
		memcpy(ufs_hixxxx_dieid + UFS_DIEID_CONTROLLER_OFFSET,
			fbuf + 1692, UFS_CONTROLLER_DIEID_SIZE);
		memcpy(ufs_hixxxx_dieid + UFS_DIEID_FLASH_OFFSET,
			fbuf + 1900, UFS_FLASH_DIE_ID_SIZE);
	}
out:
	kfree(fbuf);
}

static int hufs_get_flash_dieid(
	char *dieid, u32 offset, u32 dieid_num, u8 vendor_id, u32 *flash_id)
{
	int len = 0;
	int i = 0;
	int j = 0;
	int flag = 0;
	int ret = 0;
	dieid += offset;
	/**
	*T vendor flash id, the length is 32B.As is required,
	*the output flash ids need to formatted in hex with appropriate prefix
	*eg:\r\nDIEID_UFS_FLASH_B:0x00CD...\r\n
	*   \r\nDIEID_UFS_FLASH_C:0xAC3D...\r\n
	*/
	/*lint -save -e574 -e679 */
	if (offset > UFS_DIEID_BUFFER_SIZE)
		return -EINVAL;

	if (((strncmp(ufs_product_name, UFS_PRODUCT_NAME_THOR920, UFS_PRODUCT_NAME_LEN) == 0) ||
		(strncmp(ufs_product_name, UFS_PRODUCT_NAME_SS6100, UFS_PRODUCT_NAME_LEN) == 0)) &&
		(vendor_id == UFS_FLASH_VENDOR_M)) {
			for (i = 0; i < dieid_num; i++) {
				if (dieid_num == UFS_FLASH_FOUR_DIE && (i == UFS_FLASH_TWO_DIE || i == UFS_FLASH_THREE_DIE)) {
					i += 2;
					flag = 1;
				}

				ret = snprintf(dieid + len,
						UFS_DIEID_BUFFER_SIZE - len - offset,
						"\r\nDIEID_UFS_FLASH_%c:0x%08X%08X%08X%08X00000000000000000000000000000000\r\n",
						'B' + j++,
						*(flash_id + i * 4),      /* 4: show 4byte one line */
						*(flash_id + i * 4 + 1),  /* 4: show 4byte one line,  1:array index + 1 */
						*(flash_id + i * 4 + 2),  /* 4: show 4byte one line,  2:array index + 2 */
						*(flash_id + i * 4 + 3)); /* 4: show 4byte one line,  3:array index + 3 */

				if (ret <= 0)
					return -2;
				len += ret;

				if (flag) {
					flag = 0;
					i -= 2;
				}
			}
	} else if ((vendor_id == UFS_FLASH_VENDOR_T) || (vendor_id == UFS_FLASH_VENDOR_H) || (vendor_id == UFS_FLASH_VENDOR_M) || (vendor_id == UFS_FLASH_VENDOR_Y)) {
		for (i = 0; i < dieid_num; i++) {
			ret = snprintf(dieid + len,/* [false alarm]:<UFS_DIEID_BUFFER_SIZE - len - offset> is normal */
					UFS_DIEID_BUFFER_SIZE - len - offset,
					"\r\nDIEID_UFS_FLASH_%c:0x%08X%08X%08X%08X\r\n",
					'B' + i,
					*(flash_id + i * 4),       /* 4: show 4byte one line */
					*(flash_id + i * 4 + 1),   /* 4: show 4byte one line,  1:array index + 1 */
					*(flash_id + i * 4 + 2),   /* 4: show 4byte one line,  2:array index + 2 */
					*(flash_id + i * 4 + 3));  /* 4: show 4byte one line,  3:array index + 3 */

			if (ret <= 0)
				return -2;
			len += ret;
		}
	} else
		return -2;
	/*lint -restore*/

	return 0;
}
#endif
#endif

#ifdef CONFIG_PLATFORM_DIEID
static int hufs_get_dieid(char *dieid, unsigned int len)
{
#ifdef CONFIG_SCSI_UFS_HI1861_VCMD
	int length = 0;
	int ret = 0;
	u32 dieid_num = 0;
	u8 vendor_id = 0;
	u32 *controller_id = NULL;
	u32 *flash_id = NULL;
	u8 dieCrackNow = 0;
	u8 dieCrackEver = 0;
	char buf[UFS_DIEID_BUFFER_SIZE] = {0};

	if (dieid == NULL || ufs_hixxxx_dieid == NULL)
		return -2;
	if (is_fsr_read_failed)
		return -1;

	dieid_num = *(u32 *)ufs_hixxxx_dieid;
	vendor_id = *(u8 *)(ufs_hixxxx_dieid + UFS_DIEID_CHIP_VER_OFFSET);
	controller_id = (u32 *)(ufs_hixxxx_dieid + UFS_DIEID_CONTROLLER_OFFSET);
	flash_id = (u32 *)(ufs_hixxxx_dieid + UFS_DIEID_FLASH_OFFSET);
	dieCrackNow = *(u8 *)(ufs_hixxxx_dieid + UFS_DCIP_CRACK_NOW_OFFSET);
	dieCrackEver = *(u8 *)(ufs_hixxxx_dieid + UFS_DCIP_CRACK_EVER_OFFSET);

	ret = snprintf(buf, UFS_DIEID_BUFFER_SIZE,
			"\r\nDIEID_UFS_CONTROLLER_A:0x%08X%08X%08X%08X%08X%08X%08X%08X\r\n",
			*controller_id, *(controller_id + 1),         /* 1: array index */
			*(controller_id + 2), *(controller_id + 3),   /* 2, 3: array index */
			*(controller_id + 4), *(controller_id + 5),   /* 4, 5: array index */
			*(controller_id + 6), *(controller_id + 7));  /* 6, 7: array index */
	if (ret <= 0)
		return -2;
	length += ret;

	ret = hufs_get_flash_dieid(
		buf, length, dieid_num, vendor_id, flash_id);
	if (ret != 0)
		return ret;

	length = strlen(buf);

	ret = snprintf(buf + length, UFS_DIEID_BUFFER_SIZE - length,
		"\r\nCRACK_NOW:0x%08X\r\n\r\nCRACK_EVER:0x%08X\r\n",
		dieCrackNow, dieCrackEver);

	if (ret <= 0)
		return -2;

	if (len >= strlen(buf))
		strncat(dieid, buf, strlen(buf));
	else
		return strlen(buf);

	return 0;
#else
	return -1;
#endif
}
#endif

#if defined(CONFIG_SCSI_UFS_HI1861_VCMD) && defined(CONFIG_PLATFORM_DIEID)
void hufs_bootdevice_get_dieid(struct ufs_hba *hba)
{
	char dieid[UFS_DIEID_BUFFER_SIZE] = {0};
	int ret;
	ufshcd_ufs_set_dieid(hba);
	ret = hufs_get_dieid(dieid, UFS_DIEID_BUFFER_SIZE - 1);
	if (ret) {
		dev_err(hba->dev, "[%s]dieid out of buf\n", __func__);
	}
	set_bootdevice_hufs_dieid(BOOT_DEVICE_UFS, dieid);
}
#endif
