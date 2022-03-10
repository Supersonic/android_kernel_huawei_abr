/*
 * dp_source_switch.h
 *
 * dp source switch interface
 *
 * Copyright (c) 2021-2021 Huawei Technologies Co., Ltd.
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

#ifndef __DP_SOURCE_SWITCH_H__
#define __DP_SOURCE_SWITCH_H__

#include <linux/types.h>

enum dp_source_mode {
	DIFF_SOURCE = 0,
	SAME_SOURCE,
};

#ifdef CONFIG_HUAWEI_DP_SOURCE
int get_current_dp_source_mode(void);
#else
static inline int get_current_dp_source_mode(void)
{
	return SAME_SOURCE;
}
#endif /* CONFIG_HW_DP_SOURCE */

#endif /* __DP_SOURCE_SWITCH_H__ */

