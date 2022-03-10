/* Copyright (c) 2013-2014, 2017 The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/rwsem.h>
#include <linux/string.h>
#include <securec.h>
#include "sensors_sysfs_als.h"
#include "xhub_router/xhub_pm.h"
#include <linux/mtd/hw_nve_interface.h>
#include <apsensor_channel/ap_sensor_route.h>
#include <apsensor_channel/ap_sensor.h>
#include "sensors_sysfs.h"

#define MAX_STR_SIZE 1024
struct als_under_tp_calidata als_show_calidata;
struct als_under_tp_calidata als_store_calidata;

struct hw_nve_info_user rd_user_info;
struct hw_nve_info_user wr_user_info;

int read_calibrate_data_from_nv(int nv_number, int nv_size,
                                const char *nv_name)
{
    int ret;

    if (!nv_name) {
        pr_err("%s para err\n", __func__);
        return -1;
    }
    pr_info("read_calibrate_data_from_nv %d in!!\n", nv_number);
    memset(&rd_user_info, 0, sizeof(rd_user_info));
    rd_user_info.nv_operation = NV_READ_TAG;
    rd_user_info.nv_number = nv_number;
    rd_user_info.valid_size = nv_size;
    strncpy(rd_user_info.nv_name, nv_name,
                sizeof(rd_user_info.nv_name));
    rd_user_info.nv_name[sizeof(rd_user_info.nv_name) - 1] = '\0';
    ret = hw_nve_direct_access(&rd_user_info);
    if (ret != 0) {
        pr_err("read_calibrate_data_from_nv %d error(%d)\n", nv_number, ret);
        return -1;
    }
    return 0;
}

int write_calibrate_data_to_nv(int nv_number, int nv_size,
    const char *nv_name, const char *temp)
{
    int ret = 0;
    unsigned int nvdata_len;

    if (!nv_name || !temp) {
        pr_err("write_calibrate_data_to_nv INPUT PARA NULL\n");
        return -1;
    }
    pr_info("write_calibrate_data_to_nv %d in!!\n", nv_number);
    memset(&wr_user_info, 0, sizeof(wr_user_info));
    wr_user_info.nv_operation = NV_WRITE_TAG;
    wr_user_info.nv_number = nv_number;
    wr_user_info.valid_size = nv_size;
    strncpy(wr_user_info.nv_name, nv_name,
    sizeof(wr_user_info.nv_name));
    wr_user_info.nv_name[sizeof(wr_user_info.nv_name) - 1] = '\0';
    nvdata_len = sizeof(wr_user_info.nv_data);
    memcpy(wr_user_info.nv_data, temp,
    nvdata_len < nv_size ? nvdata_len : nv_size);
    ret = hw_nve_direct_access(&wr_user_info);
    if (ret != 0) {
        pr_err("write_calibrate_data_to_nv %d error(%d)\n", nv_number, ret);
        return -1;
    }
    return -1;
}


 /* read underscreen als nv data */
