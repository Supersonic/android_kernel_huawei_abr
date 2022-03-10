	.text
	.file	"mas_bkops_sysfs.c"
	.globl	mas_bkops_stat_open     // -- Begin function mas_bkops_stat_open
	.p2align	2
	.type	mas_bkops_stat_open,@function
mas_bkops_stat_open:                    // @mas_bkops_stat_open
.Lmas_bkops_stat_open$local:
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
	adrp	x9, kmalloc_caches+96
	stur	x8, [x29, #-8]
	ldr	x26, [x0, #720]
	ldr	x0, [x9, :lo12:kmalloc_caches+96]
	mov	x19, x1
	mov	w1, #3520
	mov	w2, #4096
	bl	kmem_cache_alloc_trace
	cbz	x0, .LBB0_4
// %bb.1:
	ldr	w3, [x26, #296]
	adrp	x2, .L.str.13
	add	x2, x2, :lo12:.L.str.13
	mov	w1, #4096
	mov	x20, x0
	str	x19, [sp]               // 8-byte Folded Spill
	mov	w19, #4096
	bl	snprintf
	ldr	w3, [x26, #300]
	mov	w21, w0
	sxtw	x8, w21
	adrp	x2, .L.str.14
	add	x0, x20, x8
	sub	x1, x19, x8
	add	x2, x2, :lo12:.L.str.14
	bl	snprintf
	ldr	w3, [x26, #304]
	add	w21, w0, w21
	sxtw	x8, w21
	adrp	x2, .L.str.15
	add	x0, x20, x8
	sub	x1, x19, x8
	add	x2, x2, :lo12:.L.str.15
	bl	snprintf
	ldr	w3, [x26, #308]
	add	w21, w0, w21
	sxtw	x8, w21
	adrp	x2, .L.str.16
	add	x0, x20, x8
	sub	x1, x19, x8
	add	x2, x2, :lo12:.L.str.16
	bl	snprintf
	ldr	w3, [x26, #312]
	add	w21, w0, w21
	sxtw	x8, w21
	adrp	x2, .L.str.17
	add	x0, x20, x8
	sub	x1, x19, x8
	add	x2, x2, :lo12:.L.str.17
	bl	snprintf
	add	w21, w0, w21
	sxtw	x8, w21
	adrp	x2, .L.str.18
	add	x0, x20, x8
	sub	x1, x19, x8
	add	x2, x2, :lo12:.L.str.18
	bl	snprintf
	ldr	w8, [x26, #488]
	add	w22, w21, w0
	sxtw	x9, w22
	add	x24, x20, x9
	sub	x23, x19, x9
	cbz	w8, .LBB0_5
// %bb.2:
	adrp	x21, .L.str.19
	mov	x25, xzr
	mov	w19, wzr
	add	x27, x26, #448          // =448
	add	x21, x21, :lo12:.L.str.19
	mov	x1, x23
	mov	x0, x24
.LBB0_3:                                // =>This Inner Loop Header: Depth=1
	ldr	x8, [x26, #496]
	ldr	w4, [x27, x25, lsl #2]
	mov	x2, x21
	ldr	x3, [x8, x25, lsl #3]
	bl	snprintf
	ldr	w8, [x26, #488]
	add	w19, w0, w19
	add	x25, x25, #1            // =1
	sxtw	x9, w19
	add	x0, x24, x9
	cmp	x25, x8
	sub	x1, x23, x9
	b.lo	.LBB0_3
	b	.LBB0_6
.LBB0_4:
	mov	w0, #-12
	b	.LBB0_71
.LBB0_5:
	mov	w19, wzr
	mov	x0, x24
	mov	x1, x23
.LBB0_6:
	adrp	x21, .L.str.18
	mov	x28, #58368
	add	x21, x21, :lo12:.L.str.18
	movk	x28, #21515, lsl #16
	mov	x2, x21
	movk	x28, #2, lsl #32
	bl	snprintf
	add	w8, w19, w22
	ldr	w3, [x26, #316]
	add	w25, w8, w0
	mov	w19, #4096
	sxtw	x8, w25
	add	x23, x20, x8
	sub	x22, x19, x8
	adrp	x2, .L.str.20
	add	x2, x2, :lo12:.L.str.20
	mov	x0, x23
	mov	x1, x22
	bl	snprintf
	ldr	w3, [x26, #320]
	mov	w24, w0
	sxtw	x8, w24
	adrp	x2, .L.str.21
	add	x0, x23, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.21
	bl	snprintf
	ldr	w3, [x26, #324]
	add	w24, w0, w24
	sxtw	x8, w24
	adrp	x2, .L.str.22
	add	x0, x23, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.22
	bl	snprintf
	ldr	w3, [x26, #328]
	add	w24, w0, w24
	sxtw	x8, w24
	adrp	x2, .L.str.23
	add	x0, x23, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.23
	bl	snprintf
	ldr	w3, [x26, #332]
	add	w24, w0, w24
	sxtw	x8, w24
	adrp	x2, .L.str.24
	add	x0, x23, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.24
	bl	snprintf
	ldr	w3, [x26, #336]
	add	w24, w0, w24
	sxtw	x8, w24
	adrp	x2, .L.str.25
	add	x0, x23, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.25
	bl	snprintf
	add	w24, w0, w24
	sxtw	x8, w24
	add	x0, x23, x8
	sub	x1, x22, x8
	mov	x2, x21
	bl	snprintf
	add	w8, w0, w25
	ldr	w3, [x26, #340]
	add	w25, w8, w24
	sxtw	x8, w25
	add	x22, x20, x8
	sub	x23, x19, x8
	adrp	x2, .L.str.26
	add	x2, x2, :lo12:.L.str.26
	mov	x0, x22
	mov	x1, x23
	bl	snprintf
	ldr	w3, [x26, #344]
	mov	w24, w0
	sxtw	x8, w24
	adrp	x2, .L.str.27
	add	x0, x22, x8
	sub	x1, x23, x8
	add	x2, x2, :lo12:.L.str.27
	bl	snprintf
	ldr	w3, [x26, #348]
	add	w24, w0, w24
	sxtw	x8, w24
	adrp	x2, .L.str.28
	add	x0, x22, x8
	sub	x1, x23, x8
	add	x2, x2, :lo12:.L.str.28
	bl	snprintf
	add	w24, w0, w24
	sxtw	x8, w24
	add	x0, x22, x8
	sub	x1, x23, x8
	mov	x2, x21
	bl	snprintf
	stp	xzr, xzr, [sp, #24]
	stp	xzr, xzr, [sp, #8]
	add	w8, w0, w25
	ldr	x3, [x26, #360]
	add	w27, w8, w24
	sxtw	x8, w27
	sub	x21, x19, x8
	adrp	x19, time_unit
	add	x22, x20, x8
	cmp	x3, #11                 // =11
	add	x19, x19, :lo12:time_unit
	b.hs	.LBB0_8
// %bb.7:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_14
.LBB0_8:
	cmp	x3, x28
	b.hs	.LBB0_11
// %bb.9:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_12
// %bb.10:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_13
.LBB0_11:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_13
.LBB0_12:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_13:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_14:
	adrp	x2, .L.str.29
	add	x2, x2, :lo12:.L.str.29
	add	x3, sp, #8              // =8
	mov	x0, x22
	mov	x1, x21
	strb	wzr, [sp, #39]
	bl	snprintf
	ldr	x3, [x26, #368]
	mov	w23, w0
	sxtw	x8, w23
	add	x24, x22, x8
	cmp	x3, #11                 // =11
	sub	x25, x21, x8
	b.hs	.LBB0_16
// %bb.15:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_22
.LBB0_16:
	cmp	x3, x28
	b.hs	.LBB0_19
// %bb.17:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_20
// %bb.18:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_21
.LBB0_19:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_21
.LBB0_20:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_21:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_22:
	adrp	x2, .L.str.30
	add	x2, x2, :lo12:.L.str.30
	add	x3, sp, #8              // =8
	mov	x0, x24
	mov	x1, x25
	strb	wzr, [sp, #39]
	bl	snprintf
	ldr	x3, [x26, #376]
	add	w25, w0, w23
	sxtw	x8, w25
	add	x23, x22, x8
	cmp	x3, #11                 // =11
	sub	x24, x21, x8
	b.hs	.LBB0_24
// %bb.23:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_30
.LBB0_24:
	cmp	x3, x28
	b.hs	.LBB0_27
// %bb.25:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_28
// %bb.26:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_29
.LBB0_27:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_29
.LBB0_28:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_29:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_30:
	adrp	x2, .L.str.31
	add	x2, x2, :lo12:.L.str.31
	add	x3, sp, #8              // =8
	mov	x0, x23
	mov	x1, x24
	strb	wzr, [sp, #39]
	bl	snprintf
	ldr	x3, [x26, #384]
	add	w25, w0, w25
	sxtw	x8, w25
	add	x23, x22, x8
	cmp	x3, #11                 // =11
	sub	x24, x21, x8
	b.hs	.LBB0_32
// %bb.31:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_38
.LBB0_32:
	cmp	x3, x28
	b.hs	.LBB0_35
// %bb.33:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_36
// %bb.34:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_37
.LBB0_35:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_37
.LBB0_36:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_37:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_38:
	adrp	x2, .L.str.32
	add	x2, x2, :lo12:.L.str.32
	add	x3, sp, #8              // =8
	mov	x0, x23
	mov	x1, x24
	strb	wzr, [sp, #39]
	bl	snprintf
	ldr	x3, [x26, #392]
	add	w25, w0, w25
	sxtw	x8, w25
	add	x23, x22, x8
	cmp	x3, #11                 // =11
	sub	x24, x21, x8
	b.hs	.LBB0_40
// %bb.39:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_46
.LBB0_40:
	cmp	x3, x28
	b.hs	.LBB0_43
// %bb.41:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_44
// %bb.42:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_45
.LBB0_43:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_45
.LBB0_44:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_45:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_46:
	adrp	x2, .L.str.33
	add	x2, x2, :lo12:.L.str.33
	add	x3, sp, #8              // =8
	mov	x0, x23
	mov	x1, x24
	strb	wzr, [sp, #39]
	bl	snprintf
	ldr	x3, [x26, #400]
	add	w25, w0, w25
	sxtw	x8, w25
	add	x23, x22, x8
	cmp	x3, #11                 // =11
	sub	x24, x21, x8
	b.hs	.LBB0_48
// %bb.47:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_54
.LBB0_48:
	cmp	x3, x28
	b.hs	.LBB0_51
// %bb.49:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_52
// %bb.50:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_53
.LBB0_51:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_53
.LBB0_52:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_53:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_54:
	adrp	x2, .L.str.34
	add	x2, x2, :lo12:.L.str.34
	add	x3, sp, #8              // =8
	mov	x0, x23
	mov	x1, x24
	strb	wzr, [sp, #39]
	bl	snprintf
	add	w23, w0, w25
	sxtw	x8, w23
	adrp	x2, .L.str.18
	add	x0, x22, x8
	sub	x1, x21, x8
	add	x2, x2, :lo12:.L.str.18
	bl	snprintf
	stp	xzr, xzr, [sp, #24]
	stp	xzr, xzr, [sp, #8]
	ldr	x3, [x26, #408]
	add	w8, w0, w27
	add	w8, w8, w23
	sxtw	x8, w8
	mov	w9, #4096
	add	x21, x20, x8
	cmp	x3, #11                 // =11
	sub	x22, x9, x8
	b.hs	.LBB0_56
// %bb.55:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_62
.LBB0_56:
	cmp	x3, x28
	b.hs	.LBB0_59
// %bb.57:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_60
// %bb.58:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_61
.LBB0_59:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_61
.LBB0_60:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_61:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_62:
	adrp	x2, .L.str.42
	add	x2, x2, :lo12:.L.str.42
	add	x3, sp, #8              // =8
	mov	x0, x21
	mov	x1, x22
	strb	wzr, [sp, #39]
	bl	snprintf
	ldr	x3, [x26, #416]
	mov	w23, w0
	sxtw	x8, w23
	add	x24, x21, x8
	cmp	x3, #11                 // =11
	sub	x25, x22, x8
	b.hs	.LBB0_64
// %bb.63:
	adrp	x2, .L.str.36
	add	x2, x2, :lo12:.L.str.36
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
	b	.LBB0_70
.LBB0_64:
	cmp	x3, x28
	b.hs	.LBB0_67
// %bb.65:
	mov	w8, #38528
	movk	w8, #152, lsl #16
	cmp	x3, x8
	b.hs	.LBB0_68
// %bb.66:
	lsr	x8, x3, #4
	mov	w9, #2
	mov	w10, #1000
	cmp	x8, #625                // =625
	cinc	x8, x9, lo
	csinc	x9, x10, xzr, hs
	b	.LBB0_69
.LBB0_67:
	mov	w9, #51712
	mov	x8, xzr
	movk	w9, #15258, lsl #16
	b	.LBB0_69
.LBB0_68:
	mov	w9, #16960
	mov	w8, #1
	movk	w9, #15, lsl #16
.LBB0_69:
	ldr	x4, [x19, x8, lsl #3]
	adrp	x2, .L.str.37
	udiv	x3, x3, x9
	add	x2, x2, :lo12:.L.str.37
	add	x0, sp, #8              // =8
	mov	w1, #32
	bl	snprintf
.LBB0_70:
	adrp	x2, .L.str.43
	add	x2, x2, :lo12:.L.str.43
	add	x3, sp, #8              // =8
	mov	x0, x24
	mov	x1, x25
	strb	wzr, [sp, #39]
	bl	snprintf
	ldr	w3, [x26, #424]
	add	w19, w0, w23
	sxtw	x8, w19
	adrp	x2, .L.str.44
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.44
	bl	snprintf
	ldr	w3, [x26, #428]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.45
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.45
	bl	snprintf
	ldr	w3, [x26, #432]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.46
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.46
	bl	snprintf
	ldr	w3, [x26, #436]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.47
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.47
	bl	snprintf
	ldr	w3, [x26, #440]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.48
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.48
	bl	snprintf
	ldr	w3, [x26, #444]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.49
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.49
	bl	snprintf
	add	w8, w0, w19
	sxtw	x8, w8
	add	x21, x21, x8
	sub	x22, x22, x8
	adrp	x2, .L.str.18
	add	x2, x2, :lo12:.L.str.18
	mov	x0, x21
	mov	x1, x22
	bl	snprintf
	ldr	w3, [x26, #504]
	mov	w23, w0
	sxtw	x8, w23
	adrp	x2, .L.str.50
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.50
	bl	snprintf
	ldr	w3, [x26, #508]
	add	w19, w0, w23
	sxtw	x8, w19
	adrp	x2, .L.str.51
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.51
	bl	snprintf
	ldr	w3, [x26, #512]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.52
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.52
	bl	snprintf
	ldr	w3, [x26, #516]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.53
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.53
	bl	snprintf
	ldr	w3, [x26, #520]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.54
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.54
	bl	snprintf
	ldr	w3, [x26, #524]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.55
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.55
	bl	snprintf
	ldr	w3, [x26, #528]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.56
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.56
	bl	snprintf
	ldr	w3, [x26, #532]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.57
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.57
	bl	snprintf
	ldr	w3, [x26, #536]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.58
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.58
	bl	snprintf
	ldr	w3, [x26, #540]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.59
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.59
	bl	snprintf
	ldr	w3, [x26, #544]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.60
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.60
	bl	snprintf
	ldr	w3, [x26, #548]
	add	w19, w0, w19
	sxtw	x8, w19
	adrp	x2, .L.str.61
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.61
	bl	snprintf
	ldr	w3, [x26, #552]
	add	w8, w0, w19
	sxtw	x8, w8
	adrp	x2, .L.str.62
	add	x0, x21, x8
	sub	x1, x22, x8
	add	x2, x2, :lo12:.L.str.62
	bl	snprintf
	ldr	x8, [sp]                // 8-byte Folded Reload
	mov	w0, wzr
	strb	wzr, [x20, #4095]
	str	x20, [x8, #216]
.LBB0_71:
	adrp	x9, __stack_chk_guard
	ldur	x8, [x29, #-8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB0_73
// %bb.72:
	ldp	x20, x19, [sp, #128]    // 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]    // 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]     // 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]     // 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]     // 16-byte Folded Reload
	ldp	x29, x30, [sp, #48]     // 16-byte Folded Reload
	add	sp, sp, #144            // =144
	ret
.LBB0_73:
	bl	__stack_chk_fail
.Lfunc_end0:
	.size	mas_bkops_stat_open, .Lfunc_end0-mas_bkops_stat_open
                                        // -- End function
	.globl	mas_bkops_stat_read     // -- Begin function mas_bkops_stat_read
	.p2align	2
	.type	mas_bkops_stat_read,@function
mas_bkops_stat_read:                    // @mas_bkops_stat_read
.Lmas_bkops_stat_read$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	ldr	x21, [x0, #216]
	mov	x29, sp
	cbz	x21, .LBB1_2
// %bb.1:
	mov	x0, x21
	mov	x19, x3
	mov	x20, x2
	mov	x22, x1
	bl	strlen
	mov	x4, x0
	mov	x0, x22
	mov	x1, x20
	mov	x2, x19
	mov	x3, x21
	bl	simple_read_from_buffer
	b	.LBB1_3
.LBB1_2:
	mov	x0, xzr
.LBB1_3:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]     // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end1:
	.size	mas_bkops_stat_read, .Lfunc_end1-mas_bkops_stat_read
                                        // -- End function
	.globl	mas_bkops_stat_release  // -- Begin function mas_bkops_stat_release
	.p2align	2
	.type	mas_bkops_stat_release,@function
mas_bkops_stat_release:                 // @mas_bkops_stat_release
.Lmas_bkops_stat_release$local:
// %bb.0:
	stp	x29, x30, [sp, #-32]!   // 16-byte Folded Spill
	ldr	x0, [x1, #216]
	str	x19, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x0, .LBB2_2
// %bb.1:
	mov	x19, x1
	bl	kfree
	str	xzr, [x19, #216]
.LBB2_2:
	ldr	x19, [sp, #16]          // 8-byte Folded Reload
	mov	w0, wzr
	ldp	x29, x30, [sp], #32     // 16-byte Folded Reload
	ret
.Lfunc_end2:
	.size	mas_bkops_stat_release, .Lfunc_end2-mas_bkops_stat_release
                                        // -- End function
	.globl	mas_bkops_force_query_open // -- Begin function mas_bkops_force_query_open
	.p2align	2
	.type	mas_bkops_force_query_open,@function
mas_bkops_force_query_open:             // @mas_bkops_force_query_open
.Lmas_bkops_force_query_open$local:
// %bb.0:
	stp	x29, x30, [sp, #-16]!   // 16-byte Folded Spill
	ldr	x8, [x0, #720]
	mov	x29, sp
	cbz	x8, .LBB3_4
// %bb.1:
	ldr	x9, [x8, #8]
	cbz	x9, .LBB3_4
// %bb.2:
	ldr	x9, [x9, #8]
	cbz	x9, .LBB3_4
// %bb.3:
	mov	w0, wzr
	str	x8, [x1, #216]
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.LBB3_4:
	adrp	x0, .L.str
	add	x0, x0, :lo12:.L.str
	bl	printk
	mov	w0, #-19
	ldp	x29, x30, [sp], #16     // 16-byte Folded Reload
	ret
.Lfunc_end3:
	.size	mas_bkops_force_query_open, .Lfunc_end3-mas_bkops_force_query_open
                                        // -- End function
	.globl	mas_bkops_force_query_read // -- Begin function mas_bkops_force_query_read
	.p2align	2
	.type	mas_bkops_force_query_read,@function
mas_bkops_force_query_read:             // @mas_bkops_force_query_read
.Lmas_bkops_force_query_read$local:
// %bb.0:
	sub	sp, sp, #80             // =80
	adrp	x8, __stack_chk_guard
	ldr	x8, [x8, :lo12:__stack_chk_guard]
	stp	x29, x30, [sp, #16]     // 16-byte Folded Spill
	str	x23, [sp, #32]          // 8-byte Folded Spill
	stp	x22, x21, [sp, #48]     // 16-byte Folded Spill
	stp	x20, x19, [sp, #64]     // 16-byte Folded Spill
	adrp	x9, kmalloc_caches+96
	str	x8, [sp, #8]
	str	wzr, [sp, #4]
	ldr	x23, [x0, #216]
	ldr	x0, [x9, :lo12:kmalloc_caches+96]
	mov	x20, x2
	mov	x22, x1
	mov	w1, #3520
	mov	w2, #4096
	add	x29, sp, #16            // =16
	mov	x19, x3
	bl	kmem_cache_alloc_trace
	cbnz	x0, .LBB4_4
// %bb.1:
	mov	x19, #-12
.LBB4_2:
	adrp	x9, __stack_chk_guard
	ldr	x8, [sp, #8]
	ldr	x9, [x9, :lo12:__stack_chk_guard]
	cmp	x9, x8
	b.ne	.LBB4_8
// %bb.3:
	mov	x0, x19
	ldp	x20, x19, [sp, #64]     // 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]     // 16-byte Folded Reload
	ldr	x23, [sp, #32]          // 8-byte Folded Reload
	ldp	x29, x30, [sp, #16]     // 16-byte Folded Reload
	add	sp, sp, #80             // =80
	ret
.LBB4_4:
	mov	x21, x0
	ldp	x8, x0, [x23, #8]
	add	x1, sp, #4              // =4
	ldr	x8, [x8, #8]
	blr	x8
	cbnz	w0, .LBB4_6
// %bb.5:
	ldr	w1, [sp, #4]
	adrp	x0, .L.str.3
	add	x0, x0, :lo12:.L.str.3
	bl	printk
	ldr	w3, [sp, #4]
	adrp	x2, .L.str.4
	add	x2, x2, :lo12:.L.str.4
	mov	w1, #4096
	mov	x0, x21
	bl	snprintf
	b	.LBB4_7
.LBB4_6:
	adrp	x0, .L.str.1
	add	x0, x0, :lo12:.L.str.1
	bl	printk
	adrp	x2, .L.str.2
	add	x2, x2, :lo12:.L.str.2
	mov	w1, #4096
	mov	x0, x21
	bl	snprintf
.LBB4_7:
	mov	x0, x21
	bl	strlen
	mov	x4, x0
	mov	x0, x22
	mov	x1, x20
	mov	x2, x19
	mov	x3, x21
	bl	simple_read_from_buffer
	mov	x19, x0
	mov	x0, x21
	bl	kfree
	b	.LBB4_2
.LBB4_8:
	bl	__stack_chk_fail
.Lfunc_end4:
	.size	mas_bkops_force_query_read, .Lfunc_end4-mas_bkops_force_query_read
                                        // -- End function
	.globl	mas_bkops_force_query_release // -- Begin function mas_bkops_force_query_release
	.p2align	2
	.type	mas_bkops_force_query_release,@function
mas_bkops_force_query_release:          // @mas_bkops_force_query_release
.Lmas_bkops_force_query_release$local:
// %bb.0:
	mov	w0, wzr
	ret
.Lfunc_end5:
	.size	mas_bkops_force_query_release, .Lfunc_end5-mas_bkops_force_query_release
                                        // -- End function
	.globl	mas_bkops_add_debugfs   // -- Begin function mas_bkops_add_debugfs
	.p2align	2
	.type	mas_bkops_add_debugfs,@function
mas_bkops_add_debugfs:                  // @mas_bkops_add_debugfs
.Lmas_bkops_add_debugfs$local:
// %bb.0:
	stp	x29, x30, [sp, #-48]!   // 16-byte Folded Spill
	stp	x20, x19, [sp, #32]     // 16-byte Folded Spill
	mov	x19, x0
	mov	w0, #-19
	str	x21, [sp, #16]          // 8-byte Folded Spill
	mov	x29, sp
	cbz	x19, .LBB6_25
// %bb.1:
	cmn	x1, #1, lsl #12         // =4096
	b.hi	.LBB6_25
// %bb.2:
	adrp	x0, .L.str.5
	add	x0, x0, :lo12:.L.str.5
	bl	debugfs_create_dir
	cmn	x0, #1, lsl #12         // =4096
	b.hi	.LBB6_24
// %bb.3:
	mov	x20, x0
	adrp	x0, .L.str.6
	adrp	x4, mas_bkops_stat_fops
	add	x0, x0, :lo12:.L.str.6
	add	x4, x4, :lo12:mas_bkops_stat_fops
	mov	w1, #384
	mov	x2, x20
	mov	x3, x19
	bl	debugfs_create_file
	cbz	x0, .LBB6_23
// %bb.4:
	adrp	x0, .L.str.7
	adrp	x4, mas_bkops_force_query_fops
	add	x0, x0, :lo12:.L.str.7
	add	x4, x4, :lo12:mas_bkops_force_query_fops
	mov	w1, #384
	mov	x2, x20
	mov	x3, x19
	bl	debugfs_create_file
	cbz	x0, .LBB6_23
// %bb.5:
	adrp	x0, .L.str.8
	add	x3, x19, #176           // =176
	add	x0, x0, :lo12:.L.str.8
	mov	w1, #384
	mov	x2, x20
	bl	debugfs_create_ulong
	cbz	x0, .LBB6_23
// %bb.6:
	adrp	x0, .L.str.9
	add	x3, x19, #248           // =248
	add	x0, x0, :lo12:.L.str.9
	mov	w1, #384
	mov	x2, x20
	bl	debugfs_create_ulong
	cbz	x0, .LBB6_23
// %bb.7:
	adrp	x0, .L.str.10
	add	x3, x19, #264           // =264
	add	x0, x0, :lo12:.L.str.10
	mov	w1, #384
	mov	x2, x20
	bl	debugfs_create_ulong
	cbz	x0, .LBB6_23
// %bb.8:
	adrp	x0, .L.str.11
	add	x3, x19, #280           // =280
	add	x0, x0, :lo12:.L.str.11
	mov	w1, #384
	mov	x2, x20
	bl	debugfs_create_ulong
	cbz	x0, .LBB6_23
// %bb.9:
	adrp	x0, .L.str.12
	add	x3, x19, #244           // =244
	add	x0, x0, :lo12:.L.str.12
	mov	w1, #384
	mov	x2, x20
	bl	debugfs_create_u32
	cbz	x0, .LBB6_23
// %bb.10:
	adrp	x0, .L.str.63
	add	x0, x0, :lo12:.L.str.63
	mov	x1, x20
	bl	debugfs_create_dir
	cmn	x0, #1, lsl #12         // =4096
	b.hi	.LBB6_23
// %bb.11:
	mov	x21, x0
	adrp	x0, .L.str.64
	add	x3, x19, #568           // =568
	add	x0, x0, :lo12:.L.str.64
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.12:
	adrp	x0, .L.str.65
	add	x3, x19, #569           // =569
	add	x0, x0, :lo12:.L.str.65
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.13:
	adrp	x0, .L.str.66
	add	x3, x19, #570           // =570
	add	x0, x0, :lo12:.L.str.66
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.14:
	adrp	x0, .L.str.67
	add	x3, x19, #571           // =571
	add	x0, x0, :lo12:.L.str.67
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.15:
	adrp	x0, .L.str.68
	add	x3, x19, #572           // =572
	add	x0, x0, :lo12:.L.str.68
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.16:
	adrp	x0, .L.str.69
	add	x3, x19, #576           // =576
	add	x0, x0, :lo12:.L.str.69
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_u32
	cbz	x0, .LBB6_22
// %bb.17:
	adrp	x0, .L.str.70
	add	x3, x19, #584           // =584
	add	x0, x0, :lo12:.L.str.70
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_u32
	cbz	x0, .LBB6_22
// %bb.18:
	adrp	x0, .L.str.71
	add	x3, x19, #580           // =580
	add	x0, x0, :lo12:.L.str.71
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.19:
	adrp	x0, .L.str.72
	add	x3, x19, #581           // =581
	add	x0, x0, :lo12:.L.str.72
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.20:
	adrp	x0, .L.str.73
	add	x3, x19, #582           // =582
	add	x0, x0, :lo12:.L.str.73
	mov	w1, #384
	mov	x2, x21
	bl	debugfs_create_bool
	cbz	x0, .LBB6_22
// %bb.21:
	mov	w0, wzr
	str	x20, [x19, #560]
	b	.LBB6_25
.LBB6_22:
	mov	x0, x21
	bl	debugfs_remove_recursive
.LBB6_23:
	mov	x0, x20
	bl	debugfs_remove_recursive
.LBB6_24:
	mov	w0, #-19
.LBB6_25:
	ldp	x20, x19, [sp, #32]     // 16-byte Folded Reload
	ldr	x21, [sp, #16]          // 8-byte Folded Reload
	ldp	x29, x30, [sp], #48     // 16-byte Folded Reload
	ret
.Lfunc_end6:
	.size	mas_bkops_add_debugfs, .Lfunc_end6-mas_bkops_add_debugfs
                                        // -- End function
	.type	.L.str,@object          // @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"\0013Invalid bkops params!\n"
	.size	.L.str, 25

	.type	.L.str.1,@object        // @.str.1
.L.str.1:
	.asciz	"\0013Failed to query bkops status!\n"
	.size	.L.str.1, 33

	.type	.L.str.2,@object        // @.str.2
.L.str.2:
	.asciz	"Failed to query bkops status!\n"
	.size	.L.str.2, 31

	.type	.L.str.3,@object        // @.str.3
.L.str.3:
	.asciz	"\0013bkops_status: %d\n"
	.size	.L.str.3, 20

	.type	.L.str.4,@object        // @.str.4
.L.str.4:
	.asciz	"bkops_status: %d\n"
	.size	.L.str.4, 18

	.type	.L.str.5,@object        // @.str.5
.L.str.5:
	.asciz	"bkops_root"
	.size	.L.str.5, 11

	.type	.L.str.6,@object        // @.str.6
.L.str.6:
	.asciz	"bkops_stat"
	.size	.L.str.6, 11

	.type	mas_bkops_stat_fops,@object // @mas_bkops_stat_fops
	.section	.rodata,"a",@progbits
	.p2align	3
mas_bkops_stat_fops:
	.xword	0
	.xword	0
	.xword	__cfi_mas_bkops_stat_read
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
	.xword	__cfi_mas_bkops_stat_open
	.xword	0
	.xword	__cfi_mas_bkops_stat_release
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
	.size	mas_bkops_stat_fops, 288

	.type	.L.str.7,@object        // @.str.7
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.7:
	.asciz	"force_bkops_query"
	.size	.L.str.7, 18

	.type	mas_bkops_force_query_fops,@object // @mas_bkops_force_query_fops
	.section	.rodata,"a",@progbits
	.p2align	3
mas_bkops_force_query_fops:
	.xword	0
	.xword	0
	.xword	__cfi_mas_bkops_force_query_read
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
	.xword	__cfi_mas_bkops_force_query_open
	.xword	0
	.xword	__cfi_mas_bkops_force_query_release
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
	.size	mas_bkops_force_query_fops, 288

	.type	.L.str.8,@object        // @.str.8
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.8:
	.asciz	"bkops_idle_delay"
	.size	.L.str.8, 17

	.type	.L.str.9,@object        // @.str.9
.L.str.9:
	.asciz	"bkops_check_interval"
	.size	.L.str.9, 21

	.type	.L.str.10,@object       // @.str.10
.L.str.10:
	.asciz	"bkops_check_discard_len"
	.size	.L.str.10, 24

	.type	.L.str.11,@object       // @.str.11
.L.str.11:
	.asciz	"bkops_check_write_len"
	.size	.L.str.11, 22

	.type	.L.str.12,@object       // @.str.12
.L.str.12:
	.asciz	"bkops_retry_en"
	.size	.L.str.12, 15

	.type	.L.str.13,@object       // @.str.13
.L.str.13:
	.asciz	"bkops retry count: %u\n"
	.size	.L.str.13, 23

	.type	.L.str.14,@object       // @.str.14
.L.str.14:
	.asciz	"bkops started count: %u\n"
	.size	.L.str.14, 25

	.type	.L.str.15,@object       // @.str.15
.L.str.15:
	.asciz	"bkops stopped count: %u\n"
	.size	.L.str.15, 25

	.type	.L.str.16,@object       // @.str.16
.L.str.16:
	.asciz	"bkops core stop count: %u\n"
	.size	.L.str.16, 27

	.type	.L.str.17,@object       // @.str.17
.L.str.17:
	.asciz	"bkops abort count: %u\n"
	.size	.L.str.17, 23

	.type	.L.str.18,@object       // @.str.18
.L.str.18:
	.asciz	"\n"
	.size	.L.str.18, 2

	.type	.L.str.19,@object       // @.str.19
.L.str.19:
	.asciz	"%s count: %u\n"
	.size	.L.str.19, 14

	.type	.L.str.20,@object       // @.str.20
.L.str.20:
	.asciz	"bkops actual query count: %u\n"
	.size	.L.str.20, 30

	.type	.L.str.21,@object       // @.str.21
.L.str.21:
	.asciz	"bkops idle work canceled count: %u\n"
	.size	.L.str.21, 36

	.type	.L.str.22,@object       // @.str.22
.L.str.22:
	.asciz	"bkops idle work waited count: %u\n"
	.size	.L.str.22, 34

	.type	.L.str.23,@object       // @.str.23
.L.str.23:
	.asciz	"bkops time based query count: %u\n"
	.size	.L.str.23, 34

	.type	.L.str.24,@object       // @.str.24
.L.str.24:
	.asciz	"bkops write based query count: %u\n"
	.size	.L.str.24, 35

	.type	.L.str.25,@object       // @.str.25
.L.str.25:
	.asciz	"bkops discard based query count: %u\n"
	.size	.L.str.25, 37

	.type	.L.str.26,@object       // @.str.26
.L.str.26:
	.asciz	"bkops query fail count: %u\n"
	.size	.L.str.26, 28

	.type	.L.str.27,@object       // @.str.27
.L.str.27:
	.asciz	"bkops start fail count: %u\n"
	.size	.L.str.27, 28

	.type	.L.str.28,@object       // @.str.28
.L.str.28:
	.asciz	"bkops stop fail count: %u\n"
	.size	.L.str.28, 27

	.type	.L.str.29,@object       // @.str.29
.L.str.29:
	.asciz	"bkops max query time: %s\n"
	.size	.L.str.29, 26

	.type	.L.str.30,@object       // @.str.30
.L.str.30:
	.asciz	"bkops average query time: %s\n"
	.size	.L.str.30, 30

	.type	.L.str.31,@object       // @.str.31
.L.str.31:
	.asciz	"bkops max start time: %s\n"
	.size	.L.str.31, 26

	.type	.L.str.32,@object       // @.str.32
.L.str.32:
	.asciz	"bkops average start time: %s\n"
	.size	.L.str.32, 30

	.type	.L.str.33,@object       // @.str.33
.L.str.33:
	.asciz	"bkops max stop time: %s\n"
	.size	.L.str.33, 25

	.type	.L.str.34,@object       // @.str.34
.L.str.34:
	.asciz	"bkops average stop time: %s\n"
	.size	.L.str.34, 29

	.type	.L.str.36,@object       // @.str.36
.L.str.36:
	.asciz	"%llu ns"
	.size	.L.str.36, 8

	.type	.L.str.37,@object       // @.str.37
.L.str.37:
	.asciz	"%llu %s"
	.size	.L.str.37, 8

	.type	time_unit,@object       // @time_unit
	.section	.rodata,"a",@progbits
	.p2align	3
time_unit:
	.xword	.L.str.38
	.xword	.L.str.39
	.xword	.L.str.40
	.xword	.L.str.41
	.size	time_unit, 32

	.type	.L.str.38,@object       // @.str.38
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.38:
	.asciz	"s"
	.size	.L.str.38, 2

	.type	.L.str.39,@object       // @.str.39
.L.str.39:
	.asciz	"ms"
	.size	.L.str.39, 3

	.type	.L.str.40,@object       // @.str.40
.L.str.40:
	.asciz	"us"
	.size	.L.str.40, 3

	.type	.L.str.41,@object       // @.str.41
.L.str.41:
	.asciz	"ns"
	.size	.L.str.41, 3

	.type	.L.str.42,@object       // @.str.42
.L.str.42:
	.asciz	"max_bkops_duration: %s\n"
	.size	.L.str.42, 24

	.type	.L.str.43,@object       // @.str.43
.L.str.43:
	.asciz	"average_bkops_duration: %s\n"
	.size	.L.str.43, 28

	.type	.L.str.44,@object       // @.str.44
.L.str.44:
	.asciz	"bkops duration less than 100ms: %u\n"
	.size	.L.str.44, 36

	.type	.L.str.45,@object       // @.str.45
.L.str.45:
	.asciz	"bkops duration less than 500ms: %u\n"
	.size	.L.str.45, 36

	.type	.L.str.46,@object       // @.str.46
.L.str.46:
	.asciz	"bkops duration less than 1000ms: %u\n"
	.size	.L.str.46, 37

	.type	.L.str.47,@object       // @.str.47
.L.str.47:
	.asciz	"bkops duration less than 2000ms: %u\n"
	.size	.L.str.47, 37

	.type	.L.str.48,@object       // @.str.48
.L.str.48:
	.asciz	"bkops duration less than 5000ms: %u\n"
	.size	.L.str.48, 37

	.type	.L.str.49,@object       // @.str.49
.L.str.49:
	.asciz	"bkops duration great than 5000ms: %u\n"
	.size	.L.str.49, 38

	.type	.L.str.50,@object       // @.str.50
.L.str.50:
	.asciz	"bkops fs work start in idle: %u\n"
	.size	.L.str.50, 33

	.type	.L.str.51,@object       // @.str.51
.L.str.51:
	.asciz	"bkops fs work start not in idle: %u\n"
	.size	.L.str.51, 37

	.type	.L.str.52,@object       // @.str.52
.L.str.52:
	.asciz	"bkops fs work start already start: %u\n"
	.size	.L.str.52, 39

	.type	.L.str.53,@object       // @.str.53
.L.str.53:
	.asciz	"bkops fs work start no need start: %u\n"
	.size	.L.str.53, 39

	.type	.L.str.54,@object       // @.str.54
.L.str.54:
	.asciz	"bkops fs work query: %u\n"
	.size	.L.str.54, 25

	.type	.L.str.55,@object       // @.str.55
.L.str.55:
	.asciz	"bkops fs work query not idle: %u\n"
	.size	.L.str.55, 34

	.type	.L.str.56,@object       // @.str.56
.L.str.56:
	.asciz	"bkops fs work query already start: %u\n"
	.size	.L.str.56, 39

	.type	.L.str.57,@object       // @.str.57
.L.str.57:
	.asciz	"bkops fs work query no need start: %u\n"
	.size	.L.str.57, 39

	.type	.L.str.58,@object       // @.str.58
.L.str.58:
	.asciz	"bkops fs work stop not start: %u\n"
	.size	.L.str.58, 34

	.type	.L.str.59,@object       // @.str.59
.L.str.59:
	.asciz	"bkops fs work stop : %u\n"
	.size	.L.str.59, 25

	.type	.L.str.60,@object       // @.str.60
.L.str.60:
	.asciz	"bkops idle in fs work start: %u\n"
	.size	.L.str.60, 33

	.type	.L.str.61,@object       // @.str.61
.L.str.61:
	.asciz	"bkops idle not in fs work start: %u\n"
	.size	.L.str.61, 37

	.type	.L.str.62,@object       // @.str.62
.L.str.62:
	.asciz	"bkops fs work end: %u\n"
	.size	.L.str.62, 23

	.type	.L.str.63,@object       // @.str.63
.L.str.63:
	.asciz	"bkops_test_root"
	.size	.L.str.63, 16

	.type	.L.str.64,@object       // @.str.64
.L.str.64:
	.asciz	"sim_bkops_start_fail"
	.size	.L.str.64, 21

	.type	.L.str.65,@object       // @.str.65
.L.str.65:
	.asciz	"sim_bkops_stop_fail"
	.size	.L.str.65, 20

	.type	.L.str.66,@object       // @.str.66
.L.str.66:
	.asciz	"sim_bkops_query_fail"
	.size	.L.str.66, 21

	.type	.L.str.67,@object       // @.str.67
.L.str.67:
	.asciz	"sim_critical_bkops"
	.size	.L.str.67, 19

	.type	.L.str.68,@object       // @.str.68
.L.str.68:
	.asciz	"sim_bkops_abort"
	.size	.L.str.68, 16

	.type	.L.str.69,@object       // @.str.69
.L.str.69:
	.asciz	"sim_bkops_stop_delay"
	.size	.L.str.69, 21

	.type	.L.str.70,@object       // @.str.70
.L.str.70:
	.asciz	"sim_bkops_query_delay"
	.size	.L.str.70, 22

	.type	.L.str.71,@object       // @.str.71
.L.str.71:
	.asciz	"skip_bkops_stop"
	.size	.L.str.71, 16

	.type	.L.str.72,@object       // @.str.72
.L.str.72:
	.asciz	"disable_bkops"
	.size	.L.str.72, 14

	.type	.L.str.73,@object       // @.str.73
.L.str.73:
	.asciz	"bkops_force_query"
	.size	.L.str.73, 18

	.ident	"Android (6443078 based on r383902) clang version 11.0.1 (https://android.googlesource.com/toolchain/llvm-project b397f81060ce6d701042b782172ed13bee898b79)"
	.section	".note.GNU-stack","",@progbits
