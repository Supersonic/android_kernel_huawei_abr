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

#include "ulog_file.h"
#include <linux/version.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <huawei_platform/log/hw_log.h>

#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG ulog_trace
HWLOG_REGIST();

static void ulog_get_time(struct tm *ptm)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0)
	struct timespec64 tv;
#else
	struct timeval tv;
#endif

	memset(&tv, 0, sizeof(tv));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0)
	ktime_get_real_ts64(&tv);
	time64_to_tm(tv.tv_sec, 0, ptm);
#else
	do_gettimeofday(&tv);
	time_to_tm(tv.tv_sec, 0, ptm);
#endif
}

static void ulog_get_file_name(struct ulog_file *file)
{
	struct tm ctm;

	ulog_get_time(&ctm);
	memset(file->name, 0, sizeof(file->name));
	snprintf(file->name, ULOG_FILE_NAME_SIZE - 1,
		"%s%u-%u-%u_%u.%u.%u",
		ULOG_FILE_PATH,
		ctm.tm_year + ULOG_BASIC_YEAR, ctm.tm_mon + 1,
		ctm.tm_mday, ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
}

static void ulog_check_file(struct ulog_file *file)
{
	struct kstat stat;

	memset(&stat, 0, sizeof(stat));
	vfs_stat(file->name, &stat);

	hwlog_info("ulog file stat size=%u\n", stat.size);
	if (stat.size > ULOG_FILE_MAX_SIZE) {
		ulog_get_file_name(file);
		hwlog_info("new ulog file %s\n", file->name);
	}
}

static struct file *ulog_open_file(struct ulog_file *file)
{
	struct file *fp = NULL;

	fp = filp_open(file->name, O_WRONLY | O_APPEND, 0644);
	if (!IS_ERR(fp))
		return fp;

	hwlog_info("file %s not exist\n", file->name);
	fp = filp_open(file->name, O_WRONLY | O_CREAT, 0644);
	if (IS_ERR(fp)) {
		hwlog_info("create %s file fail\n", file->name);
		return NULL;
	}

	return fp;
}

static int ulog_write_data(struct file *fp, const char *data, u32 size)
{
	char head[ULOG_DATA_HEAD_SIZE];
	struct tm ctm;
	loff_t pos = 0;
	int ret;

	ulog_get_time(&ctm);
	memset(head, 0, sizeof(head));
	snprintf(head, ULOG_DATA_HEAD_SIZE - 1,
		"\n\nupload time: %u-%u-%u %u:%u:%u\n\n",
		ctm.tm_year + ULOG_BASIC_YEAR, ctm.tm_mon + 1,
		ctm.tm_mday, ctm.tm_hour, ctm.tm_min, ctm.tm_sec);

	ret = vfs_write(fp, head, strlen(head), &pos);
	if (ret != strlen(head)) {
		hwlog_err("write head failed ret=%d\n", ret);
		return -EFAULT;
	}

	ret = vfs_write(fp, data, size, &pos);
	if (ret != size) {
		hwlog_err("write data failed ret=%d\n", ret);
		return -EFAULT;
	}

	return 0;
}

static int ulog_write_file(struct ulog_file *file, const char *data, u32 size)
{
	int ret;
	struct file *fp = NULL;

	fp = ulog_open_file(file);
	if (!fp) {
		hwlog_err("open %s failed\n", file->name);
		return -EFAULT;
	}

	ret = vfs_llseek(fp, 0L, SEEK_END);
	if (ret < 0) {
		hwlog_err("lseek %s failed from end\n", file->name);
		filp_close(fp, NULL);
		return -EFAULT;
	}

	ret = ulog_write_data(fp, data, size);
	if (ret) {
		hwlog_err("write %s failed ret=%d\n", file->name, ret);
		filp_close(fp, NULL);
		return -EFAULT;
	}

	filp_close(fp, NULL);
	return 0;
}

void ulog_write(struct ulog_file *file, const char *data, u32 size)
{
	mm_segment_t fs;

	if (!file || !data || (size == 0))
		return;

	ulog_check_file(file);

	fs = get_fs();
	set_fs(KERNEL_DS);
	ulog_write_file(file, data, size);
	set_fs(fs);

	hwlog_info("write ulog size=%u\n", size);
}

void ulog_init(struct ulog_file *file)
{
	if (!file)
		return;

	ulog_get_file_name(file);
}
