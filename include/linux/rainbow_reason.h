/*
  * process for rainbow driver
  *
  * Copyright (c) 2019-2019 Huawei Technologies Co., Ltd.
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

#ifndef _RAINBOW_REASON_H_
#define _RAINBOW_REASON_H_
#include <linux/types.h>

#define RB_MREASON_STR_MAX      32
#define RB_SREASON_STR_MAX      128

#define RB_REASON_STR_VALID     0x56414C44  /* VALD  0x56414C44 */
#define RB_REASON_STR_EMPTY     0

enum mreason_flag_enum {
	RB_M_UINIT = 0,
	RB_M_NORMAL,
	RB_M_APANIC,
	RB_M_BOOTFAIL,
	RB_M_AWDT,
	RB_M_POWERCOLLAPSE,
	RB_M_PRESS6S,
	RB_M_BOOTLOADER,
	RB_M_TZ,
	RB_M_UNKOWN,
};

struct rb_reason_header {
	uint32_t reset_reason;          /* reserve flag */
	uint32_t inner_reset_reason;
	uint32_t reset_type;
	uint32_t mreason_num;
	uint32_t mreason_str_flag;
	uint32_t sreason_str_flag;
	uint32_t attach_info_flag;
	uint32_t emmc_flag;
	char mreason_str[RB_MREASON_STR_MAX];
	char sreason_str[RB_SREASON_STR_MAX];
	char attach_info[RB_SREASON_STR_MAX];
};

void rb_mreason_set(uint32_t reason);
void rb_sreason_set(const char *fmt, ...);
void rb_attach_info_set(const char *fmt, ...);
void rb_kallsyms_set(const char *fmt, unsigned long addr);
void *rb_bl_log_get(unsigned int *size);
#endif
