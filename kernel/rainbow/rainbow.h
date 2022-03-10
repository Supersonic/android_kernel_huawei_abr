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

#ifndef _RAINBOW_H_
#define _RAINBOW_H_

#include <linux/rainbow_reason.h>
#include <linux/types.h>
#include <linux/printk.h>

#define RB_TRACE_SECTION_SIZE 0x80000           /* 512kb */

#define RB_LOG_STR_MAX          128

#define RB_REGION_LOG_MAGIC_VALID   0x5A5A5A5A
#define RB_REGION_LOG_MAGIC_INVALID 0

#define RB_BL_LOG_SIZE 0x40000

enum rb_addr_offset_enum {
	RB_HEADER_OFFSET = 0,
	RB_PRE_LOG_OFFSET = 0x80000,
	RB_TRACE_IRQ_OFFSET = 0x100000,
	RB_TRACE_TASK_OFFSET = 0x180000,
};

/*
 * Description: rainbow log level switch, close when code upload
 * 2: print ERR/DEBUG level log
 * 1: print only ERR level log
 */
#define RB_LOG_LEVEL 2

enum rb_log_level_enum {
	RB_LOG_INFO_FLAG,
	RB_LOG_ERROR_FLAG,
	RB_LOG_DEBUG_FLAG,
	RB_LOG_UNKNOWN_FLAG,
};

#define RB_LOG_INFO(msg, ...)						\
do {									\
	if (RB_LOG_LEVEL >= RB_LOG_INFO_FLAG)				\
		pr_info("[RB/E]%s: " msg, __func__, ##__VA_ARGS__);	\
} while (0)

#define RB_LOG_ERR(msg, ...)						\
do {									\
	if (RB_LOG_LEVEL >= RB_LOG_ERROR_FLAG)				\
		pr_err("[RB/E]%s: " msg, __func__, ##__VA_ARGS__);	\
} while (0)

#define RB_LOG_DEBUG(msg, ...)						\
do {									\
	if (RB_LOG_LEVEL >= RB_LOG_DEBUG_FLAG)				\
		pr_debug("[RB/D]%s: " msg, __func__, ##__VA_ARGS__);	\
} while (0)

struct rb_region_log_info {
	uint64_t magic;
	uint64_t virt_addr;
	uint64_t phys_addr;
	uint64_t size;
};
struct rb_header {
	uint64_t himntn_data;
	struct rb_reason_header reason_node;
	struct rb_region_log_info kernel_log;
	struct rb_region_log_info tz_log;
};
#endif
