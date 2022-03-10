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

#include "rainbow.h"
#include <linux/string.h>
#include <linux/printk.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <asm/stacktrace.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/of_fdt.h>

#ifdef CONFIG_RAINBOW_TRACE
#include "rainbow_trace.h"
#endif

struct rb_header *g_rb_header;
static struct rb_header *g_rb_header_paddr;

static void rb_header_init(void)
{
	struct device_node *rb_dev_node = NULL;
	unsigned long rb_header_dts_addr;
	const u32 *rb_dts_basep = NULL;
	unsigned long rb_header_dts_size = 0;

	rb_dev_node = of_find_compatible_node(NULL,
		NULL, "rainbow_mem");
	if (rb_dev_node == 0) {
		RB_LOG_ERR("dtsi find fail\n");
		return;
	}
	rb_dts_basep = of_get_address(rb_dev_node, 0,
		(u64 *)&rb_header_dts_size, NULL);
	if (rb_dts_basep == 0) {
		RB_LOG_ERR("Getting address failed\n");
		return;
	}
	rb_header_dts_addr = (unsigned long)of_translate_address(rb_dev_node,
		rb_dts_basep);
	if (rb_header_dts_addr == 0) {
		RB_LOG_ERR("wrong address or size\n");
		return;
	}
	g_rb_header_paddr = (struct rb_header *)rb_header_dts_addr;
	RB_LOG_INFO("phy addr %x, size is %x\n",
		rb_header_dts_addr, rb_header_dts_size);
#ifdef CONFIG_ARM
	g_rb_header = (struct rb_header *)ioremap_nocache(rb_header_dts_addr,
		rb_header_dts_size);
#else
	g_rb_header = (struct rb_header *)ioremap_wc(rb_header_dts_addr,
		rb_header_dts_size);
#endif
	if (g_rb_header == NULL)
		RB_LOG_ERR("fail\n");
	else
		RB_LOG_INFO("addr %x ioremap to %x, size is %x\n",
			rb_header_dts_addr, g_rb_header, rb_header_dts_size);
}

void *rb_bl_log_get(unsigned int *size)
{
	void *addr = NULL;
	if (g_rb_header == NULL || size == NULL) {
		RB_LOG_ERR("rb_bl_log_get: addr or size or g_rb_header is null\n");
		return NULL;
	}
	RB_LOG_DEBUG("g_rb_header:%x\n", g_rb_header);
	addr = (void *)((char *)g_rb_header + RB_PRE_LOG_OFFSET);
	*size = RB_BL_LOG_SIZE;
	return addr;
}
EXPORT_SYMBOL(rb_bl_log_get);

static void rb_header_exit(void)
{
	if (g_rb_header != NULL)
		iounmap((void *)g_rb_header);
}

void rb_mreason_set(uint32_t reason)
{
	struct rb_reason_header *reason_node_local = NULL;

	if (g_rb_header == NULL) {
		RB_LOG_ERR("rb_mreason_set g_rb_header is null\n");
		return;
	}

	if (reason <= RB_M_UINIT || reason > RB_M_UNKOWN) {
		RB_LOG_ERR("reason is invalid, %d\n", reason);
		return;
	}
	reason_node_local = &(g_rb_header->reason_node);
	RB_LOG_ERR("mreason_num is %d\n", reason_node_local->mreason_num);
	if (reason_node_local->mreason_num == RB_M_UINIT) {
		reason_node_local->mreason_num = reason;
		RB_LOG_ERR("mreason_num set to %d\n", reason);
	}
	RB_LOG_DEBUG("end\n");
}
EXPORT_SYMBOL(rb_mreason_set);

static void rb_sreason_str_set(char *sreason_info)
{
	unsigned int sreason_info_size = strlen(sreason_info);
	struct rb_reason_header *reason_node_local = NULL;

	if (sreason_info == NULL || g_rb_header == NULL)
		return;

	reason_node_local = &(g_rb_header->reason_node);
	RB_LOG_DEBUG("sreason_str set start\n");
	if (reason_node_local->sreason_str_flag  == RB_REASON_STR_VALID) {
		RB_LOG_DEBUG("sreason_str have set, so skip\n");
		return;
	}
#ifdef CONFIG_ARM64
	if (sreason_info_size >= RB_SREASON_STR_MAX) {
		memcpy_toio(reason_node_local->sreason_str,
			sreason_info, RB_SREASON_STR_MAX);
		reason_node_local->sreason_str[RB_SREASON_STR_MAX - 1] = '\0';
	} else {
		memcpy_toio(reason_node_local->sreason_str,
			sreason_info, sreason_info_size);
		reason_node_local->sreason_str[sreason_info_size] = '\0';
	}
#else
	strlcpy(reason_node_local->sreason_str,
		sreason_info, RB_SREASON_STR_MAX);
#endif
	reason_node_local->sreason_str_flag = RB_REASON_STR_VALID;
	RB_LOG_ERR("sreason_str set %s end\n",
		reason_node_local->sreason_str);
}

void rb_sreason_set(const char *fmt, ...)
{
	char buf[RB_SREASON_STR_MAX] = {0};
	int err;
	va_list ap;

	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	err = vscnprintf(buf, RB_SREASON_STR_MAX, fmt, ap);
	va_end(ap);
	if (err < 0) {
		RB_LOG_DEBUG("vscnprintf set fail\n");
		return;
	}
	rb_sreason_str_set(buf);
}
EXPORT_SYMBOL(rb_sreason_set);

