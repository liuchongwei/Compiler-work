	.data
a1:	.word	100
a2:	.word	-12345
a3:	.word	10
a:	.word	123456
b:	.word	123456789
b1:	.asciiz	"+"
b4:	.asciiz	"/"
b5:	.asciiz	"A"
b6:	.asciiz	"0"
abcde:	.asciiz	"a"
	.text
	addu	$fp	$sp	$0
	ori	$t9	$0	268500992
	j	_END_1
e:
__FSTART_LAB_1:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	m	<--
	sw	$t2	($sp)
#PARA	INTSY	m	
	subi	$sp	$sp	4
#PARA	INTSY	n	<--
	sw	$t3	($sp)
#PARA	INTSY	n	
	subi	$sp	$sp	4
#EQL	m	1	$t0
	subi	$sp	$sp	4
#MULT	m	n	$t1
	subi	$sp	$sp	4
#MINU	m	1	$t2
	subi	$sp	$sp	4
#PLUS	n	1	$t3
	subi	$sp	$sp	4
#CALL	e		$t4
	subi	$sp	$sp	4
#EQL	m	1	$t0
	lw	$t0	-8($fp)
	li	$t1	1
	bne	$t0	$t1	__tLABEL0
	li	$t0	1
	j	__tLABEL1
__tLABEL0:
	li	$t0	0
__tLABEL1:
#JNE	$t0	lable_1	
	bne	$t0	1	lable_1
#MULT	m	n	$t1
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	mul	$t0	$t0	$t1
	sw	$t0	-20($fp)
#RET		$t1	
	lw	$v0	-20($fp)
	j	__FEND_LAB_1
#JMP		lable_0	
	j	lable_0
#LAB		lable_1	
lable_1:

#MINU	m	1	$t2
	lw	$t0	-8($fp)
	li	$t1	1
	sub	$t0	$t0	$t1
	sw	$t0	-24($fp)
#VPARA	$t2	e	
	lw	$t2	-24($fp)
