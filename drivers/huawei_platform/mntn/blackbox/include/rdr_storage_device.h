/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_storage_device.h
 * Description: rdr storage interfaces
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#ifndef RDR_STORAGE_DEVICE_H
#define RDR_STORAGE_DEVICE_H

#define RDR_LOGSAVE_NUMBER 50

struct rdr_region {
	u32 size;
	char *data;
};

struct rdr_logsave_info {
	u64 log_phyaddr;
	u32 log_size;
	u64 storage_offset;
	u32 magic;
	u64 modid;
};


struct rdr_storage_device {
	char *name;
	char *sys_path;
	u64 stack_ptr;
	u64 device_size;
	struct rdr_region regions[RDR_LOGSAVE_NUMBER];
};

struct rdr_head_info {
	u64 magic;
	int used_number;
	struct rdr_logsave_info rdr_info[RDR_LOGSAVE_NUMBER];
};

#define RDR_KERNEL_SET_MAGIC     0xAF00D5E7
#define RDR_BOOT_RESPONSE_MAGIC  0xBAADF00D

long rdr_storage_allocate(long size);
int rdr_register_logsave_info(const char *modid, const struct rdr_logsave_info *info);
struct rdr_region *rdr_acquire_region(const char *modid);
int rdr_wait_storage_devices(void);

#endif