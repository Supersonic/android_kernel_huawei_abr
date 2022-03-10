/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor OIS firmware core driver
 */

#ifndef _VENDOR_OIS_FIRMWARE_H_
#define _VENDOR_OIS_FIRMWARE_H_

#include <linux/cma.h>
#include <cam_ois_dev.h>

int32_t vendor_ois_fw_download(struct cam_ois_ctrl_t *o_ctrl);

#endif
/* _VENDOR_OIS_FIRMWARE_H_ */
