/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: lc898129 OIS driver
 */

#ifndef _OIS_LC898129_H_
#define _OIS_LC898129_H_

#include <linux/cma.h>
#include <cam_ois_dev.h>

#define UP_DATA_CODE_SIZE          0x0000014c
#define UP_DATA_CODE_CHECKSUM      0x000071d6ac6f9fbf
int32_t lc898129_fw_download(struct cam_ois_ctrl_t *o_ctrl);
#endif
/* _OIS_LC898129_ */