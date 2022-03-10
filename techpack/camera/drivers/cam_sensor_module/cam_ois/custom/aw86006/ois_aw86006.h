/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor AW86006 OIS driver
 */

#ifndef _OIS_AW86006_H_
#define _OIS_AW86006_H_

#include <linux/cma.h>
#include <cam_ois_dev.h>

int32_t aw86006_fw_download(struct cam_ois_ctrl_t *o_ctrl);

#endif
/* _OIS_AW86006_H_ */