int als_under_tp_nv_read()
{
    uint8_t *buf = NULL;
    size_t cal_data_left = sizeof(struct als_under_tp_calidata);
    size_t read_len;

    pr_info("%s enter\n", __func__);

    buf = (uint8_t *)(&als_show_calidata);

    if (cal_data_left > 0) {
        read_len = ((cal_data_left > ALS_TP_CALIDATA_NV1_SIZE) ?
                        ALS_TP_CALIDATA_NV1_SIZE : cal_data_left);
        if (read_calibrate_data_from_nv(ALS_TP_CALIDATA_NV1_NUM, read_len,
        "ALSTP1"))
            return -1;
        if (memcpy_s(buf, read_len, rd_user_info.nv_data, read_len) != EOK)
            return -1;
        buf += read_len;
        cal_data_left -= read_len;
    }

    if (cal_data_left > 0) {
        read_len = ((cal_data_left > ALS_TP_CALIDATA_NV2_SIZE) ?
                         ALS_TP_CALIDATA_NV2_SIZE : cal_data_left);
        if (read_calibrate_data_from_nv(ALS_TP_CALIDATA_NV2_NUM, read_len,
        "ALSTP2"))
            return -1;
        if (memcpy_s(buf, read_len, rd_user_info.nv_data, read_len) != EOK)
            return -1;
        buf += read_len;
        cal_data_left -= read_len;
    }

    if (cal_data_left > 0) {
        read_len = (cal_data_left > ALS_TP_CALIDATA_NV3_SIZE) ?
                         ALS_TP_CALIDATA_NV3_SIZE : cal_data_left;

        if (read_calibrate_data_from_nv(ALS_TP_CALIDATA_NV3_NUM, read_len,
        "ALSTP3"))
            return -1;
        if (memcpy_s(buf, read_len, rd_user_info.nv_data, read_len) != EOK)
            return -1;
        buf += read_len;
        cal_data_left -= read_len;
    }
    if (cal_data_left > 0) {
        read_len = (cal_data_left > ALS_TP_CALIDATA_NV4_SIZE) ?
                         ALS_TP_CALIDATA_NV4_SIZE : cal_data_left;
        if (read_calibrate_data_from_nv(ALS_TP_CALIDATA_NV4_NUM, read_len,
        "ALS_CALIDATA1"))
            return -1;
        if (memcpy_s(buf, read_len, rd_user_info.nv_data, read_len) != EOK)
            return -1;
        buf += read_len;
        cal_data_left -= read_len;
    }

    if (cal_data_left > 0) {
         read_len = (cal_data_left > ALS_TP_CALIDATA_NV5_SIZE) ?
             ALS_TP_CALIDATA_NV5_SIZE : cal_data_left;
         if (read_calibrate_data_from_nv(ALS_TP_CALIDATA_NV5_NUM, read_len,
         "ALS_CALIDATA2"))
             return -1;
         if (memcpy_s(buf, read_len, rd_user_info.nv_data, read_len) != EOK)
             return -1;
         buf += read_len;
         cal_data_left -= read_len;
    }

    if (cal_data_left > 0) {
         read_len = (cal_data_left > ALS_TP_CALIDATA_NV6_SIZE) ?
                          sizeof(struct als_under_tp_calidata) -
                          ALS_TP_CALIDATA_NV1_SIZE * 5 : cal_data_left;
         if (read_calibrate_data_from_nv(ALS_TP_CALIDATA_NV6_NUM, read_len,
         "ALS_CALIDATA3"))
             return -1;
         if (memcpy_s(buf, read_len, rd_user_info.nv_data, read_len) != EOK)
             return -1;
         buf += read_len;
         cal_data_left -= read_len;
    }

    pr_info("%s: x = %d, y = %d, width = %d, len = %d\n", __func__,
    als_show_calidata.x, als_show_calidata.y,
    als_show_calidata.width,
    als_show_calidata.length);

    return 0;
}


int als_underscreen_calidata_save(void)
{
    if (write_calibrate_data_to_nv(ALS_TP_CALIDATA_NV1_NUM,
            ALS_TP_CALIDATA_NV1_SIZE,
            "ALSTP1", ((char *)&(als_store_calidata)))) {
        pr_err("%s: write_calidata_to_nv1 fail\n",
        __func__);
        return -1;
    }
    if (write_calibrate_data_to_nv(ALS_TP_CALIDATA_NV2_NUM,
        ALS_TP_CALIDATA_NV2_SIZE,
        "ALSTP2",
        ((char *)(&(als_store_calidata)) +
        ALS_TP_CALIDATA_NV1_SIZE))) {
        pr_err("%s: write_calidata_to_nv2 fail\n",
        __func__);
        return -1;
    }
    if (write_calibrate_data_to_nv(ALS_TP_CALIDATA_NV3_NUM,
            ALS_TP_CALIDATA_NV3_SIZE,
            "ALSTP3",
            ((char *)(&(als_store_calidata)) +
            ALS_TP_CALIDATA_NV1_SIZE +
            ALS_TP_CALIDATA_NV2_SIZE))) {
        pr_err("%s: write_calidata_to_nv3 fail\n",
        __func__);
        return -1;
    }
    if (write_calibrate_data_to_nv(ALS_TP_CALIDATA_NV4_NUM,
            ALS_TP_CALIDATA_NV4_SIZE,
            "ALS_CALIDATA1",
            (char *)&(als_store_calidata) +
            (ALS_TP_CALIDATA_NV4_SIZE * 3))) {
        pr_err("%s: write_calidata_to_nv4 fail\n",__func__);
        return -1;
    }
    if (write_calibrate_data_to_nv(ALS_TP_CALIDATA_NV5_NUM,
            ALS_TP_CALIDATA_NV5_SIZE,
            "ALS_CALIDATA2",
            (char *)(&(als_store_calidata)) +
            (ALS_TP_CALIDATA_NV4_SIZE * 4))) {
        pr_err("%s: write_calidata_to_nv5 fail\n",__func__);
        return -1;
    }
    if (write_calibrate_data_to_nv(ALS_TP_CALIDATA_NV6_NUM,
            ALS_TP_CALIDATA_NV6_SIZE,
            "ALS_CALIDATA3",
            (char *)(&(als_store_calidata)) +
            (ALS_TP_CALIDATA_NV4_SIZE * 4))) {
        pr_err("%s: write_calidata_to_nv6 fail\n",__func__);
        return -1;
    }

    pr_info("als_underscreen_calidata_save success\n");
    return 0;
}

