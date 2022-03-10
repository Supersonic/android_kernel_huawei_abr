	.text
	.file	"mas_blk_busy_idle_notify.c"
	.globl	mas_blk_busyidle_handler_latency_check_timer_expire // -- Begin function mas_blk_busyidle_handler_latency_check_timer_expire
	.p2align	2
	.type	mas_blk_busyidle_handler_latency_check_timer_expire,@function
mas_blk_busyidle_handler_latency_check_timer_expire: // @mas_blk_busyidle_handler_latency_check_timer_expire
.Lmas_blk_busyidle_handler_latency_check_timer_expire$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	add	x2, x0, #80             // =80
	adrp	x0, .L.str
	adrp	x1, .L__func__.mas_blk_busyidle_handler_latency_check_timer_expire
	add	x0, x0, :lo12:.L.str
	add	x1, x1, :lo12:.L__func__.mas_blk_busyidle_handler_latency_check_timer_expire
	mov	w3, #1000
	mov	x29, sp
	bl	printk
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end0:
	.size	mas_blk_busyidle_handler_latency_check_timer_expire, .Lfunc_end0-mas_blk_busyidle_handler_latency_check_timer_expire
                                        // -- End function
	.globl	mas_blk_busyidle_notify_handler // -- Begin function mas_blk_busyidle_notify_handler
	.p2align	2
	.type	mas_blk_busyidle_notify_handler,@function
