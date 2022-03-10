/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * FileName: dmd_upload
 * Description: Define some macros and some structures
 * Revision history:2020-10-14 zhangxun NVE
 */
#ifndef _DMD_UPLOAD_H
#define _DMD_UPLOAD_H

enum {
	DMD_CACHE_LEVEL_L1 = 0,
	DMD_CACHE_LEVEL_L2,
	DMD_CACHE_LEVEL_L3,
	DMD_CACHE_LEVEL_SYSCACHE,
};

enum {
	DMD_CACHE_TYPE_CE = 0,
	DMD_CACHE_TYPE_UE,
};

void report_cache_ecc_inirq(int level, int type, int cpu);

#endif
