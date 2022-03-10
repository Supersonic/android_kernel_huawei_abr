/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_file_device.h
 * Description: rdr abstration of file saving operations
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#ifndef RDR_FILE_DEVICE_H
#define RDR_FILE_DEVICE_H

#include <linux/list.h>

#define DATA_MAXLEN         14
#define DATATIME_MAXLEN     24 /* 14+8 +2, 2: '-'+'\0' */

struct rdr_file_device {
	char *name;
	char *root_path;
	char *time_root_path;
};

struct rdr_file {
	struct list_head list;
	const char *name;
	u64 size;
	u64 used_size;
	char buf[0];
};

struct rdr_file *rdr_add_file(const char *name, u64 size);
int rdr_wait_file_devices(void);

#endif