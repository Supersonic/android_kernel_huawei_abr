	.text
	.file	"mas-ufs-bkops.c"
	.globl	mas_bkops_send_stop_enable // -- Begin function mas_bkops_send_stop_enable
	.p2align	2
	.type	mas_bkops_send_stop_enable,@function
mas_bkops_send_stop_enable:             // @mas_bkops_send_stop_enable
.Lmas_bkops_send_stop_enable$local:
// %bb.0:
	adrp	x8, mas_bkops_send_stop
	mov	w9, #1
	strb	w9, [x8, :lo12:mas_bkops_send_stop]
	ret
.Lfunc_end0:
	.size	mas_bkops_send_stop_enable, .Lfunc_end0-mas_bkops_send_stop_enable
                                        // -- End function
	.globl	mas_bkops_send_stop_disable // -- Begin function mas_bkops_send_stop_disable
	.p2align	2
	.type	mas_bkops_send_stop_disable,@function
mas_bkops_send_stop_disable:            // @mas_bkops_send_stop_disable
.Lmas_bkops_send_stop_disable$local:
// %bb.0:
	adrp	x8, mas_bkops_send_stop
	strb	wzr, [x8, :lo12:mas_bkops_send_stop]
	ret
.Lfunc_end1:
	.size	mas_bkops_send_stop_disable, .Lfunc_end1-mas_bkops_send_stop_disable
                                        // -- End function
	.globl	ufshcd_bkops_status_query // -- Begin function ufshcd_bkops_status_query
	.p2align	2
	.type	ufshcd_bkops_status_query,@function
ufshcd_bkops_status_query:              // @ufshcd_bkops_status_query
.Lufshcd_bkops_status_query$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	w19, #-22
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB2_5
// %bb.1:
	mov	x20, x1
	cbz	x1, .LBB2_5