void send_als_save_data_request()
{
    als_buf_to_hal_t als_buf;
    int ret;

    als_buf.sensor_type = SENSOR_TYPE_SENSOR_NODE;
    als_buf.cmd = TAG_ALS;
    als_buf.subcmd = SEND_UNDERSCREEN_CALIDATA;
    ret = memcpy_s(&als_buf.payload, sizeof(struct als_under_tp_calidata), &als_store_calidata, sizeof(struct als_under_tp_calidata));
    if (ret < 0)
        pr_err("memcpy fail\n");
    pr_info("send_als_save_data_request payload len%d als_buf %d\n", sizeof(als_buf.payload),sizeof(als_buf));
    ap_sensor_route_write((char *)&als_buf, (sizeof(als_buf.payload) + PM_IOCTL_PKG_HEADER));
}

ssize_t als_under_tp_calidata_store_sub_nv_gamma(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int i;
    int ret;
    uint32_t als_undertp_calidata[ALS_UNDER_TP_CALDATA_GAMMA_LEN] = { 0 };
    pr_info("als_under_tp_calidata_store_sub_nv_gamma count %d\n", count);
    if (memcpy_s(als_undertp_calidata, sizeof(als_undertp_calidata),
        buf, count) != EOK)
        return -1;
    als_store_calidata.x = (uint32_t)als_undertp_calidata[0];
    als_store_calidata.y = (uint32_t)als_undertp_calidata[1];
    als_store_calidata.width = (uint32_t)als_undertp_calidata[2];
    als_store_calidata.length = (uint32_t)als_undertp_calidata[3];
    pr_info("attr_als_under_tp_calidata_store: x = %d, y = %d, width = %d, len = %d\n",
                als_store_calidata.x,
                als_store_calidata.y,
                als_store_calidata.width,
                als_store_calidata.length);

    /* start buf 4 size minus 2 */
    if (memcpy_s(&als_store_calidata.nv_type_gamma,
       sizeof(struct als_under_tp_nv_type_gamma),
        &als_undertp_calidata[4],
        (ALS_UNDER_TP_CALDATA_GAMMA_LEN - 4) * sizeof(uint32_t)) != EOK)
        pr_err("%s: memcpy_s a error\n", __func__);

    for (i = 0; i < ALS_RES_SIZE; i++)
        pr_info("attr_als_under_tp_calidata_store: als_under_tp_cal_data.res[%d] = %d\n",
                     i, als_store_calidata.nv_type_gamma.res[i]);
    for (i = 0; i < ALS_SUB_BLOCK_SIZE; i++)
        pr_info("attr_als_under_tp_calidata_store: als_under_tp_cal_data.sub_block[%d] = %d\n",
           i, als_store_calidata.nv_type_gamma.sub_block[i]);
    for (i = 0; i < ALS_DBV_SIZE; i++)
        pr_info("attr_als_under_tp_calidata_store: als_under_tp_cal_data.dbv[%d] = %d\n",
            i, als_store_calidata.nv_type_gamma.dbv[i]);
    for (i = 0; i < ALS_GAMMA_SIZE; i++)
        pr_info("attr_als_under_tp_calidata_store: als_under_tp_cal_data.gamma[%d] = %d\n",
            i, als_store_calidata.nv_type_gamma.gamma[i]);

    ret = als_underscreen_calidata_save();
    if (ret != 0)
        send_als_save_data_request();
    return count;
}