static void rb_attach_info_str_set(char *attach_info)
{
	unsigned int attach_info_size = strlen(attach_info);
	struct rb_reason_header *reason_node_local = NULL;

	if (attach_info == NULL || g_rb_header == NULL)
		return;

	reason_node_local = &(g_rb_header->reason_node);
	RB_LOG_DEBUG("attach_info set start\n");
	if (reason_node_local->attach_info_flag == RB_REASON_STR_VALID) {
		RB_LOG_ERR("attach_info have set,so skip\n");
		return;
	}

#ifdef CONFIG_ARM64
	if (attach_info_size >= RB_SREASON_STR_MAX) {
		memcpy_toio(reason_node_local->attach_info,
			attach_info, RB_SREASON_STR_MAX);
		reason_node_local->attach_info[RB_SREASON_STR_MAX - 1] = '\0';
	} else {
		memcpy_toio(reason_node_local->attach_info,
			attach_info, attach_info_size);
		reason_node_local->attach_info[attach_info_size] = '\0';
	}
#else
	strlcpy(reason_node_local->attach_info,
		attach_info, RB_SREASON_STR_MAX);
#endif
	reason_node_local->attach_info_flag = RB_REASON_STR_VALID;
	RB_LOG_ERR("attach_info set %s end\n", reason_node_local->attach_info);
}

void rb_attach_info_set(const char *fmt, ...)
{
	char attach_info_buffer[RB_SREASON_STR_MAX] = {0};
	int err;
	va_list ap;

	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	err = vsnprintf(attach_info_buffer, RB_SREASON_STR_MAX, fmt, ap);
	va_end(ap);
	if (err < 0) {
		RB_LOG_ERR("fail\n");
		return;
	}

	RB_LOG_ERR("rb_attach_info_set %s.", attach_info_buffer);
	rb_attach_info_str_set(attach_info_buffer);
}
EXPORT_SYMBOL(rb_attach_info_set);

void rb_kallsyms_set(const char *fmt, unsigned long addr)
{
	int err;
	char attach_info_buffer[RB_SREASON_STR_MAX] = {0};
	char kallsyms_buffer[KSYM_SYMBOL_LEN] = {0};

	if (fmt == NULL)
		return;

	// __check_printsym_format(fmt, "");
	sprint_symbol(kallsyms_buffer,
		(unsigned long)__builtin_extract_return_addr((void *)addr));
	err = snprintf(attach_info_buffer,
		RB_SREASON_STR_MAX, fmt, kallsyms_buffer);
	if (err < 0) {
		RB_LOG_ERR("fail\n");
		return;
	}
	RB_LOG_ERR("%s\n", attach_info_buffer);
	rb_attach_info_str_set(attach_info_buffer);
}
EXPORT_SYMBOL(rb_kallsyms_set);

static int rb_apanic_handler(struct notifier_block *this,
	unsigned long event, void *ptr)
{
	struct rb_reason_header *reason_node_local = NULL;

	RB_LOG_ERR("rb_apanic_handler enters\n");
	if (g_rb_header == NULL) {
		RB_LOG_ERR("rb_apanic_handler g_rb_header is null\n");
		return NOTIFY_DONE;
	}
	reason_node_local = &(g_rb_header->reason_node);
#ifdef CONFIG_PREEMPT
	/* Ensure that cond_resched() won't try to preempt anybody */
	preempt_count_add(PREEMPT_DISABLE_OFFSET);
#endif
#ifdef CONFIG_RAINBOW_REASON
	rb_mreason_set(RB_M_APANIC);
	if (reason_node_local->sreason_str_flag == RB_REASON_STR_VALID)
		RB_LOG_ERR("sreason:%s\n", reason_node_local->sreason_str);

	if (reason_node_local->attach_info_flag == RB_REASON_STR_VALID)
		RB_LOG_ERR("attach_info:%s\n", reason_node_local->attach_info);
#endif
#ifdef CONFIG_PREEMPT
	preempt_count_sub(PREEMPT_DISABLE_OFFSET);
#endif
	RB_LOG_ERR("rb_apanic_handler end\n");
	return NOTIFY_DONE;
}

static struct notifier_block rb_panic_event_nb = {
	.notifier_call  = rb_apanic_handler,
	.priority = INT_MAX,
};

static void rb_reason_register_apanic_notifier(void)
{
	atomic_notifier_chain_register(&panic_notifier_list,
		&rb_panic_event_nb);
}

static void rb_kernel_log_buf_reg(void)
{
	char *log_bufp = NULL;
	uint32_t log_buf_len = 0;
	struct rb_region_log_info *log_info = NULL;

	log_bufp = log_buf_addr_get();
	log_buf_len = log_buf_len_get();
	if (!log_bufp || !log_buf_len) {
		RB_LOG_ERR("Unable to find log_buf or log_size by kallsyms\n");
		return;
	}

	if (g_rb_header == NULL) {
		RB_LOG_ERR("rb_header map error\n");
		return;
	}
	log_info = &(g_rb_header->kernel_log);
	log_info->virt_addr = (uintptr_t)log_bufp;
	log_info->phys_addr = virt_to_phys(log_bufp);
	log_info->size = log_buf_len;
	log_info->magic = RB_REGION_LOG_MAGIC_VALID;
}

static int __init rainbow_init(void)
{
	rb_header_init();
	rb_reason_register_apanic_notifier();
#ifdef CONFIG_RAINBOW_TRACE
	rb_trace_init(g_rb_header_paddr, g_rb_header);
#endif
	rb_kernel_log_buf_reg();
	return 0;
}

static void __exit rainbow_exit(void)
{
	rb_header_exit();
}

module_init(rainbow_init);
module_exit(rainbow_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("process for rainbow driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
