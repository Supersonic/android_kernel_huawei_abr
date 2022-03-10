/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Define about limits.
 * Create: 2021-05-21
 */

#ifndef HWDPS_LIMITS_H
#define HWDPS_LIMITS_H

#ifdef __KERNEL__
#include <linux/limits.h>
#else
#include <sys/limits.h>
#endif

#ifndef HWDPS_POLICY_RULESET_MIN
#define HWDPS_POLICY_RULESET_MIN 10
#endif

#ifndef HWDPS_POLICY_RULESET_MAX
#define HWDPS_POLICY_RULESET_MAX 256
#endif

#ifndef HWDPS_POLICY_PACKAGE_MAX
#define HWDPS_POLICY_PACKAGE_MAX 8192 /* to avoid app_policy_len reverse */
#endif

#endif
