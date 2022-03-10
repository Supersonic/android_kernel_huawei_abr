/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_freq.c
 * Description: rdr ocp ram parser
 * Author: 2021-04-15 zhangxun
 */
#include <linux/syscalls.h>
#include <linux/kallsyms.h>

#include <linux/cpufreq.h>
#include <linux/slab.h>
#include <linux/sched/clock.h>

#include "rdr_inner.h"
#include "rdr_storage_api.h"
#include "rdr_storage_device.h"
#include "rdr_module.h"
#include "rdr_file_device.h"

#include "securec.h"

#define INFO_SIZE 64
#define PRINT_SIZE 512
struct diaginfo {
	char main[INFO_SIZE];
	char sub[INFO_SIZE];
};

#define RDR_PRINT_BUFFER_SIZE (128 * 1024)

static int register_diaginfo(void)
{
	struct rdr_logsave_info info;

	info.log_phyaddr       = 0;
	info.log_size          = sizeof(struct diaginfo);
	rdr_register_logsave_info("diaginfo", &info);

	info.log_phyaddr       = 0;
	info.log_size          = RDR_PRINT_BUFFER_SIZE;
	rdr_register_logsave_info("bsplog", &info);
	return 0;
}

static int parse_printbuf(void)
{
	struct rdr_region *region = NULL;
	struct rdr_file *file = NULL;
	char *bsplog_start = NULL;
	u32 bsplog_size;
	char print_line[PRINT_SIZE] = {0};
	char *str = NULL;
	u32 count = 0;

	region = rdr_acquire_region("bsplog");
	if (region == NULL) {
		PRINT_ERROR("%s(): aquire region failed", __func__);
		return -1;
	}
	file = rdr_add_file("bsp_errlog.txt", RDR_PRINT_BUFFER_SIZE);
	if (file == NULL) {
		PRINT_ERROR("%s(): allocate file buffer failed", __func__);
		return -1;
	}

	bsplog_start = region->data + sizeof(u32);
	bsplog_size = *(u32 *)region->data - sizeof(u32);

	PRINT_INFO("%s(): bsp vbuf:%p, logsize: %u", __func__, bsplog_start, bsplog_size);

	if (rdr_safe_copy(file->buf, RDR_PRINT_BUFFER_SIZE, bsplog_start, bsplog_size,
		region->data, region->size) != 0) {
		PRINT_ERROR("%s(): failed", __func__);
		return -1;
	}
	file->used_size = strlen(file->buf);

	for (str = file->buf; str < file->buf + file->used_size; str++) {
		print_line[count] = *str;
		count++;
		if (*str == '\n') {
			PRINT_INFO("%s", print_line);
			count = 0;
			if (memset_s(print_line, PRINT_SIZE, 0, PRINT_SIZE) != 0) {
				PRINT_ERROR("%s(): memset failed", __func__);
				break;
			}
		}
	}
	return 0;
}

static int parse_diaginfo(void)
{
	struct rdr_region *region = NULL;
	struct rdr_file *file = NULL;
	struct diaginfo info;
	u32 info_len;

	region = rdr_acquire_region("diaginfo");
	if (region == NULL) {
		PRINT_ERROR("%s(): aquire region failed", __func__);
		return -1;
	}
	if (rdr_safe_copy((char *)&info, sizeof(struct diaginfo), region->data,
		sizeof(struct diaginfo), region->data, region->size) != 0) {
		PRINT_ERROR("%s(): failed", __func__);
		return -1;
	}
	info_len = region->size * 5; // 5 of size
	file = rdr_add_file("diaginfo.log", info_len);
	if (file == NULL) {
		PRINT_ERROR("%s(): allocate file buffer failed", __func__);
		return -1;
	}
	info.main[INFO_SIZE - 1] = '\0';
	info.sub[INFO_SIZE - 1] = '\0';
	PRINT_ERROR("%s(): %s %s", __func__, info.main, info.sub);
	file->used_size = sprintf_s(file->buf, file->size, "reason [%s], sub [%s]\n",
		info.main, info.sub);
	return 0;
}

static int parse_bsp(void)
{
	if (parse_diaginfo() != 0) {
		PRINT_INFO("%s(): no bsp info need to save", __func__);
		return -1;
	}
	PRINT_INFO("%s(): save bsp info alongside with diaglog", __func__);
	if (parse_printbuf() != 0) {
		PRINT_ERROR("%s(): save additional info failed", __func__);
		return -1;
	}
	return 0;
}

struct module_data diaginfo_prv_data = {
	.name = "diaginfo",
	.level = CPUFREQ_MODULE_LEVEL,
	.on_panic = NULL,
	.init = register_diaginfo,
	.on_storage_ready = parse_bsp,
	.init_done = NULL,
	.last_reset_handler = NULL,
	.exit = NULL,
};
