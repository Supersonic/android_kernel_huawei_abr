/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_file_device.c
 * Description: rdr abstration of file saving operations
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
#include "rdr_file_device.h"

#include "securec.h"

#include <linux/rtc.h>

static LIST_HEAD(rdr_file_list);

static struct rdr_file_device g_resetlogs = {
	.name = "reset_logs",
	.root_path = "/data/log/reliability/reset_logs/",
	.time_root_path = NULL,
};

struct rdr_file_device *g_file_device = &g_resetlogs;

/*
 * Description : merge substring into whole path,
   warning: 1. MUST END with NULL !!!
            2. the returned pointer shall be freed when not in use
 */
static char *rdr_generate_path(const char *first, ...)
{
	va_list args;
	const char *arg;
	char *path = NULL;

	path = kzalloc(RDR_MAX_PATH_SIZE, GFP_KERNEL);
	if (path == NULL) {
		PRINT_ERROR("%s(): kzalloc path memory failed", __func__);
		return NULL;
	}

	va_start(args, first);
	for (arg = first; arg != NULL; arg = va_arg(args, const char*)){
		PRINT_DEBUG("%s(): %s\n", __func__, arg);
		if (strcat_s(path, RDR_MAX_PATH_SIZE, arg) != 0) {
			PRINT_ERROR("%s(): strcat failed", __func__);
			return NULL;
		}
	}
	va_end(args);
	return path;
}

/*
 * Description : add a file, return file
 */
struct rdr_file *rdr_add_file(const char *name, u64 size)
{
	struct rdr_file *file = vmalloc(sizeof(struct rdr_file) + sizeof(char) * size);
	if (file == NULL) {
		PRINT_ERROR("%s(): vmalloc rdr_file failed", __func__);
		return NULL;
	}
	file->name = name;
	file->size = size;
	memset_s(file->buf, size, 0, size);
	list_add(&file->list, &rdr_file_list);
	return file;
}

/* shall be put into separate time driver */
static char *rdr_get_timestamp(void)
{
	struct rtc_time tm;
	struct timespec64 tv;
	static char databuf[DATA_MAXLEN + 1];
	(void)memset_s(databuf, DATA_MAXLEN + 1, 0, DATA_MAXLEN + 1);
	(void)memset_s(&tv, sizeof(tv), 0, sizeof(tv));
	(void)memset_s(&tm, sizeof(tm), 0, sizeof(tm));
	ktime_get_real_ts64(&tv);
	tv.tv_sec -= (long)sys_tz.tz_minuteswest * 60; // 60 seconds
	rtc_time_to_tm(tv.tv_sec, &tm);
	(void)snprintf_s(databuf, DATA_MAXLEN + 1, DATA_MAXLEN + 1, "%04d%02d%02d%02d%02d%02d",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, // 1900 start year
		tm.tm_hour, tm.tm_min, tm.tm_sec);
	PRINT_DEBUG("rdr: %s [%s] !\n", __func__, databuf);
	return databuf;
}

static long rdr_get_hash(void)
{
	/* use only one int value to save time: */
	struct timespec64 tv;
	(void)memset_s(&tv, sizeof(tv), 0, sizeof(tv));
	ktime_get_real_ts64(&tv);
	return (long)tv.tv_nsec % 10000000; // 10000000 means 8bit max
}
/* shall be put into separate time driver */

static int rdr_time_filefolder_init(struct rdr_file_device *dev)
{
	int ret;
	char path[DATATIME_MAXLEN] = {0};

	if (dev == NULL) {
		PRINT_ERROR("%s(): device ptr is NULL", __func__);
		return -1;
	}

	ret = snprintf_s(path, DATATIME_MAXLEN, DATATIME_MAXLEN - 1,
		"%s-%08ld", rdr_get_timestamp(), rdr_get_hash());
	if (unlikely(ret < 0)) {
		PRINT_ERROR("%s(): snprintf_s date ret %d!\n", __func__, ret);
		return -1;
	}
	PRINT_DEBUG("%s(): time folder name is %s", __func__, path);

	dev->time_root_path = rdr_generate_path(dev->root_path, (const char*)path, "/", NULL);
	if (dev->time_root_path == NULL) {
		PRINT_ERROR("%s(): generate time path failed", __func__);
		return -1;
	}

	PRINT_DEBUG("%s(): time root path is %s", __func__, dev->time_root_path);
	return 0;
}

/*
 * Description : Init file register service structures
 */
static int rdr_file_device_init(void)
{
	return 0;
}

/*
 * Description : write registered & filled buffer into file
 */
static int rdr_write_buflist_to_file(void)
{
	struct rdr_file *file = NULL;
	struct list_head *cur = NULL;
	struct list_head *next = NULL;
	char *path = NULL;

	/* use list_for_each_safe here since we need to free the file structure */
	list_for_each_safe(cur, next, &rdr_file_list) {
		file = list_entry(cur, struct rdr_file, list);
		if (file == NULL)
			continue;
		path = rdr_generate_path(g_file_device->time_root_path, file->name, NULL);
		if (path == NULL)
			continue;
		PRINT_INFO("%s(): path %s", __func__, path);
		if (file->used_size > file->size || file->used_size == 0) {
			PRINT_ERROR("%s(): generate failed, used_size(%llx) allocated size(%llx)",
				__func__, file->used_size, file->size);
			kfree(path);
			continue;
		}
		if (rdr_create_dir(g_file_device->time_root_path) != 0)
			PRINT_ERROR("%s(): create path %s failed", __func__,
				g_file_device->time_root_path);
		rdr_file_create(path, file->buf, file->used_size);

		kfree(path);
		list_del(cur);
		vfree(file);
	}
	return 0;
}

static int rdr_file_device_late_init(void)
{
	int ret;
	if (rdr_time_filefolder_init(g_file_device) != 0) {
		PRINT_ERROR("%s(): init time folder path failed", __func__);
		ret = -1;
		return ret;
	}
	ret =  rdr_write_buflist_to_file();
	return ret;
}

/*
 * Description : create folder and copy files into it
 */
static int rdr_file_device_reset_handler(void)
{
	return 0;
}

static void rdr_file_device_exit(void)
{
	if (g_file_device->time_root_path != NULL) {
		PRINT_INFO("%s(): free time folder path", __func__);
		kfree(g_file_device->time_root_path);
	}
}

int rdr_wait_file_devices(void)
{
	return rdr_wait_partition("/data/log/");
}

struct module_data rdr_file_device_data = {
	.name = "rdr_file_device",
	.level = RDR_FILE_DEVICE_LEVEL,
	.on_panic = NULL,
	.init = rdr_file_device_init,
	.on_storage_ready = rdr_file_device_late_init,
	.init_done = NULL,
	.last_reset_handler = rdr_file_device_reset_handler,
	.exit = rdr_file_device_exit,
};
