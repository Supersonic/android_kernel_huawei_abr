/*
 * proc_ufs.c
 *
 * ufs proc configuration
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

#include <linux/bootdevice.h>
#include "proc_ufs.h"
#include "securec.h"
#include "ufs.h"
#include "ufshcd.h"

int ufshcd_read_geometry_desc(struct ufs_hba *hba, u8 *buf, u32 size)
{
	if (hba == NULL || buf == NULL)
		return -1;
	return ufshcd_read_desc(hba, QUERY_DESC_IDN_GEOMETRY, 0, buf, size);
}

void ufs_get_geometry_info(struct ufs_hba *hba)
{
	int err;
	uint8_t desc_buf[QUERY_DESC_GEOMETRY_MAX_SIZE];
	u64 total_raw_device_capacity;
	if (hba == NULL)
		return;
	err =
		ufshcd_read_geometry_desc(hba, desc_buf, QUERY_DESC_GEOMETRY_MAX_SIZE);
	if (err) {
		dev_err(hba->dev, "%s: Failed getting geometry info\n", __func__);
		goto out;
	}
	total_raw_device_capacity =
		(u64)desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 0] << 56 |
		(u64)desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 1] << 48 |
		(u64)desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 2] << 40 |
		(u64)desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 3] << 32 |
		(u64)desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 4] << 24 |
		(u64)desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 5] << 16 |
		(u64)desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 6] << 8 |
		desc_buf[GEOMETRY_DESC_PARAM_DEV_CAP + 7] << 0;
	set_bootdevice_size(BOOT_DEVICE_UFS, total_raw_device_capacity);

out:
	return;
}

void ufs_set_sec_unique_number(struct ufs_hba *hba,
					uint8_t *str_desc_buf,
					char *product_name)
{
	int i, idx;
	u32 cid[UFS_CID_SIZE];
	uint8_t snum_buf[SERIAL_NUM_SIZE + 1];
	struct ufs_unique_number unique_number = {0};
	if (hba == NULL || str_desc_buf == NULL || product_name == NULL)
		return;
	memset(snum_buf, 0, sizeof(snum_buf));

	switch (hba->manufacturer_id) {
	case UFS_VENDOR_SAMSUNG:
		/* Samsung V4 UFS need 24 Bytes for serial number, transfer unicode to 12 bytes
		 * the magic number 12 here was following original below HYNIX/TOSHIBA decoding method
		*/
		for (i = 0; i < 12; i++) {
			idx = QUERY_DESC_HDR_SIZE + i * 2 + 1;
			snum_buf[i] = str_desc_buf[idx];
		}
		break;
	case UFS_VENDOR_SKHYNIX:
		/* hynix only have 6Byte, add a 0x00 before every byte */
		for (i = 0; i < 6; i++) {
			/*lint -save  -e679 */
			snum_buf[i * 2] = 0x0;
			snum_buf[i * 2 + 1] =
				str_desc_buf[QUERY_DESC_HDR_SIZE + i];
			/*lint -restore*/
		}
		break;
	case UFS_VENDOR_TOSHIBA:
		/*
		 * toshiba: 20Byte, every two byte has a prefix of 0x00, skip
		 * and add two 0x00 to the end
		 */
		for (i = 0; i < 10; i++)
			snum_buf[i] =
				str_desc_buf[QUERY_DESC_HDR_SIZE + i * 2 + 1];/*lint !e679*/
		break;
	case UFS_VENDOR_HI1861:
		memcpy(snum_buf, str_desc_buf + QUERY_DESC_HDR_SIZE, 12);
		break;
	case UFS_VENDOR_MICRON:
		memcpy(snum_buf, str_desc_buf + QUERY_DESC_HDR_SIZE, 4);
		for(i = 4; i < 12; i++)
			snum_buf[i] = 0;
		break;
	case UFS_VENDOR_SANDISK:
		for (i = 0; i < 12; i++) {
			idx = QUERY_DESC_HDR_SIZE + i * 2 + 1;
			snum_buf[i] = str_desc_buf[idx];
		}
		break;
	case UFS_VENDOR_XHFZ:
		for (i = 0; i < 4; i++) {
			idx = QUERY_DESC_SNUM_XHFZ + i * 2 + 1;
			snum_buf[i] = str_desc_buf[idx];
		}
		for (i = 4; i < 12; i++) {
			idx = QUERY_DESC_SNUM_XHFZ_SEC + i * 2 + 1;
			snum_buf[i] = str_desc_buf[idx];
		}
		break;
	default:
		dev_err(hba->dev, "unknown ufs manufacturer id\n");
		break;
	}

	unique_number.manufacturer_id = hba->manufacturer_id;
	unique_number.manufacturer_date = hba->manufacturer_date;
	memcpy(unique_number.serial_number, snum_buf, SERIAL_NUM_SIZE);
	memcpy(cid, (u32 *)&unique_number, sizeof(cid));
	for (i = 0; i < UFS_CID_SIZE - 1; i++)
		cid[i] = be32_to_cpu(cid[i]);
	cid[UFS_CID_SIZE - 1] =
		((cid[UFS_CID_SIZE - 1] & 0xffff) << 16) |
		((cid[UFS_CID_SIZE - 1] >> 16) & 0xffff);
	set_bootdevice_cid(BOOT_DEVICE_UFS, (u32 *)cid);
