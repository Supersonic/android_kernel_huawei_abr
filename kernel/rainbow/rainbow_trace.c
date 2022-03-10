/*
  * process for rainbow trace record irq and task schedule trace log
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

#include "rainbow_trace.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/highmem.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/init.h>
#include <linux/module.h>
#include <soc/qcom/minidump.h>

unsigned int g_rb_t_event_len[TR_MAX] = {
	RB_TRACE_IRQ_INFO_SIZE,
	RB_TRACE_TASK_INFO_SIZE
};
struct rb_trace_buffer_info g_rb_trace_buf[TR_MAX];
struct rb_trace_mem_node g_rb_trace_mem[TR_MAX];
static atomic_t g_rb_trace_hook_on[TR_MAX] = { ATOMIC_INIT(0) };

static int rb_trace_percup_buffer_init(struct rb_trace_pinfo *q,
	unsigned int bytes, unsigned int len)
{
	unsigned int pbuf_max;

	if (q == NULL)
		return -1;

	if (bytes < (sizeof(struct rb_trace_pinfo) + sizeof(u8) * len))
		return -1;

	pbuf_max = bytes - sizeof(struct rb_trace_pinfo);
	/* max_num: records count. */
	q->max_num = pbuf_max / (sizeof(u8) * len);
	q->rear = 0;
	q->is_full = 0;
	q->field_len = len;
	return 0;
}

static void rb_trace_percup_buffer_write(struct rb_trace_pinfo *q,
	u8 *element)
{
	int rear;

	if (q == NULL || element == NULL)
		return;
	if (q->rear >= q->max_num) {
		q->is_full = 1;
		q->rear = 0;
	}

	rear = q->rear++;
	memcpy((void *)&q->data[(long)rear * q->field_len],
		(void *)element, q->field_len * sizeof(u8));
}

static u8 rb_smp_processor_id(void)
{
	return (u8) smp_processor_id();
}

void rb_trace_irq_write(unsigned int dir, unsigned int new_vec)
{
	struct rb_trace_irq_info info;
	u8 cpu;
	struct rb_trace_pinfo *cpu_pinfo = NULL;

	memset(&info, 0, sizeof(info));
	/* hook is not installed. */
	if (!atomic_read(&g_rb_trace_hook_on[TR_IRQ]))
		return;
	cpu = (u8)rb_smp_processor_id();
	info.clock = jiffies_64;
	info.dir = (u8)dir;
	info.irq = (u32)new_vec;

	cpu_pinfo = (struct rb_trace_pinfo *)g_rb_trace_buf[TR_IRQ].percpu_addr[cpu];
	rb_trace_percup_buffer_write(cpu_pinfo, (u8 *)&info);
}
EXPORT_SYMBOL(rb_trace_irq_write);

void rb_trace_task_write(void *next_task)
{
	struct task_struct *task = (struct task_struct *)next_task;
	struct rb_trace_task_info info;
	u8 cpu;
	struct rb_trace_pinfo *cpu_pinfo = NULL;

	if (next_task == NULL)
		return;
	if (!atomic_read(&g_rb_trace_hook_on[TR_TASK]))
		return;
	memset(&info, 0, sizeof(info));
	cpu = (u8)rb_smp_processor_id();
	info.clock = jiffies_64;
	info.pid = (u32)task->pid;
	(void)strncpy(info.comm, task->comm, sizeof(task->comm) - 1);
	info.comm[TASK_COMM_LEN - 1] = '\0';
	info.stack = (uintptr_t)task->stack;

	cpu_pinfo = (struct rb_trace_pinfo *)g_rb_trace_buf[TR_TASK].percpu_addr[cpu];
	rb_trace_percup_buffer_write(cpu_pinfo, (u8 *)&info);
}
EXPORT_SYMBOL(rb_trace_task_write);

static int rb_trace_buffer_init(struct rb_trace_buffer_info *buffer_info,
	enum rb_trace_type mode)
{
	int i;
	int ret;
	u32 cpu_num = num_possible_cpus();
	struct rb_trace_buffer_info *buffer = buffer_info;
	struct rb_trace_pinfo *cpu_pinfo = NULL;

