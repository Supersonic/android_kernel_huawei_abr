	.text
	.file	"mas_blk_iosched_ufs_mq.c"
	.globl	ufs_mq_inc_vip_wait_cnt // -- Begin function ufs_mq_inc_vip_wait_cnt
	.p2align	2
	.type	ufs_mq_inc_vip_wait_cnt,@function
ufs_mq_inc_vip_wait_cnt:                // @ufs_mq_inc_vip_wait_cnt
.Lufs_mq_inc_vip_wait_cnt$local:
// %bb.0:
	ldr	x8, [x0]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB0_3
// %bb.1:
	ldr	x8, [x8, #536]
	cbz	x8, .LBB0_3
// %bb.2:
	add	x8, x8, #28             // =28
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB0_3:
	ret
.Lfunc_end0:
	.size	ufs_mq_inc_vip_wait_cnt, .Lfunc_end0-ufs_mq_inc_vip_wait_cnt
                                        // -- End function
	.globl	ufs_mq_dec_vip_wait_cnt // -- Begin function ufs_mq_dec_vip_wait_cnt
	.p2align	2
	.type	ufs_mq_dec_vip_wait_cnt,@function
ufs_mq_dec_vip_wait_cnt:                // @ufs_mq_dec_vip_wait_cnt
.Lufs_mq_dec_vip_wait_cnt$local:
// %bb.0:
	ldr	x8, [x0]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB1_2
// %bb.1:
	ldr	x8, [x8, #536]
.LBB1_2:
	ldr	w9, [x8, #28]
.LBB1_3:                                // =>This Inner Loop Header: Depth=1
	subs	w10, w9, #1             // =1
	b.mi	.LBB1_5
// %bb.4:                               //   in Loop: Header=BB1_3 Depth=1
	mov	w11, w9
	add	x12, x8, #28            // =28
	//APP
		prfm	pstl1strm, [x12]
1:	ldxr	w13, [x12]
	eor	w14, w13, w11
	cbnz	w14, 2f
	stlxr	w14, w10, [x12]
	cbnz	w14, 1b
	dmb ish
2:
	//NO_APP
	cmp	w13, w9
	mov	w9, w13
	b.ne	.LBB1_3
.LBB1_5:
	ret
.Lfunc_end1:
	.size	ufs_mq_dec_vip_wait_cnt, .Lfunc_end1-ufs_mq_dec_vip_wait_cnt
                                        // -- End function
	.globl	reset_vip_wait_cnt      // -- Begin function reset_vip_wait_cnt
	.p2align	2
	.type	reset_vip_wait_cnt,@function
reset_vip_wait_cnt:                     // @reset_vip_wait_cnt
.Lreset_vip_wait_cnt$local:
// %bb.0:
	ldr	x8, [x0]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB2_2
// %bb.1:
	ldr	x8, [x8, #536]
.LBB2_2:
	ldr	w9, [x8, #24]
	cbz	w9, .LBB2_4
// %bb.3:
	ret
.LBB2_4:
	str	wzr, [x8, #28]
	ret
.Lfunc_end2:
	.size	reset_vip_wait_cnt, .Lfunc_end2-reset_vip_wait_cnt
                                        // -- End function
	.globl	ufs_mq_vip_tag_wait_cnt // -- Begin function ufs_mq_vip_tag_wait_cnt
	.p2align	2
	.type	ufs_mq_vip_tag_wait_cnt,@function
ufs_mq_vip_tag_wait_cnt:                // @ufs_mq_vip_tag_wait_cnt
.Lufs_mq_vip_tag_wait_cnt$local:
// %bb.0:
	ldr	x8, [x0]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB3_2
// %bb.1:
	ldr	x8, [x8, #536]
.LBB3_2:
	ldr	w0, [x8, #28]
	ret
.Lfunc_end3:
	.size	ufs_mq_vip_tag_wait_cnt, .Lfunc_end3-ufs_mq_vip_tag_wait_cnt
                                        // -- End function
	.globl	mas_blk_latency_req_check_ufs // -- Begin function mas_blk_latency_req_check_ufs
	.p2align	2
	.type	mas_blk_latency_req_check_ufs,@function
mas_blk_latency_req_check_ufs:          // @mas_blk_latency_req_check_ufs
.Lmas_blk_latency_req_check_ufs$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	mov	w20, w1
	mov	x19, x0
	bl	mas_blk_latency_req_check
	cmp	w20, #23                // =23
	b.ne	.LBB4_2
// %bb.1:
	ldr	x8, [x19, #800]
	add	x8, x8, #1              // =1
	str	x8, [x19, #800]
.LBB4_2:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end4:
	.size	mas_blk_latency_req_check_ufs, .Lfunc_end4-mas_blk_latency_req_check_ufs
                                        // -- End function
	.globl	mas_blk_queue_get_budget // -- Begin function mas_blk_queue_get_budget
	.p2align	2
	.type	mas_blk_queue_get_budget,@function
mas_blk_queue_get_budget:               // @mas_blk_queue_get_budget
.Lmas_blk_queue_get_budget$local:
// %bb.0:
	add	x8, x0, #1248           // =1248
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	ret
.Lfunc_end5:
	.size	mas_blk_queue_get_budget, .Lfunc_end5-mas_blk_queue_get_budget
                                        // -- End function
	.globl	mas_blk_queue_put_budget // -- Begin function mas_blk_queue_put_budget
	.p2align	2
	.type	mas_blk_queue_put_budget,@function
mas_blk_queue_put_budget:               // @mas_blk_queue_put_budget
.Lmas_blk_queue_put_budget$local:
// %bb.0:
	add	x8, x0, #1252           // =1252
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	ret
.Lfunc_end6:
	.size	mas_blk_queue_put_budget, .Lfunc_end6-mas_blk_queue_put_budget
                                        // -- End function
	.globl	get_mq_all_tag_used     // -- Begin function get_mq_all_tag_used
	.p2align	2
	.type	get_mq_all_tag_used,@function
get_mq_all_tag_used:                    // @get_mq_all_tag_used
.Lget_mq_all_tag_used$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB7_4
// %bb.1:
	bl	mas_blk_get_lld
	cbz	x0, .LBB7_4
// %bb.2:
	ldr	x8, [x0, #1944]
	cbz	x8, .LBB7_5
// %bb.3:
	ldr	w9, [x8, #24]
	ldr	w10, [x8, #16]
	ldr	w8, [x8, #20]
	add	w9, w10, w9
	add	w0, w9, w8
.LBB7_4:
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB7_5:
	mov	w0, wzr
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end7:
	.size	get_mq_all_tag_used, .Lfunc_end7-get_mq_all_tag_used
                                        // -- End function
	.globl	get_mq_prio_tag_used    // -- Begin function get_mq_prio_tag_used
	.p2align	2
	.type	get_mq_prio_tag_used,@function
get_mq_prio_tag_used:                   // @get_mq_prio_tag_used
.Lget_mq_prio_tag_used$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB8_4
// %bb.1:
	bl	mas_blk_get_lld
	cbz	x0, .LBB8_4
// %bb.2:
	ldr	x8, [x0, #1944]
	cbz	x8, .LBB8_5
// %bb.3:
	ldr	w0, [x8, #24]
.LBB8_4:
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB8_5:
	mov	w0, wzr
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end8:
	.size	get_mq_prio_tag_used, .Lfunc_end8-get_mq_prio_tag_used
                                        // -- End function
	.globl	mas_blk_is_reserved_empty // -- Begin function mas_blk_is_reserved_empty
	.p2align	2
	.type	mas_blk_is_reserved_empty,@function
mas_blk_is_reserved_empty:              // @mas_blk_is_reserved_empty
.Lmas_blk_is_reserved_empty$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x0
	ldr	x0, [x0]
	mov	x29, sp
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB9_2
// %bb.1:
	ldrb	w8, [x19, #13]
	tst	w8, #0x3
	b.eq	.LBB9_4
.LBB9_2:
	mov	w0, wzr
.LBB9_3:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB9_4:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB9_6
// %bb.5:
	ldr	x8, [x8, #536]
.LBB9_6:
	ldr	w8, [x8, #20]
	ldr	x9, [x19, #48]
	ldr	x9, [x9, #360]
	ldr	w9, [x9, #4]
	sub	w9, w9, #4              // =4
	cmp	w8, w9
	cset	w0, hs
	b	.LBB9_3
.Lfunc_end9:
	.size	mas_blk_is_reserved_empty, .Lfunc_end9-mas_blk_is_reserved_empty
                                        // -- End function
	.globl	ufs_mq_tag_get          // -- Begin function ufs_mq_tag_get
	.p2align	2
	.type	ufs_mq_tag_get,@function
ufs_mq_tag_get:                         // @ufs_mq_tag_get
.Lufs_mq_tag_get$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x0
	ldr	w21, [x0, #36]
	ldr	x0, [x0]
	mov	w20, #4096
	mov	x29, sp
	movk	w20, #64, lsl #16
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB10_11
// %bb.1:
	ldr	w8, [x19, #12]
	tbnz	w8, #8, .LBB10_13
// %bb.2:
	tbnz	w8, #9, .LBB10_15
// %bb.3:
	ldr	w9, [x19, #36]
	tst	w9, w20
	b.ne	.LBB10_36
// %bb.4:
	tbnz	w8, #7, .LBB10_36
// %bb.5:
	add	w8, w20, #2048          // =2048
	and	w8, w9, w8
	cmp	w8, #2048               // =2048
	b.eq	.LBB10_12
// %bb.6:
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #4]
	cbz	w9, .LBB10_72
// %bb.7:
	add	x1, x8, #104            // =104
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_55
// %bb.8:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_10
// %bb.9:
	ldr	x8, [x8, #536]
.LBB10_10:
	add	x8, x8, #20             // =20
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	b	.LBB10_34
.LBB10_11:
	add	w8, w20, #2048          // =2048
	and	w8, w21, w8
	cmp	w8, #2048               // =2048
	b.ne	.LBB10_58
.LBB10_12:
	mov	x0, x19
	bl	ufs_tagset_get_tag
	b	.LBB10_55
.LBB10_13:
	ldr	x0, [x19]
	ldr	x8, [x0, #1392]
	cbz	x8, .LBB10_25
// %bb.14:
	ldr	x20, [x8, #536]
	b	.LBB10_26
.LBB10_15:
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #4]
	cbz	w9, .LBB10_72
// %bb.16:
	add	x1, x8, #104            // =104
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_69
// %bb.17:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_19
// %bb.18:
	ldr	x8, [x8, #536]
.LBB10_19:
	add	x8, x8, #20             // =20
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #16]
	add	w0, w9, w0
	cmn	w0, #1                  // =1
	b.ne	.LBB10_55
// %bb.20:
	ldr	w9, [x8, #8]
	cbz	w9, .LBB10_71
.LBB10_21:
	add	x1, x8, #168            // =168
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_54
// %bb.22:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_24
// %bb.23:
	ldr	x8, [x8, #536]
.LBB10_24:
	add	x8, x8, #24             // =24
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	b	.LBB10_53
.LBB10_25:
	mov	x20, xzr
.LBB10_26:
	ldr	x1, [x19, #16]
	bl	mas_blk_is_section_ready
	tbz	w0, #0, .LBB10_35
// %bb.27:
	ldrb	w8, [x19, #12]
	tbnz	w8, #7, .LBB10_43
// %bb.28:
	ldr	w10, [x20, #20]
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #4]
	sub	w11, w9, #1             // =1
	cmp	w10, w11
	b.hs	.LBB10_56
// %bb.29:
	cbz	w9, .LBB10_72
// %bb.30:
	add	x1, x8, #104            // =104
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_55
// %bb.31:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_33
// %bb.32:
	ldr	x8, [x8, #536]
.LBB10_33:
	add	x8, x8, #20             // =20
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB10_34:
	ldr	x8, [x19, #48]
	ldr	x8, [x8, #360]
	ldr	w8, [x8, #16]
	b	.LBB10_42
.LBB10_35:
	mov	w0, #-1
	b	.LBB10_55
.LBB10_36:
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #8]
	cbz	w9, .LBB10_71
// %bb.37:
	add	x1, x8, #168            // =168
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_55
// %bb.38:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_40
// %bb.39:
	ldr	x8, [x8, #536]
.LBB10_40:
	add	x8, x8, #24             // =24
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB10_41:
	ldr	x8, [x19, #48]
	ldr	x8, [x8, #360]
	ldr	w8, [x8, #20]
.LBB10_42:
	add	w0, w8, w0
	b	.LBB10_55
.LBB10_43:
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #4]
	cbz	w9, .LBB10_72
// %bb.44:
	add	x1, x8, #104            // =104
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_70
// %bb.45:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_47
// %bb.46:
	ldr	x8, [x8, #536]
.LBB10_47:
	add	x8, x8, #20             // =20
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #16]
	add	w0, w9, w0
	cmn	w0, #1                  // =1
	b.ne	.LBB10_55
// %bb.48:
	ldr	w9, [x8, #8]
	cbz	w9, .LBB10_71
.LBB10_49:
	add	x1, x8, #168            // =168
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_54
// %bb.50:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_52
// %bb.51:
	ldr	x8, [x8, #536]
.LBB10_52:
	add	x8, x8, #24             // =24
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB10_53:
	ldr	x8, [x19, #48]
	ldr	x8, [x8, #360]
	ldr	w8, [x8, #20]
	add	w0, w8, w0
	cmn	w0, #1                  // =1
	b.ne	.LBB10_55
.LBB10_54:
	mov	x0, x19
	bl	ufs_tagset_get_tag
	cmn	w0, #1                  // =1
	b.eq	.LBB10_57
.LBB10_55:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB10_56:
	ldr	w2, [x20, #20]
	adrp	x0, .L.str.16
	adrp	x1, .L__func__.ufs_mq_get_recovery_tag
	add	x0, x0, :lo12:.L.str.16
	add	x1, x1, :lo12:.L__func__.ufs_mq_get_recovery_tag
	bl	printk
	mov	w0, #-1
	b	.LBB10_55
.LBB10_57:
	adrp	x0, .L.str.17
	adrp	x1, .L__func__.ufs_mq_get_all_tag
	add	x0, x0, :lo12:.L.str.17
	add	x1, x1, :lo12:.L__func__.ufs_mq_get_all_tag
	bl	printk
	mov	w0, #-1
	b	.LBB10_55
.LBB10_58:
	ldr	x2, [x19, #48]
	tst	w21, w20
	ldr	x8, [x2, #360]
	b.eq	.LBB10_64
// %bb.59:
	ldr	w9, [x8, #8]
	cbz	w9, .LBB10_71
// %bb.60:
	add	x1, x8, #168            // =168
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_55
// %bb.61:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_63
// %bb.62:
	ldr	x8, [x8, #536]
.LBB10_63:
	add	x8, x8, #24             // =24
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	b	.LBB10_41
.LBB10_64:
	ldr	w9, [x8, #4]
	cbz	w9, .LBB10_72
// %bb.65:
	add	x1, x8, #104            // =104
	mov	x0, x19
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB10_55
// %bb.66:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB10_68
// %bb.67:
	ldr	x8, [x8, #536]
.LBB10_68:
	add	x8, x8, #20             // =20
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	b	.LBB10_34
.LBB10_69:
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #8]
	cbnz	w9, .LBB10_21
	b	.LBB10_71
.LBB10_70:
	ldr	x2, [x19, #48]
	ldr	x8, [x2, #360]
	ldr	w9, [x8, #8]
	cbnz	w9, .LBB10_49
.LBB10_71:
	adrp	x0, .L.str.18
	add	x0, x0, :lo12:.L.str.18
	bl	mas_blk_rdr_panic
.LBB10_72:
	adrp	x0, .L.str.19
	add	x0, x0, :lo12:.L.str.19
	bl	mas_blk_rdr_panic
.Lfunc_end10:
	.size	ufs_mq_tag_get, .Lfunc_end10-ufs_mq_tag_get
                                        // -- End function
	.p2align	2               // -- Begin function ufs_tagset_get_tag
	.type	ufs_tagset_get_tag,@function
ufs_tagset_get_tag:                     // @ufs_tagset_get_tag
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x19, [sp, #32]          // 8-byte Folded Spill
	mov	x19, x0
	str	x8, [sp, #8]
	ldr	x0, [x0]
	add	x29, sp, #16            // =16
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB11_8
// %bb.1:
	ldr	x8, [x19, #16]
	cbz	x8, .LBB11_8
// %bb.2:
	ldrb	w8, [x8, #16]
	cmp	w8, #1                  // =1
	b.ne	.LBB11_8
// %bb.3:
	add	x1, sp, #4              // =4
	mov	x0, x19
	strb	wzr, [sp, #4]
	bl	ufs_tagset_bt_get_unistore
	cmn	w0, #1                  // =1
	b.eq	.LBB11_12
// %bb.4:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB11_14
// %bb.5:
	ldr	x8, [x8, #536]
	cbz	x8, .LBB11_14
// %bb.6:
	ldrb	w9, [sp, #4]
	cbz	w9, .LBB11_15
// %bb.7:
	add	x8, x8, #24             // =24
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	ldr	x8, [x19, #48]
	ldr	x8, [x8, #360]
	ldr	w8, [x8, #20]
	add	w0, w8, w0
	b	.LBB11_12
.LBB11_8:
	ldr	x2, [x19, #48]
	mov	x0, x19
	ldr	x8, [x2, #360]
	add	x1, x8, #40             // =40
	bl	ufs_tagset_bt_get
	cmn	w0, #1                  // =1
	b.eq	.LBB11_12
// %bb.9:
	ldr	x8, [x19]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB11_11
// %bb.10:
	ldr	x8, [x8, #536]
.LBB11_11:
	add	x8, x8, #16             // =16
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB11_12:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB11_16
// %bb.13:
	ldr	x19, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB11_14:
	mov	w0, #-1
	b	.LBB11_12
.LBB11_15:
	add	x8, x8, #16             // =16
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	b	.LBB11_12
.LBB11_16:
	bl	__stack_chk_fail
.Lfunc_end11:
	.size	ufs_tagset_get_tag, .Lfunc_end11-ufs_tagset_get_tag
                                        // -- End function
	.globl	ufs_mq_tag_put          // -- Begin function ufs_mq_tag_put
	.p2align	2
	.type	ufs_mq_tag_put,@function
ufs_mq_tag_put:                         // @ufs_mq_tag_put
.Lufs_mq_tag_put$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	ldr	x8, [x0, #360]
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x0
	mov	x29, sp
	ldr	w9, [x8, #20]
	cmp	w9, w1
	b.ls	.LBB12_6
// %bb.1:
	ldr	w9, [x8, #16]
	cmp	w9, w1
	b.ls	.LBB12_9
// %bb.2:
	ldr	x9, [x2, #272]
	add	x0, x8, #40             // =40
	ldr	w2, [x9, #64]
	bl	sbitmap_queue_clear
	ldr	x8, [x19, #232]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB12_4
// %bb.3:
	ldr	x8, [x8, #536]
.LBB12_4:
	add	x8, x8, #16             // =16
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
.LBB12_5:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	mov	w0, wzr
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB12_6:
	ldr	x10, [x2, #272]
	add	x0, x8, #168            // =168
	sub	w1, w1, w9
	ldr	w2, [x10, #64]
	bl	sbitmap_queue_clear
	ldr	x8, [x19, #232]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB12_8
// %bb.7:
	ldr	x8, [x8, #536]
.LBB12_8:
	add	x8, x8, #24             // =24
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
	b	.LBB12_5
.LBB12_9:
	ldr	x10, [x2, #272]
	add	x0, x8, #104            // =104
	sub	w1, w1, w9
	ldr	w2, [x10, #64]
	bl	sbitmap_queue_clear
	ldr	x8, [x19, #232]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB12_11
// %bb.10:
	ldr	x8, [x8, #536]
.LBB12_11:
	add	x8, x8, #20             // =20
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
	b	.LBB12_5
.Lfunc_end12:
	.size	ufs_mq_tag_put, .Lfunc_end12-ufs_mq_tag_put
                                        // -- End function
	.globl	ufs_mq_sync_burst_check_timer_expire // -- Begin function ufs_mq_sync_burst_check_timer_expire
	.p2align	2
	.type	ufs_mq_sync_burst_check_timer_expire,@function
ufs_mq_sync_burst_check_timer_expire:   // @ufs_mq_sync_burst_check_timer_expire
.Lufs_mq_sync_burst_check_timer_expire$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	ktime_get
	ldr	w8, [x19, #264]
	sub	w8, w8, #1              // =1
	cmp	w8, #1                  // =1
	b.hi	.LBB13_3
// %bb.1:
	ldr	x8, [x19, #176]
	mov	w9, #25856
	movk	w9, #7629, lsl #16
	add	x8, x8, x9
	cmp	x8, x0
	b.ge	.LBB13_3
// %bb.2:
	mov	w8, #3
	str	w8, [x19, #264]
.LBB13_3:
	ldr	x8, [x19, #168]
	mov	w20, #19264
	movk	w20, #76, lsl #16
	add	x8, x8, x20
	cmp	x8, x0
	b.le	.LBB13_7
// %bb.4:
	ldr	w8, [x19, #264]
	cmp	w8, #3                  // =3
	b.eq	.LBB13_12
// %bb.5:
	cmp	w8, #1                  // =1
	b.ne	.LBB13_16
// %bb.6:
	mov	w8, #2
	b	.LBB13_15
.LBB13_7:
	bl	ktime_get
	ldr	w8, [x19, #264]
	cmp	w8, #3                  // =3
	b.eq	.LBB13_10
// %bb.8:
	cmp	w8, #2                  // =2
	b.eq	.LBB13_13
// %bb.9:
	cmp	w8, #1                  // =1
	b.ne	.LBB13_16
.LBB13_10:
	ldr	x8, [x19, #184]
	add	x8, x8, x20
	cmp	x8, x0
	b.ge	.LBB13_16
// %bb.11:
	str	wzr, [x19, #264]
	b	.LBB13_17
.LBB13_12:
	ldr	w8, [x19, #20]
	cbnz	w8, .LBB13_16
	b	.LBB13_14
.LBB13_13:
	ldr	x8, [x19, #168]
	add	x8, x8, x20
	cmp	x8, x0
	b.ge	.LBB13_16
.LBB13_14:
	mov	w8, #1
.LBB13_15:
	str	w8, [x19, #264]
.LBB13_16:
	adrp	x8, jiffies
	ldr	x8, [x8, :lo12:jiffies]
	add	x0, x19, #208           // =208
	add	x1, x8, #2              // =2
	bl	mod_timer
.LBB13_17:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end13:
	.size	ufs_mq_sync_burst_check_timer_expire, .Lfunc_end13-ufs_mq_sync_burst_check_timer_expire
                                        // -- End function
	.globl	ufs_mq_dump_request     // -- Begin function ufs_mq_dump_request
	.p2align	2
	.type	ufs_mq_dump_request,@function
ufs_mq_dump_request:                    // @ufs_mq_dump_request
.Lufs_mq_dump_request$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x24, x23, [sp, #16]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	ldr	x22, [x0, #1392]
	mov	x29, sp
	cbz	x22, .LBB14_2
// %bb.1:
	ldr	x23, [x22, #536]
	cbnz	x23, .LBB14_3
.LBB14_2:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB14_3:
	mov	w20, w1
	ldr	w1, [x0, #1248]
	mov	x19, x0
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	printk
	ldr	w1, [x19, #1252]
	adrp	x0, .L.str.1
	add	x0, x0, :lo12:.L.str.1
	bl	printk
	mov	x21, x23
	ldr	x8, [x21, #56]!
	cmp	x21, x8
	b.ne	.LBB14_11
.LBB14_4:
	mov	x21, x23
	ldr	x8, [x21, #80]!
	cmp	x21, x8
	b.ne	.LBB14_16
.LBB14_5:
	ldr	x8, [x23, #112]!
	cmp	x23, x8
	b.eq	.LBB14_2
// %bb.6:
	adrp	x0, .L.str.6
	add	x0, x0, :lo12:.L.str.6
	bl	printk
	mov	x0, x19
	bl	blk_queue_query_unistore_enable
	ldr	x21, [x23]
	cmp	x23, x21
	tbz	w0, #0, .LBB14_21
// %bb.7:
	b.eq	.LBB14_25
.LBB14_8:                               // =>This Inner Loop Header: Depth=1
	mov	x0, x21
	ldr	x8, [x0, #-80]!
	cmp	x8, x19
	b.ne	.LBB14_10
// %bb.9:                               //   in Loop: Header=BB14_8 Depth=1
	mov	w1, w20
	bl	mas_blk_dump_request
.LBB14_10:                              //   in Loop: Header=BB14_8 Depth=1
	ldr	x21, [x21]
	cmp	x23, x21
	b.ne	.LBB14_8
	b	.LBB14_25
.LBB14_11:
	adrp	x0, .L.str.2
	add	x0, x0, :lo12:.L.str.2
	bl	printk
	ldr	x24, [x21]
	b	.LBB14_13
.LBB14_12:                              //   in Loop: Header=BB14_13 Depth=1
	ldr	x24, [x24]
.LBB14_13:                              // =>This Inner Loop Header: Depth=1
	cmp	x21, x24
	b.eq	.LBB14_4
// %bb.14:                              //   in Loop: Header=BB14_13 Depth=1
	mov	x0, x24
	ldr	x8, [x0, #-80]!
	cmp	x8, x19
	b.ne	.LBB14_12
// %bb.15:                              //   in Loop: Header=BB14_13 Depth=1
	mov	w1, w20
	bl	mas_blk_dump_request
	b	.LBB14_12
.LBB14_16:
	adrp	x0, .L.str.3
	add	x0, x0, :lo12:.L.str.3
	bl	printk
	ldr	x24, [x21]
	cmp	x21, x24
	b.eq	.LBB14_20
.LBB14_17:                              // =>This Inner Loop Header: Depth=1
	mov	x0, x24
	ldr	x8, [x0, #-80]!
	cmp	x8, x19
	b.ne	.LBB14_19
// %bb.18:                              //   in Loop: Header=BB14_17 Depth=1
	mov	w1, w20
	bl	mas_blk_dump_request
.LBB14_19:                              //   in Loop: Header=BB14_17 Depth=1
	ldr	x24, [x24]
	cmp	x21, x24
	b.ne	.LBB14_17
.LBB14_20:
	ldp	x1, x2, [x22, #96]
	ldp	w3, w4, [x22, #112]
	adrp	x0, .L.str.20
	add	x0, x0, :lo12:.L.str.20
	bl	printk
	ldr	w1, [x22, #88]
	adrp	x0, .L.str.4
	add	x0, x0, :lo12:.L.str.4
	bl	printk
	ldr	x2, [x22, #8]
	adrp	x21, .L.str.5
	add	x21, x21, :lo12:.L.str.5
	mov	x0, x21
	mov	w1, wzr
	bl	printk
	ldr	x2, [x22, #16]
	mov	w1, #1
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #24]
	mov	w1, #2
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #32]
	mov	w1, #3
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #40]
	mov	w1, #4
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #48]
	mov	w1, #5
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #56]
	mov	w1, #6
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #64]
	mov	w1, #7
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #72]
	mov	w1, #8
	mov	x0, x21
	bl	printk
	ldr	x2, [x22, #80]
	mov	w1, #9
	mov	x0, x21
	bl	printk
	b	.LBB14_5
.LBB14_21:
	b.eq	.LBB14_25
.LBB14_22:                              // =>This Inner Loop Header: Depth=1
	sub	x0, x21, #768           // =768
	ldr	x8, [x0]
	cmp	x8, x19
	b.ne	.LBB14_24
// %bb.23:                              //   in Loop: Header=BB14_22 Depth=1
	mov	w1, w20
	bl	mas_blk_dump_request
.LBB14_24:                              //   in Loop: Header=BB14_22 Depth=1
	ldr	x21, [x21]
	cmp	x23, x21
	b.ne	.LBB14_22
.LBB14_25:
	ldp	x1, x2, [x22, #352]
	ldr	w3, [x22, #368]
	ldr	w4, [x22, #372]
	adrp	x0, .L.str.20
	add	x0, x0, :lo12:.L.str.20
	bl	printk
	b	.LBB14_2
.Lfunc_end14:
	.size	ufs_mq_dump_request, .Lfunc_end14-ufs_mq_dump_request
                                        // -- End function
	.globl	mas_queue_cp_enabled_show // -- Begin function mas_queue_cp_enabled_show
	.p2align	2
	.type	mas_queue_cp_enabled_show,@function
mas_queue_cp_enabled_show:              // @mas_queue_cp_enabled_show
.Lmas_queue_cp_enabled_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x1
	bl	mas_blk_get_lld
	ldr	w8, [x0, #24]
	adrp	x2, .L.str.7
	add	x2, x2, :lo12:.L.str.7
	mov	w1, #4096
	ubfx	w3, w8, #7, #1
	mov	x0, x19
	bl	snprintf
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x0, w0
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end15:
	.size	mas_queue_cp_enabled_show, .Lfunc_end15-mas_queue_cp_enabled_show
                                        // -- End function
	.globl	mas_queue_cp_enabled_store // -- Begin function mas_queue_cp_enabled_store
	.p2align	2
	.type	mas_queue_cp_enabled_store,@function
mas_queue_cp_enabled_store:             // @mas_queue_cp_enabled_store
.Lmas_queue_cp_enabled_store$local:
// %bb.0:
	sub	sp, sp, #64             // =64
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x21, [sp, #32]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	mov	x20, x2
	mov	x21, x1
	str	x8, [sp, #8]
	str	xzr, [sp]
	bl	mas_blk_get_lld
	mov	x19, x0
	mov	x0, sp
	mov	x1, x21
	mov	x2, x20
	bl	queue_var_store
	tbnz	x0, #63, .LBB16_2
// %bb.1:
	ldr	x8, [x19, #24]
	ldr	x9, [sp]
	and	x10, x8, #0xffffffffffffff7f
	orr	x8, x8, #0x80
	cmp	x9, #0                  // =0
	csel	x8, x10, x8, eq
	str	x8, [x19, #24]
.LBB16_2:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB16_4
// %bb.3:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldr	x21, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.LBB16_4:
	bl	__stack_chk_fail
.Lfunc_end16:
	.size	mas_queue_cp_enabled_store, .Lfunc_end16-mas_queue_cp_enabled_store
                                        // -- End function
	.globl	mas_blk_cp_debug_en     // -- Begin function mas_blk_cp_debug_en
	.p2align	2
	.type	mas_blk_cp_debug_en,@function
mas_blk_cp_debug_en:                    // @mas_blk_cp_debug_en
.Lmas_blk_cp_debug_en$local:
// %bb.0:
	adrp	x8, cp_debug_en
	ldr	w0, [x8, :lo12:cp_debug_en]
	ret
.Lfunc_end17:
	.size	mas_blk_cp_debug_en, .Lfunc_end17-mas_blk_cp_debug_en
                                        // -- End function
	.globl	mas_queue_cp_debug_en_show // -- Begin function mas_queue_cp_debug_en_show
	.p2align	2
	.type	mas_queue_cp_debug_en_show,@function
mas_queue_cp_debug_en_show:             // @mas_queue_cp_debug_en_show
.Lmas_queue_cp_debug_en_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	adrp	x8, cp_debug_en
	ldr	w3, [x8, :lo12:cp_debug_en]
	adrp	x2, .L.str.8
	mov	x0, x1
	add	x2, x2, :lo12:.L.str.8
	mov	w1, #4096
	mov	x29, sp
	bl	snprintf
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x0, w0
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end18:
	.size	mas_queue_cp_debug_en_show, .Lfunc_end18-mas_queue_cp_debug_en_show
                                        // -- End function
	.globl	mas_queue_cp_debug_en_store // -- Begin function mas_queue_cp_debug_en_store
	.p2align	2
	.type	mas_queue_cp_debug_en_store,@function
mas_queue_cp_debug_en_store:            // @mas_queue_cp_debug_en_store
.Lmas_queue_cp_debug_en_store$local:
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	x0, sp
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x19, [sp, #32]          // 8-byte Folded Spill
	add	x29, sp, #16            // =16
	mov	x19, x2
	str	x8, [sp, #8]
	str	xzr, [sp]
	bl	queue_var_store
	ldr	w8, [sp]
	asr	x9, x0, #63
	adrp	x10, cp_debug_en
	bic	w8, w8, w9
	str	w8, [x10, :lo12:cp_debug_en]
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB19_2
// %bb.1:
	mov	x0, x19
	ldr	x19, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB19_2:
	bl	__stack_chk_fail
.Lfunc_end19:
	.size	mas_queue_cp_debug_en_store, .Lfunc_end19-mas_queue_cp_debug_en_store
                                        // -- End function
	.globl	mas_queue_cp_limit_show // -- Begin function mas_queue_cp_limit_show
	.p2align	2
	.type	mas_queue_cp_limit_show,@function
mas_queue_cp_limit_show:                // @mas_queue_cp_limit_show
.Lmas_queue_cp_limit_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x8, [x0, #1392]
	mov	x0, x1
	mov	x29, sp
	cbz	x8, .LBB20_3
// %bb.1:
	ldr	x8, [x8, #536]
	cbz	x8, .LBB20_3
// %bb.2:
	ldr	w3, [x8, #8]
	b	.LBB20_4
.LBB20_3:
	mov	w3, wzr
.LBB20_4:
	adrp	x2, .L.str.9
	add	x2, x2, :lo12:.L.str.9
	mov	w1, #4096
	bl	snprintf
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x0, w0
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end20:
	.size	mas_queue_cp_limit_show, .Lfunc_end20-mas_queue_cp_limit_show
                                        // -- End function
	.globl	mas_queue_cp_limit_store // -- Begin function mas_queue_cp_limit_store
	.p2align	2
	.type	mas_queue_cp_limit_store,@function
mas_queue_cp_limit_store:               // @mas_queue_cp_limit_store
.Lmas_queue_cp_limit_store$local:
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x19, [sp, #32]          // 8-byte Folded Spill
	add	x29, sp, #16            // =16
	str	x8, [sp, #8]
	str	xzr, [sp]
	ldr	x8, [x0, #1392]
	cbz	x8, .LBB21_2
// %bb.1:
	ldr	x19, [x8, #536]
	b	.LBB21_3
.LBB21_2:
	mov	x19, xzr
.LBB21_3:
	mov	x0, sp
	bl	queue_var_store
	cbz	x19, .LBB21_6
// %bb.4:
	tbnz	x0, #63, .LBB21_6
// %bb.5:
	ldr	x8, [sp]
	str	w8, [x19, #8]
.LBB21_6:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB21_8
// %bb.7:
	ldr	x19, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB21_8:
	bl	__stack_chk_fail
.Lfunc_end21:
	.size	mas_queue_cp_limit_store, .Lfunc_end21-mas_queue_cp_limit_store
                                        // -- End function
	.globl	ufs_mq_flush_plug_list  // -- Begin function ufs_mq_flush_plug_list
	.p2align	2
	.type	ufs_mq_flush_plug_list,@function
ufs_mq_flush_plug_list:                 // @ufs_mq_flush_plug_list
.Lufs_mq_flush_plug_list$local:
// %bb.0:
	sub	sp, sp, #144            // =144
	adrp	x9, .L__const.ufs_mq_flush_plug_list.bd
	adrp	x8, __stack_chk_guard
	add	x9, x9, :lo12:.L__const.ufs_mq_flush_plug_list.bd
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	ldp	x10, x9, [x9]
	stp	x29, x30, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #112]    // 16-byte Folded Spill
	add	x29, sp, #48            // =48
	add	x21, sp, #8             // =8
	str	x27, [sp, #64]          // 8-byte Folded Spill
	stp	x26, x25, [sp, #80]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #96]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #128]    // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	stp	x10, x9, [sp, #24]
	stp	x21, x21, [sp, #8]
	mov	x8, x0
	ldr	x9, [x8, #40]!
	cmp	x8, x9
	b.eq	.LBB22_2
// %bb.1:
	ldr	x10, [x0, #48]
	str	x21, [x9, #8]
	str	x9, [sp, #8]
	str	x21, [x10]
	str	x10, [sp, #16]
	str	x8, [x0, #40]
	str	x8, [x0, #48]
	ldr	x8, [sp, #8]
	b	.LBB22_3
.LBB22_2:
	add	x8, sp, #8              // =8
.LBB22_3:
	adrp	x22, cpu_number
	adrp	x23, __per_cpu_offset
	mov	w26, #26215
	add	x22, x22, :lo12:cpu_number
	add	x23, x23, :lo12:__per_cpu_offset
	mov	w24, #2
	adrp	x25, mas_blk_mq_sync_disp_wq
	movk	w26, #26214, lsl #16
	mov	w27, #10
.LBB22_4:                               // =>This Inner Loop Header: Depth=1
	mov	x19, x8
	ldr	x20, [x19, #-80]!
	ldp	x10, x9, [x8]
	add	x3, sp, #24             // =24
	ldr	x11, [x20, #1392]
	str	x9, [x10, #8]
	str	x10, [x9]
	str	x8, [x8]
	str	x8, [x8, #8]
	str	x19, [sp, #24]
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
	ldr	w9, [x9, x22]
	ldur	w10, [x8, #-56]
	ldr	x12, [x20, #56]
	mov	x0, x19
	ldr	x9, [x23, x9, lsl #3]
	tst	w10, #0xff
	cset	w13, eq
	tst	w10, #0x8000000
	add	x9, x9, x12
	csel	x10, x13, x24, eq
	stur	x9, [x8, #-72]
	add	x8, x9, x10, lsl #3
	ldr	x1, [x8, #80]
	ldr	x2, [x11, #536]
	mov	x4, x20
	bl	__ufs_mq_queue_rq
	cbnz	w0, .LBB22_6
.LBB22_5:                               //   in Loop: Header=BB22_4 Depth=1
	ldr	x8, [sp, #8]
	cmp	x21, x8
	b.ne	.LBB22_4
	b	.LBB22_9
.LBB22_6:                               //   in Loop: Header=BB22_4 Depth=1
	cmp	w0, #9                  // =9
	b.ne	.LBB22_8
// %bb.7:                               //   in Loop: Header=BB22_4 Depth=1
	mov	x0, x19
	bl	__blk_mq_requeue_request
	mov	x0, x19
	mov	x1, x20
	bl	ufs_mq_insert_sync_list
	ldr	x19, [x20, #1392]
	ldr	x1, [x25, :lo12:mas_blk_mq_sync_disp_wq]
	mov	w0, #8
	mov	x3, xzr
	add	x2, x19, #120           // =120
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mul	x9, x8, x26
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	msub	w8, w9, w27, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #8]
	ldr	w8, [x19, #88]
	add	w8, w8, #1              // =1
	str	w8, [x19, #88]
	b	.LBB22_5
.LBB22_8:                               //   in Loop: Header=BB22_4 Depth=1
	mov	w1, #10
	mov	x0, x19
	bl	blk_mq_end_request
	b	.LBB22_5
.LBB22_9:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB22_11
// %bb.10:
	ldp	x20, x19, [sp, #128]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]    // 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]     // 16-byte Folded Reload
	ldr	x27, [sp, #64]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #48]     // 16-byte Folded Reload
	add	sp, sp, #144            // =144
	ret
.LBB22_11:
	bl	__stack_chk_fail
.Lfunc_end22:
	.size	ufs_mq_flush_plug_list, .Lfunc_end22-ufs_mq_flush_plug_list
                                        // -- End function
	.p2align	2               // -- Begin function __ufs_mq_queue_rq
	.type	__ufs_mq_queue_rq,@function
__ufs_mq_queue_rq:                      // @__ufs_mq_queue_rq
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	mov	x20, x0
	mov	x0, x4
	stp	x26, x25, [sp, #16]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x23, x4
	mov	x21, x3
	mov	x19, x2
	mov	x22, x1
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB23_3
// %bb.1:
	ldrb	w8, [x20, #240]
	tbz	w8, #2, .LBB23_3
// %bb.2:
	ldr	w8, [x20, #24]
	mov	w9, #61439
	movk	w9, #65471, lsl #16
	and	w8, w8, w9
	str	w8, [x20, #24]
.LBB23_3:
	ldr	x1, [x19]
	mov	x0, x20
	bl	mas_blk_set_lba_flag
	ldr	w8, [x20, #24]
	tbz	w8, #11, .LBB23_52
// %bb.4:
	mov	w25, #4096
	movk	w25, #64, lsl #16
	tst	w8, w25
	b.eq	.LBB23_7
.LBB23_5:
	ldrb	w8, [x20, #200]
	tbnz	w8, #1, .LBB23_10
// %bb.6:
	add	x8, x19, #44            // =44
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	add	x8, x20, #808           // =808
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 1
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
	b	.LBB23_11
.LBB23_7:
	ldrb	w8, [x20, #200]
	tbz	w8, #7, .LBB23_18
// %bb.8:
	ldr	x0, [x20]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB23_18
// %bb.9:
	ldr	w8, [x20, #24]
	tst	w8, w25
	b.ne	.LBB23_5
.LBB23_10:
	add	x8, x19, #40            // =40
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	add	x8, x20, #808           // =808
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 2
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
.LBB23_11:
	ldr	x0, [x20]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB23_13
// %bb.12:
	ldrb	w8, [x20, #200]
	tbnz	w8, #7, .LBB23_39
.LBB23_13:
	ldr	x0, [x20]
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #24]
	tbz	w8, #7, .LBB23_39
// %bb.14:
	ldrb	w8, [x19, #536]
	cbnz	w8, .LBB23_39
// %bb.15:
	ldr	x0, [x20]
	bl	mas_blk_get_lld
	ldrb	w9, [x20, #200]
	ldr	w8, [x20, #24]
	mov	x24, x0
	tbnz	w9, #1, .LBB23_25
// %bb.16:
	tst	w8, w25
	b.eq	.LBB23_26
// %bb.17:
	ldr	w8, [x19, #40]
	cmp	w8, #2                  // =2
	b.le	.LBB23_26
	b	.LBB23_39
.LBB23_18:
	add	x8, x20, #808           // =808
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 4
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
	add	x8, x19, #32            // =32
	//APP
	// atomic_add_return
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stlxr	w10, w9, [x8]
	cbnz	w10, 1b
	dmb ish
	//NO_APP
	ldr	w8, [x19, #156]
	cmp	w9, w8
	b.le	.LBB23_20
// %bb.19:
	ldrb	w8, [x19, #536]
	cbz	w8, .LBB23_54
.LBB23_20:
	ldrb	w8, [x19, #332]
	cbnz	w8, .LBB23_39
// %bb.21:
	ldr	x0, [x20]
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #24]
	tbz	w8, #7, .LBB23_39
// %bb.22:
	ldrb	w8, [x20, #24]
	cbnz	w8, .LBB23_39
// %bb.23:
	ldr	x8, [x20, #240]
	tbnz	w8, #1, .LBB23_58
// %bb.24:
	ldr	x9, [x20, #200]
	orr	x8, x8, #0x2
	str	x8, [x20, #240]
	add	x10, x19, #12           // =12
	orr	x8, x9, #0x1
	str	x8, [x20, #200]
	//APP
	// atomic_add
	prfm	pstl1strm, [x10]
1:	ldxr	w8, [x10]
	add	w8, w8, 1
	stxr	w9, w8, [x10]
	cbnz	w9, 1b

	//NO_APP
	b	.LBB23_38
.LBB23_25:
	and	w8, w8, #0xff
	cmp	w8, #1                  // =1
	b.ne	.LBB23_36
.LBB23_26:
	ldr	w8, [x19, #32]
	cbnz	w8, .LBB23_28
// %bb.27:
	ldr	w8, [x19, #36]
	cbz	w8, .LBB23_36
.LBB23_28:
	ldr	w8, [x19, #12]
	cbz	w8, .LBB23_36
// %bb.29:
	ldr	x8, [x24, #1840]
	add	x26, x24, #1840         // =1840
	cmp	x26, x8
	b.eq	.LBB23_36
// %bb.30:
	add	x25, x24, #1872         // =1872
	mov	x0, x25
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x24, #1840]
	mov	x1, x0
	cmp	x26, x8
	b.eq	.LBB23_32
// %bb.31:
	ldur	x24, [x8, #-184]
	b	.LBB23_33
.LBB23_32:
	mov	x24, xzr
.LBB23_33:
	mov	x0, x25
	bl	_raw_spin_unlock_irqrestore
	bl	ktime_get
	mov	w8, #57600
	movk	w8, #1525, lsl #16
	add	x8, x24, x8
	cmp	x0, x8
	b.lt	.LBB23_36
// %bb.34:
	mov	w8, #25856
	movk	w8, #7629, lsl #16
	add	x8, x24, x8
	cmp	x0, x8
	b.ge	.LBB23_48
// %bb.35:
	ldr	w8, [x19, #12]
	ldr	w9, [x19, #8]
	cmp	w8, w9
	b.hs	.LBB23_50
.LBB23_36:
	ldr	x8, [x20, #240]
	tbnz	w8, #1, .LBB23_58
// %bb.37:
	ldr	x9, [x20, #200]
	orr	x8, x8, #0x2
	str	x8, [x20, #240]
	add	x10, x19, #12           // =12
	orr	x8, x9, #0x1
	str	x8, [x20, #200]
	//APP
	// atomic_add
	prfm	pstl1strm, [x10]
1:	ldxr	w8, [x10]
	add	w8, w8, 1
	stxr	w9, w8, [x10]
	cbnz	w9, 1b

	//NO_APP
.LBB23_38:
	adrp	x8, cp_debug_en
	ldr	w8, [x8, :lo12:cp_debug_en]
	cbnz	w8, .LBB23_55
.LBB23_39:
	ldr	x8, [x22, #232]
	ldr	x8, [x8, #48]
	ldr	x8, [x8, #16]
	cbz	x8, .LBB23_41
// %bb.40:
	mov	x0, x22
	blr	x8
	tbz	w0, #0, .LBB23_42
.LBB23_41:
	ldr	x8, [x23, #48]
	mov	x0, x22
	mov	x1, x21
	ldr	x8, [x8]
	blr	x8
	ldr	x8, [x21]
	mov	w21, w0
	mov	w1, #14
	mov	x0, x8
	bl	mas_blk_latency_req_check
	ands	w21, w21, #0xff
	b.eq	.LBB23_47
	b	.LBB23_43
.LBB23_42:
	ldr	x0, [x21]
	mov	w1, #15
	bl	mas_blk_latency_req_check
	mov	w21, #9
.LBB23_43:
	ldr	w8, [x20, #256]
	mov	w9, #1
	add	w8, w8, #1              // =1
	stp	w9, w8, [x20, #252]
	ldr	w8, [x19, #44]
	ldr	w9, [x19, #40]
	ldr	w10, [x19, #32]
	ldr	w11, [x19, #36]
	add	w8, w9, w8
	add	w8, w8, w10
	add	w8, w8, w11
	cmp	w8, #8                  // =8
	b.gt	.LBB23_46
// %bb.44:
	mov	w8, #4
	str	w8, [x20, #252]
	add	x8, x20, #268           // =268
.LBB23_45:
	ldr	w9, [x8]
	add	w9, w9, #1              // =1
	str	w9, [x8]
.LBB23_46:
	mov	x0, x20
	mov	x1, x19
	mov	w2, wzr
	bl	ufs_mq_rq_inflt_update
.LBB23_47:
	mov	w0, w21
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB23_48:
	adrp	x8, cp_debug_en
	ldr	w8, [x8, :lo12:cp_debug_en]
	cbz	w8, .LBB23_39
// %bb.49:
	adrp	x0, .L.str.22
	add	x0, x0, :lo12:.L.str.22
	bl	printk
	b	.LBB23_39
.LBB23_50:
	adrp	x8, cp_debug_en
	ldr	w8, [x8, :lo12:cp_debug_en]
	cbz	w8, .LBB23_39
// %bb.51:
	adrp	x0, .L.str.21
	add	x0, x0, :lo12:.L.str.21
	mov	w1, #5
	bl	printk
	b	.LBB23_39
.LBB23_52:
	add	x8, x20, #808           // =808
	//APP
	// atomic64_or
	prfm	pstl1strm, [x8]
1:	ldxr	x9, [x8]
	orr	x9, x9, 8
	stxr	w10, x9, [x8]
	cbnz	w10, 1b
	//NO_APP
	add	x8, x19, #36            // =36
	//APP
	// atomic_add_return
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stlxr	w10, w9, [x8]
	cbnz	w10, 1b
	dmb ish
	//NO_APP
	ldr	w8, [x19, #160]
	cmp	w9, w8
	b.le	.LBB23_39
// %bb.53:
	ldrb	w8, [x19, #536]
	cbnz	w8, .LBB23_39
.LBB23_54:
	mov	w9, #2
	add	x8, x20, #260           // =260
	str	w9, [x20, #252]
	mov	w21, #9
	b	.LBB23_45
.LBB23_55:
	adrp	x0, .L.str.24
	add	x1, x20, #224           // =224
	add	x0, x0, :lo12:.L.str.24
	bl	printk
	ldr	x8, [x20]
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB23_39
// %bb.56:
	ldr	x25, [x8, #536]
	cbz	x25, .LBB23_39
// %bb.57:
	ldr	w2, [x25, #28]
	adrp	x24, .L.str.44
	adrp	x0, .L.str.12
	add	x24, x24, :lo12:.L.str.44
	add	x0, x0, :lo12:.L.str.12
	mov	x1, x24
	bl	printk
	ldr	w2, [x25, #24]
	ldr	w3, [x25, #16]
	ldr	w4, [x25, #20]
	adrp	x0, .L.str.13
	add	x0, x0, :lo12:.L.str.13
	mov	x1, x24
	bl	printk
	ldr	w2, [x25, #44]
	ldr	w3, [x25, #40]
	ldr	w4, [x25, #32]
	ldr	w5, [x25, #36]
	ldr	w6, [x25, #12]
	adrp	x0, .L.str.14
	add	x0, x0, :lo12:.L.str.14
	mov	x1, x24
	bl	printk
	b	.LBB23_39
.LBB23_58:
	adrp	x0, .L.str.23
	add	x0, x0, :lo12:.L.str.23
	bl	mas_blk_rdr_panic
.Lfunc_end23:
	.size	__ufs_mq_queue_rq, .Lfunc_end23-__ufs_mq_queue_rq
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_insert_sync_list
	.type	ufs_mq_insert_sync_list,@function
ufs_mq_insert_sync_list:                // @ufs_mq_insert_sync_list
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x1, #1392]
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	w1, #12
	mov	x29, sp
	ldr	x21, [x8, #536]
	mov	x20, x0
	bl	mas_blk_latency_req_check
	add	x19, x21, #48           // =48
	mov	x0, x19
	bl	_raw_spin_lock_irqsave
	ldrb	w8, [x20, #200]
	mov	x1, x0
	tbnz	w8, #1, .LBB24_3
// %bb.1:
	ldr	w8, [x20, #24]
	mov	w9, #4096
	movk	w9, #64, lsl #16
	tst	w8, w9
	b.eq	.LBB24_9
// %bb.2:
	ldr	x9, [x21, #64]
	add	x8, x20, #80            // =80
	add	x10, x21, #56           // =56
	str	x8, [x21, #64]
	stp	x10, x9, [x20, #80]
	str	x8, [x9]
	add	x8, x21, #72            // =72
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	b	.LBB24_20
.LBB24_3:
	mov	x8, x21
	ldr	x9, [x8, #56]!
	cmp	x8, x9
	b.eq	.LBB24_17
// %bb.4:
	ldrb	w10, [x9, #120]
	tbz	w10, #1, .LBB24_8
.LBB24_5:                               // =>This Inner Loop Header: Depth=1
	ldr	x10, [x9]
	cmp	x10, x8
	b.eq	.LBB24_18
// %bb.6:                               //   in Loop: Header=BB24_5 Depth=1
	ldrb	w11, [x10, #120]
	mov	x9, x10
	tbnz	w11, #1, .LBB24_5
// %bb.7:
	mov	x9, x10
.LBB24_8:
	ldr	x8, [x9, #8]
	add	x10, x20, #80           // =80
	str	x10, [x9, #8]
	stp	x9, x8, [x20, #80]
	str	x10, [x8]
	b	.LBB24_19
.LBB24_9:
	mov	x9, x21
	ldr	x8, [x9, #80]!
	cmp	x9, x8
	b.eq	.LBB24_11
// %bb.10:
	ldr	w10, [x20, #736]
	cbnz	w10, .LBB24_13
.LBB24_11:
	ldr	x10, [x21, #88]
	add	x11, x20, #80           // =80
	str	x11, [x21, #88]
	str	x9, [x20, #80]
	b	.LBB24_16
.LBB24_12:                              //   in Loop: Header=BB24_13 Depth=1
	ldr	x8, [x8]
	cmp	x9, x8
	b.eq	.LBB24_15
.LBB24_13:                              // =>This Inner Loop Header: Depth=1
	ldr	w11, [x8, #656]
	cbz	w11, .LBB24_12
// %bb.14:                              //   in Loop: Header=BB24_13 Depth=1
	sub	w11, w10, w11
	tbz	w11, #31, .LBB24_12
.LBB24_15:
	ldr	x10, [x8, #8]
	add	x11, x20, #80           // =80
	str	x11, [x8, #8]
	str	x8, [x20, #80]
.LBB24_16:
	str	x10, [x20, #88]
	str	x11, [x10]
	add	x8, x21, #96            // =96
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	b	.LBB24_20
.LBB24_17:
	ldr	x9, [x21, #64]
.LBB24_18:
	add	x10, x20, #80           // =80
	str	x10, [x21, #64]
	stp	x8, x9, [x20, #80]
	str	x10, [x9]
.LBB24_19:
	add	x8, x21, #72            // =72
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB24_20:
	mov	x0, x19
	bl	_raw_spin_unlock_irqrestore
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end24:
	.size	ufs_mq_insert_sync_list, .Lfunc_end24-ufs_mq_insert_sync_list
                                        // -- End function
	.globl	ufs_mq_make_request     // -- Begin function ufs_mq_make_request
	.p2align	2
	.type	ufs_mq_make_request,@function
ufs_mq_make_request:                    // @ufs_mq_make_request
.Lufs_mq_make_request$local:
// %bb.0:
	sub	sp, sp, #192            // =192
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #96]     // 16-byte Folded Spill
	add	x29, sp, #96            // =96
	str	x27, [sp, #112]         // 8-byte Folded Spill
	stp	x26, x25, [sp, #128]    // 16-byte Folded Spill
	stp	x24, x23, [sp, #144]    // 16-byte Folded Spill
	stp	x22, x21, [sp, #160]    // 16-byte Folded Spill
	stp	x20, x19, [sp, #176]    // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	stur	x1, [x29, #-32]
	stp	xzr, xzr, [sp, #48]
	stp	xzr, xzr, [sp, #32]
	stp	xzr, xzr, [sp, #16]
	str	xzr, [sp, #8]
	ldr	w22, [x1, #16]
	mov	w8, #253
	mov	x21, x1
	mov	x20, x0
	tst	w22, w8
	b.eq	.LBB25_3
// %bb.1:
	mov	w8, #6144
	movk	w8, #68, lsl #16
	and	w8, w22, w8
	cbnz	w8, .LBB25_3
// %bb.2:
	mov	w19, #1
	ldr	x8, [x21, #8]
	cbnz	x8, .LBB25_10
	b	.LBB25_17
.LBB25_3:
	ldr	x8, [x20, #1392]
	orr	w9, w22, #0x800
	str	w9, [x21, #16]
	ldr	x21, [x8, #536]
	bl	ktime_get
	ldr	w8, [x21, #264]
	mov	x19, x0
	cmp	w8, #1                  // =1
	b.eq	.LBB25_7
// %bb.4:
	cbnz	w8, .LBB25_9
// %bb.5:
	ldr	x8, [x21, #168]
	mov	w9, #19264
	movk	w9, #76, lsl #16
	add	x8, x8, x9
	cmp	x8, x19
	b.le	.LBB25_9
// %bb.6:
	mov	w8, #1
	str	x19, [x21, #176]
	str	w8, [x21, #264]
	adrp	x8, jiffies
	ldr	x8, [x8, :lo12:jiffies]
	add	x0, x21, #208           // =208
	add	x1, x8, #2              // =2
	bl	mod_timer
	b	.LBB25_9
.LBB25_7:
	ldr	x8, [x21, #168]
	mov	w9, #19264
	movk	w9, #76, lsl #16
	add	x8, x8, x9
	cmp	x8, x19
	b.le	.LBB25_9
// %bb.8:
	mov	w8, #2
	str	w8, [x21, #264]
.LBB25_9:
	str	x19, [x21, #168]
	ldur	x21, [x29, #-32]
	mov	w19, wzr
	ldr	w22, [x21, #16]
	ldr	x8, [x21, #8]
	cbz	x8, .LBB25_17
.LBB25_10:
	ldr	x0, [x8, #1272]
	cbz	x0, .LBB25_17
// %bb.11:
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB25_17
// %bb.12:
	ldrb	w8, [x21, #16]
	cbnz	w8, .LBB25_20
// %bb.13:
	//APP
	mrs x8, sp_el0
	//NO_APP
	ldr	x9, [x8, #2688]
	cbz	x9, .LBB25_20
// %bb.14:
	ldr	w10, [x21, #40]
	cmp	w10, #128, lsl #12      // =524288
	b.hi	.LBB25_20
// %bb.15:
	ldr	x11, [x9, #56]
	adrp	x10, __cfi_ufs_mq_flush_plug_list
	add	x10, x10, :lo12:__cfi_ufs_mq_flush_plug_list
	cbz	x11, .LBB25_24
// %bb.16:
	cmp	x11, x10
	b.eq	.LBB25_24
	b	.LBB25_20
.LBB25_17:
	tst	w22, #0xc0000
	cset	w8, ne
	orr	w8, w19, w8
	tbnz	w8, #0, .LBB25_20
// %bb.18:
	//APP
	mrs x8, sp_el0
	//NO_APP
	ldr	x9, [x8, #2688]
	cbz	x9, .LBB25_20
// %bb.19:
	ldr	w10, [x21, #40]
	cmp	w10, #128, lsl #12      // =524288
	b.ls	.LBB25_21
.LBB25_20:
	mov	x24, xzr
	b	.LBB25_25
.LBB25_21:
	ldr	x10, [x9, #56]
	cbz	x10, .LBB25_23
// %bb.22:
	adrp	x11, __cfi_ufs_mq_flush_plug_list
	add	x11, x11, :lo12:__cfi_ufs_mq_flush_plug_list
	cmp	x10, x11
	b.ne	.LBB25_20
.LBB25_23:
	adrp	x10, __cfi_ufs_mq_flush_plug_list
	add	x10, x10, :lo12:__cfi_ufs_mq_flush_plug_list
.LBB25_24:
	str	x10, [x9, #56]
	ldr	x24, [x8, #2688]
.LBB25_25:
	sub	x1, x29, #32            // =32
	add	x2, sp, #4              // =4
	mov	x0, x20
	str	wzr, [sp, #4]
	bl	__blk_queue_split
	cbz	x24, .LBB25_36
// %bb.26:
	ldr	x26, [x24, #48]
	add	x27, x24, #40           // =40
	cmp	x27, x26
	b.eq	.LBB25_36
// %bb.27:
	ldur	x21, [x29, #-32]
	ldr	w22, [sp, #4]
	mov	x25, xzr
	b	.LBB25_30
.LBB25_28:                              //   in Loop: Header=BB25_30 Depth=1
	mov	x0, x23
	mov	x1, x21
	mov	w2, w22
	bl	bio_attempt_front_merge
	mov	x25, x23
	tbnz	w0, #0, .LBB25_35
.LBB25_29:                              //   in Loop: Header=BB25_30 Depth=1
	ldr	x26, [x26, #8]
	cmp	x27, x26
	b.eq	.LBB25_37
.LBB25_30:                              // =>This Inner Loop Header: Depth=1
	mov	x23, x26
	ldr	x8, [x23, #-80]!
	cmp	x8, x20
	b.ne	.LBB25_29
// %bb.31:                              //   in Loop: Header=BB25_30 Depth=1
	mov	x0, x23
	mov	x1, x21
	bl	blk_rq_merge_ok
	mov	x25, x23
	tbz	w0, #0, .LBB25_29
// %bb.32:                              //   in Loop: Header=BB25_30 Depth=1
	mov	x0, x23
	mov	x1, x21
	bl	blk_try_merge
	cmp	w0, #1                  // =1
	b.eq	.LBB25_28
// %bb.33:                              //   in Loop: Header=BB25_30 Depth=1
	cmp	w0, #2                  // =2
	mov	x25, x23
	b.ne	.LBB25_29
// %bb.34:                              //   in Loop: Header=BB25_30 Depth=1
	mov	x0, x23
	mov	x1, x21
	mov	w2, w22
	bl	bio_attempt_back_merge
	mov	x25, x23
	tbz	w0, #0, .LBB25_29
.LBB25_35:
	mov	w20, #-1
	b	.LBB25_75
.LBB25_36:
	mov	x25, xzr
.LBB25_37:
	ldur	x8, [x29, #-32]
	ldrh	w9, [x8, #20]
	orr	w9, w9, #0x1000
	strh	w9, [x8, #20]
	ldr	x0, [x20, #24]
	cbz	x0, .LBB25_39
// %bb.38:
	mov	x1, x8
	bl	__rq_qos_throttle
	ldur	x8, [x29, #-32]
.LBB25_39:
	mov	w1, #3
	mov	x0, x8
	bl	mas_blk_latency_bio_check
	ldur	x1, [x29, #-32]
	add	x2, sp, #8              // =8
	mov	x0, x20
	ldr	w8, [x1, #16]
	str	w8, [sp, #44]
	ldr	x8, [x1, #88]
	str	w8, [sp, #20]
	bl	blk_mq_get_request
	ldur	x1, [x29, #-32]
	cbz	x0, .LBB25_79
// %bb.40:
	ldrb	w8, [x0, #24]
	ldr	x9, [x1, #88]
	mov	x21, x0
	cmp	w8, #1                  // =1
	str	x9, [x0, #200]
	b.eq	.LBB25_44
// %bb.41:
	cmp	w8, #3                  // =3
	b.eq	.LBB25_44
// %bb.42:
	cmp	w8, #2                  // =2
	b.ne	.LBB25_45
// %bb.43:
	ldr	x0, [x21]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB25_45
.LBB25_44:
	mov	x0, x21
	bl	blk_req_set_make_req_nr
.LBB25_45:
	ldr	x0, [x20, #24]
	cbz	x0, .LBB25_47
// %bb.46:
	ldur	x2, [x29, #-32]
	mov	x1, x21
	bl	__rq_qos_track
.LBB25_47:
	ldr	w8, [x21, #32]
	ldr	x26, [sp, #56]
	cmn	w8, #1                  // =1
	b.eq	.LBB25_49
// %bb.48:
	ldr	w9, [x26, #452]
	orr	w20, w8, w9, lsl #16
	b	.LBB25_50
.LBB25_49:
	ldr	w8, [x21, #36]
	ldr	w9, [x26, #452]
	orr	w8, w8, w9, lsl #16
	orr	w20, w8, #0x80000000
.LBB25_50:
	ldur	x22, [x29, #-32]
	ldr	w8, [x22, #16]
	and	w9, w8, #0xff
	cmp	w9, #2                  // =2
	b.eq	.LBB25_78
// %bb.51:
	tbnz	w8, #19, .LBB25_78
// %bb.52:
	ldr	w9, [x21, #24]
	tbz	w9, #11, .LBB25_83
// %bb.53:
	ldr	x8, [x21]
	ldr	w2, [sp, #4]
	mov	x0, x21
	mov	x1, x22
	ldr	x23, [x8, #1392]
	stp	xzr, xzr, [x29, #-24]
	bl	blk_mq_bio_to_request
	mov	x19, x21
	cbz	x24, .LBB25_65
// %bb.54:
	ldr	x0, [x21]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB25_61
// %bb.55:
	adrp	x8, __tracepoint_block_plug+8
	ldr	x19, [x21]
	ldr	w8, [x8, :lo12:__tracepoint_block_plug+8]
	cmp	w8, #1                  // =1
	b.lt	.LBB25_61
// %bb.56:
	adrp	x9, cpu_number
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
	add	x9, x9, :lo12:cpu_number
	ldr	w8, [x8, x9]
	adrp	x10, __cpu_online_mask
	add	x10, x10, :lo12:__cpu_online_mask
	add	w9, w8, #63             // =63
	cmp	w8, #0                  // =0
	csel	w9, w9, w8, lt
	asr	w9, w9, #6
	ldr	x9, [x10, w9, sxtw #3]
	lsr	x8, x9, x8
	tbz	w8, #0, .LBB25_61
// %bb.57:
	//APP
	mrs x22, sp_el0
	//NO_APP
	ldr	w8, [x22, #16]
	add	w8, w8, #1              // =1
	str	w8, [x22, #16]
	//APP
	//NO_APP
	adrp	x8, __tracepoint_block_plug+32
	ldr	x26, [x8, :lo12:__tracepoint_block_plug+32]
	cbz	x26, .LBB25_59
.LBB25_58:                              // =>This Inner Loop Header: Depth=1
	ldp	x8, x0, [x26]
	mov	x1, x19
	blr	x8
	ldr	x8, [x26, #24]!
	cbnz	x8, .LBB25_58
.LBB25_59:
	//APP
	//NO_APP
	ldr	x8, [x22, #16]
	subs	x8, x8, #1              // =1
	str	w8, [x22, #16]
	b.eq	.LBB25_91
// %bb.60:
	ldr	x8, [x22, #16]
	cbz	x8, .LBB25_91
.LBB25_61:
	cbz	x25, .LBB25_64
.LBB25_62:
	mov	x8, x24
	ldr	x9, [x8, #40]!
	cmp	x8, x9
	b.eq	.LBB25_88
// %bb.63:
	mov	x8, x25
	ldr	x9, [x8, #80]!
	ldr	x10, [x8, #8]
	str	x10, [x9, #8]
	str	x9, [x10]
	str	x8, [x8]
	str	x8, [x8, #8]
.LBB25_64:
	ldr	x9, [x24, #48]
	add	x8, x21, #80            // =80
	add	x10, x24, #40           // =40
	mov	w1, #10
	mov	x0, x21
	str	x8, [x24, #48]
	stp	x10, x9, [x21, #80]
	str	x8, [x9]
	bl	mas_blk_latency_req_check
	mov	x19, x25
.LBB25_65:
	stur	x19, [x29, #-24]
	ldr	x0, [x21]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB25_72
// %bb.66:
	cbz	x19, .LBB25_75
// %bb.67:
	ldr	x4, [x21]
	ldrb	w9, [x19, #24]
	ldr	x8, [x4, #1392]
	ldr	x8, [x8, #536]
	cbz	w9, .LBB25_70
// %bb.68:
	mov	x9, x8
	ldr	x10, [x9, #56]!
	cmp	x9, x10
	b.ne	.LBB25_71
// %bb.69:
	mov	x9, x8
	ldr	x10, [x9, #80]!
	cmp	x9, x10
	b.ne	.LBB25_71
.LBB25_70:
	ldr	x9, [x8, #136]!
	cmp	x8, x9
	b.eq	.LBB25_77
.LBB25_71:
	mov	x0, x19
	bl	__blk_mq_requeue_request
	ldr	x19, [x21]
	ldur	x0, [x29, #-24]
	mov	x1, x19
	bl	ufs_mq_insert_sync_list
	ldr	x19, [x19, #1392]
	adrp	x8, mas_blk_mq_sync_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_sync_disp_wq]
	mov	w0, #8
	add	x2, x19, #120           // =120
	mov	x3, xzr
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #8]
	ldr	w8, [x19, #88]
	add	w8, w8, #1              // =1
	str	w8, [x19, #88]
	b	.LBB25_75
.LBB25_72:
	cbz	x19, .LBB25_75
// %bb.73:
	ldr	x1, [sp, #56]
	ldr	x2, [x23, #536]
	ldr	x4, [x21]
.LBB25_74:
	sub	x3, x29, #24            // =24
	mov	x0, x19
	bl	__ufs_mq_queue_rq
	cbnz	w0, .LBB25_89
.LBB25_75:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB25_99
// %bb.76:
	mov	w0, w20
	ldp	x20, x19, [sp, #176]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #160]    // 16-byte Folded Reload
	ldp	x24, x23, [sp, #144]    // 16-byte Folded Reload
	ldp	x26, x25, [sp, #128]    // 16-byte Folded Reload
	ldr	x27, [sp, #112]         // 8-byte Folded Reload
	ldp	x29, x30, [sp, #96]     // 16-byte Folded Reload
	add	sp, sp, #192            // =192
	ret
.LBB25_77:
	ldr	x1, [sp, #56]
	ldr	x2, [x23, #536]
	b	.LBB25_74
.LBB25_78:
	ldr	w2, [sp, #4]
	mov	x0, x21
	mov	x1, x22
	bl	blk_mq_bio_to_request
	mov	x0, x21
	bl	blk_insert_flush
	b	.LBB25_87
.LBB25_79:
	ldr	x0, [x20, #24]
	cbz	x0, .LBB25_81
// %bb.80:
	bl	__rq_qos_cleanup
	ldur	x1, [x29, #-32]
.LBB25_81:
	ldrb	w8, [x1, #18]
	tbz	w8, #7, .LBB25_35
// %bb.82:
	mov	w8, #12
	mov	x0, x1
	strb	w8, [x1, #26]
	bl	bio_endio
	mov	w20, #-1
	b	.LBB25_75
.LBB25_83:
	ldr	x23, [sp, #48]
	ldr	w2, [sp, #4]
	mov	w10, #16384
	movk	w10, #12, lsl #16
	tst	w8, w10
	b.ne	.LBB25_85
// %bb.84:
	and	w8, w9, #0xfe
	orr	w8, w8, #0x1
	cmp	w8, #33                 // =33
	b.ne	.LBB25_92
.LBB25_85:
	mov	x0, x21
	mov	x1, x22
	bl	blk_mq_bio_to_request
	mov	x0, x23
	bl	_raw_spin_lock
.LBB25_86:
	ldr	x1, [x26, #232]
	mov	x0, x21
	bl	ufs_mq_req_insert
	mov	x0, x23
	bl	_raw_spin_unlock
.LBB25_87:
	ldr	x0, [sp, #56]
	mov	w1, w19
	bl	blk_mq_run_hw_queue
	b	.LBB25_75
.LBB25_88:
	mov	x25, xzr
	b	.LBB25_64
.LBB25_89:
	ldur	x19, [x29, #-24]
	cmp	w0, #9                  // =9
	b.eq	.LBB25_71
// %bb.90:
	mov	w1, #10
	mov	x0, x19
	bl	blk_mq_end_request
	b	.LBB25_75
.LBB25_91:
	bl	preempt_schedule_notrace
	cbnz	x25, .LBB25_62
	b	.LBB25_64
.LBB25_92:
	ldr	x25, [x26, #232]
	mov	x0, x23
	str	w2, [sp]                // 4-byte Folded Spill
	mov	x27, x26
	bl	_raw_spin_lock
	ldrb	w8, [x22, #17]
	tbnz	w8, #3, .LBB25_98
// %bb.93:
	ldr	x8, [x25, #1400]
	cbz	x8, .LBB25_98
// %bb.94:
	ldr	x26, [x8, #176]
	cbz	x26, .LBB25_98
// %bb.95:
	ldr	x8, [x26]
	ldr	x8, [x8, #40]
	cbz	x8, .LBB25_98
// %bb.96:
	ldr	x8, [x25, #1392]
	ldr	x8, [x8, #536]
	add	x24, x8, #100           // =100
	mov	x0, x24
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x26]
	ldr	w2, [sp]                // 4-byte Folded Reload
	mov	x26, x0
	mov	x0, x22
	ldr	x8, [x8, #40]
	mov	x1, x25
	blr	x8
	mov	w25, w0
	mov	x0, x24
	mov	x1, x26
	bl	_raw_spin_unlock_irqrestore
	tbz	w25, #0, .LBB25_98
// %bb.97:
	mov	x0, x23
	bl	_raw_spin_unlock
	mov	x0, x21
	bl	blk_mq_free_request
	b	.LBB25_75
.LBB25_98:
	ldr	w2, [sp]                // 4-byte Folded Reload
	mov	x0, x21
	mov	x1, x22
	bl	blk_mq_bio_to_request
	mov	x26, x27
	b	.LBB25_86
.LBB25_99:
	bl	__stack_chk_fail
.Lfunc_end25:
	.size	ufs_mq_make_request, .Lfunc_end25-ufs_mq_make_request
                                        // -- End function
	.globl	ufs_mq_sync_io_dispatch_work_fn // -- Begin function ufs_mq_sync_io_dispatch_work_fn
	.p2align	2
	.type	ufs_mq_sync_io_dispatch_work_fn,@function
ufs_mq_sync_io_dispatch_work_fn:        // @ufs_mq_sync_io_dispatch_work_fn
.Lufs_mq_sync_io_dispatch_work_fn$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x0, [x0, #136]
	mov	x29, sp
	bl	ufs_mq_sync_dispatch
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end26:
	.size	ufs_mq_sync_io_dispatch_work_fn, .Lfunc_end26-ufs_mq_sync_io_dispatch_work_fn
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_sync_dispatch
	.type	ufs_mq_sync_dispatch,@function
ufs_mq_sync_dispatch:                   // @ufs_mq_sync_dispatch
// %bb.0:
	sub	sp, sp, #304            // =304
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #208]    // 16-byte Folded Spill
	add	x29, sp, #208           // =208
	stp	x28, x27, [sp, #224]    // 16-byte Folded Spill
	stp	x26, x25, [sp, #240]    // 16-byte Folded Spill
	stp	x24, x23, [sp, #256]    // 16-byte Folded Spill
	stp	x22, x21, [sp, #272]    // 16-byte Folded Spill
	stp	x20, x19, [sp, #288]    // 16-byte Folded Spill
	stur	x8, [x29, #-16]
	stp	xzr, xzr, [sp, #96]
	ldr	x27, [x0, #1392]
	mov	x20, x0
	ldr	x19, [x27, #536]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB27_93
// %bb.1:
	mov	x0, x20
	bl	mas_blk_get_lld
	mov	x26, x0
	cbz	x0, .LBB27_3
// %bb.2:
	ldr	w8, [x26, #580]
	cbz	w8, .LBB27_93
.LBB27_3:
	mov	x0, x20
	bl	mas_blk_get_lld
	cbz	x0, .LBB27_92
// %bb.4:
	ldr	x8, [x0, #88]
	mov	x22, x0
	cbz	x8, .LBB27_92
// %bb.5:
	add	x23, x22, #648          // =648
	mov	x0, x23
	bl	mutex_lock
	ldr	w8, [x22, #584]
	cbnz	w8, .LBB27_9
// %bb.6:
	ldr	w21, [x22, #588]
	mov	x0, x23
	bl	mutex_unlock
	cbz	w21, .LBB27_40
// %bb.7:
	adrp	x21, g_recovery_pwron_info
	add	x21, x21, :lo12:g_recovery_pwron_info
	ldr	w24, [x21, #4]
	mov	x0, x23
	mov	x25, x27
	cmp	w24, #0                 // =0
	cset	w27, ne
	bl	mutex_lock
	ldr	w8, [x21]
	cbnz	w8, .LBB27_12
// %bb.8:
	mov	x0, x23
	bl	mutex_unlock
	b	.LBB27_92
.LBB27_9:
	adrp	x19, g_recovery_pwron_info
	mov	w21, #1
	add	x19, x19, :lo12:g_recovery_pwron_info
	mov	x0, x23
	str	wzr, [x22, #584]
	str	w21, [x22, #588]
	str	wzr, [x19]
	bl	mutex_unlock
	adrp	x8, mas_blk_recovery_pwron_info_done
	add	x8, x8, :lo12:mas_blk_recovery_pwron_info_done
	mov	w9, #2
	str	xzr, [x19, #32]
	str	wzr, [x19, #4]
	str	x8, [x19, #64]
	strb	w9, [x19, #72]
	ldr	x8, [x22, #88]
	add	x1, x19, #8             // =8
	mov	x0, x20
	mov	w2, wzr
	blr	x8
	mov	w19, w0
	cbz	w0, .LBB27_11
// %bb.10:
	mov	x0, x23
	bl	mutex_lock
	mov	x0, x23
	str	w21, [x22, #584]
	str	wzr, [x22, #588]
	bl	mutex_unlock
.LBB27_11:
	adrp	x0, .L.str.25
	adrp	x1, .L__func__.mas_blk_recovery_pwron_info_sync
	add	x0, x0, :lo12:.L.str.25
	add	x1, x1, :lo12:.L__func__.mas_blk_recovery_pwron_info_sync
	mov	w2, w19
	bl	printk
	b	.LBB27_92
.LBB27_12:
	adrp	x8, g_recovery_pwron_info
	mov	x0, x23
	str	wzr, [x8, :lo12:g_recovery_pwron_info]
	bl	mutex_unlock
	cbnz	w24, .LBB27_39
// %bb.13:
	mov	x0, x20
	bl	mas_blk_get_lld
	cbz	x0, .LBB27_39
// %bb.14:
	ldr	w8, [x0, #248]
	cbz	w8, .LBB27_39
// %bb.15:
	ldr	w8, [x0, #252]
	cbz	w8, .LBB27_39
// %bb.16:
	str	x26, [sp, #72]          // 8-byte Folded Spill
	mov	x8, xzr
	add	x26, x0, #352           // =352
	add	x28, x0, #336           // =336
	mov	x9, x0
	str	x0, [sp, #88]           // 8-byte Folded Spill
.LBB27_17:                              // =>This Inner Loop Header: Depth=1
	mov	x0, x28
	str	x28, [sp, #56]          // 8-byte Folded Spill
	mov	x28, x8
	bl	_raw_spin_lock_irqsave
	ldr	x9, [sp, #88]           // 8-byte Folded Reload
	stp	x28, x0, [sp, #40]      // 16-byte Folded Spill
	ldr	x2, [x26, #32]
	ldr	x10, [x26]
	ldp	w9, w8, [x9, #248]
	add	x11, x21, x28, lsl #2
	ldr	w11, [x11, #12]
	ldr	x5, [x26, #64]
	add	x12, x28, #1            // =1
	mul	x8, x2, x8
	udiv	x6, x10, x9
	udiv	x3, x8, x9
	msub	x7, x6, x9, x10
	udiv	w10, w11, w9
	adrp	x0, .L.str.27
	mov	x28, x12
	msub	x4, x3, x9, x8
	msub	w8, w10, w9, w11
	add	x0, x0, :lo12:.L.str.27
	mov	w1, w28
	str	w10, [sp]
	str	w8, [sp, #8]
	bl	printk
	mov	w0, w28
	str	x28, [sp, #64]          // 8-byte Folded Spill
	bl	mas_blk_enable_disorder_stream
	tbz	w0, #0, .LBB27_19
// %bb.18:                              //   in Loop: Header=BB27_17 Depth=1
	ldr	x9, [sp, #88]           // 8-byte Folded Reload
	ldr	x8, [x26, #32]
	ldr	x10, [x26, #64]
	ldr	w9, [x9, #252]
	madd	x8, x8, x9, x10
	b	.LBB27_20
.LBB27_19:                              //   in Loop: Header=BB27_17 Depth=1
	ldr	x8, [x26]
.LBB27_20:                              //   in Loop: Header=BB27_17 Depth=1
	str	x8, [sp, #32]           // 8-byte Folded Spill
	ldr	x8, [sp, #40]           // 8-byte Folded Reload
	ldr	x0, [sp, #64]           // 8-byte Folded Reload
                                        // kill: def $w0 killed $w0 killed $x0
	add	x28, x21, x8, lsl #2
	ldr	w8, [x28, #12]
	str	x8, [x26]
	bl	mas_blk_enable_disorder_stream
	tbz	w0, #0, .LBB27_22
// %bb.21:                              //   in Loop: Header=BB27_17 Depth=1
	ldr	x9, [sp, #88]           // 8-byte Folded Reload
	ldr	w8, [x28, #12]
	ldr	w9, [x9, #252]
	udiv	w8, w8, w9
	str	x8, [x26, #32]
	ldr	w8, [x28, #12]
	udiv	w10, w8, w9
	msub	w8, w10, w9, w8
	str	x8, [x26, #64]
.LBB27_22:                              //   in Loop: Header=BB27_17 Depth=1
	bl	ktime_get
	ldr	x28, [sp, #64]          // 8-byte Folded Reload
	ldr	x1, [x26]
	str	x0, [x26, #96]
	ldr	x0, [sp, #88]           // 8-byte Folded Reload
	mov	w2, w28
	bl	mas_blk_recovery_update_section_list
	mov	w0, w28
	bl	mas_blk_enable_disorder_stream
	tbz	w0, #0, .LBB27_24
// %bb.23:                              //   in Loop: Header=BB27_17 Depth=1
	ldr	x11, [sp, #88]          // 8-byte Folded Reload
	ldr	x8, [x26, #32]
	ldr	x10, [x26, #64]
	ldr	x1, [sp, #48]           // 8-byte Folded Reload
	ldr	w9, [x11, #252]
	madd	x8, x8, x9, x10
	ldr	x10, [sp, #32]          // 8-byte Folded Reload
	b	.LBB27_25
.LBB27_24:                              //   in Loop: Header=BB27_17 Depth=1
	ldr	x8, [x26]
	ldr	x1, [sp, #48]           // 8-byte Folded Reload
	ldr	x10, [sp, #32]          // 8-byte Folded Reload
	ldr	x11, [sp, #88]          // 8-byte Folded Reload
.LBB27_25:                              //   in Loop: Header=BB27_17 Depth=1
	ldr	x9, [x11, #872]
	ldr	x28, [sp, #56]          // 8-byte Folded Reload
	sub	x8, x10, x8
	cmp	x9, x8
	csel	x8, x9, x8, hi
	mov	x0, x28
	str	x8, [x11, #872]
	bl	_raw_spin_unlock_irqrestore
	ldr	x8, [sp, #64]           // 8-byte Folded Reload
	add	x26, x26, #8            // =8
	add	x28, x28, #4            // =4
	cmp	x8, #4                  // =4
	b.ne	.LBB27_17
// %bb.26:
	ldr	x28, [sp, #88]          // 8-byte Folded Reload
	ldr	x21, [x28, #1944]
	add	x0, x21, #152           // =152
	str	x0, [sp, #32]           // 8-byte Folded Spill
	bl	_raw_spin_lock_irqsave
	add	x8, x21, #136           // =136
	mov	w21, #1
	str	x0, [sp, #24]           // 8-byte Folded Spill
	str	x8, [sp, #40]           // 8-byte Folded Spill
.LBB27_27:                              // =>This Loop Header: Depth=1
                                        //     Child Loop BB27_28 Depth 2
                                        //       Child Loop BB27_30 Depth 3
	add	x8, x28, x21, lsl #2
	add	x0, x8, #768            // =768
	str	x0, [sp, #64]           // 8-byte Folded Spill
	bl	_raw_spin_lock_irqsave
	add	x8, x28, x21, lsl #4
	ldr	x28, [x8, #688]
	add	x8, x8, #688            // =688
	stp	x8, x0, [sp, #48]       // 16-byte Folded Spill
	cmp	x28, x8
	b.eq	.LBB27_37
.LBB27_28:                              //   Parent Loop BB27_27 Depth=1
                                        // =>  This Loop Header: Depth=2
                                        //       Child Loop BB27_30 Depth 3
	sub	x26, x28, #392          // =392
	adrp	x2, g_recovery_pwron_info+8
	mov	x0, x26
	mov	x1, x20
	add	x2, x2, :lo12:g_recovery_pwron_info+8
	bl	mas_blk_bio_need_dispatch
	tbz	w0, #0, .LBB27_36
// %bb.29:                              //   in Loop: Header=BB27_28 Depth=2
	ldr	x9, [sp, #40]           // 8-byte Folded Reload
	ldr	x8, [x9]
	cmp	x8, x9
	b.eq	.LBB27_32
.LBB27_30:                              //   Parent Loop BB27_27 Depth=1
                                        //     Parent Loop BB27_28 Depth=2
                                        // =>    This Inner Loop Header: Depth=3
	sub	x9, x8, #408            // =408
	cmp	x9, x26
	b.eq	.LBB27_36
// %bb.31:                              //   in Loop: Header=BB27_30 Depth=3
	ldr	x8, [x8]
	ldr	x9, [sp, #40]           // 8-byte Folded Reload
	cmp	x9, x8
	b.ne	.LBB27_30
.LBB27_32:                              //   in Loop: Header=BB27_28 Depth=2
	mov	x0, x20
	bl	mas_blk_get_lld
	cbz	x0, .LBB27_36
// %bb.33:                              //   in Loop: Header=BB27_28 Depth=2
	ldr	x8, [x0, #1944]
	cbz	x8, .LBB27_36
// %bb.34:                              //   in Loop: Header=BB27_28 Depth=2
	ldr	x10, [x8, #144]
	add	x9, x28, #16            // =16
	add	x11, x8, #136           // =136
	str	x9, [x8, #144]
	stp	x11, x10, [x28, #16]
	str	x9, [x10]
	ldr	w8, [x0, #248]
	cbz	w8, .LBB27_36
// %bb.35:                              //   in Loop: Header=BB27_28 Depth=2
	sub	x9, x28, #360           // =360
	ldr	x9, [x9]
	sub	x10, x28, #352          // =352
	sub	x11, x28, #344          // =344
	ldr	w10, [x10]
	ldr	w11, [x11]
	ldurb	w1, [x28, #-120]
	lsr	x9, x9, #3
	udiv	x2, x9, x8
	adrp	x0, .L.str.28
	lsr	w4, w10, #12
	msub	x3, x2, x8, x9
	lsr	w5, w11, #12
	add	x0, x0, :lo12:.L.str.28
	bl	printk
.LBB27_36:                              //   in Loop: Header=BB27_28 Depth=2
	ldr	x28, [x28]
	ldr	x8, [sp, #48]           // 8-byte Folded Reload
	cmp	x8, x28
	b.ne	.LBB27_28
.LBB27_37:                              //   in Loop: Header=BB27_27 Depth=1
	ldp	x1, x0, [sp, #56]       // 16-byte Folded Reload
	bl	_raw_spin_unlock_irqrestore
	ldr	x28, [sp, #88]          // 8-byte Folded Reload
	add	x21, x21, #1            // =1
	cmp	x21, #5                 // =5
	b.ne	.LBB27_27
// %bb.38:
	ldp	x1, x0, [sp, #24]       // 16-byte Folded Reload
	bl	_raw_spin_unlock_irqrestore
	ldr	x26, [sp, #72]          // 8-byte Folded Reload
.LBB27_39:
	mov	x0, x23
	bl	mutex_lock
	mov	x0, x23
	str	w27, [x22, #584]
	str	wzr, [x22, #588]
	bl	mutex_unlock
	adrp	x0, .L.str.26
	adrp	x1, .L__func__.mas_blk_recovery_done_proc
	add	x0, x0, :lo12:.L.str.26
	add	x1, x1, :lo12:.L__func__.mas_blk_recovery_done_proc
	mov	w2, w24
	bl	printk
	mov	x27, x25
	cbnz	w24, .LBB27_92
.LBB27_40:
	mov	x0, x20
	stur	xzr, [x29, #-96]
	bl	mas_blk_get_lld
	cbz	x0, .LBB27_91
// %bb.41:
	ldr	x8, [x20, #1392]
	ldr	x8, [x8, #536]
	ldr	x9, [x8, #136]!
	cmp	x8, x9
	b.eq	.LBB27_77
// %bb.42:
	mov	x0, x20
	bl	mas_blk_get_lld
	cbz	x0, .LBB27_91
// %bb.43:
	ldr	w8, [x0, #252]
	cbz	w8, .LBB27_91
// %bb.44:
	ldr	x25, [x0, #1944]
	cbz	x25, .LBB27_91
// %bb.45:
	str	x0, [sp, #64]           // 8-byte Folded Spill
	add	x0, x25, #152           // =152
	str	x0, [sp, #56]           // 8-byte Folded Spill
	bl	_raw_spin_lock_irqsave
	add	x8, x25, #136           // =136
	mov	w23, #4096
	mov	w22, #1
	mov	w24, #680
	str	x0, [sp, #48]           // 8-byte Folded Spill
	stp	x27, x8, [sp, #80]      // 16-byte Folded Spill
	str	x26, [sp, #72]          // 8-byte Folded Spill
	b	.LBB27_49
.LBB27_46:                              //   in Loop: Header=BB27_49 Depth=1
	ldur	x0, [x29, #-32]
	mov	w1, #15
	bl	mas_blk_latency_req_check
	mov	w26, #9
.LBB27_47:                              //   in Loop: Header=BB27_49 Depth=1
	ldr	x27, [sp, #80]          // 8-byte Folded Reload
	mov	x0, x25
	bl	blk_mq_free_request
	cbnz	w26, .LBB27_85
.LBB27_48:                              //   in Loop: Header=BB27_49 Depth=1
	ldp	x9, x8, [x21]
	mov	x25, x22
	mov	w22, #1
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x21, [x21]
	str	x21, [x21, #8]
	ldp	x8, x26, [sp, #64]      // 16-byte Folded Reload
	ldr	w8, [x8, #248]
	cbnz	w8, .LBB27_76
.LBB27_49:                              // =>This Loop Header: Depth=1
                                        //     Child Loop BB27_56 Depth 2
                                        //       Child Loop BB27_62 Depth 3
	ldr	x8, [sp, #88]           // 8-byte Folded Reload
	ldr	x21, [x8]
	cmp	x21, x8
	b.ne	.LBB27_51
// %bb.50:                              //   in Loop: Header=BB27_49 Depth=1
	ldr	x8, [x25, #144]
	ldr	x9, [sp, #88]           // 8-byte Folded Reload
	cmp	x8, x9
	b.eq	.LBB27_90
.LBB27_51:                              //   in Loop: Header=BB27_49 Depth=1
	add	x8, x25, #32            // =32
	sub	x9, x21, #400           // =400
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	add	w10, w10, 1
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
	ldr	x8, [x9]
	sub	x27, x21, #408          // =408
	ldr	x26, [x8, #1272]
	mov	x8, #1099511627776
	stp	xzr, xzr, [x29, #-32]
	stp	xzr, xzr, [x29, #-48]
	stp	xzr, xzr, [x29, #-64]
	stur	xzr, [x29, #-88]
	stp	x8, xzr, [x29, #-80]
	ldr	w8, [x9, #8]
	sub	w9, w8, #3              // =3
	and	w10, w9, #0xff
	cmp	w10, #7                 // =7
	b.hs	.LBB27_53
// %bb.52:                              //   in Loop: Header=BB27_49 Depth=1
	mov	w10, #85
	lsr	w10, w10, w9
	tbnz	w10, #0, .LBB27_65
.LBB27_53:                              //   in Loop: Header=BB27_49 Depth=1
	sub	x9, x21, #368           // =368
	ldr	w9, [x9]
	cbz	w9, .LBB27_64
// %bb.54:                              //   in Loop: Header=BB27_49 Depth=1
	sub	x10, x21, #360          // =360
	sub	x12, x21, #364          // =364
	ldr	w11, [x10]
	ldr	w12, [x12]
	ldr	x10, [x21, #72]
	mov	w28, wzr
	b	.LBB27_56
.LBB27_55:                              //   in Loop: Header=BB27_56 Depth=2
	cbz	w9, .LBB27_66
.LBB27_56:                              //   Parent Loop BB27_49 Depth=1
                                        // =>  This Loop Header: Depth=2
                                        //       Child Loop BB27_62 Depth 3
	add	x13, x10, w12, uxtw #4
	ldp	w14, w15, [x13, #8]
	and	w16, w8, #0xff
	add	w28, w28, #1            // =1
	add	w15, w15, w11
	sub	w13, w14, w11
	cmp	w9, w13
	and	w15, w15, #0xfff
	csel	w17, w9, w13, lo
	sub	w15, w23, w15
	cmp	w17, w15
	csel	w15, w17, w15, lo
	cmp	w16, #9                 // =9
	b.hi	.LBB27_59
// %bb.57:                              //   in Loop: Header=BB27_56 Depth=2
	lsl	w16, w22, w16
	tst	w16, w24
	b.eq	.LBB27_59
// %bb.58:                              //   in Loop: Header=BB27_56 Depth=2
	sub	w9, w9, w15
	cbnz	w9, .LBB27_56
	b	.LBB27_66
.LBB27_59:                              //   in Loop: Header=BB27_56 Depth=2
	cbz	w15, .LBB27_55
// %bb.60:                              //   in Loop: Header=BB27_56 Depth=2
	cmp	w9, w15
	csel	w16, w15, w9, hi
	cmp	w16, w13
	csel	w16, w16, w13, lo
	add	w11, w16, w11
	cmp	w11, w14
	sub	w13, w15, w16
	sub	w9, w9, w16
	cinc	w14, w12, eq
	csel	w11, wzr, w11, eq
	cbz	w13, .LBB27_63
// %bb.61:                              //   in Loop: Header=BB27_56 Depth=2
	mov	w12, w14
.LBB27_62:                              //   Parent Loop BB27_49 Depth=1
                                        //     Parent Loop BB27_56 Depth=2
                                        // =>    This Inner Loop Header: Depth=3
	add	x15, x10, w14, uxtw #4
	ldr	w15, [x15, #8]
	cmp	w13, w9
	csel	w16, w13, w9, lo
	sub	w17, w15, w11
	cmp	w16, w17
	csel	w16, w16, w17, lo
	add	w11, w16, w11
	cmp	w11, w15
	sub	w13, w13, w16
	sub	w9, w9, w16
	csinc	w12, w12, w14, ne
	csel	w11, wzr, w11, eq
	cinc	w14, w14, eq
	cbnz	w13, .LBB27_62
	b	.LBB27_55
.LBB27_63:                              //   in Loop: Header=BB27_56 Depth=2
	mov	w12, w14
	cbnz	w9, .LBB27_56
	b	.LBB27_66
.LBB27_64:                              //   in Loop: Header=BB27_49 Depth=1
	mov	w28, wzr
	b	.LBB27_66
.LBB27_65:                              //   in Loop: Header=BB27_49 Depth=1
	adrp	x10, .Lswitch.table.ufs_mq_sync_dispatch
	sxtb	x9, w9
	add	x10, x10, :lo12:.Lswitch.table.ufs_mq_sync_dispatch
	ldr	w28, [x10, x9, lsl #2]
.LBB27_66:                              //   in Loop: Header=BB27_49 Depth=1
	sub	x2, x29, #88            // =88
	mov	x0, x26
	mov	x1, x27
	mov	x22, x25
	stur	w8, [x29, #-52]
	bl	blk_mq_get_request
	ldr	x9, [x0, #200]
	ldr	w8, [x0, #24]
	mov	w10, #61439
	movk	w10, #65471, lsl #16
	and	x9, x9, #0xffffffffffffff7f
	and	w10, w8, w10
	str	x9, [x0, #200]
	str	w10, [x0, #24]
	cbz	x0, .LBB27_84
// %bb.67:                              //   in Loop: Header=BB27_49 Depth=1
	and	w8, w8, #0xff
	mov	x25, x0
	cmp	w8, #1                  // =1
	b.eq	.LBB27_71
// %bb.68:                              //   in Loop: Header=BB27_49 Depth=1
	cmp	w8, #3                  // =3
	b.eq	.LBB27_71
// %bb.69:                              //   in Loop: Header=BB27_49 Depth=1
	cmp	w8, #2                  // =2
	b.ne	.LBB27_72
// %bb.70:                              //   in Loop: Header=BB27_49 Depth=1
	ldr	x0, [x25]
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB27_72
.LBB27_71:                              //   in Loop: Header=BB27_49 Depth=1
	mov	x0, x25
	bl	blk_req_set_make_req_nr
.LBB27_72:                              //   in Loop: Header=BB27_49 Depth=1
	mov	x0, x25
	mov	x1, x27
	mov	w2, w28
	bl	blk_mq_bio_to_request
	ldr	w8, [x25, #24]
	ldr	x9, [x25, #8]
	tst	w8, #0xff
	cset	w10, eq
	tst	w8, #0x8000000
	mov	w8, #2
	csel	x8, x10, x8, eq
	add	x8, x9, x8, lsl #3
	ldr	x27, [x8, #80]
	stur	x25, [x29, #-32]
	ldr	x8, [x27, #232]
	ldr	x8, [x8, #48]
	ldr	x8, [x8, #16]
	cbz	x8, .LBB27_74
// %bb.73:                              //   in Loop: Header=BB27_49 Depth=1
	mov	x0, x27
	blr	x8
	tbz	w0, #0, .LBB27_46
.LBB27_74:                              //   in Loop: Header=BB27_49 Depth=1
	ldr	x8, [x26, #48]
	sub	x1, x29, #32            // =32
	mov	x0, x27
	ldr	x8, [x8]
	blr	x8
	ldur	x8, [x29, #-32]
	mov	w26, w0
	mov	w1, #14
	mov	x0, x8
	bl	mas_blk_latency_req_check
	ands	w26, w26, #0xff
	b.ne	.LBB27_47
// %bb.75:                              //   in Loop: Header=BB27_49 Depth=1
	ldr	x27, [sp, #80]          // 8-byte Folded Reload
	b	.LBB27_48
.LBB27_76:                              //   in Loop: Header=BB27_49 Depth=1
	sub	x9, x21, #376           // =376
	ldr	x9, [x9]
	sub	x10, x21, #368          // =368
	ldr	w11, [x10]
	ldr	w10, [x10, #8]
	ldurb	w1, [x21, #-136]
	lsr	x9, x9, #3
	udiv	x2, x9, x8
	adrp	x0, .L.str.32
	lsr	w4, w11, #12
	msub	x3, x2, x8, x9
	lsr	w5, w10, #12
	add	x0, x0, :lo12:.L.str.32
	bl	printk
	b	.LBB27_49
.LBB27_77:
	ldr	x8, [x0, #216]
	mov	x22, x0
	sub	x1, x29, #96            // =96
	mov	x0, x20
	blr	x8
	bl	mas_blk_unistore_debug_en
	cbnz	w0, .LBB27_83
// %bb.78:
	ldur	x8, [x29, #-96]
	cbnz	x8, .LBB27_91
.LBB27_79:
	add	x23, x22, #648          // =648
	mov	x0, x23
	bl	mutex_lock
	ldr	w8, [x22, #580]
	cbz	w8, .LBB27_81
// %bb.80:
	add	x0, x22, #592           // =592
	str	wzr, [x22, #580]
	bl	up_write
.LBB27_81:
	mov	x0, x23
	bl	mutex_unlock
	bl	mas_blk_recovery_debug_on
	cmp	w0, #3                  // =3
	b.ne	.LBB27_91
// %bb.82:
	adrp	x0, .L.str.31
	add	x0, x0, :lo12:.L.str.31
	bl	mas_blk_rdr_panic
.LBB27_83:
	ldur	x2, [x29, #-96]
	adrp	x0, .L.str.30
	adrp	x1, .L__func__.ufs_mq_recovery_dispatch
	add	x0, x0, :lo12:.L.str.30
	add	x1, x1, :lo12:.L__func__.ufs_mq_recovery_dispatch
	bl	printk
	ldur	x8, [x29, #-96]
	cbnz	x8, .LBB27_91
	b	.LBB27_79
.LBB27_84:
	ldr	x27, [sp, #80]          // 8-byte Folded Reload
.LBB27_85:
	add	x8, x22, #32            // =32
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
	ldr	x21, [x22, #136]
	ldr	x8, [sp, #88]           // 8-byte Folded Reload
	ldp	x22, x26, [sp, #64]     // 16-byte Folded Reload
	cmp	x8, x21
	b.ne	.LBB27_87
	b	.LBB27_90
.LBB27_86:                              //   in Loop: Header=BB27_87 Depth=1
	ldr	x21, [x21]
	ldr	x8, [sp, #88]           // 8-byte Folded Reload
	cmp	x8, x21
	b.eq	.LBB27_90
.LBB27_87:                              // =>This Inner Loop Header: Depth=1
	ldurb	w25, [x21, #-136]
	sub	w8, w25, #1             // =1
	and	w8, w8, #0xff
	cmp	w8, #3                  // =3
	b.hi	.LBB27_86
// %bb.88:                              //   in Loop: Header=BB27_87 Depth=1
	sub	x8, x21, #376           // =376
	ldr	x8, [x8]
	add	x9, x22, x25, lsl #2
	add	x27, x9, #332           // =332
	mov	x0, x27
	lsr	x26, x8, #3
	bl	_raw_spin_lock_irqsave
	mov	x2, x26
	ldr	x26, [sp, #72]          // 8-byte Folded Reload
	mov	x28, x0
	mov	x0, x22
	mov	w1, w25
	bl	mas_blk_expected_lba_pu
	mov	w25, w0
	mov	x0, x27
	ldr	x27, [sp, #80]          // 8-byte Folded Reload
	mov	x1, x28
	bl	_raw_spin_unlock_irqrestore
	tbz	w25, #0, .LBB27_86
// %bb.89:
	ldp	x9, x8, [x21]
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x21, [x21]
	ldr	x9, [sp, #88]           // 8-byte Folded Reload
	str	x21, [x21, #8]
	ldr	x8, [x9]
	str	x21, [x8, #8]
	stp	x8, x9, [x21]
	str	x21, [x9]
.LBB27_90:
	ldp	x1, x0, [sp, #48]       // 16-byte Folded Reload
	bl	_raw_spin_unlock_irqrestore
.LBB27_91:
	ldr	w8, [x26, #580]
	cbz	w8, .LBB27_93
.LBB27_92:
	ldr	x19, [x20, #1392]
	adrp	x8, mas_blk_mq_sync_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_sync_disp_wq]
	mov	w0, #8
	add	x2, x19, #120           // =120
	mov	w3, #1
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #8]
	ldr	w8, [x19, #88]
	add	w8, w8, #1              // =1
	str	w8, [x19, #88]
	b	.LBB27_110
.LBB27_93:
	bl	ktime_get
	add	x21, x19, #48           // =48
	add	x24, x19, #56           // =56
	add	x28, x19, #80           // =80
	mov	w25, #1
	mov	w26, #2
	str	x0, [x27, #96]
.LBB27_94:                              // =>This Inner Loop Header: Depth=1
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x24]
	mov	x1, x0
	cmp	x8, x24
	b.eq	.LBB27_96
.LBB27_95:                              //   in Loop: Header=BB27_94 Depth=1
	add	x8, x19, #72            // =72
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	sub	w9, w9, w25
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	mov	x8, x24
	b	.LBB27_100
.LBB27_96:                              //   in Loop: Header=BB27_94 Depth=1
	ldr	x8, [x19, #64]
	cmp	x8, x24
	b.ne	.LBB27_95
// %bb.97:                              //   in Loop: Header=BB27_94 Depth=1
	ldr	x8, [x28]
	cmp	x8, x28
	b.ne	.LBB27_99
// %bb.98:                              //   in Loop: Header=BB27_94 Depth=1
	ldr	x8, [x19, #88]
	cmp	x8, x28
	b.eq	.LBB27_108
.LBB27_99:                              //   in Loop: Header=BB27_94 Depth=1
	add	x8, x19, #96            // =96
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	sub	w9, w9, w25
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	mov	x8, x28
.LBB27_100:                             //   in Loop: Header=BB27_94 Depth=1
	ldr	x23, [x8]
	subs	x22, x23, #80           // =80
	b.eq	.LBB27_108
// %bb.101:                             //   in Loop: Header=BB27_94 Depth=1
	ldp	x9, x8, [x23]
	ldur	x20, [x23, #-80]
	mov	x0, x21
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x23, [x23]
	str	x23, [x23, #8]
	bl	_raw_spin_unlock_irqrestore
	ldur	w8, [x23, #-56]
	ldur	x9, [x23, #-72]
	tst	w8, #0xff
	cset	w10, eq
	tst	w8, #0x8000000
	csel	x10, x10, x26, eq
	add	x9, x9, x10, lsl #3
	ldr	x23, [x9, #80]
	and	w8, w8, #0xfc
	orr	w8, w8, #0x3
	cmp	w8, #35                 // =35
	str	x22, [sp, #96]
	b.eq	.LBB27_105
// %bb.102:                             //   in Loop: Header=BB27_94 Depth=1
	add	x3, sp, #96             // =96
	mov	x0, x22
	mov	x1, x23
	mov	x2, x19
	mov	x4, x20
	bl	__ufs_mq_queue_rq
	cbz	w0, .LBB27_94
.LBB27_103:                             //   in Loop: Header=BB27_94 Depth=1
	cmp	w0, #9                  // =9
	b.eq	.LBB27_113
// %bb.104:                             //   in Loop: Header=BB27_94 Depth=1
	mov	w1, #10
	mov	x0, x22
	bl	blk_mq_end_request
	b	.LBB27_94
.LBB27_105:                             //   in Loop: Header=BB27_94 Depth=1
	ldr	x8, [x23, #232]
	ldr	x8, [x8, #48]
	ldr	x8, [x8, #16]
	cbz	x8, .LBB27_107
// %bb.106:                             //   in Loop: Header=BB27_94 Depth=1
	mov	x0, x23
	blr	x8
	tbz	w0, #0, .LBB27_112
.LBB27_107:                             //   in Loop: Header=BB27_94 Depth=1
	ldr	x8, [x20, #48]
	add	x1, sp, #96             // =96
	mov	x0, x23
	ldr	x8, [x8]
	blr	x8
	ldr	x8, [sp, #96]
	mov	w23, w0
	mov	w1, #14
	mov	x0, x8
	bl	mas_blk_latency_req_check
	and	w0, w23, #0xff
	cbz	w0, .LBB27_94
	b	.LBB27_103
.LBB27_108:
	mov	x0, x21
	bl	_raw_spin_unlock_irqrestore
.LBB27_109:
	ldr	w8, [x27, #116]
	add	w8, w8, #1              // =1
	str	w8, [x27, #116]
	bl	ktime_get
	str	x0, [x27, #104]
.LBB27_110:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-16]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB27_141
// %bb.111:
	ldp	x20, x19, [sp, #288]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #272]    // 16-byte Folded Reload
	ldp	x24, x23, [sp, #256]    // 16-byte Folded Reload
	ldp	x26, x25, [sp, #240]    // 16-byte Folded Reload
	ldp	x28, x27, [sp, #224]    // 16-byte Folded Reload
	ldp	x29, x30, [sp, #208]    // 16-byte Folded Reload
	add	sp, sp, #304            // =304
	ret
.LBB27_112:
	ldr	x0, [sp, #96]
	mov	w1, #15
	bl	mas_blk_latency_req_check
.LBB27_113:
	mov	x0, x22
	bl	__blk_mq_requeue_request
	mov	x0, x22
	mov	x1, x20
	bl	ufs_mq_insert_sync_list
	ldr	x8, [x19]
	mov	x0, x20
	str	x8, [sp, #88]           // 8-byte Folded Spill
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB27_135
// %bb.114:
	ldr	x8, [x28]
	cmp	x8, x28
	b.ne	.LBB27_116
// %bb.115:
	ldr	x8, [x19, #88]
	cmp	x8, x28
	b.eq	.LBB27_135
.LBB27_116:
	//APP
	.if 1 == 1
661:
	mrs	x22, daif
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
	mrs_s x22, (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5))
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
	and	w8, w22, #0x00000080
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
	eor	w8, w22, #0xe0
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbnz	w8, .LBB27_118
// %bb.117:
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
.LBB27_118:
	mov	x0, x21
	bl	_raw_spin_trylock
	cbz	w0, .LBB27_134
// %bb.119:
	ldr	x8, [sp, #88]           // 8-byte Folded Reload
	ldp	w2, w3, [x8, #248]
	cbz	w2, .LBB27_140
// %bb.120:
	cbz	w3, .LBB27_140
// %bb.121:
	ldr	x23, [x28]
	str	x22, [sp, #72]          // 8-byte Folded Spill
	cmp	x28, x23
	b.eq	.LBB27_132
// %bb.122:
	ldr	x22, [sp, #88]          // 8-byte Folded Reload
	str	x27, [sp, #80]          // 8-byte Folded Spill
	b	.LBB27_124
.LBB27_123:                             //   in Loop: Header=BB27_124 Depth=1
	ldr	x23, [x23]
	cmp	x28, x23
	b.eq	.LBB27_132
.LBB27_124:                             // =>This Inner Loop Header: Depth=1
	ldrb	w24, [x23, #624]
	sub	w8, w24, #1             // =1
	and	w8, w8, #0xff
	cmp	w8, #3                  // =3
	b.hi	.LBB27_123
// %bb.125:                             //   in Loop: Header=BB27_124 Depth=1
	ldurb	w8, [x23, #-56]
	cmp	w8, #1                  // =1
	b.ne	.LBB27_123
// %bb.126:                             //   in Loop: Header=BB27_124 Depth=1
	ldur	x8, [x23, #-24]
	add	x9, x22, x24, lsl #2
	add	x26, x9, #332           // =332
	mov	x0, x26
	lsr	x25, x8, #3
	bl	_raw_spin_lock_irqsave
	mov	x27, x0
	mov	x0, x22
	mov	w1, w24
	mov	x2, x25
	bl	mas_blk_expected_lba_pu
	mov	x1, x27
	ldr	x27, [sp, #80]          // 8-byte Folded Reload
	mov	w24, w0
	mov	x0, x26
	bl	_raw_spin_unlock_irqrestore
	tbz	w24, #0, .LBB27_123
// %bb.127:
	ldp	x9, x8, [x23]
	mov	x0, x21
	sub	x24, x23, #80           // =80
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x23, [x23]
	ldr	x1, [sp, #72]           // 8-byte Folded Reload
	str	x23, [x23, #8]
	bl	_raw_spin_unlock_irqrestore
	stp	xzr, xzr, [x29, #-88]
	ldur	w8, [x23, #-56]
	ldur	x9, [x23, #-72]
	tst	w8, #0xff
	cset	w10, eq
	tst	w8, #0x8000000
	mov	w8, #2
	csel	x8, x10, x8, eq
	add	x8, x9, x8, lsl #3
	ldr	x21, [x8, #80]
	stur	x24, [x29, #-88]
	bl	mas_blk_recovery_debug_on
	cmp	w0, #1                  // =1
	b.ne	.LBB27_129
// %bb.128:
	ldrb	w8, [x23, #624]
	sub	w8, w8, #1              // =1
	cmp	w8, #4                  // =4
	b.lo	.LBB27_131
.LBB27_129:
	ldur	w8, [x23, #-56]
	and	w8, w8, #0xfc
	orr	w8, w8, #0x3
	cmp	w8, #35                 // =35
	b.eq	.LBB27_142
// %bb.130:
	sub	x3, x29, #88            // =88
	mov	x0, x24
	mov	x1, x21
	mov	x2, x19
	mov	x4, x20
	bl	__ufs_mq_queue_rq
	cmp	w0, #9                  // =9
	b.ne	.LBB27_135
.LBB27_131:
	mov	x0, x24
	bl	__blk_mq_requeue_request
	mov	x0, x24
	mov	x1, x20
	bl	ufs_mq_insert_sync_list
	b	.LBB27_135
.LBB27_132:
	ldr	x1, [sp, #72]           // 8-byte Folded Reload
	mov	x0, x21
.LBB27_133:
	bl	_raw_spin_unlock_irqrestore
	b	.LBB27_135
.LBB27_134:
	//APP
	.if 1 == 1
661:
	msr	daif, x22
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
	msr_s (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5)), x22
	.purgem	msr_s
dsb	sy
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
.LBB27_135:
	mov	w21, #37888
	movk	w21, #30517, lsl #16
	bl	ktime_get
	ldr	x8, [x19, #184]
	add	x8, x8, x21
	cmp	x8, x0
	b.ge	.LBB27_138
// %bb.136:
	bl	ktime_get
	ldr	x8, [x19, #192]
	add	x8, x8, x21
	cmp	x8, x0
	b.ge	.LBB27_138
// %bb.137:
	mov	w3, #1
	b	.LBB27_139
.LBB27_138:
	mov	x3, xzr
.LBB27_139:
	ldr	x19, [x20, #1392]
	adrp	x8, mas_blk_mq_sync_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_sync_disp_wq]
	mov	w0, #8
	add	x2, x19, #120           // =120
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #8]
	ldr	w8, [x19, #88]
	add	w8, w8, #1              // =1
	str	w8, [x19, #88]
	ldr	w8, [x27, #112]
	add	w8, w8, #1              // =1
	str	w8, [x27, #112]
	b	.LBB27_109
.LBB27_140:
	adrp	x0, .L.str.34
	adrp	x1, .L__func__.ufs_mq_dispatch_match_expected_lba
	add	x0, x0, :lo12:.L.str.34
	add	x1, x1, :lo12:.L__func__.ufs_mq_dispatch_match_expected_lba
	bl	printk
	mov	x0, x21
	mov	x1, x22
	b	.LBB27_133
.LBB27_141:
	bl	__stack_chk_fail
.LBB27_142:
	ldr	x8, [x21, #232]
	ldr	x8, [x8, #48]
	ldr	x8, [x8, #16]
	cbz	x8, .LBB27_144
// %bb.143:
	mov	x0, x21
	blr	x8
	tbz	w0, #0, .LBB27_145
.LBB27_144:
	ldr	x8, [x20, #48]
	sub	x1, x29, #88            // =88
	mov	x0, x21
	ldr	x8, [x8]
	blr	x8
	ldur	x8, [x29, #-88]
	mov	w21, w0
	mov	w1, #14
	mov	x0, x8
	bl	mas_blk_latency_req_check
	ldr	x27, [sp, #80]          // 8-byte Folded Reload
	and	w0, w21, #0xff
	cmp	w0, #9                  // =9
	b.eq	.LBB27_131
	b	.LBB27_135
.LBB27_145:
	ldur	x0, [x29, #-88]
	mov	w1, #15
	bl	mas_blk_latency_req_check
	ldr	x27, [sp, #80]          // 8-byte Folded Reload
	b	.LBB27_131
.Lfunc_end27:
	.size	ufs_mq_sync_dispatch, .Lfunc_end27-ufs_mq_sync_dispatch
                                        // -- End function
	.globl	ufs_mq_write_throttle_check_timer_expire // -- Begin function ufs_mq_write_throttle_check_timer_expire
	.p2align	2
	.type	ufs_mq_write_throttle_check_timer_expire,@function
ufs_mq_write_throttle_check_timer_expire: // @ufs_mq_write_throttle_check_timer_expire
.Lufs_mq_write_throttle_check_timer_expire$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	add	x20, x0, #328           // =328
	mov	x19, x0
	mov	x0, x20
	mov	x29, sp
	bl	_raw_spin_lock_irqsave
	mov	x1, x0
	mov	w8, #1
	mov	x0, x20
	strb	w8, [x19, #332]
	bl	_raw_spin_unlock_irqrestore
	mov	x0, x19
	bl	ufs_mq_async_disp_inflt_lmt_decision
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end28:
	.size	ufs_mq_write_throttle_check_timer_expire, .Lfunc_end28-ufs_mq_write_throttle_check_timer_expire
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_async_disp_inflt_lmt_decision
	.type	ufs_mq_async_disp_inflt_lmt_decision,@function
ufs_mq_async_disp_inflt_lmt_decision:   // @ufs_mq_async_disp_inflt_lmt_decision
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x0]
	str	x21, [sp, #16]          // 8-byte Folded Spill
	str	wzr, [x0, #268]
	mov	x29, sp
	ldr	w8, [x8, #1980]
	ldr	w9, [x0, #20]
	ldr	w10, [x0, #264]
	cmp	w10, #3                 // =3
	b.hi	.LBB29_14
// %bb.1:
	adrp	x11, .LJTI29_0
	add	x11, x11, :lo12:.LJTI29_0
	adr	x12, .LBB29_2
	ldrb	w13, [x11, x10]
	add	x12, x12, x13, lsl #2
	mov	x19, x0
	br	x12
.LBB29_2:
	ldr	w8, [x19, #160]
	cmp	w8, #14                 // =14
	b.eq	.LBB29_13
// %bb.3:
	add	x20, x19, #328          // =328
	mov	x0, x20
	bl	_raw_spin_lock_irqsave
	ldrb	w8, [x19, #332]
	mov	x1, x0
	cbz	w8, .LBB29_12
// %bb.4:
	mov	w8, #14
	b	.LBB29_11
.LBB29_5:
	sub	w8, w8, w9
	ldr	w9, [x19, #160]
	cmp	w8, #4                  // =4
	mov	w8, #14
	mov	w10, #16
	csel	w21, w10, w8, lo
	cmp	w9, w21
	b.eq	.LBB29_13
// %bb.6:
	add	x20, x19, #328          // =328
	mov	x0, x20
	bl	_raw_spin_lock_irqsave
	ldrb	w8, [x19, #332]
	mov	x1, x0
	cbz	w8, .LBB29_12
// %bb.7:
	str	w21, [x19, #160]
	b	.LBB29_12
.LBB29_8:
	ldr	w8, [x19, #160]
	cmp	w8, #10                 // =10
	b.eq	.LBB29_13
// %bb.9:
	add	x20, x19, #328          // =328
	mov	x0, x20
	bl	_raw_spin_lock_irqsave
	ldrb	w8, [x19, #332]
	mov	x1, x0
	cbz	w8, .LBB29_12
// %bb.10:
	mov	w8, #10
.LBB29_11:
	str	w8, [x19, #160]
.LBB29_12:
	mov	x0, x20
	bl	_raw_spin_unlock_irqrestore
	bl	ktime_get
	str	x0, [x19, #200]
.LBB29_13:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB29_14:
	adrp	x0, .L.str.36
	add	x0, x0, :lo12:.L.str.36
	bl	mas_blk_rdr_panic
.Lfunc_end29:
	.size	ufs_mq_async_disp_inflt_lmt_decision, .Lfunc_end29-ufs_mq_async_disp_inflt_lmt_decision
	.section	.rodata,"a",@progbits
.LJTI29_0:
	.byte	(.LBB29_5-.LBB29_2)>>2
	.byte	(.LBB29_2-.LBB29_2)>>2
	.byte	(.LBB29_8-.LBB29_2)>>2
	.byte	(.LBB29_2-.LBB29_2)>>2
                                        // -- End function
	.text
	.globl	ufs_mq_write_throttle_handler // -- Begin function ufs_mq_write_throttle_handler
	.p2align	2
	.type	ufs_mq_write_throttle_handler,@function
ufs_mq_write_throttle_handler:          // @ufs_mq_write_throttle_handler
.Lufs_mq_write_throttle_handler$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x0, #1392]
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	w19, w1
	ldr	x21, [x8, #536]
	add	x20, x21, #328          // =328
	mov	x0, x20
	bl	_raw_spin_lock_irqsave
	tst	w19, #0x1
	mov	w8, #5
	mov	w9, #20
	mov	x1, x0
	csel	w8, w9, w8, ne
	mov	x0, x20
	strb	wzr, [x21, #332]
	str	w8, [x21, #160]
	bl	_raw_spin_unlock_irqrestore
	adrp	x8, jiffies
	ldr	x8, [x8, :lo12:jiffies]
	add	x0, x21, #272           // =272
	add	x1, x8, #250            // =250
	bl	mod_timer
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end30:
	.size	ufs_mq_write_throttle_handler, .Lfunc_end30-ufs_mq_write_throttle_handler
                                        // -- End function
	.globl	ufs_mq_async_io_dispatch_work_fn // -- Begin function ufs_mq_async_io_dispatch_work_fn
	.p2align	2
	.type	ufs_mq_async_io_dispatch_work_fn,@function
ufs_mq_async_io_dispatch_work_fn:       // @ufs_mq_async_io_dispatch_work_fn
.Lufs_mq_async_io_dispatch_work_fn$local:
// %bb.0:
	sub	sp, sp, #128            // =128
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #32            // =32
	str	x27, [sp, #48]          // 8-byte Folded Spill
	stp	x26, x25, [sp, #64]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #80]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #96]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #112]    // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	ldr	x25, [x0, #136]
	stp	xzr, xzr, [sp, #8]
	ldr	x24, [x25, #1392]
	ldr	x19, [x24, #536]
	bl	ktime_get
	str	x0, [x24, #352]
	mov	x0, x19
	bl	ufs_mq_async_disp_inflt_lmt_decision
	ldr	x8, [x25, #1400]
	cbz	x8, .LBB31_14
// %bb.1:
	mov	w26, #1
	mov	w27, #2
.LBB31_2:                               // =>This Inner Loop Header: Depth=1
	ldr	x20, [x8, #176]
	cbz	x20, .LBB31_14
// %bb.3:                               //   in Loop: Header=BB31_2 Depth=1
	ldr	x8, [x20]
	ldr	x8, [x8, #24]
	cbz	x8, .LBB31_14
// %bb.4:                               //   in Loop: Header=BB31_2 Depth=1
	ldr	x8, [x25, #1392]
	ldr	x22, [x8, #536]
	add	x21, x22, #100          // =100
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x20]
	mov	x23, x0
	mov	x0, x22
	ldr	x8, [x8, #24]
	blr	x8
	cbz	x0, .LBB31_13
// %bb.5:                               //   in Loop: Header=BB31_2 Depth=1
	add	x8, x22, #128           // =128
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	sub	w9, w9, w26
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	mov	x20, x0
	mov	x0, x21
	mov	x1, x23
	bl	_raw_spin_unlock_irqrestore
	ldr	w8, [x20, #24]
	ldp	x21, x9, [x20]
	tst	w8, #0xff
	cset	w10, eq
	tst	w8, #0x8000000
	csel	x10, x10, x27, eq
	add	x9, x9, x10, lsl #3
	ldr	x22, [x9, #80]
	and	w8, w8, #0xfc
	orr	w8, w8, #0x3
	cmp	w8, #35                 // =35
	str	x20, [sp, #8]
	b.eq	.LBB31_8
// %bb.6:                               //   in Loop: Header=BB31_2 Depth=1
	add	x3, sp, #8              // =8
	mov	x0, x20
	mov	x1, x22
	mov	x2, x19
	mov	x4, x21
	bl	__ufs_mq_queue_rq
	cbnz	w0, .LBB31_11
.LBB31_7:                               //   in Loop: Header=BB31_2 Depth=1
	mov	x0, x19
	bl	ufs_mq_async_disp_inflt_lmt_decision
	ldr	x8, [x25, #1400]
	cbnz	x8, .LBB31_2
	b	.LBB31_14
.LBB31_8:                               //   in Loop: Header=BB31_2 Depth=1
	ldr	x8, [x22, #232]
	ldr	x8, [x8, #48]
	ldr	x8, [x8, #16]
	cbz	x8, .LBB31_10
// %bb.9:                               //   in Loop: Header=BB31_2 Depth=1
	mov	x0, x22
	blr	x8
	tbz	w0, #0, .LBB31_16
.LBB31_10:                              //   in Loop: Header=BB31_2 Depth=1
	ldr	x8, [x21, #48]
	add	x1, sp, #8              // =8
	mov	x0, x22
	ldr	x8, [x8]
	blr	x8
	ldr	x8, [sp, #8]
	mov	w22, w0
	mov	w1, #14
	mov	x0, x8
	bl	mas_blk_latency_req_check
	and	w0, w22, #0xff
	cbz	w0, .LBB31_7
.LBB31_11:                              //   in Loop: Header=BB31_2 Depth=1
	cmp	w0, #9                  // =9
	b.eq	.LBB31_17
// %bb.12:                              //   in Loop: Header=BB31_2 Depth=1
	mov	w1, #10
	mov	x0, x20
	bl	blk_mq_end_request
	b	.LBB31_7
.LBB31_13:
	mov	x0, x21
	mov	x1, x23
	bl	_raw_spin_unlock_irqrestore
.LBB31_14:
	bl	ktime_get
	str	x0, [x24, #360]
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB31_22
// %bb.15:
	ldp	x20, x19, [sp, #112]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #80]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #64]     // 16-byte Folded Reload
	ldr	x27, [sp, #48]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #32]     // 16-byte Folded Reload
	add	sp, sp, #128            // =128
	ret
.LBB31_16:
	ldr	x0, [sp, #8]
	mov	w1, #15
	bl	mas_blk_latency_req_check
.LBB31_17:
	mov	x0, x20
	bl	__blk_mq_requeue_request
	ldr	x8, [x21, #1400]
	ldr	x19, [x21, #1392]
	cbz	x8, .LBB31_21
// %bb.18:
	ldr	x23, [x8, #176]
	cbz	x23, .LBB31_21
// %bb.19:
	ldr	x8, [x23]
	ldr	x8, [x8, #32]
	cbz	x8, .LBB31_21
// %bb.20:
	ldr	x19, [x19, #536]
	mov	w1, #13
	mov	x0, x20
	bl	mas_blk_latency_req_check
	add	x22, x19, #100          // =100
	mov	x0, x22
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x23]
	mov	x23, x0
	mov	x0, x20
	mov	x1, x19
	ldr	x8, [x8, #32]
	blr	x8
	add	x8, x19, #128           // =128
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	mov	x0, x22
	mov	x1, x23
	bl	_raw_spin_unlock_irqrestore
	ldr	x19, [x21, #1392]
.LBB31_21:
	adrp	x8, mas_blk_mq_async_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_async_disp_wq]
	add	x2, x19, #376           // =376
	mov	w0, #8
	mov	w3, #1
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #264]
	ldr	w8, [x19, #344]
	add	w8, w8, #1              // =1
	str	w8, [x19, #344]
	b	.LBB31_14
.LBB31_22:
	bl	__stack_chk_fail
.Lfunc_end31:
	.size	ufs_mq_async_io_dispatch_work_fn, .Lfunc_end31-ufs_mq_async_io_dispatch_work_fn
                                        // -- End function
	.globl	__ufs_mq_complete_request_remote // -- Begin function __ufs_mq_complete_request_remote
	.p2align	2
	.type	__ufs_mq_complete_request_remote,@function
__ufs_mq_complete_request_remote:       // @__ufs_mq_complete_request_remote
.L__ufs_mq_complete_request_remote$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	mov	w1, #30
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_latency_req_check
	ldr	x8, [x19]
	mov	x0, x19
	ldr	x8, [x8, #48]
	ldr	x8, [x8, #48]
	blr	x8
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end32:
	.size	__ufs_mq_complete_request_remote, .Lfunc_end32-__ufs_mq_complete_request_remote
                                        // -- End function
	.globl	ufs_mq_io_guard_work_fn // -- Begin function ufs_mq_io_guard_work_fn
	.p2align	2
	.type	ufs_mq_io_guard_work_fn,@function
ufs_mq_io_guard_work_fn:                // @ufs_mq_io_guard_work_fn
.Lufs_mq_io_guard_work_fn$local:
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	adrp	x19, io_guard_queue_list_lock
	add	x19, x19, :lo12:io_guard_queue_list_lock
	mov	x0, x19
	stp	x26, x25, [sp, #16]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	bl	_raw_spin_lock
	adrp	x20, mas_io_guard_queue_list
	add	x20, x20, :lo12:mas_io_guard_queue_list
	ldr	x21, [x20]
	mov	x0, x19
	bl	_raw_spin_unlock
	cmp	x21, x20
	b.eq	.LBB33_9
// %bb.1:
	adrp	x19, io_guard_queue_list_lock
	mov	w23, #26215
	add	x19, x19, :lo12:io_guard_queue_list_lock
	adrp	x22, mas_blk_mq_async_disp_wq
	movk	w23, #26214, lsl #16
	mov	w24, #10
	b	.LBB33_3
.LBB33_2:                               //   in Loop: Header=BB33_3 Depth=1
	mov	x0, x19
	bl	_raw_spin_lock
	ldr	x21, [x21]
	mov	x0, x19
	bl	_raw_spin_unlock
	cmp	x21, x20
	b.eq	.LBB33_9
.LBB33_3:                               // =>This Inner Loop Header: Depth=1
	ldr	x25, [x21, #16]
	sub	x26, x21, #520          // =520
	ldr	w8, [x25, #24]
	ldr	w9, [x25, #16]
	cmn	w8, w9
	b.eq	.LBB33_6
// %bb.4:                               //   in Loop: Header=BB33_3 Depth=1
	ldr	w8, [x25, #44]
	ldr	w9, [x25, #40]
	ldr	w10, [x25, #32]
	add	w8, w9, w8
	cmn	w8, w10
	b.ne	.LBB33_6
// %bb.5:                               //   in Loop: Header=BB33_3 Depth=1
	ldr	x0, [x26]
	bl	ufs_mq_sync_dispatch
.LBB33_6:                               //   in Loop: Header=BB33_3 Depth=1
	ldr	w8, [x25, #20]
	cbz	w8, .LBB33_2
// %bb.7:                               //   in Loop: Header=BB33_3 Depth=1
	ldr	w8, [x25, #36]
	cbnz	w8, .LBB33_2
// %bb.8:                               //   in Loop: Header=BB33_3 Depth=1
	ldr	x8, [x26]
	ldr	x1, [x22, :lo12:mas_blk_mq_async_disp_wq]
	mov	w0, #8
	mov	x3, xzr
	ldr	x25, [x8, #1392]
	add	x2, x25, #376           // =376
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x25, #344]
	mul	x9, x8, x23
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	msub	w8, w9, w24, w8
	add	x8, x25, w8, sxtw #3
	str	x0, [x8, #264]
	ldr	w8, [x25, #344]
	add	w8, w8, #1              // =1
	str	w8, [x25, #344]
	b	.LBB33_2
.LBB33_9:
	adrp	x8, mas_blk_io_guard_wq
	ldr	x1, [x8, :lo12:mas_blk_io_guard_wq]
	adrp	x2, mas_io_guard_work
	add	x2, x2, :lo12:mas_io_guard_work
	mov	w0, #8
	mov	w3, #1250
	bl	queue_delayed_work_on
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.Lfunc_end33:
	.size	ufs_mq_io_guard_work_fn, .Lfunc_end33-ufs_mq_io_guard_work_fn
                                        // -- End function
	.globl	ufs_mq_req_alloc_prep   // -- Begin function ufs_mq_req_alloc_prep
	.p2align	2
	.type	ufs_mq_req_alloc_prep,@function
ufs_mq_req_alloc_prep:                  // @ufs_mq_req_alloc_prep
.Lufs_mq_req_alloc_prep$local:
// %bb.0:
	orr	w8, w1, #0x800
	tst	w2, #0x1
	csel	x8, x1, x8, ne
	str	w8, [x0, #36]
	ret
.Lfunc_end34:
	.size	ufs_mq_req_alloc_prep, .Lfunc_end34-ufs_mq_req_alloc_prep
                                        // -- End function
	.globl	ufs_mq_req_init         // -- Begin function ufs_mq_req_init
	.p2align	2
	.type	ufs_mq_req_init,@function
ufs_mq_req_init:                        // @ufs_mq_req_init
.Lufs_mq_req_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	w8, [x1, #760]
	mov	x29, sp
	cbnz	w8, .LBB35_2
// %bb.1:
	mov	w8, #1
	str	w8, [x1, #760]
	str	x0, [x1, #272]
	mov	x0, x1
	str	xzr, [x1, #240]
	bl	mas_blk_request_init_unistore
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB35_2:
	adrp	x0, .L.str.11
	add	x0, x0, :lo12:.L.str.11
	bl	mas_blk_rdr_panic
.Lfunc_end35:
	.size	ufs_mq_req_init, .Lfunc_end35-ufs_mq_req_init
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_rdr_panic
	.type	mas_blk_rdr_panic,@function
mas_blk_rdr_panic:                      // @mas_blk_rdr_panic
// %bb.0:
	cbz	x0, .LBB36_2
// %bb.1:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x1, x0
	adrp	x0, .L.str.37
	add	x0, x0, :lo12:.L.str.37
	mov	x29, sp
	bl	printk
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
.LBB36_2:
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.Lfunc_end36:
	.size	mas_blk_rdr_panic, .Lfunc_end36-mas_blk_rdr_panic
                                        // -- End function
	.globl	ufs_mq_req_complete     // -- Begin function ufs_mq_req_complete
	.p2align	2
	.type	ufs_mq_req_complete,@function
ufs_mq_req_complete:                    // @ufs_mq_req_complete
.Lufs_mq_req_complete$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x0
	mov	x29, sp
	tbz	w2, #0, .LBB37_3
// %bb.1:
	ldr	w8, [x19, #24]
	and	w8, w8, #0xfc
	orr	w8, w8, #0x3
	cmp	w8, #35                 // =35
	b.eq	.LBB37_3
// %bb.2:
	ldr	x8, [x1, #1392]
	mov	w2, #1
	mov	x0, x19
	ldr	x1, [x8, #536]
	bl	ufs_mq_rq_inflt_update
	mov	w1, #27
	mov	x0, x19
	bl	mas_blk_latency_req_check
.LBB37_3:
	ldr	x8, [x19, #8]
	cbz	x8, .LBB37_7
// %bb.4:
	adrp	x10, cpu_number
	ldr	w8, [x8, #64]
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
	add	x10, x10, :lo12:cpu_number
	ldr	w9, [x9, x10]
	cmp	w8, w9
	b.eq	.LBB37_7
// %bb.5:
	add	w9, w8, #63             // =63
	cmp	w8, #0                  // =0
	csel	w9, w9, w8, lt
	adrp	x10, __cpu_online_mask
	asr	w9, w9, #6
	add	x10, x10, :lo12:__cpu_online_mask
	ldr	x9, [x10, w9, sxtw #3]
	lsr	x8, x9, x8
	tbz	w8, #0, .LBB37_7
// %bb.6:
	mov	w1, #28
	mov	x0, x19
	bl	mas_blk_latency_req_check
	ldr	x9, [x19, #8]
	adrp	x8, __cfi__ufs_mq_complete_request_remote
	add	x8, x8, :lo12:__cfi__ufs_mq_complete_request_remote
	str	x8, [x19, #856]
	str	x19, [x19, #864]
	str	wzr, [x19, #872]
	ldr	w0, [x9, #64]
	add	x1, x19, #848           // =848
	bl	smp_call_function_single_async
	b	.LBB37_8
.LBB37_7:
	mov	w1, #29
	mov	x0, x19
	bl	mas_blk_latency_req_check
	ldr	x8, [x19]
	mov	x0, x19
	ldr	x8, [x8, #48]
	ldr	x8, [x8, #48]
	blr	x8
.LBB37_8:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end37:
	.size	ufs_mq_req_complete, .Lfunc_end37-ufs_mq_req_complete
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_rq_inflt_update
	.type	ufs_mq_rq_inflt_update,@function
ufs_mq_rq_inflt_update:                 // @ufs_mq_rq_inflt_update
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	w8, [x0, #24]
	mov	w20, w2
	mov	x21, x0
	mov	x19, x1
	mov	x29, sp
	tbz	w8, #11, .LBB38_19
// %bb.1:
	mov	w22, #4096
	movk	w22, #64, lsl #16
	tst	w8, w22
	b.eq	.LBB38_10
.LBB38_2:
	ldr	w8, [x19, #44]
	ldr	w9, [x19, #40]
	cmn	w9, w8
	b.eq	.LBB38_22
// %bb.3:
	ldr	w8, [x21, #24]
	tst	w8, w22
	b.eq	.LBB38_6
// %bb.4:
	ldrb	w8, [x21, #200]
	tbnz	w8, #1, .LBB38_6
// %bb.5:
	add	x8, x19, #44            // =44
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
	add	x8, x21, #808           // =808
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x8]
1:	ldxr	x10, [x8]
	bic	x10, x10, x9
	stxr	w11, x10, [x8]
	cbnz	w11, 1b
	//NO_APP
	ldr	x8, [x21, #240]
	tbnz	w8, #1, .LBB38_7
	b	.LBB38_9
.LBB38_6:
	add	x8, x19, #40            // =40
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w11, [x8]
	sub	w11, w11, w9
	stxr	w12, w11, [x8]
	cbnz	w12, 1b

	//NO_APP
	add	x10, x21, #808          // =808
	mov	w8, #2
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x10]
1:	ldxr	x9, [x10]
	bic	x9, x9, x8
	stxr	w11, x9, [x10]
	cbnz	w11, 1b
	//NO_APP
	ldr	x8, [x21, #240]
	tbz	w8, #1, .LBB38_9
.LBB38_7:
	and	x8, x8, #0xfffffffffffffffd
	str	x8, [x21, #240]
	ldr	w8, [x19, #12]
	cbz	w8, .LBB38_23
// %bb.8:
	add	x8, x19, #12            // =12
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
.LBB38_9:
	tbnz	w20, #0, .LBB38_17
	b	.LBB38_18
.LBB38_10:
	ldrb	w8, [x21, #200]
	tbz	w8, #7, .LBB38_12
// %bb.11:
	ldr	x0, [x21]
	bl	blk_queue_query_unistore_enable
	tbnz	w0, #0, .LBB38_2
.LBB38_12:
	ldr	x8, [x21, #240]
	tbz	w8, #1, .LBB38_15
// %bb.13:
	and	x8, x8, #0xfffffffffffffffd
	str	x8, [x21, #240]
	ldr	w8, [x19, #12]
	cbz	w8, .LBB38_23
// %bb.14:
	add	x8, x19, #12            // =12
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w10, [x8]
	sub	w10, w10, w9
	stxr	w11, w10, [x8]
	cbnz	w11, 1b

	//NO_APP
.LBB38_15:
	ldr	w8, [x19, #32]
	cbz	w8, .LBB38_24
// %bb.16:
	add	x8, x19, #32            // =32
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w11, [x8]
	sub	w11, w11, w9
	stxr	w12, w11, [x8]
	cbnz	w12, 1b

	//NO_APP
	add	x10, x21, #808          // =808
	mov	w8, #4
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x10]
1:	ldxr	x9, [x10]
	bic	x9, x9, x8
	stxr	w11, x9, [x10]
	cbnz	w11, 1b
	//NO_APP
	tbz	w20, #0, .LBB38_18
.LBB38_17:
	bl	ktime_get
	str	x0, [x19, #184]
.LBB38_18:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB38_19:
	ldr	w8, [x19, #36]
	cbz	w8, .LBB38_25
// %bb.20:
	add	x8, x19, #36            // =36
	mov	w9, #1
	//APP
	// atomic_sub
	prfm	pstl1strm, [x8]
1:	ldxr	w11, [x8]
	sub	w11, w11, w9
	stxr	w12, w11, [x8]
	cbnz	w12, 1b

	//NO_APP
	add	x10, x21, #808          // =808
	mov	w8, #8
	//APP
	// atomic64_andnot
	prfm	pstl1strm, [x10]
1:	ldxr	x9, [x10]
	bic	x9, x9, x8
	stxr	w11, x9, [x10]
	cbnz	w11, 1b
	//NO_APP
	tbz	w20, #0, .LBB38_18
// %bb.21:
	bl	ktime_get
	str	x0, [x19, #192]
	b	.LBB38_18
.LBB38_22:
	adrp	x0, .L.str.38
	add	x0, x0, :lo12:.L.str.38
	bl	mas_blk_rdr_panic
.LBB38_23:
	adrp	x0, .L.str.41
	add	x0, x0, :lo12:.L.str.41
	bl	mas_blk_rdr_panic
.LBB38_24:
	adrp	x0, .L.str.39
	add	x0, x0, :lo12:.L.str.39
	bl	mas_blk_rdr_panic
.LBB38_25:
	adrp	x0, .L.str.40
	add	x0, x0, :lo12:.L.str.40
	bl	mas_blk_rdr_panic
.Lfunc_end38:
	.size	ufs_mq_rq_inflt_update, .Lfunc_end38-ufs_mq_rq_inflt_update
                                        // -- End function
	.globl	ufs_mq_req_deinit       // -- Begin function ufs_mq_req_deinit
	.p2align	2
	.type	ufs_mq_req_deinit,@function
ufs_mq_req_deinit:                      // @ufs_mq_req_deinit
.Lufs_mq_req_deinit$local:
// %bb.0:
	str	wzr, [x0, #760]
	ret
.Lfunc_end39:
	.size	ufs_mq_req_deinit, .Lfunc_end39-ufs_mq_req_deinit
                                        // -- End function
	.globl	ufs_mq_req_insert       // -- Begin function ufs_mq_req_insert
	.p2align	2
	.type	ufs_mq_req_insert,@function
ufs_mq_req_insert:                      // @ufs_mq_req_insert
.Lufs_mq_req_insert$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x20, x0
	mov	x0, x1
	mov	x29, sp
	mov	x19, x1
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB40_2
// %bb.1:
	ldrb	w8, [x20, #201]
	tbnz	w8, #1, .LBB40_8
.LBB40_2:
	ldr	x8, [x20]
	cmp	x8, x19
	b.ne	.LBB40_10
// %bb.3:
	ldr	w8, [x20, #24]
	tbnz	w8, #11, .LBB40_5
.LBB40_4:
	and	w8, w8, #0xfc
	orr	w8, w8, #0x3
	cmp	w8, #35                 // =35
	b.ne	.LBB40_11
.LBB40_5:
	mov	x0, x20
	mov	x1, x19
	bl	ufs_mq_insert_sync_list
.LBB40_6:
	ldr	x8, [x20]
	cmp	x8, x19
	b.eq	.LBB40_9
// %bb.7:
	bl	dump_stack
	b	.LBB40_9
.LBB40_8:
	mov	x0, x20
	mov	x1, x19
	bl	ufs_mq_insert_async_list
.LBB40_9:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB40_10:
	bl	dump_stack
	ldr	w8, [x20, #24]
	tbz	w8, #11, .LBB40_4
	b	.LBB40_5
.LBB40_11:
	mov	x0, x20
	mov	x1, x19
	bl	ufs_mq_insert_async_list
	b	.LBB40_6
.Lfunc_end40:
	.size	ufs_mq_req_insert, .Lfunc_end40-ufs_mq_req_insert
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_insert_async_list
	.type	ufs_mq_insert_async_list,@function
ufs_mq_insert_async_list:               // @ufs_mq_insert_async_list
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x1, #1392]
	mov	x20, x0
	mov	x0, x1
	mov	x29, sp
	ldr	x19, [x8, #536]
	mov	x21, x1
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB41_3
// %bb.1:
	ldrb	w8, [x20, #201]
	tbnz	w8, #1, .LBB41_3
// %bb.2:
	mov	x0, x20
	mov	x1, x21
	bl	ufs_mq_insert_sync_list
	b	.LBB41_7
.LBB41_3:
	ldr	x8, [x21, #1400]
	cbz	x8, .LBB41_7
// %bb.4:
	ldr	x22, [x8, #176]
	cbz	x22, .LBB41_7
// %bb.5:
	ldr	x8, [x22]
	ldr	x8, [x8, #16]
	cbz	x8, .LBB41_7
// %bb.6:
	mov	w1, #13
	mov	x0, x20
	bl	mas_blk_latency_req_check
	add	x21, x19, #100          // =100
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x22]
	mov	x22, x0
	mov	x0, x20
	mov	x1, x19
	ldr	x8, [x8, #16]
	blr	x8
	add	x8, x19, #128           // =128
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	mov	x0, x21
	mov	x1, x22
	bl	_raw_spin_unlock_irqrestore
.LBB41_7:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end41:
	.size	ufs_mq_insert_async_list, .Lfunc_end41-ufs_mq_insert_async_list
                                        // -- End function
	.globl	ufs_mq_req_requeue      // -- Begin function ufs_mq_req_requeue
	.p2align	2
	.type	ufs_mq_req_requeue,@function
ufs_mq_req_requeue:                     // @ufs_mq_req_requeue
.Lufs_mq_req_requeue$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x19, x0
	str	wzr, [x0, #740]
	mov	x0, x1
	mov	x29, sp
	mov	x20, x1
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB42_8
// %bb.1:
	ldrb	w8, [x19, #201]
	tbnz	w8, #1, .LBB42_9
// %bb.2:
	ldr	x8, [x19, #64]
	cbz	x8, .LBB42_5
// %bb.3:
	ldrb	w9, [x8, #16]
	tbz	w9, #0, .LBB42_5
// %bb.4:
	ldrb	w8, [x8, #360]
	cbnz	w8, .LBB42_10
.LBB42_5:
	ldr	w8, [x19, #24]
	tbnz	w8, #11, .LBB42_7
// %bb.6:
	and	w8, w8, #0xfc
	orr	w8, w8, #0x3
	cmp	w8, #35                 // =35
	b.ne	.LBB42_11
.LBB42_7:
	mov	x0, x19
	mov	x1, x20
	bl	ufs_mq_insert_sync_list
	b	.LBB42_10
.LBB42_8:
	mov	x0, x19
	mov	x1, x20
	bl	ufs_mq_req_insert
	b	.LBB42_10
.LBB42_9:
	mov	x0, x19
	mov	x1, x20
	bl	ufs_mq_insert_async_list
.LBB42_10:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB42_11:
	ldr	x8, [x20, #1400]
	cbz	x8, .LBB42_10
// %bb.12:
	ldr	x22, [x8, #176]
	cbz	x22, .LBB42_10
// %bb.13:
	ldr	x8, [x22]
	ldr	x8, [x8, #32]
	cbz	x8, .LBB42_10
// %bb.14:
	ldr	x8, [x20, #1392]
	mov	w1, #13
	mov	x0, x19
	ldr	x20, [x8, #536]
	bl	mas_blk_latency_req_check
	add	x21, x20, #100          // =100
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x22]
	mov	x22, x0
	mov	x0, x19
	mov	x1, x20
	ldr	x8, [x8, #32]
	blr	x8
	add	x8, x20, #128           // =128
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	mov	x0, x21
	mov	x1, x22
	bl	_raw_spin_unlock_irqrestore
	b	.LBB42_10
.Lfunc_end42:
	.size	ufs_mq_req_requeue, .Lfunc_end42-ufs_mq_req_requeue
                                        // -- End function
	.globl	ufs_mq_req_timeout_handler // -- Begin function ufs_mq_req_timeout_handler
	.p2align	2
	.type	ufs_mq_req_timeout_handler,@function
ufs_mq_req_timeout_handler:             // @ufs_mq_req_timeout_handler
.Lufs_mq_req_timeout_handler$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	x8, [x0]
	ldr	w9, [x0, #28]
	mov	x19, x0
	mov	x29, sp
	ldr	x10, [x8, #1392]
	ldr	x8, [x8, #48]
	orr	w9, w9, #0x200000
	ldr	x20, [x10, #536]
	str	w9, [x0, #28]
	ldr	x8, [x8, #32]
	cbz	x8, .LBB43_5
// %bb.1:
	mov	x0, x19
	mov	w1, wzr
	blr	x8
	cmp	w0, #1                  // =1
	b.eq	.LBB43_5
// %bb.2:
	cbnz	w0, .LBB43_6
// %bb.3:
	ldr	w8, [x19, #24]
	and	w8, w8, #0xfc
	orr	w8, w8, #0x3
	cmp	w8, #35                 // =35
	b.eq	.LBB43_7
// %bb.4:
	mov	w2, #1
	mov	x0, x19
	mov	x1, x20
	bl	ufs_mq_rq_inflt_update
	b	.LBB43_7
.LBB43_5:
	mov	x0, x19
	bl	blk_add_timer
	b	.LBB43_7
.LBB43_6:
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk_iosched_ufs_mq.c"; .popsection; .long 14472b - 14470b; .short 3270; .short (1 << 0)|((1 << 1) | ((9) << 8)); .popsection; 14471: brk 0x800
	//NO_APP
.LBB43_7:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end43:
	.size	ufs_mq_req_timeout_handler, .Lfunc_end43-ufs_mq_req_timeout_handler
                                        // -- End function
	.globl	ufs_mq_ctx_put          // -- Begin function ufs_mq_ctx_put
	.p2align	2
	.type	ufs_mq_ctx_put,@function
ufs_mq_ctx_put:                         // @ufs_mq_ctx_put
.Lufs_mq_ctx_put$local:
// %bb.0:
	ret
.Lfunc_end44:
	.size	ufs_mq_ctx_put, .Lfunc_end44-ufs_mq_ctx_put
                                        // -- End function
	.globl	ufs_mq_hctx_get_by_req  // -- Begin function ufs_mq_hctx_get_by_req
	.p2align	2
	.type	ufs_mq_hctx_get_by_req,@function
ufs_mq_hctx_get_by_req:                 // @ufs_mq_hctx_get_by_req
.Lufs_mq_hctx_get_by_req$local:
// %bb.0:
	ldr	w8, [x0, #24]
	ldr	x9, [x0, #272]
	tst	w8, #0xff
	cset	w10, eq
	tst	w8, #0x8000000
	mov	w8, #2
	csel	x8, x10, x8, eq
	add	x8, x9, x8, lsl #3
	ldr	x8, [x8, #80]
	str	x8, [x1]
	ret
.Lfunc_end45:
	.size	ufs_mq_hctx_get_by_req, .Lfunc_end45-ufs_mq_hctx_get_by_req
                                        // -- End function
	.globl	ufs_mq_exec_queue       // -- Begin function ufs_mq_exec_queue
	.p2align	2
	.type	ufs_mq_exec_queue,@function
ufs_mq_exec_queue:                      // @ufs_mq_exec_queue
.Lufs_mq_exec_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	ufs_mq_sync_dispatch
	ldr	x8, [x19, #1400]
	cbz	x8, .LBB46_6
// %bb.1:
	ldr	x22, [x8, #176]
	cbz	x22, .LBB46_6
// %bb.2:
	ldr	x8, [x22]
	ldr	x8, [x8, #48]
	cbz	x8, .LBB46_6
// %bb.3:
	ldr	x8, [x19, #1392]
	ldr	x20, [x8, #536]
	add	x21, x20, #100          // =100
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x22]
	mov	x22, x0
	mov	x0, x20
	ldr	x8, [x8, #48]
	blr	x8
	mov	w20, w0
	mov	x0, x21
	mov	x1, x22
	bl	_raw_spin_unlock_irqrestore
	tbnz	w20, #0, .LBB46_5
// %bb.4:
	ldr	x19, [x19, #1392]
	adrp	x8, mas_blk_mq_async_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_async_disp_wq]
	mov	w0, #8
	add	x2, x19, #376           // =376
	mov	x3, xzr
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #264]
	ldr	w8, [x19, #344]
	add	w8, w8, #1              // =1
	str	w8, [x19, #344]
.LBB46_5:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB46_6:
	adrp	x0, .L.str.42
	add	x0, x0, :lo12:.L.str.42
	bl	mas_blk_rdr_panic
.Lfunc_end46:
	.size	ufs_mq_exec_queue, .Lfunc_end46-ufs_mq_exec_queue
                                        // -- End function
	.globl	ufs_mq_run_hw_queue     // -- Begin function ufs_mq_run_hw_queue
	.p2align	2
	.type	ufs_mq_run_hw_queue,@function
ufs_mq_run_hw_queue:                    // @ufs_mq_run_hw_queue
.Lufs_mq_run_hw_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x20, [x0, #1392]
	adrp	x8, mas_blk_mq_sync_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_sync_disp_wq]
	mov	x19, x0
	add	x2, x20, #120           // =120
	mov	w0, #8
	mov	x3, xzr
	mov	x29, sp
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x20, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x20, w8, sxtw #3
	str	x0, [x8, #8]
	ldr	w8, [x20, #88]
	add	w8, w8, #1              // =1
	str	w8, [x20, #88]
	ldr	x8, [x19, #1400]
	cbz	x8, .LBB47_6
// %bb.1:
	ldr	x22, [x8, #176]
	cbz	x22, .LBB47_6
// %bb.2:
	ldr	x8, [x22]
	ldr	x8, [x8, #48]
	cbz	x8, .LBB47_6
// %bb.3:
	ldr	x8, [x19, #1392]
	ldr	x20, [x8, #536]
	add	x21, x20, #100          // =100
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x22]
	mov	x22, x0
	mov	x0, x20
	ldr	x8, [x8, #48]
	blr	x8
	mov	w20, w0
	mov	x0, x21
	mov	x1, x22
	bl	_raw_spin_unlock_irqrestore
	tbnz	w20, #0, .LBB47_5
// %bb.4:
	ldr	x19, [x19, #1392]
	adrp	x8, mas_blk_mq_async_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_async_disp_wq]
	mov	w0, #8
	add	x2, x19, #376           // =376
	mov	x3, xzr
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #264]
	ldr	w8, [x19, #344]
	add	w8, w8, #1              // =1
	str	w8, [x19, #344]
.LBB47_5:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB47_6:
	adrp	x0, .L.str.42
	add	x0, x0, :lo12:.L.str.42
	bl	mas_blk_rdr_panic
.Lfunc_end47:
	.size	ufs_mq_run_hw_queue, .Lfunc_end47-ufs_mq_run_hw_queue
                                        // -- End function
	.globl	ufs_mq_run_requeue      // -- Begin function ufs_mq_run_requeue
	.p2align	2
	.type	ufs_mq_run_requeue,@function
ufs_mq_run_requeue:                     // @ufs_mq_run_requeue
.Lufs_mq_run_requeue$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x20, [x0, #1392]
	adrp	x8, mas_blk_mq_sync_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_sync_disp_wq]
	mov	x19, x0
	add	x2, x20, #120           // =120
	mov	w0, #8
	mov	x3, xzr
	mov	x29, sp
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x20, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x20, w8, sxtw #3
	str	x0, [x8, #8]
	ldr	w8, [x20, #88]
	add	w8, w8, #1              // =1
	str	w8, [x20, #88]
	ldr	x8, [x19, #1400]
	cbz	x8, .LBB48_6
// %bb.1:
	ldr	x22, [x8, #176]
	cbz	x22, .LBB48_6
// %bb.2:
	ldr	x8, [x22]
	ldr	x8, [x8, #48]
	cbz	x8, .LBB48_6
// %bb.3:
	ldr	x8, [x19, #1392]
	ldr	x20, [x8, #536]
	add	x21, x20, #100          // =100
	mov	x0, x21
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x22]
	mov	x22, x0
	mov	x0, x20
	ldr	x8, [x8, #48]
	blr	x8
	mov	w20, w0
	mov	x0, x21
	mov	x1, x22
	bl	_raw_spin_unlock_irqrestore
	tbnz	w20, #0, .LBB48_5
// %bb.4:
	ldr	x19, [x19, #1392]
	adrp	x8, mas_blk_mq_async_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_async_disp_wq]
	mov	w0, #8
	add	x2, x19, #376           // =376
	mov	x3, xzr
	bl	queue_delayed_work_on
	bl	ktime_get
	ldrsw	x8, [x19, #344]
	mov	w9, #26215
	movk	w9, #26214, lsl #16
	mul	x9, x8, x9
	lsr	x10, x9, #63
	asr	x9, x9, #34
	add	w9, w9, w10
	mov	w10, #10
	msub	w8, w9, w10, w8
	add	x8, x19, w8, sxtw #3
	str	x0, [x8, #264]
	ldr	w8, [x19, #344]
	add	w8, w8, #1              // =1
	str	w8, [x19, #344]
.LBB48_5:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB48_6:
	adrp	x0, .L.str.42
	add	x0, x0, :lo12:.L.str.42
	bl	mas_blk_rdr_panic
.Lfunc_end48:
	.size	ufs_mq_run_requeue, .Lfunc_end48-ufs_mq_run_requeue
                                        // -- End function
	.globl	ufs_mq_poll_enable      // -- Begin function ufs_mq_poll_enable
	.p2align	2
	.type	ufs_mq_poll_enable,@function
ufs_mq_poll_enable:                     // @ufs_mq_poll_enable
.Lufs_mq_poll_enable$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	str	x23, [sp, #16]          // 8-byte Folded Spill
	adrp	x23, __cpu_online_mask
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x19, x0
	mov	w21, wzr
	mov	w20, wzr
	adrp	x22, __cpu_possible_mask
	add	x23, x23, :lo12:__cpu_online_mask
	mov	x29, sp
.LBB49_1:                               // =>This Inner Loop Header: Depth=1
	ldrb	w0, [x22, :lo12:__cpu_possible_mask]
	bl	__sw_hweight64
	cmp	w21, w0
	b.hs	.LBB49_3
// %bb.2:                               //   in Loop: Header=BB49_1 Depth=1
	add	w8, w21, #63            // =63
	cmp	w21, #0                 // =0
	csel	w8, w8, w21, lt
	asr	w8, w8, #6
	ldr	x8, [x23, w8, sxtw #3]
	lsr	x8, x8, x21
	and	w8, w8, #0x1
	add	w20, w20, w8
	add	w21, w21, #1            // =1
	b	.LBB49_1
.LBB49_3:
	cmp	w20, #1                 // =1
	cset	w8, hi
	strb	w8, [x19]
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.Lfunc_end49:
	.size	ufs_mq_poll_enable, .Lfunc_end49-ufs_mq_poll_enable
                                        // -- End function
	.globl	ufs_order_panic_wait_datasync_handle // -- Begin function ufs_order_panic_wait_datasync_handle
	.p2align	2
	.type	ufs_order_panic_wait_datasync_handle,@function
ufs_order_panic_wait_datasync_handle:   // @ufs_order_panic_wait_datasync_handle
.Lufs_order_panic_wait_datasync_handle$local:
// %bb.0:
	ldr	x8, [x0, #1944]
	ldr	w9, [x8, #24]
	ldr	w10, [x8, #16]
	add	w9, w10, w9
	cmp	w9, #1                  // =1
	cset	w0, lt
	cmp	w9, #0                  // =0
	cset	w9, gt
	strb	w9, [x8, #536]
	ret
.Lfunc_end50:
	.size	ufs_order_panic_wait_datasync_handle, .Lfunc_end50-ufs_order_panic_wait_datasync_handle
                                        // -- End function
	.globl	ufs_order_panic_datasync_handle // -- Begin function ufs_order_panic_datasync_handle
	.p2align	2
	.type	ufs_order_panic_datasync_handle,@function
ufs_order_panic_datasync_handle:        // @ufs_order_panic_datasync_handle
.Lufs_order_panic_datasync_handle$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x8, [x0, #1944]
	mov	w9, #1
	mov	w0, #8
	mov	x3, xzr
	strb	w9, [x8, #536]
	adrp	x9, mas_blk_mq_sync_disp_wq
	ldr	x1, [x9, :lo12:mas_blk_mq_sync_disp_wq]
	add	x2, x8, #400            // =400
	mov	x29, sp
	bl	queue_delayed_work_on
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end51:
	.size	ufs_order_panic_datasync_handle, .Lfunc_end51-ufs_order_panic_datasync_handle
                                        // -- End function
	.globl	ufs_mq_write_throttle_check_timer_prepare // -- Begin function ufs_mq_write_throttle_check_timer_prepare
	.p2align	2
	.type	ufs_mq_write_throttle_check_timer_prepare,@function
ufs_mq_write_throttle_check_timer_prepare: // @ufs_mq_write_throttle_check_timer_prepare
.Lufs_mq_write_throttle_check_timer_prepare$local:
// %bb.0:
	sub	x0, x0, #272            // =272
	ret
.Lfunc_end52:
	.size	ufs_mq_write_throttle_check_timer_prepare, .Lfunc_end52-ufs_mq_write_throttle_check_timer_prepare
                                        // -- End function
	.globl	ufs_mq_sync_burst_check_timer_prepare // -- Begin function ufs_mq_sync_burst_check_timer_prepare
	.p2align	2
	.type	ufs_mq_sync_burst_check_timer_prepare,@function
ufs_mq_sync_burst_check_timer_prepare:  // @ufs_mq_sync_burst_check_timer_prepare
.Lufs_mq_sync_burst_check_timer_prepare$local:
// %bb.0:
	sub	x0, x0, #208            // =208
	ret
.Lfunc_end53:
	.size	ufs_mq_sync_burst_check_timer_prepare, .Lfunc_end53-ufs_mq_sync_burst_check_timer_prepare
                                        // -- End function
	.globl	ufs_tagset_power_off_proc // -- Begin function ufs_tagset_power_off_proc
	.p2align	2
	.type	ufs_tagset_power_off_proc,@function
ufs_tagset_power_off_proc:              // @ufs_tagset_power_off_proc
.Lufs_tagset_power_off_proc$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	ldr	x19, [x0, #1944]
	mov	w8, #1
	mov	w0, #8
	mov	x3, xzr
	strb	w8, [x19, #536]
	adrp	x8, mas_blk_mq_sync_disp_wq
	ldr	x1, [x8, :lo12:mas_blk_mq_sync_disp_wq]
	add	x2, x19, #400           // =400
	mov	x29, sp
	bl	queue_delayed_work_on
	adrp	x8, jiffies
	ldr	x8, [x8, :lo12:jiffies]
	add	x0, x19, #344           // =344
	add	x1, x8, #125            // =125
	bl	mod_timer
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end54:
	.size	ufs_tagset_power_off_proc, .Lfunc_end54-ufs_tagset_power_off_proc
                                        // -- End function
	.globl	ufs_mq_status_dump      // -- Begin function ufs_mq_status_dump
	.p2align	2
	.type	ufs_mq_status_dump,@function
ufs_mq_status_dump:                     // @ufs_mq_status_dump
.Lufs_mq_status_dump$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	x8, [x0, #1392]
	adrp	x9, .L.str.44
	adrp	x10, .L.str.43
	add	x9, x9, :lo12:.L.str.44
	add	x10, x10, :lo12:.L.str.43
	cmp	w1, #1                  // =1
	csel	x19, x10, x9, eq
	mov	x29, sp
	cbz	x8, .LBB55_2
// %bb.1:
	ldr	x20, [x8, #536]
	cbnz	x20, .LBB55_3
.LBB55_2:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB55_3:
	ldr	w2, [x20, #28]
	adrp	x0, .L.str.12
	add	x0, x0, :lo12:.L.str.12
	mov	x1, x19
	bl	printk
	ldr	w2, [x20, #24]
	ldr	w3, [x20, #16]
	ldr	w4, [x20, #20]
	adrp	x0, .L.str.13
	add	x0, x0, :lo12:.L.str.13
	mov	x1, x19
	bl	printk
	ldr	w2, [x20, #44]
	ldr	w3, [x20, #40]
	ldr	w4, [x20, #32]
	ldr	w5, [x20, #36]
	ldr	w6, [x20, #12]
	adrp	x0, .L.str.14
	add	x0, x0, :lo12:.L.str.14
	mov	x1, x19
	bl	printk
	b	.LBB55_2
.Lfunc_end55:
	.size	ufs_mq_status_dump, .Lfunc_end55-ufs_mq_status_dump
                                        // -- End function
	.globl	ufs_mq_iosched_init     // -- Begin function ufs_mq_iosched_init
	.p2align	2
	.type	ufs_mq_iosched_init,@function
ufs_mq_iosched_init:                    // @ufs_mq_iosched_init
.Lufs_mq_iosched_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	adrp	x21, kmalloc_caches+80
	ldr	x8, [x21, :lo12:kmalloc_caches+80]
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x20, x0
	mov	w1, #3520
	mov	w2, #544
	mov	x0, x8
	stp	x24, x23, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	bl	kmem_cache_alloc_trace
	cbz	x0, .LBB56_19
// %bb.1:
	mov	x19, x0
	mov	x0, x20
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #1936]
	mov	x22, x0
	cbz	w8, .LBB56_9
// %bb.2:
	ldr	x8, [x22, #1944]
	add	x8, x8, #336            // =336
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
	ldr	x21, [x22, #1944]
.LBB56_3:
	cbz	x21, .LBB56_16
// %bb.4:
	ldr	x8, [x20, #1400]
	cbz	x8, .LBB56_15
// %bb.5:
	ldr	x23, [x8, #176]
	cbz	x23, .LBB56_15
// %bb.6:
	ldr	x8, [x23]
	ldr	x8, [x8, #8]
	cbz	x8, .LBB56_8
// %bb.7:
	add	x22, x21, #100          // =100
	mov	x0, x22
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x23]
	mov	x23, x0
	mov	x0, x21
	ldr	x8, [x8, #8]
	blr	x8
	mov	x0, x22
	mov	x1, x23
	bl	_raw_spin_unlock_irqrestore
.LBB56_8:
	adrp	x22, delayed_work_timer_fn
	adrp	x9, __cfi_ufs_mq_sync_io_dispatch_work_fn
	add	x22, x22, :lo12:delayed_work_timer_fn
	mov	x23, #68719476704
	add	x8, x19, #128           // =128
	add	x9, x9, :lo12:__cfi_ufs_mq_sync_io_dispatch_work_fn
	add	x0, x19, #168           // =168
	mov	w2, #2097152
	mov	x1, x22
	mov	x3, xzr
	mov	x4, xzr
	str	x23, [x19, #120]
	str	x8, [x19, #128]
	stp	x8, x9, [x19, #136]
	bl	init_timer_key
	adrp	x9, __cfi_ufs_mq_async_io_dispatch_work_fn
	add	x8, x19, #384           // =384
	add	x9, x9, :lo12:__cfi_ufs_mq_async_io_dispatch_work_fn
	add	x0, x19, #424           // =424
	mov	w2, #2097152
	mov	x1, x22
	mov	x3, xzr
	mov	x4, xzr
	str	x20, [x19, #256]
	str	x23, [x19, #376]
	str	x8, [x19, #384]
	stp	x8, x9, [x19, #392]
	bl	init_timer_key
	str	x20, [x19, #512]
	str	wzr, [x19, #88]
	str	wzr, [x19, #344]
	str	wzr, [x20, #1248]
	str	wzr, [x20, #1252]
	str	x21, [x19, #536]
	adrp	x21, io_guard_queue_list_lock
	add	x21, x21, :lo12:io_guard_queue_list_lock
	mov	x0, x21
	str	x20, [x19]
	str	x19, [x20, #1392]
	bl	_raw_spin_lock
	adrp	x8, mas_io_guard_queue_list
	add	x8, x8, :lo12:mas_io_guard_queue_list
	ldr	x9, [x8, #8]
	add	x10, x19, #520          // =520
	mov	x0, x21
	str	x10, [x8, #8]
	str	x8, [x19, #520]
	str	x9, [x19, #528]
	str	x10, [x9]
	bl	_raw_spin_unlock
	adrp	x8, mas_blk_io_guard_wq
	ldr	x1, [x8, :lo12:mas_blk_io_guard_wq]
	adrp	x2, mas_io_guard_work
	add	x2, x2, :lo12:mas_io_guard_work
	mov	w0, #8
	mov	w3, #1250
	bl	queue_delayed_work_on
	adrp	x1, __cfi_ufs_mq_make_request
	add	x1, x1, :lo12:__cfi_ufs_mq_make_request
	mov	x0, x20
	bl	blk_queue_make_request
	mov	w0, wzr
	b	.LBB56_18
.LBB56_9:
	ldr	x0, [x21, :lo12:kmalloc_caches+80]
	mov	w1, #3520
	mov	w2, #544
	bl	kmem_cache_alloc_trace
	cbz	x0, .LBB56_16
// %bb.10:
	mov	w23, #1
	adrp	x1, __ufs_turbo_check_timer_expire
	mov	x21, x0
	str	w23, [x0, #336]
	add	x0, x0, #344            // =344
	add	x1, x1, :lo12:__ufs_turbo_check_timer_expire
	mov	w2, wzr
	mov	x3, xzr
	mov	x4, xzr
	bl	init_timer_key
	adrp	x10, ufs_datasync_work
	adrp	x1, delayed_work_timer_fn
	mov	x8, #68719476704
	add	x9, x21, #408           // =408
	add	x10, x10, :lo12:ufs_datasync_work
	add	x0, x21, #448           // =448
	add	x1, x1, :lo12:delayed_work_timer_fn
	mov	w2, #2097152
	mov	x3, xzr
	mov	x4, xzr
	strb	wzr, [x21, #536]
	str	x8, [x21, #400]
	str	x9, [x21, #408]
	stp	x9, x10, [x21, #416]
	bl	init_timer_key
	add	x8, x21, #56            // =56
	str	wzr, [x21, #16]
	str	wzr, [x21, #20]
	str	wzr, [x21, #24]
	str	wzr, [x21, #28]
	str	x8, [x21, #56]
	str	x8, [x21, #64]
	add	x8, x21, #80            // =80
	str	x8, [x21, #80]
	str	x8, [x21, #88]
	add	x8, x21, #136           // =136
	str	wzr, [x21, #48]
	str	wzr, [x21, #100]
	str	x8, [x21, #136]
	str	x8, [x21, #144]
	mov	w8, #5
	str	wzr, [x21, #152]
	str	wzr, [x21, #72]
	str	wzr, [x21, #96]
	str	wzr, [x21, #128]
	str	w8, [x21, #8]
	mov	x8, #28
	movk	x8, #14, lsl #32
	stur	x8, [x21, #156]
	str	wzr, [x21, #12]
	str	wzr, [x21, #36]
	str	wzr, [x21, #44]
	str	wzr, [x21, #40]
	str	wzr, [x21, #32]
	bl	ktime_get
	str	x0, [x21, #168]
	bl	ktime_get
	str	x0, [x21, #176]
	bl	ktime_get
	str	x0, [x21, #184]
	bl	ktime_get
	str	x0, [x21, #192]
	bl	ktime_get
	adrp	x1, __cfi_ufs_mq_sync_burst_check_timer_expire
	str	x0, [x21, #200]
	add	x0, x21, #208           // =208
	add	x1, x1, :lo12:__cfi_ufs_mq_sync_burst_check_timer_expire
	mov	w2, wzr
	mov	x3, xzr
	mov	x4, xzr
	bl	init_timer_key
	adrp	x1, __cfi_ufs_mq_write_throttle_check_timer_expire
	add	x0, x21, #272           // =272
	add	x1, x1, :lo12:__cfi_ufs_mq_write_throttle_check_timer_expire
	mov	w2, wzr
	mov	x3, xzr
	mov	x4, xzr
	bl	init_timer_key
	adrp	x0, .L.str.48
	add	x0, x0, :lo12:.L.str.48
	mov	w1, #16
	mov	w2, wzr
	str	wzr, [x21, #328]
	strb	w23, [x21, #332]
	str	wzr, [x21, #264]
	str	wzr, [x21, #268]
	bl	alloc_workqueue
	adrp	x23, mas_blk_mq_sync_disp_wq
	str	x0, [x23, :lo12:mas_blk_mq_sync_disp_wq]
	cbz	x0, .LBB56_20
// %bb.11:
	adrp	x0, .L.str.50
	add	x0, x0, :lo12:.L.str.50
	mov	w1, #2
	mov	w2, wzr
	bl	alloc_workqueue
	adrp	x24, mas_blk_mq_async_disp_wq
	str	x0, [x24, :lo12:mas_blk_mq_async_disp_wq]
	cbz	x0, .LBB56_22
// %bb.12:
	adrp	x0, .L.str.51
	add	x0, x0, :lo12:.L.str.51
	mov	w1, #6
	mov	w2, wzr
	bl	alloc_workqueue
	adrp	x8, mas_blk_io_guard_wq
	str	x0, [x8, :lo12:mas_blk_io_guard_wq]
	cbz	x0, .LBB56_21
// %bb.13:
	adrp	x8, mas_io_guard_work
	add	x8, x8, :lo12:mas_io_guard_work
	mov	x9, #68719476704
	adrp	x10, __cfi_ufs_mq_io_guard_work_fn
	adrp	x1, delayed_work_timer_fn
	add	x10, x10, :lo12:__cfi_ufs_mq_io_guard_work_fn
	str	x9, [x8]
	add	x9, x8, #8              // =8
	add	x0, x8, #48             // =48
	add	x1, x1, :lo12:delayed_work_timer_fn
	mov	w2, #2097152
	mov	x3, xzr
	mov	x4, xzr
	str	x9, [x8, #8]
	stp	x9, x10, [x8, #16]
	bl	init_timer_key
	str	x22, [x21]
	ldr	w8, [x22, #4]
	mov	w9, #1
	str	x21, [x22, #1944]
	strb	w9, [x22, #1936]
	cmp	w8, #2                  // =2
	b.ne	.LBB56_3
// %bb.14:
	add	x0, x22, #1920          // =1920
	bl	mas_blk_flush_list_register
	b	.LBB56_3
.LBB56_15:
	mov	x0, x20
	bl	ufs_mq_sched_ds_lld_exit
.LBB56_16:
	mov	x0, x19
	bl	kfree
.LBB56_17:
	mov	w0, #-12
.LBB56_18:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB56_19:
	adrp	x0, .L.str.15
	adrp	x1, .L__func__.ufs_mq_iosched_init
	add	x0, x0, :lo12:.L.str.15
	add	x1, x1, :lo12:.L__func__.ufs_mq_iosched_init
	mov	w2, #3663
	bl	printk
	b	.LBB56_17
.LBB56_20:
	adrp	x0, .L.str.49
	adrp	x1, .L__func__.ufs_mq_workqueue_init
	add	x0, x0, :lo12:.L.str.49
	add	x1, x1, :lo12:.L__func__.ufs_mq_workqueue_init
	mov	w2, #3506
	bl	printk
	b	.LBB56_23
.LBB56_21:
	ldr	x0, [x24, :lo12:mas_blk_mq_async_disp_wq]
	bl	destroy_workqueue
.LBB56_22:
	ldr	x0, [x23, :lo12:mas_blk_mq_sync_disp_wq]
	bl	destroy_workqueue
	adrp	x0, .L.str.52
	adrp	x1, .L__func__.ufs_mq_workqueue_init
	add	x0, x0, :lo12:.L.str.52
	add	x1, x1, :lo12:.L__func__.ufs_mq_workqueue_init
	bl	printk
.LBB56_23:
	mov	x0, x21
	bl	kfree
	b	.LBB56_16
.Lfunc_end56:
	.size	ufs_mq_iosched_init, .Lfunc_end56-ufs_mq_iosched_init
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_sched_ds_lld_exit
	.type	ufs_mq_sched_ds_lld_exit,@function
ufs_mq_sched_ds_lld_exit:               // @ufs_mq_sched_ds_lld_exit
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	bl	mas_blk_get_lld
	ldr	w8, [x0, #4]
	ldr	x20, [x0, #1944]
	mov	x19, x0
	cmp	w8, #2                  // =2
	b.ne	.LBB57_2
// %bb.1:
	add	x0, x19, #1920          // =1920
	bl	mas_blk_flush_list_unregister
.LBB57_2:
	ldrb	w8, [x19, #1936]
	cbz	w8, .LBB57_5
// %bb.3:
	add	x8, x20, #336           // =336
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
	cbnz	w10, .LBB57_5
// %bb.4:
	strb	wzr, [x19, #1936]
	adrp	x8, mas_blk_mq_async_disp_wq
	ldr	x0, [x8, :lo12:mas_blk_mq_async_disp_wq]
	bl	destroy_workqueue
	adrp	x8, mas_blk_mq_sync_disp_wq
	ldr	x0, [x8, :lo12:mas_blk_mq_sync_disp_wq]
	bl	destroy_workqueue
	adrp	x8, mas_blk_io_guard_wq
	ldr	x0, [x8, :lo12:mas_blk_io_guard_wq]
	bl	destroy_workqueue
	ldr	x0, [x19, #1944]
	bl	kfree
	str	xzr, [x19, #1944]
.LBB57_5:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end57:
	.size	ufs_mq_sched_ds_lld_exit, .Lfunc_end57-ufs_mq_sched_ds_lld_exit
                                        // -- End function
	.globl	ufs_mq_iosched_exit     // -- Begin function ufs_mq_iosched_exit
	.p2align	2
	.type	ufs_mq_iosched_exit,@function
ufs_mq_iosched_exit:                    // @ufs_mq_iosched_exit
.Lufs_mq_iosched_exit$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	ldr	x21, [x0, #1392]
	mov	x29, sp
	cbz	x21, .LBB58_2
// %bb.1:
	mov	x19, x0
	adrp	x0, mas_io_guard_work
	add	x0, x0, :lo12:mas_io_guard_work
	bl	cancel_delayed_work_sync
	adrp	x20, io_guard_queue_list_lock
	add	x20, x20, :lo12:io_guard_queue_list_lock
	mov	x0, x20
	bl	_raw_spin_lock
	ldr	x8, [x21, #528]
	ldr	x9, [x21, #520]
	add	x10, x21, #520          // =520
	mov	x0, x20
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x10, [x21, #520]
	str	x10, [x21, #528]
	bl	_raw_spin_unlock
	mov	x0, x19
	bl	ufs_mq_sched_ds_lld_exit
	ldr	x0, [x19, #1392]
	bl	kfree
	str	xzr, [x19, #1392]
.LBB58_2:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end58:
	.size	ufs_mq_iosched_exit, .Lfunc_end58-ufs_mq_iosched_exit
                                        // -- End function
	.globl	blk_mq_tagset_ufs_mq_iosched_enable // -- Begin function blk_mq_tagset_ufs_mq_iosched_enable
	.p2align	2
	.type	blk_mq_tagset_ufs_mq_iosched_enable,@function
blk_mq_tagset_ufs_mq_iosched_enable:    // @blk_mq_tagset_ufs_mq_iosched_enable
.Lblk_mq_tagset_ufs_mq_iosched_enable$local:
// %bb.0:
	ldr	x8, [x0, #88]
	cmp	w1, #0                  // =0
	and	x9, x8, #0xffffffffffffffdf
	orr	x8, x8, #0x20
	csel	x8, x9, x8, eq
	str	x8, [x0, #88]
	ret
.Lfunc_end59:
	.size	blk_mq_tagset_ufs_mq_iosched_enable, .Lfunc_end59-blk_mq_tagset_ufs_mq_iosched_enable
                                        // -- End function
	.globl	blk_mq_get_io_in_list_count // -- Begin function blk_mq_get_io_in_list_count
	.p2align	2
	.type	blk_mq_get_io_in_list_count,@function
blk_mq_get_io_in_list_count:            // @blk_mq_get_io_in_list_count
.Lblk_mq_get_io_in_list_count$local:
// %bb.0:
	cbz	x0, .LBB60_5
// %bb.1:
	ldr	x8, [x0, #144]
	cbz	x8, .LBB60_5
// %bb.2:
	ldr	x8, [x8, #1392]
	cbz	x8, .LBB60_5
// %bb.3:
	ldr	x8, [x8, #536]
	cbz	x8, .LBB60_5
// %bb.4:
	ldr	w9, [x8, #72]
	ldr	w10, [x8, #96]
	ldr	w8, [x8, #128]
	add	w9, w10, w9
	add	w0, w9, w8
	ret
.LBB60_5:
	mov	w0, #-1
	ret
.Lfunc_end60:
	.size	blk_mq_get_io_in_list_count, .Lfunc_end60-blk_mq_get_io_in_list_count
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_recovery_pwron_info_done
	.type	mas_blk_recovery_pwron_info_done,@function
mas_blk_recovery_pwron_info_done:       // @mas_blk_recovery_pwron_info_done
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	and	w2, w1, #0xff
	cmp	w2, #2                  // =2
	mov	x29, sp
	b.ne	.LBB61_2
// %bb.1:
	adrp	x8, g_recovery_pwron_info
	add	x8, x8, :lo12:g_recovery_pwron_info
	mov	w9, #1
	str	w0, [x8, #4]
	str	w9, [x8]
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB61_2:
	adrp	x0, .L.str.29
	adrp	x1, .L__func__.mas_blk_recovery_pwron_info_done
	add	x0, x0, :lo12:.L.str.29
	add	x1, x1, :lo12:.L__func__.mas_blk_recovery_pwron_info_done
	bl	printk
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end61:
	.size	mas_blk_recovery_pwron_info_done, .Lfunc_end61-mas_blk_recovery_pwron_info_done
                                        // -- End function
	.p2align	2               // -- Begin function __ufs_turbo_check_timer_expire
	.type	__ufs_turbo_check_timer_expire,@function
__ufs_turbo_check_timer_expire:         // @__ufs_turbo_check_timer_expire
// %bb.0:
	sub	sp, sp, #64             // =64
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x21, [sp, #32]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	adrp	x19, ufs_turbo_check_timer_expire.check_count
	sub	x8, x0, #320            // =320
	sub	x9, x0, #328            // =328
	sub	x10, x0, #300           // =300
	sub	x11, x0, #304           // =304
	sub	x12, x0, #312           // =312
	sub	x13, x0, #308           // =308
	ldrb	w3, [x0, #192]
	ldr	w2, [x19, :lo12:ufs_turbo_check_timer_expire.check_count]
	ldr	w4, [x8]
	ldr	w5, [x9]
	ldr	w6, [x10]
	ldr	w7, [x11]
	ldr	w8, [x12]
	ldr	w9, [x13]
	sub	x20, x0, #344           // =344
	adrp	x0, .L.str.45
	adrp	x1, .L__func__.ufs_turbo_check_timer_expire
	add	x0, x0, :lo12:.L.str.45
	add	x1, x1, :lo12:.L__func__.ufs_turbo_check_timer_expire
	add	x29, sp, #16            // =16
	str	w9, [sp, #8]
	str	w8, [sp]
	bl	printk
	ldr	x0, [x20]
	ldr	x21, [x0, #40]
	bl	mas_blk_get_queue_by_lld
	mov	w1, wzr
	blr	x21
	ldr	w8, [x19, :lo12:ufs_turbo_check_timer_expire.check_count]
	cbz	w8, .LBB62_3
// %bb.1:
	ldr	w9, [x20, #24]
	ldr	w10, [x20, #16]
	add	w9, w10, w9
	cmp	w9, #0                  // =0
	b.le	.LBB62_4
// %bb.2:
	mov	w9, #1
	b	.LBB62_5
.LBB62_3:
	mov	w8, #18
	strb	wzr, [x20, #536]
	str	w8, [x19, :lo12:ufs_turbo_check_timer_expire.check_count]
	b	.LBB62_6
.LBB62_4:
	mov	w0, #1
	bl	blk_power_off_flush
	ldr	w8, [x19, :lo12:ufs_turbo_check_timer_expire.check_count]
	mov	w9, wzr
.LBB62_5:
	sub	w8, w8, #1              // =1
	strb	w9, [x20, #536]
	str	w8, [x19, :lo12:ufs_turbo_check_timer_expire.check_count]
	adrp	x8, jiffies
	ldr	x8, [x8, :lo12:jiffies]
	add	x0, x20, #344           // =344
	add	x1, x8, #125            // =125
	bl	mod_timer
.LBB62_6:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldr	x21, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #64             // =64
	ret
.Lfunc_end62:
	.size	__ufs_turbo_check_timer_expire, .Lfunc_end62-__ufs_turbo_check_timer_expire
                                        // -- End function
	.p2align	2               // -- Begin function ufs_datasync_work
	.type	ufs_datasync_work,@function
ufs_datasync_work:                      // @ufs_datasync_work
// %bb.0:
	sub	sp, sp, #32             // =32
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	adrp	x0, .L.str.46
	mov	w9, #1
	add	x0, x0, :lo12:.L.str.46
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	str	x8, [sp, #8]
	str	w9, [sp, #4]
	bl	printk
	adrp	x0, ufs_sync_fs
	add	x0, x0, :lo12:ufs_sync_fs
	add	x1, sp, #4              // =4
	bl	iterate_supers
	adrp	x0, .L.str.47
	add	x0, x0, :lo12:.L.str.47
	bl	printk
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB63_2
// %bb.1:
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #32             // =32
	ret
.LBB63_2:
	bl	__stack_chk_fail
.Lfunc_end63:
	.size	ufs_datasync_work, .Lfunc_end63-ufs_datasync_work
                                        // -- End function
	.p2align	2               // -- Begin function ufs_sync_fs
	.type	ufs_sync_fs,@function
ufs_sync_fs:                            // @ufs_sync_fs
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldrb	w8, [x0, #80]
	mov	x29, sp
	tbnz	w8, #0, .LBB64_3
// %bb.1:
	ldr	x8, [x0, #48]
	ldr	x8, [x8, #64]
	cbz	x8, .LBB64_3
// %bb.2:
	ldr	w1, [x1]
	blr	x8
.LBB64_3:
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end64:
	.size	ufs_sync_fs, .Lfunc_end64-ufs_sync_fs
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_async_sched_fifo_init
	.type	ufs_mq_async_sched_fifo_init,@function
ufs_mq_async_sched_fifo_init:           // @ufs_mq_async_sched_fifo_init
// %bb.0:
	ldrb	w8, [x0, #104]
	cbz	w8, .LBB65_2
// %bb.1:
	ret
.LBB65_2:
	add	x8, x0, #112            // =112
	mov	w9, #1
	str	x8, [x0, #112]
	str	x8, [x0, #120]
	strb	w9, [x0, #104]
	ret
.Lfunc_end65:
	.size	ufs_mq_async_sched_fifo_init, .Lfunc_end65-ufs_mq_async_sched_fifo_init
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_async_sched_fifo_insert
	.type	ufs_mq_async_sched_fifo_insert,@function
ufs_mq_async_sched_fifo_insert:         // @ufs_mq_async_sched_fifo_insert
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x19, x0
	ldr	x0, [x0]
	mov	x29, sp
	mov	x20, x1
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB66_2
// %bb.1:
	ldr	x9, [x20, #120]
	add	x8, x19, #80            // =80
	add	x10, x20, #112          // =112
	str	x8, [x20, #120]
	stp	x10, x9, [x19, #80]
	b	.LBB66_3
.LBB66_2:
	ldr	x9, [x20, #120]
	add	x8, x19, #768           // =768
	add	x10, x20, #112          // =112
	str	x8, [x20, #120]
	str	x10, [x19, #768]
	str	x9, [x19, #776]
.LBB66_3:
	str	x8, [x9]
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end66:
	.size	ufs_mq_async_sched_fifo_insert, .Lfunc_end66-ufs_mq_async_sched_fifo_insert
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_async_sched_fifo_seek
	.type	ufs_mq_async_sched_fifo_seek,@function
ufs_mq_async_sched_fifo_seek:           // @ufs_mq_async_sched_fifo_seek
// %bb.0:
	mov	x9, x0
	ldr	x8, [x9, #112]!
	cmp	x8, x9
	b.ne	.LBB67_2
// %bb.1:
	ldr	x10, [x0, #120]
	cmp	x10, x9
	b.eq	.LBB67_3
.LBB67_2:
	ldr	x9, [x0]
	ldp	x11, x10, [x8]
	mov	x12, #-80
	mov	x13, #-768
	ldr	x9, [x9, #24]
	str	x10, [x11, #8]
	str	x11, [x10]
	str	x8, [x8]
	tst	x9, #0x200
	csel	x9, x13, x12, eq
	add	x0, x8, x9
	str	x8, [x8, #8]
	ret
.LBB67_3:
	mov	x0, xzr
	ret
.Lfunc_end67:
	.size	ufs_mq_async_sched_fifo_seek, .Lfunc_end67-ufs_mq_async_sched_fifo_seek
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_async_sched_fifo_requeue
	.type	ufs_mq_async_sched_fifo_requeue,@function
ufs_mq_async_sched_fifo_requeue:        // @ufs_mq_async_sched_fifo_requeue
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x20, x0
	ldr	x0, [x0]
	mov	x29, sp
	mov	x19, x1
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB68_2
// %bb.1:
	ldr	x9, [x19, #112]!
	add	x8, x20, #80            // =80
	str	x8, [x9, #8]
	stp	x9, x19, [x20, #80]
	b	.LBB68_3
.LBB68_2:
	ldr	x9, [x19, #112]!
	add	x8, x20, #768           // =768
	str	x8, [x9, #8]
	str	x9, [x20, #768]
	str	x19, [x20, #776]
.LBB68_3:
	str	x8, [x19]
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end68:
	.size	ufs_mq_async_sched_fifo_requeue, .Lfunc_end68-ufs_mq_async_sched_fifo_requeue
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_async_sched_fifo_attempt_merge_bio
	.type	ufs_mq_async_sched_fifo_attempt_merge_bio,@function
ufs_mq_async_sched_fifo_attempt_merge_bio: // @ufs_mq_async_sched_fifo_attempt_merge_bio
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	ldr	x8, [x1, #1392]
	str	x23, [sp, #16]          // 8-byte Folded Spill
	mov	x20, x0
	mov	x0, x1
	ldr	x23, [x8, #536]
	mov	x29, sp
	mov	w19, w2
	mov	x21, x1
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB69_9
// %bb.1:
	ldr	x8, [x21, #1392]
	ldr	x8, [x8, #536]
	ldr	x22, [x8, #120]
	add	x23, x8, #112           // =112
	b	.LBB69_4
.LBB69_2:                               //   in Loop: Header=BB69_4 Depth=1
	mov	x0, x21
	mov	x1, x20
	mov	w2, w19
	bl	bio_attempt_front_merge
	tbnz	w0, #0, .LBB69_17
.LBB69_3:                               //   in Loop: Header=BB69_4 Depth=1
	ldr	x22, [x22, #8]
.LBB69_4:                               // =>This Inner Loop Header: Depth=1
	cmp	x23, x22
	b.eq	.LBB69_18
// %bb.5:                               //   in Loop: Header=BB69_4 Depth=1
	sub	x21, x22, #80           // =80
	mov	x0, x21
	mov	x1, x20
	bl	blk_rq_merge_ok
	tbz	w0, #0, .LBB69_3
// %bb.6:                               //   in Loop: Header=BB69_4 Depth=1
	mov	x0, x21
	mov	x1, x20
	bl	blk_try_merge
	cmp	w0, #1                  // =1
	b.eq	.LBB69_2
// %bb.7:                               //   in Loop: Header=BB69_4 Depth=1
	cmp	w0, #2                  // =2
	b.ne	.LBB69_3
// %bb.8:                               //   in Loop: Header=BB69_4 Depth=1
	mov	x0, x21
	mov	x1, x20
	mov	w2, w19
	bl	bio_attempt_back_merge
	tbz	w0, #0, .LBB69_3
	b	.LBB69_17
.LBB69_9:
	ldr	x22, [x23, #120]
	add	x23, x23, #112          // =112
	b	.LBB69_12
.LBB69_10:                              //   in Loop: Header=BB69_12 Depth=1
	mov	x0, x21
	mov	x1, x20
	mov	w2, w19
	bl	bio_attempt_front_merge
	tbnz	w0, #0, .LBB69_17
.LBB69_11:                              //   in Loop: Header=BB69_12 Depth=1
	ldr	x22, [x22, #8]
.LBB69_12:                              // =>This Inner Loop Header: Depth=1
	cmp	x23, x22
	b.eq	.LBB69_18
// %bb.13:                              //   in Loop: Header=BB69_12 Depth=1
	sub	x21, x22, #768          // =768
	mov	x0, x21
	mov	x1, x20
	bl	blk_rq_merge_ok
	tbz	w0, #0, .LBB69_11
// %bb.14:                              //   in Loop: Header=BB69_12 Depth=1
	mov	x0, x21
	mov	x1, x20
	bl	blk_try_merge
	cmp	w0, #1                  // =1
	b.eq	.LBB69_10
// %bb.15:                              //   in Loop: Header=BB69_12 Depth=1
	cmp	w0, #2                  // =2
	b.ne	.LBB69_11
// %bb.16:                              //   in Loop: Header=BB69_12 Depth=1
	mov	x0, x21
	mov	x1, x20
	mov	w2, w19
	bl	bio_attempt_back_merge
	tbz	w0, #0, .LBB69_11
.LBB69_17:
	mov	w0, #1
	b	.LBB69_19
.LBB69_18:
	mov	w0, wzr
.LBB69_19:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.Lfunc_end69:
	.size	ufs_mq_async_sched_fifo_attempt_merge_bio, .Lfunc_end69-ufs_mq_async_sched_fifo_attempt_merge_bio
                                        // -- End function
	.p2align	2               // -- Begin function ufs_mq_async_sched_fifo_empty
	.type	ufs_mq_async_sched_fifo_empty,@function
ufs_mq_async_sched_fifo_empty:          // @ufs_mq_async_sched_fifo_empty
// %bb.0:
	ldr	x8, [x0, #112]!
	cmp	x0, x8
	cset	w0, eq
	ret
.Lfunc_end70:
	.size	ufs_mq_async_sched_fifo_empty, .Lfunc_end70-ufs_mq_async_sched_fifo_empty
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\0013[BLK-IO]q get budget %d"
	.size	.L.str, 26

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"\0013[BLK-IO]q put budget %d"
	.size	.L.str.1, 26

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"\0013[BLK-IO]hp_sync_disp_list:\n"
	.size	.L.str.2, 30

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"\0013[BLK-IO]sync_disp_list:\n"
	.size	.L.str.3, 27

	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"\0013[BLK-IO]last_run_cnt: %lld"
	.size	.L.str.4, 29

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"\0013[BLK-IO]last_queue_tm(%d): %lld"
	.size	.L.str.5, 34

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"\0013[BLK-IO]async_fifo_list:\n"
	.size	.L.str.6, 28

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"cp_enabled: %d\n"
	.size	.L.str.7, 16

	.type	cp_debug_en,@object     // @cp_debug_en
	.local	cp_debug_en
	.comm	cp_debug_en,4,4
	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"cp_debug_en: %d\n"
	.size	.L.str.8, 17

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"cp_io_limit: %u\n"
	.size	.L.str.9, 17

	.type	.L__const.ufs_mq_flush_plug_list.bd,@object // @__const.ufs_mq_flush_plug_list.bd
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	3
.L__const.ufs_mq_flush_plug_list.bd:
	.xword	0
	.byte	1                       // 0x1
	.zero	7
	.size	.L__const.ufs_mq_flush_plug_list.bd, 16

	.type	g_recovery_pwron_info,@object // @g_recovery_pwron_info
	.bss
	.globl	g_recovery_pwron_info
	.p2align	3
g_recovery_pwron_info:
.Lg_recovery_pwron_info$local:
	.zero	80
	.size	g_recovery_pwron_info, 80

	.type	io_guard_queue_list_lock,@object // @io_guard_queue_list_lock
	.local	io_guard_queue_list_lock
	.comm	io_guard_queue_list_lock,4,4
	.type	mas_io_guard_queue_list,@object // @mas_io_guard_queue_list
	.data
	.p2align	3
mas_io_guard_queue_list:
	.xword	mas_io_guard_queue_list
	.xword	mas_io_guard_queue_list
	.size	mas_io_guard_queue_list, 16

	.type	mas_blk_io_guard_wq,@object // @mas_blk_io_guard_wq
	.local	mas_blk_io_guard_wq
	.comm	mas_blk_io_guard_wq,8,8
	.type	mas_io_guard_work,@object // @mas_io_guard_work
	.local	mas_io_guard_work
	.comm	mas_io_guard_work,136,8
	.type	.L.str.11,@object       // @.str.11
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.11:
	.asciz	"Reinit unreleased request!"
	.size	.L.str.11, 27

	.type	mas_blk_mq_sync_disp_wq,@object // @mas_blk_mq_sync_disp_wq
	.local	mas_blk_mq_sync_disp_wq
	.comm	mas_blk_mq_sync_disp_wq,8,8
	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"\0013[BLK-IO]%s: vip_wait_cnt: %d\n"
	.size	.L.str.12, 32

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"\0013[BLK-IO]%s: h_tag_used_cnt: %d tag_used_cnt: %d r_tag_used_cnt: %d\n"
	.size	.L.str.13, 70

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"\0013[BLK-IO]%s: fg_inflt: %d, vip_inflt: %d, s_inflt: %d a_inflt: %d, cp_inflt: %d\n"
	.size	.L.str.14, 82

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"\0013[BLK-IO]%s %d Failed to alloc sched_ds!\n"
	.size	.L.str.15, 43

	.type	.L__func__.ufs_mq_iosched_init,@object // @__func__.ufs_mq_iosched_init
.L__func__.ufs_mq_iosched_init:
	.asciz	"ufs_mq_iosched_init"
	.size	.L__func__.ufs_mq_iosched_init, 20

	.type	mas_ufs_mq_async_io_fifo_sched,@object // @mas_ufs_mq_async_io_fifo_sched
	.data
	.p2align	3
mas_ufs_mq_async_io_fifo_sched:
	.word	0                       // 0x0
	.zero	4
	.xword	ufs_mq_async_sched_fifo_init
	.xword	ufs_mq_async_sched_fifo_insert
	.xword	ufs_mq_async_sched_fifo_seek
	.xword	ufs_mq_async_sched_fifo_requeue
	.xword	ufs_mq_async_sched_fifo_attempt_merge_bio
	.xword	ufs_mq_async_sched_fifo_empty
	.size	mas_ufs_mq_async_io_fifo_sched, 56

	.type	mas_ufs_mq,@object      // @mas_ufs_mq
	.globl	mas_ufs_mq
	.p2align	3
mas_ufs_mq:
.Lmas_ufs_mq$local:
	.xword	mas_ufs_mq_async_io_fifo_sched
	.size	mas_ufs_mq, 8

	.type	.L.str.16,@object       // @.str.16
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.16:
	.asciz	"\0013[BLK-IO]%s, reserved tag used cnt: %d\n"
	.size	.L.str.16, 41

	.type	.L__func__.ufs_mq_get_recovery_tag,@object // @__func__.ufs_mq_get_recovery_tag
.L__func__.ufs_mq_get_recovery_tag:
	.asciz	"ufs_mq_get_recovery_tag"
	.size	.L__func__.ufs_mq_get_recovery_tag, 24

	.type	.L.str.17,@object       // @.str.17
.L.str.17:
	.asciz	"\0013[BLK-IO]%s, there is no tag left\n"
	.size	.L.str.17, 36

	.type	.L__func__.ufs_mq_get_all_tag,@object // @__func__.ufs_mq_get_all_tag
.L__func__.ufs_mq_get_all_tag:
	.asciz	"ufs_mq_get_all_tag"
	.size	.L__func__.ufs_mq_get_all_tag, 19

	.type	.L.str.18,@object       // @.str.18
.L.str.18:
	.asciz	"nr_high_prio_tags is zero!"
	.size	.L.str.18, 27

	.type	.L.str.19,@object       // @.str.19
.L.str.19:
	.asciz	"nr_reserved_tags is zero!"
	.size	.L.str.19, 26

	.type	.L.str.20,@object       // @.str.20
.L.str.20:
	.asciz	"\0013[BLK-IO]lst_in_t: %lld, lst_out_t: %lld, fail_exit: %lld, exit_cnt: %lld\n"
	.size	.L.str.20, 76

	.type	.L.str.21,@object       // @.str.21
.L.str.21:
	.asciz	"\0013[BLK-IO]CP limited to %d!\n"
	.size	.L.str.21, 29

	.type	.L.str.22,@object       // @.str.22
.L.str.22:
	.asciz	"\0013[BLK-IO]Forbid CP IO!\n"
	.size	.L.str.22, 25

	.type	.L.str.23,@object       // @.str.23
.L.str.23:
	.asciz	"rq has cp flag set already!"
	.size	.L.str.23, 28

	.type	.L.str.24,@object       // @.str.24
.L.str.24:
	.asciz	"\0013[BLK-IO]mas_cp: add cp flag, %s\n"
	.size	.L.str.24, 35

	.type	.L.str.25,@object       // @.str.25
.L.str.25:
	.asciz	"\0013[BLK-IO]%s ret %d\n"
	.size	.L.str.25, 21

	.type	.L__func__.mas_blk_recovery_pwron_info_sync,@object // @__func__.mas_blk_recovery_pwron_info_sync
.L__func__.mas_blk_recovery_pwron_info_sync:
	.asciz	"mas_blk_recovery_pwron_info_sync"
	.size	.L__func__.mas_blk_recovery_pwron_info_sync, 33

	.type	.L.str.26,@object       // @.str.26
.L.str.26:
	.asciz	"\0013[BLK-IO]%s ret %d"
	.size	.L.str.26, 20

	.type	.L__func__.mas_blk_recovery_done_proc,@object // @__func__.mas_blk_recovery_done_proc
.L__func__.mas_blk_recovery_done_proc:
	.asciz	"mas_blk_recovery_done_proc"
	.size	.L__func__.mas_blk_recovery_done_proc, 27

	.type	.L.str.27,@object       // @.str.27
.L.str.27:
	.asciz	"\0013[BLK-IO]recovery_stor, %d, pre_pu: 0x%llx (0x%llx - 0x%llx) - 0x%llx, pre_lba: 0x%llx - 0x%llx, new_lba: 0x%llx - 0x%llx\n"
	.size	.L.str.27, 124

	.type	.L.str.28,@object       // @.str.28
.L.str.28:
	.asciz	"\0013[BLK-IO]recovery_add, stream:%u, bio:0x%llx - 0x%llx, len:%u, done: %u\n"
	.size	.L.str.28, 74

	.type	.L.str.29,@object       // @.str.29
.L.str.29:
	.asciz	"\0013[BLK-IO]%s pwron_type is err %u\n"
	.size	.L.str.29, 35

	.type	.L__func__.mas_blk_recovery_pwron_info_done,@object // @__func__.mas_blk_recovery_pwron_info_done
.L__func__.mas_blk_recovery_pwron_info_done:
	.asciz	"mas_blk_recovery_pwron_info_done"
	.size	.L__func__.mas_blk_recovery_pwron_info_done, 33

	.type	.L.str.30,@object       // @.str.30
.L.str.30:
	.asciz	"\0013[BLK-IO]%s - lrb_in_use = 0x%llx\n"
	.size	.L.str.30, 36

	.type	.L__func__.ufs_mq_recovery_dispatch,@object // @__func__.ufs_mq_recovery_dispatch
.L__func__.ufs_mq_recovery_dispatch:
	.asciz	"ufs_mq_recovery_dispatch"
	.size	.L__func__.ufs_mq_recovery_dispatch, 25

	.type	.L.str.31,@object       // @.str.31
.L.str.31:
	.asciz	"unistore_reset!"
	.size	.L.str.31, 16

	.type	.L.str.32,@object       // @.str.32
.L.str.32:
	.asciz	"\0013[BLK-IO]recovery_disp, stream:%u, bio:0x%llx - 0x%llx, len:%u, done:%u\n"
	.size	.L.str.32, 74

	.type	.L.str.34,@object       // @.str.34
.L.str.34:
	.asciz	"\0013[BLK-IO]%s - section size: %u, pu size: %u\n"
	.size	.L.str.34, 46

	.type	.L__func__.ufs_mq_dispatch_match_expected_lba,@object // @__func__.ufs_mq_dispatch_match_expected_lba
.L__func__.ufs_mq_dispatch_match_expected_lba:
	.asciz	"ufs_mq_dispatch_match_expected_lba"
	.size	.L__func__.ufs_mq_dispatch_match_expected_lba, 35

	.type	.L.str.36,@object       // @.str.36
.L.str.36:
	.asciz	"unknown sb_dispatch_level!"
	.size	.L.str.36, 27

	.type	mas_blk_mq_async_disp_wq,@object // @mas_blk_mq_async_disp_wq
	.local	mas_blk_mq_async_disp_wq
	.comm	mas_blk_mq_async_disp_wq,8,8
	.type	.L.str.37,@object       // @.str.37
.L.str.37:
	.asciz	"\0013[BLK-IO]%s\n"
	.size	.L.str.37, 14

	.type	.L.str.38,@object       // @.str.38
.L.str.38:
	.asciz	"high_prio_sync_io is 0!"
	.size	.L.str.38, 24

	.type	.L.str.39,@object       // @.str.39
.L.str.39:
	.asciz	"sync_io_inflt_cnt is 0!"
	.size	.L.str.39, 24

	.type	.L.str.40,@object       // @.str.40
.L.str.40:
	.asciz	"async_io_inflt_cnt is 0!"
	.size	.L.str.40, 25

	.type	.L.str.41,@object       // @.str.41
.L.str.41:
	.asciz	"cp_io_inflt_cnt is zero!"
	.size	.L.str.41, 25

	.type	.L.str.42,@object       // @.str.42
.L.str.42:
	.asciz	"async_dispatch_work_trigger_judgement error!"
	.size	.L.str.42, 45

	.type	.L.str.43,@object       // @.str.43
.L.str.43:
	.asciz	"dump"
	.size	.L.str.43, 5

	.type	.L.str.44,@object       // @.str.44
.L.str.44:
	.asciz	"io_latency"
	.size	.L.str.44, 11

	.type	ufs_turbo_check_timer_expire.check_count,@object // @ufs_turbo_check_timer_expire.check_count
	.data
	.p2align	2
ufs_turbo_check_timer_expire.check_count:
	.word	18                      // 0x12
	.size	ufs_turbo_check_timer_expire.check_count, 4

	.type	.L.str.45,@object       // @.str.45
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.45:
	.asciz	"\0013[BLK-IO]%s: check_count = %d turbo_mode = %d prio io = %d, sync io = %d fg_inflt: %d, vip_inflt: %d, s_inflt: %d a_inflt: %d\n"
	.size	.L.str.45, 128

	.type	.L__func__.ufs_turbo_check_timer_expire,@object // @__func__.ufs_turbo_check_timer_expire
.L__func__.ufs_turbo_check_timer_expire:
	.asciz	"ufs_turbo_check_timer_expire"
	.size	.L__func__.ufs_turbo_check_timer_expire, 29

	.type	.L.str.46,@object       // @.str.46
.L.str.46:
	.asciz	"\0013[BLK-IO]UFS Sync start\n"
	.size	.L.str.46, 26

	.type	.L.str.47,@object       // @.str.47
.L.str.47:
	.asciz	"\0013[BLK-IO]UFS Sync complete\n"
	.size	.L.str.47, 29

	.type	.L.str.48,@object       // @.str.48
.L.str.48:
	.asciz	"sync_dispatch"
	.size	.L.str.48, 14

	.type	.L.str.49,@object       // @.str.49
.L.str.49:
	.asciz	"\0013[BLK-IO]%s %d Failed to alloc sync_dispatch_workqueue\n"
	.size	.L.str.49, 57

	.type	.L__func__.ufs_mq_workqueue_init,@object // @__func__.ufs_mq_workqueue_init
.L__func__.ufs_mq_workqueue_init:
	.asciz	"ufs_mq_workqueue_init"
	.size	.L__func__.ufs_mq_workqueue_init, 22

	.type	.L.str.50,@object       // @.str.50
.L.str.50:
	.asciz	"async_dispatch"
	.size	.L.str.50, 15

	.type	.L.str.51,@object       // @.str.51
.L.str.51:
	.asciz	"io_guard"
	.size	.L.str.51, 9

	.type	.L.str.52,@object       // @.str.52
.L.str.52:
	.asciz	"\0013[BLK-IO]%s init Failed!\n"
	.size	.L.str.52, 27

	.type	.Lswitch.table.ufs_mq_sync_dispatch,@object // @switch.table.ufs_mq_sync_dispatch
	.section	.rodata,"a",@progbits
	.p2align	2
.Lswitch.table.ufs_mq_sync_dispatch:
	.word	0                       // 0x0
	.word	0                       // 0x0
	.word	0                       // 0x0
	.word	0                       // 0x0
	.word	1                       // 0x1
	.word	0                       // 0x0
	.word	0                       // 0x0
	.size	.Lswitch.table.ufs_mq_sync_dispatch, 28

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
