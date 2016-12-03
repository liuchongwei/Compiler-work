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
#PARA	INTSY	m	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	n	
	sw	$t3	($sp)
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
f:
__FSTART_LAB_2:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	m	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	n	
	sw	$t3	($sp)
	subi	$sp	$sp	4
#MULT	m	n	$t5
	subi	$sp	$sp	4
#MULT	$t5	10	$t6
	subi	$sp	$sp	4
#MULT	m	n	$t5
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	mul	$t0	$t0	$t1
	sw	$t0	-16($fp)
#MULT	$t5	10	$t6
	lw	$t0	-16($fp)
	li	$t1	10
	mul	$t0	$t0	$t1
	sw	$t0	-20($fp)
#RET		$t6	
	lw	$v0	-20($fp)
	j	__FEND_LAB_2
__FEND_LAB_2:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_2:
	j	_END_3
g:
__FSTART_LAB_3:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#RET		97	
	li	$v0	97
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
	subi	$sp	$sp	4
	subi	$sp	$sp	4
#CALL	g		$t7
	subi	$sp	$sp	4
#PLUS	97	1	$t8
	subi	$sp	$sp	4
#LSS	97	$t8	$t9
	subi	$sp	$sp	4
#NEQ	c	0	$t10
	subi	$sp	$sp	4
#CALL	g		$t11
	subi	$sp	$sp	4
#EQL	$t11	97	$t12
	subi	$sp	$sp	4
#MINU	98	97	$t13
	subi	$sp	$sp	4
#MINU	98	97	$t14
	subi	$sp	$sp	4
#AVALUE	symbol	$t14	$t15
	subi	$sp	$sp	4
#CALL	f		$t16
	subi	$sp	$sp	4
#CALL	e		$t17
	subi	$sp	$sp	4
#BECOM	9		c
	li	$t0	9
	sw	$t0	-8($fp)
#CALL	g		$t7
	jal	g
	nop
	sw	$v0	-16($fp)
#PRINTF		$t7	
	li	$v0	11
	lw	$a0	-16($fp)
	syscall
#PLUS	97	1	$t8
	li	$t0	97
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-20($fp)
#LSS	97	$t8	$t9
	li	$t0	97
	lw	$t1	-20($fp)
	slt	$t0	$t0	$t1
#JNE	$t9	lable_3	
	bne	$t0	1	lable_3
#PRINTF	#success		
	li	$v0	11
	li	$a0	35
	syscall
	li	$v0	11
	li	$a0	115
	syscall
	li	$v0	11
	li	$a0	117
	syscall
	li	$v0	11
	li	$a0	99
	syscall
	li	$v0	11
	li	$a0	99
	syscall
	li	$v0	11
	li	$a0	101
	syscall
	li	$v0	11
	li	$a0	115
	syscall
	li	$v0	11
	li	$a0	115
	syscall
#JMP		lable_2	
	j	lable_2
#LAB		lable_3	
lable_3:
#PRINTF	#fail		
	li	$v0	11
	li	$a0	35
	syscall
	li	$v0	11
	li	$a0	102
	syscall
	li	$v0	11
	li	$a0	97
	syscall
	li	$v0	11
	li	$a0	105
	syscall
	li	$v0	11
	li	$a0	108
	syscall
#LAB		lable_2	
lable_2:
#NEQ	c	0	$t10
	lw	$t0	-8($fp)
	li	$t1	0
	beq	$t0	$t1	__tLABEL2
	li	$t0	1
	j	__tLABEL3
__tLABEL2:
	li	$t0	0
__tLABEL3:
#JNE	$t10	lable_5	
	bne	$t0	1	lable_5
#PRINTF	#haha		
	li	$v0	11
	li	$a0	35
	syscall
	li	$v0	11
	li	$a0	104
	syscall
	li	$v0	11
	li	$a0	97
	syscall
	li	$v0	11
	li	$a0	104
	syscall
	li	$v0	11
	li	$a0	97
	syscall
#JMP		lable_4	
	j	lable_4
#LAB		lable_5	
lable_5:
#LAB		lable_4	
lable_4:
#CALL	g		$t11
	jal	g
	nop
	sw	$v0	-32($fp)
#EQL	$t11	97	$t12
	lw	$t0	-32($fp)
	li	$t1	97
	bne	$t0	$t1	__tLABEL4
	li	$t0	1
	j	__tLABEL5
__tLABEL4:
	li	$t0	0
__tLABEL5:
#JNE	$t12	lable_7	
	bne	$t0	1	lable_7
#PRINTF	#hahaha		
	li	$v0	11
	li	$a0	35
	syscall
	li	$v0	11
	li	$a0	104
	syscall
	li	$v0	11
	li	$a0	97
	syscall
	li	$v0	11
	li	$a0	104
	syscall
	li	$v0	11
	li	$a0	97
	syscall
	li	$v0	11
	li	$a0	104
	syscall
	li	$v0	11
	li	$a0	97
	syscall
#JMP		lable_6	
	j	lable_6
#LAB		lable_7	
lable_7:
#LAB		lable_6	
lable_6:
#MINU	98	97	$t13
	li	$t0	98
	li	$t1	97
	sub	$t0	$t0	$t1
	sw	$t0	-40($fp)
#VARRAY	111	$t13	symbol
	lw	$t1	-40($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	0
	add	$t1	$t1	$t9
	li	$t0	111
	sw	$t0	0($t1)
#MINU	98	97	$t14
	li	$t0	98
	li	$t1	97
	sub	$t0	$t0	$t1
	sw	$t0	-44($fp)
#AVALUE	symbol	$t14	$t15
	lw	$t1	-44($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	0
	add	$t1	$t1	$t9
	lw	$t1	0($t1)
	sw	$t1	-48($fp)
#PRINTF		$t15	
	li	$v0	11
	lw	$a0	-48($fp)
	syscall
#VPARA	1	f	
	li	$t2	1
#VPARA	1	f	
	li	$t3	1
#CALL	f		$t16
	jal	f
	nop
	sw	$v0	-52($fp)
#VPARA	$t16	e	
	lw	$t2	-52($fp)
#VPARA	10	e	
	li	$t3	10
#CALL	e		$t17
	jal	e
	nop
	sw	$v0	-56($fp)
#PRINTF		$t17	
	li	$v0	1
	lw	$a0	-56($fp)
	syscall
__FEND_LAB_4:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	li	$v0	10
	syscall
