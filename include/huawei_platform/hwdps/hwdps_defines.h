/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: This file contains the function required for operations about
 *              some tools.
 * Create: 2021-06-16
 */

#ifndef HWDPS_DEFINES_H
#define HWDPS_DEFINES_H

#include <linux/types.h>

#define HWDPS_ENABLE_FLAG 0x00F0
#define HWDPS_XATTR_ENABLE_FLAG 0x0010 /* need diffence with sdp flag */

enum {
	ERR_MSG_SUCCESS = 0,
	ERR_MSG_OUT_OF_MEMORY = 1,
	ERR_MSG_NULL_PTR = 2,
	ERR_MSG_BAD_PARAM = 3,
	ERR_MSG_LIST_NODE_EXIST = 2001,
	ERR_MSG_LIST_NODE_NOT_EXIST = 2002,
	ERR_MSG_GENERATE_FAIL = 2010,
	ERR_MSG_LIST_EMPTY = 2011,
};

typedef struct {
	pid_t pid;
	uid_t uid;
	uid_t task_uid;
} encrypt_id;

#endif
