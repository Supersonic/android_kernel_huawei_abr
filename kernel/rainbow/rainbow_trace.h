/*
  * process for rainbow trace record irq and task schedule trace log
  *
  * Copyright (c) 2019 Huawei Technologies Co., Ltd.
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
#ifndef _RAINBOW_TRACE_H_
#define _RAINBOW_TRACE_H_

#ifdef CONFIG_RAINBOW_TRACE
#include <linux/sched.h>
#include "rainbow.h"

#define RB_TRACE_IRQ_INFO_SIZE 16
#define RB_TRACE_TASK_INFO_SIZE 40

#define RB_TRACE_BUFFER_PINFO_DATA_SIZE 1

#define RB_TRACE_IRQ_ENTER	0
#define RB_TRACE_IRQ_EXIT	1

#define RB_TRACE_HOOK_ON 1

/* irq */
struct rb_trace_irq_info {
	u64 clock;
	u32 irq;
	u8 dir;
};

struct rb_trace_task_info {
	u64 clock;
	uintptr_t stack;
	u32 pid;
	char comm[TASK_COMM_LEN];
};

struct rb_trace_buffer_info {
	unsigned char *buffer_addr;
	unsigned char *percpu_addr[NR_CPUS];
	unsigned int    percpu_length;
	unsigned int    buffer_size;
};

struct rb_trace_pinfo {
	u32 max_num;
	u32 field_len;
	u32 rear;
	u32 is_full;
	u8 data[RB_TRACE_BUFFER_PINFO_DATA_SIZE];
};

struct rb_trace_mem_node {
	uintptr_t  paddr;
	uintptr_t  vaddr;
	unsigned int    size;
};

enum rb_trace_type {
	TR_IRQ = 0,
	TR_TASK,
	TR_MAX
};

void rb_trace_init(void *paddr_start, void *vaddr_start);
void rb_trace_irq_write(unsigned int dir, unsigned int new_vec);
void rb_trace_task_write(void *next_task);
#endif
#endif
