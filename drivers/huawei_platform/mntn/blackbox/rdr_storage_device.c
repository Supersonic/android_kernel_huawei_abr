/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_storage_device.c
 * Description: rdr abstration of dfx partition (non-volatile device) handling operations
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/printk.h>
#include <linux/fs.h>

#include "rdr_inner.h"
#include "rdr_module.h"
#include "rdr_storage_api.h"
#include "rdr_storage_device.h"

#include "securec.h"

static struct rdr_storage_device g_dfx = {
	.name = "dfx",
	.sys_path = "/dev/block/by-name/dfx",
	.stack_ptr = 0,
	.device_size = 0x2000000, /* DFX partition size is fixed to 32M */
};

struct rdr_storage_device *g_storage_device = &g_dfx;

struct rdr_head_info g_head = {
	.magic = 0,
	.used_number = 0,
};
struct rdr_head_info *g_head_info = &g_head;

struct rdr_head_info g_last_head = {
	.magic = 0,
	.used_number = 0,
};
struct rdr_head_info *g_last_head_info = &g_last_head;

/*
 * Description : allocate space on rdr storage device
	return offset on rdr storage device, -1 when fail to allocate
 */
long rdr_storage_allocate(long size)
{
	long ret;
	if (size + g_storage_device->stack_ptr > g_storage_device->device_size) {
		PRINT_ERROR("%s(): no enough space on device %s, stack_ptr: %llu, tried size: %ld",
			__func__, g_storage_device->name, g_storage_device->stack_ptr, size);
		return -1;
	}
	ret = g_storage_device->stack_ptr;
	g_storage_device->stack_ptr += size;
	return ret;
}

int rdr_wait_storage_devices(void)
{
	return rdr_wait_partition(g_storage_device->sys_path);
}

/*
 * Description : allocate space for rdr header in memory
 */
int rdr_storage_device_init(void)
{
	int i;
	long header_offset;
	header_offset = rdr_storage_allocate(sizeof(struct rdr_head_info));
	if (header_offset == -1) {
		PRINT_ERROR("%s(): allocate space for logsave info failed", __func__);
		return -1;
	}
	memset_s(g_head_info, sizeof(struct rdr_head_info), 0, sizeof(struct rdr_head_info));
	g_head_info->magic = RDR_KERNEL_SET_MAGIC;
	g_head_info->used_number = 0;

	/* rdr region work */
	for (i = 0; i < RDR_LOGSAVE_NUMBER; i++) {
		memset_s(&g_storage_device->regions[i], sizeof(struct rdr_region), 0, sizeof(struct rdr_region));
	}
	return 0;
}

static void rdr_storage_device_release(void)
{
	int i;
	/* rdr region work */
	for (i = 0; i < RDR_LOGSAVE_NUMBER; i++) {
		if (g_storage_device->regions[i].data != NULL) {
			PRINT_INFO("%s(): release malloc-ed memory for mod number[%d], size: %u",
				__func__, i, g_storage_device->regions[i].size);
			vfree(g_storage_device->regions[i].data);
			g_storage_device->regions[i].data = NULL;
		}
	}
}

static void rdr_storage_device_initdone(void)
{
	rdr_storage_device_release();
}


/*
 * Description : write allocated space info back to storage
 */
int rdr_storage_device_apply(void)
{
	if (rdr_partition_read(g_storage_device->sys_path, 0, (char *)g_last_head_info,
		sizeof(struct rdr_head_info)) != 0)
		return -1;
	if (rdr_partition_write(g_storage_device->sys_path, 0, (char *)g_head_info,
		sizeof(struct rdr_head_info)) != 0)
		return -1;
	return 0;
}

static u64 rdr_hash(const char *str)
{
	u64 hash = 5381; // 5381 seed
	if (!str)
		return 0;
	while (*str) {
		hash = ((hash << 5) + hash) + *str; /* 5 : hash * 33 + c */
		str++;
	}
	return hash;
}

