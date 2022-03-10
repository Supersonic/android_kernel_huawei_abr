	.text
	.file	"mas_bkops_core.c"
	.globl	mas_bkops_update_dur    // -- Begin function mas_bkops_update_dur
	.p2align	2
	.type	mas_bkops_update_dur,@function
mas_bkops_update_dur:                   // @mas_bkops_update_dur
.Lmas_bkops_update_dur$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB0_16
// %bb.1:
	mov	x19, x0
	bl	ktime_get
	ldr	x8, [x19, #56]
	ldr	x9, [x19, #112]
	sub	x8, x0, x8
	cmp	x8, x9
	b.ls	.LBB0_3
// %bb.2:
	str	x8, [x19, #112]
.LBB0_3:
	mov	w9, #57599
	movk	w9, #1525, lsl #16
	cmp	x8, x9
	b.hi	.LBB0_5
// %bb.4:
	ldr	w9, [x19, #128]
	add	w9, w9, #1              // =1
	str	w9, [x19, #128]
	b	.LBB0_14
.LBB0_5:
	mov	w9, #25855
	movk	w9, #7629, lsl #16
	cmp	x8, x9
	b.hi	.LBB0_7
// %bb.6:
	ldr	w9, [x19, #132]
	add	w9, w9, #1              // =1
	str	w9, [x19, #132]
	b	.LBB0_14
.LBB0_7:
	mov	w9, #51711
	movk	w9, #15258, lsl #16
	cmp	x8, x9
	b.hi	.LBB0_9
// %bb.8:
	ldr	w9, [x19, #136]
	add	w9, w9, #1              // =1
	str	w9, [x19, #136]
	b	.LBB0_14
.LBB0_9:
	mov	w9, #37887
	movk	w9, #30517, lsl #16
	cmp	x8, x9
	b.hi	.LBB0_11
// %bb.10:
	ldr	w9, [x19, #140]
	add	w9, w9, #1              // =1
	str	w9, [x19, #140]
	b	.LBB0_14
.LBB0_11:
	mov	x9, #61951
	movk	x9, #10757, lsl #16
	movk	x9, #1, lsl #32
	cmp	x8, x9
	b.hi	.LBB0_13
// %bb.12:
	ldr	w9, [x19, #144]
	add	w9, w9, #1              // =1
	str	w9, [x19, #144]
	b	.LBB0_14
.LBB0_13:
	ldr	w9, [x19, #148]
	add	w9, w9, #1              // =1
	str	w9, [x19, #148]
.LBB0_14:
	ldr	w9, [x19, #8]
	cbz	w9, .LBB0_16
// %bb.15:
	ldr	x10, [x19, #120]
	sub	w11, w9, #1             // =1
	madd	x8, x10, x11, x8
	udiv	x8, x8, x9
	str	x8, [x19, #120]
.LBB0_16:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end0:
	.size	mas_bkops_update_dur, .Lfunc_end0-mas_bkops_update_dur
                                        // -- End function
	.globl	mas_bkops_set_status_str // -- Begin function mas_bkops_set_status_str
	.p2align	2
	.type	mas_bkops_set_status_str,@function
mas_bkops_set_status_str:               // @mas_bkops_set_status_str
.Lmas_bkops_set_status_str$local:
// %bb.0:
	cbz	x0, .LBB1_3
// %bb.1:
	cbz	x2, .LBB1_3
// %bb.2:
	str	w1, [x0, #488]
	str	x2, [x0, #496]
.LBB1_3:
	ret
.Lfunc_end1:
	.size	mas_bkops_set_status_str, .Lfunc_end1-mas_bkops_set_status_str
                                        // -- End function
	.globl	bkops_idle_work_func    // -- Begin function bkops_idle_work_func
	.p2align	2
	.type	bkops_idle_work_func,@function
bkops_idle_work_func:                   // @bkops_idle_work_func
.Lbkops_idle_work_func$local:
// %bb.0:
	sub	sp, sp, #64             // =64
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #32            // =32
	str	x19, [sp, #48]          // 8-byte Folded Spill
	stur	x8, [x29, #-8]
	stur	wzr, [x29, #-12]
	ldur	x8, [x0, #-8]
	sub	x19, x0, #32            // =32
	mov	x0, x8
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB2_3
// %bb.1:
	ldr	x8, [x19, #168]
	str	x8, [sp, #8]
	ldr	x8, [sp, #8]
	tbz	w8, #8, .LBB2_3
// %bb.2:
	add	x8, x19, #168           // =168
	mov	w9, #256
	//APP
	// atomic64_fetch_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x11, x10, x9
	stlxr	w12, x11, [x8]
	cbnz	w12, 1b
	dmb ish
	//NO_APP
	tbnz	w10, #8, .LBB2_12
.LBB2_3:
	sub	x1, x29, #12            // =12
	mov	x0, x19
	bl	mas_bkops_query
	cbnz	w0, .LBB2_14
// %bb.4:
	ldr	x8, [x19, #168]
	ldr	x0, [x19, #24]
	and	x8, x8, #0xfffffffffffffffd
	str	x8, [x19, #168]
	ldur	w8, [x29, #-12]
	str	w8, [x19, #240]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB2_8
// %bb.5:
	ldr	w8, [x19]
	cmp	w8, #2                  // =2
	b.ne	.LBB2_8
// %bb.6:
	ldur	w8, [x29, #-12]
	cbnz	w8, .LBB2_9
// %bb.7:
	add	x8, x19, #168           // =168
	mov	w9, #128
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
.LBB2_8:
	ldur	w8, [x29, #-12]
	cbz	w8, .LBB2_12
.LBB2_9:
	ldr	x8, [x19, #168]
	tbnz	w8, #0, .LBB2_12
// %bb.10:
	mov	x0, x19
	bl	mas_bkops_start
	cbnz	w0, .LBB2_15
// %bb.11:
	add	x8, x19, #168           // =168
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 1
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
.LBB2_12:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB2_16
// %bb.13:
	ldr	x19, [sp, #48]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #32]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.LBB2_14:
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	printk
	b	.LBB2_12
.LBB2_15:
	adrp	x0, .L.str.1
	add	x0, x0, :lo12:.L.str.1
	bl	printk
	b	.LBB2_12
.LBB2_16:
	bl	__stack_chk_fail
.Lfunc_end2:
	.size	bkops_idle_work_func, .Lfunc_end2-bkops_idle_work_func
                                        // -- End function
	.p2align	2               // -- Begin function mas_bkops_query
	.type	mas_bkops_query,@function
mas_bkops_query:                        // @mas_bkops_query
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x21, [x0, #8]
	mov	x19, x0
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	mov	x29, sp
	mov	x20, x1
	bl	mutex_lock
	adrp	x8, bkops_enabled
	ldrb	w8, [x8, :lo12:bkops_enabled]
	tbz	w8, #0, .LBB3_2
// %bb.1:
	mov	w21, wzr
	b	.LBB3_12
.LBB3_2:
	ldr	w3, [x19, #584]
	cbnz	w3, .LBB3_13
.LBB3_3:
	bl	ktime_get
	ldr	x8, [x19, #16]
	ldr	x9, [x21, #8]
	mov	x22, x0
	mov	x1, x20
	mov	x0, x8
	blr	x9
	adrp	x8, user_manual_gc
	ldrb	w9, [x8, :lo12:user_manual_gc]
	mov	w21, w0
	cbz	w9, .LBB3_6
// %bb.4:
	ldr	w9, [x20]
	cbnz	w9, .LBB3_6
// %bb.5:
	strb	wzr, [x8, :lo12:user_manual_gc]
.LBB3_6:
	ldr	w8, [x19, #316]
	add	w8, w8, #1              // =1
	str	w8, [x19, #316]
	cbnz	w21, .LBB3_9
// %bb.7:
	ldr	w8, [x20]
	ldr	w9, [x19, #488]
	cmp	w8, w9
	b.hs	.LBB3_9
// %bb.8:
	add	x8, x19, x8, lsl #2
	ldr	w9, [x8, #448]
	add	w9, w9, #1              // =1
	str	w9, [x8, #448]
.LBB3_9:
	bl	ktime_get
	ldr	x9, [x19, #360]
	sub	x8, x0, x22
	cmp	x8, x9
	b.ls	.LBB3_11
// %bb.10:
	str	x8, [x19, #360]
.LBB3_11:
	ldr	w9, [x19, #316]
	ldr	x10, [x19, #368]
	sub	w11, w9, #1             // =1
	madd	x8, x10, x11, x8
	udiv	x8, x8, x9
	str	x8, [x19, #368]
.LBB3_12:
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	bl	mutex_unlock
	mov	w0, w21
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB3_13:
	adrp	x0, .L.str.15
	adrp	x1, .L__func__.mas_bkops_query
	add	x0, x0, :lo12:.L.str.15
	add	x1, x1, :lo12:.L__func__.mas_bkops_query
	mov	w2, #113
	bl	printk
	ldr	w0, [x19, #584]
	bl	msleep
	b	.LBB3_3
.Lfunc_end3:
	.size	mas_bkops_query, .Lfunc_end3-mas_bkops_query
                                        // -- End function
	.p2align	2               // -- Begin function mas_bkops_start
	.type	mas_bkops_start,@function
mas_bkops_start:                        // @mas_bkops_start
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x20, [x0, #8]
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	ktime_get
	mov	x21, x0
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	bl	mutex_lock
	adrp	x8, bkops_enabled
	ldrb	w8, [x8, :lo12:bkops_enabled]
	tbz	w8, #0, .LBB4_2
// %bb.1:
	mov	w20, wzr
	b	.LBB4_6
.LBB4_2:
	ldr	x8, [x20]
	ldr	x0, [x19, #16]
	mov	w1, #1
	blr	x8
	ldr	w8, [x19, #300]
	mov	w20, w0
	adrp	x0, mas_bkops_start.bkops_start_rs
	adrp	x1, .L__func__.mas_bkops_start
	add	w8, w8, #1              // =1
	add	x0, x0, :lo12:mas_bkops_start.bkops_start_rs
	add	x1, x1, :lo12:.L__func__.mas_bkops_start
	str	w8, [x19, #300]
	bl	___ratelimit
	cbnz	w0, .LBB4_7
.LBB4_3:
	bl	ktime_get
	ldr	x9, [x19, #376]
	sub	x8, x0, x21
	str	x0, [x19, #352]
	cmp	x8, x9
	b.ls	.LBB4_5
// %bb.4:
	str	x8, [x19, #376]
.LBB4_5:
	ldr	w9, [x19, #300]
	ldr	x10, [x19, #384]
	sub	w11, w9, #1             // =1
	madd	x8, x10, x11, x8
	udiv	x8, x8, x9
	str	x8, [x19, #384]
.LBB4_6:
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	bl	mutex_unlock
	mov	w0, w20
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB4_7:
	ldr	w1, [x19, #300]
	adrp	x0, .L.str.16
	add	x0, x0, :lo12:.L.str.16
	bl	printk
	b	.LBB4_3
.Lfunc_end4:
	.size	mas_bkops_start, .Lfunc_end4-mas_bkops_start
                                        // -- End function
	.globl	bkops_io_busyidle_notify_handler // -- Begin function bkops_io_busyidle_notify_handler
	.p2align	2
	.type	bkops_io_busyidle_notify_handler,@function
bkops_io_busyidle_notify_handler:       // @bkops_io_busyidle_notify_handler
.Lbkops_io_busyidle_notify_handler$local:
// %bb.0:
	sub	sp, sp, #48             // =48
	sub	w8, w1, #2              // =2
	cmp	w8, #2                  // =2
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	b.lo	.LBB5_28
// %bb.1:
	ldr	x19, [x0, #40]
	mov	w20, w1
	cbz	w1, .LBB5_12
// %bb.2:
	cmp	w20, #1                 // =1
	b.ne	.LBB5_19
// %bb.3:
	ldr	x0, [x19, #24]
	bl	blk_queue_query_unistore_enable
	tbnz	w0, #0, .LBB5_5
// %bb.4:
	mov	x0, x19
	bl	bkops_should_query
	cbz	w0, .LBB5_29
.LBB5_5:
	ldr	x0, [x19, #24]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB5_11
// %bb.6:
	ldr	w8, [x19]
	cmp	w8, #2                  // =2
	b.ne	.LBB5_9
// %bb.7:
	add	x8, x19, #168           // =168
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 64
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
	ldr	x8, [x19, #168]
	tbz	w8, #7, .LBB5_28
// %bb.8:
	ldr	w8, [x19]
	cmp	w8, #2                  // =2
	b.eq	.LBB5_10
.LBB5_9:
	mov	x0, x19
	bl	bkops_should_query
	cbz	w0, .LBB5_29
.LBB5_10:
	ldr	x8, [x19, #168]
	tbnz	w8, #5, .LBB5_28
.LBB5_11:
	add	x8, x19, #168           // =168
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x11, [x8]
	orr	x11, x11, 32
	stxr	w12, x11, [x8]
	cbnz	w12, 1b
	//NO_APP
	mov	w9, #256
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x11, [x8]
	bic	x11, x11, x9
	stxr	w12, x11, [x8]
	cbnz	w12, 1b
	//NO_APP
	adrp	x10, bkops_workqueue_g
	ldr	w0, [x19, #176]
	ldr	x20, [x10, :lo12:bkops_workqueue_g]
	add	x19, x19, #32           // =32
	bl	__msecs_to_jiffies
	mov	x3, x0
	mov	w0, #8
	mov	x1, x20
	mov	x2, x19
	bl	queue_delayed_work_on
	mov	w0, #1
	b	.LBB5_29
.LBB5_12:
	ldr	x0, [x19, #24]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB5_15
// %bb.13:
	ldr	w8, [x19]
	cmp	w8, #2                  // =2
	b.ne	.LBB5_15
// %bb.14:
	add	x8, x19, #168           // =168
	mov	w9, #64
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
.LBB5_15:
	ldr	x8, [x19, #168]
	str	x8, [sp, #8]
	ldr	x8, [sp, #8]
	tbz	w8, #5, .LBB5_28
// %bb.16:
	add	x8, x19, #168           // =168
	mov	w9, #32
	//APP
	// atomic64_fetch_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x11, x10, x9
	stlxr	w12, x11, [x8]
	cbnz	w12, 1b
	dmb ish
	//NO_APP
	tbz	w10, #5, .LBB5_28
// %bb.17:
	mov	w9, #256
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
	add	x0, x19, #32            // =32
	bl	cancel_delayed_work_sync
	tbz	w0, #0, .LBB5_21
// %bb.18:
	ldr	w8, [x19, #320]
	add	w8, w8, #1              // =1
	str	w8, [x19, #320]
	b	.LBB5_22
.LBB5_19:
	adrp	x0, bkops_io_busyidle_notify_handler._rs
	adrp	x1, .L__func__.bkops_io_busyidle_notify_handler
	add	x0, x0, :lo12:bkops_io_busyidle_notify_handler._rs
	add	x1, x1, :lo12:.L__func__.bkops_io_busyidle_notify_handler
	bl	___ratelimit
	cbz	w0, .LBB5_29
// %bb.20:
	adrp	x0, .L.str.2
	adrp	x1, .L__func__.bkops_io_busyidle_notify_handler
	add	x0, x0, :lo12:.L.str.2
	add	x1, x1, :lo12:.L__func__.bkops_io_busyidle_notify_handler
	mov	w2, w20
	bl	printk
	b	.LBB5_28
.LBB5_21:
	ldr	w8, [x19, #324]
	add	w8, w8, #1              // =1
	str	w8, [x19, #324]
.LBB5_22:
	ldr	x8, [x19, #168]
	tbz	w8, #0, .LBB5_28
// %bb.23:
	ldrb	w8, [x19, #580]
	cbnz	w8, .LBB5_27
// %bb.24:
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	bl	mutex_lock
	adrp	x8, bkops_enabled
	ldrb	w8, [x8, :lo12:bkops_enabled]
	tbnz	w8, #0, .LBB5_26
// %bb.25:
	ldp	x8, x0, [x19, #8]
	mov	w1, wzr
	ldr	x8, [x8]
	blr	x8
	ldr	w8, [x19, #308]
	add	w8, w8, #1              // =1
	str	w8, [x19, #308]
.LBB5_26:
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	bl	mutex_unlock
.LBB5_27:
	add	x8, x19, #168           // =168
	mov	w9, #1
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
.LBB5_28:
	mov	w0, wzr
.LBB5_29:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.Lfunc_end5:
	.size	bkops_io_busyidle_notify_handler, .Lfunc_end5-bkops_io_busyidle_notify_handler
                                        // -- End function
	.globl	mas_bkops_work_query    // -- Begin function mas_bkops_work_query
	.p2align	2
	.type	mas_bkops_work_query,@function
mas_bkops_work_query:                   // @mas_bkops_work_query
.Lmas_bkops_work_query$local:
// %bb.0:
	sub	sp, sp, #64             // =64
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #32            // =32
	str	x19, [sp, #48]          // 8-byte Folded Spill
	stur	x8, [x29, #-8]
	stur	wzr, [x29, #-12]
	cbz	x0, .LBB6_8
// %bb.1:
	ldr	x0, [x0, #144]
	bl	mas_blk_get_lld
	cbz	x0, .LBB6_8
// %bb.2:
	ldrb	w8, [x0, #25]
	tbz	w8, #1, .LBB6_7
// %bb.3:
	ldr	x19, [x0, #328]
	cbz	x19, .LBB6_23
// %bb.4:
	ldr	w8, [x19]
	cmp	w8, #2                  // =2
	b.ne	.LBB6_7
// %bb.5:
	ldr	w8, [x19, #520]
	add	w8, w8, #1              // =1
	str	w8, [x19, #520]
	ldr	x8, [x19, #168]
	tbnz	w8, #6, .LBB6_11
// %bb.6:
	ldr	w8, [x19, #524]
	mov	w0, #4
	add	w8, w8, #1              // =1
	str	w8, [x19, #524]
	b	.LBB6_9
.LBB6_7:
	mov	w0, #1
	b	.LBB6_9
.LBB6_8:
	mov	w0, #2
.LBB6_9:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB6_24
// %bb.10:
	ldr	x19, [sp, #48]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #32]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.LBB6_11:
	ldr	x8, [x19, #168]
	tbnz	w8, #0, .LBB6_16
// %bb.12:
	ldr	x8, [x19, #168]
	str	x8, [sp, #8]
	ldr	x8, [sp, #8]
	tbz	w8, #5, .LBB6_18
// %bb.13:
	add	x8, x19, #168           // =168
	mov	w9, #32
	//APP
	// atomic64_fetch_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x11, x10, x9
	stlxr	w12, x11, [x8]
	cbnz	w12, 1b
	dmb ish
	//NO_APP
	tbz	w10, #5, .LBB6_18
// %bb.14:
	mov	w9, #256
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
	add	x0, x19, #32            // =32
	bl	cancel_delayed_work_sync
	tbz	w0, #0, .LBB6_17
// %bb.15:
	ldr	w8, [x19, #320]
	add	w8, w8, #1              // =1
	str	w8, [x19, #320]
	b	.LBB6_18
.LBB6_16:
	ldr	w8, [x19, #528]
	mov	w0, #5
	add	w8, w8, #1              // =1
	str	w8, [x19, #528]
	b	.LBB6_9
.LBB6_17:
	ldr	w8, [x19, #324]
	add	w8, w8, #1              // =1
	str	w8, [x19, #324]
.LBB6_18:
	sub	x1, x29, #12            // =12
	mov	x0, x19
	bl	mas_bkops_query
	cbnz	w0, .LBB6_22
// %bb.19:
	ldur	w8, [x29, #-12]
	cbz	w8, .LBB6_21
// %bb.20:
	mov	w0, #8
	b	.LBB6_9
.LBB6_21:
	add	x8, x19, #168           // =168
	mov	w9, #128
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
	ldr	w8, [x19, #552]
	ldr	w9, [x19, #532]
	mov	w0, #7
	add	w8, w8, #1              // =1
	add	w9, w9, #1              // =1
	str	w8, [x19, #552]
	str	w9, [x19, #532]
	b	.LBB6_9
.LBB6_22:
	mov	w2, w0
	adrp	x0, .L.str.3
	adrp	x1, .L__func__.mas_bkops_work_query
	add	x0, x0, :lo12:.L.str.3
	add	x1, x1, :lo12:.L__func__.mas_bkops_work_query
	bl	printk
	mov	w0, #6
	b	.LBB6_9
.LBB6_23:
	mov	w0, #3
	b	.LBB6_9
.LBB6_24:
	bl	__stack_chk_fail
.Lfunc_end6:
	.size	mas_bkops_work_query, .Lfunc_end6-mas_bkops_work_query
                                        // -- End function
	.globl	mas_bkops_work_start    // -- Begin function mas_bkops_work_start
	.p2align	2
	.type	mas_bkops_work_start,@function
mas_bkops_work_start:                   // @mas_bkops_work_start
.Lmas_bkops_work_start$local:
// %bb.0:
	sub	sp, sp, #64             // =64
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #32            // =32
	str	x19, [sp, #48]          // 8-byte Folded Spill
	stur	x8, [x29, #-8]
	stur	wzr, [x29, #-12]
	cbz	x0, .LBB7_8
// %bb.1:
	ldr	x0, [x0, #144]
	bl	mas_blk_get_lld
	cbz	x0, .LBB7_8
// %bb.2:
	ldrb	w8, [x0, #25]
	tbz	w8, #1, .LBB7_7
// %bb.3:
	ldr	x19, [x0, #328]
	cbz	x19, .LBB7_25
// %bb.4:
	ldr	w8, [x19]
	cmp	w8, #2                  // =2
	b.ne	.LBB7_7
// %bb.5:
	ldr	x8, [x19, #168]
	tbnz	w8, #6, .LBB7_11
// %bb.6:
	ldr	w8, [x19, #508]
	mov	w0, #4
	add	w8, w8, #1              // =1
	str	w8, [x19, #508]
	b	.LBB7_9
.LBB7_7:
	mov	w0, #1
	b	.LBB7_9
.LBB7_8:
	mov	w0, #2
.LBB7_9:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB7_26
// %bb.10:
	ldr	x19, [sp, #48]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #32]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.LBB7_11:
	ldr	x8, [x19, #168]
	tbnz	w8, #0, .LBB7_16
// %bb.12:
	ldr	x8, [x19, #168]
	str	x8, [sp, #8]
	ldr	x8, [sp, #8]
	tbz	w8, #5, .LBB7_18
// %bb.13:
	add	x8, x19, #168           // =168
	mov	w9, #32
	//APP
	// atomic64_fetch_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x11, x10, x9
	stlxr	w12, x11, [x8]
	cbnz	w12, 1b
	dmb ish
	//NO_APP
	tbz	w10, #5, .LBB7_18
// %bb.14:
	mov	w9, #256
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
	add	x0, x19, #32            // =32
	bl	cancel_delayed_work_sync
	tbz	w0, #0, .LBB7_17
// %bb.15:
	ldr	w8, [x19, #320]
	add	w8, w8, #1              // =1
	str	w8, [x19, #320]
	b	.LBB7_18
.LBB7_16:
	ldr	w8, [x19, #512]
	mov	w0, #5
	add	w8, w8, #1              // =1
	str	w8, [x19, #512]
	b	.LBB7_9
.LBB7_17:
	ldr	w8, [x19, #324]
	add	w8, w8, #1              // =1
	str	w8, [x19, #324]
.LBB7_18:
	sub	x1, x29, #12            // =12
	mov	x0, x19
	bl	mas_bkops_query
	cbnz	w0, .LBB7_23
// %bb.19:
	ldur	w8, [x29, #-12]
	cbz	w8, .LBB7_22
// %bb.20:
	mov	x0, x19
	bl	mas_bkops_start
	cbnz	w0, .LBB7_24
// %bb.21:
	add	x8, x19, #168           // =168
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 128
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
	ldr	w9, [x19, #504]
	add	w9, w9, #1              // =1
	str	w9, [x19, #504]
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 1
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
	b	.LBB7_9
.LBB7_22:
	add	x8, x19, #168           // =168
	mov	w9, #128
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
	ldr	w8, [x19, #552]
	ldr	w9, [x19, #516]
	mov	w0, #7
	add	w8, w8, #1              // =1
	add	w9, w9, #1              // =1
	str	w8, [x19, #552]
	str	w9, [x19, #516]
	b	.LBB7_9
.LBB7_23:
	mov	w2, w0
	adrp	x0, .L.str.3
	adrp	x1, .L__func__.mas_bkops_work_start
	add	x0, x0, :lo12:.L.str.3
	add	x1, x1, :lo12:.L__func__.mas_bkops_work_start
	bl	printk
	mov	w0, #6
	b	.LBB7_9
.LBB7_24:
	mov	w2, w0
	adrp	x0, .L.str.17
	adrp	x1, .L__func__.mas_bkops_work_start_ret
	add	x0, x0, :lo12:.L.str.17
	add	x1, x1, :lo12:.L__func__.mas_bkops_work_start_ret
	bl	printk
	mov	w0, #9
	b	.LBB7_9
.LBB7_25:
	mov	w0, #3
	b	.LBB7_9
.LBB7_26:
	bl	__stack_chk_fail
.Lfunc_end7:
	.size	mas_bkops_work_start, .Lfunc_end7-mas_bkops_work_start
                                        // -- End function
	.globl	mas_bkops_work_stop     // -- Begin function mas_bkops_work_stop
	.p2align	2
	.type	mas_bkops_work_stop,@function
mas_bkops_work_stop:                    // @mas_bkops_work_stop
.Lmas_bkops_work_stop$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB8_8
// %bb.1:
	ldr	x0, [x0, #144]
	bl	mas_blk_get_lld
	cbz	x0, .LBB8_8
// %bb.2:
	ldr	x19, [x0, #328]
	cbz	x19, .LBB8_8
// %bb.3:
	ldr	x8, [x0, #24]
	tbz	w8, #9, .LBB8_8
// %bb.4:
	ldr	w8, [x19]
	cmp	w8, #2                  // =2
	b.ne	.LBB8_8
// %bb.5:
	ldr	x8, [x19, #168]
	str	x8, [x29, #24]
	ldr	x8, [x29, #24]
	tbz	w8, #7, .LBB8_7
// %bb.6:
	add	x8, x19, #168           // =168
	mov	w9, #128
	//APP
	// atomic64_fetch_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x11, x10, x9
	stlxr	w12, x11, [x8]
	cbnz	w12, 1b
	dmb ish
	//NO_APP
	tbnz	w10, #7, .LBB8_9
.LBB8_7:
	ldr	w8, [x19, #536]
	add	w8, w8, #1              // =1
	str	w8, [x19, #536]
.LBB8_8:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB8_9:
	ldr	w8, [x19, #540]
	add	w8, w8, #1              // =1
	str	w8, [x19, #540]
	ldr	x8, [x19, #168]
	str	x8, [x29, #24]
	ldr	x8, [x29, #24]
	tbz	w8, #5, .LBB8_12
// %bb.10:
	add	x8, x19, #168           // =168
	mov	w9, #32
	//APP
	// atomic64_fetch_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x11, x10, x9
	stlxr	w12, x11, [x8]
	cbnz	w12, 1b
	dmb ish
	//NO_APP
	tbz	w10, #5, .LBB8_12
// %bb.11:
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 256
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
.LBB8_12:
	ldr	x8, [x19, #168]
	tbz	w8, #0, .LBB8_8
// %bb.13:
	ldrb	w8, [x19, #580]
	cbnz	w8, .LBB8_17
// %bb.14:
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	bl	mutex_lock
	adrp	x8, bkops_enabled
	ldrb	w8, [x8, :lo12:bkops_enabled]
	tbnz	w8, #0, .LBB8_16
// %bb.15:
	ldp	x8, x0, [x19, #8]
	mov	w1, wzr
	ldr	x8, [x8]
	blr	x8
	ldr	w8, [x19, #308]
	add	w8, w8, #1              // =1
	str	w8, [x19, #308]
.LBB8_16:
	adrp	x0, bkops_lock
	add	x0, x0, :lo12:bkops_lock
	bl	mutex_unlock
.LBB8_17:
	add	x8, x19, #168           // =168
	mov	w9, #1
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
	b	.LBB8_8
.Lfunc_end8:
	.size	mas_bkops_work_stop, .Lfunc_end8-mas_bkops_work_stop
                                        // -- End function
	.globl	bkops_pm_callback       // -- Begin function bkops_pm_callback
	.p2align	2
	.type	bkops_pm_callback,@function
bkops_pm_callback:                      // @bkops_pm_callback
.Lbkops_pm_callback$local:
// %bb.0:
	sub	x8, x1, #1              // =1
	cmp	x8, #3                  // =3
	b.hi	.LBB9_2
// %bb.1:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	adrp	x19, bkops_lock
	add	x19, x19, :lo12:bkops_lock
	tst	x8, #0x1
	mov	x0, x19
	mov	x29, sp
	cset	w20, eq
	bl	mutex_lock
	adrp	x8, bkops_enabled
	mov	x0, x19
	strb	w20, [x8, :lo12:bkops_enabled]
	bl	mutex_unlock
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	mov	w0, #1
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB9_2:
	mov	w0, wzr
	ret
.Lfunc_end9:
	.size	bkops_pm_callback, .Lfunc_end9-bkops_pm_callback
                                        // -- End function
	.globl	bkops_notify_reboot     // -- Begin function bkops_notify_reboot
	.p2align	2
	.type	bkops_notify_reboot,@function
bkops_notify_reboot:                    // @bkops_notify_reboot
.Lbkops_notify_reboot$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	adrp	x19, bkops_lock
	add	x19, x19, :lo12:bkops_lock
	mov	x0, x19
	mov	x29, sp
	bl	mutex_lock
	adrp	x8, bkops_enabled
	mov	w9, #1
	mov	x0, x19
	strb	w9, [x8, :lo12:bkops_enabled]
	bl	mutex_unlock
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	mov	w0, wzr
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end10:
	.size	bkops_notify_reboot, .Lfunc_end10-bkops_notify_reboot
                                        // -- End function
	.globl	mas_bkops_alloc         // -- Begin function mas_bkops_alloc
	.p2align	2
	.type	mas_bkops_alloc,@function
mas_bkops_alloc:                        // @mas_bkops_alloc
.Lmas_bkops_alloc$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	adrp	x8, kmalloc_caches+80
	ldr	x0, [x8, :lo12:kmalloc_caches+80]
	mov	w1, #3520
	mov	w2, #592
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	bl	kmem_cache_alloc_trace
	mov	x19, x0
	cbz	x0, .LBB11_3
// %bb.1:
	adrp	x10, __ufs_bkops_idle_work_func
	adrp	x1, delayed_work_timer_fn
	mov	x8, #68719476704
	add	x9, x19, #40            // =40
	add	x10, x10, :lo12:__ufs_bkops_idle_work_func
	add	x0, x19, #80            // =80
	add	x1, x1, :lo12:delayed_work_timer_fn
	mov	w2, #2097152
	mov	x3, xzr
	mov	x4, xzr
	str	x8, [x19, #32]
	str	x9, [x19, #40]
	stp	x9, x10, [x19, #48]
	bl	init_timer_key
	mov	w8, #1000
	mov	w9, #3600
	mov	w10, #536870912
	str	x8, [x19, #176]
	str	x9, [x19, #248]
	str	x10, [x19, #264]
	str	x10, [x19, #280]
.LBB11_2:
	mov	x0, x19
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB11_3:
	adrp	x0, .L.str.4
	add	x0, x0, :lo12:.L.str.4
	bl	printk
	b	.LBB11_2
.Lfunc_end11:
	.size	mas_bkops_alloc, .Lfunc_end11-mas_bkops_alloc
                                        // -- End function
	.globl	mas_bkops_manual_gc_proc_show // -- Begin function mas_bkops_manual_gc_proc_show
	.p2align	2
	.type	mas_bkops_manual_gc_proc_show,@function
mas_bkops_manual_gc_proc_show:          // @mas_bkops_manual_gc_proc_show
.Lmas_bkops_manual_gc_proc_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	adrp	x8, user_manual_gc
	ldrb	w2, [x8, :lo12:user_manual_gc]
	adrp	x1, .L.str.5
	add	x1, x1, :lo12:.L.str.5
	mov	x29, sp
	bl	seq_printf
	mov	w0, wzr
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end12:
	.size	mas_bkops_manual_gc_proc_show, .Lfunc_end12-mas_bkops_manual_gc_proc_show
                                        // -- End function
	.globl	mas_bkops_manual_gc_proc_open // -- Begin function mas_bkops_manual_gc_proc_open
	.p2align	2
	.type	mas_bkops_manual_gc_proc_open,@function
mas_bkops_manual_gc_proc_open:          // @mas_bkops_manual_gc_proc_open
.Lmas_bkops_manual_gc_proc_open$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x0, x1
	adrp	x1, __cfi_mas_bkops_manual_gc_proc_show
	add	x1, x1, :lo12:__cfi_mas_bkops_manual_gc_proc_show
	mov	x2, xzr
	mov	x29, sp
	bl	single_open
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end13:
	.size	mas_bkops_manual_gc_proc_open, .Lfunc_end13-mas_bkops_manual_gc_proc_open
                                        // -- End function
	.globl	mas_bkops_manual_gc_proc_write // -- Begin function mas_bkops_manual_gc_proc_write
	.p2align	2
	.type	mas_bkops_manual_gc_proc_write,@function
mas_bkops_manual_gc_proc_write:         // @mas_bkops_manual_gc_proc_write
.Lmas_bkops_manual_gc_proc_write$local:
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	x0, #-22
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x19, [sp, #32]          // 8-byte Folded Spill
	add	x29, sp, #16            // =16
	str	x8, [sp, #8]
	strb	wzr, [sp, #4]
	cbz	x1, .LBB14_9
// %bb.1:
	mov	x19, x2
	cbz	x2, .LBB14_9
// %bb.2:
	//APP
	mrs x8, sp_el0
	//NO_APP
	ldrb	w11, [x8, #54]
	ldr	x10, [x8, #8]
	lsl	x9, x1, #8
	tbnz	w11, #5, .LBB14_4
// %bb.3:
	ldr	x12, [x8]
	mov	x11, x1
	tbz	w12, #26, .LBB14_5
.LBB14_4:
	and	x11, x1, x9, asr #8
.LBB14_5:
	//APP
		adds	x11, x11, 1
	csel	x10, xzr, x10, hi
	csinv	x11, x11, xzr, cc
	sbcs	xzr, x11, x10
	cset	x11, ls

	//NO_APP
	cbz	x11, .LBB14_12
// %bb.6:
	//APP
	.if 1 == 1
661:
	nop
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 10
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	.inst (0xd500401f | ((0) << 16 | (4) << 5) | ((!!0) << 8))
	
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	ldr	x10, [x8, #8]
	and	x9, x1, x9, asr #8
	//APP
		bics	xzr, x9, x10
	csel	x8, x1, xzr, eq

	//NO_APP
	//APP
	hint #20
	//NO_APP
	add	x0, sp, #4              // =4
	mov	w2, #1
	mov	x1, x8
	bl	__arch_copy_from_user
	//APP
	.if 1 == 1
661:
	nop
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 10
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	.inst (0xd500401f | ((0) << 16 | (4) << 5) | ((!!1) << 8))
	
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbnz	x0, .LBB14_13
// %bb.7:
	ldrb	w8, [sp, #4]
	sub	w8, w8, #48             // =48
	cmp	w8, #2                  // =2
	b.hs	.LBB14_11
// %bb.8:
	adrp	x9, user_manual_gc
	mov	x0, x19
	strb	w8, [x9, :lo12:user_manual_gc]
.LBB14_9:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB14_15
// %bb.10:
	ldr	x19, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB14_11:
	adrp	x0, .L.str.6
	adrp	x1, .L__func__.mas_bkops_manual_gc_proc_write
	add	x0, x0, :lo12:.L.str.6
	add	x1, x1, :lo12:.L__func__.mas_bkops_manual_gc_proc_write
	mov	w2, #745
	bl	printk
	mov	x0, #-22
	b	.LBB14_9
.LBB14_12:
	mov	w2, #1
	b	.LBB14_14
.LBB14_13:
	mov	x2, x0
.LBB14_14:
	add	x8, sp, #4              // =4
	sub	x8, x8, x2
	add	x0, x8, #1              // =1
	mov	w1, wzr
	bl	memset
	mov	x0, #-14
	b	.LBB14_9
.LBB14_15:
	bl	__stack_chk_fail
.Lfunc_end14:
	.size	mas_bkops_manual_gc_proc_write, .Lfunc_end14-mas_bkops_manual_gc_proc_write
                                        // -- End function
	.globl	mas_bkops_status_proc_show // -- Begin function mas_bkops_status_proc_show
	.p2align	2
	.type	mas_bkops_status_proc_show,@function
mas_bkops_status_proc_show:             // @mas_bkops_status_proc_show
.Lmas_bkops_status_proc_show$local:
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x19, [sp, #32]          // 8-byte Folded Spill
	mov	x19, x0
	str	x8, [sp, #8]
	str	wzr, [sp, #4]
	ldr	x0, [x0, #128]
	add	x1, sp, #4              // =4
	add	x29, sp, #16            // =16
	bl	mas_bkops_query
	ldr	w2, [sp, #4]
	adrp	x1, .L.str.7
	add	x1, x1, :lo12:.L.str.7
	mov	x0, x19
	bl	seq_printf
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB15_2
// %bb.1:
	ldr	x19, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	mov	w0, wzr
	add	sp, sp, #48             // =48
	ret
.LBB15_2:
	bl	__stack_chk_fail
.Lfunc_end15:
	.size	mas_bkops_status_proc_show, .Lfunc_end15-mas_bkops_status_proc_show
                                        // -- End function
	.globl	mas_bkops_status_proc_open // -- Begin function mas_bkops_status_proc_open
	.p2align	2
	.type	mas_bkops_status_proc_open,@function
mas_bkops_status_proc_open:             // @mas_bkops_status_proc_open
.Lmas_bkops_status_proc_open$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x1
	bl	PDE_DATA
	adrp	x1, __cfi_mas_bkops_status_proc_show
	mov	x2, x0
	add	x1, x1, :lo12:__cfi_mas_bkops_status_proc_show
	mov	x0, x19
	bl	single_open
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end16:
	.size	mas_bkops_status_proc_open, .Lfunc_end16-mas_bkops_status_proc_open
                                        // -- End function
	.globl	mas_bkops_enable        // -- Begin function mas_bkops_enable
	.p2align	2
	.type	mas_bkops_enable,@function
mas_bkops_enable:                       // @mas_bkops_enable
.Lmas_bkops_enable$local:
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	mov	x21, x0
	mov	w0, #-22
	str	x25, [sp, #16]          // 8-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x21, .LBB17_17
// %bb.1:
	mov	x19, x1
	cbz	x1, .LBB17_17
// %bb.2:
	mov	x22, x2
	cbz	x2, .LBB17_17
// %bb.3:
	mov	x0, x21
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #24]
	tbz	w8, #3, .LBB17_23
// %bb.4:
	ldr	w8, [x19]
	sub	w8, w8, #1              // =1
	cmp	w8, #3                  // =3
	b.hs	.LBB17_18
// %bb.5:
	ldr	x8, [x19, #8]
	cbz	x8, .LBB17_19
// %bb.6:
	adrp	x25, mas_bkops_inited
	ldrb	w8, [x25, :lo12:mas_bkops_inited]
	mov	x20, x0
	adrp	x24, bkops_workqueue_g
	tbnz	w8, #0, .LBB17_9
// %bb.7:
	adrp	x0, .L.str.19
	add	x0, x0, :lo12:.L.str.19
	mov	w1, #24
	mov	w2, wzr
	bl	alloc_workqueue
	str	x0, [x24, :lo12:bkops_workqueue_g]
	cbz	x0, .LBB17_21
// %bb.8:
	adrp	x0, bkops_pm_notifier
	add	x0, x0, :lo12:bkops_pm_notifier
	bl	register_pm_notifier
	adrp	x0, bkops_reboot_notifier
	add	x0, x0, :lo12:bkops_reboot_notifier
	bl	register_reboot_notifier
	mov	w8, #1
	strb	w8, [x25, :lo12:mas_bkops_inited]
.LBB17_9:
	mov	x0, x19
	mov	x1, x22
	add	x23, x19, #184          // =184
	bl	mas_bkops_add_debugfs
	cbnz	w0, .LBB17_20
.LBB17_10:
	adrp	x1, .L.str.13
	add	x1, x1, :lo12:.L.str.13
	mov	w2, #32
	mov	x0, x23
	str	x21, [x19, #24]
	bl	strncpy
	adrp	x8, __cfi_bkops_io_busyidle_notify_handler
	add	x8, x8, :lo12:__cfi_bkops_io_busyidle_notify_handler
	mov	x0, x20
	mov	x1, x23
	strb	wzr, [x19, #215]
	stp	x8, x19, [x19, #216]
	bl	blk_lld_busyidle_event_subscribe
	cbz	w0, .LBB17_14
// %bb.11:
	ldr	x8, [x24, :lo12:bkops_workqueue_g]
	cbz	x8, .LBB17_13
// %bb.12:
	adrp	x0, bkops_reboot_notifier
	add	x0, x0, :lo12:bkops_reboot_notifier
	strb	wzr, [x25, :lo12:mas_bkops_inited]
	bl	unregister_reboot_notifier
	adrp	x0, bkops_pm_notifier
	add	x0, x0, :lo12:bkops_pm_notifier
	bl	unregister_pm_notifier
	ldr	x0, [x24, :lo12:bkops_workqueue_g]
	bl	destroy_workqueue
	str	xzr, [x24, :lo12:bkops_workqueue_g]
.LBB17_13:
	mov	w0, #-22
	b	.LBB17_17
.LBB17_14:
	adrp	x0, .L.str.20
	add	x0, x0, :lo12:.L.str.20
	mov	x1, xzr
	bl	proc_mkdir
	cbz	x0, .LBB17_22
// %bb.15:
	mov	x20, x0
	adrp	x0, .L.str.22
	adrp	x3, mas_bkops_manual_gc_fops
	add	x0, x0, :lo12:.L.str.22
	add	x3, x3, :lo12:mas_bkops_manual_gc_fops
	mov	w1, #432
	mov	x2, x20
	bl	proc_create
	adrp	x0, .L.str.23
	adrp	x3, mas_bkops_status_fops
	add	x0, x0, :lo12:.L.str.23
	add	x3, x3, :lo12:mas_bkops_status_fops
	mov	w1, #288
	mov	x2, x20
	mov	x4, x19
	bl	proc_create_data
.LBB17_16:
	mov	w0, wzr
.LBB17_17:
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldr	x25, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB17_18:
	adrp	x0, .L.str.9
	add	x0, x0, :lo12:.L.str.9
	bl	printk
	b	.LBB17_13
.LBB17_19:
	adrp	x0, .L.str.10
	add	x0, x0, :lo12:.L.str.10
	bl	printk
	b	.LBB17_13
.LBB17_20:
	adrp	x0, .L.str.12
	add	x0, x0, :lo12:.L.str.12
	bl	printk
	b	.LBB17_10
.LBB17_21:
	adrp	x0, .L.str.11
	adrp	x1, .L__func__.mas_bkops_enable
	add	x0, x0, :lo12:.L.str.11
	add	x1, x1, :lo12:.L__func__.mas_bkops_enable
	mov	w2, #830
	bl	printk
	b	.LBB17_13
.LBB17_22:
	adrp	x0, .L.str.21
	adrp	x1, .L__func__.mas_bkops_manual_gc_proc_init
	add	x0, x0, :lo12:.L.str.21
	add	x1, x1, :lo12:.L__func__.mas_bkops_manual_gc_proc_init
	mov	w2, #792
	bl	printk
	b	.LBB17_16
.LBB17_23:
	adrp	x0, .L.str.8
	add	x0, x0, :lo12:.L.str.8
	bl	printk
	b	.LBB17_13
.Lfunc_end17:
	.size	mas_bkops_enable, .Lfunc_end17-mas_bkops_enable
                                        // -- End function
	.globl	blk_mq_tagset_tz_query_register // -- Begin function blk_mq_tagset_tz_query_register
	.p2align	2
	.type	blk_mq_tagset_tz_query_register,@function
blk_mq_tagset_tz_query_register:        // @blk_mq_tagset_tz_query_register
.Lblk_mq_tagset_tz_query_register$local:
// %bb.0:
	cbz	x0, .LBB18_2
// %bb.1:
	str	x1, [x0, #128]
.LBB18_2:
	ret
.Lfunc_end18:
	.size	blk_mq_tagset_tz_query_register, .Lfunc_end18-blk_mq_tagset_tz_query_register
                                        // -- End function
	.globl	blk_lld_tz_query        // -- Begin function blk_lld_tz_query
	.p2align	2
	.type	blk_lld_tz_query,@function
blk_lld_tz_query:                       // @blk_lld_tz_query
.Lblk_lld_tz_query$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	mov	x8, x0
	mov	w0, #-1
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x8, .LBB19_7
// %bb.1:
	mov	x20, x2
	cbz	x2, .LBB19_7
// %bb.2:
	ldr	x8, [x8, #136]
	ldr	x22, [x8, #1272]
	cbz	x22, .LBB19_6
// %bb.3:
	mov	x0, x22
	mov	w19, w3
	mov	w21, w1
	bl	mas_blk_get_lld
	cbz	x0, .LBB19_6
// %bb.4:
	ldr	x8, [x0, #64]
	cbz	x8, .LBB19_6
// %bb.5:
	mov	x0, x22
	mov	w1, w21
	mov	x2, x20
	mov	w3, w19
	blr	x8
	b	.LBB19_7
.LBB19_6:
	mov	w0, #-1
.LBB19_7:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end19:
	.size	blk_lld_tz_query, .Lfunc_end19-blk_lld_tz_query
                                        // -- End function
	.globl	blk_mq_tagset_tz_ctrl_register // -- Begin function blk_mq_tagset_tz_ctrl_register
	.p2align	2
	.type	blk_mq_tagset_tz_ctrl_register,@function
blk_mq_tagset_tz_ctrl_register:         // @blk_mq_tagset_tz_ctrl_register
.Lblk_mq_tagset_tz_ctrl_register$local:
// %bb.0:
	cbz	x0, .LBB20_2
// %bb.1:
	str	x1, [x0, #136]
.LBB20_2:
	ret
.Lfunc_end20:
	.size	blk_mq_tagset_tz_ctrl_register, .Lfunc_end20-blk_mq_tagset_tz_ctrl_register
                                        // -- End function
	.globl	blk_lld_tz_ctrl         // -- Begin function blk_lld_tz_ctrl
	.p2align	2
	.type	blk_lld_tz_ctrl,@function
blk_lld_tz_ctrl:                        // @blk_lld_tz_ctrl
.Lblk_lld_tz_ctrl$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB21_5
// %bb.1:
	ldr	x8, [x0, #136]
	ldr	x21, [x8, #1272]
	cbz	x21, .LBB21_5
// %bb.2:
	mov	x0, x21
	mov	w19, w2
	mov	w20, w1
	bl	mas_blk_get_lld
	cbz	x0, .LBB21_5
// %bb.3:
	ldr	x8, [x0, #72]
	cbz	x8, .LBB21_5
// %bb.4:
	mov	x0, x21
	mov	w1, w20
	mov	w2, w19
	blr	x8
	b	.LBB21_6
.LBB21_5:
	mov	w0, #-1
.LBB21_6:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end21:
	.size	blk_lld_tz_ctrl, .Lfunc_end21-blk_lld_tz_ctrl
                                        // -- End function
	.globl	mas_queue_tz_write_bytes_show // -- Begin function mas_queue_tz_write_bytes_show
	.p2align	2
	.type	mas_queue_tz_write_bytes_show,@function
mas_queue_tz_write_bytes_show:          // @mas_queue_tz_write_bytes_show
.Lmas_queue_tz_write_bytes_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x3, [x0, #1096]
	adrp	x2, .L.str.14
	mov	x0, x1
	add	x2, x2, :lo12:.L.str.14
	mov	w1, #4096
	mov	x29, sp
	bl	snprintf
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x0, w0
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end22:
	.size	mas_queue_tz_write_bytes_show, .Lfunc_end22-mas_queue_tz_write_bytes_show
                                        // -- End function
	.globl	blk_mq_tagset_health_query_register // -- Begin function blk_mq_tagset_health_query_register
	.p2align	2
	.type	blk_mq_tagset_health_query_register,@function
blk_mq_tagset_health_query_register:    // @blk_mq_tagset_health_query_register
.Lblk_mq_tagset_health_query_register$local:
// %bb.0:
	cbz	x0, .LBB23_2
// %bb.1:
	str	x1, [x0, #144]
.LBB23_2:
	ret
.Lfunc_end23:
	.size	blk_mq_tagset_health_query_register, .Lfunc_end23-blk_mq_tagset_health_query_register
                                        // -- End function
	.globl	blk_lld_health_query    // -- Begin function blk_lld_health_query
	.p2align	2
	.type	blk_lld_health_query,@function
blk_lld_health_query:                   // @blk_lld_health_query
.Lblk_lld_health_query$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	mov	x8, x0
	mov	w0, #-1
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x8, .LBB24_8
// %bb.1:
	mov	x21, x1
	cbz	x1, .LBB24_8
// %bb.2:
	mov	x20, x2
	cbz	x2, .LBB24_8
// %bb.3:
	mov	x19, x3
	cbz	x3, .LBB24_8
// %bb.4:
	ldr	x8, [x8, #136]
	ldr	x22, [x8, #1272]
	mov	x0, x22
	bl	mas_blk_get_lld
	cbz	x0, .LBB24_7
// %bb.5:
	ldr	x8, [x0, #80]
	cbz	x8, .LBB24_7
// %bb.6:
	mov	x0, x22
	mov	x1, x21
	mov	x2, x20
	mov	x3, x19
	blr	x8
	b	.LBB24_8
.LBB24_7:
	mov	w0, #-1
.LBB24_8:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end24:
	.size	blk_lld_health_query, .Lfunc_end24-blk_lld_health_query
                                        // -- End function
	.p2align	2               // -- Begin function bkops_should_query
	.type	bkops_should_query,@function
bkops_should_query:                     // @bkops_should_query
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldrb	w8, [x0, #581]
	mov	x19, x0
	mov	x29, sp
	cbz	w8, .LBB25_3
// %bb.1:
	ldr	x8, [x19, #168]
.LBB25_2:
	and	x8, x8, #0xfffffffffffffffd
	mov	w0, wzr
	b	.LBB25_20
.LBB25_3:
	ldrb	w8, [x19, #582]
	cbnz	w8, .LBB25_19
// %bb.4:
	ldrb	w8, [x19, #168]
	tbnz	w8, #1, .LBB25_19
// %bb.5:
	ldr	w8, [x19, #244]
	cbz	w8, .LBB25_8
// %bb.6:
	ldr	w8, [x19, #240]
	cbz	w8, .LBB25_8
// %bb.7:
	ldr	w8, [x19, #296]
	add	w8, w8, #1              // =1
	str	w8, [x19, #296]
	b	.LBB25_19
.LBB25_8:
	ldr	x0, [x19, #24]
	bl	mas_blk_get_lld
	ldr	x8, [x19, #168]
	mov	x20, x0
	tbz	w8, #4, .LBB25_12
// %bb.9:
	mov	w0, #1
	bl	ktime_get_with_offset
	bl	ns_to_timespec64
	ldr	x8, [x19, #256]
	cbz	x8, .LBB25_21
// %bb.10:
	ldr	x9, [x19, #248]
	sub	x8, x0, x8
	cmp	x8, x9
	b.ge	.LBB25_21
// %bb.11:
	ldr	x8, [x19, #168]
.LBB25_12:
	tbz	w8, #3, .LBB25_14
// %bb.13:
	ldr	x9, [x20, #1888]
	ldp	x11, x10, [x19, #264]
	sub	x10, x9, x10
	cmp	x10, x11
	b.hs	.LBB25_17
.LBB25_14:
	tbz	w8, #2, .LBB25_16
// %bb.15:
	ldr	x9, [x20, #1880]
	ldp	x11, x10, [x19, #280]
	sub	x10, x9, x10
	cmp	x10, x11
	b.hs	.LBB25_18
.LBB25_16:
	adrp	x9, user_manual_gc
	ldrb	w9, [x9, :lo12:user_manual_gc]
	cbnz	w9, .LBB25_19
	b	.LBB25_2
.LBB25_17:
	ldr	w8, [x19, #336]
	str	x9, [x19, #272]
	add	w8, w8, #1              // =1
	str	w8, [x19, #336]
	b	.LBB25_19
.LBB25_18:
	ldr	w8, [x19, #332]
	str	x9, [x19, #288]
	add	w8, w8, #1              // =1
	str	w8, [x19, #332]
.LBB25_19:
	ldr	x8, [x19, #168]
	mov	w0, #1
	orr	x8, x8, #0x2
.LBB25_20:
	str	x8, [x19, #168]
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB25_21:
	ldr	w8, [x19, #328]
	str	x0, [x19, #256]
	add	w8, w8, #1              // =1
	str	w8, [x19, #328]
	b	.LBB25_19
.Lfunc_end25:
	.size	bkops_should_query, .Lfunc_end25-bkops_should_query
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\0013[BKOPS]mas_bkops_query failed!\n"
	.size	.L.str, 34

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"\0013[BKOPS]mas_bkops_start failed!\n"
	.size	.L.str.1, 34

	.type	bkops_io_busyidle_notify_handler._rs,@object // @bkops_io_busyidle_notify_handler._rs
	.data
	.p2align	3
bkops_io_busyidle_notify_handler._rs:
	.zero	4
	.word	1250                    // 0x4e2
	.word	10                      // 0xa
	.word	0                       // 0x0
	.word	0                       // 0x0
	.zero	4
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.size	bkops_io_busyidle_notify_handler._rs, 40

	.type	.L__func__.bkops_io_busyidle_notify_handler,@object // @__func__.bkops_io_busyidle_notify_handler
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__func__.bkops_io_busyidle_notify_handler:
	.asciz	"bkops_io_busyidle_notify_handler"
	.size	.L__func__.bkops_io_busyidle_notify_handler, 33

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"\0013[BKOPS]%s unknown state: %d\n"
	.size	.L.str.2, 31

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"\0013[BKOPS]%s: mas_bkops_query failed, ret = %d!\n"
	.size	.L.str.3, 48

	.type	.L__func__.mas_bkops_work_query,@object // @__func__.mas_bkops_work_query
.L__func__.mas_bkops_work_query:
	.asciz	"mas_bkops_work_query"
	.size	.L__func__.mas_bkops_work_query, 21

	.type	.L__func__.mas_bkops_work_start,@object // @__func__.mas_bkops_work_start
.L__func__.mas_bkops_work_start:
	.asciz	"mas_bkops_work_start"
	.size	.L__func__.mas_bkops_work_start, 21

	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"\0013[BKOPS]Failed to alloc bkops_func\n"
	.size	.L.str.4, 37

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"%d\n"
	.size	.L.str.5, 4

	.type	user_manual_gc,@object  // @user_manual_gc
	.local	user_manual_gc
	.comm	user_manual_gc,1,4
	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"\0013[BKOPS]%s %d: input error.\n"
	.size	.L.str.6, 30

	.type	.L__func__.mas_bkops_manual_gc_proc_write,@object // @__func__.mas_bkops_manual_gc_proc_write
.L__func__.mas_bkops_manual_gc_proc_write:
	.asciz	"mas_bkops_manual_gc_proc_write"
	.size	.L__func__.mas_bkops_manual_gc_proc_write, 31

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"%u\n"
	.size	.L.str.7, 4

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"\0013[BKOPS]Busy Idle was not enabled!\n"
	.size	.L.str.8, 37

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"\0013[BKOPS]bkops dev type not specified!\n"
	.size	.L.str.9, 40

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"\0013[BKOPS]bkops_ops is NULL!\n"
	.size	.L.str.10, 29

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"\0013[BKOPS]%s %d mas_bkops_init failed!\n"
	.size	.L.str.11, 39

	.type	.L__func__.mas_bkops_enable,@object // @__func__.mas_bkops_enable
.L__func__.mas_bkops_enable:
	.asciz	"mas_bkops_enable"
	.size	.L__func__.mas_bkops_enable, 17

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"\0013[BKOPS]Failed to create mmc bkops debugfs!\n"
	.size	.L.str.12, 46

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"mas_bkops"
	.size	.L.str.13, 10

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"tz_write_bytes: %lu\n"
	.size	.L.str.14, 21

	.type	bkops_lock,@object      // @bkops_lock
	.data
	.p2align	3
bkops_lock:
	.zero	8
	.zero	4
	.zero	4
	.xword	bkops_lock+16
	.xword	bkops_lock+16
	.xword	0
	.size	bkops_lock, 40

	.type	bkops_enabled,@object   // @bkops_enabled
	.local	bkops_enabled
	.comm	bkops_enabled,1,1
	.type	.L.str.15,@object       // @.str.15
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.15:
	.asciz	"\0013[BKOPS]%s %d simulate bkops query delay %u ms\n"
	.size	.L.str.15, 49

	.type	.L__func__.mas_bkops_query,@object // @__func__.mas_bkops_query
.L__func__.mas_bkops_query:
	.asciz	"mas_bkops_query"
	.size	.L__func__.mas_bkops_query, 16

	.type	mas_bkops_start.bkops_start_rs,@object // @mas_bkops_start.bkops_start_rs
	.data
	.p2align	3
mas_bkops_start.bkops_start_rs:
	.zero	4
	.word	15000                   // 0x3a98
	.word	1                       // 0x1
	.word	0                       // 0x0
	.word	0                       // 0x0
	.zero	4
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.size	mas_bkops_start.bkops_start_rs, 40

	.type	.L__func__.mas_bkops_start,@object // @__func__.mas_bkops_start
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__func__.mas_bkops_start:
	.asciz	"mas_bkops_start"
	.size	.L__func__.mas_bkops_start, 16

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"\0016[BKOPS]bkops_start_count: %u\n"
	.size	.L.str.16, 32

	.type	bkops_workqueue_g,@object // @bkops_workqueue_g
	.local	bkops_workqueue_g
	.comm	bkops_workqueue_g,8,8
	.type	.L.str.17,@object       // @.str.17
.L.str.17:
	.asciz	"\0013[BKOPS]%s: mas_bkops_start failed, ret = %d!\n"
	.size	.L.str.17, 48

	.type	.L__func__.mas_bkops_work_start_ret,@object // @__func__.mas_bkops_work_start_ret
.L__func__.mas_bkops_work_start_ret:
	.asciz	"mas_bkops_work_start_ret"
	.size	.L__func__.mas_bkops_work_start_ret, 25

	.type	mas_bkops_inited,@object // @mas_bkops_inited
	.local	mas_bkops_inited
	.comm	mas_bkops_inited,1,4
	.type	.L.str.19,@object       // @.str.19
.L.str.19:
	.asciz	"bkops_workqueue"
	.size	.L.str.19, 16

	.type	bkops_pm_notifier,@object // @bkops_pm_notifier
	.data
	.p2align	3
bkops_pm_notifier:
	.xword	__cfi_bkops_pm_callback
	.xword	0
	.word	0                       // 0x0
	.zero	4
	.size	bkops_pm_notifier, 24

	.type	bkops_reboot_notifier,@object // @bkops_reboot_notifier
	.p2align	3
bkops_reboot_notifier:
	.xword	__cfi_bkops_notify_reboot
	.xword	0
	.word	2147483647              // 0x7fffffff
	.zero	4
	.size	bkops_reboot_notifier, 24

	.type	.L.str.20,@object       // @.str.20
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.20:
	.asciz	"block"
	.size	.L.str.20, 6

	.type	.L.str.21,@object       // @.str.21
.L.str.21:
	.asciz	"\0013[BKOPS]%s %d: proc mk error.\n"
	.size	.L.str.21, 32

	.type	.L__func__.mas_bkops_manual_gc_proc_init,@object // @__func__.mas_bkops_manual_gc_proc_init
.L__func__.mas_bkops_manual_gc_proc_init:
	.asciz	"mas_bkops_manual_gc_proc_init"
	.size	.L__func__.mas_bkops_manual_gc_proc_init, 30

	.type	.L.str.22,@object       // @.str.22
.L.str.22:
	.asciz	"manual_gc"
	.size	.L.str.22, 10

	.type	mas_bkops_manual_gc_fops,@object // @mas_bkops_manual_gc_fops
	.section	.rodata,"a",@progbits
	.p2align	3
mas_bkops_manual_gc_fops:
	.xword	0
	.xword	seq_lseek
	.xword	seq_read
	.xword	__cfi_mas_bkops_manual_gc_proc_write
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0                       // 0x0
	.xword	__cfi_mas_bkops_manual_gc_proc_open
	.xword	0
	.xword	single_release
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.size	mas_bkops_manual_gc_fops, 288

	.type	.L.str.23,@object       // @.str.23
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.23:
	.asciz	"status"
	.size	.L.str.23, 7

	.type	mas_bkops_status_fops,@object // @mas_bkops_status_fops
	.section	.rodata,"a",@progbits
	.p2align	3
mas_bkops_status_fops:
	.xword	0
	.xword	seq_lseek
	.xword	seq_read
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0                       // 0x0
	.xword	__cfi_mas_bkops_status_proc_open
	.xword	0
	.xword	single_release
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.size	mas_bkops_status_fops, 288

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
