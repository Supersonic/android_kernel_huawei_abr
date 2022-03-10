/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * FileName: kernel/drivers/mtd/hisi_nve_whitelist.h
 * Description: define nve whitelist
 * Author: jinguojun
 * Author: Chengxiaoyi
 * Create: 2020-09-28
 */
#ifndef __HW_NVE_WHITELIST__
#define __HW_NVE_WHITELIST__

/* NV IDs to be protected */
static uint32_t nv_num_whitelist[] = { 2, 193, 194, 364 };
/*
  processes that could modify protected NV IDs.
  process name length MUST be less than 16 Bytes.
*/
static char *nv_process_whitelist[] = {"oeminfo_nvm_ser"};

#endif
