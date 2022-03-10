/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_dmesg.c
 * Description: rdr printk ram parser
 * Author: 2021-03-27 zhangxun dfx re-design
 */
#include <linux/syscalls.h>
#include <linux/kallsyms.h>

#include "rdr_inner.h"
#include "rdr_storage_api.h"
#include "rdr_storage_device.h"
#include "rdr_module.h"
#include "rdr_file_device.h"

#include "securec.h"

struct rdr_printk_log {
	u64 ts_nsec;		/* timestamp in nanoseconds */
	u16 len;		/* length of entire record */
	u16 text_len;		/* length of text buffer */
	u16 dict_len;		/* length of dictionary buffer */
	u8 facility;		/* syslog facility */
	u8 flags:5;		/* internal record flags */
	u8 level:3;		/* syslog level */
#ifdef CONFIG_PRINTK_CALLER
	u32 caller_id;            /* thread id or processor id */
#endif
	pid_t pid;		                /* task pid */
	char comm[TASK_COMM_LEN];		/* task name */
}
#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
__packed __aligned(4)
#endif
;

static int register_logbuf(void)
{
	struct rdr_logsave_info info;
	struct rdr_logsave_info start_info;
	struct rdr_logsave_info end_info;
	u32 *log_first_idx_addr = NULL;
	u32 *log_last_idx_addr = NULL;

	PRINT_START();

	log_first_idx_addr = (u32 *)kallsyms_lookup_name("log_first_idx");
	log_last_idx_addr = (u32 *)kallsyms_lookup_name("log_next_idx");

	if (!log_first_idx_addr || !log_last_idx_addr) {
		PRINT_ERROR("Unable to find printk ring buffer index by kallsyms!\n");
		return -1;
	}

	info.log_phyaddr       = virt_to_phys(log_buf_addr_get());
	info.log_size          = log_buf_len_get();
	start_info.log_phyaddr = virt_to_phys(log_first_idx_addr);
	start_info.log_size    = sizeof(u32);
	end_info.log_phyaddr   = virt_to_phys(log_last_idx_addr);
	end_info.log_size      = sizeof(u32);

	PRINT_DEBUG("%s(): log_first_idx_addr %p %llx %lu", __func__, log_first_idx_addr,
		virt_to_phys(log_first_idx_addr), sizeof(u32));
	PRINT_DEBUG("%s(): log_last_idx_addr  %p %llx %lu", __func__, log_last_idx_addr,
		virt_to_phys(log_last_idx_addr), sizeof(u32));

	rdr_register_logsave_info("KLOGBUF", &info);
	rdr_register_logsave_info("LOGSTART", &start_info);
	rdr_register_logsave_info("LOGEND", &end_info);

	PRINT_END();
	return 0;
}

