/*
  * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
  * Description: header file for dp aux driver
  * Author: zhoujiaqiao
  * Create: 2021-03-22
  */

#ifndef __DP_AUX_SWITCH_H__
#define __DP_AUX_SWITCH_H__

#include <linux/kernel.h>

#ifdef CONFIG_HUAWEI_DP_AUX
void dp_aux_switch_enable(bool enable);
#else
static inline void dp_aux_switch_enable(void) {}
#endif /* !CONFIG_DP_AUX_SWITCH */

#endif /* __DP_AUX_SWITCH_H__ */
