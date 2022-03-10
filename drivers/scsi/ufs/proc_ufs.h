/*
 * proc_ufs.h
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

#ifndef _PROCUFS_H_
#define _PROCUFS_H_

#include "ufshcd.h"
#include "ufs.h"

/* UNIQUE NUMBER definition */
struct ufs_unique_number {
	u8 serial_number[12];
	u16 manufacturer_date;
	u16 manufacturer_id;
};

#define MAX_MODEL_LEN 16
#define MAX_PRL_LEN  5

void ufs_get_geometry_info(struct ufs_hba *hba);
void ufs_set_health_desc(struct ufs_hba *hba);
void ufs_set_sec_unique_number(struct ufs_hba *hba,
					uint8_t *str_desc_buf,
					char *product_name);
int ufs_proc_set_device(struct ufs_hba *hba,
						u8 model_index,
						u8 *desc_buf,
						size_t buff_len);
#endif