#ifdef CONFIG_HUAWEI_KERNEL_DEBUG
	pr_notice("ufs_debug manufacturer_id:%d\n", unique_number.manufacturer_id);
	pr_notice("ufs_debug manufacturer_date:%d\n", unique_number.manufacturer_date);
#endif
}

int ufshcd_read_health_desc(struct ufs_hba *hba, u8 *buf, u32 size)
{
	return ufshcd_read_desc(hba, QUERY_DESC_IDN_HEALTH, 0, buf, size);
}
EXPORT_SYMBOL(ufshcd_read_health_desc);

void ufs_set_health_desc(struct ufs_hba *hba)
{
	int err;
	int buff_len = QUERY_DESC_HEALTH_DEF_SIZE;
	uint8_t desc_buf[QUERY_DESC_HEALTH_DEF_SIZE] = {0};
	if (hba == NULL)
		return;

	err = ufshcd_read_health_desc(hba, desc_buf, buff_len);
	if (err) {
		dev_info(hba->dev, "%s: Failed reading PRL. err = %d\n",
				__func__, err);
		return;
	}
	set_bootdevice_pre_eol_info(BOOT_DEVICE_UFS, (u8)desc_buf[2]);
	set_bootdevice_life_time_est_typ_a(BOOT_DEVICE_UFS, (u8)desc_buf[3]);
	set_bootdevice_life_time_est_typ_b(BOOT_DEVICE_UFS, (u8)desc_buf[4]);
}

int ufs_proc_set_device(struct ufs_hba *hba, u8 model_index, u8 *desc_buf, size_t buff_len)
{
	int err;
	u8 serial_num_index;
	u8 prl_index;
	char prl[MAX_PRL_LEN + 1] = {0};
	u8 *dev_buf = NULL;
	struct ufs_dev_info *dev_info = &hba->dev_info;
	if (hba == NULL)
		return -1;
	hba->manufacturer_id = hba->dev_info.wmanufacturerid;
	hba->manufacturer_date = desc_buf[DEVICE_DESC_PARAM_MANF_DATE] << 8 |
				     desc_buf[DEVICE_DESC_PARAM_MANF_DATE + 1];
	serial_num_index = desc_buf[DEVICE_DESC_PARAM_SN];
	prl_index = desc_buf[DEVICE_DESC_PARAM_PRDCT_REV];

	err = ufshcd_read_string_desc(hba, model_index, &dev_buf, true/*ASCII*/);
	if (err < 0) {
		dev_err(hba->dev, "%s: Failed reading Product Name. err = %d\n",
			__func__, err);
		return err;
	}

	strlcpy(dev_info->model_name, dev_buf,
		min_t(u8, dev_buf[QUERY_DESC_LENGTH_OFFSET],
		      sizeof(dev_info->model_name)));
	kfree(dev_buf);

	/* Get serial number */
	err = ufshcd_read_string_desc(hba, serial_num_index, &dev_buf, false);
	if (err < 0) {
		dev_info(hba->dev, "%s: Failed reading Serial Number. err = %d\n",
			__func__, err);
		return err;
	}

	ufs_set_sec_unique_number(hba, dev_buf, dev_info->model_name);
	set_bootdevice_product_name(BOOT_DEVICE_UFS, dev_info->model_name);
	set_bootdevice_manfid(BOOT_DEVICE_UFS, hba->manufacturer_id);
	kfree(dev_buf);

	err = ufshcd_read_string_desc(hba, prl_index, &dev_buf, true);
	if (err < 0) {
		dev_info(hba->dev, "%s: Failed reading PRL. err = %d\n",
				__func__, err);
		return err;
	}

	strlcpy(prl, dev_buf,
		min_t(u8, dev_buf[QUERY_DESC_LENGTH_OFFSET],
			MAX_PRL_LEN));
	kfree(dev_buf);

	set_bootdevice_fw_version(BOOT_DEVICE_UFS, prl);
	return 0;
}
