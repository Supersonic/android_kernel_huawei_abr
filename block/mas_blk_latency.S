	.text
	.file	"mas_blk_latency.c"
	.p2align	2               // -- Begin function mas_blk_latency_bio_submit_func
	.type	mas_blk_latency_bio_submit_func,@function
mas_blk_latency_bio_submit_func:        // @mas_blk_latency_bio_submit_func
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	ldr	x8, [x0, #8]
	str	x25, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x8, .LBB0_17
// %bb.1:
	ldr	x22, [x8, #1272]
	cbz	x22, .LBB0_17
// %bb.2:
	ldr	w8, [x22, #1276]
	cbz	w8, .LBB0_17
// %bb.3:
	ldrb	w8, [x0, #145]
	mov	x19, x0
	cmp	w8, #76                 // =76
	b.eq	.LBB0_16
// %bb.4:
	add	x20, x19, #184          // =184
	adrp	x1, __mas_blk_latency_check_timer_expire
	mov	w8, #1
	add	x1, x1, :lo12:__mas_blk_latency_check_timer_expire
	mov	x0, x20
	mov	w2, wzr
	mov	x3, xzr
	mov	x4, xzr
	str	w8, [x19, #240]
	bl	init_timer_key
	adrp	x8, jiffies
	ldr	x21, [x8, :lo12:jiffies]
	ldr	x8, [x19, #88]
	tst	x8, #0x18
	b.eq	.LBB0_7
// %bb.5:
	tst	x8, #0x8
	mov	w8, #100
	mov	w9, #500
	csel	w0, w9, w8, eq
.LBB0_6:
	bl	__msecs_to_jiffies
	b	.LBB0_9
.LBB0_7:
	ldr	w8, [x19, #16]
	mov	w9, #4096
	movk	w9, #64, lsl #16
	tst	w8, w9
	b.eq	.LBB0_18
// %bb.8:
	mov	w0, #250
.LBB0_9:
	ldrh	w8, [x19, #20]
	add	x9, x0, x21
	str	x9, [x19, #200]
	add	x10, x19, #472          // =472
	orr	w8, w8, #0x100
	strh	w8, [x19, #20]
	//APP
	dmb ish
	//NO_APP
	//APP
	// atomic_add
	prfm	pstl1strm, [x10]
1:	ldxr	w8, [x10]
	add	w8, w8, 1
	stxr	w9, w8, [x10]
	cbnz	w9, 1b

	//NO_APP
	adrp	x23, cpu_number
	//APP
	.if 1 == 1
661:
	mrs x8, tpidr_el1
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 11
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	mrs x8, tpidr_el2
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	add	x23, x23, :lo12:cpu_number
	adrp	x24, __cpu_possible_mask
	ldr	w8, [x8, x23]
	ldrb	w0, [x24, :lo12:__cpu_possible_mask]
	add	w21, w8, #1             // =1
	bl	__sw_hweight64
	udiv	w8, w21, w0
	msub	w21, w8, w0, w21
	add	w8, w21, #63            // =63
	cmp	w21, #0                 // =0
	csel	w8, w8, w21, lt
	adrp	x9, __cpu_online_mask
	asr	w8, w8, #6
	add	x9, x9, :lo12:__cpu_online_mask
	ldr	x8, [x9, w8, sxtw #3]
	lsr	x8, x8, x21
	tbnz	w8, #0, .LBB0_13
// %bb.10:
	adrp	x22, __cpu_online_mask
	mov	w21, #-1
	add	x22, x22, :lo12:__cpu_online_mask
	adrp	x25, nr_cpu_ids
.LBB0_11:                               // =>This Inner Loop Header: Depth=1
	mov	w0, w21
	mov	x1, x22
	bl	cpumask_next
	ldr	w8, [x25, :lo12:nr_cpu_ids]
	mov	w21, w0
	cmp	w0, w8
	b.hs	.LBB0_13
// %bb.12:                              //   in Loop: Header=BB0_11 Depth=1
	//APP
	.if 1 == 1
661:
	mrs x8, tpidr_el1
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 11
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	mrs x8, tpidr_el2
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	ldr	w8, [x8, x23]
	cmp	w21, w8
	b.eq	.LBB0_11
.LBB0_13:
	ldrb	w0, [x24, :lo12:__cpu_possible_mask]
	bl	__sw_hweight64
	cmp	w21, w0
	b.lo	.LBB0_15
// %bb.14:
	//APP
	.if 1 == 1
661:
	mrs x8, tpidr_el1
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 11
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	mrs x8, tpidr_el2
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	ldr	w21, [x8, x23]
.LBB0_15:
	mov	x0, x20
	mov	w1, w21
	bl	add_timer_on
	//APP
	mrs x8, sp_el0
	//NO_APP
	str	x8, [x19, #112]
	ldr	w9, [x8, #1944]
	add	x0, x19, #128           // =128
	add	x1, x8, #2408           // =2408
	mov	w2, #16
	str	w9, [x19, #120]
	ldr	w9, [x8, #1948]
	str	w9, [x19, #124]
	bl	memcpy
.LBB0_16:
	mov	w8, #76
	str	xzr, [x19, #104]
	stp	xzr, xzr, [x19, #152]
	strb	w8, [x19, #145]
	stp	xzr, xzr, [x19, #168]
.LBB0_17:
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldr	x25, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB0_18:
	ldr	w0, [x22, #1280]
	b	.LBB0_6
.Lfunc_end0:
	.size	mas_blk_latency_bio_submit_func, .Lfunc_end0-mas_blk_latency_bio_submit_func
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_latency_bio_endio_func
	.type	mas_blk_latency_bio_endio_func,@function
mas_blk_latency_bio_endio_func:         // @mas_blk_latency_bio_endio_func
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	str	x23, [sp, #16]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	//APP
	mrs x8, sp_el0
	//NO_APP
	ldr	w8, [x8, #16]
	ldr	x20, [x0, #96]
	mov	w9, #1000
	mov	x19, x0
	tst	w8, #0x1fff00
	mov	w8, #34464
	movk	w8, #1, lsl #16
	csel	x22, x8, x9, eq
	mov	x29, sp
	cbz	x20, .LBB1_8
// %bb.1:
	bl	ktime_get
	ldr	x9, [x19, #88]
	ldr	x8, [x19, #152]
	tst	x9, #0x18
	b.eq	.LBB1_3
// %bb.2:
	tst	x9, #0x8
	mov	w9, #100
	mov	w10, #500
	csel	x9, x10, x9, eq
	b	.LBB1_6
.LBB1_3:
	ldr	w9, [x19, #16]
	mov	w10, #4096
	movk	w10, #64, lsl #16
	tst	w9, w10
	b.eq	.LBB1_5
// %bb.4:
	mov	w9, #1000
	b	.LBB1_6
.LBB1_5:
	ldr	w9, [x20, #1280]
.LBB1_6:
	mov	w10, #16960
	movk	w10, #15, lsl #16
	madd	x8, x9, x10, x8
	cmp	x0, x8
	b.le	.LBB1_8
// %bb.7:
	adrp	x0, latency_endbio_ratelmt_state
	adrp	x1, .L.str.60
	add	x0, x0, :lo12:latency_endbio_ratelmt_state
	add	x1, x1, :lo12:.L.str.60
	bl	___ratelimit
	cbnz	w0, .LBB1_18
.LBB1_8:
	add	x0, x19, #184           // =184
	strb	wzr, [x19, #145]
	bl	del_timer
	cbz	w0, .LBB1_11
// %bb.9:
	mov	x0, x19
	bl	bio_put
.LBB1_10:
	str	wzr, [x19, #240]
	str	xzr, [x19, #104]
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB1_11:
	ldr	w8, [x19, #240]
	cbz	w8, .LBB1_10
// %bb.12:
	mov	w8, wzr
	sub	x9, x22, #1             // =1
.LBB1_13:                               // =>This Inner Loop Header: Depth=1
	tbnz	w8, #0, .LBB1_10
// %bb.14:                              //   in Loop: Header=BB1_13 Depth=1
	mov	w0, #42950
	mov	x20, x9
	bl	__const_udelay
	ldr	w10, [x19, #240]
	cmp	x20, #0                 // =0
	cset	w8, eq
	sub	x9, x20, #1             // =1
	cbnz	w10, .LBB1_13
// %bb.15:
	cbnz	x20, .LBB1_10
// %bb.16:
	adrp	x0, latency_endbio_ratelmt_state
	adrp	x1, .L.str.61
	add	x0, x0, :lo12:latency_endbio_ratelmt_state
	add	x1, x1, :lo12:.L.str.61
	bl	___ratelimit
	cbz	w0, .LBB1_10
// %bb.17:
	bl	dump_stack
	b	.LBB1_10
.LBB1_18:
	mov	x0, x20
	bl	mas_blk_get_lld
	ldr	x23, [x20, #1400]
	mov	x21, x0
	mov	x0, x19
	mov	w1, wzr
	bl	mas_blk_dump_bio
	mov	x0, x20
	mov	w1, wzr
	bl	mas_blk_dump_queue_status
	ldr	x8, [x19, #352]
	cbz	x8, .LBB1_20
// %bb.19:
	blr	x8
.LBB1_20:
	ldr	x8, [x21, #40]
	cbz	x8, .LBB1_22
// %bb.21:
	mov	x0, x20
	mov	w1, wzr
	blr	x8
.LBB1_22:
	cbz	x23, .LBB1_8
// %bb.23:
	ldr	x8, [x23, #160]
	cbz	x8, .LBB1_8
// %bb.24:
	mov	x0, x20
	mov	w1, wzr
	blr	x8
	b	.LBB1_8
.Lfunc_end1:
	.size	mas_blk_latency_bio_endio_func, .Lfunc_end1-mas_blk_latency_bio_endio_func
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_latency_req_init_func
	.type	mas_blk_latency_req_init_func,@function
mas_blk_latency_req_init_func:          // @mas_blk_latency_req_init_func
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x8, [x0]
	mov	x29, sp
	cbz	x8, .LBB2_5
// %bb.1:
	ldr	w8, [x8, #1276]
	cbz	w8, .LBB2_5
// %bb.2:
	mov	w9, #76
	mov	x8, xzr
	strb	w9, [x0, #248]
	add	x9, x0, #288            // =288
.LBB2_3:                                // =>This Inner Loop Header: Depth=1
	str	xzr, [x9, x8]
	add	x8, x8, #8              // =8
	cmp	x8, #416                // =416
	b.ne	.LBB2_3
// %bb.4:
	//APP
	mrs x9, sp_el0
	//NO_APP
	str	x9, [x0, #208]
	ldr	w8, [x9, #1944]
	add	x1, x9, #2408           // =2408
	mov	w2, #16
	str	w8, [x0, #216]
	ldr	w10, [x9, #1948]
	add	x8, x0, #224            // =224
	str	w10, [x0, #220]
	mov	x0, x8
	bl	memcpy
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB2_5:
	strb	wzr, [x0, #248]
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end2:
	.size	mas_blk_latency_req_init_func, .Lfunc_end2-mas_blk_latency_req_init_func
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_latency_req_free_func
	.type	mas_blk_latency_req_free_func,@function
mas_blk_latency_req_free_func:          // @mas_blk_latency_req_free_func
// %bb.0:
	ldr	x8, [x0, #64]
	strb	wzr, [x0, #248]
	str	xzr, [x0, #208]
	cbz	x8, .LBB3_2
.LBB3_1:                                // =>This Inner Loop Header: Depth=1
	str	xzr, [x8, #104]
	ldr	x8, [x8]
	str	x8, [x0, #64]
	cbnz	x8, .LBB3_1
.LBB3_2:
	ret
.Lfunc_end3:
	.size	mas_blk_latency_req_free_func, .Lfunc_end3-mas_blk_latency_req_free_func
                                        // -- End function
	.globl	mas_blk_latency_check_timer_expire // -- Begin function mas_blk_latency_check_timer_expire
	.p2align	2
	.type	mas_blk_latency_check_timer_expire,@function
mas_blk_latency_check_timer_expire:     // @mas_blk_latency_check_timer_expire
.Lmas_blk_latency_check_timer_expire$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x19, x0
	adrp	x0, latency_log_protect_lock
	add	x0, x0, :lo12:latency_log_protect_lock
	str	x23, [sp, #16]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	bl	_raw_spin_lock_irqsave
	ldrb	w3, [x19, #145]
	mov	x20, x0
	cmp	w3, #76                 // =76
	b.ne	.LBB4_14
// %bb.1:
	ldr	x8, [x19, #8]
	cbz	x8, .LBB4_13
// %bb.2:
	ldr	x21, [x8, #1272]
	cbz	x21, .LBB4_13
// %bb.3:
	mov	x0, x21
	bl	mas_blk_get_lld
	adrp	x0, latency_log_ratelmt_state
	adrp	x1, .L.str.58
	add	x0, x0, :lo12:latency_log_ratelmt_state
	add	x1, x1, :lo12:.L.str.58
	bl	___ratelimit
	cbz	w0, .LBB4_12
// %bb.4:
	mov	x0, x21
	bl	mas_blk_get_lld
	ldr	x23, [x21, #1400]
	mov	x22, x0
	mov	x0, x19
	mov	w1, wzr
	bl	mas_blk_dump_bio
	mov	x0, x21
	mov	w1, wzr
	bl	mas_blk_dump_queue_status
	ldr	x8, [x19, #352]
	cbz	x8, .LBB4_6
// %bb.5:
	blr	x8
.LBB4_6:
	ldr	x8, [x22, #40]
	cbz	x8, .LBB4_8
// %bb.7:
	mov	x0, x21
	mov	w1, wzr
	blr	x8
.LBB4_8:
	cbz	x23, .LBB4_11
// %bb.9:
	ldr	x8, [x23, #160]
	cbz	x8, .LBB4_11
// %bb.10:
	mov	x0, x21
	mov	w1, wzr
	blr	x8
.LBB4_11:
	ldp	w0, w1, [x19, #120]
	adrp	x2, .L.str.58
	add	x2, x2, :lo12:.L.str.58
	bl	iowp_report
.LBB4_12:
	adrp	x0, latency_log_protect_lock
	add	x0, x0, :lo12:latency_log_protect_lock
	mov	x1, x20
	bl	_raw_spin_unlock_irqrestore
	mov	x0, x19
	str	wzr, [x19, #240]
	bl	bio_put
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB4_13:
	adrp	x0, .L.str.57
	adrp	x1, .L__func__.mas_blk_latency_check_timer_expire
	add	x0, x0, :lo12:.L.str.57
	add	x1, x1, :lo12:.L__func__.mas_blk_latency_check_timer_expire
	mov	w2, #347
	bl	printk
	b	.LBB4_12
.LBB4_14:
	adrp	x0, .L.str.56
	adrp	x1, .L__func__.mas_blk_latency_check_timer_expire
	add	x0, x0, :lo12:.L.str.56
	add	x1, x1, :lo12:.L__func__.mas_blk_latency_check_timer_expire
	mov	w2, #342
	bl	printk
	b	.LBB4_12
.Lfunc_end4:
	.size	mas_blk_latency_check_timer_expire, .Lfunc_end4-mas_blk_latency_check_timer_expire
                                        // -- End function
	.globl	mas_blk_latency_bio_check // -- Begin function mas_blk_latency_bio_check
	.p2align	2
	.type	mas_blk_latency_bio_check,@function
mas_blk_latency_bio_check:              // @mas_blk_latency_bio_check
.Lmas_blk_latency_bio_check$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x19, x0
	mov	x29, sp
	cbz	w1, .LBB5_3
// %bb.1:
	ldrb	w8, [x19, #145]
	cmp	w8, #76                 // =76
	b.ne	.LBB5_6
// %bb.2:
	cmp	w1, #1                  // =1
	mov	w20, w1
	b.ls	.LBB5_4
	b	.LBB5_5
.LBB5_3:
	mov	x20, xzr
.LBB5_4:
	adrp	x8, bio_stage_cfg
	add	x8, x8, :lo12:bio_stage_cfg
	add	x8, x8, x20, lsl #4
	ldr	x8, [x8, #8]
	mov	x0, x19
	blr	x8
.LBB5_5:
	bl	ktime_get
	add	x8, x19, x20, lsl #3
	str	x0, [x8, #152]
.LBB5_6:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end5:
	.size	mas_blk_latency_bio_check, .Lfunc_end5-mas_blk_latency_bio_check
                                        // -- End function
	.globl	mas_blk_latency_req_check // -- Begin function mas_blk_latency_req_check
	.p2align	2
	.type	mas_blk_latency_req_check,@function
mas_blk_latency_req_check:              // @mas_blk_latency_req_check
.Lmas_blk_latency_req_check$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x19, x0
	mov	x29, sp
	cbz	w1, .LBB6_4
// %bb.1:
	ldrb	w8, [x19, #248]
	cmp	w8, #76                 // =76
	b.ne	.LBB6_8
// %bb.2:
	cmp	w1, #51                 // =51
	b.ne	.LBB6_6
// %bb.3:
	mov	w20, #51
	b	.LBB6_5
.LBB6_4:
	mov	x20, xzr
.LBB6_5:
	adrp	x8, req_stage_cfg
	add	x8, x8, :lo12:req_stage_cfg
	add	x8, x8, x20, lsl #4
	ldr	x8, [x8, #8]
	mov	x0, x19
	blr	x8
	b	.LBB6_7
.LBB6_6:
	mov	w20, w1
.LBB6_7:
	bl	ktime_get
	add	x8, x19, x20, lsl #3
	str	x0, [x8, #288]
.LBB6_8:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end6:
	.size	mas_blk_latency_req_check, .Lfunc_end6-mas_blk_latency_req_check
                                        // -- End function
	.globl	mas_blk_latency_for_merge // -- Begin function mas_blk_latency_for_merge
	.p2align	2
	.type	mas_blk_latency_for_merge,@function
mas_blk_latency_for_merge:              // @mas_blk_latency_for_merge
.Lmas_blk_latency_for_merge$local:
// %bb.0:
	ldr	x8, [x1, #64]
	cbz	x8, .LBB7_2
.LBB7_1:                                // =>This Inner Loop Header: Depth=1
	str	x0, [x8, #104]
	ldr	x8, [x8]
	cbnz	x8, .LBB7_1
.LBB7_2:
	ret
.Lfunc_end7:
	.size	mas_blk_latency_for_merge, .Lfunc_end7-mas_blk_latency_for_merge
                                        // -- End function
	.globl	mas_blk_queue_latency_init // -- Begin function mas_blk_queue_latency_init
	.p2align	2
	.type	mas_blk_queue_latency_init,@function
mas_blk_queue_latency_init:             // @mas_blk_queue_latency_init
.Lmas_blk_queue_latency_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_get_lld
	ldr	w8, [x0, #48]
	cbnz	w8, .LBB8_2
// %bb.1:
	mov	w8, #2000
	str	w8, [x0, #48]
.LBB8_2:
	str	w8, [x19, #1280]
	ldr	w8, [x0, #24]
	ubfx	w8, w8, #1, #1
	str	w8, [x19, #1276]
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end8:
	.size	mas_blk_queue_latency_init, .Lfunc_end8-mas_blk_queue_latency_init
                                        // -- End function
	.section	.init.text,"ax",@progbits
	.globl	mas_blk_latency_init    // -- Begin function mas_blk_latency_init
	.p2align	2
	.type	mas_blk_latency_init,@function
mas_blk_latency_init:                   // @mas_blk_latency_init
.Lmas_blk_latency_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	adrp	x8, latency_log_protect_lock
	mov	x29, sp
	str	wzr, [x8, :lo12:latency_log_protect_lock]
	bl	iowp_workqueue_init
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end9:
	.size	mas_blk_latency_init, .Lfunc_end9-mas_blk_latency_init
                                        // -- End function
	.text
	.globl	blk_mq_tagset_latency_warning_set // -- Begin function blk_mq_tagset_latency_warning_set
	.p2align	2
	.type	blk_mq_tagset_latency_warning_set,@function
blk_mq_tagset_latency_warning_set:      // @blk_mq_tagset_latency_warning_set
.Lblk_mq_tagset_latency_warning_set$local:
// %bb.0:
	ldr	x8, [x0, #88]
	cmp	w1, #0                  // =0
	mov	w9, #2000
	csel	w9, w9, w1, eq
	orr	x8, x8, #0x2
	str	x8, [x0, #88]
	str	w9, [x0, #112]
	ret
.Lfunc_end10:
	.size	blk_mq_tagset_latency_warning_set, .Lfunc_end10-blk_mq_tagset_latency_warning_set
                                        // -- End function
	.globl	blk_queue_latency_warning_set // -- Begin function blk_queue_latency_warning_set
	.p2align	2
	.type	blk_queue_latency_warning_set,@function
blk_queue_latency_warning_set:          // @blk_queue_latency_warning_set
.Lblk_queue_latency_warning_set$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	mov	w19, w1
	mov	x20, x0
	bl	mas_blk_get_lld
	ldr	x8, [x0, #24]
	cmp	w19, #0                 // =0
	mov	w9, #2000
	mov	w10, #1
	csel	w9, w9, w19, eq
	orr	x8, x8, #0x2
	str	w9, [x0, #48]
	str	x8, [x0, #24]
	str	w9, [x20, #1280]
	str	w10, [x20, #1276]
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end11:
	.size	blk_queue_latency_warning_set, .Lfunc_end11-blk_queue_latency_warning_set
                                        // -- End function
	.globl	mas_queue_io_latency_warning_threshold_store // -- Begin function mas_queue_io_latency_warning_threshold_store
	.p2align	2
	.type	mas_queue_io_latency_warning_threshold_store,@function
mas_queue_io_latency_warning_threshold_store: // @mas_queue_io_latency_warning_threshold_store
.Lmas_queue_io_latency_warning_threshold_store$local:
// %bb.0:
	sub	sp, sp, #64             // =64
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x20, x0
	mov	x0, sp
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	mov	x19, x2
	str	x8, [sp, #8]
	str	xzr, [sp]
	bl	queue_var_store
	tbz	x0, #63, .LBB12_3
.LBB12_1:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB12_6
// %bb.2:
	mov	x0, x19
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.LBB12_3:
	ldr	x2, [sp]
	adrp	x0, .L.str.59
	adrp	x1, .L__func__.mas_queue_io_latency_warning_threshold_store
	add	x0, x0, :lo12:.L.str.59
	add	x1, x1, :lo12:.L__func__.mas_queue_io_latency_warning_threshold_store
	bl	printk
	ldr	x21, [sp]
	mov	x0, x20
	cmp	x21, #0                 // =0
	cset	w22, ne
	bl	mas_blk_get_lld
	ldr	w8, [x20, #1276]
	cmp	w8, w22
	b.eq	.LBB12_5
// %bb.4:
	str	w22, [x20, #1276]
	ldr	x8, [x0, #24]
	cmp	x21, #0                 // =0
	and	x9, x8, #0xfffffffffffffffd
	orr	x8, x8, #0x2
	csel	x8, x8, x9, ne
	str	x8, [x0, #24]
.LBB12_5:
	ldr	w21, [sp]
	mov	x0, x20
	bl	mas_blk_get_lld
	cmp	w21, #0                 // =0
	mov	w8, #2000
	csel	w8, w8, w21, eq
	str	w8, [x0, #48]
	str	w8, [x20, #1280]
	b	.LBB12_1
.LBB12_6:
	bl	__stack_chk_fail
.Lfunc_end12:
	.size	mas_queue_io_latency_warning_threshold_store, .Lfunc_end12-mas_queue_io_latency_warning_threshold_store
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"BIO_SUBMIT"
	.size	.L.str, 11

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"BIO_END"
	.size	.L.str.1, 8

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"BIO_MAKE_REQ"
	.size	.L.str.2, 13

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"BIO_WBT_DONE"
	.size	.L.str.3, 13

	.type	bio_stage_cfg,@object   // @bio_stage_cfg
	.section	.rodata,"a",@progbits
	.globl	bio_stage_cfg
	.p2align	3
bio_stage_cfg:
.Lbio_stage_cfg$local:
	.xword	.L.str
	.xword	mas_blk_latency_bio_submit_func
	.xword	.L.str.1
	.xword	mas_blk_latency_bio_endio_func
	.xword	.L.str.2
	.xword	0
	.xword	.L.str.3
	.xword	0
	.size	bio_stage_cfg, 64

	.type	.L.str.4,@object        // @.str.4
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.4:
	.asciz	"REQ_INIT"
	.size	.L.str.4, 9

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"FSEQ_PREFLUSH"
	.size	.L.str.5, 14

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"FSEQ_DATA"
	.size	.L.str.6, 10

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"FSEQ_POSTFLUSH"
	.size	.L.str.7, 15

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"FSEQ_DONE"
	.size	.L.str.8, 10

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"SCSI_EXECUTE_RQ_BEFORE"
	.size	.L.str.9, 23

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"SCSI_EXECUTE_RQ_WAIT"
	.size	.L.str.10, 21

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"SCSI_EXECUTE_RQ_COMPLETE"
	.size	.L.str.11, 25

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"SCSI_EXECUTE_RQ_AFTER"
	.size	.L.str.12, 22

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"REQ_RQ_START"
	.size	.L.str.13, 13

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"REQ_MQ_ADDTO_PLUGLIST"
	.size	.L.str.14, 22

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"REQ_MQ_FLUSH_PLUGLIST"
	.size	.L.str.15, 22

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"REQ_MQ_ADDTO_SYNC_LIST"
	.size	.L.str.16, 23

	.type	.L.str.17,@object       // @.str.17
.L.str.17:
	.asciz	"REQ_MQ_ADDTO_ASYNC_LIST"
	.size	.L.str.17, 24

	.type	.L.str.18,@object       // @.str.18
.L.str.18:
	.asciz	"REQ_MQ_QUEUE_RQ"
	.size	.L.str.18, 16

	.type	.L.str.19,@object       // @.str.19
.L.str.19:
	.asciz	"REQ_PROC_STAGE_MQ_BUDGET_FAIL"
	.size	.L.str.19, 30

	.type	.L.str.20,@object       // @.str.20
.L.str.20:
	.asciz	"REQ_MQ_QUEUE_SCSI_PREP"
	.size	.L.str.20, 23

	.type	.L.str.21,@object       // @.str.21
.L.str.21:
	.asciz	"REQ_MQ_QUEUE_SCSI_TARGET"
	.size	.L.str.21, 25

	.type	.L.str.22,@object       // @.str.22
.L.str.22:
	.asciz	"REQ_MQ_QUEUE_SCSI_HOST"
	.size	.L.str.22, 23

	.type	.L.str.23,@object       // @.str.23
.L.str.23:
	.asciz	"REQ_MQ_QUEUE_SCSI_RECOVERY"
	.size	.L.str.23, 27

	.type	.L.str.24,@object       // @.str.24
.L.str.24:
	.asciz	"REQ_MQ_QUEUE_SCSI_HBB"
	.size	.L.str.24, 22

	.type	.L.str.25,@object       // @.str.25
.L.str.25:
	.asciz	"REQ_MQ_QUEUE_SCSI_CAN"
	.size	.L.str.25, 22

	.type	.L.str.26,@object       // @.str.26
.L.str.26:
	.asciz	"REQ_MQ_QUEUE_SCSI_SELF_BLOCKED"
	.size	.L.str.26, 31

	.type	.L.str.27,@object       // @.str.27
.L.str.27:
	.asciz	"REQ_MQ_HOLD_START"
	.size	.L.str.27, 18

	.type	.L.str.28,@object       // @.str.28
.L.str.28:
	.asciz	"REQ_MQ_HOLD_FAIL"
	.size	.L.str.28, 17

	.type	.L.str.29,@object       // @.str.29
.L.str.29:
	.asciz	"REQ_MQ_HOLD_SUCCESS"
	.size	.L.str.29, 20

	.type	.L.str.30,@object       // @.str.30
.L.str.30:
	.asciz	"REQ_SQ_REQUEUE"
	.size	.L.str.30, 15

	.type	.L.str.31,@object       // @.str.31
.L.str.31:
	.asciz	"REQ_RQ_INFLT_UPDATE"
	.size	.L.str.31, 20

	.type	.L.str.32,@object       // @.str.32
.L.str.32:
	.asciz	"REQ_RQ_COMPLETE_REMOTE"
	.size	.L.str.32, 23

	.type	.L.str.33,@object       // @.str.33
.L.str.33:
	.asciz	"REQ_RQ_COMPLETE_CURRENT"
	.size	.L.str.33, 24

	.type	.L.str.34,@object       // @.str.34
.L.str.34:
	.asciz	"REQ_SCSI_SFT_IRQ_DONE"
	.size	.L.str.34, 22

	.type	.L.str.35,@object       // @.str.35
.L.str.35:
	.asciz	"REQ_SCSI_SFT_IRQ_DONE_IN"
	.size	.L.str.35, 25

	.type	.L.str.36,@object       // @.str.36
.L.str.36:
	.asciz	"REQ_SCSI_SFT_IRQ_DONE_DECIDE"
	.size	.L.str.36, 29

	.type	.L.str.37,@object       // @.str.37
.L.str.37:
	.asciz	"REQ_SCSI_SFT_IRQ_DONE_UNISTORE"
	.size	.L.str.37, 31

	.type	.L.str.38,@object       // @.str.38
.L.str.38:
	.asciz	"REQ_SCSI_SFT_IRQ_FINISH"
	.size	.L.str.38, 24

	.type	.L.str.39,@object       // @.str.39
.L.str.39:
	.asciz	"REQ_SCSI_SFT_IRQ_INSERT"
	.size	.L.str.39, 24

	.type	.L.str.40,@object       // @.str.40
.L.str.40:
	.asciz	"REQ_SCSI_SFT_IRQ_FINISH_REQUEUE"
	.size	.L.str.40, 32

	.type	.L.str.41,@object       // @.str.41
.L.str.41:
	.asciz	"REQ_SCSI_SFT_IRQ_EH_ADD"
	.size	.L.str.41, 24

	.type	.L.str.42,@object       // @.str.42
.L.str.42:
	.asciz	"REQ_SCSI_EH_INC"
	.size	.L.str.42, 16

	.type	.L.str.43,@object       // @.str.43
.L.str.43:
	.asciz	"REQ_SCSI_EH_STU"
	.size	.L.str.43, 16

	.type	.L.str.44,@object       // @.str.44
.L.str.44:
	.asciz	"REQ_SCSI_EH_DEVICE"
	.size	.L.str.44, 19

	.type	.L.str.45,@object       // @.str.45
.L.str.45:
	.asciz	"REQ_SCSI_EH_TARGET"
	.size	.L.str.45, 19

	.type	.L.str.46,@object       // @.str.46
.L.str.46:
	.asciz	"REQ_SCSI_EH_BUS"
	.size	.L.str.46, 16

	.type	.L.str.47,@object       // @.str.47
.L.str.47:
	.asciz	"REQ_SCSI_EH_HOST"
	.size	.L.str.47, 17

	.type	.L.str.48,@object       // @.str.48
.L.str.48:
	.asciz	"REQ_SCSI_EH_OFFLINE"
	.size	.L.str.48, 20

	.type	.L.str.49,@object       // @.str.49
.L.str.49:
	.asciz	"REQ_SCSI_EH_FLUSH"
	.size	.L.str.49, 18

	.type	.L.str.50,@object       // @.str.50
.L.str.50:
	.asciz	"REQ_UFS_EH_START"
	.size	.L.str.50, 17

	.type	.L.str.51,@object       // @.str.51
.L.str.51:
	.asciz	"REQ_UFS_EH_END"
	.size	.L.str.51, 15

	.type	.L.str.52,@object       // @.str.52
.L.str.52:
	.asciz	"REQ_RQ_UPDATE"
	.size	.L.str.52, 14

	.type	.L.str.53,@object       // @.str.53
.L.str.53:
	.asciz	"REQ_RQ_END_IO_START"
	.size	.L.str.53, 20

	.type	.L.str.54,@object       // @.str.54
.L.str.54:
	.asciz	"REQ_RQ_END_IO"
	.size	.L.str.54, 14

	.type	.L.str.55,@object       // @.str.55
.L.str.55:
	.asciz	"REQ_RQ_FREE"
	.size	.L.str.55, 12

	.type	req_stage_cfg,@object   // @req_stage_cfg
	.section	.rodata,"a",@progbits
	.globl	req_stage_cfg
	.p2align	3
req_stage_cfg:
.Lreq_stage_cfg$local:
	.xword	.L.str.4
	.xword	mas_blk_latency_req_init_func
	.xword	.L.str.5
	.xword	0
	.xword	.L.str.6
	.xword	0
	.xword	.L.str.7
	.xword	0
	.xword	.L.str.8
	.xword	0
	.xword	.L.str.9
	.xword	0
	.xword	.L.str.10
	.xword	0
	.xword	.L.str.11
	.xword	0
	.xword	.L.str.12
	.xword	0
	.xword	.L.str.13
	.xword	0
	.xword	.L.str.14
	.xword	0
	.xword	.L.str.15
	.xword	0
	.xword	.L.str.16
	.xword	0
	.xword	.L.str.17
	.xword	0
	.xword	.L.str.18
	.xword	0
	.xword	.L.str.19
	.xword	0
	.xword	.L.str.20
	.xword	0
	.xword	.L.str.21
	.xword	0
	.xword	.L.str.22
	.xword	0
	.xword	.L.str.23
	.xword	0
	.xword	.L.str.24
	.xword	0
	.xword	.L.str.25
	.xword	0
	.xword	.L.str.26
	.xword	0
	.xword	.L.str.27
	.xword	0
	.xword	.L.str.28
	.xword	0
	.xword	.L.str.29
	.xword	0
	.xword	.L.str.30
	.xword	0
	.xword	.L.str.31
	.xword	0
	.xword	.L.str.32
	.xword	0
	.xword	.L.str.33
	.xword	0
	.xword	.L.str.34
	.xword	0
	.xword	.L.str.35
	.xword	0
	.xword	.L.str.36
	.xword	0
	.xword	.L.str.37
	.xword	0
	.xword	.L.str.38
	.xword	0
	.xword	.L.str.39
	.xword	0
	.xword	.L.str.40
	.xword	0
	.xword	.L.str.41
	.xword	0
	.xword	.L.str.42
	.xword	0
	.xword	.L.str.43
	.xword	0
	.xword	.L.str.44
	.xword	0
	.xword	.L.str.45
	.xword	0
	.xword	.L.str.46
	.xword	0
	.xword	.L.str.47
	.xword	0
	.xword	.L.str.48
	.xword	0
	.xword	.L.str.49
	.xword	0
	.xword	.L.str.50
	.xword	0
	.xword	.L.str.51
	.xword	0
	.xword	.L.str.52
	.xword	0
	.xword	.L.str.53
	.xword	0
	.xword	.L.str.54
	.xword	0
	.xword	.L.str.55
	.xword	mas_blk_latency_req_free_func
	.size	req_stage_cfg, 832

	.type	latency_log_protect_lock,@object // @latency_log_protect_lock
	.local	latency_log_protect_lock
	.comm	latency_log_protect_lock,4,4
	.type	.L.str.56,@object       // @.str.56
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.56:
	.asciz	"\0013[BLK-IO]%s %d Invalid fs_io_flag: 0x%x\n"
	.size	.L.str.56, 42

	.type	.L__func__.mas_blk_latency_check_timer_expire,@object // @__func__.mas_blk_latency_check_timer_expire
.L__func__.mas_blk_latency_check_timer_expire:
	.asciz	"mas_blk_latency_check_timer_expire"
	.size	.L__func__.mas_blk_latency_check_timer_expire, 35

	.type	.L.str.57,@object       // @.str.57
.L.str.57:
	.asciz	"\0013[BLK-IO]%s %d Invalid request queue !\n"
	.size	.L.str.57, 41

	.type	latency_log_ratelmt_state,@object // @latency_log_ratelmt_state
	.data
	.p2align	3
latency_log_ratelmt_state:
	.zero	4
	.word	750                     // 0x2ee
	.word	5                       // 0x5
	.word	0                       // 0x0
	.word	0                       // 0x0
	.zero	4
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.size	latency_log_ratelmt_state, 40

	.type	.L.str.58,@object       // @.str.58
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.58:
	.asciz	"io latency"
	.size	.L.str.58, 11

	.type	.L.str.59,@object       // @.str.59
.L.str.59:
	.asciz	"\0013[BLK-IO]%s: io latency warning threshold is set to %lu\n"
	.size	.L.str.59, 58

	.type	.L__func__.mas_queue_io_latency_warning_threshold_store,@object // @__func__.mas_queue_io_latency_warning_threshold_store
.L__func__.mas_queue_io_latency_warning_threshold_store:
	.asciz	"mas_queue_io_latency_warning_threshold_store"
	.size	.L__func__.mas_queue_io_latency_warning_threshold_store, 45

	.type	latency_endbio_ratelmt_state,@object // @latency_endbio_ratelmt_state
	.data
	.p2align	3
latency_endbio_ratelmt_state:
	.zero	4
	.word	1250                    // 0x4e2
	.word	10                      // 0xa
	.word	0                       // 0x0
	.word	0                       // 0x0
	.zero	4
	.xword	0                       // 0x0
	.xword	0                       // 0x0
	.size	latency_endbio_ratelmt_state, 40

	.type	.L.str.60,@object       // @.str.60
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.60:
	.asciz	"bio timeout"
	.size	.L.str.60, 12

	.type	.L.str.61,@object       // @.str.61
.L.str.61:
	.asciz	"Failed to wait io latency timer"
	.size	.L.str.61, 32

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
