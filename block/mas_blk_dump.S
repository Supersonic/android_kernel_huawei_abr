	.text
	.file	"mas_blk_dump.c"
	.globl	mas_blk_dump_request    // -- Begin function mas_blk_dump_request
	.p2align	2
	.type	mas_blk_dump_request,@function
mas_blk_dump_request:                   // @mas_blk_dump_request
.Lmas_blk_dump_request$local:
// %bb.0:
	stp	x29, x30, [sp, #-96]!   // 16-byte Folded Spill
	str	x28, [sp, #16]          // 8-byte Folded Spill
	stp	x26, x25, [sp, #32]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #1072           // =1072
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	w20, w1
	mov	x19, x0
	add	x0, sp, #40             // =40
	mov	w2, #1024
	mov	w1, wzr
	stur	x8, [x29, #-8]
	mov	w23, #1024
	add	x24, sp, #40            // =40
	bl	memset
	strh	wzr, [sp, #32]
	str	xzr, [sp, #24]
	ldr	w1, [x19, #24]
	ldr	x2, [x19, #200]
	add	x3, sp, #24             // =24
	and	x0, x1, #0xff
                                        // kill: def $w1 killed $w1 killed $x1
	bl	mas_blk_io_type
	ldr	w8, [x19, #24]
	adrp	x9, .L.str.24
	adrp	x10, .L.str.23
	add	x9, x9, :lo12:.L.str.24
	and	w8, w8, #0xfe
	orr	w8, w8, #0x1
	add	x10, x10, :lo12:.L.str.23
	cmp	w8, #35                 // =35
	ldr	w5, [x19, #48]
	ldr	w7, [x19, #252]
	csel	x9, x10, x9, eq
	ldr	w10, [x19, #32]
	cmp	w8, #33                 // =33
	adrp	x8, .L.str.22
	add	x8, x8, :lo12:.L.str.22
	adrp	x2, .L.str
	mov	x4, x0
	csel	x6, x8, x9, eq
	add	x2, x2, :lo12:.L.str
	add	x0, sp, #40             // =40
	mov	w1, #1024
	mov	x3, x19
	add	x25, x19, #208          // =208
	str	w10, [sp]
	bl	snprintf
	ldr	w4, [x19, #256]
	ldr	w5, [x19, #32]
	adrp	x22, .L.str.1
	mov	w8, w0
	add	x22, x22, :lo12:.L.str.1
	mov	w21, w0
	add	x0, x24, x8
	sub	x1, x23, x8
	mov	x2, x22
	mov	w3, wzr
	bl	snprintf
	ldr	w4, [x25, #52]
	ldr	w5, [x19, #32]
	add	w21, w0, w21
	add	x0, x24, x21
	sub	x1, x23, x21
	mov	w3, #1
	mov	x2, x22
	bl	snprintf
	ldr	w4, [x19, #264]
	ldr	w5, [x19, #32]
	add	w21, w0, w21
	add	x0, x24, x21
	sub	x1, x23, x21
	mov	w3, #2
	mov	x2, x22
	bl	snprintf
	ldr	w4, [x25, #60]
	ldr	w5, [x19, #32]
	add	w21, w0, w21
	add	x0, x24, x21
	sub	x1, x23, x21
	mov	w3, #3
	mov	x2, x22
	bl	snprintf
	ldr	x3, [x19, #800]
	ldr	x4, [x19, #808]
	add	w21, w0, w21
	adrp	x8, .L.str.21
	adrp	x9, .L.str.20
	adrp	x2, .L.str.2
	add	x8, x8, :lo12:.L.str.21
	add	x0, x24, x21
	sub	x1, x23, x21
	add	x9, x9, :lo12:.L.str.20
	cmp	w20, #1                 // =1
	add	x2, x2, :lo12:.L.str.2
	csel	x20, x9, x8, eq
	bl	snprintf
	ldr	x8, [x19, #240]
	add	w21, w0, w21
	adrp	x2, .L.str.3
	add	x0, x24, x21
	sub	x1, x23, x21
	and	x3, x8, #0x1
	and	x4, x8, #0x2
	add	x2, x2, :lo12:.L.str.3
	bl	snprintf
	ldr	x5, [x19, #208]
	add	w21, w0, w21
	add	x0, x24, x21
	sub	x1, x23, x21
	cbnz	x5, .LBB0_2
// %bb.1:
	adrp	x2, .L.str.5
	add	x2, x2, :lo12:.L.str.5
	bl	snprintf
	add	w21, w0, w21
	b	.LBB0_8
.LBB0_2:
	ldr	w3, [x19, #216]
	adrp	x2, .L.str.4
	add	x4, x19, #224           // =224
	add	x2, x2, :lo12:.L.str.4
	bl	snprintf
	ldr	x8, [x19]
	add	w22, w0, w21
	mov	x0, x8
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB0_7
// %bb.3:
	ldr	x0, [x19]
	bl	mas_blk_get_sec_size
	ldr	x8, [x19]
	mov	w21, w0
	mov	x0, x8
	bl	mas_blk_get_pu_size
	mov	w8, wzr
	cbz	w21, .LBB0_6
// %bb.4:
	cbz	w0, .LBB0_6
// %bb.5:
	ldr	x10, [x19, #56]
	sub	w9, w23, w22
	ldr	w3, [x19, #736]
	ldr	w4, [x19, #740]
	ldrb	w13, [x19, #704]
	mov	w11, w0
	mov	w12, w21
	sxtw	x1, w9
	lsr	x9, x10, #3
	add	x8, x24, x22
	udiv	x5, x9, x11
	udiv	x7, x9, x12
	adrp	x2, .L.str.25
	msub	x6, x5, x11, x9
	msub	x9, x7, x12, x9
	add	x2, x2, :lo12:.L.str.25
	mov	x0, x8
	str	w13, [sp, #8]
	str	x9, [sp]
	bl	snprintf
	mov	w8, w0
.LBB0_6:
	add	w21, w8, w22
	b	.LBB0_8
.LBB0_7:
	mov	w8, wzr
	add	w21, wzr, w22
.LBB0_8:
	add	x23, x19, #288          // =288
	adrp	x24, req_stage_cfg
	adrp	x19, .L.str.6
	mov	x22, xzr
	add	x24, x24, :lo12:req_stage_cfg
	add	x25, sp, #40            // =40
	mov	w26, #1024
	add	x19, x19, :lo12:.L.str.6
.LBB0_9:                                // =>This Inner Loop Header: Depth=1
	ldr	x4, [x23, x22]
	cbz	x4, .LBB0_11
// %bb.10:                              //   in Loop: Header=BB0_9 Depth=1
	ldr	x3, [x24]
	mov	w8, w21
	add	x0, x25, x8
	sub	x1, x26, x8
	mov	x2, x19
	bl	snprintf
	add	w21, w0, w21
.LBB0_11:                               //   in Loop: Header=BB0_9 Depth=1
	add	x22, x22, #8            // =8
	cmp	x22, #416               // =416
	add	x24, x24, #16           // =16
	b.ne	.LBB0_9
// %bb.12:
	adrp	x0, .L.str.7
	add	x0, x0, :lo12:.L.str.7
	add	x2, sp, #40             // =40
	mov	x1, x20
	strb	wzr, [sp, #1063]
	bl	printk
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB0_14
// %bb.13:
	add	sp, sp, #1072           // =1072
	ldp	x20, x19, [sp, #80]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]     // 16-byte Folded Reload
	ldr	x28, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #96     // 16-byte Folded Reload
	ret
.LBB0_14:
	bl	__stack_chk_fail
.Lfunc_end0:
	.size	mas_blk_dump_request, .Lfunc_end0-mas_blk_dump_request
                                        // -- End function
	.globl	mas_blk_dump_bio        // -- Begin function mas_blk_dump_bio
	.p2align	2
	.type	mas_blk_dump_bio,@function
mas_blk_dump_bio:                       // @mas_blk_dump_bio
.Lmas_blk_dump_bio$local:
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	stp	x28, x25, [sp, #16]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #1056           // =1056
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	w19, w1
	mov	x20, x0
	add	x0, sp, #24             // =24
	mov	w2, #1024
	mov	w1, wzr
	stur	x8, [x29, #-8]
	mov	w24, #1024
	add	x23, sp, #24            // =24
	bl	memset
	strh	wzr, [sp, #16]
	str	xzr, [sp, #8]
	ldr	w1, [x20, #16]
	ldr	x2, [x20, #88]
	add	x3, sp, #8              // =8
	and	x0, x1, #0xff
                                        // kill: def $w1 killed $w1 killed $x1
	bl	mas_blk_io_type
	ldr	w5, [x20, #40]
	adrp	x8, .L.str.21
	adrp	x9, .L.str.20
	adrp	x2, .L.str.8
	mov	x4, x0
	add	x8, x8, :lo12:.L.str.21
	add	x9, x9, :lo12:.L.str.20
	cmp	w19, #1                 // =1
	add	x2, x2, :lo12:.L.str.8
	add	x0, sp, #24             // =24
	mov	w1, #1024
	mov	x3, x20
	csel	x21, x9, x8, eq
	bl	snprintf
	ldr	x8, [x20, #112]
	mov	w22, w0
	cbz	x8, .LBB1_7
// %bb.1:
	ldr	w3, [x20, #120]
	sxtw	x8, w22
	adrp	x2, .L.str.9
	add	x0, x23, x8
	sub	x1, x24, x8
	add	x4, x20, #128           // =128
	add	x2, x2, :lo12:.L.str.9
	bl	snprintf
	ldr	x3, [x20, #112]
	add	w25, w0, w22
	sxtw	x8, w25
	adrp	x2, .L.str.10
	add	x0, x23, x8
	sub	x1, x24, x8
	add	x2, x2, :lo12:.L.str.10
	bl	snprintf
	ldr	x8, [x20, #8]
	add	w25, w0, w25
	ldr	x22, [x8, #1272]
	mov	x0, x22
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB1_5
// %bb.2:
	ldr	x8, [x20, #104]
	cbnz	x8, .LBB1_5
// %bb.3:
	mov	x0, x22
	bl	mas_blk_get_sec_size
	cbz	w0, .LBB1_6
// %bb.4:
	ldr	x9, [x20, #32]
	sub	w10, w24, w25
	ldrb	w5, [x20, #272]
	sxtw	x1, w10
	lsr	x9, x9, #3
	mov	w10, w0
	add	x8, x23, w25, sxtw
	udiv	x3, x9, x10
	adrp	x2, .L.str.27
	msub	x4, x3, x10, x9
	add	x2, x2, :lo12:.L.str.27
	mov	x0, x8
	bl	snprintf
	b	.LBB1_6
.LBB1_5:
	mov	w0, wzr
.LBB1_6:
	add	w22, w0, w25
.LBB1_7:
	sxtw	x8, w22
	mov	w9, #1024
	adrp	x2, .L.str.5
	add	x0, x23, x8
	sub	x1, x9, x8
	add	x2, x2, :lo12:.L.str.5
	bl	snprintf
	ldr	x4, [x20, #152]
	add	w22, w0, w22
	cbnz	x4, .LBB1_16
// %bb.8:
	ldr	x4, [x20, #160]
	cbnz	x4, .LBB1_17
.LBB1_9:
	ldr	x4, [x20, #168]
	cbnz	x4, .LBB1_18
.LBB1_10:
	ldr	x4, [x20, #176]
	cbz	x4, .LBB1_12
.LBB1_11:
	adrp	x10, bio_stage_cfg+48
	ldr	x3, [x10, :lo12:bio_stage_cfg+48]
	sxtw	x8, w22
	add	x9, sp, #24             // =24
	add	x0, x9, x8
	mov	w9, #1024
	adrp	x2, .L.str.11
	sub	x1, x9, x8
	add	x2, x2, :lo12:.L.str.11
	bl	snprintf
	add	w22, w0, w22
.LBB1_12:
	add	x8, sp, #24             // =24
	strb	wzr, [x8, w22, sxtw]
	bl	ktime_get
	mov	x3, x0
	adrp	x0, .L.str.12
	add	x0, x0, :lo12:.L.str.12
	add	x2, sp, #24             // =24
	mov	x1, x21
	bl	printk
	ldr	x0, [x20, #104]
	cbz	x0, .LBB1_14
// %bb.13:
	mov	w1, w19
	bl	mas_blk_dump_request
.LBB1_14:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB1_19
// %bb.15:
	add	sp, sp, #1056           // =1056
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldp	x28, x25, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB1_16:
	adrp	x10, bio_stage_cfg
	ldr	x3, [x10, :lo12:bio_stage_cfg]
	sxtw	x8, w22
	add	x9, sp, #24             // =24
	add	x0, x9, x8
	mov	w9, #1024
	adrp	x2, .L.str.11
	sub	x1, x9, x8
	add	x2, x2, :lo12:.L.str.11
	bl	snprintf
	add	w22, w0, w22
	ldr	x4, [x20, #160]
	cbz	x4, .LBB1_9
.LBB1_17:
	adrp	x10, bio_stage_cfg+16
	ldr	x3, [x10, :lo12:bio_stage_cfg+16]
	sxtw	x8, w22
	add	x9, sp, #24             // =24
	add	x0, x9, x8
	mov	w9, #1024
	adrp	x2, .L.str.11
	sub	x1, x9, x8
	add	x2, x2, :lo12:.L.str.11
	bl	snprintf
	add	w22, w0, w22
	ldr	x4, [x20, #168]
	cbz	x4, .LBB1_10
.LBB1_18:
	adrp	x10, bio_stage_cfg+32
	ldr	x3, [x10, :lo12:bio_stage_cfg+32]
	sxtw	x8, w22
	add	x9, sp, #24             // =24
	add	x0, x9, x8
	mov	w9, #1024
	adrp	x2, .L.str.11
	sub	x1, x9, x8
	add	x2, x2, :lo12:.L.str.11
	bl	snprintf
	add	w22, w0, w22
	ldr	x4, [x20, #176]
	cbnz	x4, .LBB1_11
	b	.LBB1_12
.LBB1_19:
	bl	__stack_chk_fail
.Lfunc_end1:
	.size	mas_blk_dump_bio, .Lfunc_end1-mas_blk_dump_bio
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_io_type
	.type	mas_blk_io_type,@function
mas_blk_io_type:                        // @mas_blk_io_type
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x19, x3
	mov	x20, x2
	cmp	x0, #7                  // =7
	mov	w21, w1
	mov	x29, sp
	b.hi	.LBB2_38
// %bb.1:
	adrp	x8, .LJTI2_0
	add	x8, x8, :lo12:.LJTI2_0
	adr	x9, .LBB2_2
	ldrb	w10, [x8, x0]
	add	x9, x9, x10, lsl #2
	br	x9
.LBB2_2:
	mov	w8, #82
	b	.LBB2_8
.LBB2_3:
	mov	w8, #87
	b	.LBB2_8
.LBB2_4:
	mov	w8, #70
	b	.LBB2_8
.LBB2_5:
	mov	w8, #68
	b	.LBB2_8
.LBB2_6:
	mov	w8, #69
	b	.LBB2_8
.LBB2_7:
	mov	w8, #84
.LBB2_8:
	strb	w8, [x19]
	mov	w9, #1
.LBB2_9:
	mov	w8, w21
	add	x11, x19, x9
	tbnz	w8, #18, .LBB2_11
// %bb.10:
	mov	w10, wzr
	mov	w12, #10
	tbnz	w8, #11, .LBB2_12
	b	.LBB2_13
.LBB2_11:
	mov	w10, #65
	strb	w10, [x11]
	mov	w10, #1
	mov	w12, #10
	tbz	w8, #11, .LBB2_13
.LBB2_12:
	mov	w13, #83
	strb	w13, [x11, w10, uxtw]
	add	w10, w10, #1            // =1
.LBB2_13:
	sub	w12, w12, w9
	tbz	w8, #12, .LBB2_16
// %bb.14:
	cmp	w10, w12
	b.ge	.LBB2_16
// %bb.15:
	mov	w13, #77
	strb	w13, [x11, w10, uxtw]
	add	w10, w10, #1            // =1
.LBB2_16:
	tbz	w8, #19, .LBB2_19
// %bb.17:
	cmp	w10, w12
	b.ge	.LBB2_19
// %bb.18:
	mov	w12, #70
	strb	w12, [x11, w10, uxtw]
	add	w10, w10, #1            // =1
.LBB2_19:
	mov	w11, wzr
	add	x9, x10, x9
	tbz	w20, #7, .LBB2_22
// %bb.20:
	cmp	w9, #9                  // =9
	b.gt	.LBB2_22
// %bb.21:
	mov	w10, #76
	mov	w11, #1
	strb	w10, [x19, x9]
.LBB2_22:
	add	w9, w11, w9
	mov	w10, wzr
	add	x11, x19, x9
	mov	w12, #10
	tbz	w8, #22, .LBB2_25
// %bb.23:
	cmp	w9, #9                  // =9
	b.gt	.LBB2_25
// %bb.24:
	mov	w8, #72
	mov	w10, #1
	strb	w8, [x11]
.LBB2_25:
	sub	w8, w12, w9
	tbz	w20, #0, .LBB2_28
// %bb.26:
	cmp	w10, w8
	b.ge	.LBB2_28
// %bb.27:
	mov	w12, #67
	strb	w12, [x11, w10, uxtw]
	add	w10, w10, #1            // =1
.LBB2_28:
	tbz	w20, #2, .LBB2_31
// %bb.29:
	cmp	w10, w8
	b.ge	.LBB2_31
// %bb.30:
	mov	w12, #90
	strb	w12, [x11, w10, uxtw]
	add	w10, w10, #1            // =1
.LBB2_31:
	tbz	w20, #1, .LBB2_34
// %bb.32:
	cmp	w10, w8
	b.ge	.LBB2_34
// %bb.33:
	mov	w12, #86
	strb	w12, [x11, w10, uxtw]
	add	w10, w10, #1            // =1
.LBB2_34:
	tbz	w20, #5, .LBB2_37
// %bb.35:
	cmp	w10, w8
	b.ge	.LBB2_37
// %bb.36:
	mov	w8, #84
	strb	w8, [x11, w10, uxtw]
	add	w10, w10, #1            // =1
.LBB2_37:
	add	w8, w10, w9
	strb	wzr, [x19, w8, uxtw]
	mov	x0, x19
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB2_38:
	adrp	x0, .L.str.26
	adrp	x1, .L__func__.mas_blk_io_type_op
	add	x0, x0, :lo12:.L.str.26
	add	x1, x1, :lo12:.L__func__.mas_blk_io_type_op
	bl	printk
	mov	w9, wzr
	b	.LBB2_9
.Lfunc_end2:
	.size	mas_blk_io_type, .Lfunc_end2-mas_blk_io_type
	.section	.rodata,"a",@progbits
.LJTI2_0:
	.byte	(.LBB2_2-.LBB2_2)>>2
	.byte	(.LBB2_3-.LBB2_2)>>2
	.byte	(.LBB2_4-.LBB2_2)>>2
	.byte	(.LBB2_5-.LBB2_2)>>2
	.byte	(.LBB2_38-.LBB2_2)>>2
	.byte	(.LBB2_6-.LBB2_2)>>2
	.byte	(.LBB2_38-.LBB2_2)>>2
	.byte	(.LBB2_7-.LBB2_2)>>2
                                        // -- End function
	.text
	.globl	mas_blk_dump_queue_status2 // -- Begin function mas_blk_dump_queue_status2
	.p2align	2
	.type	mas_blk_dump_queue_status2,@function
mas_blk_dump_queue_status2:             // @mas_blk_dump_queue_status2
.Lmas_blk_dump_queue_status2$local:
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	str	x28, [sp, #16]          // 8-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #1040           // =1040
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	w22, w2
	mov	x19, x1
	mov	x21, x0
	add	x0, sp, #8              // =8
	mov	w2, #1024
	mov	w1, wzr
	stur	x8, [x29, #-8]
	bl	memset
	adrp	x2, .L.str.28
	add	x2, x2, :lo12:.L.str.28
	add	x0, sp, #8              // =8
	mov	w1, #1024
	bl	snprintf
	cbz	x19, .LBB3_16
// %bb.1:
	cmp	w22, #1                 // =1
	b.lt	.LBB3_16
// %bb.2:
	adrp	x2, .L.str.29
	mov	w1, w22
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #8              // =8
	mov	x0, x19
	bl	snprintf
	mov	w20, w0
.LBB3_3:
	add	x0, sp, #8              // =8
	mov	w2, #1024
	mov	w1, wzr
	sub	w22, w22, w20
	bl	memset
	ldr	w8, [x21, #1276]
	cbz	w8, .LBB3_5
// %bb.4:
	ldr	w3, [x21, #1280]
	adrp	x2, .L.str.31
	add	x2, x2, :lo12:.L.str.31
	add	x0, sp, #8              // =8
	mov	w1, #1024
	bl	snprintf
	b	.LBB3_6
.LBB3_5:
	adrp	x2, .L.str.32
	add	x2, x2, :lo12:.L.str.32
	add	x0, sp, #8              // =8
	mov	w1, #1024
	bl	snprintf
.LBB3_6:
	cmp	w22, #1                 // =1
	add	x24, x19, w20, sxtw
	b.lt	.LBB3_17
// %bb.7:
	cbz	x24, .LBB3_17
// %bb.8:
	adrp	x2, .L.str.29
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #8              // =8
	mov	x0, x24
	mov	x1, x22
	bl	snprintf
	mov	w23, w0
.LBB3_9:
	ldr	w8, [x21, #1272]
	adrp	x9, .L.str.34
	adrp	x10, .L.str.35
	add	x9, x9, :lo12:.L.str.34
	add	x10, x10, :lo12:.L.str.35
	cmp	w8, #0                  // =0
	adrp	x2, .L.str.33
	csel	x3, x10, x9, eq
	add	x2, x2, :lo12:.L.str.33
	add	x0, sp, #8              // =8
	mov	w1, #1024
	bl	snprintf
	sub	w8, w22, w23
	cmp	w8, #1                  // =1
	b.lt	.LBB3_18
// %bb.10:
	add	x0, x24, w23, sxtw
	cbz	x0, .LBB3_18
// %bb.11:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #8              // =8
	bl	snprintf
.LBB3_12:
	add	w8, w23, w20
	add	w0, w8, w0
	cbz	x19, .LBB3_14
// %bb.13:
	strb	wzr, [x19, w0, sxtw]
.LBB3_14:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB3_19
// %bb.15:
	add	sp, sp, #1040           // =1040
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldr	x28, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB3_16:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #8              // =8
	bl	printk
	mov	w20, wzr
	b	.LBB3_3
.LBB3_17:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #8              // =8
	bl	printk
	mov	w23, wzr
	b	.LBB3_9
.LBB3_18:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #8              // =8
	bl	printk
	mov	w0, wzr
	b	.LBB3_12
.LBB3_19:
	bl	__stack_chk_fail
.Lfunc_end3:
	.size	mas_blk_dump_queue_status2, .Lfunc_end3-mas_blk_dump_queue_status2
                                        // -- End function
	.globl	mas_blk_dump_queue_status // -- Begin function mas_blk_dump_queue_status
	.p2align	2
	.type	mas_blk_dump_queue_status,@function
mas_blk_dump_queue_status:              // @mas_blk_dump_queue_status
.Lmas_blk_dump_queue_status$local:
// %bb.0:
	sub	sp, sp, #64             // =64
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x21, [sp, #32]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	str	x8, [sp, #8]
	str	xzr, [sp]
	ldr	x8, [x0, #1064]
	add	x29, sp, #16            // =16
	cbnz	x8, .LBB4_3
.LBB4_1:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB4_8
// %bb.2:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldr	x21, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.LBB4_3:
	adrp	x9, .L.str.21
	adrp	x10, .L.str.20
	mov	w20, w1
	add	x9, x9, :lo12:.L.str.21
	add	x10, x10, :lo12:.L.str.20
	cmp	w1, #1                  // =1
	add	x1, x8, #72             // =72
	mov	x2, sp
	mov	x19, x0
	csel	x21, x10, x9, eq
	bl	blk_mq_in_flight_rw
	ldrb	w9, [x19, #1072]
	ldr	x8, [x19, #1064]
	ldp	w4, w5, [sp]
	adrp	x10, .L.str.14
	adrp	x11, .L.str.15
	add	x10, x10, :lo12:.L.str.14
	add	x11, x11, :lo12:.L.str.15
	cmp	w9, #0                  // =0
	adrp	x0, .L.str.13
	add	x2, x8, #12             // =12
	csel	x3, x11, x10, eq
	add	x0, x0, :lo12:.L.str.13
	mov	x1, x21
	bl	printk
	adrp	x0, .L.str.16
	add	x0, x0, :lo12:.L.str.16
	mov	x1, x19
	bl	printk
	cmp	w20, #1                 // =1
	b.ne	.LBB4_5
// %bb.4:
	mov	x0, x19
	mov	x1, xzr
	mov	w2, wzr
	bl	mas_blk_dump_queue_status2
.LBB4_5:
	ldr	x8, [x19, #1400]
	cbz	x8, .LBB4_1
// %bb.6:
	ldr	x8, [x8, #160]
	cbz	x8, .LBB4_1
// %bb.7:
	mov	x0, x19
	mov	w1, w20
	blr	x8
	b	.LBB4_1
.LBB4_8:
	bl	__stack_chk_fail
.Lfunc_end4:
	.size	mas_blk_dump_queue_status, .Lfunc_end4-mas_blk_dump_queue_status
                                        // -- End function
	.globl	mas_blk_dump_lld_status // -- Begin function mas_blk_dump_lld_status
	.p2align	2
	.type	mas_blk_dump_lld_status,@function
mas_blk_dump_lld_status:                // @mas_blk_dump_lld_status
.Lmas_blk_dump_lld_status$local:
// %bb.0:
	stp	x29, x30, [sp, #-96]!   // 16-byte Folded Spill
	stp	x28, x27, [sp, #16]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #32]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #1072           // =1072
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	w21, w2
	mov	x19, x1
	mov	x20, x0
	add	x0, sp, #32             // =32
	mov	w2, #1024
	mov	w1, wzr
	stur	x8, [x29, #-16]
	bl	memset
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	cbz	x19, .LBB5_16
// %bb.1:
	cmp	w21, #1                 // =1
	b.lt	.LBB5_16
// %bb.2:
	adrp	x2, .L.str.29
	mov	w1, w21
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	mov	x0, x19
	bl	snprintf
	mov	w22, w0
.LBB5_3:
	ldr	x8, [x20, #24]
	adrp	x24, .L.str.34
	adrp	x25, .L.str.35
	add	x24, x24, :lo12:.L.str.34
	add	x25, x25, :lo12:.L.str.35
	tst	x8, #0x1
	adrp	x2, .L.str.37
	csel	x3, x25, x24, eq
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w21, w22
	cmp	w8, #1                  // =1
	b.lt	.LBB5_17
// %bb.4:
	add	x0, x19, w22, sxtw
	cbz	x0, .LBB5_17
// %bb.5:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_6:
	add	w22, w0, w22
	add	x0, sp, #32             // =32
	mov	w2, #1024
	mov	w1, wzr
	sub	w23, w21, w22
	bl	memset
	ldrb	w8, [x20, #24]
	tbnz	w8, #1, .LBB5_8
// %bb.7:
	adrp	x2, .L.str.39
	add	x2, x2, :lo12:.L.str.39
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	cmp	w23, #1                 // =1
	b.ge	.LBB5_9
	b	.LBB5_18
.LBB5_8:
	ldr	w3, [x20, #48]
	adrp	x2, .L.str.38
	add	x2, x2, :lo12:.L.str.38
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	cmp	w23, #1                 // =1
	b.lt	.LBB5_18
.LBB5_9:
	add	x0, x19, w22, sxtw
	cbz	x0, .LBB5_18
// %bb.10:
	adrp	x2, .L.str.29
	mov	w1, w23
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_11:
	add	w26, w0, w22
	add	x0, sp, #32             // =32
	mov	w2, #1024
	mov	w1, wzr
	add	x22, x19, w26, sxtw
	sub	w27, w21, w26
	bl	memset
	ldr	x8, [x20, #24]
	adrp	x2, .L.str.40
	add	x2, x2, :lo12:.L.str.40
	add	x0, sp, #32             // =32
	tst	x8, #0x8
	csel	x3, x25, x24, eq
	mov	w1, #1024
	bl	snprintf
	cmp	w27, #1                 // =1
	b.lt	.LBB5_19
// %bb.12:
	cbz	x22, .LBB5_19
// %bb.13:
	adrp	x2, .L.str.29
	mov	w1, w27
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	mov	x0, x22
	bl	snprintf
	mov	w23, w0
	ldr	w8, [x20, #1168]
	cbz	w8, .LBB5_20
.LBB5_14:
	cmp	w8, #1                  // =1
	b.ne	.LBB5_28
// %bb.15:
	adrp	x2, .L.str.42
	add	x2, x2, :lo12:.L.str.42
	b	.LBB5_21
.LBB5_16:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w22, wzr
	b	.LBB5_3
.LBB5_17:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_6
.LBB5_18:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_11
.LBB5_19:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w23, wzr
	ldr	w8, [x20, #1168]
	cbnz	w8, .LBB5_14
.LBB5_20:
	adrp	x2, .L.str.41
	add	x2, x2, :lo12:.L.str.41
.LBB5_21:
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w27, w23
	cmp	w8, #1                  // =1
	b.lt	.LBB5_34
// %bb.22:
	add	x0, x22, w23, sxtw
	cbz	x0, .LBB5_34
// %bb.23:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_24:
	ldr	w3, [x20, #1120]
	adrp	x2, .L.str.43
	add	w23, w0, w23
	add	x2, x2, :lo12:.L.str.43
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w27, w23
	cmp	w8, #1                  // =1
	b.lt	.LBB5_35
// %bb.25:
	add	x0, x22, w23, sxtw
	cbz	x0, .LBB5_35
// %bb.26:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_27:
	add	w23, w0, w23
.LBB5_28:
	ldr	x28, [x20, #24]
	add	w26, w23, w26
	tbz	w28, #9, .LBB5_46
// %bb.29:
	add	x0, sp, #32             // =32
	mov	w2, #1024
	mov	w1, wzr
	add	x22, x19, w26, sxtw
	sub	w27, w21, w26
	bl	memset
	tst	x28, #0x8
	adrp	x2, .L.str.44
	csel	x3, x25, x24, eq
	add	x2, x2, :lo12:.L.str.44
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	cmp	w27, #1                 // =1
	b.lt	.LBB5_36
// %bb.30:
	cbz	x22, .LBB5_36
// %bb.31:
	adrp	x2, .L.str.29
	mov	w1, w27
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	mov	x0, x22
	bl	snprintf
	mov	w23, w0
	ldr	w8, [x20, #1816]
	cbz	w8, .LBB5_37
.LBB5_32:
	cmp	w8, #1                  // =1
	b.ne	.LBB5_45
// %bb.33:
	adrp	x2, .L.str.46
	add	x2, x2, :lo12:.L.str.46
	b	.LBB5_38
.LBB5_34:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_24
.LBB5_35:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_27
.LBB5_36:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w23, wzr
	ldr	w8, [x20, #1816]
	cbnz	w8, .LBB5_32
.LBB5_37:
	adrp	x2, .L.str.45
	add	x2, x2, :lo12:.L.str.45
.LBB5_38:
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w27, w23
	cmp	w8, #1                  // =1
	b.lt	.LBB5_99
// %bb.39:
	add	x0, x22, w23, sxtw
	cbz	x0, .LBB5_99
// %bb.40:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_41:
	ldr	w3, [x20, #1772]
	adrp	x2, .L.str.47
	add	w23, w0, w23
	add	x2, x2, :lo12:.L.str.47
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w27, w23
	cmp	w8, #1                  // =1
	b.lt	.LBB5_100
// %bb.42:
	add	x0, x22, w23, sxtw
	cbz	x0, .LBB5_100
// %bb.43:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_44:
	add	w23, w0, w23
.LBB5_45:
	add	w26, w23, w26
.LBB5_46:
	add	x0, sp, #32             // =32
	mov	w2, #1024
	mov	w1, wzr
	add	x22, x19, w26, sxtw
	sub	w27, w21, w26
	bl	memset
	adrp	x2, .L.str.48
	add	x2, x2, :lo12:.L.str.48
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	cmp	w27, #1                 // =1
	b.lt	.LBB5_87
// %bb.47:
	cbz	x22, .LBB5_87
// %bb.48:
	adrp	x2, .L.str.29
	mov	w1, w27
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	mov	x0, x22
	bl	snprintf
	mov	w23, w0
.LBB5_49:
	ldr	x8, [x20, #24]
	adrp	x9, .L.str.49
	adrp	x10, .L.str.50
	add	x9, x9, :lo12:.L.str.49
	add	x10, x10, :lo12:.L.str.50
	tst	x8, #0x20
	csel	x2, x10, x9, eq
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w27, w23
	cmp	w8, #1                  // =1
	b.lt	.LBB5_88
// %bb.50:
	add	x0, x22, w23, sxtw
	cbz	x0, .LBB5_88
// %bb.51:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_52:
	add	w8, w23, w26
	add	w23, w8, w0
	add	x0, sp, #32             // =32
	mov	w2, #1024
	mov	w1, wzr
	add	x22, x19, w23, sxtw
	sub	w26, w21, w23
	bl	memset
	ldr	x8, [x20, #24]
	adrp	x2, .L.str.33
	add	x2, x2, :lo12:.L.str.33
	add	x0, sp, #32             // =32
	tst	x8, #0x4
	csel	x3, x25, x24, eq
	mov	w1, #1024
	bl	snprintf
	cmp	w26, #1                 // =1
	b.lt	.LBB5_89
// %bb.53:
	cbz	x22, .LBB5_89
// %bb.54:
	adrp	x2, .L.str.29
	mov	w1, w26
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	mov	x0, x22
	bl	snprintf
	mov	w21, w0
.LBB5_55:
	ldr	x8, [x20, #24]
	adrp	x2, .L.str.51
	add	x2, x2, :lo12:.L.str.51
	add	x0, sp, #32             // =32
	tst	x8, #0x10
	csel	x3, x25, x24, eq
	mov	w1, #1024
	bl	snprintf
	sub	w8, w26, w21
	cmp	w8, #1                  // =1
	b.lt	.LBB5_90
// %bb.56:
	add	x0, x22, w21, sxtw
	cbz	x0, .LBB5_90
// %bb.57:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_58:
	ldr	x8, [x20, #24]
	adrp	x2, .L.str.52
	add	w21, w0, w21
	add	x2, x2, :lo12:.L.str.52
	tst	x8, #0x80
	csel	x3, x25, x24, eq
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w26, w21
	cmp	w8, #1                  // =1
	b.lt	.LBB5_91
// %bb.59:
	add	x0, x22, w21, sxtw
	cbz	x0, .LBB5_91
// %bb.60:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_61:
	ldr	x8, [x20, #24]
	adrp	x2, .L.str.53
	add	w21, w0, w21
	add	x2, x2, :lo12:.L.str.53
	tst	x8, #0x100
	csel	x3, x25, x24, eq
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w26, w21
	cmp	w8, #1                  // =1
	b.lt	.LBB5_92
// %bb.62:
	add	x0, x22, w21, sxtw
	cbz	x0, .LBB5_92
// %bb.63:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_64:
	ldrb	w8, [x20, #25]
	add	w24, w0, w21
	tbnz	w8, #1, .LBB5_66
// %bb.65:
	mov	w8, wzr
	b	.LBB5_83
.LBB5_66:
	ldr	w8, [x20, #248]
	cbz	w8, .LBB5_93
// %bb.67:
	ldrb	w8, [x20, #317]
	ldr	w4, [x20, #1900]
	ldrb	w5, [x20, #316]
	adrp	x9, .L.str.56
	adrp	x10, .L.str.57
	add	x9, x9, :lo12:.L.str.56
	add	x10, x10, :lo12:.L.str.57
	cmp	w8, #0                  // =0
	adrp	x2, .L.str.55
	adrp	x3, .L.str.34
	csel	x6, x10, x9, eq
	add	x2, x2, :lo12:.L.str.55
	add	x3, x3, :lo12:.L.str.34
	add	x0, sp, #32             // =32
	mov	w1, #1024
	add	x21, x22, w24, sxtw
	sub	w25, w26, w24
	bl	snprintf
	cmp	w25, #1                 // =1
	b.lt	.LBB5_94
// %bb.68:
	cbz	x21, .LBB5_94
// %bb.69:
	adrp	x2, .L.str.29
	mov	w1, w25
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	mov	x0, x21
	bl	snprintf
	mov	w22, w0
.LBB5_70:
	ldr	w8, [x20, #248]
	ldp	x9, x10, [x20, #352]
	ldp	x11, x12, [x20, #368]
	adrp	x2, .L.str.58
	add	x2, x2, :lo12:.L.str.58
	udiv	x3, x9, x8
	udiv	x5, x10, x8
	udiv	x7, x11, x8
	udiv	x13, x12, x8
	msub	x4, x3, x8, x9
	msub	x6, x5, x8, x10
	msub	x9, x7, x8, x11
	msub	x8, x13, x8, x12
	add	x0, sp, #32             // =32
	mov	w1, #1024
	stp	x13, x8, [sp, #8]
	str	x9, [sp]
	bl	snprintf
	sub	w8, w25, w22
	cmp	w8, #1                  // =1
	b.lt	.LBB5_95
// %bb.71:
	add	x0, x21, w22, sxtw
	cbz	x0, .LBB5_95
// %bb.72:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_73:
	ldr	x3, [x20, #408]
	ldr	x4, [x20, #440]
	adrp	x2, .L.str.59
	add	w22, w0, w22
	add	x2, x2, :lo12:.L.str.59
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w25, w22
	cmp	w8, #1                  // =1
	b.lt	.LBB5_96
// %bb.74:
	add	x0, x21, w22, sxtw
	cbz	x0, .LBB5_96
// %bb.75:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_76:
	ldp	x3, x4, [x20, #448]
	ldp	x5, x6, [x20, #464]
	adrp	x2, .L.str.60
	add	w22, w0, w22
	add	x2, x2, :lo12:.L.str.60
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w25, w22
	cmp	w8, #1                  // =1
	b.lt	.LBB5_97
// %bb.77:
	add	x0, x21, w22, sxtw
	cbz	x0, .LBB5_97
// %bb.78:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_79:
	ldp	x3, x4, [x20, #480]
	ldp	x5, x6, [x20, #496]
	adrp	x2, .L.str.61
	add	w20, w0, w22
	add	x2, x2, :lo12:.L.str.61
	add	x0, sp, #32             // =32
	mov	w1, #1024
	bl	snprintf
	sub	w8, w25, w20
	cmp	w8, #1                  // =1
	b.lt	.LBB5_98
// %bb.80:
	add	x0, x21, w20, sxtw
	cbz	x0, .LBB5_98
// %bb.81:
	adrp	x2, .L.str.29
	mov	w1, w8
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #32             // =32
	bl	snprintf
.LBB5_82:
	add	w8, w0, w20
.LBB5_83:
	add	w9, w24, w23
	add	w0, w9, w8
	cbz	x19, .LBB5_85
// %bb.84:
	strb	wzr, [x19, w0, sxtw]
.LBB5_85:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-16]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB5_101
// %bb.86:
	add	sp, sp, #1072           // =1072
	ldp	x20, x19, [sp, #80]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #96     // 16-byte Folded Reload
	ret
.LBB5_87:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w23, wzr
	b	.LBB5_49
.LBB5_88:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_52
.LBB5_89:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w21, wzr
	b	.LBB5_55
.LBB5_90:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_58
.LBB5_91:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_61
.LBB5_92:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_64
.LBB5_93:
	adrp	x0, .L.str.54
	adrp	x1, .L__func__.mas_blk_lld_dump_opt_feature_unist
	add	x0, x0, :lo12:.L.str.54
	add	x1, x1, :lo12:.L__func__.mas_blk_lld_dump_opt_feature_unist
	mov	w2, wzr
	bl	printk
	mov	w8, wzr
	b	.LBB5_83
.LBB5_94:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w22, wzr
	b	.LBB5_70
.LBB5_95:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_73
.LBB5_96:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_76
.LBB5_97:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_79
.LBB5_98:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_82
.LBB5_99:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_41
.LBB5_100:
	adrp	x0, .L.str.30
	add	x0, x0, :lo12:.L.str.30
	add	x1, sp, #32             // =32
	bl	printk
	mov	w0, wzr
	b	.LBB5_44
.LBB5_101:
	bl	__stack_chk_fail
.Lfunc_end5:
	.size	mas_blk_dump_lld_status, .Lfunc_end5-mas_blk_dump_lld_status
                                        // -- End function
	.globl	mas_blk_panic_dump      // -- Begin function mas_blk_panic_dump
	.p2align	2
	.type	mas_blk_panic_dump,@function
mas_blk_panic_dump:                     // @mas_blk_panic_dump
.Lmas_blk_panic_dump$local:
// %bb.0:
	sub	sp, sp, #112            // =112
	stp	x28, x27, [sp, #32]     // 16-byte Folded Spill
	adrp	x28, dump_list
	add	x28, x28, :lo12:dump_list
	stp	x22, x21, [sp, #80]     // 16-byte Folded Spill
	ldr	x21, [x28]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #48]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #64]     // 16-byte Folded Spill
	cmp	x21, x28
	stp	x20, x19, [sp, #96]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	b.eq	.LBB6_39
// %bb.1:
	adrp	x19, .L.str.76
	adrp	x20, .L.str.75
	adrp	x27, .L.str.63
	adrp	x23, .L.str.64
	add	x19, x19, :lo12:.L.str.76
	add	x20, x20, :lo12:.L.str.75
	add	x27, x27, :lo12:.L.str.63
	add	x23, x23, :lo12:.L.str.64
	b	.LBB6_3
.LBB6_2:                                //   in Loop: Header=BB6_3 Depth=1
	ldr	x21, [x21]
	cmp	x21, x28
	b.eq	.LBB6_35
.LBB6_3:                                // =>This Loop Header: Depth=1
                                        //     Child Loop BB6_25 Depth 2
                                        //     Child Loop BB6_16 Depth 2
                                        //     Child Loop BB6_21 Depth 2
	sub	x22, x21, #1288         // =1288
	mov	x0, x22
	bl	mas_blk_get_lld
	ldr	x8, [x0, #40]
	mov	x24, x0
	cbnz	x8, .LBB6_13
.LBB6_4:                                //   in Loop: Header=BB6_3 Depth=1
	mov	w1, #1
	mov	x0, x22
	bl	mas_blk_dump_queue_status
	cbz	x24, .LBB6_8
// %bb.5:                               //   in Loop: Header=BB6_3 Depth=1
	ldr	w8, [x24, #248]
	cbz	w8, .LBB6_8
// %bb.6:                               //   in Loop: Header=BB6_3 Depth=1
	ldr	w9, [x24, #252]
	cbz	w9, .LBB6_8
// %bb.7:                               //   in Loop: Header=BB6_3 Depth=1
	ldrb	w9, [x24, #25]
	tbnz	w9, #1, .LBB6_24
.LBB6_8:                                //   in Loop: Header=BB6_3 Depth=1
	ldr	x8, [x24, #1840]
	add	x25, x24, #1840         // =1840
	cmp	x25, x8
	b.ne	.LBB6_14
.LBB6_9:                                //   in Loop: Header=BB6_3 Depth=1
	ldr	x8, [x24, #1856]
	add	x24, x24, #1856         // =1856
	cmp	x24, x8
	b.ne	.LBB6_19
.LBB6_10:                               //   in Loop: Header=BB6_3 Depth=1
	ldr	x8, [x21, #112]
	cbz	x8, .LBB6_2
// %bb.11:                              //   in Loop: Header=BB6_3 Depth=1
	ldr	x8, [x8, #168]
	cbz	x8, .LBB6_2
// %bb.12:                              //   in Loop: Header=BB6_3 Depth=1
	mov	w1, #1
	mov	x0, x22
	blr	x8
	b	.LBB6_2
.LBB6_13:                               //   in Loop: Header=BB6_3 Depth=1
	mov	x0, x24
	mov	x1, xzr
	mov	w2, wzr
	bl	mas_blk_dump_lld_status
	ldr	x1, [x24, #1824]
	ldr	x2, [x24, #1832]
	adrp	x0, .L.str.62
	add	x0, x0, :lo12:.L.str.62
	bl	printk
	ldr	x8, [x24, #40]
	mov	w1, #1
	mov	x0, x22
	blr	x8
	b	.LBB6_4
.LBB6_14:                               //   in Loop: Header=BB6_3 Depth=1
	mov	x0, x20
	bl	printk
	ldr	x26, [x25]
	b	.LBB6_16
.LBB6_15:                               //   in Loop: Header=BB6_16 Depth=2
	ldr	x26, [x26]
.LBB6_16:                               //   Parent Loop BB6_3 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	cmp	x25, x26
	b.eq	.LBB6_9
// %bb.17:                              //   in Loop: Header=BB6_16 Depth=2
	ldur	x8, [x26, #-240]
	cmp	x8, x22
	b.ne	.LBB6_15
// %bb.18:                              //   in Loop: Header=BB6_16 Depth=2
	sub	x0, x26, #336           // =336
	mov	w1, #1
	bl	mas_blk_dump_bio
	b	.LBB6_15
.LBB6_19:                               //   in Loop: Header=BB6_3 Depth=1
	mov	x0, x19
	bl	printk
	ldr	x25, [x24]
	b	.LBB6_21
.LBB6_20:                               //   in Loop: Header=BB6_21 Depth=2
	ldr	x25, [x25]
.LBB6_21:                               //   Parent Loop BB6_3 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	cmp	x24, x25
	b.eq	.LBB6_10
// %bb.22:                              //   in Loop: Header=BB6_21 Depth=2
	sub	x0, x25, #784           // =784
	ldr	x8, [x0]
	cmp	x8, x22
	b.ne	.LBB6_20
// %bb.23:                              //   in Loop: Header=BB6_21 Depth=2
	mov	w1, #1
	bl	mas_blk_dump_request
	b	.LBB6_20
.LBB6_24:                               //   in Loop: Header=BB6_3 Depth=1
	ldp	x9, x10, [x24, #352]
	ldp	x11, x12, [x24, #368]
	mov	x0, x27
	udiv	x1, x9, x8
	udiv	x3, x10, x8
	udiv	x5, x11, x8
	udiv	x7, x12, x8
	msub	x2, x1, x8, x9
	msub	x4, x3, x8, x10
	msub	x6, x5, x8, x11
	msub	x8, x7, x8, x12
	str	x8, [sp]
	bl	printk
	ldr	x1, [x24, #408]
	ldp	w9, w8, [x24, #248]
	mov	x0, x23
	mul	x8, x1, x8
	udiv	x2, x8, x9
	msub	x3, x2, x9, x8
	bl	printk
	ldr	x1, [x24, #440]
	adrp	x0, .L.str.65
	add	x0, x0, :lo12:.L.str.65
	bl	printk
	ldp	x1, x2, [x24, #480]
	ldp	x3, x4, [x24, #496]
	adrp	x0, .L.str.66
	add	x0, x0, :lo12:.L.str.66
	bl	printk
	ldr	w1, [x24, #576]
	adrp	x0, .L.str.67
	add	x0, x0, :lo12:.L.str.67
	bl	printk
	ldr	w1, [x24, #580]
	adrp	x0, .L.str.68
	add	x0, x0, :lo12:.L.str.68
	bl	printk
	ldr	w1, [x24, #584]
	adrp	x0, .L.str.69
	add	x0, x0, :lo12:.L.str.69
	bl	printk
	ldr	w1, [x24, #588]
	adrp	x0, .L.str.70
	add	x0, x0, :lo12:.L.str.70
	bl	printk
	ldr	x1, [x24, #872]
	adrp	x0, .L.str.71
	add	x0, x0, :lo12:.L.str.71
	bl	printk
	mov	x25, xzr
.LBB6_25:                               //   Parent Loop BB6_3 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	//APP
	.if 1 == 1
661:
	mrs	x26, daif
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 42
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
		.irp	num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
	.equ	.L__reg_num_x\num, \num
	.endr
	.equ	.L__reg_num_xzr, 31
	.macro	mrs_s, rt, sreg
.inst (0xd5200000|(\sreg)|(.L__reg_num_\rt))
		.endm
	mrs_s x26, (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5))
	.purgem	mrs_s

664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	//APP
	.if 1 == 1
661:
	and	w8, w26, #0x00000080
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 42
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	eor	w8, w26, #0xe0
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	mov	x23, x20
	mov	x20, x19
	mov	x19, x27
	cbnz	w8, .LBB6_27
// %bb.26:                              //   in Loop: Header=BB6_25 Depth=2
	mov	w8, #96
	//APP
	.if 1 == 1
661:
	msr	daifset, #2		// arch_local_irq_disable
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 42
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
		.irp	num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
	.equ	.L__reg_num_x\num, \num
	.endr
	.equ	.L__reg_num_xzr, 31
	.macro	msr_s, sreg, rt
.inst (0xd5000000|(\sreg)|(.L__reg_num_\rt))
		.endm
	msr_s (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5)), x8
	.purgem	msr_s

664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
.LBB6_27:                               //   in Loop: Header=BB6_25 Depth=2
	add	x8, x24, x25, lsl #2
	add	x27, x8, #336           // =336
	mov	x0, x27
	bl	_raw_spin_trylock
	cbnz	w0, .LBB6_29
// %bb.28:                              //   in Loop: Header=BB6_25 Depth=2
	//APP
	.if 1 == 1
661:
	msr	daif, x26
nop
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 42
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
		.irp	num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
	.equ	.L__reg_num_x\num, \num
	.endr
	.equ	.L__reg_num_xzr, 31
	.macro	msr_s, sreg, rt
.inst (0xd5000000|(\sreg)|(.L__reg_num_\rt))
		.endm
	msr_s (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5)), x26
	.purgem	msr_s
dsb	sy
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	add	x25, x25, #1            // =1
	adrp	x0, .L.str.72
	add	x0, x0, :lo12:.L.str.72
	mov	w1, w25
	bl	printk
	b	.LBB6_34
.LBB6_29:                               //   in Loop: Header=BB6_25 Depth=2
	add	x9, x24, x25, lsl #4
	ldr	x8, [x9, #512]
	add	x10, x9, #512           // =512
	cmp	x8, x10
	b.ne	.LBB6_32
// %bb.30:                              //   in Loop: Header=BB6_25 Depth=2
	ldr	x9, [x9, #520]
	cmp	x9, x10
	b.ne	.LBB6_32
// %bb.31:                              //   in Loop: Header=BB6_25 Depth=2
	add	x25, x25, #1            // =1
	adrp	x0, .L.str.73
	add	x0, x0, :lo12:.L.str.73
	mov	w1, w25
	bl	printk
	b	.LBB6_33
.LBB6_32:                               //   in Loop: Header=BB6_25 Depth=2
	ldr	x2, [x8, #32]
	ldr	x3, [x8, #48]
	ldrb	w4, [x8, #57]
	add	x25, x25, #1            // =1
	adrp	x0, .L.str.74
	add	x0, x0, :lo12:.L.str.74
	mov	w1, w25
	bl	printk
.LBB6_33:                               //   in Loop: Header=BB6_25 Depth=2
	mov	x0, x27
	mov	x1, x26
	bl	_raw_spin_unlock_irqrestore
.LBB6_34:                               //   in Loop: Header=BB6_25 Depth=2
	mov	x27, x19
	mov	x19, x20
	mov	x20, x23
	adrp	x23, .L.str.64
	cmp	x25, #4                 // =4
	add	x23, x23, :lo12:.L.str.64
	b.eq	.LBB6_8
	b	.LBB6_25
.LBB6_35:
	cbz	x22, .LBB6_39
// %bb.36:
	mov	x0, x22
	bl	mas_blk_get_lld
	cbz	x0, .LBB6_39
// %bb.37:
	ldr	w8, [x0, #248]
	mov	x19, x0
	cbz	w8, .LBB6_39
// %bb.38:
	ldrb	w8, [x19, #25]
	tbnz	w8, #1, .LBB6_40
.LBB6_39:
	ldp	x20, x19, [sp, #96]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #48]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #112            // =112
	ret
.LBB6_40:
	adrp	x0, .L.str.77
	adrp	x1, .L__func__.mas_blk_dump_unistore_buf_bio
	add	x0, x0, :lo12:.L.str.77
	add	x1, x1, :lo12:.L__func__.mas_blk_dump_unistore_buf_bio
	bl	printk
	adrp	x21, .L.str.78
	adrp	x22, .L.str.79
	mov	w20, #1
	add	x21, x21, :lo12:.L.str.78
	add	x22, x22, :lo12:.L.str.79
.LBB6_41:                               // =>This Loop Header: Depth=1
                                        //     Child Loop BB6_44 Depth 2
	add	x8, x19, x20, lsl #2
	add	x23, x8, #768           // =768
	mov	x0, x23
	bl	_raw_spin_lock_irqsave
	add	x8, x19, x20, lsl #4
	ldr	x26, [x8, #688]
	add	x25, x8, #688           // =688
	mov	x24, x0
	cmp	x26, x25
	b.ne	.LBB6_44
// %bb.42:                              //   in Loop: Header=BB6_41 Depth=1
	ldr	x8, [x8, #696]
	cmp	x8, x25
	b.ne	.LBB6_45
// %bb.43:                              //   in Loop: Header=BB6_41 Depth=1
	mov	x0, x21
	mov	w1, w20
	bl	printk
	b	.LBB6_45
.LBB6_44:                               //   Parent Loop BB6_41 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	sub	x8, x26, #360           // =360
	ldr	x8, [x8]
	ldr	w9, [x19, #248]
	sub	x10, x26, #344          // =344
	ldr	w10, [x10]
	ldur	w2, [x26, #-28]
	lsr	x8, x8, #3
	udiv	x3, x8, x9
	msub	x4, x3, x9, x8
	lsr	w5, w10, #12
	mov	x0, x22
	mov	w1, w20
	bl	printk
	ldr	x26, [x26]
	cmp	x25, x26
	b.ne	.LBB6_44
.LBB6_45:                               //   in Loop: Header=BB6_41 Depth=1
	mov	x0, x23
	mov	x1, x24
	bl	_raw_spin_unlock_irqrestore
	add	x20, x20, #1            // =1
	cmp	x20, #5                 // =5
	b.ne	.LBB6_41
// %bb.46:
	adrp	x0, .L.str.80
	adrp	x1, .L__func__.mas_blk_dump_unistore_buf_bio
	add	x0, x0, :lo12:.L.str.80
	add	x1, x1, :lo12:.L__func__.mas_blk_dump_unistore_buf_bio
	bl	printk
	b	.LBB6_39
.Lfunc_end6:
	.size	mas_blk_panic_dump, .Lfunc_end6-mas_blk_panic_dump
                                        // -- End function
	.globl	mas_blk_panic_notify    // -- Begin function mas_blk_panic_notify
	.p2align	2
	.type	mas_blk_panic_notify,@function
mas_blk_panic_notify:                   // @mas_blk_panic_notify
.Lmas_blk_panic_notify$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	bl	ktime_get
	adrp	x19, .L__func__.mas_blk_panic_notify
	mov	x2, x0
	adrp	x0, .L.str.17
	add	x19, x19, :lo12:.L__func__.mas_blk_panic_notify
	add	x0, x0, :lo12:.L.str.17
	mov	x1, x19
	bl	printk
	bl	mas_blk_panic_dump
	adrp	x0, .L.str.18
	add	x0, x0, :lo12:.L.str.18
	mov	x1, x19
	bl	printk
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	mov	w0, wzr
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end7:
	.size	mas_blk_panic_notify, .Lfunc_end7-mas_blk_panic_notify
                                        // -- End function
	.globl	mas_blk_dump_unistore   // -- Begin function mas_blk_dump_unistore
	.p2align	2
	.type	mas_blk_dump_unistore,@function
mas_blk_dump_unistore:                  // @mas_blk_dump_unistore
.Lmas_blk_dump_unistore$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x1
	bl	blk_queue_query_unistore_enable
	tbnz	w0, #0, .LBB8_2
.LBB8_1:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB8_2:
	adrp	x0, .L.str.19
	adrp	x1, .L__func__.mas_blk_dump_unistore
	add	x0, x0, :lo12:.L.str.19
	add	x1, x1, :lo12:.L__func__.mas_blk_dump_unistore
	mov	x2, x19
	bl	printk
	bl	mas_blk_recovery_debug_off
	b	.LBB8_1
.Lfunc_end8:
	.size	mas_blk_dump_unistore, .Lfunc_end8-mas_blk_dump_unistore
                                        // -- End function
	.globl	mas_blk_dump_register_queue // -- Begin function mas_blk_dump_register_queue
	.p2align	2
	.type	mas_blk_dump_register_queue,@function
mas_blk_dump_register_queue:            // @mas_blk_dump_register_queue
.Lmas_blk_dump_register_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #24]
	tbz	w8, #0, .LBB9_3
// %bb.1:
	ldr	x8, [x19, #1288]
	add	x21, x19, #1288         // =1288
	cmp	x21, x8
	b.ne	.LBB9_3
// %bb.2:
	adrp	x20, dump_list_lock
	add	x20, x20, :lo12:dump_list_lock
	mov	x0, x20
	bl	_raw_spin_lock
	adrp	x8, dump_list
	add	x8, x8, :lo12:dump_list
	ldr	x9, [x8, #8]
	mov	x0, x20
	str	x21, [x8, #8]
	str	x8, [x19, #1288]
	str	x9, [x19, #1296]
	str	x21, [x9]
	bl	_raw_spin_unlock
.LBB9_3:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end9:
	.size	mas_blk_dump_register_queue, .Lfunc_end9-mas_blk_dump_register_queue
                                        // -- End function
	.globl	mas_blk_dump_unregister_queue // -- Begin function mas_blk_dump_unregister_queue
	.p2align	2
	.type	mas_blk_dump_unregister_queue,@function
mas_blk_dump_unregister_queue:          // @mas_blk_dump_unregister_queue
.Lmas_blk_dump_unregister_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x0
	adrp	x0, dump_list_lock
	add	x0, x0, :lo12:dump_list_lock
	mov	x29, sp
	bl	_raw_spin_lock
	adrp	x9, dump_list
	add	x9, x9, :lo12:dump_list
	mov	x8, x9
.LBB10_1:                               // =>This Inner Loop Header: Depth=1
	ldr	x8, [x8]
	cmp	x8, x9
	b.eq	.LBB10_4
// %bb.2:                               //   in Loop: Header=BB10_1 Depth=1
	sub	x10, x8, #1288          // =1288
	cmp	x10, x19
	b.ne	.LBB10_1
// %bb.3:
	ldp	x10, x9, [x8]
	mov	x11, #256
	movk	x11, #57005, lsl #48
	mov	w12, #34
	orr	x12, x11, x12
	str	x9, [x10, #8]
	str	x10, [x9]
	stp	x11, x12, [x8]
.LBB10_4:
	adrp	x0, dump_list_lock
	add	x0, x0, :lo12:dump_list_lock
	bl	_raw_spin_unlock
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end10:
	.size	mas_blk_dump_unregister_queue, .Lfunc_end10-mas_blk_dump_unregister_queue
                                        // -- End function
	.section	.init.text,"ax",@progbits
	.globl	mas_blk_dump_init       // -- Begin function mas_blk_dump_init
	.p2align	2
	.type	mas_blk_dump_init,@function
mas_blk_dump_init:                      // @mas_blk_dump_init
.Lmas_blk_dump_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	adrp	x0, panic_notifier_list
	adrp	x1, mas_blk_panic_nb
	add	x0, x0, :lo12:panic_notifier_list
	add	x1, x1, :lo12:mas_blk_panic_nb
	mov	x29, sp
	bl	atomic_notifier_chain_register
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end11:
	.size	mas_blk_dump_init, .Lfunc_end11-mas_blk_dump_init
                                        // -- End function
	.text
	.globl	blk_mq_tagset_dump_register // -- Begin function blk_mq_tagset_dump_register
	.p2align	2
	.type	blk_mq_tagset_dump_register,@function
blk_mq_tagset_dump_register:            // @blk_mq_tagset_dump_register
.Lblk_mq_tagset_dump_register$local:
// %bb.0:
	ldr	x8, [x0, #88]
	str	x1, [x0, #104]
	orr	x8, x8, #0x1
	str	x8, [x0, #88]
	ret
.Lfunc_end12:
	.size	blk_mq_tagset_dump_register, .Lfunc_end12-blk_mq_tagset_dump_register
                                        // -- End function
	.globl	blk_queue_dump_register // -- Begin function blk_queue_dump_register
	.p2align	2
	.type	blk_queue_dump_register,@function
blk_queue_dump_register:                // @blk_queue_dump_register
.Lblk_queue_dump_register$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x20, x1
	mov	x19, x0
	bl	mas_blk_get_lld
	ldr	x8, [x0, #24]
	str	x20, [x0, #40]
	orr	x8, x8, #0x1
	str	x8, [x0, #24]
	mov	x0, x19
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #24]
	tbz	w8, #0, .LBB13_3
// %bb.1:
	ldr	x8, [x19, #1288]
	add	x21, x19, #1288         // =1288
	cmp	x21, x8
	b.ne	.LBB13_3
// %bb.2:
	adrp	x20, dump_list_lock
	add	x20, x20, :lo12:dump_list_lock
	mov	x0, x20
	bl	_raw_spin_lock
	adrp	x8, dump_list
	add	x8, x8, :lo12:dump_list
	ldr	x9, [x8, #8]
	mov	x0, x20
	str	x21, [x8, #8]
	str	x8, [x19, #1288]
	str	x9, [x19, #1296]
	str	x21, [x9]
	bl	_raw_spin_unlock
.LBB13_3:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end13:
	.size	blk_queue_dump_register, .Lfunc_end13-blk_queue_dump_register
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"rq: 0x%pK, type:%s len:%u %s requeue_reason: %d tag: %d "
	.size	.L.str, 57

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"requeue_cnt%d %d tag: %d "
	.size	.L.str.1, 26

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"hold_cnt: %u inflt_flag %x "
	.size	.L.str.2, 28

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"rq_hoq_flag: %llx rq_cp_flag: %llx "
	.size	.L.str.3, 36

	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"disp_pid: %d %s, task: 0x%pK\n"
	.size	.L.str.4, 30

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"\n"
	.size	.L.str.5, 2

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"%s: %lld "
	.size	.L.str.6, 10

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"\0013[BLK-IO]%s: %s\n"
	.size	.L.str.7, 18

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"bio: 0x%pK, type:%s len:%u "
	.size	.L.str.8, 28

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"disp_pid: %d %s"
	.size	.L.str.9, 16

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	", task: 0x%pK"
	.size	.L.str.10, 14

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"<%s:%lld> "
	.size	.L.str.11, 11

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"\0013[BLK-IO]%s %s,current: %lld\n"
	.size	.L.str.12, 31

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"\0013[BLK-IO]%s: bdev %s, ptable %s, r_inflt: %d w_inflt: %d"
	.size	.L.str.13, 58

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"exist"
	.size	.L.str.14, 6

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"none"
	.size	.L.str.15, 5

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"\0013[BLK-IO]q: 0x%pK"
	.size	.L.str.16, 19

	.type	dump_list,@object       // @dump_list
	.data
	.p2align	3
dump_list:
	.xword	dump_list
	.xword	dump_list
	.size	dump_list, 16

	.type	.L.str.17,@object       // @.str.17
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.17:
	.asciz	"\0013[BLK-IO]%s: ++ current = %lld\n"
	.size	.L.str.17, 33

	.type	.L__func__.mas_blk_panic_notify,@object // @__func__.mas_blk_panic_notify
.L__func__.mas_blk_panic_notify:
	.asciz	"mas_blk_panic_notify"
	.size	.L__func__.mas_blk_panic_notify, 21

	.type	.L.str.18,@object       // @.str.18
.L.str.18:
	.asciz	"\0013[BLK-IO]%s: --\n"
	.size	.L.str.18, 18

	.type	.L.str.19,@object       // @.str.19
.L.str.19:
	.asciz	"\0013[BLK-IO]%s - %s\n"
	.size	.L.str.19, 19

	.type	.L__func__.mas_blk_dump_unistore,@object // @__func__.mas_blk_dump_unistore
.L__func__.mas_blk_dump_unistore:
	.asciz	"mas_blk_dump_unistore"
	.size	.L__func__.mas_blk_dump_unistore, 22

	.type	dump_list_lock,@object  // @dump_list_lock
	.local	dump_list_lock
	.comm	dump_list_lock,4,4
	.type	mas_blk_panic_nb,@object // @mas_blk_panic_nb
	.data
	.p2align	3
mas_blk_panic_nb:
	.xword	__cfi_mas_blk_panic_notify
	.xword	0
	.word	0                       // 0x0
	.zero	4
	.size	mas_blk_panic_nb, 24

	.type	.L.str.20,@object       // @.str.20
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.20:
	.asciz	"dump"
	.size	.L.str.20, 5

	.type	.L.str.21,@object       // @.str.21
.L.str.21:
	.asciz	"io_latency"
	.size	.L.str.21, 11

	.type	.L.str.22,@object       // @.str.22
.L.str.22:
	.asciz	"TYPE_BLOCK_PC"
	.size	.L.str.22, 14

	.type	.L.str.23,@object       // @.str.23
.L.str.23:
	.asciz	"TYPE_DRV_PRIV"
	.size	.L.str.23, 14

	.type	.L.str.24,@object       // @.str.24
.L.str.24:
	.asciz	"TYPE_FS"
	.size	.L.str.24, 8

	.type	.L.str.25,@object       // @.str.25
.L.str.25:
	.asciz	"make_nr:%u, order:%u, pu:0x%llx, offset:0x%llx, lba:0x%llx - 0x%llx. stream type: %d\n"
	.size	.L.str.25, 86

	.type	.L.str.26,@object       // @.str.26
.L.str.26:
	.asciz	"\0014[BLK-IO]%s: Unknown OP!\n"
	.size	.L.str.26, 27

	.type	.L__func__.mas_blk_io_type_op,@object // @__func__.mas_blk_io_type_op
.L__func__.mas_blk_io_type_op:
	.asciz	"mas_blk_io_type_op"
	.size	.L__func__.mas_blk_io_type_op, 19

	.type	.L.str.27,@object       // @.str.27
.L.str.27:
	.asciz	"\nlba:0x%llx - 0x%llx. stream type: %d"
	.size	.L.str.27, 38

	.type	.L.str.28,@object       // @.str.28
.L.str.28:
	.asciz	"Queue Status:\n"
	.size	.L.str.28, 15

	.type	.L.str.29,@object       // @.str.29
.L.str.29:
	.asciz	"%s"
	.size	.L.str.29, 3

	.type	.L.str.30,@object       // @.str.30
.L.str.30:
	.asciz	"\0013[BLK-IO]%s"
	.size	.L.str.30, 13

	.type	.L.str.31,@object       // @.str.31
.L.str.31:
	.asciz	"\tIO Latency Warning: enable\tThreshold(ms): %u\n"
	.size	.L.str.31, 47

	.type	.L.str.32,@object       // @.str.32
.L.str.32:
	.asciz	"\tIO Latency Warning: disable\n"
	.size	.L.str.32, 30

	.type	.L.str.33,@object       // @.str.33
.L.str.33:
	.asciz	"\tFlush Reduce: %s\n"
	.size	.L.str.33, 19

	.type	.L.str.34,@object       // @.str.34
.L.str.34:
	.asciz	"enable"
	.size	.L.str.34, 7

	.type	.L.str.35,@object       // @.str.35
.L.str.35:
	.asciz	"disable"
	.size	.L.str.35, 8

	.type	.L.str.36,@object       // @.str.36
.L.str.36:
	.asciz	"LLD Status:\n"
	.size	.L.str.36, 13

	.type	.L.str.37,@object       // @.str.37
.L.str.37:
	.asciz	"\tPanic Dump: %s\n"
	.size	.L.str.37, 17

	.type	.L.str.38,@object       // @.str.38
.L.str.38:
	.asciz	"\tLatency Warning:enable\tThreshold(ms):%u\n"
	.size	.L.str.38, 42

	.type	.L.str.39,@object       // @.str.39
.L.str.39:
	.asciz	"\tLatency Warning: disable\n"
	.size	.L.str.39, 27

	.type	.L.str.40,@object       // @.str.40
.L.str.40:
	.asciz	"\tBusy/Idle Notifier: %s\t"
	.size	.L.str.40, 25

	.type	.L.str.41,@object       // @.str.41
.L.str.41:
	.asciz	"\tState: BUSY\t"
	.size	.L.str.41, 14

	.type	.L.str.42,@object       // @.str.42
.L.str.42:
	.asciz	"\tState: IDLE\t"
	.size	.L.str.42, 14

	.type	.L.str.43,@object       // @.str.43
.L.str.43:
	.asciz	"\tIO Count: %d\n"
	.size	.L.str.43, 15

	.type	.L.str.44,@object       // @.str.44
.L.str.44:
	.asciz	"\tFGIO/Busy/Idle Notifier: %s\t"
	.size	.L.str.44, 30

	.type	.L.str.45,@object       // @.str.45
.L.str.45:
	.asciz	"\tFG State: BUSY\t"
	.size	.L.str.45, 17

	.type	.L.str.46,@object       // @.str.46
.L.str.46:
	.asciz	"\tFG State: IDLE\t"
	.size	.L.str.46, 17

	.type	.L.str.47,@object       // @.str.47
.L.str.47:
	.asciz	"\tFG IO Count: %d\n"
	.size	.L.str.47, 18

	.type	.L.str.48,@object       // @.str.48
.L.str.48:
	.asciz	"\tIO Scheduler: "
	.size	.L.str.48, 16

	.type	.L.str.49,@object       // @.str.49
.L.str.49:
	.asciz	"MAS MQ\n"
	.size	.L.str.49, 8

	.type	.L.str.50,@object       // @.str.50
.L.str.50:
	.asciz	"Default Scheduler\n"
	.size	.L.str.50, 19

	.type	.L.str.51,@object       // @.str.51
.L.str.51:
	.asciz	"\tInline Crypto: %s\n"
	.size	.L.str.51, 20

	.type	.L.str.52,@object       // @.str.52
.L.str.52:
	.asciz	"\tUFS command priority: %s\n"
	.size	.L.str.52, 27

	.type	.L.str.53,@object       // @.str.53
.L.str.53:
	.asciz	"\tUFS order preserving: %s\n"
	.size	.L.str.53, 27

	.type	.L.str.54,@object       // @.str.54
.L.str.54:
	.asciz	"\0013[BLK-IO]%s, section size error: %u\n"
	.size	.L.str.54, 38

	.type	.L__func__.mas_blk_lld_dump_opt_feature_unist,@object // @__func__.mas_blk_lld_dump_opt_feature_unist
.L__func__.mas_blk_lld_dump_opt_feature_unist:
	.asciz	"mas_blk_lld_dump_opt_feature_unist"
	.size	.L__func__.mas_blk_lld_dump_opt_feature_unist, 35

	.type	.L.str.55,@object       // @.str.55
.L.str.55:
	.asciz	"Write Order: %s Order Num: %uLAST STREAM TYPE: %u, fsync ind: %s\n"
	.size	.L.str.55, 66

	.type	.L.str.56,@object       // @.str.56
.L.str.56:
	.asciz	"true"
	.size	.L.str.56, 5

	.type	.L.str.57,@object       // @.str.57
.L.str.57:
	.asciz	"false"
	.size	.L.str.57, 6

	.type	.L.str.58,@object       // @.str.58
.L.str.58:
	.asciz	"expected_lba: 0x%llx - 0x%llx, 0x%llx - 0x%llx, 0x%llx - 0x%llx, 0x%llx - 0x%llx\n"
	.size	.L.str.58, 82

	.type	.L.str.59,@object       // @.str.59
.L.str.59:
	.asciz	"expected_pu: 0x%llx, current_pu_size: 0x%llx\n"
	.size	.L.str.59, 46

	.type	.L.str.60,@object       // @.str.60
.L.str.60:
	.asciz	"expected_lba_time: %llu, %llu, %llu, %llu\n"
	.size	.L.str.60, 43

	.type	.L.str.61,@object       // @.str.61
.L.str.61:
	.asciz	"old_section: 0x%llx, 0x%llx, 0x%llx, 0x%llx\n"
	.size	.L.str.61, 45

	.type	.L.str.62,@object       // @.str.62
.L.str.62:
	.asciz	"\0013[BLK-IO]bio_count = %llu req_count = %llu\n"
	.size	.L.str.62, 45

	.type	.L.str.63,@object       // @.str.63
.L.str.63:
	.asciz	"\0013[BLK-IO]expected_lba: 0x%llx - 0x%llx, 0x%llx - 0x%llx, 0x%llx - 0x%llx, 0x%llx - 0x%llx\n"
	.size	.L.str.63, 92

	.type	.L.str.64,@object       // @.str.64
.L.str.64:
	.asciz	"\0013[BLK-IO]expected_pu: 0x%llx (0x%llx - 0x%llx)"
	.size	.L.str.64, 48

	.type	.L.str.65,@object       // @.str.65
.L.str.65:
	.asciz	"\0013[BLK-IO]current_pu_size: 0x%llx"
	.size	.L.str.65, 34

	.type	.L.str.66,@object       // @.str.66
.L.str.66:
	.asciz	"\0013[BLK-IO]old_section: 0x%llx, 0x%llx, 0x%llx, 0x%llx\n"
	.size	.L.str.66, 55

	.type	.L.str.67,@object       // @.str.67
.L.str.67:
	.asciz	"\0013[BLK-IO]reset_count: %d\n"
	.size	.L.str.67, 27

	.type	.L.str.68,@object       // @.str.68
.L.str.68:
	.asciz	"\0013[BLK-IO]recovery_flag: %d\n"
	.size	.L.str.68, 29

	.type	.L.str.69,@object       // @.str.69
.L.str.69:
	.asciz	"\0013[BLK-IO]recovery_pwron_flag: %d\n"
	.size	.L.str.69, 35

	.type	.L.str.70,@object       // @.str.70
.L.str.70:
	.asciz	"\0013[BLK-IO]recovery_pwron_inprocess_flag: %d\n"
	.size	.L.str.70, 45

	.type	.L.str.71,@object       // @.str.71
.L.str.71:
	.asciz	"\0013[BLK-IO]max_recovery_size: 0x%llx\n"
	.size	.L.str.71, 37

	.type	.L.str.72,@object       // @.str.72
.L.str.72:
	.asciz	"\0013[BLK-IO]stream %u, trylock fail\n"
	.size	.L.str.72, 35

	.type	.L.str.73,@object       // @.str.73
.L.str.73:
	.asciz	"\0013[BLK-IO]stream %u, section list is empty\n"
	.size	.L.str.73, 44

	.type	.L.str.74,@object       // @.str.74
.L.str.74:
	.asciz	"\0013[BLK-IO]stream %u, first 0x%llx, next 0x%llx, rcv flag %d\n"
	.size	.L.str.74, 61

	.type	.L.str.75,@object       // @.str.75
.L.str.75:
	.asciz	"\0013[BLK-IO]bio_list:\n"
	.size	.L.str.75, 21

	.type	.L.str.76,@object       // @.str.76
.L.str.76:
	.asciz	"\0013[BLK-IO]req_list:\n"
	.size	.L.str.76, 21

	.type	.L.str.77,@object       // @.str.77
.L.str.77:
	.asciz	"\0013[BLK-IO]%s, dump buf bio start\n"
	.size	.L.str.77, 34

	.type	.L__func__.mas_blk_dump_unistore_buf_bio,@object // @__func__.mas_blk_dump_unistore_buf_bio
.L__func__.mas_blk_dump_unistore_buf_bio:
	.asciz	"mas_blk_dump_unistore_buf_bio"
	.size	.L__func__.mas_blk_dump_unistore_buf_bio, 30

	.type	.L.str.78,@object       // @.str.78
.L.str.78:
	.asciz	"\0013[BLK-IO]recovery_dump, stream: %u, buffer list is empty\n"
	.size	.L.str.78, 59

	.type	.L.str.79,@object       // @.str.79
.L.str.79:
	.asciz	"\0013[BLK-IO]recovery_dump, stream: %u, bio_nr: %u, bio_lba: 0x%llx - 0x%llx, len: %u\n"
	.size	.L.str.79, 84

	.type	.L.str.80,@object       // @.str.80
.L.str.80:
	.asciz	"\0013[BLK-IO]%s, dump buf bio end\n"
	.size	.L.str.80, 32

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