	if (buffer_info == NULL)
		return -1;

	if (mode >= TR_MAX) {
		RB_LOG_ERR("Wrong mode\n");
		return -1;
	}

	buffer->buffer_addr = (unsigned char *)g_rb_trace_mem[mode].vaddr;
	buffer->percpu_length = g_rb_trace_mem[mode].size / cpu_num;
	buffer->buffer_size = g_rb_trace_mem[mode].size;

	for (i = 0; i < TR_MAX; i++) {
		if (i == TR_IRQ)
			g_rb_t_event_len[i] = sizeof(struct rb_trace_irq_info);
		if (i == TR_TASK)
			g_rb_t_event_len[i] = sizeof(struct rb_trace_task_info);
	}

	for (i = 0; i < (int)cpu_num; i++) {
		buffer->percpu_addr[i] =
			(unsigned char *)(g_rb_trace_mem[mode].vaddr +
			(uintptr_t)(i * (buffer->percpu_length)));
		cpu_pinfo = (struct rb_trace_pinfo *)buffer->percpu_addr[i];
		ret = rb_trace_percup_buffer_init(cpu_pinfo,
			buffer->percpu_length, g_rb_t_event_len[mode]);
		if (ret) {
			RB_LOG_ERR("cpu %d ringbuffer init failed\n", i);
			return -1;
		}
	}

	atomic_set(&g_rb_trace_hook_on[mode], RB_TRACE_HOOK_ON);
	return 0;
}

void rb_trace_init(void *paddr_start, void *vaddr_start)
{
	struct md_region md_entry_irq;
	struct md_region md_entry_task;

	if (vaddr_start == NULL || paddr_start == NULL)
		return;

	g_rb_trace_mem[TR_IRQ].paddr = (uintptr_t)paddr_start + RB_TRACE_IRQ_OFFSET;
	g_rb_trace_mem[TR_IRQ].size = RB_TRACE_SECTION_SIZE;
	g_rb_trace_mem[TR_IRQ].vaddr = (uintptr_t)vaddr_start + RB_TRACE_IRQ_OFFSET;
	RB_LOG_ERR("%x,%x\n", g_rb_trace_mem[TR_IRQ].paddr,
		g_rb_trace_mem[TR_IRQ].vaddr);
	strlcpy(md_entry_irq.name, "KIRQTRACE", sizeof(md_entry_irq.name));
	md_entry_irq.virt_addr = g_rb_trace_mem[TR_IRQ].vaddr;
	md_entry_irq.phys_addr = g_rb_trace_mem[TR_IRQ].paddr;
	md_entry_irq.size = RB_TRACE_SECTION_SIZE;

	g_rb_trace_mem[TR_TASK].paddr = (uintptr_t)paddr_start + RB_TRACE_TASK_OFFSET;
	g_rb_trace_mem[TR_TASK].size = RB_TRACE_SECTION_SIZE;
	g_rb_trace_mem[TR_TASK].vaddr = (uintptr_t)vaddr_start + RB_TRACE_TASK_OFFSET;
	RB_LOG_ERR("%x,%x\n", g_rb_trace_mem[TR_TASK].paddr,
		g_rb_trace_mem[TR_TASK].vaddr);
	strlcpy(md_entry_task.name, "KTASKTRACE", sizeof(md_entry_task.name));
	md_entry_task.virt_addr = g_rb_trace_mem[TR_TASK].vaddr;
	md_entry_task.phys_addr = g_rb_trace_mem[TR_TASK].paddr;
	md_entry_task.size = RB_TRACE_SECTION_SIZE;

	rb_trace_buffer_init(&(g_rb_trace_buf[TR_IRQ]), TR_IRQ);
	rb_trace_buffer_init(&(g_rb_trace_buf[TR_TASK]), TR_TASK);

	if (msm_minidump_add_region(&md_entry_irq))
		RB_LOG_ERR("rb_trace:irq trace fail to add minidump\n");

	if (msm_minidump_add_region(&md_entry_task))
		RB_LOG_ERR("rb_trace:task trace fail to add minidump\n");

	RB_LOG_ERR("sucess\n");
}
