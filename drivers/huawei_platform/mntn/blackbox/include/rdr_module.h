/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_module.h
 * Description: rdr_module types
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#ifndef RDR_MODULE_H
#define RDR_MODULE_H

struct system_table {
	u64 reason_type;
	u64 himntn;
};

struct module_data {
	char *name;
	unsigned char level;
	/* stages */
	int (*on_panic)(void);
	int (*init)(void);
	int (*on_storage_ready)(void);
	void (*init_done)(void);
	int (*last_reset_handler)(void);
	void (*exit)(void);
};

enum {
	/* after this level, all module can uses rdr storage */
	RDR_STORAGE_DEVICE_LEVEL, /* this level is fixed, think through when multi-threading */
	DMESG_MODULE_LEVEL,
	CPUFREQ_MODULE_LEVEL,
	FULLDUMP_MODULE_LEVEL,
	RDR_FILE_DEVICE_LEVEL, /* save parsed memory to file-system */
};

#endif