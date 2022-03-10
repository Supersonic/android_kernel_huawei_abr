/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vendor dw9781 OIS driver
 */

#ifndef _VENDOR_OIS_DW9781_H_
#define _VENDOR_OIS_DW9781_H_

#include <linux/cma.h>
#include <cam_ois_dev.h>

/* Project Name */
#define PTAURUS                         0x00
#define PVOGUE                          0x01
#define PLION1                          0x02
#define PLION2                          0x03
#define PLION3                          0x04
#define PCOCOPLUS                       0x08 /* 2019. 09. 02 Add Coco plus */
#define PTIANSHAN                       0x0B /* 2019. 11. 27 Add */
#define POWEN                           0x0C /* 2020. 01. 16 Add */
#define PNOLAN                          0x0D /* 2020. 03. 03 Add */
#define PSHUIXIANHUA                    0x0E /* 2020. 11. 10 Add */
#define PMEIGUI                         0x0F /* 2020. 11. 24 Add */

#define DW9781B_SLAVE_ID                (0x54 >> 1)
#define DW9781B_LOGIC_SLAVE_ID          (0xE4 >> 1)
#define DW9781B_LOGIC_RESET_ADDRESS     0xD002
#define DW9781B_LOGIC_RESET_VAL         0x0001


int32_t dw9781_fw_download(struct cam_ois_ctrl_t *o_ctrl);
int dw9781b_ois_write_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint16_t value);

int dw9781b_ois_read_addr16_value16(struct cam_ois_ctrl_t *o_ctrl,
	uint16_t reg, uint16_t *value);

#endif
/* _VENDOR_OIS_DW9781_H_ */
