/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: This file contains the function required for KERNEL fs callback.
 * Create: 2021-05-21
 */

#ifndef HWDPS_FS_CALLBACKS_H
#define HWDPS_FS_CALLBACKS_H

#include <huawei_platform/hwdps/hwdps_fs_hooks.h>

void hwdps_register_fs_callbacks_proxy(void);

void hwdps_unregister_fs_callbacks_proxy(void);

#endif
