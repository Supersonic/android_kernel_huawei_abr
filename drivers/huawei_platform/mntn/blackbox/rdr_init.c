/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_init.c
 * Description: rdr main entry & stage info indicator and module dispatcher
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#include <linux/module.h>
#include <linux/kthread.h>

#include "rdr_module.h"
#include "rdr_inner.h"
#include "rdr_storage_api.h"
#include "rdr_storage_device.h"
#include "rdr_file_device.h"

#include "securec.h"

static struct system_table g_systable = {
	.reason_type = 0,
	.himntn = 0,
};

extern struct module_data rdr_storage_device_data;
extern struct module_data dmesg_prv_data;
extern struct module_data freq_prv_data;
extern struct module_data fulldump_prv_data;
extern struct module_data rdr_file_device_data;
extern struct module_data diaginfo_prv_data;
struct module_data *g_module_data[] = {
	&rdr_storage_device_data,
	&dmesg_prv_data,
	&freq_prv_data,
	&diaginfo_prv_data,
	&fulldump_prv_data,
	&rdr_file_device_data,
};

static void rdr_module_late_init(void)
{
	int i;
	for (i = 0; i < sizeof(g_module_data) / sizeof(g_module_data[0]); i++) {
		if (g_module_data[i] == NULL)
			continue;
		if (g_module_data[i]->on_storage_ready == NULL)
			continue;
		PRINT_INFO("on_storage_ready: <%s>", g_module_data[i]->name);
		if (g_module_data[i]->on_storage_ready() != 0)
			PRINT_ERROR("on_storage_ready: <%s> failed", g_module_data[i]->name);
	}
}

static void rdr_module_init(void)
{
	int i;
	for (i = 0; i < sizeof(g_module_data) / sizeof(g_module_data[0]); i++) {
		if (g_module_data[i] == NULL)
			continue;
		if (g_module_data[i]->init == NULL)
			continue;
		PRINT_INFO("init: <%s>", g_module_data[i]->name);
		if (g_module_data[i]->init() != 0) {
			PRINT_ERROR("init: <%s> failed", g_module_data[i]->name);
			return;
		}
	}
}

static void rdr_module_init_done(void)
{
	int i;
	for (i = 0; i < sizeof(g_module_data) / sizeof(g_module_data[0]); i++) {
		if (g_module_data[i] == NULL)
			continue;
		if (g_module_data[i]->init_done == NULL)
			continue;
		PRINT_INFO("init_done: <%s>", g_module_data[i]->name);
		g_module_data[i]->init_done();
	}
}

static void rdr_module_exit(void)
{
	int i;
	for (i = 0; i < sizeof(g_module_data) / sizeof(g_module_data[0]); i++) {
		if (g_module_data[i] == NULL)
			continue;
		if (g_module_data[i]->exit == NULL)
			continue;
		PRINT_INFO("exit: <%s>", g_module_data[i]->name);
		g_module_data[i]->exit();
	}
}

static int rdr_init_thread_body(void *arg)
{
	if (rdr_wait_storage_devices() != 0) {
		PRINT_ERROR("rdr_wait_file_devices failed");
		return -1;
	}
	if (rdr_wait_file_devices() != 0)
		PRINT_ERROR("rdr_wait_file_devices failed");
	/* storage device online, do inits that depend on them */
	rdr_module_late_init();
	/* routine done, exit thread */
	rdr_module_init_done();
	return 0;
}

/*
 * Description:    create /proc/data-ready
 * Return:         0:success;-1:fail
 */
static int __init rdr_init(void)
{
	struct task_struct *mntn_main = NULL;

	rdr_module_init();

	mntn_main = kthread_run(rdr_init_thread_body, NULL, "huawei_mntn");
	if (mntn_main == NULL) {
		PRINT_ERROR("create mntn init thread faild\n");
		return -1;
	}

	return 0;
}
module_init(rdr_init);

static void __exit rdr_exit(void)
{
	rdr_module_exit();
}
module_exit(rdr_exit);

static int early_parse_himntn_cmdline(char *p)
{
	int ret;

	if (p == NULL)
		return -1;

	ret = sscanf_s(p, "%llx", &g_systable.himntn);
	if (ret != 1) {
		PRINT_ERROR("sscanf parse error\n");
		return -1;
	}

	PRINT_INFO("g_himntn_value is 0x%llx\n", g_systable.himntn);
	return 0;
}
early_param("HIMNTN", early_parse_himntn_cmdline);
