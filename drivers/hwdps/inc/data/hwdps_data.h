/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: This file contains the function definations required for
 *              data list.
 * Create: 2021-05-21
 */

#ifndef HWDPS_DATA_H
#define HWDPS_DATA_H

void hwdps_data_init(void);

void hwdps_data_deinit(void);

void hwdps_data_read_lock(void);

void hwdps_data_read_unlock(void);

void hwdps_data_write_lock(void);

void hwdps_data_write_unlock(void);
#endif