static u32 dmesg_binary_parser(char *dmesg, u32 dmesg_len, char *klogbuf, u32 logbuf_len, u32 first_idx, u32 last_idx)
{
	/* stucture member address in while-loop */
	u64 ts;
	u16 text_len;
	u16 msg_len;
	char *text_str_addr = NULL;

	char *logbuf_addr = klogbuf;
	char *logbuf_end = klogbuf + logbuf_len;
	char *curr_idx = logbuf_addr + first_idx;

	/* wrap_cnt is used to break when msg_len is 0. On the first begining, msg_len is 0.
	 * In the end, msg_len keeps to be 0, which doesn't meet the breakout rule of while-loop.
	 * condition: (curr_idx < logbuf_addr + last_idx).
	 * Thus we need to break when msg_len is 0 the second time (wrap_cnt < 2)
	 */
	int wrap_cnt = 0;

	u32 accumulate_len = 0;
	u32 ts_len;
	unsigned long rem_nsec;

	while (curr_idx != logbuf_addr + last_idx && wrap_cnt < 2) { // 2
		ts            = *(u64 *)(curr_idx + offsetof(struct rdr_printk_log, ts_nsec));
		text_len      = *(u16 *)(curr_idx + offsetof(struct rdr_printk_log, text_len));
		msg_len       = *(u16 *)(curr_idx + offsetof(struct rdr_printk_log, len));
		text_str_addr =         (curr_idx + sizeof(struct rdr_printk_log));

		if (curr_idx + msg_len >= logbuf_end) {
			PRINT_ERROR("%s(): src overflow! ACCESS: [%p - %p], VALID: [%p - %p]",
				__func__, curr_idx, curr_idx + msg_len, logbuf_addr, logbuf_end);
			break;
		}

		rem_nsec = do_div(ts, 1000000000); // 1000000000 nsec
		ts_len = sprintf_s(dmesg + accumulate_len, dmesg_len - accumulate_len,
			"[%5lu.%06lu] ", (unsigned long)ts, rem_nsec / 1000); // 1000 for reminder
		if (ts_len == -1)
			break;

		accumulate_len += ts_len;
		if (rdr_safe_copy(dmesg + accumulate_len, dmesg_len - accumulate_len,
			text_str_addr, text_len, klogbuf, logbuf_len) != 0) {
			PRINT_ERROR("%s(): memcpy_s text failed, dst: %p, src: %p, aclen: %u, ts_len:%u, text_len: %hu",
				__func__, dmesg + accumulate_len, text_str_addr, accumulate_len, ts_len, text_len);
			break;
		}

		accumulate_len += text_len;
		if (memcpy_s(dmesg + accumulate_len, dmesg_len - accumulate_len,
			"\r\n", 2) != 0) { // end is 2
			PRINT_ERROR("%s(): memcpy_s endline failed, dst: %p, aclen: %u",
				__func__, dmesg + accumulate_len, accumulate_len);
			break;
		}
		accumulate_len += 2; // end is 2

		if (msg_len == 0) {
			PRINT_INFO("%s(): wrap around to start of klogbuf, idx_offset: %ld",
				__func__, curr_idx - logbuf_addr);
			wrap_cnt++;
			curr_idx = logbuf_addr;
		} else {
			curr_idx += msg_len;
		}
	}
	return accumulate_len;
}

static int extra_dmesg(void)
{
	struct rdr_file *dmesg = NULL;
	struct rdr_region *klogbuf = NULL;
	struct rdr_region *start = NULL;
	struct rdr_region *end = NULL;
	u32 start_idx;
	u32 end_idx;
	u32 dmesg_len;

	klogbuf = rdr_acquire_region("KLOGBUF");
	start = rdr_acquire_region("LOGSTART");
	end = rdr_acquire_region("LOGEND");
	if (klogbuf == NULL || start == NULL || end == NULL) {
		PRINT_ERROR("%s(): aquire region failed", __func__);
		return -1;
	}

	start_idx = *(u32 *)start->data;
	end_idx   = *(u32 *)end->data;
	PRINT_INFO("%s(): start: %u, end: %u", __func__, start_idx, end_idx);

	if (start_idx > klogbuf->size || end_idx > klogbuf->size) {
		PRINT_ERROR("%s(): start or end index is not right!", __func__);
		return -1;
	}

	dmesg_len = klogbuf->size * 2; // 2 of logbuf_len
	dmesg = rdr_add_file("dmesg.txt", dmesg_len);
	if (dmesg == NULL) {
		PRINT_ERROR("%s(): allocate file buffer failed", __func__);
		return -1;
	}

	dmesg->used_size = dmesg_binary_parser(dmesg->buf, dmesg_len, klogbuf->data, klogbuf->size,
		start_idx, end_idx);
	PRINT_INFO("%s(): used size %llu", __func__, dmesg->used_size);
	return 0;
}

struct module_data dmesg_prv_data = {
	.name = "dmesg",
	.level = DMESG_MODULE_LEVEL,
	.on_panic = NULL,
	.init = register_logbuf,
	.on_storage_ready = extra_dmesg,
	.init_done = NULL,
	.last_reset_handler = NULL,
	.exit = NULL,
};