static int rdr_get_logsave_index(const struct rdr_head_info *head_info, u64 modid)
{
	int i;
	const struct rdr_logsave_info *curr_info = NULL;
	for (i = 0; i < head_info->used_number; i++) {
		curr_info = &head_info->rdr_info[i];
		PRINT_DEBUG("%s(): modid 0x%llx", __func__, curr_info->modid);
		if (curr_info->modid == modid)
			return i;
	}
	return -1;
}

int rdr_register_logsave_info(const char *modname, const struct rdr_logsave_info *info)
{
	int index;
	long long storage_offset;
	struct rdr_logsave_info *curr_info = NULL;
	u64 modid;

	modid = rdr_hash(modname);
	PRINT_INFO("%s(): modid %llx", __func__, modid);
	index = rdr_get_logsave_index(g_head_info, modid);
	if (index != -1) {
		PRINT_INFO("%s(): modid already registered", __func__);
		return 0;
	}

	if (g_head_info->used_number >= RDR_LOGSAVE_NUMBER) {
		PRINT_ERROR("%s(): rdr logsave entries used up", __func__);
		return -1;
	}
	storage_offset = rdr_storage_allocate(info->log_size);
	if (storage_offset == -1) {
		PRINT_ERROR("%s(): allocate space for dump part failed, accquired size: %u",
			__func__, info->log_size);
		return -1;
	}
	curr_info = &g_head_info->rdr_info[g_head_info->used_number];

	/* can be optimized using memcpy */
	curr_info->log_phyaddr = info->log_phyaddr;
	curr_info->log_size = info->log_size;
	curr_info->storage_offset = storage_offset;
	curr_info->magic = RDR_KERNEL_SET_MAGIC;
	curr_info->modid = modid;
	/* can be optimized using memcpy */

	g_head_info->used_number++;

	PRINT_INFO("%s(): phyaddr: 0x%llx, len: %u, offset: 0x%llx, used number: %d",
		__func__, curr_info->log_phyaddr, curr_info->log_size, curr_info->storage_offset,
		g_head_info->used_number);
	return 0;
}

struct rdr_region *rdr_acquire_region(const char *modname)
{
	int index;
	struct rdr_logsave_info *curr_info = NULL;
	struct rdr_region *region = NULL;
	u64 modid;

	modid = rdr_hash(modname);
	index = rdr_get_logsave_index(g_last_head_info, modid);
	if (index == -1)
		return NULL;
	curr_info = &g_last_head_info->rdr_info[index];
	if (curr_info->magic != RDR_BOOT_RESPONSE_MAGIC) {
		PRINT_INFO("%s(): mod 0x%llx is found, but magic is not set, return now",
			__func__, modid);
		return NULL;
	}

	/* rdr region work */
	region = &g_storage_device->regions[index];

	PRINT_INFO("%s(): header check success, logsize: %u\n", __func__, curr_info->log_size);

	if (region->data != NULL) {
		PRINT_ERROR("%s(): already acquired, return without read", __func__);
		return region;
	}

	region->data = vmalloc(curr_info->log_size);
	if (region->data == NULL) {
		PRINT_ERROR("%s(): no memory\n", __func__);
		return NULL;
	}

	/* set region size, and clear allocated memory */
	region->size = curr_info->log_size;
	memset_s(region->data, curr_info->log_size, 0, curr_info->log_size);

	if (rdr_partition_read(g_storage_device->sys_path, curr_info->storage_offset,
		region->data, curr_info->log_size) != 0) {
		PRINT_ERROR("%s(): read partition failed\n", __func__);
		vfree(region->data);
		return NULL;
	}
	/* no free, given region to caller */
	return region;
}

struct module_data rdr_storage_device_data = {
	.name = "rdr_storage_device",
	.level = RDR_STORAGE_DEVICE_LEVEL,
	.on_panic = NULL,
	.init = rdr_storage_device_init,
	.on_storage_ready = rdr_storage_device_apply,
	.init_done = rdr_storage_device_initdone,
	.last_reset_handler = NULL,
	.exit = NULL,
};
