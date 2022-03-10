/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor OIS firmware core driver
 */

#include <linux/module.h>
#include <linux/firmware.h>
#include <linux/dma-contiguous.h>
#include <cam_sensor_cmn_header.h>
#include <cam_ois_core.h>
#include <cam_ois_soc.h>
#include <cam_sensor_util.h>
#include <cam_debug_util.h>
#include <cam_res_mgr_api.h>
#include <cam_common_util.h>
#include <cam_packet_util.h>

#include "vendor_ois_firmware.h"
#include "ois_dw9781.h"
#include "ois_dw9787.h"
#include "ois_aw86006.h"
#include "ois_lc898129.h"
#define FW_NAME_SIZE 64

struct fw_func {
	char fw_name[FW_NAME_SIZE];
	int32_t (*fw_download)(struct cam_ois_ctrl_t *o_ctrl);
};

static struct fw_func g_fw_func_map[] = {
	{"dw9781", dw9781_fw_download},
	{"dw9787", dw9787_fw_download},
	{"aw86006", aw86006_fw_download},
	{"lc898129", lc898129_fw_download},
	{"", NULL},
};


/**
 * vendor_ois_fw_download - download ois fw
 * @o_ctrl:     ctrl structure
 *
 * Returns success or failure
 */
int32_t vendor_ois_fw_download(struct cam_ois_ctrl_t *o_ctrl)
{
	int32_t i;

	for (i = 0; i < ARRAY_SIZE(g_fw_func_map); i++) {
		if (strcmp(o_ctrl->ois_name, g_fw_func_map[i].fw_name) == 0) {
			return g_fw_func_map[i].fw_download(o_ctrl);
		}
	}
	return -EINVAL;
}
