/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor dw9787 OIS driver
 */

#ifndef _OIS_DW9787_H_
#define _OIS_DW9787_H_

#include <linux/cma.h>
#include <cam_ois_dev.h>

int32_t dw9787_fw_download(struct cam_ois_ctrl_t *o_ctrl);

#endif
/* _OIS_DW9787_H_ */
