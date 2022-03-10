/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SENSORS_SYSFS_ALS_H
#define _SENSORS_SYSFS_ALS_H


#include <linux/mtd/hw_nve_interface.h>

#define NV_READ_TAG                    1
 #define NV_WRITE_TAG                   0
#define ALS_CALIDATA_L_R_NUM 175
#define ALS_CALIDATA_L_R_SIZE 12
#define ALS_CALIDATA_NV_NUM 339
#define ALS_CALIDATA_NV_TOTAL_SIZE 72
#define ALS_CALIDATA_NV_ONE_SIZE 24
#define ALS_CALIDATA_NV_ALS1_ADDR ALS_CALIDATA_NV_ONE_SIZE
#define ALS_CALIDATA_NV_ALS2_ADDR (ALS_CALIDATA_NV_ONE_SIZE * 2)
#define ALS_CALIDATA_NV_SIZE 12
#define ALS_CALIDATA_NV_SIZE_WITH_DARK_NOISE_OFFSET 14
#define ALS_UD_CALI_LEN 4
#define ALS_UNDER_TP_RAWDATA_LEN 4
#define ALS_UNDER_TP_RGB_DATA_LEN 16
#define ALS_MMI_PARA_LEN 2
#define ALS_MMI_LUX_MIN_ID 0
#define ALS_MMI_LUX_MIN_VAL 30
#define ALS_MMI_LUX_MAX_ID 1
#define ALS_MMI_LUX_MAX_VAL 320
#define ALS_RES_SIZE 6
#define ALS_SUB_BLOCK_SIZE 13
#define ALS_DBV_SIZE 60
#define ALS_GAMMA_SIZE 32
#define ALS_TP_CALIDATA_NV1_NUM 403
#define ALS_TP_CALIDATA_NV1_SIZE 104
#define ALS_TP_CALIDATA_NV2_NUM 404
#define ALS_TP_CALIDATA_NV2_SIZE 104
#define ALS_TP_CALIDATA_NV3_NUM 405
#define ALS_TP_CALIDATA_NV3_SIZE 104
#define ALS_TP_CALIDATA_NV4_NUM 442
#define ALS_TP_CALIDATA_NV4_SIZE 104
#define ALS_TP_CALIDATA_NV5_NUM 443
#define ALS_TP_CALIDATA_NV5_SIZE 104
#define ALS_TP_CALIDATA_NV6_NUM 444
#define ALS_TP_CALIDATA_NV6_SIZE 104
#define ALS_UNDER_TP_CALDATA_GAMMA_LEN 115

#pragma pack (1)
struct als_under_tp_nv_type_gamma{
    uint32_t sub_block[ALS_SUB_BLOCK_SIZE];
    uint32_t res[ALS_RES_SIZE];
    uint32_t dbv[ALS_DBV_SIZE];
    uint32_t gamma[ALS_GAMMA_SIZE];
};

struct als_under_tp_calidata {
    uint32_t x; /* left_up x-aix */
    uint32_t y; /* left_up y-aix */
    uint32_t width;
    uint32_t length;
    struct als_under_tp_nv_type_gamma nv_type_gamma;
};

typedef struct als_buf_to_hal {
    uint8_t sensor_type;
    uint8_t cmd;
    uint8_t subcmd;
    uint8_t reserved;
    union {
        struct als_under_tp_calidata payload;
        int32_t data[3];
    };
} als_buf_to_hal_t;
#pragma pack()

#define als_ud_nv_size  (sizeof(struct als_under_tp_calidata))
ssize_t als_under_tp_calidata_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count);
ssize_t als_under_tp_calidata_show(struct device *dev,
    struct device_attribute *attr, char *buf);
int als_underscreen_calidata_save(void);
#endif		/* __LINUX_SENSORS_H_INCLUDED */
