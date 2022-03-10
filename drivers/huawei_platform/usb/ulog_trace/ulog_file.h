/*
 * ulog_file.h
 *
 * save adsp log
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#ifndef _ULOG_FILE_H_
#define _ULOG_FILE_H_

#include <linux/types.h>

#define ULOG_FILE_NAME_SIZE  256
#define ULOG_FILE_MAX_SIZE   4194304 /* 4M */
#define ULOG_FILE_PATH       "/data/log/hilogs/adsp-log-"
#define ULOG_BASIC_YEAR      1900
#define ULOG_DATA_HEAD_SIZE  256

struct ulog_file {
	char name[ULOG_FILE_NAME_SIZE];
};

void ulog_init(struct ulog_file *file);
void ulog_write(struct ulog_file *file, const char *data, u32 size);

#endif /* _ULOG_FILE_H_ */