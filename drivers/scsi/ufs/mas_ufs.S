	.text
	.file	"mas_ufs.c"
	.globl	mas_ufshcd_prepare_req_desc_hdr // -- Begin function mas_ufshcd_prepare_req_desc_hdr
	.p2align	2
	.type	mas_ufshcd_prepare_req_desc_hdr,@function
mas_ufshcd_prepare_req_desc_hdr:        // @mas_ufshcd_prepare_req_desc_hdr
.Lmas_ufshcd_prepare_req_desc_hdr$local:
// %bb.0:
	ldr	x8, [x0, #64]
	cbz	x8, .LBB0_5
// %bb.1:
	ldr	x9, [x8, #336]
	cbz	x9, .LBB0_3
// %bb.2:
	ldrb	w9, [x9, #240]
	tbnz	w9, #0, .LBB0_6
.LBB0_3:
	ldr	x8, [x8, #336]
	cbz	x8, .LBB0_5
// %bb.4:
	ldrb	w8, [x8, #240]
	tbnz	w8, #1, .LBB0_7
.LBB0_5:
	ret
.LBB0_6:
	ldr	w8, [x1]
	orr	w8, w8, #0x2
	str	w8, [x1]
	ldr	x8, [x0, #64]
	cbnz	x8, .LBB0_3
	b	.LBB0_5
.LBB0_7:
	ldr	w8, [x1]
	orr	w8, w8, #0x4
	str	w8, [x1]
	ret
.Lfunc_end0:
	.size	mas_ufshcd_prepare_req_desc_hdr, .Lfunc_end0-mas_ufshcd_prepare_req_desc_hdr
                                        // -- End function
	.globl	mas_ufshcd_slave_config // -- Begin function mas_ufshcd_slave_config
	.p2align	2
	.type	mas_ufshcd_slave_config,@function
mas_ufshcd_slave_config:                // @mas_ufshcd_slave_config
.Lmas_ufshcd_slave_config$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	ldr	x8, [x1]
	mov	w9, #4672
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x19, x1
	ldrb	w1, [x8, x9]
	mov	x29, sp
	bl	mas_blk_queue_unistore_enable
	mov	x0, x19
	bl	mas_ufs_manual_bkops_config
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end1:
	.size	mas_ufshcd_slave_config, .Lfunc_end1-mas_ufshcd_slave_config
                                        // -- End function
	.globl	ufshcd_send_scsi_sync_cache_init // -- Begin function ufshcd_send_scsi_sync_cache_init
	.p2align	2
	.type	ufshcd_send_scsi_sync_cache_init,@function
ufshcd_send_scsi_sync_cache_init:       // @ufshcd_send_scsi_sync_cache_init
.Lufshcd_send_scsi_sync_cache_init$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	adrp	x19, sync_cache_buffer
	ldr	x8, [x19, :lo12:sync_cache_buffer]
	mov	x29, sp
	cbnz	x8, .LBB2_2
// %bb.1:
	adrp	x8, kmalloc_caches+56
	ldr	x0, [x8, :lo12:kmalloc_caches+56]
	mov	w1, #3520
	mov	w2, #96
	bl	kmem_cache_alloc_trace
	str	x0, [x19, :lo12:sync_cache_buffer]
	cbz	x0, .LBB2_4
.LBB2_2:
	mov	w0, wzr
.LBB2_3:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.LBB2_4:
	mov	w0, #-12
	b	.LBB2_3
.Lfunc_end2:
	.size	ufshcd_send_scsi_sync_cache_init, .Lfunc_end2-ufshcd_send_scsi_sync_cache_init
                                        // -- End function
	.globl	ufshcd_send_scsi_sync_cache_deinit // -- Begin function ufshcd_send_scsi_sync_cache_deinit
	.p2align	2
	.type	ufshcd_send_scsi_sync_cache_deinit,@function
ufshcd_send_scsi_sync_cache_deinit:     // @ufshcd_send_scsi_sync_cache_deinit
.Lufshcd_send_scsi_sync_cache_deinit$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	str	x19, [sp, #16]          // 8-byte Folded Spill
	adrp	x19, sync_cache_buffer
	ldr	x0, [x19, :lo12:sync_cache_buffer]
	mov	x29, sp
	cbz	x0, .LBB3_2
// %bb.1:
	bl	kfree
	str	xzr, [x19, :lo12:sync_cache_buffer]
.LBB3_2:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end3:
	.size	ufshcd_send_scsi_sync_cache_deinit, .Lfunc_end3-ufshcd_send_scsi_sync_cache_deinit
                                        // -- End function
	.globl	__ufshcd_wait_for_doorbell_clr // -- Begin function __ufshcd_wait_for_doorbell_clr
	.p2align	2
	.type	__ufshcd_wait_for_doorbell_clr,@function
__ufshcd_wait_for_doorbell_clr:         // @__ufshcd_wait_for_doorbell_clr
.L__ufshcd_wait_for_doorbell_clr$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	str	x23, [sp, #16]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	mov	x19, x0
	bl	ktime_get
	ldr	x8, [x19]
	mov	x20, x0
	mov	w0, #1
	add	x21, x8, #120           // =120
	mov	x1, x21
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	.if 1 == 1
661:
	ldr w21, [x21]
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 1
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	ldar w21, [x21]
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbz	w0, .LBB4_2
// %bb.1:
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB4_2:
	//APP
	dmb oshld
	//NO_APP
	mov	w8, w21
	//APP
	eor	x8, x8, x8
cbnz	x8, .
	//NO_APP
	ldr	x8, [x19]
	mov	w0, #1
	add	x22, x8, #88            // =88
	mov	x1, x22
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	.if 1 == 1
661:
	ldr w4, [x22]
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 1
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	ldar w4, [x22]
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbz	w0, .LBB4_4
// %bb.3:
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB4_4:
	//APP
	dmb oshld
	//NO_APP
	mov	w8, w4
	//APP
	eor	x8, x8, x8
cbnz	x8, .
	//NO_APP
	orr	w8, w4, w21
	cbnz	w8, .LBB4_7
// %bb.5:
	mov	w20, wzr
.LBB4_6:
	mov	w0, w20
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x23, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB4_7:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.31
	adrp	x2, .L.str
	add	x1, x1, :lo12:.L.str.31
	add	x2, x2, :lo12:.L.str
	mov	w3, w21
	bl	_dev_err
	mov	w23, #38888
	movk	w23, #30517, lsl #16
.LBB4_8:                                // =>This Inner Loop Header: Depth=1
	ldr	x8, [x19]
	mov	w0, #1
	add	x21, x8, #120           // =120
	mov	x1, x21
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	.if 1 == 1
661:
	ldr w21, [x21]
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 1
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	ldar w21, [x21]
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbz	w0, .LBB4_10
// %bb.9:                               //   in Loop: Header=BB4_8 Depth=1
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB4_10:                               //   in Loop: Header=BB4_8 Depth=1
	//APP
	dmb oshld
	//NO_APP
	mov	w8, w21
	//APP
	eor	x8, x8, x8
cbnz	x8, .
	//NO_APP
	ldr	x8, [x19]
	mov	w0, #1
	add	x22, x8, #88            // =88
	mov	x1, x22
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	.if 1 == 1
661:
	ldr w22, [x22]
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 1
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	ldar w22, [x22]
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbz	w0, .LBB4_12
// %bb.11:                              //   in Loop: Header=BB4_8 Depth=1
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB4_12:                               //   in Loop: Header=BB4_8 Depth=1
	//APP
	dmb oshld
	//NO_APP
	mov	w8, w22
	//APP
	eor	x8, x8, x8
cbnz	x8, .
	//NO_APP
	bl	ktime_get
	sub	x8, x0, x20
	cmp	x8, x23
	b.ge	.LBB4_15
// %bb.13:                              //   in Loop: Header=BB4_8 Depth=1
	orr	w8, w22, w21
	cbnz	w8, .LBB4_8
// %bb.14:
	mov	w20, wzr
	b	.LBB4_16
.LBB4_15:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.1
	add	x1, x1, :lo12:.L.str.1
	bl	_dev_err
	mov	w20, #-5
.LBB4_16:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.31
	adrp	x2, .L.str.2
	add	x1, x1, :lo12:.L.str.31
	add	x2, x2, :lo12:.L.str.2
	mov	w3, w21
	mov	w4, w22
	bl	_dev_err
	b	.LBB4_6
.Lfunc_end4:
	.size	__ufshcd_wait_for_doorbell_clr, .Lfunc_end4-__ufshcd_wait_for_doorbell_clr
                                        // -- End function
	.globl	ufshcd_direct_flush_test // -- Begin function ufshcd_direct_flush_test
	.p2align	2
	.type	ufshcd_direct_flush_test,@function
ufshcd_direct_flush_test:               // @ufshcd_direct_flush_test
.Lufshcd_direct_flush_test$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	adrp	x20, .L__func__.ufshcd_direct_flush_test
	mov	x19, x0
	adrp	x0, .L.str.3
	add	x20, x20, :lo12:.L__func__.ufshcd_direct_flush_test
	add	x0, x0, :lo12:.L.str.3
	mov	w2, #283
	mov	x1, x20
	mov	x29, sp
	bl	printk
	ldr	x0, [x19, #96]
	bl	ufshcd_direct_flush
	mov	w19, w0
	adrp	x0, .L.str.4
	add	x0, x0, :lo12:.L.str.4
	mov	w2, #285
	mov	x1, x20
	bl	printk
	mov	w0, w19
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end5:
	.size	ufshcd_direct_flush_test, .Lfunc_end5-ufshcd_direct_flush_test
                                        // -- End function
	.globl	ufshcd_direct_flush     // -- Begin function ufshcd_direct_flush
	.p2align	2
	.type	ufshcd_direct_flush,@function
ufshcd_direct_flush:                    // @ufshcd_direct_flush
.Lufshcd_direct_flush$local:
// %bb.0:
	stp	x29, x30, [sp, #-96]!   // 16-byte Folded Spill
	stp	x28, x27, [sp, #16]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #32]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #48]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #64]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #80]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #576            // =576
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stur	x8, [x29, #-16]
	cbz	x0, .LBB6_17
// %bb.1:
	ldr	x23, [x0]
	mov	w8, #4688
	mov	x19, x0
	adds	x20, x23, x8
	b.eq	.LBB6_18
// %bb.2:
	mov	w8, #25728
	add	x24, x23, x8
	ldrb	w8, [x24, #496]
	cbnz	w8, .LBB6_16
// %bb.3:
	ldr	w8, [x23, #4848]
	cbnz	w8, .LBB6_16
// %bb.4:
	mov	w22, #31072
	movk	w22, #65534, lsl #16
	mov	w25, #96
.LBB6_5:                                // =>This Inner Loop Header: Depth=1
	//APP
	.if 1 == 1
661:
	mrs	x21, daif
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
	mrs_s x21, (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5))
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
	and	w8, w21, #0x00000080
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
	eor	w8, w21, #0xe0
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbnz	w8, .LBB6_7
// %bb.6:                               //   in Loop: Header=BB6_5 Depth=1
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
	msr_s (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5)), x25
	.purgem	msr_s

664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
.LBB6_7:                                //   in Loop: Header=BB6_5 Depth=1
	ldr	x8, [x23, #4744]
	ldr	x0, [x8, #56]
	bl	_raw_spin_trylock
	cbnz	w0, .LBB6_10
// %bb.8:                               //   in Loop: Header=BB6_5 Depth=1
	//APP
	.if 1 == 1
661:
	msr	daif, x21
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
	msr_s (((3) << 19) | ((0) << 16) | ((4) << 12) | ((6) << 8) | ((0) << 5)), x21
	.purgem	msr_s
dsb	sy
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	mov	w0, #42950
	bl	__const_udelay
	adds	w22, w22, #1            // =1
	b.lo	.LBB6_5
// %bb.9:
	ldr	x0, [x23, #4752]
	adrp	x1, .L.str.8
	adrp	x2, .L__func__.ufshcd_direct_flush
	add	x1, x1, :lo12:.L.str.8
	add	x2, x2, :lo12:.L__func__.ufshcd_direct_flush
	bl	_dev_err
	mov	w22, #-5
	b	.LBB6_20
.LBB6_10:
	mov	x0, x19
	bl	scsi_device_get
	mov	w22, w0
	cbnz	w0, .LBB6_13
// %bb.11:
	ldr	w8, [x19, #2456]
	cmp	w8, #7                  // =7
	b.hi	.LBB6_13
// %bb.12:
	mov	w9, #1
	lsl	w8, w9, w8
	mov	w9, #208
	tst	w8, w9
	b.ne	.LBB6_41
.LBB6_13:
	ldr	w3, [x20, #404]
	cmp	w3, #2                  // =2
	b.ne	.LBB6_22
.LBB6_14:
	ldr	x8, [x23, #4744]
	mov	x1, x21
	ldr	x0, [x8, #56]
	bl	_raw_spin_unlock_irqrestore
	cbz	w22, .LBB6_23
.LBB6_15:
	mov	x0, x19
	bl	scsi_device_put
	b	.LBB6_20
.LBB6_16:
	ldr	x0, [x23, #4752]
	adrp	x1, .L.str.7
	adrp	x2, .L__func__.ufshcd_direct_flush
	add	x1, x1, :lo12:.L.str.7
	add	x2, x2, :lo12:.L__func__.ufshcd_direct_flush
	bl	_dev_err
	mov	w22, wzr
	b	.LBB6_20
.LBB6_17:
	adrp	x0, .L.str.5
	add	x0, x0, :lo12:.L.str.5
	b	.LBB6_19
.LBB6_18:
	adrp	x0, .L.str.6
	add	x0, x0, :lo12:.L.str.6
.LBB6_19:
	adrp	x1, .L__func__.ufshcd_direct_flush
	add	x1, x1, :lo12:.L__func__.ufshcd_direct_flush
	bl	printk
	mov	w22, #-19
.LBB6_20:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-16]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB6_51
// %bb.21:
	mov	w0, w22
	add	sp, sp, #576            // =576
	ldp	x20, x19, [sp, #80]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #96     // 16-byte Folded Reload
	ret
.LBB6_22:
	ldr	x0, [x23, #4752]
	adrp	x1, .L.str.10
	adrp	x2, .L__func__.ufshcd_direct_flush
	add	x1, x1, :lo12:.L.str.10
	add	x2, x2, :lo12:.L__func__.ufshcd_direct_flush
	bl	_dev_err
	mov	w22, #4181
	b	.LBB6_14
.LBB6_23:
	add	x0, sp, #72             // =72
	mov	w2, #488
	mov	w1, wzr
	bl	memset
	mov	w8, #53
	add	x0, sp, #72             // =72
	mov	w2, #488
	mov	w1, wzr
	strh	wzr, [sp, #64]
	str	x8, [sp, #56]
	bl	memset
	adrp	x8, sync_cache_buffer
	ldr	x3, [x8, :lo12:sync_cache_buffer]
	add	x0, sp, #72             // =72
	add	x2, sp, #56             // =56
	mov	w4, #3
	mov	x1, x19
	mov	x5, xzr
	mov	w6, wzr
	mov	w7, wzr
	bl	ufshcd_compose_scsi_cmd
	str	wzr, [sp, #52]
	str	wzr, [sp, #52]
	str	wzr, [sp, #48]
	str	wzr, [sp, #48]
	ldr	x0, [x23, #4744]
	bl	scsi_block_requests
	ldr	x0, [x23, #4752]
	mov	w1, #4
	bl	__pm_runtime_resume
	ldr	x8, [x23, #4744]
	ldr	x0, [x8, #56]
	bl	_raw_spin_lock_irqsave
	ldrb	w8, [x24, #104]
	mov	x21, x0
	mov	w25, wzr
	tbz	w8, #0, .LBB6_26
// %bb.24:
	ldr	w8, [x24]
	cmp	w8, #1                  // =1
	b.ne	.LBB6_50
// %bb.25:
	ldrb	w8, [x24, #16]
	mov	w9, #1
	strb	w9, [x24, #16]
	cmp	w8, #0                  // =0
	cset	w25, ne
.LBB6_26:
	mov	x0, x20
	bl	__ufshcd_wait_for_doorbell_clr
	cbnz	w0, .LBB6_42
// %bb.27:
	ldr	x8, [x20]
	mov	w0, #1
	add	x22, x8, #36            // =36
	mov	x1, x22
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	.if 1 == 1
661:
	ldr w8, [x22]
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 1
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	ldar w8, [x22]
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbz	w0, .LBB6_29
// %bb.28:
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB6_29:
	//APP
	dmb oshld
	//NO_APP
	mov	w9, w8
	//APP
	eor	x9, x9, x9
cbnz	x9, .
	//NO_APP
	str	w8, [sp, #52]
	ldr	w8, [sp, #52]
	cbz	w8, .LBB6_32
// %bb.30:
	//APP
	dmb oshst
	//NO_APP
	ldr	x8, [x20]
	mov	w0, #2
	add	x22, x8, #36            // =36
	mov	x1, x22
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	mov	w8, wzr
	//APP
	nop
	//NO_APP
	//APP
	str w8, [x22]
	//NO_APP
	cbz	w0, .LBB6_32
// %bb.31:
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB6_32:
	ldr	x8, [x23, #4864]
	ldr	w9, [x23, #4892]
	mvn	x10, x8
	rbit	x10, x10
	clz	x26, x10
	cmp	w9, w26
	b.le	.LBB6_45
// %bb.33:
	cmn	x8, #1                  // =1
	mov	w9, #4864
	cset	w8, eq
	add	x22, x23, x9
	lsl	x28, x8, #3
	ldr	x8, [x22, x28]
	mov	w9, #1
	lsl	x27, x9, x26
	mov	w9, #144
	orr	x8, x8, x27
	str	x8, [x22, x28]
	ldr	x8, [x23, #4856]
	add	x10, sp, #72            // =72
	mov	x0, x20
	madd	x1, x26, x9, x8
	mov	w8, #96
	mov	x9, x1
	str	x10, [x9, #64]!
	str	w8, [x1, #80]
	ldr	x8, [sp, #416]
	str	w26, [x1, #92]
	str	x9, [sp, #40]           // 8-byte Folded Spill
	mov	w9, #49408
	str	x8, [x1, #72]
	ldr	x8, [sp, #128]
	ldr	w8, [x8, #136]
	strb	wzr, [x1, #97]
	str	wzr, [x1, #88]
	and	w10, w8, #0xff00
	and	w11, w8, #0x7f
	orr	w8, w8, #0x80
	cmp	w10, w9
	csel	w8, w8, w11, eq
	strb	w8, [x1, #96]
	bl	ufshcd_compose_upiu
	cbnz	w0, .LBB6_45
// %bb.34:
	str	x22, [sp, #24]          // 8-byte Folded Spill
	mov	w8, #4880
	//APP
	dsb st
	//NO_APP
	add	x9, x23, x8
	ldr	x8, [x9, x28]
	stp	x9, x27, [sp, #8]       // 16-byte Folded Spill
	mov	w0, #2
	orr	x8, x8, x27
	str	x8, [x9, x28]
	//APP
	dmb oshst
	//NO_APP
	ldr	x8, [x23, #4688]
	add	x22, x8, #88            // =88
	mov	x1, x22
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	mov	w8, #1
	lsl	w8, w8, w26
	//APP
	nop
	//NO_APP
	str	w8, [sp, #36]           // 4-byte Folded Spill
	//APP
	str w8, [x22]
	//NO_APP
	cbz	w0, .LBB6_36
// %bb.35:
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB6_36:
	//APP
	dsb st
	//NO_APP
	mov	x27, #-19264
	movk	x27, #65459, lsl #16
.LBB6_37:                               // =>This Inner Loop Header: Depth=1
	ldr	x8, [x20]
	mov	w0, #1
	add	x22, x8, #88            // =88
	mov	x1, x22
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	.if 1 == 1
661:
	ldr w8, [x22]
662:
.pushsection .altinstructions,"a"
 .word 661b - .
 .word 663f - .
 .hword 1
 .byte 662b-661b
 .byte 664f-663f
.popsection
.pushsection .altinstr_replacement, "a"
663:
	ldar w8, [x22]
664:
	.popsection
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endif

	//NO_APP
	cbz	w0, .LBB6_39
// %bb.38:                              //   in Loop: Header=BB6_37 Depth=1
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB6_39:                               //   in Loop: Header=BB6_37 Depth=1
	//APP
	dmb oshld
	//NO_APP
	mov	w9, w8
	//APP
	eor	x9, x9, x9
cbnz	x9, .
	//NO_APP
	str	w8, [sp, #48]
	ldr	w8, [sp, #48]
	ldr	w9, [sp, #36]           // 4-byte Folded Reload
	tst	w8, w9
	b.eq	.LBB6_43
// %bb.40:                              //   in Loop: Header=BB6_37 Depth=1
	mov	w0, #18142
	movk	w0, #3, lsl #16
	bl	__const_udelay
	adds	x27, x27, #50           // =50
	b.ne	.LBB6_37
	b	.LBB6_44
.LBB6_41:
	ldr	x0, [x23, #4752]
	adrp	x1, .L.str.9
	adrp	x2, .L__func__.ufshcd_direct_flush
	add	x1, x1, :lo12:.L.str.9
	add	x2, x2, :lo12:.L__func__.ufshcd_direct_flush
	mov	w3, wzr
	bl	_dev_err
	mov	w22, #-19
	b	.LBB6_13
.LBB6_42:
	ldr	x0, [x23, #4752]
	adrp	x1, .L.str.33
	add	x1, x1, :lo12:.L.str.33
	bl	_dev_err
	b	.LBB6_48
.LBB6_43:
	ldr	x10, [sp, #8]           // 8-byte Folded Reload
	mov	w9, #1
	lsl	x9, x9, x26
	ldr	x8, [x10]
	eor	x8, x8, x9
	str	x8, [x10]
.LBB6_44:
	ldr	x8, [sp, #40]           // 8-byte Folded Reload
	str	xzr, [x8]
	ldp	x10, x9, [sp, #16]      // 16-byte Folded Reload
	ldr	x8, [x9, x28]
	bic	x8, x8, x10
	str	x8, [x9, x28]
.LBB6_45:
	ldr	w8, [sp, #52]
	cbz	w8, .LBB6_48
// %bb.46:
	//APP
	dmb oshst
	//NO_APP
	ldr	x8, [x20]
	mov	w0, #2
	add	x20, x8, #36            // =36
	mov	x1, x20
	bl	uncached_logk
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	//APP
	b .+4

	//NO_APP
	//APP
	nop
	//NO_APP
	ldr	w8, [sp, #52]
	//APP
	str w8, [x20]
	//NO_APP
	cbz	w0, .LBB6_48
// %bb.47:
	//APP
	dsb sy
	//NO_APP
	//APP
	isb
	//NO_APP
.LBB6_48:
	ldrb	w8, [x24, #104]
	tbz	w8, #0, .LBB6_50
// %bb.49:
	strb	w25, [x24, #16]
.LBB6_50:
	ldr	x8, [x23, #4744]
	mov	x1, x21
	ldr	x0, [x8, #56]
	bl	_raw_spin_unlock_irqrestore
	ldr	x0, [x23, #4752]
	mov	w1, #13
	bl	__pm_runtime_suspend
	ldr	x0, [x23, #4744]
	bl	scsi_unblock_requests
	ldr	x0, [x23, #4752]
	ldr	x3, [x19, #136]
	adrp	x1, .L.str.32
	adrp	x2, .L__func__.ufshcd_send_scsi_sync_cache_on_specified_disk
	add	x1, x1, :lo12:.L.str.32
	add	x2, x2, :lo12:.L__func__.ufshcd_send_scsi_sync_cache_on_specified_disk
	mov	w4, wzr
	bl	_dev_err
	mov	w22, wzr
	b	.LBB6_15
.LBB6_51:
	bl	__stack_chk_fail
.Lfunc_end6:
	.size	ufshcd_direct_flush, .Lfunc_end6-ufshcd_direct_flush
                                        // -- End function
	.globl	ufshcd_dump_status      // -- Begin function ufshcd_dump_status
	.p2align	2
	.type	ufshcd_dump_status,@function
ufshcd_dump_status:                     // @ufshcd_dump_status
.Lufshcd_dump_status$local:
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	mov	x19, x0
	ldr	x0, [x0, #4752]
	ldr	x2, [x19, #4864]
	mov	w20, w1
	adrp	x1, .L.str.11
	add	x1, x1, :lo12:.L.str.11
	str	x25, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	bl	_dev_err
	cmp	w20, #1                 // =1
	b.eq	.LBB7_2
.LBB7_1:
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldr	x25, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB7_2:
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2440]
	adrp	x20, .L.str.12
	add	x20, x20, :lo12:.L.str.12
	mov	x1, x20
	mov	w2, wzr
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2448]
	mov	w2, #1
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2456]
	mov	w2, #2
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2464]
	mov	w2, #3
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2472]
	mov	w2, #4
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2480]
	mov	w2, #5
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2488]
	mov	w2, #6
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2496]
	mov	w2, #7
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2504]
	mov	w2, #8
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2512]
	mov	w2, #9
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2520]
	adrp	x20, .L.str.13
	add	x20, x20, :lo12:.L.str.13
	mov	x1, x20
	mov	w2, wzr
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2528]
	mov	w2, #1
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2536]
	mov	w2, #2
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2544]
	mov	w2, #3
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2552]
	mov	w2, #4
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2560]
	mov	w2, #5
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2568]
	mov	w2, #6
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2576]
	mov	w2, #7
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2584]
	mov	w2, #8
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2592]
	mov	w2, #9
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2608]
	adrp	x20, .L.str.14
	add	x20, x20, :lo12:.L.str.14
	mov	x1, x20
	mov	w2, wzr
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2616]
	mov	w2, #1
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2624]
	mov	w2, #2
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2632]
	mov	w2, #3
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2640]
	mov	w2, #4
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	x3, [x19, #2648]
	mov	w2, #5
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	w3, [x19, #2656]
	adrp	x20, .L.str.15
	add	x20, x20, :lo12:.L.str.15
	mov	x1, x20
	mov	w2, wzr
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	w3, [x19, #2660]
	mov	w2, #1
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	w3, [x19, #2664]
	mov	w2, #2
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	w3, [x19, #2668]
	mov	w2, #3
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	w3, [x19, #2672]
	mov	w2, #4
	mov	x1, x20
	bl	_dev_err
	ldr	x0, [x19, #4752]
	ldr	w3, [x19, #2676]
	mov	w2, #5
	mov	x1, x20
	bl	_dev_err
	ldrsw	x1, [x19, #4892]
	mov	w8, #4880
	add	x20, x19, x8
	mov	x0, x20
	mov	x2, xzr
	bl	find_next_bit
	ldr	w8, [x19, #4892]
	mov	x22, x0
	cmp	w8, w22
	b.le	.LBB7_1
// %bb.3:
	mov	x24, #63439
	movk	x24, #58195, lsl #16
	movk	x24, #39845, lsl #32
	adrp	x21, .L.str.16
	mov	w23, #144
	movk	x24, #8388, lsl #48
	add	x21, x21, :lo12:.L.str.16
	mov	x25, #4294967296
.LBB7_4:                                // =>This Inner Loop Header: Depth=1
	ldr	x8, [x19, #4856]
	ldr	x0, [x19, #4752]
	mov	x1, x21
	mov	w2, w22
	smaddl	x8, w22, w23, x8
	ldp	x9, x8, [x8, #104]
	smulh	x9, x9, x24
	smulh	x8, x8, x24
	asr	x10, x9, #7
	asr	x11, x8, #7
	add	x3, x10, x9, lsr #63
	add	x4, x11, x8, lsr #63
	bl	_dev_err
	ldrsw	x1, [x19, #4892]
	add	x8, x25, x22, lsl #32
	asr	x2, x8, #32
	mov	x0, x20
	bl	find_next_bit
	ldr	w8, [x19, #4892]
	mov	x22, x0
	cmp	w8, w22
	b.le	.LBB7_1
	b	.LBB7_4
.Lfunc_end7:
	.size	ufshcd_dump_status, .Lfunc_end7-ufshcd_dump_status
                                        // -- End function
	.globl	ufshcd_send_scsi_request_sense // -- Begin function ufshcd_send_scsi_request_sense
	.p2align	2
	.type	ufshcd_send_scsi_request_sense,@function
ufshcd_send_scsi_request_sense:         // @ufshcd_send_scsi_request_sense
.Lufshcd_send_scsi_request_sense$local:
// %bb.0:
	stp	x29, x30, [sp, #-80]!   // 16-byte Folded Spill
	str	x28, [sp, #16]          // 8-byte Folded Spill
	stp	x24, x23, [sp, #32]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #544            // =544
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	w20, w2
	mov	x23, x1
	mov	x19, x0
	add	x0, sp, #48             // =48
	mov	w2, #488
	mov	w1, wzr
	mov	w22, w3
	stur	x8, [x29, #-8]
	stp	xzr, xzr, [sp, #24]
	stp	xzr, xzr, [sp, #8]
	bl	memset
	adrp	x9, kmalloc_caches+56
	ldr	x0, [x9, :lo12:kmalloc_caches+56]
	mov	w8, #96
	strh	w8, [sp, #44]
	mov	w8, #3
	mov	w1, #3520
	mov	w2, #96
	str	w8, [sp, #40]
	bl	kmem_cache_alloc_trace
	cbz	x0, .LBB8_4
// %bb.1:
	adrp	x8, kmalloc_caches+96
	mov	x21, x0
	ldr	x0, [x8, :lo12:kmalloc_caches+96]
	mov	w1, #3520
	mov	w2, #4096
	bl	kmem_cache_alloc_trace
	cbz	x0, .LBB8_5
// %bb.2:
	mov	x24, x0
	add	x0, sp, #8              // =8
	mov	w2, #4096
	mov	x1, x24
	bl	sg_init_one
	add	x0, sp, #48             // =48
	mov	w2, #488
	mov	w1, wzr
	bl	memset
	add	x0, sp, #48             // =48
	add	x2, sp, #40             // =40
	add	x5, sp, #8              // =8
	mov	w4, #2
	mov	w6, #1
	mov	w7, #4096
	mov	x1, x23
	mov	x3, x21
	bl	ufshcd_compose_scsi_cmd
	and	w3, w22, #0x1
	add	x1, sp, #48             // =48
	mov	x0, x19
	mov	w2, w20
	bl	ufshcd_queuecommand_directly
	mov	w20, w0
	cbnz	w0, .LBB8_9
.LBB8_3:
	mov	x0, x24
	bl	kfree
	b	.LBB8_6
.LBB8_4:
	mov	w20, #-12
	b	.LBB8_7
.LBB8_5:
	mov	w20, #-12
.LBB8_6:
	mov	x0, x21
	bl	kfree
.LBB8_7:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB8_10
// %bb.8:
	mov	w0, w20
	add	sp, sp, #544            // =544
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]     // 16-byte Folded Reload
	ldr	x28, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #80     // 16-byte Folded Reload
	ret
.LBB8_9:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.17
	adrp	x2, .L__func__.ufshcd_send_scsi_request_sense
	add	x1, x1, :lo12:.L.str.17
	add	x2, x2, :lo12:.L__func__.ufshcd_send_scsi_request_sense
	mov	w3, w20
	bl	_dev_err
	b	.LBB8_3
.LBB8_10:
	bl	__stack_chk_fail
.Lfunc_end8:
	.size	ufshcd_send_scsi_request_sense, .Lfunc_end8-ufshcd_send_scsi_request_sense
                                        // -- End function
	.globl	ufshcd_send_request_sense_directly // -- Begin function ufshcd_send_request_sense_directly
	.p2align	2
	.type	ufshcd_send_request_sense_directly,@function
ufshcd_send_request_sense_directly:     // @ufshcd_send_request_sense_directly
.Lufshcd_send_request_sense_directly$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #16]     // 16-byte Folded Spill
	ldr	x20, [x0]
	mov	w9, #4672
	mov	x29, sp
	ldrb	w10, [x20, x9]
	cbz	w10, .LBB9_3
// %bb.1:
	mov	x8, x0
	mov	w9, w1
	mov	w10, #4688
	add	x0, x20, x10
	and	w3, w2, #0x1
	mov	x1, x8
	mov	w2, w9
	bl	ufshcd_send_scsi_request_sense
	mov	w19, w0
	cbz	w0, .LBB9_4
// %bb.2:
	ldr	x0, [x20, #4752]
	adrp	x1, .L.str.17
	adrp	x2, .L__func__.ufshcd_send_request_sense_directly
	add	x1, x1, :lo12:.L.str.17
	add	x2, x2, :lo12:.L__func__.ufshcd_send_request_sense_directly
	mov	w3, w19
	bl	_dev_err
	b	.LBB9_4
.LBB9_3:
	mov	w19, #-14
.LBB9_4:
	mov	w0, w19
	ldp	x20, x19, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end9:
	.size	ufshcd_send_request_sense_directly, .Lfunc_end9-ufshcd_send_request_sense_directly
                                        // -- End function
	.globl	ufshcd_send_scsi_ssu    // -- Begin function ufshcd_send_scsi_ssu
	.p2align	2
	.type	ufshcd_send_scsi_ssu,@function
ufshcd_send_scsi_ssu:                   // @ufshcd_send_scsi_ssu
.Lufshcd_send_scsi_ssu$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	stp	x28, x23, [sp, #16]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #496            // =496
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	x22, x2
	mov	x23, x1
	mov	x19, x0
	mov	x0, sp
	mov	w2, #488
	mov	w1, wzr
	mov	w20, w3
	stur	x8, [x29, #-8]
	bl	memset
	adrp	x8, kmalloc_caches+56
	ldr	x0, [x8, :lo12:kmalloc_caches+56]
	mov	w1, #3520
	mov	w2, #96
	bl	kmem_cache_alloc_trace
	cbz	x0, .LBB10_3
// %bb.1:
	mov	x21, x0
	mov	x0, sp
	mov	w2, #488
	mov	w1, wzr
	bl	memset
	mov	x0, sp
	mov	w4, #3
	mov	x1, x23
	mov	x2, x22
	mov	x3, x21
	mov	x5, xzr
	mov	w6, wzr
	mov	w7, wzr
	bl	ufshcd_compose_scsi_cmd
	mov	x1, sp
	mov	w3, #1
	mov	x0, x19
	mov	w2, w20
	bl	ufshcd_queuecommand_directly
	mov	w20, w0
	cbnz	w0, .LBB10_6
.LBB10_2:
	mov	x0, x21
	bl	kfree
	b	.LBB10_4
.LBB10_3:
	mov	w20, #-12
.LBB10_4:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB10_7
// %bb.5:
	mov	w0, w20
	add	sp, sp, #496            // =496
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldp	x28, x23, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB10_6:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.17
	adrp	x2, .L__func__.ufshcd_send_scsi_ssu
	add	x1, x1, :lo12:.L.str.17
	add	x2, x2, :lo12:.L__func__.ufshcd_send_scsi_ssu
	mov	w3, w20
	bl	_dev_err
	b	.LBB10_2
.LBB10_7:
	bl	__stack_chk_fail
.Lfunc_end10:
	.size	ufshcd_send_scsi_ssu, .Lfunc_end10-ufshcd_send_scsi_ssu
                                        // -- End function
	.globl	ufshcd_send_scsi_sync_cache // -- Begin function ufshcd_send_scsi_sync_cache
	.p2align	2
	.type	ufshcd_send_scsi_sync_cache,@function
ufshcd_send_scsi_sync_cache:            // @ufshcd_send_scsi_sync_cache
.Lufshcd_send_scsi_sync_cache$local:
// %bb.0:
	stp	x29, x30, [sp, #-64]!   // 16-byte Folded Spill
	str	x28, [sp, #16]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #32]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #48]     // 16-byte Folded Spill
	mov	x29, sp
	sub	sp, sp, #512            // =512
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	mov	x22, x1
	mov	x20, x0
	add	x0, sp, #16             // =16
	mov	w2, #488
	mov	w1, wzr
	stur	x8, [x29, #-8]
	bl	memset
	strh	wzr, [sp, #8]
	str	xzr, [sp]
	cbz	x22, .LBB11_7
// %bb.1:
	adrp	x8, kmalloc_caches+96
	ldr	x0, [x8, :lo12:kmalloc_caches+96]
	mov	w8, #53
	mov	w1, #3520
	mov	w2, #2504
	strb	w8, [sp]
	bl	kmem_cache_alloc_trace
	mov	x19, x0
	cbz	x0, .LBB11_8
// %bb.2:
	adrp	x8, kmalloc_caches+56
	ldr	x0, [x8, :lo12:kmalloc_caches+56]
	mov	w1, #3520
	mov	w2, #96
	bl	kmem_cache_alloc_trace
	mov	x21, x0
	cbz	x0, .LBB11_9
// %bb.3:
	add	x0, sp, #16             // =16
	mov	w2, #488
	mov	w1, wzr
	bl	memset
	ldr	x8, [x22]
	mov	w9, #2
	add	x0, sp, #16             // =16
	mov	x2, sp
	mov	w4, #3
	mov	x1, x19
	mov	x3, x21
	mov	x5, xzr
	mov	w6, wzr
	mov	w7, wzr
	str	x8, [x19]
	str	x9, [x19, #136]
	bl	ufshcd_compose_scsi_cmd
	add	x1, sp, #16             // =16
	mov	w2, #2000
	mov	w3, #1
	mov	x0, x20
	bl	ufshcd_queuecommand_directly
	mov	w22, w0
	cbnz	w0, .LBB11_6
// %bb.4:
	mov	w8, #1
	add	x0, sp, #16             // =16
	mov	x2, sp
	mov	w4, #3
	mov	x1, x19
	mov	x3, x21
	mov	x5, xzr
	mov	w6, wzr
	mov	w7, wzr
	str	x8, [x19, #136]
	bl	ufshcd_compose_scsi_cmd
	add	x1, sp, #16             // =16
	mov	w2, #2000
	mov	w3, #1
	mov	x0, x20
	bl	ufshcd_queuecommand_directly
	cbz	w0, .LBB11_10
// %bb.5:
	mov	w22, w0
.LBB11_6:
	ldr	x0, [x20, #64]
	ldr	x3, [x19, #136]
	adrp	x1, .L.str.18
	adrp	x2, .L__func__.ufshcd_send_scsi_sync_cache
	add	x1, x1, :lo12:.L.str.18
	add	x2, x2, :lo12:.L__func__.ufshcd_send_scsi_sync_cache
	mov	w4, w22
	bl	_dev_err
	b	.LBB11_10
.LBB11_7:
	mov	w22, #-19
	b	.LBB11_11
.LBB11_8:
	mov	x21, xzr
.LBB11_9:
	mov	w22, #-12
.LBB11_10:
	mov	x0, x19
	bl	kfree
	mov	x0, x21
	bl	kfree
.LBB11_11:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB11_13
// %bb.12:
	mov	w0, w22
	add	sp, sp, #512            // =512
	ldp	x20, x19, [sp, #48]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]     // 16-byte Folded Reload
	ldr	x28, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #64     // 16-byte Folded Reload
	ret
.LBB11_13:
	bl	__stack_chk_fail
.Lfunc_end11:
	.size	ufshcd_send_scsi_sync_cache, .Lfunc_end11-ufshcd_send_scsi_sync_cache
                                        // -- End function
	.globl	ufshcd_get_health_info  // -- Begin function ufshcd_get_health_info
	.p2align	2
	.type	ufshcd_get_health_info,@function
ufshcd_get_health_info:                 // @ufshcd_get_health_info
.Lufshcd_get_health_info$local:
// %bb.0:
	sub	sp, sp, #128            // =128
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #64]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #112]    // 16-byte Folded Spill
	add	x29, sp, #64            // =64
	mov	w19, #-14
	str	x23, [sp, #80]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #96]     // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	stur	xzr, [sp, #47]
	stp	xzr, xzr, [sp, #32]
	stp	xzr, xzr, [sp, #16]
	stp	xzr, xzr, [sp]
	cbz	x0, .LBB12_8
// %bb.1:
	mov	x22, x1
	cbz	x1, .LBB12_8
// %bb.2:
	mov	x21, x2
	cbz	x2, .LBB12_8
// %bb.3:
	mov	x20, x3
	cbz	x3, .LBB12_8
// %bb.4:
	ldr	x23, [x0]
	mov	w8, #4688
	adds	x0, x23, x8
	b.eq	.LBB12_7
// %bb.5:
	mov	x1, sp
	mov	w2, #55
	bl	ufshcd_read_health_desc
	mov	w19, w0
	cbnz	w0, .LBB12_10
// %bb.6:
	ldrb	w8, [sp, #2]
	strb	w8, [x22]
	ldrb	w8, [sp, #3]
	strb	w8, [x21]
	ldrb	w8, [sp, #4]
	strb	w8, [x20]
	b	.LBB12_8
.LBB12_7:
	mov	w19, #-14
.LBB12_8:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB12_11
// %bb.9:
	mov	w0, w19
	ldp	x20, x19, [sp, #112]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]     // 16-byte Folded Reload
	ldr	x23, [sp, #80]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #64]     // 16-byte Folded Reload
	add	sp, sp, #128            // =128
	ret
.LBB12_10:
	ldr	x0, [x23, #4752]
	adrp	x1, .L.str.19
	adrp	x2, .L__func__.ufshcd_get_health_info
	add	x1, x1, :lo12:.L.str.19
	add	x2, x2, :lo12:.L__func__.ufshcd_get_health_info
	bl	_dev_err
	b	.LBB12_8
.LBB12_11:
	bl	__stack_chk_fail
.Lfunc_end12:
	.size	ufshcd_get_health_info, .Lfunc_end12-ufshcd_get_health_info
                                        // -- End function
	.globl	mas_ufs_populate_mgc_dt // -- Begin function mas_ufs_populate_mgc_dt
	.p2align	2
	.type	mas_ufs_populate_mgc_dt,@function
mas_ufs_populate_mgc_dt:                // @mas_ufs_populate_mgc_dt
.Lmas_ufs_populate_mgc_dt$local:
// %bb.0:
	sub	sp, sp, #144            // =144
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #48]     // 16-byte Folded Spill
	add	x29, sp, #48            // =48
	stp	x28, x27, [sp, #64]     // 16-byte Folded Spill
	stp	x26, x25, [sp, #80]     // 16-byte Folded Spill
	stp	x24, x23, [sp, #96]     // 16-byte Folded Spill
	stp	x22, x21, [sp, #112]    // 16-byte Folded Spill
	stp	x20, x19, [sp, #128]    // 16-byte Folded Spill
	stur	x8, [x29, #-8]
	stp	xzr, xzr, [sp, #24]
	str	xzr, [sp, #16]
	ldr	x8, [x0, #64]
	mov	w9, #23048
	add	x26, x0, x9
	mov	x19, x0
	ldr	x20, [x8, #776]
	mov	w8, #23064
	add	x25, x0, x8
	str	wzr, [sp, #12]
	str	x26, [x0, #23048]
	str	x26, [x0, #23056]
	str	x25, [x0, #23064]
	str	x25, [x0, #23072]
	mov	x0, x20
	mov	x1, xzr
	bl	of_get_next_child
	cbz	x0, .LBB13_20
// %bb.1:
	add	x8, sp, #16             // =16
	adrp	x22, .L.str.20
	adrp	x23, .L.str.22
	mov	x21, x0
	add	x22, x22, :lo12:.L.str.20
	add	x23, x23, :lo12:.L.str.22
	add	x27, x8, #8             // =8
	add	x28, x8, #16            // =16
	b	.LBB13_4
.LBB13_2:                               //   in Loop: Header=BB13_4 Depth=1
	ldr	x9, [x26]
	str	x8, [x9, #8]
	stp	x9, x26, [x0, #24]
	str	x8, [x26]
.LBB13_3:                               //   in Loop: Header=BB13_4 Depth=1
	mov	x0, x20
	mov	x1, x21
	bl	of_get_next_child
	mov	x21, x0
	cbz	x0, .LBB13_20
.LBB13_4:                               // =>This Inner Loop Header: Depth=1
	add	x2, sp, #16             // =16
	mov	x0, x21
	mov	x1, x22
	bl	of_property_read_string
	cbnz	w0, .LBB13_15
// %bb.5:                               //   in Loop: Header=BB13_4 Depth=1
	ldr	x1, [sp, #16]
	mov	x0, x23
	bl	strcmp
	cbz	w0, .LBB13_8
// %bb.6:                               //   in Loop: Header=BB13_4 Depth=1
	ldr	x1, [sp, #16]
	adrp	x0, .L.str.23
	add	x0, x0, :lo12:.L.str.23
	bl	strcmp
	cbnz	w0, .LBB13_15
// %bb.7:                               //   in Loop: Header=BB13_4 Depth=1
	mov	w24, wzr
	b	.LBB13_9
.LBB13_8:                               //   in Loop: Header=BB13_4 Depth=1
	mov	w24, #1
.LBB13_9:                               //   in Loop: Header=BB13_4 Depth=1
	adrp	x1, .L.str.24
	add	x2, sp, #12             // =12
	mov	w3, #1
	mov	x0, x21
	add	x1, x1, :lo12:.L.str.24
	mov	x4, xzr
	bl	of_property_read_variable_u32_array
	tbnz	w0, #31, .LBB13_16
// %bb.10:                              //   in Loop: Header=BB13_4 Depth=1
	adrp	x1, .L.str.26
	mov	x0, x21
	add	x1, x1, :lo12:.L.str.26
	mov	x2, x27
	bl	of_property_read_string
	cbnz	w0, .LBB13_17
// %bb.11:                              //   in Loop: Header=BB13_4 Depth=1
	adrp	x1, .L.str.28
	mov	x0, x21
	add	x1, x1, :lo12:.L.str.28
	mov	x2, x28
	bl	of_property_read_string
	ldr	x8, [x19, #64]
	cbnz	w0, .LBB13_18
// %bb.12:                              //   in Loop: Header=BB13_4 Depth=1
	mov	w1, #40
	mov	w2, #3520
	mov	x0, x8
	bl	devm_kmalloc
	cbz	x0, .LBB13_19
// %bb.13:                              //   in Loop: Header=BB13_4 Depth=1
	ldr	w9, [sp, #12]
	ldp	x10, x11, [sp, #24]
	add	x8, x0, #24             // =24
	strh	w9, [x0]
	stp	x10, x11, [x0, #8]
	str	x8, [x0, #24]
	str	x8, [x0, #32]
	cbnz	w24, .LBB13_2
// %bb.14:                              //   in Loop: Header=BB13_4 Depth=1
	ldr	x9, [x25]
	str	x8, [x9, #8]
	stp	x9, x25, [x0, #24]
	str	x8, [x25]
	b	.LBB13_3
.LBB13_15:                              //   in Loop: Header=BB13_4 Depth=1
	ldr	x0, [x19, #64]
	ldr	x2, [x21]
	adrp	x1, .L.str.21
	add	x1, x1, :lo12:.L.str.21
	bl	_dev_err
	b	.LBB13_3
.LBB13_16:                              //   in Loop: Header=BB13_4 Depth=1
	ldr	x0, [x19, #64]
	ldr	x2, [x21]
	adrp	x1, .L.str.25
	add	x1, x1, :lo12:.L.str.25
	bl	_dev_err
	b	.LBB13_3
.LBB13_17:                              //   in Loop: Header=BB13_4 Depth=1
	ldr	x0, [x19, #64]
	ldr	x2, [x21]
	adrp	x1, .L.str.27
	add	x1, x1, :lo12:.L.str.27
	bl	_dev_err
	b	.LBB13_3
.LBB13_18:                              //   in Loop: Header=BB13_4 Depth=1
	ldr	x2, [x21]
	adrp	x1, .L.str.29
	mov	x0, x8
	add	x1, x1, :lo12:.L.str.29
	bl	_dev_err
	b	.LBB13_3
.LBB13_19:
	ldr	x0, [x19, #64]
	adrp	x1, .L.str.30
	adrp	x2, .L__func__.mas_ufs_populate_mgc_dt
	add	x1, x1, :lo12:.L.str.30
	add	x2, x2, :lo12:.L__func__.mas_ufs_populate_mgc_dt
	mov	w3, #624
	bl	_dev_err
.LBB13_20:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB13_22
// %bb.21:
	ldp	x20, x19, [sp, #128]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]    // 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #48]     // 16-byte Folded Reload
	add	sp, sp, #144            // =144
	ret
.LBB13_22:
	bl	__stack_chk_fail
.Lfunc_end13:
	.size	mas_ufs_populate_mgc_dt, .Lfunc_end13-mas_ufs_populate_mgc_dt
                                        // -- End function
	.globl	ufshcd_mas_mq_init      // -- Begin function ufshcd_mas_mq_init
	.p2align	2
	.type	ufshcd_mas_mq_init,@function
ufshcd_mas_mq_init:                     // @ufshcd_mas_mq_init
.Lufshcd_mas_mq_init$local:
// %bb.0:
	ldr	x8, [x0, #2424]
	mov	w9, #175
	mov	x10, #1
	movk	x10, #192, lsl #32
	mov	x11, #274877907008
	mov	w12, #192
	orr	x8, x8, x9
	str	xzr, [x0, #2600]
	str	x10, [x0, #2408]
	str	x11, [x0, #2416]
	str	x8, [x0, #2424]
	str	w12, [x0, #2372]
	ret
.Lfunc_end14:
	.size	ufshcd_mas_mq_init, .Lfunc_end14-ufshcd_mas_mq_init
                                        // -- End function
	.type	sync_cache_buffer,@object // @sync_cache_buffer
	.local	sync_cache_buffer
	.comm	sync_cache_buffer,8,8
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"begin"
	.size	.L.str, 6

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"wait doorbell clean timeout\n"
	.size	.L.str.1, 29

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"end"
	.size	.L.str.2, 4

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"\0013%s %d++\n"
	.size	.L.str.3, 11

	.type	.L__func__.ufshcd_direct_flush_test,@object // @__func__.ufshcd_direct_flush_test
.L__func__.ufshcd_direct_flush_test:
	.asciz	"ufshcd_direct_flush_test"
	.size	.L__func__.ufshcd_direct_flush_test, 25

	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"\0013%s %d--\n"
	.size	.L.str.4, 11

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"\0013%s, sdev is null!\n"
	.size	.L.str.5, 21

	.type	.L__func__.ufshcd_direct_flush,@object // @__func__.ufshcd_direct_flush
.L__func__.ufshcd_direct_flush:
	.asciz	"ufshcd_direct_flush"
	.size	.L__func__.ufshcd_direct_flush, 20

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"\0013%s, hba is null!\n"
	.size	.L.str.6, 20

	.type	.L.str.7,@object        // @.str.7
.L.str.7:
	.asciz	"%s sys has suspended!\n"
	.size	.L.str.7, 23

	.type	.L.str.8,@object        // @.str.8
.L.str.8:
	.asciz	"%s, can't get the hostlock!\n"
	.size	.L.str.8, 29

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"%s, scsi_device_get error or device not online, %d\n"
	.size	.L.str.9, 52

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"%s, ufshcd_state = %d\n"
	.size	.L.str.10, 23

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"ufshcd: lrb_in_use = 0x%lx\n"
	.size	.L.str.11, 28

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"last_scsi_eh_time(%d): %lld"
	.size	.L.str.12, 28

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"last_ufs_eh_time(%d): %lld"
	.size	.L.str.13, 27

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"last_autopm_time(%d): %lld"
	.size	.L.str.14, 27

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"last_autopm_record(%d): %d"
	.size	.L.str.15, 27

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"UPIU[%d] - issue time %lld - complete time %lld\n"
	.size	.L.str.16, 49

	.type	.L__const.ufshcd_send_scsi_request_sense.cmd,@object // @__const.ufshcd_send_scsi_request_sense.cmd
	.section	.rodata,"a",@progbits
.L__const.ufshcd_send_scsi_request_sense.cmd:
	.asciz	"\003\000\000\000`"
	.size	.L__const.ufshcd_send_scsi_request_sense.cmd, 6

	.type	.L.str.17,@object       // @.str.17
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.17:
	.asciz	"%s: failed with err %d\n"
	.size	.L.str.17, 24

	.type	.L__func__.ufshcd_send_scsi_request_sense,@object // @__func__.ufshcd_send_scsi_request_sense
.L__func__.ufshcd_send_scsi_request_sense:
	.asciz	"ufshcd_send_scsi_request_sense"
	.size	.L__func__.ufshcd_send_scsi_request_sense, 31

	.type	.L__func__.ufshcd_send_request_sense_directly,@object // @__func__.ufshcd_send_request_sense_directly
.L__func__.ufshcd_send_request_sense_directly:
	.asciz	"ufshcd_send_request_sense_directly"
	.size	.L__func__.ufshcd_send_request_sense_directly, 35

	.type	.L__func__.ufshcd_send_scsi_ssu,@object // @__func__.ufshcd_send_scsi_ssu
.L__func__.ufshcd_send_scsi_ssu:
	.asciz	"ufshcd_send_scsi_ssu"
	.size	.L__func__.ufshcd_send_scsi_ssu, 21

	.type	.L.str.18,@object       // @.str.18
.L.str.18:
	.asciz	"%s: failed for lun %llu, ret = %d\n"
	.size	.L.str.18, 35

	.type	.L__func__.ufshcd_send_scsi_sync_cache,@object // @__func__.ufshcd_send_scsi_sync_cache
.L__func__.ufshcd_send_scsi_sync_cache:
	.asciz	"ufshcd_send_scsi_sync_cache"
	.size	.L__func__.ufshcd_send_scsi_sync_cache, 28

	.type	.L.str.19,@object       // @.str.19
.L.str.19:
	.asciz	"%s: Failed getting device health info\n"
	.size	.L.str.19, 39

	.type	.L__func__.ufshcd_get_health_info,@object // @__func__.ufshcd_get_health_info
.L__func__.ufshcd_get_health_info:
	.asciz	"ufshcd_get_health_info"
	.size	.L__func__.ufshcd_get_health_info, 23

	.type	.L.str.20,@object       // @.str.20
.L.str.20:
	.asciz	"compatible"
	.size	.L.str.20, 11

	.type	.L.str.21,@object       // @.str.21
.L.str.21:
	.asciz	"check the compatible %s\n"
	.size	.L.str.21, 25

	.type	.L.str.22,@object       // @.str.22
.L.str.22:
	.asciz	"white"
	.size	.L.str.22, 6

	.type	.L.str.23,@object       // @.str.23
.L.str.23:
	.asciz	"black"
	.size	.L.str.23, 6

	.type	.L.str.24,@object       // @.str.24
.L.str.24:
	.asciz	"manufacturer_id"
	.size	.L.str.24, 16

	.type	.L.str.25,@object       // @.str.25
.L.str.25:
	.asciz	"check the manufacturer_id %s\n"
	.size	.L.str.25, 30

	.type	.L.str.26,@object       // @.str.26
.L.str.26:
	.asciz	"model"
	.size	.L.str.26, 6

	.type	.L.str.27,@object       // @.str.27
.L.str.27:
	.asciz	"check the model %s\n"
	.size	.L.str.27, 20

	.type	.L.str.28,@object       // @.str.28
.L.str.28:
	.asciz	"rev"
	.size	.L.str.28, 4

	.type	.L.str.29,@object       // @.str.29
.L.str.29:
	.asciz	"check the rev %s\n"
	.size	.L.str.29, 18

	.type	.L.str.30,@object       // @.str.30
.L.str.30:
	.asciz	"%s %d Failed to alloc bkops_id\n"
	.size	.L.str.30, 32

	.type	.L__func__.mas_ufs_populate_mgc_dt,@object // @__func__.mas_ufs_populate_mgc_dt
.L__func__.mas_ufs_populate_mgc_dt:
	.asciz	"mas_ufs_populate_mgc_dt"
	.size	.L__func__.mas_ufs_populate_mgc_dt, 24

	.type	.L.str.31,@object       // @.str.31
.L.str.31:
	.asciz	"wait door bell clean %s:tm_doorbell:0x%x, tr_doorbell:0x%x\n"
	.size	.L.str.31, 60

	.type	.L.str.32,@object       // @.str.32
.L.str.32:
	.asciz	"UFS:<%s> Emergency sync cache lun=%llu ret = %d\n"
	.size	.L.str.32, 49

	.type	.L__func__.ufshcd_send_scsi_sync_cache_on_specified_disk,@object // @__func__.ufshcd_send_scsi_sync_cache_on_specified_disk
.L__func__.ufshcd_send_scsi_sync_cache_on_specified_disk:
	.asciz	"ufshcd_send_scsi_sync_cache_on_specified_disk"
	.size	.L__func__.ufshcd_send_scsi_sync_cache_on_specified_disk, 46

	.type	.L.str.33,@object       // @.str.33
.L.str.33:
	.asciz	"wait doorbell clear timeout\n"
	.size	.L.str.33, 29

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