#PLUS	n	1	$t3
	lw	$t0	-12($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-28($fp)
#VPARA	$t3	e	
	lw	$t3	-28($fp)
#CALL	e		$t4
	jal	e
	nop
	sw	$v0	-32($fp)
#LAB		lable_0	
lable_0:

__FEND_LAB_1:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_1:
	j	_END_2
e1:
__FSTART_LAB_2:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	m	<--
	sw	$t2	($sp)
#PARA	INTSY	m	
	subi	$sp	$sp	4
#PARA	INTSY	n	<--
	sw	$t3	($sp)
#PARA	INTSY	n	
	subi	$sp	$sp	4
#MULT	m	n	$t5
	subi	$sp	$sp	4
#MULT	m	n	$t5
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	mul	$t0	$t0	$t1
	sw	$t0	-16($fp)
#RET		$t5	
	lw	$v0	-16($fp)
	j	__FEND_LAB_2
__FEND_LAB_2:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_2:
	j	_END_3
f:
__FSTART_LAB_3:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	m	<--
	sw	$t2	($sp)
#PARA	INTSY	m	
	subi	$sp	$sp	4
#PARA	INTSY	n	<--
	sw	$t3	($sp)
#PARA	INTSY	n	
	subi	$sp	$sp	4
#MULT	m	n	$t6
	subi	$sp	$sp	4
#MULT	$t6	10	$t7
	subi	$sp	$sp	4
#MULT	m	n	$t6
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	mul	$t0	$t0	$t1
	sw	$t0	-16($fp)
#MULT	$t6	10	$t7
	lw	$t0	-16($fp)
	li	$t1	10
	mul	$t0	$t0	$t1
	sw	$t0	-20($fp)
#RET		$t7	
	lw	$v0	-20($fp)
	j	__FEND_LAB_3
__FEND_LAB_3:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_3:
_Start:
__FSTART_LAB_4:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#VAROBJ	INTSY	c	
	subi	$sp	$sp	4
#VAROBJ	INTSY	d	
	subi	$sp	$sp	4
#LSS	c	10	$t8
	subi	$sp	$sp	4
#PLUS	c	1	$t9
	subi	$sp	$sp	4
#LEQ	d	c	$t10
	subi	$sp	$sp	4
#PLUS	d	1	$t11
	subi	$sp	$sp	4
#CALL	e1		$t12
	subi	$sp	$sp	4
#CALL	e		$t13
	subi	$sp	$sp	4
#CALL	f		$t14
	subi	$sp	$sp	4
#PLUS	c	1	$t15
	subi	$sp	$sp	4
#MINU	c	1	$t16
	subi	$sp	$sp	4
#MULT	c	d	$t17
	subi	$sp	$sp	4
#MULT	c	d	$t18
	subi	$sp	$sp	4
#LSS	c	d	$t19
	subi	$sp	$sp	4
#LEQ	c	9	$t20
	subi	$sp	$sp	4
#PLUS	c	1	$t21
	subi	$sp	$sp	4
#AVALUE	c2	c	$t22
	subi	$sp	$sp	4
#BECOM	1		c
	li	$t0	1
	sw	$t0	-8($fp)
#LAB		lable_2	
lable_2:

#LSS	c	10	$t8
	lw	$t0	-8($fp)
	li	$t1	10
	slt	$t0	$t0	$t1
#JNE	$t8	lable_3	
	bne	$t0	1	lable_3
#JMP		lable_5	
	j	lable_5
#LAB		lable_4	
lable_4:

#PLUS	c	1	$t9
	lw	$t0	-8($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-20($fp)
#BECOM	$t9		c
	lw	$t0	-20($fp)
	sw	$t0	-8($fp)
#JMP		lable_2	
	j	lable_2
#LAB		lable_5	
lable_5:

#BECOM	1		d
	li	$t0	1
	sw	$t0	-12($fp)
#LAB		lable_6	
lable_6:

#LEQ	d	c	$t10
	lw	$t0	-12($fp)
	lw	$t1	-8($fp)
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t10	lable_7	
	bne	$t0	1	lable_7
#JMP		lable_9	
	j	lable_9
#LAB		lable_8	
lable_8:

#PLUS	d	1	$t11
	lw	$t0	-12($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-28($fp)
#BECOM	$t11		d
	lw	$t0	-28($fp)
	sw	$t0	-12($fp)
#JMP		lable_6	
	j	lable_6
#LAB		lable_9	
lable_9:

#VPARA	c	e1	
	lw	$t2	-8($fp)
#VPARA	d	e1	
	lw	$t3	-12($fp)
#CALL	e1		$t12
	jal	e1
	nop
	sw	$v0	-32($fp)
#PRINTF	@	$t12	
	li	$v0	11
	li	$a0	64
	syscall
	li	$v0	1
	lw	$a0	-32($fp)
	syscall
#JMP		lable_8	
	j	lable_8
#LAB		lable_7	
lable_7:

#JMP		lable_4	
	j	lable_4
#LAB		lable_3	
lable_3:

#SCANF		c	
	li	$v0	5
	syscall
	sw	$v0	-8($fp)
#PRINTF	----------c	c	
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	99
	syscall
	li	$v0	1
	lw	$a0	-8($fp)
	syscall
#SCANF		d	
	li	$v0	5
	syscall
	sw	$v0	-12($fp)
#PRINTF	----------d	d	
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	45
	syscall
	li	$v0	11
	li	$a0	100
	syscall
	li	$v0	1
	lw	$a0	-12($fp)
	syscall
#VPARA	c	e	
	lw	$t2	-8($fp)
#VPARA	d	e	
	lw	$t3	-12($fp)
#CALL	e		$t13
	jal	e
	nop
	sw	$v0	-36($fp)
#PRINTF	#	$t13	
	li	$v0	11
	li	$a0	35
	syscall
	li	$v0	1
	lw	$a0	-36($fp)
	syscall
#VPARA	c	f	
	lw	$t2	-8($fp)
#VPARA	d	f	
	lw	$t3	-12($fp)
#CALL	f		$t14
	jal	f
	nop
	sw	$v0	-40($fp)
#PRINTF	#	$t14	
	li	$v0	11
	li	$a0	35
	syscall
	li	$v0	1
	lw	$a0	-40($fp)
	syscall
#BECOM	0		c
	li	$t0	0
	sw	$t0	-8($fp)
#LAB		lable_10	
lable_10:

#PLUS	c	1	$t15
	lw	$t0	-8($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-44($fp)
#BECOM	$t15		c
	lw	$t0	-44($fp)
	sw	$t0	-8($fp)
#MINU	c	1	$t16
	lw	$t0	-8($fp)
	li	$t1	1
	sub	$t0	$t0	$t1
	sw	$t0	-48($fp)
#MULT	c	d	$t17
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	mul	$t0	$t0	$t1
	sw	$t0	-52($fp)
#VARRAY	$t17	$t16	c2
	lw	$t1	-48($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	96
	add	$t1	$t1	$t9
	lw	$t0	-52($fp)
	sw	$t0	0($t1)
#MULT	c	d	$t18
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	mul	$t0	$t0	$t1
	sw	$t0	-56($fp)
#PRINTF	#	$t18	
	li	$v0	11
	li	$a0	35
	syscall
	li	$v0	1
	lw	$a0	-56($fp)
	syscall
#LSS	c	d	$t19
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	slt	$t0	$t0	$t1
#JEQ	$t19	lable_10	
	bne	$t0	0	lable_10
#BECOM	0		c
	li	$t0	0
	sw	$t0	-8($fp)
#LAB		lable_11	
lable_11:

#LEQ	c	9	$t20
	lw	$t0	-8($fp)
	li	$t1	9
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t20	lable_12	
	bne	$t0	1	lable_12
#JMP		lable_14	
	j	lable_14
#LAB		lable_13	
lable_13:

#PLUS	c	1	$t21
	lw	$t0	-8($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-68($fp)
#BECOM	$t21		c
	lw	$t0	-68($fp)
	sw	$t0	-8($fp)
#JMP		lable_11	
	j	lable_11
#LAB		lable_14	
lable_14:

#AVALUE	c2	c	$t22
	lw	$t1	-8($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	96
	add	$t1	$t1	$t9
	lw	$t1	0($t1)
	sw	$t1	-72($fp)
#PRINTF	*	$t22	
	li	$v0	11
	li	$a0	42
	syscall
	li	$v0	1
	lw	$a0	-72($fp)
	syscall
#JMP		lable_13	
	j	lable_13
#LAB		lable_12	
lable_12:

__FEND_LAB_4:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	li	$v0	10
	syscall
