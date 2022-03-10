	.text
	.file	"mas_blk_flush.c"
                                        // Start of file scope inline assembly
.section	".initcall6.init", "a"	
__initcall___cfi_mas_blk_flush_init6:			
.long	__cfi_mas_blk_flush_init - .			
.previous					

                                        // End of file scope inline assembly
	.globl	mas_blk_flush_work_fn   // -- Begin function mas_blk_flush_work_fn
	.p2align	2
	.type	mas_blk_flush_work_fn,@function
mas_blk_flush_work_fn:                  // @mas_blk_flush_work_fn
.Lmas_blk_flush_work_fn$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	ldur	x0, [x0, #-40]
	mov	w1, wzr
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	bl	bdget_disk
	cbz	x0, .LBB0_4
// %bb.1:
	mov	w1, #2
	mov	x2, xzr
	mov	x19, x0
	bl	blkdev_get
	cbnz	w0, .LBB0_5
// %bb.2:
	mov	w1, #3264
	mov	x0, x19
	mov	x2, xzr
	bl	blkdev_issue_flush
	cbnz	w0, .LBB0_6
.LBB0_3:
	mov	w1, #2
	mov	x0, x19
	bl	blkdev_put
.LBB0_4:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB0_5:
	adrp	x0, .L.str
	adrp	x1, .L__func__.mas_blk_flush_work_fn
	add	x0, x0, :lo12:.L.str
	add	x1, x1, :lo12:.L__func__.mas_blk_flush_work_fn
	bl	printk
	b	.LBB0_4
.LBB0_6:
	adrp	x0, .L.str.1
	adrp	x1, .L__func__.mas_blk_flush_work_fn
	add	x0, x0, :lo12:.L.str.1
	add	x1, x1, :lo12:.L__func__.mas_blk_flush_work_fn
	bl	printk
	b	.LBB0_3
.Lfunc_end0:
	.size	mas_blk_flush_work_fn, .Lfunc_end0-mas_blk_flush_work_fn
                                        // -- End function
	.globl	mas_blk_flush_async_dispatch // -- Begin function mas_blk_flush_async_dispatch
	.p2align	2
	.type	mas_blk_flush_async_dispatch,@function
mas_blk_flush_async_dispatch:           // @mas_blk_flush_async_dispatch
.Lmas_blk_flush_async_dispatch$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x20, x1
	mov	x19, x0
	bl	blk_queue_query_unistore_enable
	tbz	w0, #0, .LBB1_3
// %bb.1:
	ldrb	w8, [x20, #88]
	tbnz	w8, #1, .LBB1_3
// %bb.2:
	ldr	w8, [x20, #40]
	cbz	w8, .LBB1_8
.LBB1_3:
	ldr	w8, [x19, #1272]
	cbz	w8, .LBB1_8
// %bb.4:
	ldr	w8, [x20, #16]
	tbz	w8, #19, .LBB1_6
// %bb.5:
	ldr	w9, [x20, #40]
	cbz	w9, .LBB1_10
.LBB1_6:
	and	w8, w8, #0xff
	cmp	w8, #1                  // =1
	b.ne	.LBB1_8
// %bb.7:
	add	x8, x19, #1244          // =1244
	//APP
	// atomic_add
	prfm	pstl1strm, [x8]
1:	ldxr	w9, [x8]
	add	w9, w9, 1
	stxr	w10, w9, [x8]
	cbnz	w10, 1b

	//NO_APP
.LBB1_8:
	mov	w0, wzr
.LBB1_9:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB1_10:
	ldrb	w9, [x20, #146]
	cbz	w9, .LBB1_12
// %bb.11:
	add	x19, x19, #1104         // =1104
	mov	x0, x19
	bl	cancel_delayed_work
	adrp	x8, blk_async_flush_workqueue
	ldr	x1, [x8, :lo12:blk_async_flush_workqueue]
	mov	w0, #8
	mov	w3, #5
	mov	x2, x19
	bl	queue_delayed_work_on
	mov	w0, #1
	b	.LBB1_9
.LBB1_12:
	ldr	w9, [x19, #1244]
	cbnz	w9, .LBB1_6
// %bb.13:
	mov	w0, #1
	b	.LBB1_9
.Lfunc_end1:
	.size	mas_blk_flush_async_dispatch, .Lfunc_end1-mas_blk_flush_async_dispatch
                                        // -- End function
	.globl	mas_blk_flush_update    // -- Begin function mas_blk_flush_update
	.p2align	2
	.type	mas_blk_flush_update,@function
mas_blk_flush_update:                   // @mas_blk_flush_update
.Lmas_blk_flush_update$local:
// %bb.0:
	cbnz	w1, .LBB2_2
// %bb.1:
	ldr	x8, [x0]
	str	wzr, [x8, #1244]
.LBB2_2:
	ret
.Lfunc_end2:
	.size	mas_blk_flush_update, .Lfunc_end2-mas_blk_flush_update
                                        // -- End function
	.globl	blk_power_off_flush     // -- Begin function blk_power_off_flush
	.p2align	2
	.type	blk_power_off_flush,@function
blk_power_off_flush:                    // @blk_power_off_flush
.Lblk_power_off_flush$local:
// %bb.0:
	stp	x29, x30, [sp, #-96]!   // 16-byte Folded Spill
	mov	w2, w0
	adrp	x0, .L.str.2
	adrp	x1, .L__func__.blk_power_off_flush
	add	x0, x0, :lo12:.L.str.2
	add	x1, x1, :lo12:.L__func__.blk_power_off_flush
	stp	x28, x27, [sp, #16]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #32]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]     // 16-byte Folded Spill
	mov	x29, sp
	bl	printk
	adrp	x0, all_flush_q_lock
	add	x0, x0, :lo12:all_flush_q_lock
	bl	mutex_lock
	adrp	x26, all_flush_q_list
	add	x26, x26, :lo12:all_flush_q_list
	ldr	x27, [x26]
	cmp	x27, x26
	b.eq	.LBB3_18
// %bb.1:
	adrp	x19, .L.str.3
	adrp	x20, .L__func__.blk_power_off_flush
	adrp	x21, .L.str.4
	adrp	x22, .L.str.1
	adrp	x23, .L.str.5
	adrp	x24, .L.str
	add	x19, x19, :lo12:.L.str.3
	add	x20, x20, :lo12:.L__func__.blk_power_off_flush
	add	x21, x21, :lo12:.L.str.4
	add	x22, x22, :lo12:.L.str.1
	add	x23, x23, :lo12:.L.str.5
	add	x24, x24, :lo12:.L.str
.LBB3_2:                                // =>This Inner Loop Header: Depth=1
	ldurb	w8, [x27, #-184]
	cbz	w8, .LBB3_17
// %bb.3:                               //   in Loop: Header=BB3_2 Depth=1
	sub	x0, x27, #1256          // =1256
	bl	mas_blk_get_lld
	cbz	x0, .LBB3_17
// %bb.4:                               //   in Loop: Header=BB3_2 Depth=1
	ldr	x8, [x0, #56]
	cbz	x8, .LBB3_17
// %bb.5:                               //   in Loop: Header=BB3_2 Depth=1
	ldur	x28, [x27, #-192]
	cbz	x28, .LBB3_7
// %bb.6:                               //   in Loop: Header=BB3_2 Depth=1
	add	x2, x28, #12            // =12
	mov	x0, x19
	mov	x1, x20
	bl	printk
	ldur	x0, [x27, #-192]
	b	.LBB3_8
.LBB3_7:                                //   in Loop: Header=BB3_2 Depth=1
	mov	x0, xzr
.LBB3_8:                                //   in Loop: Header=BB3_2 Depth=1
	mov	w1, wzr
	bl	bdget_disk
	cbnz	x0, .LBB3_10
// %bb.9:                               //   in Loop: Header=BB3_2 Depth=1
	mov	x0, x21
	mov	x1, x20
	bl	printk
	b	.LBB3_17
.LBB3_10:                               //   in Loop: Header=BB3_2 Depth=1
	mov	w1, #2
	mov	x2, xzr
	mov	x25, x0
	bl	blkdev_get
	cbnz	w0, .LBB3_14
// %bb.11:                              //   in Loop: Header=BB3_2 Depth=1
	mov	w1, #3264
	mov	x0, x25
	mov	x2, xzr
	bl	blkdev_issue_flush
	cbz	w0, .LBB3_13
// %bb.12:                              //   in Loop: Header=BB3_2 Depth=1
	mov	x0, x22
	mov	x1, x20
	bl	printk
.LBB3_13:                               //   in Loop: Header=BB3_2 Depth=1
	mov	w1, #2
	mov	x0, x25
	bl	blkdev_put
	b	.LBB3_15
.LBB3_14:                               //   in Loop: Header=BB3_2 Depth=1
	mov	x0, x24
	mov	x1, x20
	bl	printk
.LBB3_15:                               //   in Loop: Header=BB3_2 Depth=1
	cbz	x28, .LBB3_17
// %bb.16:                              //   in Loop: Header=BB3_2 Depth=1
	add	x2, x28, #12            // =12
	mov	x0, x23
	mov	x1, x20
	bl	printk
.LBB3_17:                               //   in Loop: Header=BB3_2 Depth=1
	ldr	x27, [x27]
	cmp	x27, x26
	b.ne	.LBB3_2
.LBB3_18:
	adrp	x0, all_flush_q_lock
	add	x0, x0, :lo12:all_flush_q_lock
	bl	mutex_unlock
	adrp	x0, .L.str.6
	adrp	x1, .L__func__.blk_power_off_flush
	add	x0, x0, :lo12:.L.str.6
	add	x1, x1, :lo12:.L__func__.blk_power_off_flush
	bl	printk
	ldp	x20, x19, [sp, #80]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #96     // 16-byte Folded Reload
	ret
.Lfunc_end3:
	.size	blk_power_off_flush, .Lfunc_end3-blk_power_off_flush
                                        // -- End function
	.globl	mas_blk_flush_list_register // -- Begin function mas_blk_flush_list_register
	.p2align	2
	.type	mas_blk_flush_list_register,@function
mas_blk_flush_list_register:            // @mas_blk_flush_list_register
.Lmas_blk_flush_list_register$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	adrp	x20, all_object_lock
	add	x20, x20, :lo12:all_object_lock
	mov	x19, x0
	mov	x0, x20
	mov	x29, sp
	bl	_raw_spin_lock_irqsave
	adrp	x8, all_object_list
	add	x8, x8, :lo12:all_object_list
	ldr	x9, [x8]
	mov	x1, x0
	adrp	x10, all_object_list
	mov	x0, x20
	str	x19, [x9, #8]
	stp	x9, x8, [x19]
	str	x19, [x10, :lo12:all_object_list]
	bl	_raw_spin_unlock_irqrestore
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end4:
	.size	mas_blk_flush_list_register, .Lfunc_end4-mas_blk_flush_list_register
                                        // -- End function
	.globl	mas_blk_flush_list_unregister // -- Begin function mas_blk_flush_list_unregister
	.p2align	2
	.type	mas_blk_flush_list_unregister,@function
mas_blk_flush_list_unregister:          // @mas_blk_flush_list_unregister
.Lmas_blk_flush_list_unregister$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	adrp	x20, all_object_lock
	add	x20, x20, :lo12:all_object_lock
	mov	x19, x0
	mov	x0, x20
	mov	x29, sp
	bl	_raw_spin_lock_irqsave
	ldp	x9, x8, [x19]
	mov	x1, x0
	mov	x0, x20
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x19, [x19]
	str	x19, [x19, #8]
	bl	_raw_spin_unlock_irqrestore
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end5:
	.size	mas_blk_flush_list_unregister, .Lfunc_end5-mas_blk_flush_list_unregister
                                        // -- End function
	.globl	mas_blk_order_panic_wait_flush // -- Begin function mas_blk_order_panic_wait_flush
	.p2align	2
	.type	mas_blk_order_panic_wait_flush,@function
mas_blk_order_panic_wait_flush:         // @mas_blk_order_panic_wait_flush
.Lmas_blk_order_panic_wait_flush$local:
// %bb.0:
	stp	x29, x30, [sp, #-96]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]     // 16-byte Folded Spill
	adrp	x19, .L__func__.mas_blk_order_panic_wait_flush
	adrp	x0, .L.str.7
	add	x19, x19, :lo12:.L__func__.mas_blk_order_panic_wait_flush
	add	x0, x0, :lo12:.L.str.7
	mov	x1, x19
	stp	x28, x27, [sp, #16]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #32]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]     // 16-byte Folded Spill
	mov	x29, sp
	bl	printk
	adrp	x20, all_object_lock
	adrp	x25, all_object_list
	adrp	x26, .L.str.10
	adrp	x27, .L.str.9
	adrp	x21, .L.str.8
	mov	w24, #200
	add	x20, x20, :lo12:all_object_lock
	add	x25, x25, :lo12:all_object_list
	add	x26, x26, :lo12:.L.str.10
	add	x27, x27, :lo12:.L.str.9
	add	x21, x21, :lo12:.L.str.8
.LBB6_1:                                // =>This Loop Header: Depth=1
                                        //     Child Loop BB6_4 Depth 2
	mov	w0, #35160
	movk	w0, #65, lsl #16
	bl	__const_udelay
	mov	x0, x20
	bl	_raw_spin_lock_irqsave
	ldr	x28, [x25]
	mov	x22, x0
	cmp	x28, x25
	b.ne	.LBB6_3
// %bb.2:                               //   in Loop: Header=BB6_1 Depth=1
	mov	x0, x20
	mov	x1, x22
	bl	_raw_spin_unlock_irqrestore
	b	.LBB6_11
.LBB6_3:                                //   in Loop: Header=BB6_1 Depth=1
	mov	w23, wzr
.LBB6_4:                                //   Parent Loop BB6_1 Depth=1
                                        // =>  This Inner Loop Header: Depth=2
	sub	x8, x28, #1916          // =1916
	ldr	w2, [x8]
	cmp	w2, #2                  // =2
	b.ne	.LBB6_7
// %bb.5:                               //   in Loop: Header=BB6_4 Depth=2
	sub	x8, x28, #1896          // =1896
	ldr	x8, [x8]
	tst	x8, #0x100
	cset	w9, eq
	orr	w9, w23, w9
	tbz	w9, #0, .LBB6_8
// %bb.6:                               //   in Loop: Header=BB6_4 Depth=2
	ubfx	x8, x8, #8, #1
	orr	w23, w23, w8
	b	.LBB6_9
.LBB6_7:                                //   in Loop: Header=BB6_4 Depth=2
	mov	x0, x21
	mov	x1, x19
	bl	printk
	b	.LBB6_9
.LBB6_8:                                //   in Loop: Header=BB6_4 Depth=2
	sub	x0, x28, #1920          // =1920
	bl	ufs_order_panic_wait_datasync_handle
	mov	w23, w0
.LBB6_9:                                //   in Loop: Header=BB6_4 Depth=2
	ldr	x28, [x28]
	cmp	x28, x25
	b.ne	.LBB6_4
// %bb.10:                              //   in Loop: Header=BB6_1 Depth=1
	mov	x0, x20
	mov	x1, x22
	bl	_raw_spin_unlock_irqrestore
	mov	x22, x27
	tbnz	w23, #0, .LBB6_12
.LBB6_11:                               //   in Loop: Header=BB6_1 Depth=1
	subs	w24, w24, #1            // =1
	mov	x22, x26
	b.ne	.LBB6_1
.LBB6_12:
	mov	w0, #1
	bl	blk_power_off_flush
	adrp	x1, .L__func__.mas_blk_order_panic_wait_flush
	add	x1, x1, :lo12:.L__func__.mas_blk_order_panic_wait_flush
	mov	x0, x22
	bl	printk
	ldp	x20, x19, [sp, #80]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #96     // 16-byte Folded Reload
	ret
.Lfunc_end6:
	.size	mas_blk_order_panic_wait_flush, .Lfunc_end6-mas_blk_order_panic_wait_flush
                                        // -- End function
	.globl	mas_blk_panic_flush     // -- Begin function mas_blk_panic_flush
	.p2align	2
	.type	mas_blk_panic_flush,@function
mas_blk_panic_flush:                    // @mas_blk_panic_flush
.Lmas_blk_panic_flush$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	mov	w0, wzr
	stp	x24, x23, [sp, #16]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	bl	blk_power_off_flush
	adrp	x0, all_object_lock
	add	x0, x0, :lo12:all_object_lock
	bl	_raw_spin_lock_irqsave
	adrp	x22, all_object_list
	add	x22, x22, :lo12:all_object_list
	ldr	x23, [x22]
	mov	x19, x0
	cmp	x23, x22
	b.eq	.LBB7_10
// %bb.1:
	adrp	x20, .L.str.8
	adrp	x21, .L__func__.mas_blk_panic_flush
	mov	w24, wzr
	add	x20, x20, :lo12:.L.str.8
	add	x21, x21, :lo12:.L__func__.mas_blk_panic_flush
.LBB7_2:                                // =>This Inner Loop Header: Depth=1
	sub	x8, x23, #1916          // =1916
	ldr	w2, [x8]
	cmp	w2, #2                  // =2
	b.ne	.LBB7_5
// %bb.3:                               //   in Loop: Header=BB7_2 Depth=1
	sub	x8, x23, #1895          // =1895
	ldrb	w8, [x8]
	tbnz	w8, #0, .LBB7_6
.LBB7_4:                                //   in Loop: Header=BB7_2 Depth=1
	ldr	x23, [x23]
	cmp	x23, x22
	b.ne	.LBB7_2
	b	.LBB7_8
.LBB7_5:                                //   in Loop: Header=BB7_2 Depth=1
	mov	x0, x20
	mov	x1, x21
	bl	printk
	b	.LBB7_4
.LBB7_6:                                //   in Loop: Header=BB7_2 Depth=1
	sub	x0, x23, #1920          // =1920
	bl	ufs_order_panic_datasync_handle
	ldr	x23, [x23]
	mov	w24, #1
	cmp	x23, x22
	b.ne	.LBB7_2
// %bb.7:
	adrp	x0, all_object_lock
	add	x0, x0, :lo12:all_object_lock
	mov	x1, x19
	bl	_raw_spin_unlock_irqrestore
	b	.LBB7_9
.LBB7_8:
	adrp	x0, all_object_lock
	add	x0, x0, :lo12:all_object_lock
	mov	x1, x19
	bl	_raw_spin_unlock_irqrestore
	tbz	w24, #0, .LBB7_11
.LBB7_9:
	bl	mas_blk_order_panic_wait_flush
	b	.LBB7_11
.LBB7_10:
	adrp	x0, all_object_lock
	add	x0, x0, :lo12:all_object_lock
	mov	x1, x19
	bl	_raw_spin_unlock_irqrestore
.LBB7_11:
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.Lfunc_end7:
	.size	mas_blk_panic_flush, .Lfunc_end7-mas_blk_panic_flush
                                        // -- End function
	.globl	mas_blk_poweroff_flush_notifier_call // -- Begin function mas_blk_poweroff_flush_notifier_call
	.p2align	2
	.type	mas_blk_poweroff_flush_notifier_call,@function
mas_blk_poweroff_flush_notifier_call:   // @mas_blk_poweroff_flush_notifier_call
.Lmas_blk_poweroff_flush_notifier_call$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x1, .LBB8_3
// %bb.1:
	cmp	x1, #1                  // =1
	b.ne	.LBB8_4
// %bb.2:
	adrp	x19, system_wq
	ldr	x1, [x19, :lo12:system_wq]
	adrp	x2, mas_blk_order_power_off_work
	add	x2, x2, :lo12:mas_blk_order_power_off_work
	mov	w0, #8
	mov	w3, #250
	bl	queue_delayed_work_on
	ldr	x1, [x19, :lo12:system_wq]
	adrp	x2, mas_blk_power_off_work
	add	x2, x2, :lo12:mas_blk_power_off_work
	mov	w0, #8
	mov	w3, #1500
	bl	queue_delayed_work_on
	b	.LBB8_4
.LBB8_3:
	adrp	x0, mas_blk_power_off_work
	add	x0, x0, :lo12:mas_blk_power_off_work
	bl	cancel_delayed_work
	adrp	x0, mas_blk_order_power_off_work
	add	x0, x0, :lo12:mas_blk_order_power_off_work
	bl	cancel_delayed_work
.LBB8_4:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	mov	w0, wzr
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end8:
	.size	mas_blk_poweroff_flush_notifier_call, .Lfunc_end8-mas_blk_poweroff_flush_notifier_call
                                        // -- End function
	.section	.init.text,"ax",@progbits
	.globl	mas_blk_flush_init      // -- Begin function mas_blk_flush_init
	.p2align	2
	.type	mas_blk_flush_init,@function
mas_blk_flush_init:                     // @mas_blk_flush_init
.Lmas_blk_flush_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	adrp	x8, mas_blk_power_off_work
	adrp	x19, delayed_work_timer_fn
	add	x8, x8, :lo12:mas_blk_power_off_work
	adrp	x9, mas_blk_power_off_work_fn
	add	x19, x19, :lo12:delayed_work_timer_fn
	mov	x20, #68719476704
	add	x9, x9, :lo12:mas_blk_power_off_work_fn
	add	x10, x8, #8             // =8
	add	x0, x8, #48             // =48
	mov	w2, #2097152
	mov	x1, x19
	mov	x3, xzr
	mov	x4, xzr
	mov	x29, sp
	str	x20, [x8]
	str	x10, [x8, #8]
	stp	x10, x9, [x8, #16]
	bl	init_timer_key
	adrp	x8, mas_blk_order_power_off_work
	add	x8, x8, :lo12:mas_blk_order_power_off_work
	adrp	x9, mas_blk_order_power_off_work_fn
	add	x9, x9, :lo12:mas_blk_order_power_off_work_fn
	add	x10, x8, #8             // =8
	add	x0, x8, #48             // =48
	mov	w2, #2097152
	mov	x1, x19
	mov	x3, xzr
	mov	x4, xzr
	str	x20, [x8]
	str	x10, [x8, #8]
	stp	x10, x9, [x8, #16]
	bl	init_timer_key
	adrp	x0, mas_blk_poweroff_flush_nb
	add	x0, x0, :lo12:mas_blk_poweroff_flush_nb
	bl	power_key_register_notifier
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end9:
	.size	mas_blk_flush_init, .Lfunc_end9-mas_blk_flush_init
                                        // -- End function
	.text
	.p2align	2               // -- Begin function mas_blk_power_off_work_fn
	.type	mas_blk_power_off_work_fn,@function
mas_blk_power_off_work_fn:              // @mas_blk_power_off_work_fn
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	w0, #1
	mov	x29, sp
	bl	blk_power_off_flush
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end10:
	.size	mas_blk_power_off_work_fn, .Lfunc_end10-mas_blk_power_off_work_fn
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_order_power_off_work_fn
	.type	mas_blk_order_power_off_work_fn,@function
mas_blk_order_power_off_work_fn:        // @mas_blk_order_power_off_work_fn
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	adrp	x0, all_object_lock
	add	x0, x0, :lo12:all_object_lock
	str	x23, [sp, #16]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	bl	_raw_spin_lock_irqsave
	adrp	x22, all_object_list
	add	x22, x22, :lo12:all_object_list
	ldr	x23, [x22]
	mov	x19, x0
	cmp	x23, x22
	b.eq	.LBB11_10
// %bb.1:
	adrp	x20, .L.str.8
	adrp	x21, .L__func__.mas_blk_order_poweroff_proc
	add	x20, x20, :lo12:.L.str.8
	add	x21, x21, :lo12:.L__func__.mas_blk_order_poweroff_proc
	b	.LBB11_4
.LBB11_2:                               //   in Loop: Header=BB11_4 Depth=1
	mov	x0, x20
	mov	x1, x21
	bl	printk
.LBB11_3:                               //   in Loop: Header=BB11_4 Depth=1
	ldr	x23, [x23]
	cmp	x23, x22
	b.eq	.LBB11_10
.LBB11_4:                               // =>This Inner Loop Header: Depth=1
	sub	x8, x23, #1916          // =1916
	ldr	w2, [x8]
	cmp	w2, #2                  // =2
	b.ne	.LBB11_2
// %bb.5:                               //   in Loop: Header=BB11_4 Depth=1
	sub	x8, x23, #1895          // =1895
	ldrb	w8, [x8]
	tbz	w8, #0, .LBB11_3
// %bb.6:                               //   in Loop: Header=BB11_4 Depth=1
	sub	x8, x23, #1912          // =1912
	ldr	x8, [x8]
	cbz	x8, .LBB11_3
// %bb.7:                               //   in Loop: Header=BB11_4 Depth=1
	ldr	x8, [x8, #2024]
	cbz	x8, .LBB11_3
// %bb.8:                               //   in Loop: Header=BB11_4 Depth=1
	ldr	x8, [x8, #32]
	cbz	x8, .LBB11_3
// %bb.9:                               //   in Loop: Header=BB11_4 Depth=1
	sub	x0, x23, #1920          // =1920
	blr	x8
	b	.LBB11_3
.LBB11_10:
	adrp	x0, all_object_lock
	add	x0, x0, :lo12:all_object_lock
	mov	x1, x19
	bl	_raw_spin_unlock_irqrestore
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.Lfunc_end11:
	.size	mas_blk_order_power_off_work_fn, .Lfunc_end11-mas_blk_order_power_off_work_fn
                                        // -- End function
	.globl	mas_blk_flush_reduced_queue_unregister // -- Begin function mas_blk_flush_reduced_queue_unregister
	.p2align	2
	.type	mas_blk_flush_reduced_queue_unregister,@function
mas_blk_flush_reduced_queue_unregister: // @mas_blk_flush_reduced_queue_unregister
.Lmas_blk_flush_reduced_queue_unregister$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	w8, [x0, #1272]
	mov	x29, sp
	cbz	w8, .LBB12_4
// %bb.1:
	ldr	x8, [x0, #1256]
	mov	x19, x0
	cbz	x8, .LBB12_5
// %bb.2:
	ldr	x8, [x19, #1264]
	cbz	x8, .LBB12_5
// %bb.3:
	adrp	x20, all_flush_q_lock
	add	x20, x20, :lo12:all_flush_q_lock
	mov	x0, x20
	bl	mutex_lock
	ldr	x8, [x19, #1264]
	ldr	x9, [x19, #1256]
	mov	x10, #256
	movk	x10, #57005, lsl #48
	mov	w11, #34
	orr	x11, x10, x11
	mov	x0, x20
	str	x8, [x9, #8]
	str	x9, [x8]
	str	x10, [x19, #1256]
	str	x11, [x19, #1264]
	bl	mutex_unlock
.LBB12_4:
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB12_5:
	bl	mas_blk_rdr_panic
.Lfunc_end12:
	.size	mas_blk_flush_reduced_queue_unregister, .Lfunc_end12-mas_blk_flush_reduced_queue_unregister
                                        // -- End function
	.p2align	2               // -- Begin function mas_blk_rdr_panic
	.type	mas_blk_rdr_panic,@function
mas_blk_rdr_panic:                      // @mas_blk_rdr_panic
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	adrp	x0, .L.str.16
	adrp	x1, .L.str.11
	add	x0, x0, :lo12:.L.str.16
	add	x1, x1, :lo12:.L.str.11
	mov	x29, sp
	bl	printk
	//APP
	.pushsection __bug_table,"aw"; .align 2; 14470: .long 14471f - 14470b; .pushsection .rodata.str,"aMS",@progbits,1; 14472: .string "block/mas_blk.h"; .popsection; .long 14472b - 14470b; .short 449; .short 0; .popsection; 14471: brk 0x800
	//NO_APP
.Lfunc_end13:
	.size	mas_blk_rdr_panic, .Lfunc_end13-mas_blk_rdr_panic
                                        // -- End function
	.globl	blk_mq_tagset_direct_flush_register // -- Begin function blk_mq_tagset_direct_flush_register
	.p2align	2
	.type	blk_mq_tagset_direct_flush_register,@function
blk_mq_tagset_direct_flush_register:    // @blk_mq_tagset_direct_flush_register
.Lblk_mq_tagset_direct_flush_register$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB14_2
// %bb.1:
	str	x1, [x0, #120]
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB14_2:
	adrp	x0, .L.str.12
	adrp	x1, .L__func__.blk_mq_tagset_direct_flush_register
	add	x0, x0, :lo12:.L.str.12
	add	x1, x1, :lo12:.L__func__.blk_mq_tagset_direct_flush_register
	bl	printk
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end14:
	.size	blk_mq_tagset_direct_flush_register, .Lfunc_end14-blk_mq_tagset_direct_flush_register
                                        // -- End function
	.globl	mas_blk_queue_async_flush_init // -- Begin function mas_blk_queue_async_flush_init
	.p2align	2
	.type	mas_blk_queue_async_flush_init,@function
mas_blk_queue_async_flush_init:         // @mas_blk_queue_async_flush_init
.Lmas_blk_queue_async_flush_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	str	x21, [sp, #16]          // 8-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	mas_blk_get_lld
	ldrb	w8, [x0, #24]
	tbz	w8, #2, .LBB15_10
// %bb.1:
	adrp	x21, blk_async_flush_workqueue
	ldr	x8, [x21, :lo12:blk_async_flush_workqueue]
	mov	x20, x0
	cbnz	x8, .LBB15_3
// %bb.2:
	adrp	x0, .L.str.13
	add	x0, x0, :lo12:.L.str.13
	mov	w1, #2
	mov	w2, wzr
	bl	alloc_workqueue
	str	x0, [x21, :lo12:blk_async_flush_workqueue]
	cbz	x0, .LBB15_11
.LBB15_3:
	str	wzr, [x19, #1244]
	ldr	x8, [x19, #1256]
	add	x21, x19, #1256         // =1256
	cbnz	x8, .LBB15_6
// %bb.4:
	ldr	x8, [x19, #1264]
	cbnz	x8, .LBB15_6
// %bb.5:
	str	x21, [x19, #1256]
	str	x21, [x19, #1264]
.LBB15_6:
	adrp	x10, __cfi_mas_blk_flush_work_fn
	adrp	x1, delayed_work_timer_fn
	mov	x8, #68719476704
	add	x9, x19, #1112          // =1112
	add	x10, x10, :lo12:__cfi_mas_blk_flush_work_fn
	add	x0, x19, #1152          // =1152
	add	x1, x1, :lo12:delayed_work_timer_fn
	mov	w2, #2097152
	mov	x3, xzr
	mov	x4, xzr
	str	x8, [x19, #1104]
	str	x9, [x19, #1112]
	str	x9, [x19, #1120]
	str	x10, [x19, #1128]
	bl	init_timer_key
	ldr	x8, [x20, #56]
	cbz	x8, .LBB15_9
// %bb.7:
	ldr	x8, [x21]
	cmp	x21, x8
	b.ne	.LBB15_9
// %bb.8:
	adrp	x20, all_flush_q_lock
	add	x20, x20, :lo12:all_flush_q_lock
	mov	x0, x20
	bl	mutex_lock
	adrp	x8, all_flush_q_list
	add	x8, x8, :lo12:all_flush_q_list
	ldr	x9, [x8]
	adrp	x10, all_flush_q_list
	mov	x0, x20
	str	x21, [x9, #8]
	str	x9, [x19, #1256]
	str	x8, [x19, #1264]
	str	x21, [x10, :lo12:all_flush_q_list]
	bl	mutex_unlock
.LBB15_9:
	mov	w8, #1
	str	w8, [x19, #1272]
.LBB15_10:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.LBB15_11:
	adrp	x0, .L.str.14
	adrp	x1, .L__func__.mas_blk_queue_async_flush_init
	add	x0, x0, :lo12:.L.str.14
	add	x1, x1, :lo12:.L__func__.mas_blk_queue_async_flush_init
	bl	printk
	b	.LBB15_10
.Lfunc_end15:
	.size	mas_blk_queue_async_flush_init, .Lfunc_end15-mas_blk_queue_async_flush_init
                                        // -- End function
	.globl	blk_flush_set_async     // -- Begin function blk_flush_set_async
	.p2align	2
	.type	blk_flush_set_async,@function
blk_flush_set_async:                    // @blk_flush_set_async
.Lblk_flush_set_async$local:
// %bb.0:
	cbz	x0, .LBB16_2
// %bb.1:
	mov	w8, #1
	strb	w8, [x0, #146]
.LBB16_2:
	ret
.Lfunc_end16:
	.size	blk_flush_set_async, .Lfunc_end16-blk_flush_set_async
                                        // -- End function
	.globl	blk_mq_tagset_flush_reduce_config // -- Begin function blk_mq_tagset_flush_reduce_config
	.p2align	2
	.type	blk_mq_tagset_flush_reduce_config,@function
blk_mq_tagset_flush_reduce_config:      // @blk_mq_tagset_flush_reduce_config
.Lblk_mq_tagset_flush_reduce_config$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB17_5
// %bb.1:
	ldr	x8, [x0, #88]
	tbz	w1, #0, .LBB17_3
// %bb.2:
	orr	x8, x8, #0x4
	b	.LBB17_4
.LBB17_3:
	and	x8, x8, #0xfffffffffffffffb
.LBB17_4:
	str	x8, [x0, #88]
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB17_5:
	adrp	x0, .L.str.12
	adrp	x1, .L__func__.blk_mq_tagset_flush_reduce_config
	add	x0, x0, :lo12:.L.str.12
	add	x1, x1, :lo12:.L__func__.blk_mq_tagset_flush_reduce_config
	bl	printk
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end17:
	.size	blk_mq_tagset_flush_reduce_config, .Lfunc_end17-blk_mq_tagset_flush_reduce_config
                                        // -- End function
	.globl	blk_flush_async_support // -- Begin function blk_flush_async_support
	.p2align	2
	.type	blk_flush_async_support,@function
blk_flush_async_support:                // @blk_flush_async_support
.Lblk_flush_async_support$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB18_3
// %bb.1:
	ldr	x8, [x0, #136]
	ldr	x8, [x8, #1272]
	cbz	x8, .LBB18_4
// %bb.2:
	ldr	w0, [x8, #1272]
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB18_3:
	adrp	x0, .L.str.15
	adrp	x1, .L__func__.blk_flush_async_support
	add	x0, x0, :lo12:.L.str.15
	add	x1, x1, :lo12:.L__func__.blk_flush_async_support
	bl	printk
.LBB18_4:
	mov	w0, wzr
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end18:
	.size	blk_flush_async_support, .Lfunc_end18-blk_flush_async_support
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\0013[BLK-IO]%s: blkdev_get fail!\n"
	.size	.L.str, 32

	.type	.L__func__.mas_blk_flush_work_fn,@object // @__func__.mas_blk_flush_work_fn
.L__func__.mas_blk_flush_work_fn:
	.asciz	"mas_blk_flush_work_fn"
	.size	.L__func__.mas_blk_flush_work_fn, 22

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"\0013[BLK-IO]%s: blkdev_issue_flush fail!\n"
	.size	.L.str.1, 40

	.type	blk_async_flush_workqueue,@object // @blk_async_flush_workqueue
	.local	blk_async_flush_workqueue
	.comm	blk_async_flush_workqueue,8,8
	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"\0013[BLK-IO]%s: emergency = %d\n"
	.size	.L.str.2, 30

	.type	.L__func__.blk_power_off_flush,@object // @__func__.blk_power_off_flush
.L__func__.blk_power_off_flush:
	.asciz	"blk_power_off_flush"
	.size	.L__func__.blk_power_off_flush, 20

	.type	all_flush_q_lock,@object // @all_flush_q_lock
	.data
	.p2align	3
all_flush_q_lock:
	.zero	8
	.zero	4
	.zero	4
	.xword	all_flush_q_lock+16
	.xword	all_flush_q_lock+16
	.xword	0
	.size	all_flush_q_lock, 40

	.type	all_flush_q_list,@object // @all_flush_q_list
	.p2align	3
all_flush_q_list:
	.xword	all_flush_q_list
	.xword	all_flush_q_list
	.size	all_flush_q_list, 16

	.type	.L.str.3,@object        // @.str.3
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.3:
	.asciz	"\0013[BLK-IO]%s: emergency flush on %s\n"
	.size	.L.str.3, 37

	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"\0013[BLK-IO]%s, bdev is NULL!\n"
	.size	.L.str.4, 29

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"\0013[BLK-IO]%s: emergency flush on %s done!\n"
	.size	.L.str.5, 43

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"\0013[BLK-IO]%s: done!\n"
	.size	.L.str.6, 21

	.type	all_object_lock,@object // @all_object_lock
	.local	all_object_lock
	.comm	all_object_lock,4,4
	.type	all_object_list,@object // @all_object_list
	.data
	.p2align	3
all_object_list:
	.xword	all_object_list
	.xword	all_object_list
	.size	all_object_list, 16

	.type	.L.str.7,@object        // @.str.7
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.7:
	.asciz	"\0013[BLK-IO]<%s> start\n"
	.size	.L.str.7, 22

	.type	.L__func__.mas_blk_order_panic_wait_flush,@object // @__func__.mas_blk_order_panic_wait_flush
.L__func__.mas_blk_order_panic_wait_flush:
	.asciz	"mas_blk_order_panic_wait_flush"
	.size	.L__func__.mas_blk_order_panic_wait_flush, 31

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"\0013[BLK-IO]<%s> type = %d\n"
	.size	.L.str.8, 26

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"\0013[BLK-IO]<%s> normal end\n"
	.size	.L.str.9, 27

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"\0013[BLK-IO]<%s> timeout end\n"
	.size	.L.str.10, 28

	.type	.L__func__.mas_blk_panic_flush,@object // @__func__.mas_blk_panic_flush
.L__func__.mas_blk_panic_flush:
	.asciz	"mas_blk_panic_flush"
	.size	.L__func__.mas_blk_panic_flush, 20

	.type	mas_blk_order_power_off_work,@object // @mas_blk_order_power_off_work
	.local	mas_blk_order_power_off_work
	.comm	mas_blk_order_power_off_work,136,8
	.type	mas_blk_power_off_work,@object // @mas_blk_power_off_work
	.local	mas_blk_power_off_work
	.comm	mas_blk_power_off_work,136,8
	.type	mas_blk_poweroff_flush_nb,@object // @mas_blk_poweroff_flush_nb
	.data
	.p2align	3
mas_blk_poweroff_flush_nb:
	.xword	__cfi_mas_blk_poweroff_flush_notifier_call
	.xword	0
	.word	0                       // 0x0
	.zero	4
	.size	mas_blk_poweroff_flush_nb, 24

	.type	__addressable___cfi_mas_blk_flush_init328,@object // @__addressable___cfi_mas_blk_flush_init328
	.section	.discard.addressable,"aw",@progbits
	.p2align	3
__addressable___cfi_mas_blk_flush_init328:
	.xword	__cfi_mas_blk_flush_init
	.size	__addressable___cfi_mas_blk_flush_init328, 8

	.type	.L.str.11,@object       // @.str.11
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.11:
	.asciz	"Invalid flush_queue_node!"
	.size	.L.str.11, 26

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"\0013[BLK-IO]%s: tag_set is NULL!\n"
	.size	.L.str.12, 32

	.type	.L__func__.blk_mq_tagset_direct_flush_register,@object // @__func__.blk_mq_tagset_direct_flush_register
.L__func__.blk_mq_tagset_direct_flush_register:
	.asciz	"blk_mq_tagset_direct_flush_register"
	.size	.L__func__.blk_mq_tagset_direct_flush_register, 36

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"async_flush"
	.size	.L.str.13, 12

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"\0013[BLK-IO]%s: Failed to alloc workqueue!\n"
	.size	.L.str.14, 42

	.type	.L__func__.mas_blk_queue_async_flush_init,@object // @__func__.mas_blk_queue_async_flush_init
.L__func__.mas_blk_queue_async_flush_init:
	.asciz	"mas_blk_queue_async_flush_init"
	.size	.L__func__.mas_blk_queue_async_flush_init, 31

	.type	.L__func__.blk_mq_tagset_flush_reduce_config,@object // @__func__.blk_mq_tagset_flush_reduce_config
.L__func__.blk_mq_tagset_flush_reduce_config:
	.asciz	"blk_mq_tagset_flush_reduce_config"
	.size	.L__func__.blk_mq_tagset_flush_reduce_config, 34

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"\0013[BLK-IO]%s: bi_bdev is NULL!\n"
	.size	.L.str.15, 32

	.type	.L__func__.blk_flush_async_support,@object // @__func__.blk_flush_async_support
.L__func__.blk_flush_async_support:
	.asciz	"blk_flush_async_support"
	.size	.L__func__.blk_flush_async_support, 24

	.type	.L__func__.mas_blk_order_poweroff_proc,@object // @__func__.mas_blk_order_poweroff_proc
.L__func__.mas_blk_order_poweroff_proc:
	.asciz	"mas_blk_order_poweroff_proc"
	.size	.L__func__.mas_blk_order_poweroff_proc, 28

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"\0013[BLK-IO]%s\n"
	.size	.L.str.16, 14

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