mas_blk_busyidle_notify_handler:        // @mas_blk_busyidle_notify_handler
.Lmas_blk_busyidle_notify_handler$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	ldr	x8, [x0, #56]
	add	x19, x0, #24            // =24
	str	x23, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x8, .LBB1_20
// %bb.1:
	mov	x23, x1
	mov	x22, x0
	bl	ktime_get
	adrp	x8, jiffies
	ldr	x8, [x8, :lo12:jiffies]
	sub	x21, x22, #56           // =56
	mov	x20, x0
	mov	x0, x21
	add	x1, x8, #250            // =250
	bl	mod_timer
	cmp	x23, #3                 // =3
	b.hi	.LBB1_21
// %bb.2:
	adrp	x8, .LJTI1_0
	add	x8, x8, :lo12:.LJTI1_0
	adr	x9, .LBB1_3
	ldrb	w10, [x8, x23]
	add	x9, x9, x10, lsl #2
	br	x9
.LBB1_3:
	ldr	w8, [x22, #80]
	cmp	w8, #1                  // =1
	b.ne	.LBB1_12
// %bb.4:
	ldr	x8, [x22, #56]
	mov	x0, x19
	mov	w1, wzr
	blr	x8
	str	wzr, [x22, #80]
	b	.LBB1_11
.LBB1_5:
	ldr	x8, [x22, #72]
	ldrb	w8, [x8, #25]
	tbz	w8, #1, .LBB1_12
// %bb.6:
	ldr	w8, [x22, #84]
	cmp	w8, #1                  // =1
	b.ne	.LBB1_12
// %bb.7:
	ldr	x8, [x22, #56]
	mov	w1, #2
	mov	x0, x19
	blr	x8
	str	wzr, [x22, #84]
	b	.LBB1_11
.LBB1_8:
	ldr	x8, [x22, #72]
	ldrb	w8, [x8, #25]
	tbz	w8, #1, .LBB1_12
// %bb.9:
	ldr	w8, [x22, #84]
	cbnz	w8, .LBB1_12
// %bb.10:
	ldr	x8, [x22, #56]
	mov	w1, #3
	mov	x0, x19
	blr	x8
	mov	w8, #1
	cmp	w0, #1                  // =1
	str	w8, [x22, #84]
	b.eq	.LBB1_17
.LBB1_11:
	cmp	w0, #2                  // =2
	b.eq	.LBB1_18
.LBB1_12:
	mov	w22, wzr
.LBB1_13:
	mov	x0, x21
	bl	del_timer_sync
	bl	ktime_get
	mov	w8, #38528
	movk	w8, #152, lsl #16
	add	x8, x20, x8
	cmp	x0, x8
	b.gt	.LBB1_19
.LBB1_14:
	mov	w0, w22
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB1_15:
	ldr	w8, [x22, #80]
	cbnz	w8, .LBB1_12
// %bb.16:
	ldr	x8, [x22, #56]
	mov	w1, #1
	mov	x0, x19
	mov	w23, #1
	blr	x8
	cmp	w0, #1                  // =1
	str	w23, [x22, #80]
	b.ne	.LBB1_11
.LBB1_17:
	mov	w22, #32769
	b	.LBB1_13
.LBB1_18:
	adrp	x0, .L.str.2
	adrp	x1, .L__func__.mas_blk_busyidle_notify_handler
	add	x0, x0, :lo12:.L.str.2
	add	x1, x1, :lo12:.L__func__.mas_blk_busyidle_notify_handler
	mov	x2, x19
	bl	printk
	bl	dump_stack
	b	.LBB1_12
.LBB1_19:
	bl	ktime_get
	mov	x9, #13531
	movk	x9, #55222, lsl #16
	movk	x9, #56962, lsl #32
	sub	x8, x0, x20
	movk	x9, #17179, lsl #48
	smulh	x8, x8, x9
	asr	x9, x8, #18
	adrp	x0, .L.str.3
	adrp	x1, .L__func__.mas_blk_busyidle_notify_handler
	add	x3, x9, x8, lsr #63
	add	x0, x0, :lo12:.L.str.3
	add	x1, x1, :lo12:.L__func__.mas_blk_busyidle_notify_handler
	mov	x2, x19
	bl	printk
	b	.LBB1_14
.LBB1_20:
	adrp	x0, .L.str.1
	adrp	x1, .L__func__.mas_blk_busyidle_notify_handler
	add	x0, x0, :lo12:.L.str.1
	add	x1, x1, :lo12:.L__func__.mas_blk_busyidle_notify_handler
	mov	x2, x19
	bl	printk
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.LBB1_21:
	adrp	x0, .L.str.34
	add	x0, x0, :lo12:.L.str.34
	bl	mas_blk_rdr_panic
.Lfunc_end1:
	.size	mas_blk_busyidle_notify_handler, .Lfunc_end1-mas_blk_busyidle_notify_handler
	.section	.rodata,"a",@progbits
.LJTI1_0:
	.byte	(.LBB1_3-.LBB1_3)>>2
	.byte	(.LBB1_15-.LBB1_3)>>2
	.byte	(.LBB1_5-.LBB1_3)>>2
	.byte	(.LBB1_8-.LBB1_3)>>2
                                        // -- End function
	.text
	.p2align	2               // -- Begin function mas_blk_rdr_panic
	.type	mas_blk_rdr_panic,@function
mas_blk_rdr_panic:                      // @mas_blk_rdr_panic
// %bb.0:
	cbz	x0, .LBB2_2
// %bb.1:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x1, x0
	adrp	x0, .L.str.33
	add	x0, x0, :lo12:.L.str.33
	mov	x29, sp
	bl	printk
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
.LBB2_2:
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.Lfunc_end2:
	.size	mas_blk_rdr_panic, .Lfunc_end2-mas_blk_rdr_panic
                                        // -- End function
	.globl	blk_lld_fg_io_idle_notify // -- Begin function blk_lld_fg_io_idle_notify
	.p2align	2
	.type	blk_lld_fg_io_idle_notify,@function
blk_lld_fg_io_idle_notify:              // @blk_lld_fg_io_idle_notify
.Lblk_lld_fg_io_idle_notify$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	w8, [x0]
	mov	x29, sp
	cmp	w8, #122                // =122
	b.ne	.LBB3_2
// %bb.1:
	adrp	x8, blk_fg_io_idle_notify_workqueue
	ldr	x19, [x8, :lo12:blk_fg_io_idle_notify_workqueue]
	ldr	w8, [x0, #1768]
	add	x20, x0, #1632          // =1632
	mov	w0, w8
	bl	__msecs_to_jiffies
	mov	x3, x0
	mov	w0, #8
	mov	x1, x19
	mov	x2, x20
	bl	queue_delayed_work_on
.LBB3_2:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end3:
	.size	blk_lld_fg_io_idle_notify, .Lfunc_end3-blk_lld_fg_io_idle_notify
                                        // -- End function
	.globl	mas_blk_idle_notify_work // -- Begin function mas_blk_idle_notify_work
	.p2align	2
	.type	mas_blk_idle_notify_work,@function
mas_blk_idle_notify_work:               // @mas_blk_idle_notify_work
.Lmas_blk_idle_notify_work$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x20, x0
	ldrb	w8, [x20, #-8]!
	mov	x29, sp
	cbz	w8, .LBB4_2
// %bb.1:
	str	wzr, [x20, #232]
.LBB4_2:
	add	x19, x20, #240          // =240
	mov	x0, x19
	bl	mutex_lock
	ldr	w8, [x20, #232]
	cbnz	w8, .LBB4_7
// %bb.3:
	ldr	w8, [x20, #280]
	cmp	w8, #1                  // =1
	b.eq	.LBB4_7
// %bb.4:
	ldr	x8, [x20, #320]
	mov	w9, #1
	str	w9, [x20, #280]
	add	x8, x8, #1              // =1
	str	x8, [x20, #320]
	bl	ktime_get
	ldr	x8, [x20, #296]
	str	x0, [x20, #288]
	cbz	x8, .LBB4_6
// %bb.5:
	ldr	x9, [x20, #304]
	sub	x8, x0, x8
	add	x8, x8, x9
	str	x8, [x20, #304]
.LBB4_6:
	add	x0, x20, #168           // =168
	mov	w1, #1
	mov	x2, xzr
	bl	blocking_notifier_call_chain
.LBB4_7:
	mov	x0, x19
	bl	mutex_unlock
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end4:
	.size	mas_blk_idle_notify_work, .Lfunc_end4-mas_blk_idle_notify_work
                                        // -- End function
	.globl	blk_lld_idle_notify     // -- Begin function blk_lld_idle_notify
	.p2align	2
	.type	blk_lld_idle_notify,@function
blk_lld_idle_notify:                    // @blk_lld_idle_notify
.Lblk_lld_idle_notify$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	w8, [x0]
	mov	x29, sp
	cmp	w8, #122                // =122
	b.ne	.LBB5_2
// %bb.1:
	adrp	x8, blk_idle_notify_workqueue
	ldr	x19, [x8, :lo12:blk_idle_notify_workqueue]
	ldr	w8, [x0, #1032]
	add	x20, x0, #896           // =896
	mov	w0, w8
	bl	__msecs_to_jiffies
	mov	x3, x0
	mov	w0, #8
	mov	x1, x19
	mov	x2, x20
	bl	queue_delayed_work_on
.LBB5_2:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end5:
	.size	blk_lld_idle_notify, .Lfunc_end5-blk_lld_idle_notify
                                        // -- End function
	.globl	mas_queue_busyidle_enable_store // -- Begin function mas_queue_busyidle_enable_store
	.p2align	2
	.type	mas_queue_busyidle_enable_store,@function
mas_queue_busyidle_enable_store:        // @mas_queue_busyidle_enable_store
.Lmas_queue_busyidle_enable_store$local:
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x20, x0
	mov	x0, sp
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	str	x8, [sp, #8]
	str	xzr, [sp]
	bl	queue_var_store
	mov	x19, x0
	tbnz	x0, #63, .LBB6_4
// %bb.1:
	ldr	x8, [sp]
	cbz	x8, .LBB6_4
// %bb.2:
	adrp	x0, .L.str.4
	adrp	x1, .L__func__.mas_queue_busyidle_enable_store
	add	x0, x0, :lo12:.L.str.4
	add	x1, x1, :lo12:.L__func__.mas_queue_busyidle_enable_store
	bl	printk
	cbz	x20, .LBB6_7
// %bb.3:
	mov	x0, x20
	bl	mas_blk_get_lld
	ldr	x8, [x0, #24]
	orr	x8, x8, #0x8
	b	.LBB6_6
.LBB6_4:
	adrp	x0, .L.str.5
	adrp	x1, .L__func__.mas_queue_busyidle_enable_store
	add	x0, x0, :lo12:.L.str.5
	add	x1, x1, :lo12:.L__func__.mas_queue_busyidle_enable_store
	bl	printk
	cbz	x20, .LBB6_7
// %bb.5:
	mov	x0, x20
	bl	mas_blk_get_lld
	ldr	x8, [x0, #24]
	and	x8, x8, #0xfffffffffffffff7
.LBB6_6:
	str	x8, [x0, #24]
.LBB6_7:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB6_9
// %bb.8:
	mov	x0, x19
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB6_9:
	bl	__stack_chk_fail
.Lfunc_end6:
	.size	mas_queue_busyidle_enable_store, .Lfunc_end6-mas_queue_busyidle_enable_store
                                        // -- End function
	.globl	blk_queue_busyidle_enable // -- Begin function blk_queue_busyidle_enable
	.p2align	2
	.type	blk_queue_busyidle_enable,@function
blk_queue_busyidle_enable:              // @blk_queue_busyidle_enable
.Lblk_queue_busyidle_enable$local:
// %bb.0:
	cbz	x0, .LBB7_2
// %bb.1:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	w19, w1
	bl	mas_blk_get_lld
	ldr	x8, [x0, #24]
	cmp	w19, #0                 // =0
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	and	x9, x8, #0xfffffffffffffff7
	orr	x8, x8, #0x8
	csel	x8, x9, x8, eq
	str	x8, [x0, #24]
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
.LBB7_2:
	ret
.Lfunc_end7:
	.size	blk_queue_busyidle_enable, .Lfunc_end7-blk_queue_busyidle_enable
                                        // -- End function
	.globl	mas_queue_busyidle_statistic_reset_store // -- Begin function mas_queue_busyidle_statistic_reset_store
	.p2align	2
	.type	mas_queue_busyidle_statistic_reset_store,@function
mas_queue_busyidle_statistic_reset_store: // @mas_queue_busyidle_statistic_reset_store
.Lmas_queue_busyidle_statistic_reset_store$local:
// %bb.0:
	sub	sp, sp, #64             // =64
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x21, [sp, #32]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	mov	x19, x2
	mov	x21, x1
	str	x8, [sp, #8]
	str	xzr, [sp]
	bl	mas_blk_get_lld
	mov	x20, x0
	mov	x0, sp
	mov	x1, x21
	mov	x2, x19
	bl	queue_var_store
	mov	x19, x0
	tbnz	x0, #63, .LBB8_2
// %bb.1:
	ldr	x8, [sp]
	cbnz	x8, .LBB8_4
.LBB8_2:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB8_5
// %bb.3:
	mov	x0, x19
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldr	x21, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.LBB8_4:
	adrp	x0, .L.str.6
	adrp	x1, .L__func__.mas_queue_busyidle_statistic_reset_store
	add	x0, x0, :lo12:.L.str.6
	add	x1, x1, :lo12:.L__func__.mas_queue_busyidle_statistic_reset_store
	bl	printk
	add	x0, x20, #1216          // =1216
	mov	w2, #72
	mov	w1, wzr
	str	xzr, [x20, #1200]
	str	xzr, [x20, #1192]
	str	xzr, [x20, #1208]
	bl	memset
	str	xzr, [x20, #1288]
	b	.LBB8_2
.LBB8_5:
	bl	__stack_chk_fail
.Lfunc_end8:
	.size	mas_queue_busyidle_statistic_reset_store, .Lfunc_end8-mas_queue_busyidle_statistic_reset_store
                                        // -- End function
	.globl	mas_queue_busyidle_statistic_show // -- Begin function mas_queue_busyidle_statistic_show
	.p2align	2
	.type	mas_queue_busyidle_statistic_show,@function
mas_queue_busyidle_statistic_show:      // @mas_queue_busyidle_statistic_show
.Lmas_queue_busyidle_statistic_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x2
	mov	x20, x1
	bl	mas_blk_get_lld
	ldr	x8, [x0, #1192]
	ldr	x9, [x0, #1200]
	mov	x10, #13531
	movk	x10, #55222, lsl #16
	movk	x10, #56962, lsl #32
	movk	x10, #17179, lsl #48
	smulh	x8, x8, x10
	smulh	x9, x9, x10
	asr	x10, x8, #18
	asr	x11, x9, #18
	adrp	x2, .L.str.7
	mov	x21, x0
	add	x3, x10, x8, lsr #63
	add	x4, x11, x9, lsr #63
	add	x2, x2, :lo12:.L.str.7
	mov	x0, x20
	mov	x1, x19
	bl	snprintf
	ldr	x3, [x21, #1208]
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x22, w0
	adrp	x2, .L.str.8
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.8
	bl	snprintf
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.9
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.9
	bl	snprintf
	ldr	x3, [x21, #1288]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.10
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.10
	bl	snprintf
	ldr	x3, [x21, #1216]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.11
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.11
	bl	snprintf
	ldr	x3, [x21, #1224]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.12
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.12
	bl	snprintf
	ldr	x3, [x21, #1232]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.13
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.13
	bl	snprintf
	ldr	x3, [x21, #1240]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.14
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.14
	bl	snprintf
	ldr	x3, [x21, #1248]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.15
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.15
	bl	snprintf
	ldr	x3, [x21, #1256]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.16
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.16
	bl	snprintf
	ldr	x3, [x21, #1264]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.17
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.17
	bl	snprintf
	ldr	x3, [x21, #1272]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.18
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.18
	bl	snprintf
	ldr	x3, [x21, #1280]
	add	x22, x22, w0, sxtw
	adrp	x2, .L.str.19
	add	x0, x20, x22
	sub	x1, x19, x22
	add	x2, x2, :lo12:.L.str.19
	bl	snprintf
	ldr	x3, [x21, #1208]
	add	x21, x22, w0, sxtw
	adrp	x2, .L.str.20
	add	x0, x20, x21
	sub	x1, x19, x21
	add	x2, x2, :lo12:.L.str.20
	bl	snprintf
	add	x0, x21, w0, sxtw
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end9:
	.size	mas_queue_busyidle_statistic_show, .Lfunc_end9-mas_queue_busyidle_statistic_show
                                        // -- End function
	.globl	mas_queue_hw_idle_enable_show // -- Begin function mas_queue_hw_idle_enable_show
	.p2align	2
	.type	mas_queue_hw_idle_enable_show,@function
mas_queue_hw_idle_enable_show:          // @mas_queue_hw_idle_enable_show
.Lmas_queue_hw_idle_enable_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x2
	mov	x20, x1
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #888]
	adrp	x9, .L.str.22
	adrp	x10, .L.str.23
	add	x9, x9, :lo12:.L.str.22
	add	x10, x10, :lo12:.L.str.23
	cmp	w8, #0                  // =0
	adrp	x2, .L.str.21
	csel	x3, x10, x9, eq
	add	x2, x2, :lo12:.L.str.21
	mov	x0, x20
	mov	x1, x19
	bl	snprintf
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x0, w0
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end10:
	.size	mas_queue_hw_idle_enable_show, .Lfunc_end10-mas_queue_hw_idle_enable_show
                                        // -- End function
	.globl	mas_queue_idle_state_show // -- Begin function mas_queue_idle_state_show
	.p2align	2
	.type	mas_queue_idle_state_show,@function
mas_queue_idle_state_show:              // @mas_queue_idle_state_show
.Lmas_queue_idle_state_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x2
	mov	x20, x1
	bl	mas_blk_get_lld
	ldr	w8, [x0, #1168]
	adrp	x9, .L.str.26
	adrp	x10, .L.str.25
	add	x9, x9, :lo12:.L.str.26
	add	x10, x10, :lo12:.L.str.25
	cmp	w8, #1                  // =1
	adrp	x2, .L.str.24
	mov	x21, x0
	csel	x3, x10, x9, eq
	add	x2, x2, :lo12:.L.str.24
	mov	x0, x20
	mov	x1, x19
	bl	snprintf
	ldr	w3, [x21, #1120]
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x21, w0
	adrp	x2, .L.str.27
	add	x0, x20, x21
	sub	x1, x19, x21
	add	x2, x2, :lo12:.L.str.27
	bl	snprintf
	add	x0, x21, w0, sxtw
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end11:
	.size	mas_queue_idle_state_show, .Lfunc_end11-mas_queue_idle_state_show
                                        // -- End function
	.globl	mas_blk_busyidle_check_bio // -- Begin function mas_blk_busyidle_check_bio
	.p2align	2
	.type	mas_blk_busyidle_check_bio,@function
mas_blk_busyidle_check_bio:             // @mas_blk_busyidle_check_bio
.Lmas_blk_busyidle_check_bio$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x21, x1
	mov	x19, x0
	bl	mas_blk_get_lld
	ldrb	w8, [x21, #144]
	str	x19, [x21, #96]
	tbnz	w8, #0, .LBB12_7
// %bb.1:
	add	x22, x0, #1872          // =1872
	mov	x20, x0
	orr	w8, w8, #0x1
	mov	x0, x22
	strb	w8, [x21, #144]
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x20, #1824]
	ldr	x11, [x20, #1848]
	mov	x1, x0
	add	x9, x21, #336           // =336
	add	x10, x20, #1840         // =1840
	add	x8, x8, #1              // =1
	mov	x0, x22
	str	x9, [x20, #1848]
	str	x8, [x20, #1824]
	stp	x10, x11, [x21, #336]
	str	x9, [x11]
	bl	_raw_spin_unlock_irqrestore
	mov	x0, x20
	bl	blk_busy_count
	ldrb	w8, [x20, #25]
	tbz	w8, #1, .LBB12_7
// %bb.2:
	ldrb	w8, [x21, #298]
	cbz	w8, .LBB12_7
// %bb.3:
	ldrb	w8, [x21, #144]
	tbnz	w8, #5, .LBB12_7
// %bb.4:
	orr	w8, w8, #0x20
	strb	w8, [x21, #144]
	ldr	w8, [x20, #24]
	mov	w9, #520
	mvn	w8, w8
	tst	x8, x9
	b.ne	.LBB12_7
// %bb.5:
	ldr	w8, [x20, #1772]
	add	x22, x20, #1772         // =1772
	cbz	w8, .LBB12_8
.LBB12_6:
	//APP
	// atomic_add
	prfm	pstl1strm, [x22]
1:	ldxr	w8, [x22]
	add	w8, w8, 1
	stxr	w9, w8, [x22]
	cbnz	w9, 1b

	//NO_APP
.LBB12_7:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB12_8:
	add	x21, x20, #1776         // =1776
	mov	x0, x21
	bl	mutex_lock
	add	x0, x20, #1632          // =1632
	bl	cancel_delayed_work
	ldr	w8, [x20, #1816]
	cmp	w8, #1                  // =1
	b.ne	.LBB12_10
// %bb.9:
	add	x0, x20, #1056          // =1056
	mov	w1, #2
	mov	x2, x19
	str	wzr, [x20, #1816]
	bl	blocking_notifier_call_chain
.LBB12_10:
	mov	x0, x21
	bl	mutex_unlock
	b	.LBB12_6
.Lfunc_end12:
	.size	mas_blk_busyidle_check_bio, .Lfunc_end12-mas_blk_busyidle_check_bio
                                        // -- End function
	.p2align	2               // -- Begin function blk_busy_count
	.type	blk_busy_count,@function
blk_busy_count:                         // @blk_busy_count
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldrb	w8, [x0, #24]
	mov	x29, sp
	tbz	w8, #3, .LBB13_3
// %bb.1:
	ldr	w8, [x0, #1120]
	mov	x19, x0
	cbz	w8, .LBB13_4
.LBB13_2:
	add	x8, x19, #1120          // =1120
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB13_3:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB13_4:
	add	x20, x19, #1128         // =1128
	mov	x0, x20
	bl	mutex_lock
	add	x0, x19, #896           // =896
	bl	cancel_delayed_work
	ldr	w8, [x19, #1168]
	cmp	w8, #1                  // =1
	b.ne	.LBB13_29
// %bb.5:
	str	wzr, [x19, #1168]
	bl	ktime_get
	ldr	x8, [x19, #1176]
	str	x0, [x19, #1184]
	cbz	x8, .LBB13_28
// %bb.6:
	cbz	x0, .LBB13_27
// %bb.7:
	mov	x9, #13531
	movk	x9, #55222, lsl #16
	movk	x9, #56962, lsl #32
	movk	x9, #17179, lsl #48
	ldr	x10, [x19, #1288]
	smulh	x11, x0, x9
	smulh	x9, x8, x9
	asr	x12, x11, #18
	asr	x13, x9, #18
	add	x11, x12, x11, lsr #63
	add	x9, x13, x9, lsr #63
	sub	x9, x11, x9
	cmp	x9, x10
	b.le	.LBB13_9
// %bb.8:
	str	x9, [x19, #1288]
.LBB13_9:
	cmp	x9, #99                 // =99
	b.gt	.LBB13_11
// %bb.10:
	add	x9, x19, #1216          // =1216
	b	.LBB13_26
.LBB13_11:
	cmp	x9, #499                // =499
	b.gt	.LBB13_13
// %bb.12:
	add	x9, x19, #1224          // =1224
	b	.LBB13_26
.LBB13_13:
	cmp	x9, #999                // =999
	b.gt	.LBB13_15
// %bb.14:
	add	x9, x19, #1232          // =1232
	b	.LBB13_26
.LBB13_15:
	cmp	x9, #1999               // =1999
	b.gt	.LBB13_17
// %bb.16:
	add	x9, x19, #1240          // =1240
	b	.LBB13_26
.LBB13_17:
	cmp	x9, #3999               // =3999
	b.gt	.LBB13_19
// %bb.18:
	add	x9, x19, #1248          // =1248
	b	.LBB13_26
.LBB13_19:
	mov	w10, #5999
	cmp	x9, x10
	b.gt	.LBB13_21
// %bb.20:
	add	x9, x19, #1256          // =1256
	b	.LBB13_26
.LBB13_21:
	mov	w10, #7999
	cmp	x9, x10
	b.gt	.LBB13_23
// %bb.22:
	add	x9, x19, #1264          // =1264
	b	.LBB13_26
.LBB13_23:
	mov	w10, #9999
	cmp	x9, x10
	b.gt	.LBB13_25
// %bb.24:
	add	x9, x19, #1272          // =1272
	b	.LBB13_26
.LBB13_25:
	add	x9, x19, #1280          // =1280
.LBB13_26:
	ldr	x10, [x9]
	add	x10, x10, #1            // =1
	str	x10, [x9]
.LBB13_27:
	ldr	x9, [x19, #1200]
	sub	x8, x0, x8
	add	x8, x8, x9
	str	x8, [x19, #1200]
.LBB13_28:
	add	x0, x19, #1056          // =1056
	mov	x1, xzr
	mov	x2, xzr
	bl	blocking_notifier_call_chain
.LBB13_29:
	mov	x0, x20
	bl	mutex_unlock
	b	.LBB13_2
.Lfunc_end13:
	.size	blk_busy_count, .Lfunc_end13-blk_busy_count
                                        // -- End function
	.globl	mas_blk_busyidle_check_request_bio // -- Begin function mas_blk_busyidle_check_request_bio
	.p2align	2
	.type	mas_blk_busyidle_check_request_bio,@function
mas_blk_busyidle_check_request_bio:     // @mas_blk_busyidle_check_request_bio
.Lmas_blk_busyidle_check_request_bio$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	x19, [x1, #64]
	mov	x29, sp
	cbz	x19, .LBB14_4
// %bb.1:
	mov	x20, x0
.LBB14_2:                               // =>This Inner Loop Header: Depth=1
	mov	x0, x20
	mov	x1, x19
	bl	mas_blk_busyidle_check_bio
	ldr	x19, [x19]
	cbnz	x19, .LBB14_2
// %bb.3:
	mov	w0, #1
	b	.LBB14_5
.LBB14_4:
	mov	w0, wzr
.LBB14_5:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end14:
	.size	mas_blk_busyidle_check_request_bio, .Lfunc_end14-mas_blk_busyidle_check_request_bio
                                        // -- End function
	.globl	mas_blk_busyidle_end_rq // -- Begin function mas_blk_busyidle_end_rq
	.p2align	2
	.type	mas_blk_busyidle_end_rq,@function
mas_blk_busyidle_end_rq:                // @mas_blk_busyidle_end_rq
.Lmas_blk_busyidle_end_rq$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x19, x0
	ldr	x0, [x0]
	mov	x29, sp
	mov	w20, w1
	bl	mas_blk_get_lld
	ldr	x8, [x0, #1832]
	cbz	x8, .LBB15_10
// %bb.1:
	add	x22, x0, #1872          // =1872
	mov	x21, x0
	mov	x0, x22
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x21, #1832]
	mov	x1, x0
	add	x10, x19, #784          // =784
	mov	x0, x22
	sub	x8, x8, #1              // =1
	str	x8, [x21, #1832]
	ldr	x8, [x19, #792]
	ldr	x9, [x19, #784]
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x10, [x19, #784]
	str	x10, [x19, #792]
	bl	_raw_spin_unlock_irqrestore
	ldrb	w8, [x21, #24]
	tbz	w8, #3, .LBB15_5
// %bb.2:
	ldrb	w8, [x21, #888]
	cbnz	w8, .LBB15_5
// %bb.3:
	ldr	w8, [x21, #1120]
	cbz	w8, .LBB15_11
// %bb.4:
	add	x8, x21, #1120          // =1120
	mov	w9, #1
	//APP
	// atomic_sub_return
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stlxr	w11, w10, [x8]
	cbnz	w11, 1b
	dmb ish
	//NO_APP
	cbz	w10, .LBB15_8
.LBB15_5:
	ldr	x8, [x19, #280]
	cbz	x8, .LBB15_7
// %bb.6:
	mov	x0, x19
	mov	w1, w20
	blr	x8
.LBB15_7:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB15_8:
	ldr	w8, [x21]
	cmp	w8, #122                // =122
	b.ne	.LBB15_5
// %bb.9:
	adrp	x8, blk_idle_notify_workqueue
	ldr	w0, [x21, #1032]
	ldr	x22, [x8, :lo12:blk_idle_notify_workqueue]
	add	x21, x21, #896          // =896
	bl	__msecs_to_jiffies
	mov	x3, x0
	mov	w0, #8
	mov	x1, x22
	mov	x2, x21
	bl	queue_delayed_work_on
	b	.LBB15_5
.LBB15_10:
	adrp	x0, .L.str.35
	add	x0, x0, :lo12:.L.str.35
	bl	mas_blk_rdr_panic
.LBB15_11:
	adrp	x0, .L.str.36
	adrp	x1, .L__func__.blk_idle_count
	add	x0, x0, :lo12:.L.str.36
	add	x1, x1, :lo12:.L__func__.blk_idle_count
	bl	printk
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.Lfunc_end15:
	.size	mas_blk_busyidle_end_rq, .Lfunc_end15-mas_blk_busyidle_end_rq
                                        // -- End function
	.globl	mas_blk_busyidle_check_execute_request // -- Begin function mas_blk_busyidle_check_execute_request
	.p2align	2
	.type	mas_blk_busyidle_check_execute_request,@function
mas_blk_busyidle_check_execute_request: // @mas_blk_busyidle_check_execute_request
.Lmas_blk_busyidle_check_execute_request$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x21, [x1, #64]
	mov	x19, x2
	mov	x20, x1
	mov	x29, sp
	cbz	x21, .LBB16_4
// %bb.1:
	mov	x22, x0
.LBB16_2:                               // =>This Inner Loop Header: Depth=1
	mov	x0, x22
	mov	x1, x21
	bl	mas_blk_busyidle_check_bio
	ldr	x21, [x21]
	cbnz	x21, .LBB16_2
// %bb.3:
	add	x8, x20, #880           // =880
	b	.LBB16_5
.LBB16_4:
	ldr	x0, [x20]
	bl	mas_blk_get_lld
	add	x22, x0, #1872          // =1872
	mov	x21, x0
	mov	x0, x22
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x21, #1832]
	ldr	x11, [x21, #1864]
	mov	x1, x0
	add	x9, x20, #784           // =784
	add	x10, x21, #1856         // =1856
	add	x8, x8, #1              // =1
	mov	x0, x22
	str	x9, [x21, #1864]
	str	x8, [x21, #1832]
	str	x10, [x20, #784]
	str	x11, [x20, #792]
	str	x9, [x11]
	bl	_raw_spin_unlock_irqrestore
	mov	x0, x21
	bl	blk_busy_count
	adrp	x8, __cfi_mas_blk_busyidle_end_rq
	add	x8, x8, :lo12:__cfi_mas_blk_busyidle_end_rq
	str	x8, [x20, #880]
	add	x8, x20, #280           // =280
.LBB16_5:
	str	x19, [x8]
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end16:
	.size	mas_blk_busyidle_check_execute_request, .Lfunc_end16-mas_blk_busyidle_check_execute_request
                                        // -- End function
	.globl	mas_blk_busyidle_check_bio_endio // -- Begin function mas_blk_busyidle_check_bio_endio
	.p2align	2
	.type	mas_blk_busyidle_check_bio_endio,@function
mas_blk_busyidle_check_bio_endio:       // @mas_blk_busyidle_check_bio_endio
.Lmas_blk_busyidle_check_bio_endio$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x0, #8]
	add	x9, x0, #96             // =96
	mov	x20, x0
	mov	x29, sp
	add	x10, x8, #1272          // =1272
	cmp	x8, #0                  // =0
	csel	x8, x9, x10, eq
	ldr	x0, [x8]
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #25]
	mov	x19, x0
	tbz	w8, #1, .LBB17_6
// %bb.1:
	ldrb	w8, [x20, #298]
	cbz	w8, .LBB17_6
// %bb.2:
	ldrb	w8, [x20, #144]
	tbz	w8, #5, .LBB17_6
// %bb.3:
	and	w8, w8, #0xffffffdf
	strb	w8, [x20, #144]
	ldr	w8, [x19, #24]
	mov	w9, #520
	mvn	w8, w8
	tst	x8, x9
	b.ne	.LBB17_6
// %bb.4:
	ldr	w8, [x19, #1772]
	cbz	w8, .LBB17_19
// %bb.5:
	add	x8, x19, #1772          // =1772
	mov	w9, #1
	//APP
	// atomic_sub_return
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stlxr	w11, w10, [x8]
	cbnz	w11, 1b
	dmb ish
	//NO_APP
	cbz	w10, .LBB17_15
.LBB17_6:
	ldrb	w8, [x20, #144]
	tbz	w8, #0, .LBB17_12
// %bb.7:
	ldr	x8, [x19, #1824]
	cbz	x8, .LBB17_17
// %bb.8:
	add	x21, x19, #1872         // =1872
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x19, #1824]
	mov	x1, x0
	add	x10, x20, #336          // =336
	mov	x0, x21
	sub	x8, x8, #1              // =1
	str	x8, [x19, #1824]
	ldp	x9, x8, [x20, #336]
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x10, [x20, #336]
	str	x10, [x20, #344]
	bl	_raw_spin_unlock_irqrestore
	strb	wzr, [x20, #144]
	ldrb	w8, [x19, #24]
	tbz	w8, #3, .LBB17_12
// %bb.9:
	ldrb	w8, [x19, #888]
	cbnz	w8, .LBB17_12
// %bb.10:
	ldr	w8, [x19, #1120]
	cbz	w8, .LBB17_18
// %bb.11:
	add	x8, x19, #1120          // =1120
	mov	w9, #1
	//APP
	// atomic_sub_return
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stlxr	w11, w10, [x8]
	cbnz	w11, 1b
	dmb ish
	//NO_APP
	cbz	w10, .LBB17_13
.LBB17_12:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB17_13:
	ldr	w8, [x19]
	cmp	w8, #122                // =122
	b.ne	.LBB17_12
// %bb.14:
	adrp	x8, blk_idle_notify_workqueue
	ldr	w0, [x19, #1032]
	ldr	x20, [x8, :lo12:blk_idle_notify_workqueue]
	add	x19, x19, #896          // =896
	bl	__msecs_to_jiffies
	mov	x3, x0
	mov	w0, #8
	mov	x1, x20
	mov	x2, x19
	bl	queue_delayed_work_on
	b	.LBB17_12
.LBB17_15:
	ldr	w8, [x19]
	cmp	w8, #122                // =122
	b.ne	.LBB17_6
// %bb.16:
	adrp	x8, blk_fg_io_idle_notify_workqueue
	ldr	w0, [x19, #1768]
	ldr	x21, [x8, :lo12:blk_fg_io_idle_notify_workqueue]
	add	x22, x19, #1632         // =1632
	bl	__msecs_to_jiffies
	mov	x3, x0
	mov	w0, #8
	mov	x1, x21
	mov	x2, x22
	bl	queue_delayed_work_on
	b	.LBB17_6
.LBB17_17:
	adrp	x0, .L.str.39
	add	x0, x0, :lo12:.L.str.39
	bl	mas_blk_rdr_panic
.LBB17_18:
	adrp	x0, .L.str.36
	adrp	x1, .L__func__.blk_idle_count
	add	x0, x0, :lo12:.L.str.36
	add	x1, x1, :lo12:.L__func__.blk_idle_count
	bl	printk
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.LBB17_19:
	adrp	x0, .L.str.37
	adrp	x1, .L__func__.blk_fg_io_idle_count
	add	x0, x0, :lo12:.L.str.37
	add	x1, x1, :lo12:.L__func__.blk_fg_io_idle_count
	bl	printk
	adrp	x0, .L.str.38
	add	x0, x0, :lo12:.L.str.38
	bl	mas_blk_rdr_panic
.Lfunc_end17:
	.size	mas_blk_busyidle_check_bio_endio, .Lfunc_end17-mas_blk_busyidle_check_bio_endio
                                        // -- End function
	.globl	blk_busyidle_event_subscribe // -- Begin function blk_busyidle_event_subscribe
	.p2align	2
	.type	blk_busyidle_event_subscribe,@function
blk_busyidle_event_subscribe:           // @blk_busyidle_event_subscribe
.Lblk_busyidle_event_subscribe$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	mov	x8, x0
	mov	w0, #-22
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x8, .LBB18_5
// %bb.1:
	mov	x19, x1
	cbz	x1, .LBB18_5
// %bb.2:
	ldr	x8, [x8, #136]
	ldr	x0, [x8, #1272]
	cbz	x0, .LBB18_4
// %bb.3:
	bl	mas_blk_get_lld
	mov	x1, x19
	bl	blk_lld_busyidle_event_subscribe
	b	.LBB18_5
.LBB18_4:
	mov	w0, #-22
.LBB18_5:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end18:
	.size	blk_busyidle_event_subscribe, .Lfunc_end18-blk_busyidle_event_subscribe
                                        // -- End function
	.globl	blk_queue_busyidle_event_subscribe // -- Begin function blk_queue_busyidle_event_subscribe
	.p2align	2
	.type	blk_queue_busyidle_event_subscribe,@function
blk_queue_busyidle_event_subscribe:     // @blk_queue_busyidle_event_subscribe
.Lblk_queue_busyidle_event_subscribe$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	mov	x8, x0
	mov	w0, #-22
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x8, .LBB19_3
// %bb.1:
	mov	x19, x1
	cbz	x1, .LBB19_3
// %bb.2:
	mov	x0, x8
	bl	mas_blk_get_lld
	mov	x1, x19
	bl	blk_lld_busyidle_event_subscribe
.LBB19_3:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end19:
	.size	blk_queue_busyidle_event_subscribe, .Lfunc_end19-blk_queue_busyidle_event_subscribe
                                        // -- End function
	.globl	blk_lld_busyidle_event_subscribe // -- Begin function blk_lld_busyidle_event_subscribe
	.p2align	2
	.type	blk_lld_busyidle_event_subscribe,@function
blk_lld_busyidle_event_subscribe:       // @blk_lld_busyidle_event_subscribe
.Lblk_lld_busyidle_event_subscribe$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x19, x0
	mov	w0, #-22
	stp	x24, x23, [sp, #16]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x19, .LBB20_18
// %bb.1:
	mov	x22, x1
	cbz	x1, .LBB20_18
// %bb.2:
	str	x19, [x22, #48]
	adrp	x8, kmalloc_caches+16
	ldr	x0, [x8, :lo12:kmalloc_caches+16]
	mov	w1, #3520
	mov	w2, #160
	bl	kmem_cache_alloc_trace
	cbz	x0, .LBB20_17
// %bb.3:
	add	x20, x0, #96            // =96
	mov	x21, x0
	mov	w2, #56
	mov	x0, x20
	mov	x1, x22
	bl	memcpy
	ldr	x8, [x21, #128]
	cbz	x8, .LBB20_20
// %bb.4:
	mov	x0, x20
	bl	strlen
	cbz	x0, .LBB20_19
// %bb.5:
	mov	x0, x20
	bl	strlen
	cmp	x0, #32                 // =32
	b.hs	.LBB20_19
// %bb.6:
	add	x22, x19, #1128         // =1128
	mov	x0, x22
	bl	mutex_lock
	ldrb	w8, [x19, #25]
	tbz	w8, #1, .LBB20_8
// %bb.7:
	add	x0, x19, #1776          // =1776
	bl	mutex_lock
.LBB20_8:
	ldr	x24, [x19, #1040]
	add	x23, x19, #1040         // =1040
	b	.LBB20_10
.LBB20_9:                               //   in Loop: Header=BB20_10 Depth=1
	ldr	x24, [x24]
.LBB20_10:                              // =>This Inner Loop Header: Depth=1
	cmp	x24, x23
	b.eq	.LBB20_14
// %bb.11:                              //   in Loop: Header=BB20_10 Depth=1
	add	x1, x24, #96            // =96
	mov	x0, x20
	bl	strcmp
	cbnz	w0, .LBB20_9
// %bb.12:                              //   in Loop: Header=BB20_10 Depth=1
	ldr	x8, [x21, #128]
	ldr	x9, [x24, #128]
	cmp	x8, x9
	b.ne	.LBB20_9
// %bb.13:
	cbnz	x24, .LBB20_22
.LBB20_14:
	adrp	x1, __cfi_mas_blk_busyidle_handler_latency_check_timer_expire
	add	x0, x21, #16            // =16
	add	x1, x1, :lo12:__cfi_mas_blk_busyidle_handler_latency_check_timer_expire
	mov	w2, wzr
	mov	x3, xzr
	mov	x4, xzr
	bl	init_timer_key
	adrp	x8, __cfi_mas_blk_busyidle_notify_handler
	add	x8, x8, :lo12:__cfi_mas_blk_busyidle_notify_handler
	mov	x1, x21
	mov	x9, #4294967297
	str	x8, [x1, #72]!
	str	wzr, [x21, #88]
	str	x9, [x21, #152]
	ldr	x8, [x19, #1048]
	add	x0, x19, #1056          // =1056
	str	x21, [x19, #1048]
	stp	x23, x8, [x21]
	str	x21, [x8]
	bl	blocking_notifier_chain_register
	adrp	x0, .L.str.42
	add	x0, x0, :lo12:.L.str.42
	mov	x1, x20
	bl	printk
	ldrb	w8, [x19, #25]
	tbz	w8, #1, .LBB20_16
// %bb.15:
	add	x0, x19, #1776          // =1776
	bl	mutex_unlock
.LBB20_16:
	mov	x0, x22
	bl	mutex_unlock
	mov	w0, wzr
	b	.LBB20_18
.LBB20_17:
	mov	w0, #-12
.LBB20_18:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB20_19:
	adrp	x0, .L.str.44
	add	x0, x0, :lo12:.L.str.44
	b	.LBB20_21
.LBB20_20:
	adrp	x0, .L.str.43
	add	x0, x0, :lo12:.L.str.43
.LBB20_21:
	adrp	x1, .L__func__.is_valid_event_node
	add	x1, x1, :lo12:.L__func__.is_valid_event_node
	bl	printk
	adrp	x0, .L.str.40
	add	x0, x0, :lo12:.L.str.40
	bl	mas_blk_rdr_panic
.LBB20_22:
	adrp	x0, .L.str.41
	adrp	x1, .L__func__.mas_blk_busyidle_event_register
	add	x0, x0, :lo12:.L.str.41
	add	x1, x1, :lo12:.L__func__.mas_blk_busyidle_event_register
	mov	x2, x20
	bl	printk
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.Lfunc_end20:
	.size	blk_lld_busyidle_event_subscribe, .Lfunc_end20-blk_lld_busyidle_event_subscribe
                                        // -- End function
	.globl	blk_busyidle_event_unsubscribe // -- Begin function blk_busyidle_event_unsubscribe
	.p2align	2
	.type	blk_busyidle_event_unsubscribe,@function
blk_busyidle_event_unsubscribe:         // @blk_busyidle_event_unsubscribe
.Lblk_busyidle_event_unsubscribe$local:
// %bb.0:
	cbz	x0, .LBB21_2
// %bb.1:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	bl	mas_blk_busyidle_event_unregister
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB21_2:
	mov	w0, #-22
	ret
.Lfunc_end21:
	.size	blk_busyidle_event_unsubscribe, .Lfunc_end21-blk_busyidle_event_unsubscribe
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_busyidle_event_unregister
	.type	mas_blk_busyidle_event_unregister,@function
mas_blk_busyidle_event_unregister:      // @mas_blk_busyidle_event_unregister
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x24, x23, [sp, #16]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	ldr	x23, [x0, #48]
	mov	x21, x0
	mov	x29, sp
	add	x19, x23, #1128         // =1128
	mov	x0, x19
	bl	mutex_lock
	ldrb	w8, [x23, #25]
	tbz	w8, #1, .LBB22_2
// %bb.1:
	add	x0, x23, #1776          // =1776
	bl	mutex_lock
.LBB22_2:
	ldr	x20, [x23, #1040]
	add	x24, x23, #1040         // =1040
	b	.LBB22_4
.LBB22_3:                               //   in Loop: Header=BB22_4 Depth=1
	ldr	x20, [x20]
.LBB22_4:                               // =>This Inner Loop Header: Depth=1
	cmp	x20, x24
	b.eq	.LBB22_8
// %bb.5:                               //   in Loop: Header=BB22_4 Depth=1
	add	x22, x20, #96           // =96
	mov	x0, x21
	mov	x1, x22
	bl	strcmp
	cbnz	w0, .LBB22_3
// %bb.6:                               //   in Loop: Header=BB22_4 Depth=1
	ldr	x8, [x21, #32]
	ldr	x9, [x20, #128]
	cmp	x8, x9
	b.ne	.LBB22_3
// %bb.7:
	cbnz	x20, .LBB22_11
.LBB22_8:
	adrp	x0, .L.str.45
	add	x0, x0, :lo12:.L.str.45
	bl	printk
	bl	dump_stack
	mov	w20, #-22
	ldrb	w8, [x23, #25]
	tbz	w8, #1, .LBB22_10
.LBB22_9:
	add	x0, x23, #1776          // =1776
	bl	mutex_unlock
.LBB22_10:
	mov	x0, x19
	bl	mutex_unlock
	mov	w0, w20
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB22_11:
	add	x21, x20, #72           // =72
	add	x0, x23, #1056          // =1056
	mov	x1, x21
	bl	blocking_notifier_chain_unregister
	adrp	x0, .L.str.46
	add	x0, x0, :lo12:.L.str.46
	mov	x1, x22
	mov	x2, x21
	bl	printk
	ldp	x9, x8, [x20]
	mov	x0, x20
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x20, [x20]
	str	x20, [x20, #8]
	bl	kfree
	mov	w20, wzr
	ldrb	w8, [x23, #25]
	tbnz	w8, #1, .LBB22_9
	b	.LBB22_10
.Lfunc_end22:
	.size	mas_blk_busyidle_event_unregister, .Lfunc_end22-mas_blk_busyidle_event_unregister
                                        // -- End function
	.globl	blk_queue_busyidle_event_unsubscribe // -- Begin function blk_queue_busyidle_event_unsubscribe
	.p2align	2
	.type	blk_queue_busyidle_event_unsubscribe,@function
blk_queue_busyidle_event_unsubscribe:   // @blk_queue_busyidle_event_unsubscribe
.Lblk_queue_busyidle_event_unsubscribe$local:
// %bb.0:
	cbz	x0, .LBB23_2
// %bb.1:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	bl	mas_blk_busyidle_event_unregister
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB23_2:
	mov	w0, #-22
	ret
.Lfunc_end23:
	.size	blk_queue_busyidle_event_unsubscribe, .Lfunc_end23-blk_queue_busyidle_event_unsubscribe
                                        // -- End function
	.globl	blk_mq_tagset_busyidle_enable // -- Begin function blk_mq_tagset_busyidle_enable
	.p2align	2
	.type	blk_mq_tagset_busyidle_enable,@function
blk_mq_tagset_busyidle_enable:          // @blk_mq_tagset_busyidle_enable
.Lblk_mq_tagset_busyidle_enable$local:
// %bb.0:
	cbz	x0, .LBB24_2
// %bb.1:
	ldr	x8, [x0, #88]
	cmp	w1, #0                  // =0
	and	x9, x8, #0xfffffffffffffff7
	orr	x8, x8, #0x8
	csel	x8, x9, x8, eq
	str	x8, [x0, #88]
.LBB24_2:
	ret
.Lfunc_end24:
	.size	blk_mq_tagset_busyidle_enable, .Lfunc_end24-blk_mq_tagset_busyidle_enable
                                        // -- End function
	.globl	blk_mq_tagset_hw_idle_notify_enable // -- Begin function blk_mq_tagset_hw_idle_notify_enable
	.p2align	2
	.type	blk_mq_tagset_hw_idle_notify_enable,@function
blk_mq_tagset_hw_idle_notify_enable:    // @blk_mq_tagset_hw_idle_notify_enable
.Lblk_mq_tagset_hw_idle_notify_enable$local:
// %bb.0:
	cbz	x0, .LBB25_2
// %bb.1:
	cmp	w1, #0                  // =0
	cset	w8, ne
	strb	w8, [x0, #952]
.LBB25_2:
	ret
.Lfunc_end25:
	.size	blk_mq_tagset_hw_idle_notify_enable, .Lfunc_end25-blk_mq_tagset_hw_idle_notify_enable
                                        // -- End function
	.globl	mas_blk_busyidle_state_init // -- Begin function mas_blk_busyidle_state_init
	.p2align	2
	.type	mas_blk_busyidle_state_init,@function
mas_blk_busyidle_state_init:            // @mas_blk_busyidle_state_init
.Lmas_blk_busyidle_state_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	adrp	x20, blk_idle_notify_workqueue
	ldr	x8, [x20, :lo12:blk_idle_notify_workqueue]
	mov	x19, x0
	str	x23, [sp, #16]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbnz	x8, .LBB26_2
// %bb.1:
	adrp	x0, .L.str.28
	add	x0, x0, :lo12:.L.str.28
	mov	w1, #24
	mov	w2, wzr
	bl	alloc_workqueue
	str	x0, [x20, :lo12:blk_idle_notify_workqueue]
	cbz	x0, .LBB26_6
.LBB26_2:
	adrp	x20, blk_fg_io_idle_notify_workqueue
	ldr	x8, [x20, :lo12:blk_fg_io_idle_notify_workqueue]
	cbnz	x8, .LBB26_4
// %bb.3:
	adrp	x0, .L.str.47
	add	x0, x0, :lo12:.L.str.47
	mov	w1, #24
	mov	w2, wzr
	bl	alloc_workqueue
	str	x0, [x20, :lo12:blk_fg_io_idle_notify_workqueue]
	cbz	x0, .LBB26_7
.LBB26_4:
	add	x0, x19, #328           // =328
	mov	w2, #72
	mov	w1, wzr
	stp	xzr, xzr, [x19, #304]
	stp	xzr, xzr, [x19, #288]
	bl	memset
	adrp	x20, delayed_work_timer_fn
	adrp	x9, __cfi_mas_blk_idle_notify_work
	add	x20, x20, :lo12:delayed_work_timer_fn
	mov	w21, #5
	mov	x22, #68719476704
	add	x8, x19, #16            // =16
	add	x9, x9, :lo12:__cfi_mas_blk_idle_notify_work
	add	x0, x19, #56            // =56
	mov	w2, #2097152
	mov	x1, x20
	mov	x3, xzr
	mov	x4, xzr
	str	xzr, [x19, #400]
	str	xzr, [x19, #320]
	str	w21, [x19, #144]
	str	x22, [x19, #8]
	str	x8, [x19, #16]
	stp	x8, x9, [x19, #24]
	bl	init_timer_key
	adrp	x1, .L.str.30
	adrp	x2, mas_blk_busyidle_state_init.__key
	add	x0, x19, #240           // =240
	add	x1, x1, :lo12:.L.str.30
	add	x2, x2, :lo12:mas_blk_busyidle_state_init.__key
	str	wzr, [x19, #232]
	bl	__mutex_init
	adrp	x1, .L.str.32
	adrp	x2, mas_blk_busyidle_state_init.__key.31
	add	x8, x19, #152           // =152
	add	x0, x19, #168           // =168
	add	x1, x1, :lo12:.L.str.32
	add	x2, x2, :lo12:mas_blk_busyidle_state_init.__key.31
	str	x8, [x19, #152]
	str	x8, [x19, #160]
	bl	__init_rwsem
	adrp	x9, mas_blk_fg_io_idle_notify_work
	mov	w23, #1
	add	x8, x19, #752           // =752
	add	x9, x9, :lo12:mas_blk_fg_io_idle_notify_work
	add	x0, x19, #792           // =792
	mov	w2, #2097152
	mov	x1, x20
	mov	x3, xzr
	mov	x4, xzr
	str	xzr, [x19, #224]
	str	w21, [x19, #880]
	str	x22, [x19, #744]
	str	w23, [x19, #280]
	str	x8, [x19, #752]
	str	x8, [x19, #760]
	str	x9, [x19, #768]
	bl	init_timer_key
	adrp	x1, .L.str.48
	adrp	x2, mas_blk_fg_io_busyidle_state_init.__key
	add	x0, x19, #888           // =888
	add	x1, x1, :lo12:.L.str.48
	add	x2, x2, :lo12:mas_blk_fg_io_busyidle_state_init.__key
	str	wzr, [x19, #884]
	bl	__mutex_init
	mov	w0, wzr
	add	x8, x19, #952           // =952
	add	x9, x19, #968           // =968
	str	w23, [x19, #928]
	str	xzr, [x19, #936]
	str	xzr, [x19, #944]
	str	x8, [x19, #952]
	str	x8, [x19, #960]
	str	x9, [x19, #968]
	str	x9, [x19, #976]
	str	wzr, [x19, #984]
.LBB26_5:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB26_6:
	adrp	x0, .L.str.29
	adrp	x1, .L__func__.mas_blk_busyidle_state_init
	add	x0, x0, :lo12:.L.str.29
	add	x1, x1, :lo12:.L__func__.mas_blk_busyidle_state_init
	b	.LBB26_8
.LBB26_7:
	adrp	x0, .L.str.29
	adrp	x1, .L__func__.mas_blk_alloc_fg_io_workqueue
	add	x0, x0, :lo12:.L.str.29
	add	x1, x1, :lo12:.L__func__.mas_blk_alloc_fg_io_workqueue
.LBB26_8:
	bl	printk
	mov	w0, #-12
	b	.LBB26_5
.Lfunc_end26:
	.size	mas_blk_busyidle_state_init, .Lfunc_end26-mas_blk_busyidle_state_init
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_fg_io_idle_notify_work
	.type	mas_blk_fg_io_idle_notify_work,@function
mas_blk_fg_io_idle_notify_work:         // @mas_blk_fg_io_idle_notify_work
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	x21, x0
	sub	x0, x0, #1632           // =1632
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	bl	mas_blk_get_queue_by_lld
	add	x19, x21, #144          // =144
	mov	x20, x0
	mov	x0, x19
	bl	mutex_lock
	ldr	w8, [x21, #140]
	cbnz	w8, .LBB27_3
// %bb.1:
	sub	x8, x21, #744           // =744
	ldr	w9, [x8, #928]
	cmp	w9, #1                  // =1
	b.eq	.LBB27_3
// %bb.2:
	mov	w9, #1
	add	x0, x8, #168            // =168
	mov	w1, #3
	mov	x2, x20
	str	w9, [x8, #928]
	bl	blocking_notifier_call_chain
.LBB27_3:
	mov	x0, x19
	bl	mutex_unlock
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end27:
	.size	mas_blk_fg_io_idle_notify_work, .Lfunc_end27-mas_blk_fg_io_idle_notify_work
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\0013[BLK-IO]%s: %s process time is more than %u ms\n"
	.size	.L.str, 50

	.type	.L__func__.mas_blk_busyidle_handler_latency_check_timer_expire,@object // @__func__.mas_blk_busyidle_handler_latency_check_timer_expire
.L__func__.mas_blk_busyidle_handler_latency_check_timer_expire:
	.asciz	"mas_blk_busyidle_handler_latency_check_timer_expire"
	.size	.L__func__.mas_blk_busyidle_handler_latency_check_timer_expire, 52

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"\0013[BLK-IO]%s: %s NULL callback\n"
	.size	.L.str.1, 32

	.type	.L__func__.mas_blk_busyidle_notify_handler,@object // @__func__.mas_blk_busyidle_notify_handler
.L__func__.mas_blk_busyidle_notify_handler:
	.asciz	"mas_blk_busyidle_notify_handler"
	.size	.L__func__.mas_blk_busyidle_notify_handler, 32

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"\0013[BLK-IO]%s: %s error!\n"
	.size	.L.str.2, 25

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"\0013[BLK-IO]%s: %s busy idle callback cost %lld ms\n"
	.size	.L.str.3, 50

	.type	blk_fg_io_idle_notify_workqueue,@object // @blk_fg_io_idle_notify_workqueue
	.local	blk_fg_io_idle_notify_workqueue
	.comm	blk_fg_io_idle_notify_workqueue,8,8
	.type	blk_idle_notify_workqueue,@object // @blk_idle_notify_workqueue
	.local	blk_idle_notify_workqueue
	.comm	blk_idle_notify_workqueue,8,8
	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"\0013[BLK-IO]%s: busy idle enable\n"
	.size	.L.str.4, 32

	.type	.L__func__.mas_queue_busyidle_enable_store,@object // @__func__.mas_queue_busyidle_enable_store
.L__func__.mas_queue_busyidle_enable_store:
	.asciz	"mas_queue_busyidle_enable_store"
	.size	.L__func__.mas_queue_busyidle_enable_store, 32

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"\0013[BLK-IO]%s: busy idle disable\n"
	.size	.L.str.5, 33

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"\0013[BLK-IO]%s: io busy idle statistic result reset!\n"
	.size	.L.str.6, 52

	.type	.L__func__.mas_queue_busyidle_statistic_reset_store,@object // @__func__.mas_queue_busyidle_statistic_reset_store
.L__func__.mas_queue_busyidle_statistic_reset_store:
	.asciz	"mas_queue_busyidle_statistic_reset_store"
	.size	.L__func__.mas_queue_busyidle_statistic_reset_store, 41

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"Total Busy Time: %lld ms  Total Idle Time: %lld ms\r\n"
	.size	.L.str.7, 53

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"Total Idle Count: %llu \r\n"
	.size	.L.str.8, 26

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"block idle interval statistic:\n"
	.size	.L.str.9, 32

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"max_idle_duration: %lldms\n"
	.size	.L.str.10, 27

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"less than 100ms: %lld\n"
	.size	.L.str.11, 23

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"less than 500ms: %lld\n"
	.size	.L.str.12, 23

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"less than 1s: %lld\n"
	.size	.L.str.13, 20

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"less than 2s: %lld\n"
	.size	.L.str.14, 20

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"less than 4s: %lld\n"
	.size	.L.str.15, 20

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"less than 6s: %lld\n"
	.size	.L.str.16, 20

	.type	.L.str.17,@object       // @.str.17
.L.str.17:
	.asciz	"less than 8s: %lld\n"
	.size	.L.str.17, 20

	.type	.L.str.18,@object       // @.str.18
.L.str.18:
	.asciz	"less than 10s: %lld\n"
	.size	.L.str.18, 21

	.type	.L.str.19,@object       // @.str.19
.L.str.19:
	.asciz	"more than 10s: %lld\n"
	.size	.L.str.19, 21

	.type	.L.str.20,@object       // @.str.20
.L.str.20:
	.asciz	"idle total counts: %llu\n"
	.size	.L.str.20, 25

	.type	.L.str.21,@object       // @.str.21
.L.str.21:
	.asciz	"%s\n"
	.size	.L.str.21, 4

	.type	.L.str.22,@object       // @.str.22
.L.str.22:
	.asciz	"enabled"
	.size	.L.str.22, 8

	.type	.L.str.23,@object       // @.str.23
.L.str.23:
	.asciz	"disabled"
	.size	.L.str.23, 9

	.type	.L.str.24,@object       // @.str.24
.L.str.24:
	.asciz	"idle_state: %s\n"
	.size	.L.str.24, 16

	.type	.L.str.25,@object       // @.str.25
.L.str.25:
	.asciz	"idle"
	.size	.L.str.25, 5

	.type	.L.str.26,@object       // @.str.26
.L.str.26:
	.asciz	"busy"
	.size	.L.str.26, 5

	.type	.L.str.27,@object       // @.str.27
.L.str.27:
	.asciz	"io_count: %d\n"
	.size	.L.str.27, 14

	.type	.L.str.28,@object       // @.str.28
.L.str.28:
	.asciz	"busyidle_notify"
	.size	.L.str.28, 16

	.type	.L.str.29,@object       // @.str.29
.L.str.29:
	.asciz	"\0013[BLK-IO]%s: Failed to alloc workqueue!\n"
	.size	.L.str.29, 42

	.type	.L__func__.mas_blk_busyidle_state_init,@object // @__func__.mas_blk_busyidle_state_init
.L__func__.mas_blk_busyidle_state_init:
	.asciz	"mas_blk_busyidle_state_init"
	.size	.L__func__.mas_blk_busyidle_state_init, 28

	.type	mas_blk_busyidle_state_init.__key,@object // @mas_blk_busyidle_state_init.__key
	.local	mas_blk_busyidle_state_init.__key
	.comm	mas_blk_busyidle_state_init.__key,1,1
	.type	.L.str.30,@object       // @.str.30
.L.str.30:
	.asciz	"&blk_idle->io_count_mutex"
	.size	.L.str.30, 26

	.type	mas_blk_busyidle_state_init.__key.31,@object // @mas_blk_busyidle_state_init.__key.31
	.local	mas_blk_busyidle_state_init.__key.31
	.comm	mas_blk_busyidle_state_init.__key.31,1,1
	.type	.L.str.32,@object       // @.str.32
.L.str.32:
	.asciz	"&(&blk_idle->nh)->rwsem"
	.size	.L.str.32, 24

	.type	.L.str.33,@object       // @.str.33
.L.str.33:
	.asciz	"\0013[BLK-IO]%s\n"
	.size	.L.str.33, 14

	.type	.L.str.34,@object       // @.str.34
.L.str.34:
	.asciz	"Unknown busy idle event!"
	.size	.L.str.34, 25

	.type	.L.str.35,@object       // @.str.35
.L.str.35:
	.asciz	"req_count is zero!"
	.size	.L.str.35, 19

	.type	.L.str.36,@object       // @.str.36
.L.str.36:
	.asciz	"\0013[BLK-IO]%s: io_count has been zero\n"
	.size	.L.str.36, 38

	.type	.L__func__.blk_idle_count,@object // @__func__.blk_idle_count
.L__func__.blk_idle_count:
	.asciz	"blk_idle_count"
	.size	.L__func__.blk_idle_count, 15

	.type	.L.str.37,@object       // @.str.37
.L.str.37:
	.asciz	"\0013[BLK-IO]<%s> io_count has been zero\n"
	.size	.L.str.37, 39

	.type	.L__func__.blk_fg_io_idle_count,@object // @__func__.blk_fg_io_idle_count
.L__func__.blk_fg_io_idle_count:
	.asciz	"blk_fg_io_idle_count"
	.size	.L__func__.blk_fg_io_idle_count, 21

	.type	.L.str.38,@object       // @.str.38
.L.str.38:
	.asciz	"blk fg io idle count"
	.size	.L.str.38, 21

	.type	.L.str.39,@object       // @.str.39
.L.str.39:
	.asciz	"bio_count is zero!"
	.size	.L.str.39, 19

	.type	.L.str.40,@object       // @.str.40
.L.str.40:
	.asciz	"Invalid event node!"
	.size	.L.str.40, 20

	.type	.L.str.41,@object       // @.str.41
.L.str.41:
	.asciz	"\0013[BLK-IO]%s: %s has been registered already!\n"
	.size	.L.str.41, 47

	.type	.L__func__.mas_blk_busyidle_event_register,@object // @__func__.mas_blk_busyidle_event_register
.L__func__.mas_blk_busyidle_event_register:
	.asciz	"mas_blk_busyidle_event_register"
	.size	.L__func__.mas_blk_busyidle_event_register, 32

	.type	.L.str.42,@object       // @.str.42
.L.str.42:
	.asciz	"\0013[BLK-IO]%s registered to busy idle\n"
	.size	.L.str.42, 38

	.type	.L.str.43,@object       // @.str.43
.L.str.43:
	.asciz	"\0010[BLK-IO]%s: notifier_callback is NULL\n"
	.size	.L.str.43, 41

	.type	.L__func__.is_valid_event_node,@object // @__func__.is_valid_event_node
.L__func__.is_valid_event_node:
	.asciz	"is_valid_event_node"
	.size	.L__func__.is_valid_event_node, 20

	.type	.L.str.44,@object       // @.str.44
.L.str.44:
	.asciz	"\0010[BLK-IO]%s: Invalid subscriber\n"
	.size	.L.str.44, 34

	.type	.L.str.45,@object       // @.str.45
.L.str.45:
	.asciz	"\0013[BLK-IO]Trying to unregister an unregistered event_node!\n"
	.size	.L.str.45, 60

	.type	.L.str.46,@object       // @.str.46
.L.str.46:
	.asciz	"\0013[BLK-IO]%s unregistered from busy idle module 0x%pK\n"
	.size	.L.str.46, 55

	.type	.L.str.47,@object       // @.str.47
.L.str.47:
	.asciz	"fg_busy_idle_notify"
	.size	.L.str.47, 20

	.type	.L__func__.mas_blk_alloc_fg_io_workqueue,@object // @__func__.mas_blk_alloc_fg_io_workqueue
.L__func__.mas_blk_alloc_fg_io_workqueue:
	.asciz	"mas_blk_alloc_fg_io_workqueue"
	.size	.L__func__.mas_blk_alloc_fg_io_workqueue, 30

	.type	mas_blk_fg_io_busyidle_state_init.__key,@object // @mas_blk_fg_io_busyidle_state_init.__key
	.local	mas_blk_fg_io_busyidle_state_init.__key
	.comm	mas_blk_fg_io_busyidle_state_init.__key,1,1
	.type	.L.str.48,@object       // @.str.48
.L.str.48:
	.asciz	"&blk_idle->fg_io_count_mutex"
	.size	.L.str.48, 29

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
