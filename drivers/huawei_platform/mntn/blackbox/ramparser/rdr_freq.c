/*
 * Copyright (C) Huawei technologies Co., Ltd All rights reserved.
 * Filename: rdr_freq.c
 * Description: rdr freq ram parser
 * Author: 2021-04-15 zhangxun
 */
#include <linux/syscalls.h>
#include <linux/kallsyms.h>

#include <linux/cpufreq.h>
#include <linux/slab.h>
#include <linux/sched/clock.h>

#include "rdr_inner.h"
#include "rdr_storage_api.h"
#include "rdr_storage_device.h"
#include "rdr_module.h"
#include "rdr_file_device.h"

#include "securec.h"

struct freq_domain_log {
	u64 ts;
	u32 index;
	u32 freq;
};

/************* data structure **************/
#define MAX_QUEUE_SIZE 20
struct trace_queue {
	struct freq_domain_log data[MAX_QUEUE_SIZE];
	u32 front;
	u32 rear;
	u32 round;
};
struct freq_trace {
	struct trace_queue queue[3]; // 3 domains
};

static int init_queue(struct trace_queue *q)
{
	q->front = 0;
	q->rear = 0;
	q->round = 0;
	PRINT_DEBUG("%s(): front %u rear %u", __func__, q->front, q->rear);
	return 0;
}

static int enqueue(struct trace_queue *q, struct freq_domain_log *e)
{
	if ((q->rear + 1) % MAX_QUEUE_SIZE == q->front) {
		q->front = (q->front + 1) % MAX_QUEUE_SIZE;
	}
	if (memcpy_s(&(q->data[q->rear]), sizeof(struct freq_domain_log), e,
		sizeof(struct freq_domain_log)) != 0) {
		PRINT_ERROR("%s(): memcpy_s failed", __func__);
		return -1;
	}
	q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
	PRINT_DEBUG("%s(): front %u rear %u", __func__, q->front, q->rear);
	if (q->rear == 0) {
		q->round++;
		PRINT_DEBUG("%s(): round %u", __func__, q->round);
	}
	return 0;
}
/************* data structure **********************/

static struct freq_trace *g_freq_trace = NULL;

static int register_freqtrace(void)
{
	struct rdr_logsave_info info;
	struct freq_trace *freq_trace = NULL;

	PRINT_START();

	freq_trace = kzalloc(sizeof(struct freq_trace), GFP_KERNEL);
	if (freq_trace == NULL) {
		PRINT_ERROR("%s(): kzalloc file->buf failed", __func__);
		return -1;
	}
	init_queue(&freq_trace->queue[0]); // cluster 0
	init_queue(&freq_trace->queue[1]); // cluster 1
	init_queue(&freq_trace->queue[2]); // cluster 2

	/* set g_freq_trace after memset-ed since there could be async-access same time */
	g_freq_trace = freq_trace;

	info.log_phyaddr       = virt_to_phys(g_freq_trace);
	info.log_size          = sizeof(struct freq_trace);
	rdr_register_logsave_info("CPUFREQ", &info);

	PRINT_END();
	return 0;
}

static int parse(char *freq, u32 freq_len, const struct trace_queue *q)
{
	u32 i;
	u32 accumulate_len = 0;
	u32 text_len;
	struct freq_domain_log log;
	const struct freq_domain_log *plog;
	unsigned long rem_nsec;
	u64 ts;

	plog = &log;
	for (i = q->front; i != q->rear; i = (i + 1) % MAX_QUEUE_SIZE) {
		if (rdr_safe_copy((char *)plog, sizeof(struct freq_domain_log), (char *)&q->data[i],
			sizeof(struct freq_domain_log), (char *)q, sizeof(struct trace_queue)) != 0) {
			PRINT_ERROR("%s(): memcpy_s text failed, i: %u, front: %u, rear: %u",
				__func__, i, q->front, q->rear);
			break;
		}
		ts = plog->ts;
		rem_nsec = do_div(ts, 1000000000); // 1000000000 nsec
		text_len = sprintf_s(freq + accumulate_len, freq_len - accumulate_len,
			"[%5lu.%06lu] %x %u\n", (unsigned long)ts, rem_nsec / 1000, // 1000 ms
			plog->index, plog->freq);
		if (text_len == -1)
			break;
		accumulate_len += text_len;
	}
	return accumulate_len;
}

static int parse_freqtrace(void)
{
	struct rdr_region *region = NULL;
	struct rdr_file *freq0 = NULL;
	struct rdr_file *freq1 = NULL;
	struct rdr_file *freq2 = NULL;
	struct freq_trace *trace = NULL;
	u32 freq_len;

	region = rdr_acquire_region("CPUFREQ");
	if (region == NULL) {
		PRINT_ERROR("%s(): aquire region failed", __func__);
		return -1;
	}
	trace = (struct freq_trace *)region->data;
	freq_len = region->size * 5; // 5 of logbuf_len
	freq0 = rdr_add_file("freq_domain0.txt", freq_len);
	freq1 = rdr_add_file("freq_domain1.txt", freq_len);
	freq2 = rdr_add_file("freq_domain2.txt", freq_len);
	if (freq0 == NULL || freq1 == NULL || freq2 == NULL) {
		PRINT_ERROR("%s(): allocate file buffer failed", __func__);
		return -1;
	}
	freq0->used_size = parse(freq0->buf, freq_len, &trace->queue[0]); // cluster 0
	freq1->used_size = parse(freq1->buf, freq_len, &trace->queue[1]); // cluster 1
	freq2->used_size = parse(freq2->buf, freq_len, &trace->queue[2]); // cluster 2
	return 0;
}

struct module_data freq_prv_data = {
	.name = "freq",
	.level = CPUFREQ_MODULE_LEVEL,
	.on_panic = NULL,
	.init = register_freqtrace,
	.on_storage_ready = parse_freqtrace,
	.init_done = NULL,
	.last_reset_handler = NULL,
	.exit = NULL,
};

static int g_rdr_cluster_map[8] = { 0, 0, 0, 0, 1, 1, 1, 2};
int rdr_cpufreq_trace(const struct cpufreq_policy *policy, unsigned int index)
{
	struct freq_domain_log log;
	if (unlikely(policy == NULL)) {
		PRINT_ERROR("%s(): policy is NULL", __func__);
		return -1;
	}
	if (g_freq_trace == NULL) {
		PRINT_DEBUG("%s(): g_freq_trace not inited", __func__);
		return -1;
	}
	PRINT_DEBUG("%s(): cpu %x index %u freq %u", __func__, policy->cpu, index,
		policy->freq_table[index].frequency);
	log.index = index;
	log.freq = policy->freq_table[index].frequency;
	log.ts = local_clock();
	if (unlikely(policy->cpu >= sizeof(g_rdr_cluster_map))) {
		PRINT_ERROR("%s(): policy's cpu number wrong", __func__);
		return -1;
	}
	enqueue(&g_freq_trace->queue[g_rdr_cluster_map[policy->cpu]], &log);
	return 0;
}
