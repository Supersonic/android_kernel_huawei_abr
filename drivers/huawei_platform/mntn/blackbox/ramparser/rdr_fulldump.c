/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_fulldump.c
 * Description: rdr fulldump adaptor
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#include "rdr_inner.h"
#include "rdr_storage_api.h"
#include "rdr_module.h"

static int skp_save_kdump_file(void)
{
	PRINT_START();
	PRINT_END();
	return 0;
}

struct module_data fulldump_prv_data = {
	.name = "fulldump",
	.level = FULLDUMP_MODULE_LEVEL,
	.on_panic = NULL,
	.init = NULL,
	.on_storage_ready = NULL,
	.init_done = NULL,
	.last_reset_handler = skp_save_kdump_file,
	.exit = NULL,
};
