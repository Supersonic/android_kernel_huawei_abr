/*
* ufs_wb_trace_event.h
*
* Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/

#undef TRACE_SYSTEM
#define TRACE_SYSTEM ufs_wb_trace

#if !defined(_UFS_WB_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _UFS_WB_TRACE_H

#include <linux/tracepoint.h>

TRACE_EVENT(ufs_wb_runtime,
	TP_PROTO(u16 slot_cnt, u32 total_size,
		u16 low_speed_cnt, u16 curr_bw, u8 wb_enabled),

	TP_ARGS(slot_cnt, total_size, low_speed_cnt, curr_bw, wb_enabled),

	TP_STRUCT__entry(
		__field(u16, slot_cnt)
		__field(u32, total_size)
		__field(u16, low_speed_cnt)
		__field(u16, curr_bw)
		__field(u8, wb_enabled)
	),

	TP_fast_assign(
		__entry->slot_cnt = slot_cnt;
		__entry->total_size = total_size;
		__entry->low_speed_cnt = low_speed_cnt;
		__entry->curr_bw = curr_bw;
		__entry->wb_enabled = wb_enabled;
	),

	TP_printk(
		"slot_cnt: %u, total_size: %u, low_speed_cnt: %u, "
		"curr_bw: %u MB/s, wb_enabled: %u",
		__entry->slot_cnt, __entry->total_size, __entry->low_speed_cnt,
		__entry->curr_bw, __entry->wb_enabled
	)
);
#endif /* if !defined(_UFS_WB_TRACE_H) || defined(TRACE_HEADER_MULTI_READ) */

/* This part must be outside protection */
#include <trace/define_trace.h>
