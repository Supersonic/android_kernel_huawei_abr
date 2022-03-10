/* SPDX-License-Identifier: GPL-2.0 */
/*
 * D2D tracing system. Used for protocol diagnostic and performance tuning
 */

/*
 * Global switch for the D2DP tracing
 */
#ifdef CONFIG_D2D_PROTOCOL_TRACING

#undef TRACE_SYSTEM
#define TRACE_SYSTEM d2d

#if !defined(_D2D_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _D2D_TRACE_H

#include <linux/tracepoint.h>

#include "headers.h"

/* General event template for D2D header related events */
DECLARE_EVENT_CLASS(d2d_header_event_template,
	TP_PROTO(struct d2d_header *hdr),
	TP_ARGS(hdr),

	TP_STRUCT__entry(
		__field(__u16,  flags)
		__field(__u32, seq_id)
		__field(__u32, data_len)
	),

	TP_fast_assign(
		__entry->flags    = hdr->flags;
		__entry->seq_id   = hdr->seq_id;
		__entry->data_len = hdr->data_len;
	),

	TP_printk("flags=%s id:%u sacks:%u",
		  __print_flags(__entry->flags, "",
				{ D2D_FLAG_ACK,     "A" },
				{ D2D_FLAG_SUSPEND, "S" }),
		  __entry->seq_id, __entry->data_len)
);

DEFINE_EVENT(d2d_header_event_template,
	     d2d_ack_received,
	     TP_PROTO(struct d2d_header *hdr),
	     TP_ARGS(hdr));

DEFINE_EVENT(d2d_header_event_template,
	     d2d_ack_sent,
	     TP_PROTO(struct d2d_header *hdr),
	     TP_ARGS(hdr));

/* Simple marker event class which is used just to measure time */
DECLARE_EVENT_CLASS(d2d_marker_template,
	TP_PROTO(unsigned int num),
	TP_ARGS(num),

	TP_STRUCT__entry(
		__field(__u32, num)
	),

	TP_fast_assign(
		__entry->num = num;
	),

	TP_printk("%d", __entry->num)
);

DEFINE_EVENT(d2d_marker_template, d2d_ack_timer,
	     TP_PROTO(unsigned int num), TP_ARGS(num));

DEFINE_EVENT(d2d_marker_template, d2d_rto_timer,
	     TP_PROTO(unsigned int num), TP_ARGS(num));

DEFINE_EVENT(d2d_marker_template, d2d_tx_status,
	     TP_PROTO(unsigned int num), TP_ARGS(num));

#endif /* _D2D_TRACE_H */

/* Make traces from local file and not from trace/events directory */
#undef TRACE_INCLUDE_PATH
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE trace
#include <trace/define_trace.h>

#else /* CONFIG_D2D_PROTOCOL_TRACING */

#include "headers.h"

/*
 * Turn off all the traces to save some kernel image size
 */
static inline void trace_d2d_ack_received(struct d2d_header *hdr)
{
}

static inline void trace_d2d_ack_sent(struct d2d_header *hdr)
{
}

static inline void trace_d2d_ack_timer(int num)
{
}

static inline void trace_d2d_rto_timer(int num)
{
}

static inline void trace_d2d_tx_status(int num)
{
}

#endif