// %bb.2:
	mov	x21, x0
	ldr	x22, [x0, #23040]
	ldr	x0, [x0, #64]
	mov	w1, #4
	bl	__pm_runtime_resume
	ldr	x8, [x21, #23032]
	mov	x0, x21
	mov	x1, x20
	ldr	x8, [x8]
	blr	x8
	ldr	x8, [x21, #64]
	mov	w19, w0
	mov	w1, #4
	mov	x0, x8
	bl	__pm_runtime_idle
	ldrb	w8, [x22, #571]
	cbnz	w8, .LBB2_6
// %bb.3:
	ldrb	w8, [x22, #570]
	cbnz	w8, .LBB2_7
.LBB2_4:
	cbnz	w19, .LBB2_8
.LBB2_5:
	mov	w0, w19
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB2_6:
	adrp	x0, .L.str
	mov	w8, #1
	add	x0, x0, :lo12:.L.str
	str	w8, [x20]
	bl	printk
	strb	wzr, [x22, #571]
	ldrb	w8, [x22, #570]
	cbz	w8, .LBB2_4
.LBB2_7:
	adrp	x0, .L.str.1
	add	x0, x0, :lo12:.L.str.1
	str	wzr, [x20]
	bl	printk
	strb	wzr, [x22, #570]
	mov	w19, #-1
.LBB2_8:
	adrp	x0, .L.str.2
	add	x0, x0, :lo12:.L.str.2
	bl	printk
	b	.LBB2_5
.Lfunc_end2:
	.size	ufshcd_bkops_status_query, .Lfunc_end2-ufshcd_bkops_status_query
                                        // -- End function
	.globl	ufshcd_bkops_start_stop // -- Begin function ufshcd_bkops_start_stop
	.p2align	2
	.type	ufshcd_bkops_start_stop,@function
ufshcd_bkops_start_stop:                // @ufshcd_bkops_start_stop
.Lufshcd_bkops_start_stop$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB3_4
// %bb.1:
	mov	x19, x0
	cbz	w1, .LBB3_5
// %bb.2:
	ldr	x0, [x19, #64]
	mov	w1, #4
	bl	__pm_runtime_resume
	ldr	x8, [x19, #23032]
	mov	x0, x19
	ldr	x8, [x8, #8]
	blr	x8
	ldr	x8, [x19, #64]
	mov	w19, w0
	mov	w1, #4
	mov	x0, x8
	bl	__pm_runtime_idle
	cbz	w19, .LBB3_11
// %bb.3:
	adrp	x0, .L.str.5
	add	x0, x0, :lo12:.L.str.5
	b	.LBB3_13
.LBB3_4:
	mov	w19, #-22
	b	.LBB3_11
.LBB3_5:
	ldr	x22, [x19, #23040]
	add	x20, x22, #296          // =296
	bl	ktime_get
	adrp	x8, mas_bkops_send_stop
	ldrb	w8, [x8, :lo12:mas_bkops_send_stop]
	mov	x21, x0
	cmp	w8, #1                  // =1
	b.ne	.LBB3_7
// %bb.6:
	ldr	x0, [x19, #64]
	mov	w1, #4
	bl	__pm_runtime_resume
	ldr	x8, [x19, #23032]
	mov	x0, x19
	ldr	x8, [x8, #16]
	blr	x8
	ldr	x8, [x19, #64]
	mov	w19, w0
	mov	w1, #4
	mov	x0, x8
	bl	__pm_runtime_idle
	b	.LBB3_8
.LBB3_7:
	mov	w19, wzr
.LBB3_8:
	ldr	w8, [x22, #304]
	mov	x0, x20
	add	w8, w8, #1              // =1
	str	w8, [x22, #304]
	bl	mas_bkops_update_dur
	bl	ktime_get
	ldr	x9, [x22, #392]
	sub	x8, x0, x21
	cmp	x8, x9
	b.ls	.LBB3_10
// %bb.9:
	str	x8, [x22, #392]
.LBB3_10:
	ldr	w9, [x22, #304]
	ldr	x10, [x22, #400]
	sub	w11, w9, #1             // =1
	madd	x8, x10, x11, x8
	udiv	x8, x8, x9
	str	x8, [x22, #400]
	cbnz	w19, .LBB3_12
.LBB3_11:
	mov	w0, w19
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB3_12:
	adrp	x0, .L.str.6
	add	x0, x0, :lo12:.L.str.6
.LBB3_13:
	bl	printk
	b	.LBB3_11
.Lfunc_end3:
	.size	ufshcd_bkops_start_stop, .Lfunc_end3-ufshcd_bkops_start_stop
                                        // -- End function
	.globl	mas_ufs_manual_bkops_config // -- Begin function mas_ufs_manual_bkops_config
	.p2align	2
	.type	mas_ufs_manual_bkops_config,@function
mas_ufs_manual_bkops_config:            // @mas_ufs_manual_bkops_config
.Lmas_ufs_manual_bkops_config$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x24, x23, [sp, #16]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB4_11
// %bb.1:
	ldr	x22, [x0]
	mov	x20, x0
	ldrb	w8, [x22, #2432]
	tbz	w8, #0, .LBB4_10
// %bb.2:
	ldr	x21, [x22, #27752]
	ldr	x19, [x20, #8]
	mov	w8, #27752
	add	x23, x22, x8
	cmp	x21, x23
	b.eq	.LBB4_5
.LBB4_3:                                // =>This Inner Loop Header: Depth=1
	ldp	x2, x3, [x20, #240]
	ldrh	w1, [x22, #5088]
	sub	x0, x21, #24            // =24
	bl	mas_ufs_is_bkops_id_match
	tbnz	w0, #0, .LBB4_9
// %bb.4:                               //   in Loop: Header=BB4_3 Depth=1
	ldr	x21, [x21]
	cmp	x21, x23
	b.ne	.LBB4_3
.LBB4_5:
	ldr	x23, [x22, #27736]
	mov	w8, #27736
	add	x24, x22, x8
	cmp	x23, x24
	b.eq	.LBB4_9
// %bb.6:
	mov	w8, #4688
	add	x21, x22, x8
.LBB4_7:                                // =>This Inner Loop Header: Depth=1
	ldp	x2, x3, [x20, #240]
	ldrh	w1, [x22, #5088]
	sub	x0, x23, #24            // =24
	bl	mas_ufs_is_bkops_id_match
	tbnz	w0, #0, .LBB4_13
// %bb.8:                               //   in Loop: Header=BB4_7 Depth=1
	ldr	x23, [x23]
	cmp	x23, x24
	b.ne	.LBB4_7
.LBB4_9:
	adrp	x0, .L.str.3
	add	x0, x0, :lo12:.L.str.3
	bl	printk
.LBB4_10:
	mov	w0, wzr
	b	.LBB4_12
.LBB4_11:
	mov	w0, #-22
.LBB4_12:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB4_13:
	mov	w8, #27712
	add	x23, x22, x8
	ldrb	w8, [x23]
	cbnz	w8, .LBB4_9
// %bb.14:
	bl	mas_bkops_alloc
	cbz	x0, .LBB4_18
// %bb.15:
	ldrh	w8, [x22, #5088]
	mov	x20, x0
	cmp	w8, #429                // =429
	b.eq	.LBB4_19
// %bb.16:
	cmp	w8, #2230               // =2230
	b.ne	.LBB4_21
// %bb.17:
	ldr	x8, [x20, #168]
	adrp	x10, hi1861_bkops_ops
	mov	w9, #2
	add	x10, x10, :lo12:hi1861_bkops_ops
	orr	x8, x8, #0x1c
	adrp	x2, bkops_status_str_1861
	str	w9, [x20]
	str	x8, [x20, #168]
	str	x10, [x22, #27720]
	add	x2, x2, :lo12:bkops_status_str_1861
	mov	w1, #6
	b	.LBB4_20
.LBB4_18:
	mov	w0, #-12
	b	.LBB4_12
.LBB4_19:
	ldr	x8, [x20, #168]
	adrp	x10, hynix_bkops_ops
	adrp	x2, bkops_status_str_hynix
	mov	w9, #3
	add	x10, x10, :lo12:hynix_bkops_ops
	orr	x8, x8, #0x1c
	add	x2, x2, :lo12:bkops_status_str_hynix
	mov	w1, #2
	str	w9, [x20]
	str	x8, [x20, #168]
	str	x10, [x22, #27720]
.LBB4_20:
	mov	x0, x20
	bl	mas_bkops_set_status_str
.LBB4_21:
	adrp	x8, ufs_bkops_ops
	adrp	x0, .L.str.4
	add	x8, x8, :lo12:ufs_bkops_ops
	mov	w9, #1
	mov	w10, #3600
	add	x0, x0, :lo12:.L.str.4
	mov	x1, xzr
	str	x20, [x22, #27728]
	stp	x8, x21, [x20, #8]
	str	w9, [x20, #244]
	str	x10, [x20, #248]
	bl	debugfs_create_dir
	cmn	x0, #1, lsl #12         // =4096
	b.hi	.LBB4_24
// %bb.22:
	mov	x21, x0
	mov	x0, x19
	mov	x1, x20
	mov	x2, x21
	bl	mas_bkops_enable
	cbz	w0, .LBB4_25
// %bb.23:
	mov	x0, x21
	bl	debugfs_remove_recursive
.LBB4_24:
	mov	x0, x20
	bl	kfree
	mov	w0, #-11
	b	.LBB4_12
.LBB4_25:
	mov	w8, #1
	strb	w8, [x23]
	ldr	x8, [x22, #4744]
	mov	w9, #4672
	ldrb	w8, [x8, x9]
	cbz	w8, .LBB4_10
// %bb.26:
	mov	x0, x19
	mov	x1, x20
	bl	mas_blk_mq_tagset_set_bkops
	b	.LBB4_10
.Lfunc_end4:
	.size	mas_ufs_manual_bkops_config, .Lfunc_end4-mas_ufs_manual_bkops_config
                                        // -- End function
	.p2align	2               // -- Begin function mas_ufs_is_bkops_id_match
	.type	mas_ufs_is_bkops_id_match,@function
mas_ufs_is_bkops_id_match:              // @mas_ufs_is_bkops_id_match
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldrh	w8, [x0]
	mov	x29, sp
	cmp	w8, w1, uxth
	b.ne	.LBB5_5
// %bb.1:
	ldr	x22, [x0, #8]
	mov	x19, x0
	mov	x20, x3
	mov	x21, x2
	mov	x0, x22
	bl	strlen
	mov	x2, x0
	mov	x0, x21
	mov	x1, x22
	bl	strncmp
	cbz	w0, .LBB5_3
// %bb.2:
	ldr	x1, [x19, #8]
	adrp	x0, .L.str.7
	add	x0, x0, :lo12:.L.str.7
	bl	strcmp
	cbnz	w0, .LBB5_5
.LBB5_3:
	ldr	x21, [x19, #16]
	mov	x0, x21
	bl	strlen
	mov	x2, x0
	mov	x0, x20
	mov	x1, x21
	bl	strncmp
	cbz	w0, .LBB5_7
// %bb.4:
	ldr	x1, [x19, #16]
	adrp	x0, .L.str.8
	add	x0, x0, :lo12:.L.str.8
	bl	strcmp
	cbz	w0, .LBB5_7
.LBB5_5:
	mov	w0, wzr
.LBB5_6:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB5_7:
	mov	w0, #1
	b	.LBB5_6
.Lfunc_end5:
	.size	mas_ufs_is_bkops_id_match, .Lfunc_end5-mas_ufs_is_bkops_id_match
                                        // -- End function
	.p2align	2               // -- Begin function ufshcd_1861_bkops_query
	.type	ufshcd_1861_bkops_query,@function
ufshcd_1861_bkops_query:                // @ufshcd_1861_bkops_query
// %bb.0:
	cbz	x1, .LBB6_2
// %bb.1:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x2, x1
	mov	w1, #242
	mov	x29, sp
	bl	ufshcd_query_hi1861_bkops
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB6_2:
	mov	w0, #-22
	ret
.Lfunc_end6:
	.size	ufshcd_1861_bkops_query, .Lfunc_end6-ufshcd_1861_bkops_query
                                        // -- End function
	.p2align	2               // -- Begin function ufshcd_1861_bkops_start
	.type	ufshcd_1861_bkops_start,@function
ufshcd_1861_bkops_start:                // @ufshcd_1861_bkops_start
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w1, #240
	mov	x2, xzr
	mov	x29, sp
	bl	ufshcd_query_hi1861_bkops
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end7:
	.size	ufshcd_1861_bkops_start, .Lfunc_end7-ufshcd_1861_bkops_start
                                        // -- End function
	.p2align	2               // -- Begin function ufshcd_1861_bkops_stop
	.type	ufshcd_1861_bkops_stop,@function
ufshcd_1861_bkops_stop:                 // @ufshcd_1861_bkops_stop
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w1, #241
	mov	x2, xzr
	mov	x29, sp
	bl	ufshcd_query_hi1861_bkops
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end8:
	.size	ufshcd_1861_bkops_stop, .Lfunc_end8-ufshcd_1861_bkops_stop
                                        // -- End function
	.p2align	2               // -- Begin function ufshcd_query_hi1861_bkops
	.type	ufshcd_query_hi1861_bkops,@function
ufshcd_query_hi1861_bkops:              // @ufshcd_query_hi1861_bkops
// %bb.0:
	sub	sp, sp, #80             // =80
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #32]     // 16-byte Folded Spill
	add	x29, sp, #32            // =32
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	stp	xzr, xzr, [sp, #8]
	ldr	x8, [x0, #56]
	mov	x19, x0
	mov	x21, x2
	mov	w22, w1
	ldr	x0, [x8, #56]
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x19, #56]
	ldr	w20, [x19, #404]
	mov	x1, x0
	ldr	x8, [x8, #56]
	mov	x0, x8
	bl	_raw_spin_unlock_irqrestore
	cmp	w20, #2                 // =2
	b.ne	.LBB9_3
// %bb.1:
	mov	x0, x19
	mov	w1, wzr
	bl	ufshcd_hold
	mov	w8, #20344
	add	x20, x19, x8
	mov	x0, x20
	bl	mutex_lock
	add	x1, sp, #16             // =16
	add	x2, sp, #8              // =8
	mov	x0, x19
	mov	w3, w22
	mov	w4, wzr
	mov	w5, wzr
	mov	w6, wzr
	bl	ufshcd_init_query
	ldr	x8, [sp, #16]
	sub	w9, w22, #240           // =240
	cmp	w9, #2                  // =2
	b.hs	.LBB9_4
// %bb.2:
	mov	w9, #129
	strb	w9, [x8]
	b	.LBB9_6
.LBB9_3:
	mov	w22, #-16
	b	.LBB9_10
.LBB9_4:
	cmp	w22, #242               // =242
	b.ne	.LBB9_12
// %bb.5:
	mov	w9, #1
	strb	w9, [x8]
	cbz	x21, .LBB9_14
.LBB9_6:
	mov	w1, #1
	mov	w2, #1300
	mov	x0, x19
	bl	ufshcd_exec_dev_cmd
	cbnz	w0, .LBB9_13
// %bb.7:
	mov	w22, wzr
	cbz	x21, .LBB9_9
// %bb.8:
	ldr	x8, [sp, #8]
	ldrb	w8, [x8, #19]
	str	w8, [x21]
.LBB9_9:
	mov	x0, x20
	bl	mutex_unlock
	mov	x0, x19
	bl	ufshcd_release
.LBB9_10:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB9_15
// %bb.11:
	mov	w0, w22
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #32]     // 16-byte Folded Reload
	add	sp, sp, #80             // =80
	ret
.LBB9_12:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.11
	adrp	x2, .L__func__.mas_bkops_upiu_query_opcode
	add	x1, x1, :lo12:.L.str.11
	add	x2, x2, :lo12:.L__func__.mas_bkops_upiu_query_opcode
	mov	w3, w22
	bl	_dev_err
	mov	w22, #-22
	b	.LBB9_9
.LBB9_13:
	mov	w22, w0
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.9
	adrp	x2, .L__func__.ufshcd_query_hi1861_bkops
	add	x1, x1, :lo12:.L.str.9
	add	x2, x2, :lo12:.L__func__.ufshcd_query_hi1861_bkops
	mov	w3, w22
	bl	_dev_err
	b	.LBB9_9
.LBB9_14:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.10
	adrp	x2, .L__func__.mas_bkops_upiu_query_opcode
	add	x1, x1, :lo12:.L.str.10
	add	x2, x2, :lo12:.L__func__.mas_bkops_upiu_query_opcode
	bl	_dev_err
	mov	w22, #-22
	b	.LBB9_9
.LBB9_15:
	bl	__stack_chk_fail
.Lfunc_end9:
	.size	ufshcd_query_hi1861_bkops, .Lfunc_end9-ufshcd_query_hi1861_bkops
                                        // -- End function
	.p2align	2               // -- Begin function ufshcd_hynix_bkops_query
	.type	ufshcd_hynix_bkops_query,@function
ufshcd_hynix_bkops_query:               // @ufshcd_hynix_bkops_query
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	cbz	x1, .LBB10_3
// %bb.1:
	ldr	x8, [x0, #56]
	mov	x20, x0
	mov	x19, x1
	ldr	x0, [x8, #56]
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x20, #56]
	ldr	w21, [x20, #404]
	mov	x1, x0
	ldr	x8, [x8, #56]
	mov	x0, x8
	bl	_raw_spin_unlock_irqrestore
	cmp	w21, #2                 // =2
	b.ne	.LBB10_4
// %bb.2:
	mov	w1, #3
	mov	w2, #19
	mov	x0, x20
	mov	w3, wzr
	mov	w4, wzr
	mov	x5, x19
	bl	ufshcd_query_attr
	b	.LBB10_5
.LBB10_3:
	mov	w0, #-22
	b	.LBB10_5
.LBB10_4:
	mov	w0, #-16
.LBB10_5:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end10:
	.size	ufshcd_hynix_bkops_query, .Lfunc_end10-ufshcd_hynix_bkops_query
                                        // -- End function
	.p2align	2               // -- Begin function ufshcd_hynix_bkops_start
	.type	ufshcd_hynix_bkops_start,@function
ufshcd_hynix_bkops_start:               // @ufshcd_hynix_bkops_start
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	w9, #1
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	str	x8, [sp, #8]
	str	w9, [sp, #4]
	ldr	x8, [x0, #56]
	mov	x19, x0
	add	x29, sp, #16            // =16
	ldr	x0, [x8, #56]
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x19, #56]
	ldr	w20, [x19, #404]
	mov	x1, x0
	ldr	x8, [x8, #56]
	mov	x0, x8
	bl	_raw_spin_unlock_irqrestore
	cmp	w20, #2                 // =2
	b.ne	.LBB11_2
// %bb.1:
	add	x5, sp, #4              // =4
	mov	w1, #4
	mov	w2, #18
	mov	x0, x19
	mov	w3, wzr
	mov	w4, wzr
	bl	ufshcd_query_attr
	b	.LBB11_3
.LBB11_2:
	mov	w0, #-16
.LBB11_3:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB11_5
// %bb.4:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB11_5:
	bl	__stack_chk_fail
.Lfunc_end11:
	.size	ufshcd_hynix_bkops_start, .Lfunc_end11-ufshcd_hynix_bkops_start
                                        // -- End function
	.p2align	2               // -- Begin function ufshcd_hynix_bkops_stop
	.type	ufshcd_hynix_bkops_stop,@function
ufshcd_hynix_bkops_stop:                // @ufshcd_hynix_bkops_stop
// %bb.0:
	sub	sp, sp, #48             // =48
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x19, x0
	str	x8, [sp, #8]
	str	wzr, [sp, #4]
	ldr	x8, [x0, #56]
	add	x29, sp, #16            // =16
	ldr	x0, [x8, #56]
	bl	_raw_spin_lock_irqsave
	ldr	x8, [x19, #56]
	ldr	w20, [x19, #404]
	mov	x1, x0
	ldr	x8, [x8, #56]
	mov	x0, x8
	bl	_raw_spin_unlock_irqrestore
	cmp	w20, #2                 // =2
	b.ne	.LBB12_2
// %bb.1:
	add	x5, sp, #4              // =4
	mov	w1, #4
	mov	w2, #18
	mov	x0, x19
	mov	w3, wzr
	mov	w4, wzr
	bl	ufshcd_query_attr
	b	.LBB12_3
.LBB12_2:
	mov	w0, #-16
.LBB12_3:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB12_5
// %bb.4:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #48             // =48
	ret
.LBB12_5:
	bl	__stack_chk_fail
.Lfunc_end12:
	.size	ufshcd_hynix_bkops_stop, .Lfunc_end12-ufshcd_hynix_bkops_stop
                                        // -- End function
	.type	mas_bkops_send_stop,@object // @mas_bkops_send_stop
	.local	mas_bkops_send_stop
	.comm	mas_bkops_send_stop,1,4
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\0013sim_critical_bkops\n"
	.size	.L.str, 22

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"\0013simulate bkops query failure!\n"
	.size	.L.str.1, 33

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"\0013UFS bkops status query failed!\n"
	.size	.L.str.2, 34

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"\0013bkops not supported or has been enabled already!\n"
	.size	.L.str.3, 52

	.type	ufs_bkops_ops,@object   // @ufs_bkops_ops
	.data
	.p2align	3
ufs_bkops_ops:
	.xword	__ufshcd_bkops_start_stop
	.xword	__ufshcd_bkops_status_query
	.size	ufs_bkops_ops, 16

	.type	.L.str.4,@object        // @.str.4
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.4:
	.asciz	"bkops"
	.size	.L.str.4, 6

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"\0013UFS bkops start failed!\n"
	.size	.L.str.5, 27

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"\0013UFS bkops stop failed!\n"
	.size	.L.str.6, 26

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"MAS_UFS_MODEL_ANY"
	.size	.L.str.7, 18

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"MAS_UFS_REV_ANY"
	.size	.L.str.8, 16

	.type	hi1861_bkops_ops,@object // @hi1861_bkops_ops
	.data
	.p2align	3
hi1861_bkops_ops:
	.xword	ufshcd_1861_bkops_query
	.xword	ufshcd_1861_bkops_start
	.xword	ufshcd_1861_bkops_stop
	.size	hi1861_bkops_ops, 24

	.type	bkops_status_str_1861,@object // @bkops_status_str_1861
	.p2align	3
bkops_status_str_1861:
	.xword	.L.str.12
	.xword	.L.str.13
	.xword	.L.str.14
	.xword	.L.str.15
	.xword	.L.str.16
	.xword	.L.str.17
	.size	bkops_status_str_1861, 48

	.type	hynix_bkops_ops,@object // @hynix_bkops_ops
	.p2align	3
hynix_bkops_ops:
	.xword	ufshcd_hynix_bkops_query
	.xword	ufshcd_hynix_bkops_start
	.xword	ufshcd_hynix_bkops_stop
	.size	hynix_bkops_ops, 24

	.type	bkops_status_str_hynix,@object // @bkops_status_str_hynix
	.p2align	3
bkops_status_str_hynix:
	.xword	.L.str.12
	.xword	.L.str.18
	.size	bkops_status_str_hynix, 16

	.type	.L.str.9,@object        // @.str.9
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.9:
	.asciz	"%s: Sending bkops query failed, err = %d\n"
	.size	.L.str.9, 42

	.type	.L__func__.ufshcd_query_hi1861_bkops,@object // @__func__.ufshcd_query_hi1861_bkops
.L__func__.ufshcd_query_hi1861_bkops:
	.asciz	"ufshcd_query_hi1861_bkops"
	.size	.L__func__.ufshcd_query_hi1861_bkops, 26

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"%s: Invalid argument for read request\n"
	.size	.L.str.10, 39

	.type	.L__func__.mas_bkops_upiu_query_opcode,@object // @__func__.mas_bkops_upiu_query_opcode
.L__func__.mas_bkops_upiu_query_opcode:
	.asciz	"mas_bkops_upiu_query_opcode"
	.size	.L__func__.mas_bkops_upiu_query_opcode, 28

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"%s: Expected query bkops opcode but got = %d\n"
	.size	.L.str.11, 46

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"bkops none"
	.size	.L.str.12, 11

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"bkops normal"
	.size	.L.str.13, 13

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"bkops urgent count"
	.size	.L.str.14, 19

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"bkops wl"
	.size	.L.str.15, 9

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"bkops not care"
	.size	.L.str.16, 15

	.type	.L.str.17,@object       // @.str.17
.L.str.17:
	.asciz	"bkops slc cache"
	.size	.L.str.17, 16

	.type	.L.str.18,@object       // @.str.18
.L.str.18:
	.asciz	"bkops needed"
	.size	.L.str.18, 13

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