ssize_t als_under_tp_calidata_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;

    if (!buf || (count != ALS_UNDER_TP_CALDATA_GAMMA_LEN * sizeof(int)))
        pr_err("%s: buf NULL or count is %zu\n", __func__, count);

    ret = als_under_tp_calidata_store_sub_nv_gamma(dev,attr,buf,count);
    return ret;
}

ssize_t als_under_tp_calidata_show_cct(char *buf)
{
    int ret;
    int i, size;

    size = ARRAY_SIZE(als_show_calidata.nv_type_gamma.sub_block);
    for (i = 0; i < size; i++) {
        ret = snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%s,%d",
        buf, als_show_calidata.nv_type_gamma.sub_block[i]);
        if (ret <= 0) {
            pr_info("%s: write sub_block[%d] to buf fail\n", __func__, i);
            return -1;
        }
    }
    size = ARRAY_SIZE(als_show_calidata.nv_type_gamma.res);
    for (i = 0; i < size; i++) {
        ret = snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%s,%d",
        buf, als_show_calidata.nv_type_gamma.res[i]);
        if (ret <= 0) {
            pr_info("%s: write res[%d] to buf fail\n", __func__, i);
            return -1;
        }
    }
    size = ARRAY_SIZE(als_show_calidata.nv_type_gamma.dbv);
    for (i = 0; i < size; i++) {
        ret = snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%s,%d",
        buf, als_show_calidata.nv_type_gamma.dbv[i]);
        if (ret <= 0) {
            pr_info("%s: write dbv[%d] to buf fail\n", __func__, i);
            return -1;
        }
    }
    size = ARRAY_SIZE(als_show_calidata.nv_type_gamma.gamma);
    for (i = 0; i < size; i++) {
        ret = snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%s,%d",
        buf, als_show_calidata.nv_type_gamma.gamma[i]);
        if (ret <= 0) {
            pr_info("%s: write gamma[%d] to buf fail\n", __func__, i);
            return -1;
        }
    }
    return ret;
}

ssize_t als_under_tp_calidata_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    int ret;

    if (!dev || !attr || !buf)
        return -1;

    ret = als_under_tp_nv_read();
    if (ret != 0) {
        pr_info("%s: read under screen als fail\n", __func__);
        return -1;
    }
    ret = snprintf_s(buf, MAX_STR_SIZE, MAX_STR_SIZE - 1, "%d,%d,%d,%d",
    als_show_calidata.x,
    als_show_calidata.y,
    als_show_calidata.width,
    als_show_calidata.length);
    if (ret <= 0) {
        pr_info("%s: write data to buf fail\n", __func__);
        return -1;
    }
    ret = als_under_tp_calidata_show_cct(buf);

    return (ssize_t)ret;
}
static void als_enable_rgb_data(unsigned long value, uint8_t subcmd)
{
    als_buf_to_hal_t als_buf;

    als_buf.sensor_type = SENSOR_TYPE_SENSOR_NODE;
    als_buf.cmd = TAG_ALS;
    als_buf.subcmd = subcmd;
    als_buf.data[0] = value;

    pr_info("%s: rgb_data %d (1:ON,0:OFF)\n", __func__, value);
    ap_sensor_route_write((char *)&als_buf, (sizeof(als_buf.data) + PM_IOCTL_PKG_HEADER));
}

ssize_t attr_als_rgb_enable_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned long value = 0;
    uint8_t subcmd = ENABLE_RAW_DATA;

    if (kstrtoul(buf, 10, &value)) {
        pr_err("%s: enable val %lu invalid", __func__, value);
    }
    pr_info("%s: send_rgb_enable val %lu\n", __func__, value);
    if (value != 0 && value != 1) {
       return count;
    }
    als_enable_rgb_data(value, subcmd);
    return count;
}

ssize_t attr_als_rgb_debug_enable_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t count)
{
    unsigned long value = 0;
    uint8_t subcmd = ENABLE_DEBUG_RAW_DATA;

    if (kstrtoul(buf, 10, &value)) {
        pr_err("%s: enable val %lu invalid", __func__, value);
    }
    pr_info("%s: send_als_rgb_debug_enable val %lu\n", __func__, value);
    if (value != 0 && value != 1) {
       return count;
    }
    als_enable_rgb_data(value, subcmd);
    return count;
}

