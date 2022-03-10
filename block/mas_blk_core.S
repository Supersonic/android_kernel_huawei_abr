	.text
	.file	"mas_blk_core.c"
	.globl	mas_blk_get_lld         // -- Begin function mas_blk_get_lld
	.p2align	2
	.type	mas_blk_get_lld,@function
mas_blk_get_lld:                        // @mas_blk_get_lld
.Lmas_blk_get_lld$local:
// %bb.0:
	ldr	x8, [x0, #48]
	cbz	x8, .LBB0_2
// %bb.1:
	ldr	x8, [x0, #3968]
	add	x9, x0, #1408           // =1408
	add	x10, x8, #64            // =64
	cmp	x8, #0                  // =0
	csel	x0, x9, x10, eq
	ret
.LBB0_2:
	add	x0, x0, #1408           // =1408
	ret
.Lfunc_end0:
	.size	mas_blk_get_lld, .Lfunc_end0-mas_blk_get_lld
                                        // -- End function
	.globl	mas_blk_get_queue_by_lld // -- Begin function mas_blk_get_queue_by_lld
	.p2align	2
	.type	mas_blk_get_queue_by_lld,@function
mas_blk_get_queue_by_lld:               // @mas_blk_get_queue_by_lld
.Lmas_blk_get_queue_by_lld$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	w8, [x0, #4]
	mov	x29, sp
	cmp	w8, #2                  // =2
	b.eq	.LBB1_5
// %bb.1:
	cmp	w8, #1                  // =1
	b.eq	.LBB1_4
// %bb.2:
	cbnz	w8, .LBB1_7
// %bb.3:
	ldr	x0, [x0, #8]
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB1_4:
	ldr	x8, [x0, #8]
	ldr	x8, [x8, #88]
	b	.LBB1_6
.LBB1_5:
	ldr	x8, [x0, #8]
	ldr	x8, [x8, #2128]
.LBB1_6:
	sub	x0, x8, #3976           // =3976
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB1_7:
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	mas_blk_rdr_panic
.Lfunc_end1:
	.size	mas_blk_get_queue_by_lld, .Lfunc_end1-mas_blk_get_queue_by_lld
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_rdr_panic
	.type	mas_blk_rdr_panic,@function
mas_blk_rdr_panic:                      // @mas_blk_rdr_panic
// %bb.0:
	cbz	x0, .LBB2_2
// %bb.1:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x1, x0
	adrp	x0, .L.str.7
	add	x0, x0, :lo12:.L.str.7
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
	.globl	blk_dio_ck              // -- Begin function blk_dio_ck
	.p2align	2
	.type	blk_dio_ck,@function
blk_dio_ck:                             // @blk_dio_ck
.Lblk_dio_ck$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB3_12
// %bb.1:
	ldr	x8, [x0, #1272]
	cbz	x8, .LBB3_12
// %bb.2:
	mov	w19, w3
	mov	x20, x1
	cmp	w2, #1                  // =1
	b.eq	.LBB3_7
// %bb.3:
	cbnz	w2, .LBB3_12
// %bb.4:
	ldr	x9, [x8, #48]
	cbz	x9, .LBB3_13
// %bb.5:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x8, x8, x10, eq
.LBB3_6:
	ldrsw	x21, [x8, #1952]
	cbnz	w21, .LBB3_9
	b	.LBB3_12
.LBB3_7:
	ldr	x9, [x8, #48]
	cbz	x9, .LBB3_14
// %bb.8:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x8, x8, x10, eq
	ldrsw	x21, [x8, #1956]
	cbz	w21, .LBB3_12
.LBB3_9:
	bl	ktime_get
	sub	x8, x0, x20
	cmn	x8, #999                // =999
	b.lt	.LBB3_12
// %bb.10:
	mov	x9, #63439
	movk	x9, #58195, lsl #16
	movk	x9, #39845, lsl #32
	movk	x9, #8388, lsl #48
	mov	w10, #3906
	smulh	x8, x8, x9
	smull	x9, w19, w10
	asr	x10, x8, #7
	add	x8, x10, x8, lsr #63
	sdiv	x9, x9, x21
	sub	x0, x9, x8
	cmp	x0, #1                  // =1
	b.lt	.LBB3_12
// %bb.11:
	bl	__udelay
.LBB3_12:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB3_13:
	add	x8, x8, #1408           // =1408
	b	.LBB3_6
.LBB3_14:
	add	x8, x8, #1408           // =1408
	ldrsw	x21, [x8, #1956]
	cbnz	w21, .LBB3_9
	b	.LBB3_12
.Lfunc_end3:
	.size	blk_dio_ck, .Lfunc_end3-blk_dio_ck
                                        // -- End function
	.globl	blk_mq_tagset_vl_setup  // -- Begin function blk_mq_tagset_vl_setup
	.p2align	2
	.type	blk_mq_tagset_vl_setup,@function
blk_mq_tagset_vl_setup:                 // @blk_mq_tagset_vl_setup
.Lblk_mq_tagset_vl_setup$local:
// %bb.0:
	lsr	x8, x1, #28
	mov	w9, #500
	mov	w10, #300
	cmp	x8, #0                  // =0
	csel	w8, w10, w9, eq
	str	wzr, [x0, #2016]
	str	w8, [x0, #2020]
	ret
.Lfunc_end4:
	.size	blk_mq_tagset_vl_setup, .Lfunc_end4-blk_mq_tagset_vl_setup
                                        // -- End function
	.globl	blk_queue_set_inline_crypto_flag // -- Begin function blk_queue_set_inline_crypto_flag
	.p2align	2
	.type	blk_queue_set_inline_crypto_flag,@function
blk_queue_set_inline_crypto_flag:       // @blk_queue_set_inline_crypto_flag
.Lblk_queue_set_inline_crypto_flag$local:
// %bb.0:
	ldr	x8, [x0, #48]
	cbz	x8, .LBB5_3
// %bb.1:
	ldr	x8, [x0, #3968]
	add	x9, x0, #1408           // =1408
	add	x10, x8, #64            // =64
	cmp	x8, #0                  // =0
	csel	x8, x9, x10, eq
.LBB5_2:
	ldr	x9, [x8, #24]
	tst	w1, #0x1
	and	x10, x9, #0xffffffffffffffef
	orr	x9, x9, #0x10
	csel	x9, x9, x10, ne
	str	x9, [x8, #24]
	ret
.LBB5_3:
	add	x8, x0, #1408           // =1408
	b	.LBB5_2
.Lfunc_end5:
	.size	blk_queue_set_inline_crypto_flag, .Lfunc_end5-blk_queue_set_inline_crypto_flag
                                        // -- End function
	.globl	mas_blk_queue_usr_ctrl_set // -- Begin function mas_blk_queue_usr_ctrl_set
	.p2align	2
	.type	mas_blk_queue_usr_ctrl_set,@function
mas_blk_queue_usr_ctrl_set:             // @mas_blk_queue_usr_ctrl_set
.Lmas_blk_queue_usr_ctrl_set$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x8, [x0, #1080]
	mov	w9, #28570
	movk	w9, #4, lsl #16
	mov	x29, sp
	cmp	x8, x9
	b.ne	.LBB6_2
// %bb.1:
	mov	w8, #100
	str	x8, [x0, #1328]
	adrp	x8, jiffies
	ldr	x8, [x8, :lo12:jiffies]
	mov	w9, #48032
	movk	w9, #13, lsl #16
	add	x0, x0, #1336           // =1336
	add	x1, x8, x9
	bl	mod_timer
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB6_2:
	str	xzr, [x0, #1328]
	add	x0, x0, #1336           // =1336
	bl	del_timer_sync
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end6:
	.size	mas_blk_queue_usr_ctrl_set, .Lfunc_end6-mas_blk_queue_usr_ctrl_set
                                        // -- End function
	.globl	mas_blk_queue_usr_ctrl_recovery_timer_expire // -- Begin function mas_blk_queue_usr_ctrl_recovery_timer_expire
	.p2align	2
	.type	mas_blk_queue_usr_ctrl_recovery_timer_expire,@function
mas_blk_queue_usr_ctrl_recovery_timer_expire: // @mas_blk_queue_usr_ctrl_recovery_timer_expire
.Lmas_blk_queue_usr_ctrl_recovery_timer_expire$local:
// %bb.0:
	str	xzr, [x0, #1080]
	str	xzr, [x0, #1328]
	ret
.Lfunc_end7:
	.size	mas_blk_queue_usr_ctrl_recovery_timer_expire, .Lfunc_end7-mas_blk_queue_usr_ctrl_recovery_timer_expire
                                        // -- End function
	.globl	mas_blk_bio_clone_fast  // -- Begin function mas_blk_bio_clone_fast
	.p2align	2
	.type	mas_blk_bio_clone_fast,@function
mas_blk_bio_clone_fast:                 // @mas_blk_bio_clone_fast
.Lmas_blk_bio_clone_fast$local:
// %bb.0:
	ldr	x8, [x1, #88]
	str	x8, [x0, #88]
	ldrb	w8, [x1, #272]
	strb	w8, [x0, #272]
	ldrb	w8, [x1, #273]
	strb	w8, [x0, #273]
	ldr	x8, [x1, #280]
	str	x8, [x0, #280]
	ldr	x8, [x1, #288]
	str	x8, [x0, #288]
	ldrb	w8, [x1, #296]
	strb	w8, [x0, #296]
	ldrb	w8, [x1, #297]
	strb	w8, [x0, #297]
	ret
.Lfunc_end8:
	.size	mas_blk_bio_clone_fast, .Lfunc_end8-mas_blk_bio_clone_fast
                                        // -- End function
	.globl	mas_blk_bio_queue_split // -- Begin function mas_blk_bio_queue_split
	.p2align	2
	.type	mas_blk_bio_queue_split,@function
mas_blk_bio_queue_split:                // @mas_blk_bio_queue_split
.Lmas_blk_bio_queue_split$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	x8, [x1]
	mov	x20, x0
	mov	x29, sp
	mov	x19, x2
	mov	x0, x8
	bl	mas_blk_busyidle_check_bio_endio
	mov	x0, x20
	mov	x1, x19
	bl	mas_blk_busyidle_check_bio
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end9:
	.size	mas_blk_bio_queue_split, .Lfunc_end9-mas_blk_bio_queue_split
                                        // -- End function
	.globl	mas_blk_bio_split_pre   // -- Begin function mas_blk_bio_split_pre
	.p2align	2
	.type	mas_blk_bio_split_pre,@function
mas_blk_bio_split_pre:                  // @mas_blk_bio_split_pre
.Lmas_blk_bio_split_pre$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	x8, [x0, #304]
	mov	x29, sp
	cbz	x8, .LBB10_4
// %bb.1:
	ldr	w9, [x0, #312]
	mov	x19, x0
	mov	x20, x1
	cmp	w9, #48                 // =48
	b.eq	.LBB10_3
// %bb.2:
	cmp	w9, #64                 // =64
	b.ne	.LBB10_5
.LBB10_3:
	str	x8, [x20, #304]
	ldr	w8, [x19, #312]
	str	w8, [x20, #312]
	ldr	w8, [x19, #316]
	str	w8, [x20, #316]
	ldr	x8, [x19, #320]
	str	x8, [x20, #320]
.LBB10_4:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB10_5:
	adrp	x0, .L.str.8
	adrp	x1, .L__func__.mas_blk_lld_inline_crypto_bio_split_pre
	add	x0, x0, :lo12:.L.str.8
	add	x1, x1, :lo12:.L__func__.mas_blk_lld_inline_crypto_bio_split_pre
	bl	printk
	ldr	x8, [x19, #304]
	b	.LBB10_3
.Lfunc_end10:
	.size	mas_blk_bio_split_pre, .Lfunc_end10-mas_blk_bio_split_pre
                                        // -- End function
	.globl	mas_blk_bio_split_post  // -- Begin function mas_blk_bio_split_post
	.p2align	2
	.type	mas_blk_bio_split_post,@function
mas_blk_bio_split_post:                 // @mas_blk_bio_split_post
.Lmas_blk_bio_split_post$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	ldr	x8, [x0, #304]
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x8, .LBB11_4
// %bb.1:
	ldr	w8, [x0, #312]
	mov	x19, x0
	cmp	w8, #48                 // =48
	b.eq	.LBB11_3
// %bb.2:
	cmp	w8, #64                 // =64
	b.ne	.LBB11_5
.LBB11_3:
	ldr	x8, [x19, #480]
	ldp	w9, w10, [x19, #44]
	add	x8, x8, x9, lsl #4
	ldr	w9, [x8, #12]
	ldr	x8, [x8]
	add	w9, w10, w9
	lsr	w9, w9, #12
	add	x8, x8, x9, lsl #6
	ldr	x8, [x8, #32]
	str	x8, [x19, #320]
.LBB11_4:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB11_5:
	adrp	x0, .L.str.9
	adrp	x1, .L__func__.mas_blk_lld_inline_crypto_bio_split_post
	add	x0, x0, :lo12:.L.str.9
	add	x1, x1, :lo12:.L__func__.mas_blk_lld_inline_crypto_bio_split_post
	bl	printk
	b	.LBB11_3
.Lfunc_end11:
	.size	mas_blk_bio_split_post, .Lfunc_end11-mas_blk_bio_split_post
                                        // -- End function
	.globl	mas_blk_bio_merge_allow // -- Begin function mas_blk_bio_merge_allow
	.p2align	2
	.type	mas_blk_bio_merge_allow,@function
mas_blk_bio_merge_allow:                // @mas_blk_bio_merge_allow
.Lmas_blk_bio_merge_allow$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x0]
	mov	x19, x0
	mov	x20, x1
	str	x21, [sp, #16]          // 8-byte Folded Spill
	ldr	x9, [x8, #48]
	mov	x29, sp
	cbz	x9, .LBB12_17
// %bb.1:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x8, x8, x10, eq
	ldrb	w8, [x8, #24]
	tbz	w8, #4, .LBB12_12
.LBB12_2:
	ldr	x8, [x20, #304]
	ldr	x9, [x19, #744]
	cbz	x8, .LBB12_11
// %bb.3:
	cbz	x9, .LBB12_11
// %bb.4:
	ldr	w10, [x20, #312]
	ldr	w11, [x19, #752]
	cmp	w10, w11
	b.ne	.LBB12_15
// %bb.5:
	cmp	w10, #48                // =48
	b.eq	.LBB12_7
// %bb.6:
	cmp	w10, #64                // =64
	b.ne	.LBB12_14
.LBB12_7:
	cmp	x8, x9
	b.ne	.LBB12_15
.LBB12_8:
	ldr	x21, [x19, #72]
	mov	x0, x19
	mov	x1, x20
	bl	blk_try_merge
	cmp	w0, #1                  // =1
	b.eq	.LBB12_13
// %bb.9:
	cmp	w0, #2                  // =2
	b.ne	.LBB12_15
// %bb.10:
	ldr	x8, [x21, #320]
	ldrh	w9, [x21, #466]
	ldr	x10, [x20, #320]
	add	x8, x8, x9
	cmp	x8, x10
	b.eq	.LBB12_12
	b	.LBB12_15
.LBB12_11:
	orr	x8, x8, x9
	cbnz	x8, .LBB12_15
.LBB12_12:
	ldr	w8, [x19, #240]
	ldr	w9, [x20, #328]
	eor	w8, w9, w8, lsr #3
	tst	w8, #0x1
	cset	w0, eq
	b	.LBB12_16
.LBB12_13:
	ldr	x8, [x19, #64]
	ldr	x9, [x20, #320]
	ldrh	w10, [x20, #466]
	ldr	x8, [x8, #320]
	add	x9, x9, x10
	cmp	x9, x8
	b.eq	.LBB12_12
	b	.LBB12_15
.LBB12_14:
	adrp	x0, .L.str.10
	adrp	x1, .L__func__.mas_blk_lld_inline_crypto_bio_merge_allow
	add	x0, x0, :lo12:.L.str.10
	add	x1, x1, :lo12:.L__func__.mas_blk_lld_inline_crypto_bio_merge_allow
	bl	printk
	ldr	x8, [x20, #304]
	ldr	x9, [x19, #744]
	cmp	x8, x9
	b.eq	.LBB12_8
.LBB12_15:
	mov	w0, wzr
.LBB12_16:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB12_17:
	add	x8, x8, #1408           // =1408
	ldrb	w8, [x8, #24]
	tbnz	w8, #4, .LBB12_2
	b	.LBB12_12
.Lfunc_end12:
	.size	mas_blk_bio_merge_allow, .Lfunc_end12-mas_blk_bio_merge_allow
                                        // -- End function
	.globl	mas_blk_bio_merge_done  // -- Begin function mas_blk_bio_merge_done
	.p2align	2
	.type	mas_blk_bio_merge_done,@function
mas_blk_bio_merge_done:                 // @mas_blk_bio_merge_done
.Lmas_blk_bio_merge_done$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x0, x1
	mov	x1, x2
	mov	x29, sp
	bl	mas_blk_latency_for_merge
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end13:
	.size	mas_blk_bio_merge_done, .Lfunc_end13-mas_blk_bio_merge_done
                                        // -- End function
	.globl	mas_blk_account_io_completion // -- Begin function mas_blk_account_io_completion
	.p2align	2
	.type	mas_blk_account_io_completion,@function
mas_blk_account_io_completion:          // @mas_blk_account_io_completion
.Lmas_blk_account_io_completion$local:
// %bb.0:
	cbz	x0, .LBB14_7
// %bb.1:
	ldr	x8, [x0]
	ldr	x9, [x8, #48]
	cbz	x9, .LBB14_8
// %bb.2:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x8, x8, x10, eq
	cbz	x8, .LBB14_9
.LBB14_3:
	ldrb	w9, [x0, #24]
	cmp	w9, #3                  // =3
	b.ne	.LBB14_5
// %bb.4:
	ldr	x9, [x8, #1888]
	add	x9, x9, w1, uxtw
	str	x9, [x8, #1888]
	ldrb	w9, [x0, #24]
.LBB14_5:
	cmp	w9, #1                  // =1
	b.ne	.LBB14_9
// %bb.6:
	ldr	x9, [x8, #1880]
	mov	w0, wzr
	add	x9, x9, w1, uxtw
	str	x9, [x8, #1880]
	ret
.LBB14_7:
	mov	w0, #-1
	ret
.LBB14_8:
	add	x8, x8, #1408           // =1408
	cbnz	x8, .LBB14_3
.LBB14_9:
	mov	w0, wzr
	ret
.Lfunc_end14:
	.size	mas_blk_account_io_completion, .Lfunc_end14-mas_blk_account_io_completion
                                        // -- End function
	.globl	mas_queue_io_prio_sim_show // -- Begin function mas_queue_io_prio_sim_show
	.p2align	2
	.type	mas_queue_io_prio_sim_show,@function
mas_queue_io_prio_sim_show:             // @mas_queue_io_prio_sim_show
.Lmas_queue_io_prio_sim_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldrb	w3, [x0, #1088]
	adrp	x2, .L.str.1
	mov	x0, x1
	add	x2, x2, :lo12:.L.str.1
	mov	w1, #4096
	mov	x29, sp
	bl	snprintf
                                        // kill: def $w0 killed $w0 def $x0
	sxtw	x0, w0
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end15:
	.size	mas_queue_io_prio_sim_show, .Lfunc_end15-mas_queue_io_prio_sim_show
                                        // -- End function
	.globl	mas_queue_io_prio_sim_store // -- Begin function mas_queue_io_prio_sim_store
	.p2align	2
	.type	mas_queue_io_prio_sim_store,@function
mas_queue_io_prio_sim_store:            // @mas_queue_io_prio_sim_store
.Lmas_queue_io_prio_sim_store$local:
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x20, x0
	mov	x0, sp
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	add	x29, sp, #16            // =16
	mov	x19, x2
	str	x8, [sp, #8]
	str	xzr, [sp]
	bl	queue_var_store
	ldr	x8, [sp]
	cmp	x0, #0                  // =0
	cset	w9, ge
	cmp	x8, #0                  // =0
	cset	w8, ne
	and	w8, w9, w8
	strb	w8, [x20, #1088]
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB16_2
// %bb.1:
	mov	x0, x19
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB16_2:
	bl	__stack_chk_fail
.Lfunc_end16:
	.size	mas_queue_io_prio_sim_store, .Lfunc_end16-mas_queue_io_prio_sim_store
                                        // -- End function
	.globl	mas_blk_generic_make_request_check // -- Begin function mas_blk_generic_make_request_check
	.p2align	2
	.type	mas_blk_generic_make_request_check,@function
mas_blk_generic_make_request_check:     // @mas_blk_generic_make_request_check
.Lmas_blk_generic_make_request_check$local:
// %bb.0:
	sub	sp, sp, #80             // =80
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #32            // =32
	str	x21, [sp, #48]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	ldr	x8, [x0, #8]
	mov	x19, x0
	ldr	x20, [x8, #1272]
	stp	xzr, xzr, [sp, #8]
	ldrb	w8, [x20, #1088]
	cbnz	w8, .LBB17_5
.LBB17_1:
	mov	x0, x19
	bl	mas_blk_bio_set_opf_unistore
	mov	x0, x19
	mov	w1, wzr
	bl	mas_blk_latency_bio_check
	mov	x0, x20
	mov	x1, x19
	bl	mas_blk_flush_async_dispatch
	tbnz	w0, #0, .LBB17_7
// %bb.2:
	mov	x0, x20
	mov	x1, x19
	bl	mas_blk_busyidle_check_bio
	mov	w0, wzr
.LBB17_3:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB17_15
// %bb.4:
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldr	x21, [sp, #48]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #32]     // 16-byte Folded Reload
	add	sp, sp, #80             // =80
	ret
.LBB17_5:
	add	x0, sp, #8              // =8
	mov	w1, #16
	//APP
	mrs x2, sp_el0
	//NO_APP
	bl	__get_task_comm
	adrp	x21, process_io_prio_array
	add	x21, x21, :lo12:process_io_prio_array
	add	x0, sp, #8              // =8
	mov	x1, x21
	strb	wzr, [sp, #23]
	bl	strstr
	cbz	x0, .LBB17_8
// %bb.6:
	mov	x8, xzr
	b	.LBB17_14
.LBB17_7:
	mov	x0, x19
	strb	wzr, [x19, #26]
	bl	bio_endio
	mov	w0, #1
	b	.LBB17_3
.LBB17_8:
	adrp	x1, process_io_prio_array+28
	add	x1, x1, :lo12:process_io_prio_array+28
	add	x0, sp, #8              // =8
	bl	strstr
	cbz	x0, .LBB17_10
// %bb.9:
	mov	w8, #1
	b	.LBB17_14
.LBB17_10:
	adrp	x1, process_io_prio_array+56
	add	x1, x1, :lo12:process_io_prio_array+56
	add	x0, sp, #8              // =8
	bl	strstr
	cbz	x0, .LBB17_12
// %bb.11:
	mov	w8, #2
	b	.LBB17_14
.LBB17_12:
	adrp	x1, process_io_prio_array+84
	add	x1, x1, :lo12:process_io_prio_array+84
	add	x0, sp, #8              // =8
	bl	strstr
	cbz	x0, .LBB17_1
// %bb.13:
	mov	w8, #3
.LBB17_14:
	mov	w9, #28
	madd	x8, x8, x9, x21
	ldr	w10, [x19, #16]
	ldp	w9, w12, [x8, #16]
	ldr	x11, [x19, #88]
	ldr	w8, [x8, #24]
	orr	w9, w10, w9
	orr	x10, x11, x12
	bic	w8, w9, w8
	str	x10, [x19, #88]
	str	w8, [x19, #16]
	b	.LBB17_1
.LBB17_15:
	bl	__stack_chk_fail
.Lfunc_end17:
	.size	mas_blk_generic_make_request_check, .Lfunc_end17-mas_blk_generic_make_request_check
                                        // -- End function
	.globl	mas_blk_generic_make_request // -- Begin function mas_blk_generic_make_request
	.p2align	2
	.type	mas_blk_generic_make_request,@function
mas_blk_generic_make_request:           // @mas_blk_generic_make_request
.Lmas_blk_generic_make_request$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w1, #2
	mov	x29, sp
	bl	mas_blk_latency_bio_check
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end18:
	.size	mas_blk_generic_make_request, .Lfunc_end18-mas_blk_generic_make_request
                                        // -- End function
	.globl	mas_blk_start_plug      // -- Begin function mas_blk_start_plug
	.p2align	2
	.type	mas_blk_start_plug,@function
mas_blk_start_plug:                     // @mas_blk_start_plug
.Lmas_blk_start_plug$local:
// %bb.0:
	add	x8, x0, #40             // =40
	str	x8, [x0, #40]
	stp	x8, xzr, [x0, #48]
	ret
.Lfunc_end19:
	.size	mas_blk_start_plug, .Lfunc_end19-mas_blk_start_plug
                                        // -- End function
	.globl	mas_blk_flush_plug_list // -- Begin function mas_blk_flush_plug_list
	.p2align	2
	.type	mas_blk_flush_plug_list,@function
mas_blk_flush_plug_list:                // @mas_blk_flush_plug_list
.Lmas_blk_flush_plug_list$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x8, x0
	ldr	x9, [x8, #40]!
	mov	x29, sp
	cmp	x8, x9
	b.eq	.LBB20_3
// %bb.1:
	ldr	x8, [x0, #56]
	cbz	x8, .LBB20_3
// %bb.2:
	and	w1, w1, #0x1
	blr	x8
.LBB20_3:
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end20:
	.size	mas_blk_flush_plug_list, .Lfunc_end20-mas_blk_flush_plug_list
                                        // -- End function
	.globl	mas_blk_bio_endio       // -- Begin function mas_blk_bio_endio
	.p2align	2
	.type	mas_blk_bio_endio,@function
mas_blk_bio_endio:                      // @mas_blk_bio_endio
.Lmas_blk_bio_endio$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_busyidle_check_bio_endio
	mov	w1, #1
	mov	x0, x19
	bl	mas_blk_latency_bio_check
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end21:
	.size	mas_blk_bio_endio, .Lfunc_end21-mas_blk_bio_endio
                                        // -- End function
	.globl	mas_blk_bio_free        // -- Begin function mas_blk_bio_free
	.p2align	2
	.type	mas_blk_bio_free,@function
mas_blk_bio_free:                       // @mas_blk_bio_free
.Lmas_blk_bio_free$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldrb	w8, [x0, #144]
	mov	x29, sp
	tbnz	w8, #0, .LBB22_2
// %bb.1:
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB22_2:
	mov	x1, x0
	adrp	x0, .L.str.2
	add	x0, x0, :lo12:.L.str.2
	bl	printk
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.Lfunc_end22:
	.size	mas_blk_bio_free, .Lfunc_end22-mas_blk_bio_free
                                        // -- End function
	.globl	mas_blk_request_init_from_bio // -- Begin function mas_blk_request_init_from_bio
	.p2align	2
	.type	mas_blk_request_init_from_bio,@function
mas_blk_request_init_from_bio:          // @mas_blk_request_init_from_bio
.Lmas_blk_request_init_from_bio$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x19, x1
	mov	w1, wzr
	mov	x29, sp
	mov	x20, x0
	bl	mas_blk_latency_req_check
	add	x8, x20, #260           // =260
	mov	x0, x20
	mov	x1, x19
	stur	xzr, [x20, #252]
	str	xzr, [x8]
	str	wzr, [x20, #268]
	bl	mas_blk_request_init_from_bio_unistore
	str	x20, [x19, #104]
	ldr	x8, [x20]
	ldr	x9, [x8, #48]
	cbz	x9, .LBB23_11
// %bb.1:
	ldr	x9, [x8, #3968]
	add	x10, x8, #1408          // =1408
	add	x11, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x9, x10, x11, eq
	ldrb	w9, [x9, #24]
	tbz	w9, #4, .LBB23_6
.LBB23_2:
	ldr	x9, [x19, #304]
	cbz	x9, .LBB23_6
// %bb.3:
	ldr	w10, [x19, #312]
	cmp	w10, #48                // =48
	b.eq	.LBB23_5
// %bb.4:
	cmp	w10, #64                // =64
	b.ne	.LBB23_12
.LBB23_5:
	str	x9, [x20, #744]
	ldr	w9, [x19, #312]
	str	w9, [x20, #752]
	ldr	w9, [x19, #316]
	str	w9, [x20, #756]
.LBB23_6:
	ldrb	w9, [x19, #328]
	tbz	w9, #0, .LBB23_10
// %bb.7:
	ldr	x9, [x20, #240]
	orr	x9, x9, #0x8
	str	x9, [x20, #240]
	ldrb	w9, [x19, #328]
	tbz	w9, #0, .LBB23_10
// %bb.8:
	ldrb	w9, [x19, #16]
	cmp	w9, #1                  // =1
	b.ne	.LBB23_10
// %bb.9:
	ldr	w9, [x19, #40]
	ldr	x10, [x8, #1096]
	add	x9, x10, x9
	str	x9, [x8, #1096]
.LBB23_10:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB23_11:
	add	x9, x8, #1408           // =1408
	ldrb	w9, [x9, #24]
	tbnz	w9, #4, .LBB23_2
	b	.LBB23_6
.LBB23_12:
	adrp	x0, .L.str.11
	adrp	x1, .L__func__.mas_blk_lld_inline_crypto_init_request_from_bio
	add	x0, x0, :lo12:.L.str.11
	add	x1, x1, :lo12:.L__func__.mas_blk_lld_inline_crypto_init_request_from_bio
	mov	w2, #64
	mov	w3, #48
	bl	printk
	adrp	x0, .L.str.12
	add	x0, x0, :lo12:.L.str.12
	bl	mas_blk_rdr_panic
.Lfunc_end23:
	.size	mas_blk_request_init_from_bio, .Lfunc_end23-mas_blk_request_init_from_bio
                                        // -- End function
	.globl	mas_blk_insert_cloned_request // -- Begin function mas_blk_insert_cloned_request
	.p2align	2
	.type	mas_blk_insert_cloned_request,@function
mas_blk_insert_cloned_request:          // @mas_blk_insert_cloned_request
.Lmas_blk_insert_cloned_request$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	bl	mas_blk_busyidle_check_request_bio
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end24:
	.size	mas_blk_insert_cloned_request, .Lfunc_end24-mas_blk_insert_cloned_request
                                        // -- End function
	.globl	mas_blk_request_execute_nowait // -- Begin function mas_blk_request_execute_nowait
	.p2align	2
	.type	mas_blk_request_execute_nowait,@function
mas_blk_request_execute_nowait:         // @mas_blk_request_execute_nowait
.Lmas_blk_request_execute_nowait$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	bl	mas_blk_busyidle_check_execute_request
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end25:
	.size	mas_blk_request_execute_nowait, .Lfunc_end25-mas_blk_request_execute_nowait
                                        // -- End function
	.globl	mas_blk_mq_rq_ctx_init  // -- Begin function mas_blk_mq_rq_ctx_init
	.p2align	2
	.type	mas_blk_mq_rq_ctx_init,@function
mas_blk_mq_rq_ctx_init:                 // @mas_blk_mq_rq_ctx_init
.Lmas_blk_mq_rq_ctx_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	ldr	x8, [x0, #1400]
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x2
	mov	x29, sp
	cbz	x8, .LBB26_3
// %bb.1:
	ldr	x8, [x8, #32]
	cbz	x8, .LBB26_3
// %bb.2:
	mov	x2, x19
	blr	x8
.LBB26_3:
	mov	x8, #-4294967296
	str	xzr, [x19, #744]
	str	x8, [x19, #752]
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end26:
	.size	mas_blk_mq_rq_ctx_init, .Lfunc_end26-mas_blk_mq_rq_ctx_init
                                        // -- End function
	.globl	mas_blk_mq_request_start // -- Begin function mas_blk_mq_request_start
	.p2align	2
	.type	mas_blk_mq_request_start,@function
mas_blk_mq_request_start:               // @mas_blk_mq_request_start
.Lmas_blk_mq_request_start$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	mov	w1, #9
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_latency_req_check
	ldr	x8, [x19]
	mov	w10, #28570
	movk	w10, #4, lsl #16
	ldr	x9, [x8, #1080]
	cmp	x9, x10
	b.eq	.LBB27_2
.LBB27_1:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB27_2:
	ldrb	w9, [x19, #24]
	orr	w10, w9, #0x3
	cmp	w10, #35                // =35
	b.eq	.LBB27_1
// %bb.3:
	ldr	w10, [x19, #48]
	cbz	w10, .LBB27_1
// %bb.4:
	cmp	w9, #1                  // =1
	b.ne	.LBB27_11
// %bb.5:
	cmp	w10, #32, lsl #12       // =131072
	b.hi	.LBB27_8
// %bb.6:
	ldr	x0, [x8, #1328]
	cbz	x0, .LBB27_8
// %bb.7:
	bl	__udelay
.LBB27_8:
	ldr	x8, [x19]
	ldr	x0, [x8, #1312]
	cbz	x0, .LBB27_11
// %bb.9:
	ldr	w8, [x19, #48]
	cmp	w8, #32, lsl #12        // =131072
	b.ls	.LBB27_11
// %bb.10:
	bl	__udelay
.LBB27_11:
	ldrb	w8, [x19, #24]
	cbnz	w8, .LBB27_1
// %bb.12:
	ldr	x8, [x19]
	ldr	x0, [x8, #1320]
	cbz	x0, .LBB27_15
// %bb.13:
	ldr	w8, [x19, #48]
	cmp	w8, #32, lsl #12        // =131072
	b.hi	.LBB27_15
// %bb.14:
	bl	__udelay
.LBB27_15:
	ldr	x8, [x19]
	ldr	x0, [x8, #1304]
	cbz	x0, .LBB27_1
// %bb.16:
	ldr	w8, [x19, #48]
	cmp	w8, #32, lsl #12        // =131072
	b.ls	.LBB27_1
// %bb.17:
	bl	__udelay
	b	.LBB27_1
.Lfunc_end27:
	.size	mas_blk_mq_request_start, .Lfunc_end27-mas_blk_mq_request_start
                                        // -- End function
	.globl	mas_blk_request_start   // -- Begin function mas_blk_request_start
	.p2align	2
	.type	mas_blk_request_start,@function
mas_blk_request_start:                  // @mas_blk_request_start
.Lmas_blk_request_start$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w1, #9
	mov	x29, sp
	bl	mas_blk_latency_req_check
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end28:
	.size	mas_blk_request_start, .Lfunc_end28-mas_blk_request_start
                                        // -- End function
	.globl	mas_blk_requeue_request // -- Begin function mas_blk_requeue_request
	.p2align	2
	.type	mas_blk_requeue_request,@function
mas_blk_requeue_request:                // @mas_blk_requeue_request
.Lmas_blk_requeue_request$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w1, #26
	mov	x29, sp
	bl	mas_blk_latency_req_check
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end29:
	.size	mas_blk_requeue_request, .Lfunc_end29-mas_blk_requeue_request
                                        // -- End function
	.globl	mas_blk_request_update  // -- Begin function mas_blk_request_update
	.p2align	2
	.type	mas_blk_request_update,@function
mas_blk_request_update:                 // @mas_blk_request_update
.Lmas_blk_request_update$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x0]
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	w19, w2
	mov	x20, x0
	ldr	x9, [x8, #48]
	mov	w21, w1
	mov	x29, sp
	cbz	x9, .LBB30_9
// %bb.1:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x8, x8, x10, eq
.LBB30_2:
	ldrb	w9, [x20, #24]
	cmp	w9, #3                  // =3
	b.eq	.LBB30_7
// %bb.3:
	cmp	w9, #1                  // =1
	b.eq	.LBB30_8
.LBB30_4:
	cmp	w9, #2                  // =2
	b.ne	.LBB30_6
.LBB30_5:
	and	w1, w21, #0xff
	mov	x0, x20
	bl	mas_blk_flush_update
.LBB30_6:
	mov	w1, #48
	mov	x0, x20
	bl	mas_blk_latency_req_check
	mov	x0, x20
	mov	w1, w21
	mov	w2, w19
	bl	mas_blk_req_update_unistore
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB30_7:
	ldr	x9, [x8, #1888]
	add	x9, x9, w19, uxtw
	str	x9, [x8, #1888]
	ldrb	w9, [x20, #24]
	cmp	w9, #1                  // =1
	b.ne	.LBB30_4
.LBB30_8:
	ldr	x9, [x8, #1880]
	add	x9, x9, w19, uxtw
	str	x9, [x8, #1880]
	ldrb	w9, [x20, #24]
	cmp	w9, #2                  // =2
	b.eq	.LBB30_5
	b	.LBB30_6
.LBB30_9:
	add	x8, x8, #1408           // =1408
	b	.LBB30_2
.Lfunc_end30:
	.size	mas_blk_request_update, .Lfunc_end30-mas_blk_request_update
                                        // -- End function
	.globl	mas_blk_mq_request_free // -- Begin function mas_blk_mq_request_free
	.p2align	2
	.type	mas_blk_mq_request_free,@function
mas_blk_mq_request_free:                // @mas_blk_mq_request_free
.Lmas_blk_mq_request_free$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w1, #51
	mov	x29, sp
	str	xzr, [x0, #736]
	bl	mas_blk_latency_req_check
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end31:
	.size	mas_blk_mq_request_free, .Lfunc_end31-mas_blk_mq_request_free
                                        // -- End function
	.globl	mas_blk_request_put     // -- Begin function mas_blk_request_put
	.p2align	2
	.type	mas_blk_request_put,@function
mas_blk_request_put:                    // @mas_blk_request_put
.Lmas_blk_request_put$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w1, #51
	mov	x29, sp
	str	xzr, [x0, #736]
	bl	mas_blk_latency_req_check
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end32:
	.size	mas_blk_request_put, .Lfunc_end32-mas_blk_request_put
                                        // -- End function
	.globl	mas_blk_check_partition_done // -- Begin function mas_blk_check_partition_done
	.p2align	2
	.type	mas_blk_check_partition_done,@function
mas_blk_check_partition_done:           // @mas_blk_check_partition_done
.Lmas_blk_check_partition_done$local:
// %bb.0:
	ldr	x8, [x0, #1272]
	and	w9, w1, #0x1
	strb	w9, [x8, #1072]
	ret
.Lfunc_end33:
	.size	mas_blk_check_partition_done, .Lfunc_end33-mas_blk_check_partition_done
                                        // -- End function
	.globl	mas_blk_allocated_queue_init // -- Begin function mas_blk_allocated_queue_init
	.p2align	2
	.type	mas_blk_allocated_queue_init,@function
mas_blk_allocated_queue_init:           // @mas_blk_allocated_queue_init
.Lmas_blk_allocated_queue_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	add	x8, x0, #1288           // =1288
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	str	x8, [x0, #1288]
	ldr	w9, [x0, #1408]
	mov	x29, sp
	str	x8, [x0, #1296]
	strb	wzr, [x0, #1072]
	cmp	w9, #122                // =122
	b.eq	.LBB34_6
// %bb.1:
	mov	x19, x0
	add	x0, x0, #1408           // =1408
	mov	w8, #122
	add	x9, x19, #3308          // =3308
	add	x10, x19, #3316         // =3316
	mov	w11, #1
	str	w8, [x19, #1408]
	str	xzr, [x9]
	str	x11, [x10]
	bl	mas_blk_dev_lld_init_unistore
	add	x20, x19, #2296         // =2296
	add	x8, x19, #3328          // =3328
	mov	x0, x20
	str	x8, [x19, #3328]
	str	x8, [x19, #3336]
	str	wzr, [x19, #1412]
	str	x19, [x19, #1416]
	bl	mas_blk_busyidle_state_init
	ldr	x8, [x19, #1432]
	cbz	w0, .LBB34_3
// %bb.2:
	and	x8, x8, #0xfffffffffffffff7
	str	x8, [x19, #1432]
.LBB34_3:
	tbz	w8, #6, .LBB34_5
// %bb.4:
	mov	w9, #1
	strb	w9, [x20]
.LBB34_5:
	and	x8, x8, #0xfffffffffffffe7f
	orr	x8, x8, #0x80
	strb	wzr, [x19, #3304]
	str	x8, [x19, #1432]
	str	xzr, [x19, #3360]
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB34_6:
	adrp	x0, .L.str.13
	add	x0, x0, :lo12:.L.str.13
	bl	mas_blk_rdr_panic
.Lfunc_end34:
	.size	mas_blk_allocated_queue_init, .Lfunc_end34-mas_blk_allocated_queue_init
                                        // -- End function
	.globl	mas_blk_sq_init_allocated_queue // -- Begin function mas_blk_sq_init_allocated_queue
	.p2align	2
	.type	mas_blk_sq_init_allocated_queue,@function
mas_blk_sq_init_allocated_queue:        // @mas_blk_sq_init_allocated_queue
.Lmas_blk_sq_init_allocated_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_dump_register_queue
	mov	x0, x19
	bl	mas_blk_queue_latency_init
	mov	x0, x19
	bl	mas_blk_queue_async_flush_init
	adrp	x1, _cfi_mas_blk_queue_usr_ctrl_recovery_timer_expire
	add	x0, x19, #1336          // =1336
	add	x1, x1, :lo12:_cfi_mas_blk_queue_usr_ctrl_recovery_timer_expire
	mov	w2, wzr
	mov	x3, xzr
	mov	x4, xzr
	str	xzr, [x19, #1304]
	str	xzr, [x19, #1312]
	str	xzr, [x19, #1320]
	str	xzr, [x19, #1328]
	bl	init_timer_key
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end35:
	.size	mas_blk_sq_init_allocated_queue, .Lfunc_end35-mas_blk_sq_init_allocated_queue
                                        // -- End function
	.globl	mas_blk_mq_init_allocated_queue // -- Begin function mas_blk_mq_init_allocated_queue
	.p2align	2
	.type	mas_blk_mq_init_allocated_queue,@function
mas_blk_mq_init_allocated_queue:        // @mas_blk_mq_init_allocated_queue
.Lmas_blk_mq_init_allocated_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	x20, [x0, #3968]
	mov	x19, x0
	mov	x29, sp
	ldr	w8, [x20, #2036]
	cbz	w8, .LBB36_5
// %bb.1:
	mov	x9, xzr
	b	.LBB36_3
.LBB36_2:                               //   in Loop: Header=BB36_3 Depth=1
	add	x9, x9, #1              // =1
	cmp	x9, w8, uxtw
	b.hs	.LBB36_5
.LBB36_3:                               // =>This Inner Loop Header: Depth=1
	ldr	x10, [x20, #2072]
	ldr	x10, [x10, x9, lsl #3]
	cbz	x10, .LBB36_2
// %bb.4:                               //   in Loop: Header=BB36_3 Depth=1
	str	x20, [x10, #24]
	ldr	w8, [x20, #2036]
	b	.LBB36_2
.LBB36_5:
	mov	x0, x19
	bl	mas_blk_dump_register_queue
	mov	x0, x19
	bl	mas_blk_queue_latency_init
	mov	x0, x19
	bl	mas_blk_queue_async_flush_init
	adrp	x1, _cfi_mas_blk_queue_usr_ctrl_recovery_timer_expire
	add	x0, x19, #1336          // =1336
	add	x1, x1, :lo12:_cfi_mas_blk_queue_usr_ctrl_recovery_timer_expire
	mov	w2, wzr
	mov	x3, xzr
	mov	x4, xzr
	str	xzr, [x19, #1304]
	str	xzr, [x19, #1312]
	str	xzr, [x19, #1320]
	str	xzr, [x19, #1328]
	bl	init_timer_key
	ldr	x8, [x20, #2024]
	cbz	x8, .LBB36_10
// %bb.6:
	ldr	x8, [x8, #40]
	str	x8, [x19, #1400]
	cbz	x8, .LBB36_11
// %bb.7:
	ldr	x8, [x8, #8]
	cbz	x8, .LBB36_11
// %bb.8:
	mov	x0, x19
	blr	x8
	cbz	w0, .LBB36_11
// %bb.9:
	mov	w2, w0
	adrp	x0, .L.str.3
	adrp	x1, .L__func__.mas_blk_mq_init_allocated_queue
	add	x0, x0, :lo12:.L.str.3
	add	x1, x1, :lo12:.L__func__.mas_blk_mq_init_allocated_queue
	bl	printk
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.LBB36_10:
	str	xzr, [x19, #1400]
.LBB36_11:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end36:
	.size	mas_blk_mq_init_allocated_queue, .Lfunc_end36-mas_blk_mq_init_allocated_queue
                                        // -- End function
	.globl	mas_blk_mq_free_queue   // -- Begin function mas_blk_mq_free_queue
	.p2align	2
	.type	mas_blk_mq_free_queue,@function
mas_blk_mq_free_queue:                  // @mas_blk_mq_free_queue
.Lmas_blk_mq_free_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x8, [x0, #1400]
	mov	x29, sp
	cbz	x8, .LBB37_3
// %bb.1:
	ldr	x8, [x8, #16]
	cbz	x8, .LBB37_3
// %bb.2:
	blr	x8
.LBB37_3:
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end37:
	.size	mas_blk_mq_free_queue, .Lfunc_end37-mas_blk_mq_free_queue
                                        // -- End function
	.globl	mas_blk_cleanup_queue   // -- Begin function mas_blk_cleanup_queue
	.p2align	2
	.type	mas_blk_cleanup_queue,@function
mas_blk_cleanup_queue:                  // @mas_blk_cleanup_queue
.Lmas_blk_cleanup_queue$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_dump_unregister_queue
	add	x0, x19, #1336          // =1336
	bl	del_timer_sync
	mov	x0, x19
	bl	mas_blk_flush_reduced_queue_unregister
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end38:
	.size	mas_blk_cleanup_queue, .Lfunc_end38-mas_blk_cleanup_queue
                                        // -- End function
	.globl	mas_blk_allocated_tags_init // -- Begin function mas_blk_allocated_tags_init
	.p2align	2
	.type	mas_blk_allocated_tags_init,@function
mas_blk_allocated_tags_init:            // @mas_blk_allocated_tags_init
.Lmas_blk_allocated_tags_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	adrp	x1, .L.str.4
	adrp	x2, mas_blk_allocated_tags_init.__key
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x19, x0
	add	x0, x0, #40             // =40
	add	x1, x1, :lo12:.L.str.4
	add	x2, x2, :lo12:mas_blk_allocated_tags_init.__key
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	bl	__mutex_init
	add	x20, x19, #96           // =96
	add	x8, x19, #80            // =80
	mov	w2, #1960
	mov	x0, x20
	mov	w1, wzr
	str	x8, [x19, #80]
	str	x8, [x19, #88]
	bl	memset
	ldr	w8, [x19, #96]
	cmp	w8, #122                // =122
	b.eq	.LBB39_6
// %bb.1:
	mov	w8, #122
	add	x9, x19, #1996          // =1996
	add	x10, x19, #2004         // =2004
	mov	w21, #1
	mov	x0, x20
	str	w8, [x19, #96]
	str	xzr, [x9]
	str	x21, [x10]
	bl	mas_blk_dev_lld_init_unistore
	add	x20, x19, #984          // =984
	add	x8, x19, #2016          // =2016
	mov	x0, x20
	str	x8, [x19, #2016]
	str	x8, [x19, #2024]
	str	w21, [x19, #100]
	str	x19, [x19, #104]
	bl	mas_blk_busyidle_state_init
	ldr	x8, [x19, #120]
	cbz	w0, .LBB39_3
// %bb.2:
	and	x8, x8, #0xfffffffffffffff7
	str	x8, [x19, #120]
.LBB39_3:
	tbz	w8, #6, .LBB39_5
// %bb.4:
	mov	w9, #1
	strb	w9, [x20]
.LBB39_5:
	and	x8, x8, #0xfffffffffffffe7f
	orr	x8, x8, #0x80
	strb	wzr, [x19, #1992]
	str	x8, [x19, #120]
	str	xzr, [x19, #2048]
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB39_6:
	adrp	x0, .L.str.13
	add	x0, x0, :lo12:.L.str.13
	bl	mas_blk_rdr_panic
.Lfunc_end39:
	.size	mas_blk_allocated_tags_init, .Lfunc_end39-mas_blk_allocated_tags_init
                                        // -- End function
	.globl	blk_add_queue_tags      // -- Begin function blk_add_queue_tags
	.p2align	2
	.type	blk_add_queue_tags,@function
blk_add_queue_tags:                     // @blk_add_queue_tags
.Lblk_add_queue_tags$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	add	x21, x0, #40            // =40
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x20, x0
	mov	x0, x21
	mov	x29, sp
	mov	x19, x1
	bl	mutex_lock
	ldr	x9, [x20, #88]
	add	x8, x19, #3976          // =3976
	add	x10, x20, #80           // =80
	mov	x0, x21
	str	x8, [x20, #88]
	str	x10, [x19, #3976]
	str	x9, [x19, #3984]
	str	x8, [x9]
	bl	mutex_unlock
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end40:
	.size	blk_add_queue_tags, .Lfunc_end40-blk_add_queue_tags
                                        // -- End function
	.globl	mas_blk_mq_allocated_tagset_init // -- Begin function mas_blk_mq_allocated_tagset_init
	.p2align	2
	.type	mas_blk_mq_allocated_tagset_init,@function
mas_blk_mq_allocated_tagset_init:       // @mas_blk_mq_allocated_tagset_init
.Lmas_blk_mq_allocated_tagset_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x19, x0
	ldr	w8, [x0, #64]!
	mov	x29, sp
	cmp	w8, #122                // =122
	b.eq	.LBB41_6
// %bb.1:
	mov	w8, #122
	add	x9, x19, #1964          // =1964
	add	x10, x19, #1972         // =1972
	mov	w11, #1
	str	w8, [x19, #64]
	str	xzr, [x9]
	str	x11, [x10]
	bl	mas_blk_dev_lld_init_unistore
	add	x20, x19, #952          // =952
	add	x8, x19, #1984          // =1984
	mov	w9, #2
	mov	x0, x20
	str	x8, [x19, #1984]
	str	x8, [x19, #1992]
	str	w9, [x19, #68]
	str	x19, [x19, #72]
	bl	mas_blk_busyidle_state_init
	ldr	x8, [x19, #88]
	cbz	w0, .LBB41_3
// %bb.2:
	and	x8, x8, #0xfffffffffffffff7
	str	x8, [x19, #88]
.LBB41_3:
	tbz	w8, #6, .LBB41_5
// %bb.4:
	mov	w9, #1
	strb	w9, [x20]
.LBB41_5:
	adrp	x10, mas_ufs_blk_tagset_ops
	and	x9, x8, #0xfffffffffffffe7f
	lsl	x8, x8, #58
	add	x10, x10, :lo12:mas_ufs_blk_tagset_ops
	orr	x9, x9, #0x80
	and	x8, x10, x8, asr #63
	strb	wzr, [x19, #1960]
	str	xzr, [x19, #2016]
	str	x9, [x19, #88]
	str	x8, [x19, #2024]
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB41_6:
	adrp	x0, .L.str.13
	add	x0, x0, :lo12:.L.str.13
	bl	mas_blk_rdr_panic
.Lfunc_end41:
	.size	mas_blk_mq_allocated_tagset_init, .Lfunc_end41-mas_blk_mq_allocated_tagset_init
                                        // -- End function
	.globl	mas_blk_queue_register  // -- Begin function mas_blk_queue_register
	.p2align	2
	.type	mas_blk_queue_register,@function
mas_blk_queue_register:                 // @mas_blk_queue_register
.Lmas_blk_queue_register$local:
// %bb.0:
	str	x1, [x0, #1064]
	ret
.Lfunc_end42:
	.size	mas_blk_queue_register, .Lfunc_end42-mas_blk_queue_register
                                        // -- End function
	.section	.init.text,"ax",@progbits
	.globl	mas_blk_dev_init        // -- Begin function mas_blk_dev_init
	.p2align	2
	.type	mas_blk_dev_init,@function
mas_blk_dev_init:                       // @mas_blk_dev_init
.Lmas_blk_dev_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	bl	mas_blk_dump_init
	bl	mas_blk_latency_init
	mov	w0, wzr
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end43:
	.size	mas_blk_dev_init, .Lfunc_end43-mas_blk_dev_init
                                        // -- End function
	.text
	.globl	blk_write_throttle      // -- Begin function blk_write_throttle
	.p2align	2
	.type	blk_write_throttle,@function
blk_write_throttle:                     // @blk_write_throttle
.Lblk_write_throttle$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB44_4
// %bb.1:
	ldr	x8, [x0, #1400]
	cbz	x8, .LBB44_4
// %bb.2:
	ldr	x8, [x8, #152]
	cbz	x8, .LBB44_4
// %bb.3:
	cmp	w1, #0                  // =0
	cset	w1, ne
	blr	x8
.LBB44_4:
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end44:
	.size	blk_write_throttle, .Lfunc_end44-blk_write_throttle
                                        // -- End function
	.globl	blk_generic_freeze      // -- Begin function blk_generic_freeze
	.p2align	2
	.type	blk_generic_freeze,@function
blk_generic_freeze:                     // @blk_generic_freeze
.Lblk_generic_freeze$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB45_20
// %bb.1:
	mov	w19, w2
	cmp	w1, #1                  // =1
	b.eq	.LBB45_9
// %bb.2:
	cbnz	w1, .LBB45_21
// %bb.3:
	ldr	w8, [x0, #4]
	cmp	w8, #2                  // =2
	b.eq	.LBB45_13
// %bb.4:
	cmp	w8, #1                  // =1
	b.eq	.LBB45_12
// %bb.5:
	cbnz	w8, .LBB45_22
// %bb.6:
	ldr	x0, [x0, #8]
	ldr	x8, [x0, #48]
	cbz	x8, .LBB45_12
// %bb.7:
	tbnz	w19, #0, .LBB45_11
.LBB45_8:
	bl	blk_mq_unfreeze_queue
	b	.LBB45_12
.LBB45_9:
	ldr	x8, [x0, #48]
	cbz	x8, .LBB45_12
// %bb.10:
	tbz	w19, #0, .LBB45_8
.LBB45_11:
	bl	blk_mq_freeze_queue
.LBB45_12:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB45_13:
	ldr	x8, [x0, #8]
	ldr	x20, [x8, #2120]
	add	x21, x8, #2120          // =2120
	cmp	x21, x20
	b.ne	.LBB45_16
	b	.LBB45_12
.LBB45_14:                              //   in Loop: Header=BB45_16 Depth=1
	bl	blk_mq_unfreeze_queue
.LBB45_15:                              //   in Loop: Header=BB45_16 Depth=1
	ldr	x20, [x20]
	cmp	x21, x20
	b.eq	.LBB45_12
.LBB45_16:                              // =>This Inner Loop Header: Depth=1
	subs	x0, x20, #3976          // =3976
	b.eq	.LBB45_20
// %bb.17:                              //   in Loop: Header=BB45_16 Depth=1
	sub	x8, x20, #3928          // =3928
	ldr	x8, [x8]
	cbz	x8, .LBB45_15
// %bb.18:                              //   in Loop: Header=BB45_16 Depth=1
	tbz	w19, #0, .LBB45_14
// %bb.19:                              //   in Loop: Header=BB45_16 Depth=1
	bl	blk_mq_freeze_queue
	ldr	x20, [x20]
	cmp	x21, x20
	b.ne	.LBB45_16
	b	.LBB45_12
.LBB45_20:
	adrp	x0, .L.str.5
	add	x0, x0, :lo12:.L.str.5
	bl	mas_blk_rdr_panic
.LBB45_21:
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.LBB45_22:
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	mas_blk_rdr_panic
.Lfunc_end45:
	.size	blk_generic_freeze, .Lfunc_end45-blk_generic_freeze
                                        // -- End function
	.globl	blk_dev_write_order_preserved // -- Begin function blk_dev_write_order_preserved
	.p2align	2
	.type	blk_dev_write_order_preserved,@function
blk_dev_write_order_preserved:          // @blk_dev_write_order_preserved
.Lblk_dev_write_order_preserved$local:
// %bb.0:
	cbz	x0, .LBB46_6
// %bb.1:
	ldr	x8, [x0, #136]
	cbz	x8, .LBB46_7
// %bb.2:
	ldr	x8, [x8, #1272]
	cbz	x8, .LBB46_7
// %bb.3:
	ldr	x9, [x8, #48]
	cbz	x9, .LBB46_8
// %bb.4:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x8, x8, x10, eq
.LBB46_5:
	ldrb	w8, [x8, #25]
	and	w0, w8, #0x1
.LBB46_6:
	ret
.LBB46_7:
	mov	w0, wzr
	ret
.LBB46_8:
	add	x8, x8, #1408           // =1408
	b	.LBB46_5
.Lfunc_end46:
	.size	blk_dev_write_order_preserved, .Lfunc_end46-blk_dev_write_order_preserved
                                        // -- End function
	.globl	blk_req_set_make_req_nr // -- Begin function blk_req_set_make_req_nr
	.p2align	2
	.type	blk_req_set_make_req_nr,@function
blk_req_set_make_req_nr:                // @blk_req_set_make_req_nr
.Lblk_req_set_make_req_nr$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x0]
	mov	x19, x0
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	ldr	x9, [x8, #48]
	cbz	x9, .LBB47_7
// %bb.1:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x21, x8, x10, eq
	ldr	x8, [x21, #24]
	tbz	w8, #9, .LBB47_3
.LBB47_2:
	ldr	w8, [x19, #24]
	ldr	x9, [x19, #200]
	mov	w10, #59391
	movk	w10, #65471, lsl #16
	and	w8, w8, w10
	and	x9, x9, #0xfffffffffffffffd
	orr	w8, w8, #0x800
	str	x9, [x19, #200]
	str	w8, [x19, #24]
	ldr	x8, [x21, #24]
.LBB47_3:
	tst	x8, #0x300
	b.eq	.LBB47_6
// %bb.4:
	ldr	x8, [x19, #240]
	add	x20, x21, #1912         // =1912
	mov	x0, x20
	orr	x8, x8, #0x4
	str	x8, [x19, #240]
	bl	_raw_spin_lock_irqsave
	ldr	w8, [x21, #1908]
	mov	x1, x0
	cbz	w8, .LBB47_8
.LBB47_5:
	add	w9, w8, #1              // =1
	mov	x0, x20
	str	w9, [x21, #1908]
	str	w8, [x19, #736]
	bl	_raw_spin_unlock_irqrestore
.LBB47_6:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB47_7:
	add	x21, x8, #1408          // =1408
	ldr	x8, [x21, #24]
	tbnz	w8, #9, .LBB47_2
	b	.LBB47_3
.LBB47_8:
	mov	w8, #1
	str	w8, [x21, #1908]
	b	.LBB47_5
.Lfunc_end47:
	.size	blk_req_set_make_req_nr, .Lfunc_end47-blk_req_set_make_req_nr
                                        // -- End function
	.globl	blk_req_get_order_nr    // -- Begin function blk_req_get_order_nr
	.p2align	2
	.type	blk_req_get_order_nr,@function
blk_req_get_order_nr:                   // @blk_req_get_order_nr
.Lblk_req_get_order_nr$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldrb	w8, [x0, #240]
	mov	x19, x0
	mov	x29, sp
	tbnz	w8, #1, .LBB48_4
// %bb.1:
	ldr	x8, [x19]
	mov	w20, w1
	ldr	x9, [x8, #48]
	cbz	x9, .LBB48_12
// %bb.2:
	ldr	x9, [x8, #3968]
	add	x8, x8, #1408           // =1408
	add	x10, x9, #64            // =64
	cmp	x9, #0                  // =0
	csel	x22, x8, x10, eq
	ldr	w1, [x19, #740]
	cbnz	w1, .LBB48_13
.LBB48_3:
	ldrb	w8, [x22, #25]
	tbnz	w8, #0, .LBB48_7
.LBB48_4:
	mov	w21, wzr
.LBB48_5:
	str	w21, [x19, #740]
.LBB48_6:
	mov	w0, w21
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB48_7:
	tbz	w20, #0, .LBB48_9
// %bb.8:
	mov	x1, xzr
	b	.LBB48_10
.LBB48_9:
	add	x0, x22, #1904          // =1904
	bl	_raw_spin_lock_irqsave
	mov	x1, x0
.LBB48_10:
	ldr	w8, [x22, #1900]
	adds	w9, w8, #1              // =1
	add	w8, w8, #2              // =2
	csel	w21, w8, w9, hs
	str	w21, [x22, #1900]
	tbnz	w20, #0, .LBB48_5
// %bb.11:
	add	x0, x22, #1904          // =1904
	bl	_raw_spin_unlock_irqrestore
	b	.LBB48_5
.LBB48_12:
	add	x22, x8, #1408          // =1408
	ldr	w1, [x19, #740]
	cbz	w1, .LBB48_3
.LBB48_13:
	adrp	x0, .L.str.6
	add	x0, x0, :lo12:.L.str.6
	bl	printk
	ldr	w21, [x19, #740]
	b	.LBB48_6
.Lfunc_end48:
	.size	blk_req_get_order_nr, .Lfunc_end48-blk_req_get_order_nr
                                        // -- End function
	.globl	blk_queue_order_enable  // -- Begin function blk_queue_order_enable
	.p2align	2
	.type	blk_queue_order_enable,@function
blk_queue_order_enable:                 // @blk_queue_order_enable
.Lblk_queue_order_enable$local:
// %bb.0:
	ldr	x8, [x0, #48]
	cbz	x8, .LBB49_3
// %bb.1:
	ldr	x8, [x0, #3968]
	add	x9, x0, #1408           // =1408
	add	x10, x8, #64            // =64
	cmp	x8, #0                  // =0
	csel	x8, x9, x10, eq
.LBB49_2:
	ldr	x9, [x8, #24]
	tst	w1, #0x1
	and	w10, w1, #0x1
	strb	w10, [x8, #1896]
	and	x11, x9, #0xfffffffffffffeff
	orr	x9, x9, #0x100
	csel	x9, x9, x11, ne
	str	x9, [x8, #24]
	ret
.LBB49_3:
	add	x8, x0, #1408           // =1408
	b	.LBB49_2
.Lfunc_end49:
	.size	blk_queue_order_enable, .Lfunc_end49-blk_queue_order_enable
                                        // -- End function
	.globl	blk_queue_query_order_enable // -- Begin function blk_queue_query_order_enable
	.p2align	2
	.type	blk_queue_query_order_enable,@function
blk_queue_query_order_enable:           // @blk_queue_query_order_enable
.Lblk_queue_query_order_enable$local:
// %bb.0:
	ldr	x8, [x0, #48]
	cbz	x8, .LBB50_3
// %bb.1:
	ldr	x8, [x0, #3968]
	add	x9, x0, #1408           // =1408
	add	x10, x8, #64            // =64
	cmp	x8, #0                  // =0
	csel	x8, x9, x10, eq
.LBB50_2:
	ldrb	w8, [x8, #25]
	and	w0, w8, #0x1
	ret
.LBB50_3:
	add	x8, x0, #1408           // =1408
	b	.LBB50_2
.Lfunc_end50:
	.size	blk_queue_query_order_enable, .Lfunc_end50-blk_queue_query_order_enable
                                        // -- End function
	.globl	blk_order_nr_reset      // -- Begin function blk_order_nr_reset
	.p2align	2
	.type	blk_order_nr_reset,@function
blk_order_nr_reset:                     // @blk_order_nr_reset
.Lblk_order_nr_reset$local:
// %bb.0:
	cbz	x0, .LBB51_2
// %bb.1:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	add	x8, x0, #64             // =64
	str	wzr, [x0, #1964]
	mov	x0, x8
	mov	x29, sp
	bl	mas_blk_order_info_reset
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
.LBB51_2:
	ret
.Lfunc_end51:
	.size	blk_order_nr_reset, .Lfunc_end51-blk_order_nr_reset
                                        // -- End function
	.globl	blk_queue_cp_enable     // -- Begin function blk_queue_cp_enable
	.p2align	2
	.type	blk_queue_cp_enable,@function
blk_queue_cp_enable:                    // @blk_queue_cp_enable
.Lblk_queue_cp_enable$local:
// %bb.0:
	ldr	x8, [x0, #48]
	cbz	x8, .LBB52_3
// %bb.1:
	ldr	x8, [x0, #3968]
	add	x9, x0, #1408           // =1408
	add	x10, x8, #64            // =64
	cmp	x8, #0                  // =0
	csel	x8, x9, x10, eq
.LBB52_2:
	ldr	x9, [x8, #24]
	tst	w1, #0x1
	and	x10, x9, #0xffffffffffffff7f
	orr	x9, x9, #0x80
	csel	x9, x9, x10, ne
	str	x9, [x8, #24]
	ret
.LBB52_3:
	add	x8, x0, #1408           // =1408
	b	.LBB52_2
.Lfunc_end52:
	.size	blk_queue_cp_enable, .Lfunc_end52-blk_queue_cp_enable
                                        // -- End function
	.globl	mas_queue_status_show   // -- Begin function mas_queue_status_show
	.p2align	2
	.type	mas_queue_status_show,@function
mas_queue_status_show:                  // @mas_queue_status_show
.Lmas_queue_status_show$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x8, [x0, #48]
	mov	x20, x2
	mov	x19, x0
	mov	x21, x1
	mov	x29, sp
	cbz	x8, .LBB53_3
// %bb.1:
	ldr	x8, [x19, #3968]
	add	x9, x19, #1408          // =1408
	add	x10, x8, #64            // =64
	cmp	x8, #0                  // =0
	csel	x0, x9, x10, eq
.LBB53_2:
	mov	x1, x21
	mov	w2, w20
	bl	mas_blk_dump_lld_status
	mov	w22, w0
	add	x1, x21, w0, sxtw
	sub	w2, w20, w0
	mov	x0, x19
	bl	mas_blk_dump_queue_status2
	add	w8, w0, w22
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	sxtw	x0, w8
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB53_3:
	add	x0, x19, #1408          // =1408
	b	.LBB53_2
.Lfunc_end53:
	.size	mas_queue_status_show, .Lfunc_end53-mas_queue_status_show
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Unknown lld type"
	.size	.L.str, 17

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"%d\n"
	.size	.L.str.1, 4

	.type	process_io_prio_array,@object // @process_io_prio_array
	.section	.rodata,"a",@progbits
	.p2align	2
process_io_prio_array:
	.asciz	"fio_vip\000\000\000\000\000\000\000\000"
	.word	4196352                 // 0x400800
	.word	2                       // 0x2
	.word	0                       // 0x0
	.asciz	"fio_fg\000\000\000\000\000\000\000\000\000"
	.word	4196352                 // 0x400800
	.word	0                       // 0x0
	.word	0                       // 0x0
	.asciz	"fio_sync\000\000\000\000\000\000\000"
	.word	2048                    // 0x800
	.word	0                       // 0x0
	.word	0                       // 0x0
	.asciz	"fio_async\000\000\000\000\000\000"
	.word	0                       // 0x0
	.word	0                       // 0x0
	.word	2048                    // 0x800
	.size	process_io_prio_array, 112

	.type	.L.str.2,@object        // @.str.2
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.2:
	.asciz	"\0010[BLK-IO]free counted bio, bio=0x%lx\n"
	.size	.L.str.2, 39

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"\0013[BLK-IO]%s: mq_iosched_init_fn failed. err = %d\n"
	.size	.L.str.3, 51

	.type	.L__func__.mas_blk_mq_init_allocated_queue,@object // @__func__.mas_blk_mq_init_allocated_queue
.L__func__.mas_blk_mq_init_allocated_queue:
	.asciz	"mas_blk_mq_init_allocated_queue"
	.size	.L__func__.mas_blk_mq_init_allocated_queue, 32

	.type	mas_blk_allocated_tags_init.__key,@object // @mas_blk_allocated_tags_init.__key
	.local	mas_blk_allocated_tags_init.__key
	.comm	mas_blk_allocated_tags_init.__key,1,1
	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"&tags->tag_list_lock"
	.size	.L.str.4, 21

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"freeze_obj is NULL!"
	.size	.L.str.5, 20

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"\0013[BLK-IO]old protocol_nr exist! %u\n"
	.size	.L.str.6, 37

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"\0013[BLK-IO]%s\n"
	.size	.L.str.7, 14

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"\0013[BLK-IO]%s: Invalid ci_key_len!\n"
	.size	.L.str.8, 35

	.type	.L__func__.mas_blk_lld_inline_crypto_bio_split_pre,@object // @__func__.mas_blk_lld_inline_crypto_bio_split_pre
.L__func__.mas_blk_lld_inline_crypto_bio_split_pre:
	.asciz	"mas_blk_lld_inline_crypto_bio_split_pre"
	.size	.L__func__.mas_blk_lld_inline_crypto_bio_split_pre, 40

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"\0013[BLK-IO]%s: init key len invalid!\n"
	.size	.L.str.9, 37

	.type	.L__func__.mas_blk_lld_inline_crypto_bio_split_post,@object // @__func__.mas_blk_lld_inline_crypto_bio_split_post
.L__func__.mas_blk_lld_inline_crypto_bio_split_post:
	.asciz	"mas_blk_lld_inline_crypto_bio_split_post"
	.size	.L__func__.mas_blk_lld_inline_crypto_bio_split_post, 41

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"\0013[BLK-IO]%s: key len invalid!\n"
	.size	.L.str.10, 32

	.type	.L__func__.mas_blk_lld_inline_crypto_bio_merge_allow,@object // @__func__.mas_blk_lld_inline_crypto_bio_merge_allow
.L__func__.mas_blk_lld_inline_crypto_bio_merge_allow:
	.asciz	"mas_blk_lld_inline_crypto_bio_merge_allow"
	.size	.L__func__.mas_blk_lld_inline_crypto_bio_merge_allow, 42

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"\0013[BLK-IO]%s: init rq key len not %d or %d\n"
	.size	.L.str.11, 44

	.type	.L__func__.mas_blk_lld_inline_crypto_init_request_from_bio,@object // @__func__.mas_blk_lld_inline_crypto_init_request_from_bio
.L__func__.mas_blk_lld_inline_crypto_init_request_from_bio:
	.asciz	"mas_blk_lld_inline_crypto_init_request_from_bio"
	.size	.L__func__.mas_blk_lld_inline_crypto_init_request_from_bio, 48

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"Invalid ci_key_len!"
	.size	.L.str.12, 20

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"blk_lld has been inited already!"
	.size	.L.str.13, 33

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
