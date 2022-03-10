/*
 * vpu_freq_scale.h
 *
 * vpu freq scale declaration
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#ifndef _VPU_FREQ_SCALE_H_
#define _VPU_FREQ_SCALE_H_
#include <linux/kobject.h>

bool is_vpu_freq_scale(void);
unsigned long get_vpu_freq_scale(void);
int vpu_create_group(struct kobject *kobj);
void vpu_remove_group(struct kobject *kobj);
#endif
