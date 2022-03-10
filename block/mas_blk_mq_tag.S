	.text
	.file	"mas_blk_mq_tag.c"
	.globl	ufs_tagset_tag_busy_iter // -- Begin function ufs_tagset_tag_busy_iter
	.p2align	2
	.type	ufs_tagset_tag_busy_iter,@function
ufs_tagset_tag_busy_iter:               // @ufs_tagset_tag_busy_iter
.Lufs_tagset_tag_busy_iter$local:
// %bb.0:
	sub	sp, sp, #80             // =80
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #48]     // 16-byte Folded Spill
	add	x29, sp, #48            // =48
	str	x19, [sp, #64]          // 8-byte Folded Spill
	stur	x8, [x29, #-8]
	stp	xzr, xzr, [sp, #24]
	stp	xzr, xzr, [sp, #8]
	ldr	x19, [x0, #360]
	stp	x0, x1, [sp, #8]
	add	x1, sp, #8              // =8
	str	x2, [sp, #24]
	add	x0, x19, #40            // =40
	strh	wzr, [sp, #32]
	bl	ufs_tagset_bt_for_each
	ldr	w8, [x19, #8]
	cbz	w8, .LBB0_2
// %bb.1:
	mov	w8, #1
	add	x0, x19, #168           // =168
	add	x1, sp, #8              // =8
	strb	w8, [sp, #33]
	bl	ufs_tagset_bt_for_each
.LBB0_2:
	ldr	w8, [x19, #4]
	cbz	w8, .LBB0_4
// %bb.3:
	mov	w8, #1
	add	x0, x19, #104           // =104
	add	x1, sp, #8              // =8
	strh	w8, [sp, #32]
	bl	ufs_tagset_bt_for_each
.LBB0_4:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB0_6
// %bb.5:
	ldr	x19, [sp, #64]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #48]     // 16-byte Folded Reload
	add	sp, sp, #80             // =80
	ret
.LBB0_6:
	bl	__stack_chk_fail
.Lfunc_end0:
	.size	ufs_tagset_tag_busy_iter, .Lfunc_end0-ufs_tagset_tag_busy_iter
                                        // -- End function
	.p2align	2               // -- Begin function ufs_tagset_bt_for_each
	.type	ufs_tagset_bt_for_each,@function
ufs_tagset_bt_for_each:                 // @ufs_tagset_bt_for_each
// %bb.0:
	sub	sp, sp, #96             // =96
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #32]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]     // 16-byte Folded Spill
	str	x8, [sp, #8]
	ldr	w8, [x0]
	add	x29, sp, #16            // =16
	cbz	w8, .LBB1_15
// %bb.1:
	mov	x19, x0
	mov	x20, x1
	mov	w23, wzr
	mov	w24, wzr
	mov	w25, #192
	b	.LBB1_3
.LBB1_2:                                //   in Loop: Header=BB1_3 Depth=1
	ldr	w9, [x19, #8]
	ldr	w8, [x19]
	add	w10, w23, #1            // =1
	cmp	w10, w9
	csinc	w23, wzr, w23, hs
	cmp	w8, w24
	b.ls	.LBB1_15
.LBB1_3:                                // =>This Loop Header: Depth=1
                                        //     Child Loop BB1_6 Depth 2
	str	xzr, [sp]
	ldr	x9, [x19, #16]
	sub	w8, w8, w24
	umaddl	x9, w23, w25, x9
	ldr	w10, [x9]
	ldr	x11, [x9, #64]
	ldr	x9, [x9, #128]
	cmp	w8, w10
	csel	w26, w10, w8, hi
	bics	x8, x11, x9
	add	w24, w26, w24
	str	x8, [sp]
	b.eq	.LBB1_2
// %bb.4:                               //   in Loop: Header=BB1_3 Depth=1
	mov	w21, w26
	mov	x0, sp
	mov	x1, x21
	mov	x2, xzr
	b	.LBB1_6
.LBB1_5:                                //   in Loop: Header=BB1_6 Depth=2
	add	w2, w22, #1             // =1
	mov	x0, sp
	mov	x1, x21
.LBB1_6:                                //   Parent Loop BB1_3 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	bl	find_next_bit
	mov	x22, x0
	cmp	w26, w22
	b.ls	.LBB1_2
// %bb.7:                               //   in Loop: Header=BB1_6 Depth=2
	ldr	x0, [x20]
	ldr	w10, [x19, #4]
	ldrb	w8, [x20, #24]
	ldr	x9, [x0, #360]
	lsl	w10, w23, w10
	add	w10, w10, w22
	cbz	w8, .LBB1_9
// %bb.8:                               //   in Loop: Header=BB1_6 Depth=2
	ldr	w11, [x9, #16]
	add	w10, w11, w10
.LBB1_9:                                //   in Loop: Header=BB1_6 Depth=2
	ldrb	w11, [x20, #25]
	cbz	w11, .LBB1_11
// %bb.10:                              //   in Loop: Header=BB1_6 Depth=2
	ldr	w11, [x9, #20]
	add	w10, w11, w10
.LBB1_11:                               //   in Loop: Header=BB1_6 Depth=2
	ldr	x9, [x9, #232]
	ldr	x1, [x9, w10, uxtw #3]
	cbz	x1, .LBB1_5
// %bb.12:                              //   in Loop: Header=BB1_6 Depth=2
	ldr	x9, [x1]
	ldr	x10, [x0, #232]
	cmp	x9, x10
	b.ne	.LBB1_5
// %bb.13:                              //   in Loop: Header=BB1_6 Depth=2
	ldr	x9, [x20, #8]
	cbz	x9, .LBB1_5
// %bb.14:                              //   in Loop: Header=BB1_6 Depth=2
	ldr	x2, [x20, #16]
	cmp	w8, #0                  // =0
	cset	w3, ne
	blr	x9
	b	.LBB1_5
.LBB1_15:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB1_17
// %bb.16:
	ldp	x20, x19, [sp, #80]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #96             // =96
	ret
.LBB1_17:
	bl	__stack_chk_fail
.Lfunc_end1:
	.size	ufs_tagset_bt_for_each, .Lfunc_end1-ufs_tagset_bt_for_each
                                        // -- End function
	.globl	ufs_tagset_bt_get       // -- Begin function ufs_tagset_bt_get
	.p2align	2
	.type	ufs_tagset_bt_get,@function
ufs_tagset_bt_get:                      // @ufs_tagset_bt_get
.Lufs_tagset_bt_get$local:
// %bb.0:
	sub	sp, sp, #160            // =160
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #64]     // 16-byte Folded Spill
	add	x29, sp, #64            // =64
	stp	x28, x27, [sp, #80]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #96]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #112]    // 16-byte Folded Spill
	stp	x22, x21, [sp, #128]    // 16-byte Folded Spill
	stp	x20, x19, [sp, #144]    // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	cbz	x2, .LBB2_4
// %bb.1:
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
	bl	ufs_sbitmap_queue_get
	mov	w23, w0
	cmn	w0, #1                  // =1
	b.ne	.LBB2_5
// %bb.2:
	ldr	x0, [x19]
	bl	blk_queue_query_unistore_enable
	ldr	w24, [x19, #12]
	tbz	w24, #8, .LBB2_7
// %bb.3:
	tbz	w0, #0, .LBB2_7
.LBB2_4:
	mov	w23, #-1
.LBB2_5:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB2_33
// %bb.6:
	mov	w0, w23
	ldp	x20, x19, [sp, #144]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #128]    // 16-byte Folded Reload
	ldp	x24, x23, [sp, #112]    // 16-byte Folded Reload
	ldp	x26, x25, [sp, #96]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #80]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #64]     // 16-byte Folded Reload
	add	sp, sp, #160            // =160
	ret
.LBB2_7:
	adrp	x9, autoremove_wake_function
	//APP
	mrs x8, sp_el0
	//NO_APP
	add	x9, x9, :lo12:autoremove_wake_function
	add	x10, sp, #8             // =8
	stp	x8, x9, [sp, #24]
	add	x8, x10, #32            // =32
	stp	xzr, xzr, [sp, #8]
	stp	x8, x8, [sp, #40]
	ldr	w22, [x19, #36]
	mov	w8, w22
	tbz	w24, #1, .LBB2_9
// %bb.8:
	mov	x0, x19
	bl	ufs_mq_inc_vip_wait_cnt
	ldr	w8, [x19, #36]
.LBB2_9:
	mov	w25, #4096
	movk	w25, #64, lsl #16
	tst	w8, w25
	str	w22, [sp, #4]           // 4-byte Folded Spill
	b.eq	.LBB2_12
// %bb.10:
	ldrb	w8, [x19, #12]
	tbnz	w8, #1, .LBB2_12
// %bb.11:
	bl	ktime_get
	str	x0, [x19, #24]
.LBB2_12:
	ldr	x22, [x20, #40]
	add	x2, sp, #8              // =8
	mov	w3, #2
	mov	x0, x20
	mov	x1, x22
	bl	sbitmap_prepare_to_wait
	mov	x0, x19
	mov	x1, x20
	bl	ufs_sbitmap_queue_get
	cmn	w0, #1                  // =1
	b.eq	.LBB2_16
.LBB2_13:
	mov	w23, w0
	tbz	w24, #1, .LBB2_15
.LBB2_14:
	mov	x0, x19
	bl	ufs_mq_dec_vip_wait_cnt
.LBB2_15:
	add	x2, sp, #8              // =8
	mov	x0, x20
	mov	x1, x22
	bl	sbitmap_finish_wait
	b	.LBB2_5
.LBB2_16:
	ldr	w8, [sp, #4]            // 4-byte Folded Reload
	mov	w9, #2
	adrp	x27, cpu_number
	adrp	x26, __per_cpu_offset
	tst	w8, #0xff
	and	w10, w8, w25
	str	w10, [sp]               // 4-byte Folded Spill
	cset	w10, eq
	tst	w8, #0x8000000
	add	x27, x27, :lo12:cpu_number
	csel	x28, x10, x9, eq
	add	x26, x26, :lo12:__per_cpu_offset
.LBB2_17:                               // =>This Inner Loop Header: Depth=1
	mov	x0, x21
	mov	w1, wzr
	bl	blk_mq_run_hw_queue
	mov	x0, x19
	mov	x1, x20
	bl	ufs_sbitmap_queue_get
	cmn	w0, #1                  // =1
	b.ne	.LBB2_13
// %bb.18:                              //   in Loop: Header=BB2_17 Depth=1
	ldr	w8, [x19, #36]
	tst	w8, w25
	b.eq	.LBB2_22
// %bb.19:                              //   in Loop: Header=BB2_17 Depth=1
	ldrb	w8, [x19, #12]
	tbnz	w8, #1, .LBB2_22
// %bb.20:                              //   in Loop: Header=BB2_17 Depth=1
	mov	w0, #1
	bl	io_schedule_timeout
	cbnz	x0, .LBB2_23
// %bb.21:                              //   in Loop: Header=BB2_17 Depth=1
	mov	x0, x19
	bl	reset_vip_wait_cnt
	b	.LBB2_23
.LBB2_22:                               //   in Loop: Header=BB2_17 Depth=1
	bl	io_schedule
.LBB2_23:                               //   in Loop: Header=BB2_17 Depth=1
	ldr	x8, [x19]
	//APP
	.if 1 == 1
661:
	mrs x9, tpidr_el1
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
	mrs x9, tpidr_el2
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	ldr	w9, [x9, x27]
	ldr	x8, [x8, #56]
	ldr	x9, [x26, x9, lsl #3]
	add	x8, x9, x8
	str	x8, [x19, #40]
	add	x8, x8, x28, lsl #3
	ldr	x8, [x8, #80]
	str	x8, [x19, #48]
	cbz	x8, .LBB2_32
// %bb.24:                              //   in Loop: Header=BB2_17 Depth=1
	cmp	x21, x8
	b.ne	.LBB2_26
.LBB2_25:                               //   in Loop: Header=BB2_17 Depth=1
	add	x2, sp, #8              // =8
	mov	x0, x20
	mov	x1, x22
	bl	sbitmap_finish_wait
	add	x2, sp, #8              // =8
	mov	w3, #2
	mov	x0, x20
	mov	x1, x22
	bl	sbitmap_prepare_to_wait
	mov	x0, x19
	mov	x1, x20
	bl	ufs_sbitmap_queue_get
	cmn	w0, #1                  // =1
	b.eq	.LBB2_17
	b	.LBB2_13
.LBB2_26:                               //   in Loop: Header=BB2_17 Depth=1
	ldr	x9, [x8, #360]
	ldr	w10, [sp, #4]           // 4-byte Folded Reload
	tbz	w10, #11, .LBB2_30
// %bb.27:                              //   in Loop: Header=BB2_17 Depth=1
	ldr	w10, [sp]               // 4-byte Folded Reload
	cbnz	w10, .LBB2_31
// %bb.28:                              //   in Loop: Header=BB2_17 Depth=1
	add	x20, x9, #40            // =40
.LBB2_29:                               //   in Loop: Header=BB2_17 Depth=1
	mov	x21, x8
	b	.LBB2_25
.LBB2_30:                               //   in Loop: Header=BB2_17 Depth=1
	add	x20, x9, #104           // =104
	b	.LBB2_29
.LBB2_31:                               //   in Loop: Header=BB2_17 Depth=1
	add	x20, x9, #168           // =168
	b	.LBB2_29
.LBB2_32:
	mov	w23, #-1
	tbnz	w24, #1, .LBB2_14
	b	.LBB2_15
.LBB2_33:
	bl	__stack_chk_fail
.Lfunc_end2:
	.size	ufs_tagset_bt_get, .Lfunc_end2-ufs_tagset_bt_get
                                        // -- End function
	.p2align	2               // -- Begin function ufs_sbitmap_queue_get
	.type	ufs_sbitmap_queue_get,@function
ufs_sbitmap_queue_get:                  // @ufs_sbitmap_queue_get
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	w8, [x0, #36]
	mov	w9, #4096
	movk	w9, #64, lsl #16
	mov	x20, x0
	tst	w8, w9
	mov	x19, x1
	mov	x29, sp
	b.eq	.LBB3_5
// %bb.1:
	ldrb	w8, [x20, #12]
	tbnz	w8, #1, .LBB3_5
// %bb.2:
	mov	x0, x20
	bl	ufs_mq_vip_tag_wait_cnt
	cbz	w0, .LBB3_5
// %bb.3:
	ldr	x8, [x20, #24]
	cbz	x8, .LBB3_7
// %bb.4:
	bl	ktime_get
	ldr	x8, [x20, #24]
	mov	w9, #25856
	movk	w9, #7629, lsl #16
	add	x8, x8, x9
	cmp	x0, x8
	b.le	.LBB3_7
.LBB3_5:
	ldr	x0, [x20]
	ldr	x1, [x20, #16]
	bl	mas_blk_is_section_ready
	tbz	w0, #0, .LBB3_7
// %bb.6:
	mov	x0, x20
	bl	mas_blk_is_reserved_empty
	tbz	w0, #0, .LBB3_8
.LBB3_7:
	mov	w0, #-1
	b	.LBB3_9
.LBB3_8:
	mov	x0, x19
	bl	__sbitmap_queue_get
.LBB3_9:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end3:
	.size	ufs_sbitmap_queue_get, .Lfunc_end3-ufs_sbitmap_queue_get
                                        // -- End function
	.globl	ufs_tagset_bt_get_unistore // -- Begin function ufs_tagset_bt_get_unistore
	.p2align	2
	.type	ufs_tagset_bt_get_unistore,@function
ufs_tagset_bt_get_unistore:             // @ufs_tagset_bt_get_unistore
.Lufs_tagset_bt_get_unistore$local:
// %bb.0:
	sub	sp, sp, #160            // =160
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #64]     // 16-byte Folded Spill
	add	x29, sp, #64            // =64
	stp	x28, x27, [sp, #80]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #96]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #112]    // 16-byte Folded Spill
	stp	x22, x21, [sp, #128]    // 16-byte Folded Spill
	stp	x20, x19, [sp, #144]    // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	ldr	x23, [x0, #48]
	cbz	x23, .LBB4_16
// %bb.1:
	ldr	x22, [x23, #360]
	mov	x20, x1
	mov	x19, x0
	add	x21, x22, #40           // =40
	mov	x1, x21
	bl	ufs_sbitmap_queue_get
	mov	w24, w0
	cmn	w0, #1                  // =1
	b.ne	.LBB4_17
// %bb.2:
	adrp	x9, autoremove_wake_function
	//APP
	mrs x8, sp_el0
	//NO_APP
	add	x9, x9, :lo12:autoremove_wake_function
	add	x10, sp, #8             // =8
	stp	x8, x9, [sp, #24]
	add	x8, x10, #32            // =32
	stp	xzr, xzr, [sp, #8]
	stp	x8, x8, [sp, #40]
	ldr	x22, [x22, #80]
	adrp	x27, cpu_number
	adrp	x28, __per_cpu_offset
	mov	w25, #2
	mov	w26, #1
	add	x27, x27, :lo12:cpu_number
	add	x28, x28, :lo12:__per_cpu_offset
.LBB4_3:                                // =>This Inner Loop Header: Depth=1
	add	x2, sp, #8              // =8
	mov	w3, #2
	mov	x0, x21
	mov	x1, x22
	bl	sbitmap_prepare_to_wait
	ldrb	w8, [x19, #12]
	tbnz	w8, #7, .LBB4_6
// %bb.4:                               //   in Loop: Header=BB4_3 Depth=1
	ldr	x0, [x19]
	ldr	x1, [x19, #16]
	bl	mas_blk_match_expected_lba
	tbz	w0, #0, .LBB4_6
// %bb.5:                               //   in Loop: Header=BB4_3 Depth=1
	ldr	w8, [x19, #12]
	ldr	x9, [x19, #48]
	orr	w8, w8, #0x80
	str	w8, [x19, #12]
	ldr	x8, [x9, #360]
	strb	w26, [x20]
	add	x21, x8, #168           // =168
.LBB4_6:                                //   in Loop: Header=BB4_3 Depth=1
	mov	x0, x19
	mov	x1, x21
	bl	ufs_sbitmap_queue_get
	cmn	w0, #1                  // =1
	b.ne	.LBB4_19
// %bb.7:                               //   in Loop: Header=BB4_3 Depth=1
	mov	x0, x23
	mov	w1, wzr
	bl	blk_mq_run_hw_queue
	mov	x0, x19
	mov	x1, x21
	bl	ufs_sbitmap_queue_get
	cmn	w0, #1                  // =1
	b.ne	.LBB4_19
// %bb.8:                               //   in Loop: Header=BB4_3 Depth=1
	ldr	w24, [x19, #36]
	mov	w0, #25
	bl	io_schedule_timeout
	ldr	x8, [x19]
	//APP
	.if 1 == 1
661:
	mrs x9, tpidr_el1
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
	mrs x9, tpidr_el2
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	ldr	w9, [x9, x27]
	ldr	w10, [x19, #36]
	ldr	x8, [x8, #56]
	ldr	x9, [x28, x9, lsl #3]
	tst	w10, #0xff
	cset	w11, eq
	tst	w10, #0x8000000
	csel	x10, x11, x25, eq
	add	x8, x9, x8
	str	x8, [x19, #40]
	add	x8, x8, x10, lsl #3
	ldr	x8, [x8, #80]
	str	x8, [x19, #48]
	cbz	x8, .LBB4_20
// %bb.9:                               //   in Loop: Header=BB4_3 Depth=1
	cmp	x23, x8
	b.ne	.LBB4_11
.LBB4_10:                               //   in Loop: Header=BB4_3 Depth=1
	add	x2, sp, #8              // =8
	mov	x0, x21
	mov	x1, x22
	bl	sbitmap_finish_wait
	b	.LBB4_3
.LBB4_11:                               //   in Loop: Header=BB4_3 Depth=1
	mov	w9, #4096
	movk	w9, #64, lsl #16
	tst	w24, w9
	b.ne	.LBB4_14
// %bb.12:                              //   in Loop: Header=BB4_3 Depth=1
	ldrb	w9, [x19, #12]
	tbnz	w9, #7, .LBB4_14
// %bb.13:                              //   in Loop: Header=BB4_3 Depth=1
	ldr	x9, [x8, #360]
	add	x21, x9, #40            // =40
	b	.LBB4_15
.LBB4_14:                               //   in Loop: Header=BB4_3 Depth=1
	ldr	x9, [x8, #360]
	add	x21, x9, #168           // =168
.LBB4_15:                               //   in Loop: Header=BB4_3 Depth=1
	mov	x23, x8
	b	.LBB4_10
.LBB4_16:
	mov	w24, #-1
.LBB4_17:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB4_22
// %bb.18:
	mov	w0, w24
	ldp	x20, x19, [sp, #144]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #128]    // 16-byte Folded Reload
	ldp	x24, x23, [sp, #112]    // 16-byte Folded Reload
	ldp	x26, x25, [sp, #96]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #80]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #64]     // 16-byte Folded Reload
	add	sp, sp, #160            // =160
	ret
.LBB4_19:
	mov	w24, w0
	b	.LBB4_21
.LBB4_20:
	mov	w24, #-1
.LBB4_21:
	add	x2, sp, #8              // =8
	mov	x0, x21
	mov	x1, x22
	bl	sbitmap_finish_wait
	b	.LBB4_17
.LBB4_22:
	bl	__stack_chk_fail
.Lfunc_end4:
	.size	ufs_tagset_bt_get_unistore, .Lfunc_end4-ufs_tagset_bt_get_unistore
                                        // -- End function
	.globl	ufs_tagset_all_tag_busy_iter // -- Begin function ufs_tagset_all_tag_busy_iter
	.p2align	2
	.type	ufs_tagset_all_tag_busy_iter,@function
ufs_tagset_all_tag_busy_iter:           // @ufs_tagset_all_tag_busy_iter
.Lufs_tagset_all_tag_busy_iter$local:
// %bb.0:
	sub	sp, sp, #80             // =80
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #48]     // 16-byte Folded Spill
	add	x29, sp, #48            // =48
	str	x19, [sp, #64]          // 8-byte Folded Spill
	mov	x19, x0
	stur	x8, [x29, #-8]
	stp	x0, x1, [sp, #8]
	add	x0, x0, #40             // =40
	add	x1, sp, #8              // =8
	stp	x2, xzr, [sp, #24]
	bl	ufs_tagset_bt_tags_for_each
	ldr	w8, [x19, #8]
	cbz	w8, .LBB5_2
// %bb.1:
	mov	w8, #1
	add	x0, x19, #168           // =168
	add	x1, sp, #8              // =8
	strb	w8, [sp, #33]
	bl	ufs_tagset_bt_tags_for_each
.LBB5_2:
	ldr	w8, [x19, #4]
	cbz	w8, .LBB5_4
// %bb.3:
	mov	w8, #1
	add	x0, x19, #104           // =104
	add	x1, sp, #8              // =8
	strh	w8, [sp, #32]
	bl	ufs_tagset_bt_tags_for_each
.LBB5_4:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB5_6
// %bb.5:
	ldr	x19, [sp, #64]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #48]     // 16-byte Folded Reload
	add	sp, sp, #80             // =80
	ret
.LBB5_6:
	bl	__stack_chk_fail
.Lfunc_end5:
	.size	ufs_tagset_all_tag_busy_iter, .Lfunc_end5-ufs_tagset_all_tag_busy_iter
                                        // -- End function
	.p2align	2               // -- Begin function ufs_tagset_bt_tags_for_each
	.type	ufs_tagset_bt_tags_for_each,@function
ufs_tagset_bt_tags_for_each:            // @ufs_tagset_bt_tags_for_each
// %bb.0:
	sub	sp, sp, #96             // =96
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #32]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]     // 16-byte Folded Spill
	str	x8, [sp, #8]
	ldr	x8, [x1]
	add	x29, sp, #16            // =16
	ldr	x8, [x8, #232]
	cbz	x8, .LBB6_14
// %bb.1:
	ldr	w8, [x0]
	mov	x20, x0
	cbz	w8, .LBB6_14
// %bb.2:
	mov	x19, x1
	mov	w23, wzr
	mov	w24, wzr
	mov	w25, #192
	b	.LBB6_4
.LBB6_3:                                //   in Loop: Header=BB6_4 Depth=1
	ldr	w9, [x20, #8]
	ldr	w8, [x20]
	add	w10, w23, #1            // =1
	cmp	w10, w9
	csinc	w23, wzr, w23, hs
	cmp	w8, w24
	b.ls	.LBB6_14
.LBB6_4:                                // =>This Loop Header: Depth=1
                                        //     Child Loop BB6_7 Depth 2
	str	xzr, [sp]
	ldr	x9, [x20, #16]
	sub	w8, w8, w24
	umaddl	x9, w23, w25, x9
	ldr	w10, [x9]
	ldr	x11, [x9, #64]
	ldr	x9, [x9, #128]
	cmp	w8, w10
	csel	w26, w10, w8, hi
	bics	x8, x11, x9
	add	w24, w26, w24
	str	x8, [sp]
	b.eq	.LBB6_3
// %bb.5:                               //   in Loop: Header=BB6_4 Depth=1
	mov	w21, w26
	mov	x0, sp
	mov	x1, x21
	mov	x2, xzr
	b	.LBB6_7
.LBB6_6:                                //   in Loop: Header=BB6_7 Depth=2
	add	w2, w22, #1             // =1
	mov	x0, sp
	mov	x1, x21
.LBB6_7:                                //   Parent Loop BB6_4 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	bl	find_next_bit
	mov	x22, x0
	cmp	w26, w22
	b.ls	.LBB6_3
// %bb.8:                               //   in Loop: Header=BB6_7 Depth=2
	ldr	w10, [x20, #4]
	ldrb	w8, [x19, #24]
	ldr	x9, [x19]
	lsl	w10, w23, w10
	add	w10, w10, w22
	cbz	w8, .LBB6_10
// %bb.9:                               //   in Loop: Header=BB6_7 Depth=2
	ldr	w11, [x9, #16]
	add	w10, w11, w10
.LBB6_10:                               //   in Loop: Header=BB6_7 Depth=2
	ldrb	w11, [x19, #25]
	cbz	w11, .LBB6_12
// %bb.11:                              //   in Loop: Header=BB6_7 Depth=2
	ldr	w11, [x9, #20]
	add	w10, w11, w10
.LBB6_12:                               //   in Loop: Header=BB6_7 Depth=2
	ldr	x11, [x19, #8]
	cbz	x11, .LBB6_6
// %bb.13:                              //   in Loop: Header=BB6_7 Depth=2
	ldr	x9, [x9, #232]
	ldr	x1, [x19, #16]
	cmp	w8, #0                  // =0
	cset	w2, ne
	ldr	x0, [x9, w10, uxtw #3]
	blr	x11
	b	.LBB6_6
.LBB6_14:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB6_16
// %bb.15:
	ldp	x20, x19, [sp, #80]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #96             // =96
	ret
.LBB6_16:
	bl	__stack_chk_fail
.Lfunc_end6:
	.size	ufs_tagset_bt_tags_for_each, .Lfunc_end6-ufs_tagset_bt_tags_for_each
                                        // -- End function
	.globl	ufs_tagset_free_tags    // -- Begin function ufs_tagset_free_tags
	.p2align	2
	.type	ufs_tagset_free_tags,@function
ufs_tagset_free_tags:                   // @ufs_tagset_free_tags
.Lufs_tagset_free_tags$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x0
	ldr	x0, [x0, #80]
	mov	x29, sp
	bl	kfree
	ldr	x0, [x19, #64]
	bl	free_percpu
	ldr	x0, [x19, #56]
	bl	kfree
	ldr	x0, [x19, #144]
	str	xzr, [x19, #56]
	bl	kfree
	ldr	x0, [x19, #128]
	bl	free_percpu
	ldr	x0, [x19, #120]
	bl	kfree
	ldr	x0, [x19, #208]
	str	xzr, [x19, #120]
	bl	kfree
	ldr	x0, [x19, #192]
	bl	free_percpu
	ldr	x0, [x19, #184]
	bl	kfree
	mov	x0, x19
	str	xzr, [x19, #184]
	bl	kfree
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end7:
	.size	ufs_tagset_free_tags, .Lfunc_end7-ufs_tagset_free_tags
                                        // -- End function
	.globl	ufs_mq_tag_wakeup_all   // -- Begin function ufs_mq_tag_wakeup_all
	.p2align	2
	.type	ufs_mq_tag_wakeup_all,@function
ufs_mq_tag_wakeup_all:                  // @ufs_mq_tag_wakeup_all
.Lufs_mq_tag_wakeup_all$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x0
	add	x0, x0, #40             // =40
	mov	x29, sp
	bl	sbitmap_queue_wake_all
	add	x0, x19, #104           // =104
	bl	sbitmap_queue_wake_all
	add	x0, x19, #168           // =168
	bl	sbitmap_queue_wake_all
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	mov	w0, wzr
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end8:
	.size	ufs_mq_tag_wakeup_all, .Lfunc_end8-ufs_mq_tag_wakeup_all
                                        // -- End function
	.globl	ufs_tagset_init_tags    // -- Begin function ufs_tagset_init_tags
	.p2align	2
	.type	ufs_tagset_init_tags,@function
ufs_tagset_init_tags:                   // @ufs_tagset_init_tags
.Lufs_tagset_init_tags$local:
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	adrp	x8, kmalloc_caches+72
	ldr	x8, [x8, :lo12:kmalloc_caches+72]
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	mov	w23, w2
	mov	w24, w1
	mov	w22, w0
	mov	w1, #3520
	mov	w2, #264
	mov	x0, x8
	str	x25, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	mov	x29, sp
	mov	w21, w4
	mov	w20, w3
	bl	kmem_cache_alloc_trace
	mov	x19, x0
	cbnz	x0, .LBB9_2
.LBB9_1:
	mov	x0, x19
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldr	x25, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB9_2:
	sub	w8, w22, w24
	stp	w22, w24, [x19]
	sub	w22, w8, w23
	adrp	x0, .L.str.1
	cmp	w21, #1                 // =1
	add	x0, x0, :lo12:.L.str.1
	mov	w1, w22
	mov	w2, w24
	mov	w3, w23
	str	w23, [x19, #8]
	cset	w25, eq
	bl	printk
	add	x0, x19, #40            // =40
	mov	w2, #-1
	mov	w4, #3264
	mov	w1, w22
	mov	w3, w25
	mov	w5, w20
	bl	sbitmap_queue_init_node
	cbnz	w0, .LBB9_8
// %bb.3:
	ldr	w1, [x19, #4]
	cmp	w21, #1                 // =1
	cset	w3, eq
	add	x0, x19, #104           // =104
	mov	w2, #-1
	mov	w4, #3264
	mov	w5, w20
	bl	sbitmap_queue_init_node
	cbnz	w0, .LBB9_7
// %bb.4:
	ldr	w1, [x19, #8]
	cmp	w21, #1                 // =1
	cset	w3, eq
	add	x0, x19, #168           // =168
	mov	w2, #-1
	mov	w4, #3264
	mov	w5, w20
	bl	sbitmap_queue_init_node
	cbnz	w0, .LBB9_6
// %bb.5:
	ldr	w8, [x19, #4]
	stp	wzr, w22, [x19, #12]
	add	w8, w8, w22
	str	w8, [x19, #20]
	b	.LBB9_1
.LBB9_6:
	ldr	x0, [x19, #144]
	bl	kfree
	ldr	x0, [x19, #128]
	bl	free_percpu
	ldr	x0, [x19, #120]
	bl	kfree
	str	xzr, [x19, #120]
.LBB9_7:
	ldr	x0, [x19, #80]
	bl	kfree
	ldr	x0, [x19, #64]
	bl	free_percpu
	ldr	x0, [x19, #56]
	bl	kfree
	str	xzr, [x19, #56]
.LBB9_8:
	adrp	x0, .L.str.2
	adrp	x1, .L__func__.ufs_tagset_init_bitmap_tags
	add	x0, x0, :lo12:.L.str.2
	add	x1, x1, :lo12:.L__func__.ufs_tagset_init_bitmap_tags
	bl	printk
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	printk
	mov	x0, x19
	bl	kfree
	mov	x19, xzr
	b	.LBB9_1
.Lfunc_end9:
	.size	ufs_tagset_init_tags, .Lfunc_end9-ufs_tagset_init_tags
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\0013[BLK-IO]Failed to init tagset bitmaps!\n"
	.size	.L.str, 42

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"\0013[BLK-IO]depth = %d, reserved = %d, high prio = %d \r\n"
	.size	.L.str.1, 55

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"\0013[BLK-IO]%s: error nomem\n"
	.size	.L.str.2, 27

	.type	.L__func__.ufs_tagset_init_bitmap_tags,@object // @__func__.ufs_tagset_init_bitmap_tags
.L__func__.ufs_tagset_init_bitmap_tags:
	.asciz	"ufs_tagset_init_bitmap_tags"
	.size	.L__func__.ufs_tagset_init_bitmap_tags, 28

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
