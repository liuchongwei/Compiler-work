	.data
NewLine:.asciiz	"\n"
	.text
	addu	$fp	$sp	$0
	ori	$t9	$0	268500992
	li	$t0	1	#one=1
	li	$t1	268500996
	sw	$t0	($t1)
	li	$t0	2	#two=2
	li	$t1	268501000
	sw	$t0	($t1)
	j	_END_1
isDigit:
__FSTART_LAB_1:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	CHARSY	c1	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#GEQ	c1	48	$t0
	subi	$sp	$sp	4
#LEQ	c1	57	$t1
	subi	$sp	$sp	4
#GEQ	c1	48	$t0
	lw	$t0	-8($fp)
	li	$t1	48
	slt	$t0	$t0	$t1
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t0	lable_1	
	bne	$t0	1	lable_1
#LEQ	c1	57	$t1
	lw	$t0	-8($fp)
	li	$t1	57
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t1	lable_3	
	bne	$t0	1	lable_3
#RET		49	
	li	$v0	49
	j	__FEND_LAB_1
#JMP		lable_2	
	j	lable_2
#LAB		lable_3	
lable_3:
#RET		48	
	li	$v0	48
	j	__FEND_LAB_1
#LAB		lable_2	
lable_2:
#JMP		lable_0	
	j	lable_0
#LAB		lable_1	
lable_1:
#LAB		lable_0	
lable_0:
#RET		48	
	li	$v0	48
	j	__FEND_LAB_1
__FEND_LAB_1:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_1:
	j	_END_2
isLetter:
__FSTART_LAB_2:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	CHARSY	c1	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#EQL	c1	95	$t2
	subi	$sp	$sp	4
#GEQ	c1	97	$t3
	subi	$sp	$sp	4
#LEQ	c1	122	$t4
	subi	$sp	$sp	4
#GEQ	c1	65	$t5
	subi	$sp	$sp	4
#LEQ	c1	90	$t6
	subi	$sp	$sp	4
#EQL	c1	95	$t2
	lw	$t0	-8($fp)
	li	$t1	95
	bne	$t0	$t1	__tLABEL0
	li	$t0	1
	j	__tLABEL1
__tLABEL0:
	li	$t0	0
__tLABEL1:
#JNE	$t2	lable_5	
	bne	$t0	1	lable_5
#RET		1	
	li	$v0	1
	j	__FEND_LAB_2
#JMP		lable_4	
	j	lable_4
#LAB		lable_5	
lable_5:
#GEQ	c1	97	$t3
	lw	$t0	-8($fp)
	li	$t1	97
	slt	$t0	$t0	$t1
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t3	lable_7	
	bne	$t0	1	lable_7
#LEQ	c1	122	$t4
	lw	$t0	-8($fp)
	li	$t1	122
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t4	lable_9	
	bne	$t0	1	lable_9
#RET		1	
	li	$v0	1
	j	__FEND_LAB_2
#JMP		lable_8	
	j	lable_8
#LAB		lable_9	
lable_9:
#RET		0	
	li	$v0	0
	j	__FEND_LAB_2
#LAB		lable_8	
lable_8:
#JMP		lable_6	
	j	lable_6
#LAB		lable_7	
lable_7:
#GEQ	c1	65	$t5
	lw	$t0	-8($fp)
	li	$t1	65
	slt	$t0	$t0	$t1
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t5	lable_11	
	bne	$t0	1	lable_11
#LEQ	c1	90	$t6
	lw	$t0	-8($fp)
	li	$t1	90
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t6	lable_13	
	bne	$t0	1	lable_13
#RET		1	
	li	$v0	1
	j	__FEND_LAB_2
#JMP		lable_12	
	j	lable_12
#LAB		lable_13	
lable_13:
#LAB		lable_12	
lable_12:
#JMP		lable_10	
	j	lable_10
#LAB		lable_11	
lable_11:
#LAB		lable_10	
lable_10:
#LAB		lable_6	
lable_6:
#LAB		lable_4	
lable_4:
#RET		0	
	li	$v0	0
	j	__FEND_LAB_2
__FEND_LAB_2:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_2:
	j	_END_3
dient:
__FSTART_LAB_3:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	length	
	sw	$t2	($sp)
	subi	$sp	$sp	4
	li	$t0	84	#true=T
	li	$t1	268501408
	sw	$t0	($t1)
	li	$t0	70	#flase=F
	li	$t1	268501412
	sw	$t0	($t1)
	subi	$sp	$sp	4
#AVALUE	array	0	$t7
	subi	$sp	$sp	4
#CALL	isLetter		$t8
	subi	$sp	$sp	4
#EQL	$t8	1	$t9
	subi	$sp	$sp	4
#LSS	i	length	$t10
	subi	$sp	$sp	4
#PLUS	i	1	$t11
	subi	$sp	$sp	4
#AVALUE	array	i	$t12
	subi	$sp	$sp	4
#CALL	isLetter		$t13
	subi	$sp	$sp	4
#NEQ	$t13	0	$t14
	subi	$sp	$sp	4
#AVALUE	array	i	$t15
	subi	$sp	$sp	4
#CALL	isDigit		$t16
	subi	$sp	$sp	4
#EQL	$t16	49	$t17
	subi	$sp	$sp	4
#PLUS	length	1	$t18
	subi	$sp	$sp	4
#EQL	i	length	$t19
	subi	$sp	$sp	4
#BECOM	0		i
	li	$t0	0
	sw	$t0	-12($fp)
#AVALUE	array	0	$t7
	lw	$t0	12($t9)
	sw	$t0	-16($fp)
#VPARA	$t7	isLetter	
	lw	$t2	-16($fp)
#CALL	isLetter		$t8
	jal	isLetter
	nop
	sw	$v0	-20($fp)
#EQL	$t8	1	$t9
	lw	$t0	-20($fp)
	li	$t1	1
	bne	$t0	$t1	__tLABEL2
	li	$t0	1
	j	__tLABEL3
__tLABEL2:
	li	$t0	0
__tLABEL3:
#JNE	$t9	lable_15	
	bne	$t0	1	lable_15
#BECOM	1		i
	li	$t0	1
	sw	$t0	-12($fp)
#LAB		lable_16	
lable_16:
#LSS	i	length	$t10
	lw	$t0	-12($fp)
	lw	$t1	-8($fp)
	slt	$t0	$t0	$t1
#JNE	$t10	lable_17	
	bne	$t0	1	lable_17
#JMP		lable_19	
	j	lable_19
#LAB		lable_18	
lable_18:
#PLUS	i	1	$t11
	lw	$t0	-12($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-32($fp)
#BECOM	$t11		i
	lw	$t0	-32($fp)
	sw	$t0	-12($fp)
#JMP		lable_16	
	j	lable_16
#LAB		lable_19	
lable_19:
#AVALUE	array	i	$t12
	lw	$t1	-12($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	12
	add	$t1	$t1	$t9
	lw	$t1	0($t1)
	sw	$t1	-36($fp)
#VPARA	$t12	isLetter	
	lw	$t2	-36($fp)
#CALL	isLetter		$t13
	jal	isLetter
	nop
	sw	$v0	-40($fp)
#NEQ	$t13	0	$t14
	lw	$t0	-40($fp)
	li	$t1	0
	beq	$t0	$t1	__tLABEL4
	li	$t0	1
	j	__tLABEL5
__tLABEL4:
	li	$t0	0
__tLABEL5:
#JNE	$t14	lable_21	
	bne	$t0	1	lable_21
#JMP		lable_20	
	j	lable_20
#LAB		lable_21	
lable_21:
#AVALUE	array	i	$t15
	lw	$t1	-12($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	12
	add	$t1	$t1	$t9
	lw	$t1	0($t1)
	sw	$t1	-48($fp)
#VPARA	$t15	isDigit	
	lw	$t2	-48($fp)
#CALL	isDigit		$t16
	jal	isDigit
	nop
	sw	$v0	-52($fp)
#EQL	$t16	49	$t17
	lw	$t0	-52($fp)
	li	$t1	49
	bne	$t0	$t1	__tLABEL6
	li	$t0	1
	j	__tLABEL7
__tLABEL6:
	li	$t0	0
__tLABEL7:
#JNE	$t17	lable_23	
	bne	$t0	1	lable_23
#JMP		lable_22	
	j	lable_22
#LAB		lable_23	
lable_23:
#PLUS	length	1	$t18
	lw	$t0	-8($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-60($fp)
#BECOM	$t18		i
	lw	$t0	-60($fp)
	sw	$t0	-12($fp)
#LAB		lable_22	
lable_22:
#LAB		lable_20	
lable_20:
#JMP		lable_18	
	j	lable_18
#LAB		lable_17	
lable_17:
#EQL	i	length	$t19
	lw	$t0	-12($fp)
	lw	$t1	-8($fp)
	bne	$t0	$t1	__tLABEL8
	li	$t0	1
	j	__tLABEL9
__tLABEL8:
	li	$t0	0
__tLABEL9:
#JNE	$t19	lable_25	
	bne	$t0	1	lable_25
#PRINTF		true	
	li	$v0	11
	lw	$a0	416($t9)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#JMP		lable_24	
	j	lable_24
#LAB		lable_25	
lable_25:
#PRINTF		flase	
	li	$v0	11
	lw	$a0	420($t9)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#LAB		lable_24	
lable_24:
#JMP		lable_14	
	j	lable_14
#LAB		lable_15	
lable_15:
#PRINTF		flase	
	li	$v0	11
	lw	$a0	420($t9)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#LAB		lable_14	
lable_14:
__FEND_LAB_3:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_3:
	j	_END_4
sum:
__FSTART_LAB_4:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	n	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	m	
	sw	$t3	($sp)
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
#PLUS	res	i	$t20
	subi	$sp	$sp	4
#PLUS	i	1	$t21
	subi	$sp	$sp	4
#LEQ	i	m	$t22
	subi	$sp	$sp	4
#BECOM	0		res
	li	$t0	0
	sw	$t0	-20($fp)
#BECOM	n		i
	lw	$t0	-8($fp)
	sw	$t0	-16($fp)
#LAB		lable_26	
lable_26:
#PLUS	res	i	$t20
	lw	$t0	-20($fp)
	lw	$t1	-16($fp)
	add	$t0	$t0	$t1
	sw	$t0	-24($fp)
#BECOM	$t20		res
	lw	$t0	-24($fp)
	sw	$t0	-20($fp)
#PLUS	i	1	$t21
	lw	$t0	-16($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-28($fp)
#BECOM	$t21		i
	lw	$t0	-28($fp)
	sw	$t0	-16($fp)
#LEQ	i	m	$t22
	lw	$t0	-16($fp)
	lw	$t1	-12($fp)
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JEQ	$t22	lable_26	
	bne	$t0	0	lable_26
#RET		res	
	lw	$v0	-20($fp)
	j	__FEND_LAB_4
__FEND_LAB_4:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_4:
	j	_END_5
printASCII:
__FSTART_LAB_5:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PRINTF	hello world!		
	li	$v0	11
	li	$a0	104
	syscall
	li	$v0	11
	li	$a0	101
	syscall
	li	$v0	11
	li	$a0	108
	syscall
	li	$v0	11
	li	$a0	108
	syscall
	li	$v0	11
	li	$a0	111
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	119
	syscall
	li	$v0	11
	li	$a0	111
	syscall
	li	$v0	11
	li	$a0	114
	syscall
	li	$v0	11
	li	$a0	108
	syscall
	li	$v0	11
	li	$a0	100
	syscall
	li	$v0	11
	li	$a0	33
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
__FEND_LAB_5:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_5:
	j	_END_6
toInt:
__FSTART_LAB_6:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	CHARSY	c	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#MINU	c	48	$t23
	subi	$sp	$sp	4
#MINU	c	48	$t23
	lw	$t0	-8($fp)
	li	$t1	48
	sub	$t0	$t0	$t1
	sw	$t0	-12($fp)
#RET		$t23	
	lw	$v0	-12($fp)
	j	__FEND_LAB_6
__FEND_LAB_6:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_6:
	j	_END_7
kongkong:
__FSTART_LAB_7:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
__FEND_LAB_7:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_7:
	j	_END_8
fib:
__FSTART_LAB_8:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	INTSY	n	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#EQL	n	1	$t24
	subi	$sp	$sp	4
#EQL	n	2	$t25
	subi	$sp	$sp	4
#MINU	n	1	$t26
	subi	$sp	$sp	4
#CALL	fib		$t27
	subi	$sp	$sp	4
#MINU	n	2	$t28
	subi	$sp	$sp	4
#CALL	fib		$t29
	subi	$sp	$sp	4
#PLUS	$t27	$t29	$t30
	subi	$sp	$sp	4
#EQL	n	1	$t24
	lw	$t0	-8($fp)
	li	$t1	1
	bne	$t0	$t1	__tLABEL10
	li	$t0	1
	j	__tLABEL11
__tLABEL10:
	li	$t0	0
__tLABEL11:
#JNE	$t24	lable_28	
	bne	$t0	1	lable_28
#RET		1	
	li	$v0	1
	j	__FEND_LAB_8
#JMP		lable_27	
	j	lable_27
#LAB		lable_28	
lable_28:
#LAB		lable_27	
lable_27:
#EQL	n	2	$t25
	lw	$t0	-8($fp)
	li	$t1	2
	bne	$t0	$t1	__tLABEL12
	li	$t0	1
	j	__tLABEL13
__tLABEL12:
	li	$t0	0
__tLABEL13:
#JNE	$t25	lable_30	
	bne	$t0	1	lable_30
#RET		1	
	li	$v0	1
	j	__FEND_LAB_8
#JMP		lable_29	
	j	lable_29
#LAB		lable_30	
lable_30:
#MINU	n	1	$t26
	lw	$t0	-8($fp)
	li	$t1	1
	sub	$t0	$t0	$t1
	sw	$t0	-20($fp)
#VPARA	$t26	fib	
	lw	$t2	-20($fp)
#CALL	fib		$t27
	jal	fib
	nop
	sw	$v0	-24($fp)
#MINU	n	2	$t28
	lw	$t0	-8($fp)
	li	$t1	2
	sub	$t0	$t0	$t1
	sw	$t0	-28($fp)
#VPARA	$t28	fib	
	lw	$t2	-28($fp)
#CALL	fib		$t29
	jal	fib
	nop
	sw	$v0	-32($fp)
#PLUS	$t27	$t29	$t30
	lw	$t0	-24($fp)
	lw	$t1	-32($fp)
	add	$t0	$t0	$t1
	sw	$t0	-36($fp)
#RET		$t30	
	lw	$v0	-36($fp)
	j	__FEND_LAB_8
#LAB		lable_29	
lable_29:
__FEND_LAB_8:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_8:
	j	_END_9
relation:
__FSTART_LAB_9:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
#PARA	CHARSY	c1	
	sw	$t2	($sp)
	subi	$sp	$sp	4
#PARA	CHARSY	c2	
	sw	$t3	($sp)
	subi	$sp	$sp	4
#LEQ	c1	c2	$t31
	subi	$sp	$sp	4
#LSS	c1	c2	$t32
	subi	$sp	$sp	4
#GEQ	c1	c2	$t33
	subi	$sp	$sp	4
#GTR	c1	c2	$t34
	subi	$sp	$sp	4
#NEQ	c1	c2	$t35
	subi	$sp	$sp	4
#EQL	c1	c2	$t36
	subi	$sp	$sp	4
#LEQ	c1	c2	$t31
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t31	lable_32	
	bne	$t0	1	lable_32
#PRINTF		c1	
	li	$v0	11
	lw	$a0	-8($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	<= 	c2	
	li	$v0	11
	li	$a0	60
	syscall
	li	$v0	11
	li	$a0	61
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	lw	$a0	-12($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#JMP		lable_31	
	j	lable_31
#LAB		lable_32	
lable_32:
#LAB		lable_31	
lable_31:
#LSS	c1	c2	$t32
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	slt	$t0	$t0	$t1
#JNE	$t32	lable_34	
	bne	$t0	1	lable_34
#PRINTF		c1	
	li	$v0	11
	lw	$a0	-8($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	< 	c2	
	li	$v0	11
	li	$a0	60
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	lw	$a0	-12($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#JMP		lable_33	
	j	lable_33
#LAB		lable_34	
lable_34:
#LAB		lable_33	
lable_33:
#GEQ	c1	c2	$t33
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	slt	$t0	$t0	$t1
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t33	lable_36	
	bne	$t0	1	lable_36
#PRINTF		c1	
	li	$v0	11
	lw	$a0	-8($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	>= 	c2	
	li	$v0	11
	li	$a0	62
	syscall
	li	$v0	11
	li	$a0	61
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	lw	$a0	-12($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#JMP		lable_35	
	j	lable_35
#LAB		lable_36	
lable_36:
#LAB		lable_35	
lable_35:
#GTR	c1	c2	$t34
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	slt	$t0	$t1	$t0
#JNE	$t34	lable_38	
	bne	$t0	1	lable_38
#PRINTF		c1	
	li	$v0	11
	lw	$a0	-8($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	> 	c2	
	li	$v0	11
	li	$a0	62
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	lw	$a0	-12($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#JMP		lable_37	
	j	lable_37
#LAB		lable_38	
lable_38:
#LAB		lable_37	
lable_37:
#NEQ	c1	c2	$t35
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	beq	$t0	$t1	__tLABEL14
	li	$t0	1
	j	__tLABEL15
__tLABEL14:
	li	$t0	0
__tLABEL15:
#JNE	$t35	lable_40	
	bne	$t0	1	lable_40
#PRINTF		c1	
	li	$v0	11
	lw	$a0	-8($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	!= 	c2	
	li	$v0	11
	li	$a0	33
	syscall
	li	$v0	11
	li	$a0	61
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	lw	$a0	-12($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#JMP		lable_39	
	j	lable_39
#LAB		lable_40	
lable_40:
#LAB		lable_39	
lable_39:
#EQL	c1	c2	$t36
	lw	$t0	-8($fp)
	lw	$t1	-12($fp)
	bne	$t0	$t1	__tLABEL16
	li	$t0	1
	j	__tLABEL17
__tLABEL16:
	li	$t0	0
__tLABEL17:
#JNE	$t36	lable_42	
	bne	$t0	1	lable_42
#PRINTF		c1	
	li	$v0	11
	lw	$a0	-8($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	==	c2	
	li	$v0	11
	li	$a0	61
	syscall
	li	$v0	11
	li	$a0	61
	syscall
	li	$v0	11
	lw	$a0	-12($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#JMP		lable_41	
	j	lable_41
#LAB		lable_42	
lable_42:
#LAB		lable_41	
lable_41:
#RET			
	j	__FEND_LAB_9
__FEND_LAB_9:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	jr	$ra	
_END_9:
_Start:
__FSTART_LAB_10:
	sw	$fp	($sp)
	add	$fp	$sp	$0
	subi	$sp	$sp	4
	sw	$ra	($sp)
	subi	$sp	$sp	4
	li	$t0	97	#CA=a
	li	$t1	268501416
	sw	$t0	($t1)
	li	$t0	49	#cc1=1
	li	$t1	268501420
	sw	$t0	($t1)
	li	$t0	50	#cc2=2
	li	$t1	268501424
	sw	$t0	($t1)
	li	$t0	51	#cc3=3
	li	$t1	268501428
	sw	$t0	($t1)
	li	$t0	3	#three=3
	li	$t1	268501432
	sw	$t0	($t1)
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
	subi	$sp	$sp	4
#PLUS	0	-2	$t37
	subi	$sp	$sp	4
#PLUS	input1	input2	$t38
	subi	$sp	$sp	4
#PLUS	$t38	input3	$t39
	subi	$sp	$sp	4
#PLUS	$t37	$t39	$t40
	subi	$sp	$sp	4
#LEQ	index	25	$t41
	subi	$sp	$sp	4
#PLUS	index	1	$t42
	subi	$sp	$sp	4
#PLUS	cA	1	$t43
	subi	$sp	$sp	4
#LEQ	index	25	$t44
	subi	$sp	$sp	4
#PLUS	index	1	$t45
	subi	$sp	$sp	4
#PLUS	cA	1	$t46
	subi	$sp	$sp	4
#CALL	fib		$t47
	subi	$sp	$sp	4
#PLUS	index	1	$t48
	subi	$sp	$sp	4
#LEQ	index	10	$t49
	subi	$sp	$sp	4
#CALL	toInt		$t50
	subi	$sp	$sp	4
#CALL	toInt		$t51
	subi	$sp	$sp	4
#CALL	sum		$t52
	subi	$sp	$sp	4
#PLUS	ca	1	$t53
	subi	$sp	$sp	4
#LEQ	ca	122	$t54
	subi	$sp	$sp	4
#BECOM	CA		ca
	lw	$t0	424($t9)
	sw	$t0	-40($fp)
#BECOM	65		cA
	li	$t0	65
	sw	$t0	-44($fp)
#SCANF		input1	
	li	$v0	5
	syscall
	sw	$v0	-16($fp)
#SCANF		input2	
	li	$v0	5
	syscall
	sw	$v0	-20($fp)
#SCANF		input3	
	li	$v0	5
	syscall
	sw	$v0	-24($fp)
#PLUS	0	-2	$t37
	li	$t0	0
	li	$t1	-2
	add	$t0	$t0	$t1
	sw	$t0	-48($fp)
#PLUS	input1	input2	$t38
	lw	$t0	-16($fp)
	lw	$t1	-20($fp)
	add	$t0	$t0	$t1
	sw	$t0	-52($fp)
#PLUS	$t38	input3	$t39
	lw	$t0	-52($fp)
	lw	$t1	-24($fp)
	add	$t0	$t0	$t1
	sw	$t0	-56($fp)
#PLUS	$t37	$t39	$t40
	lw	$t0	-48($fp)
	lw	$t1	-56($fp)
	add	$t0	$t0	$t1
	sw	$t0	-60($fp)
#PRINTF		$t40	
	li	$v0	1
	lw	$a0	-60($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#BECOM	0		index
	li	$t0	0
	sw	$t0	-8($fp)
#LAB		lable_43	
lable_43:
#LEQ	index	25	$t41
	lw	$t0	-8($fp)
	li	$t1	25
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t41	lable_44	
	bne	$t0	1	lable_44
#JMP		lable_46	
	j	lable_46
#LAB		lable_45	
lable_45:
#PLUS	index	1	$t42
	lw	$t0	-8($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-68($fp)
#BECOM	$t42		index
	lw	$t0	-68($fp)
	sw	$t0	-8($fp)
#JMP		lable_43	
	j	lable_43
#LAB		lable_46	
lable_46:
#VARRAY	cA	index	array
	lw	$t1	-8($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	12
	add	$t1	$t1	$t9
	lw	$t0	-44($fp)
	sw	$t0	0($t1)
#PLUS	cA	1	$t43
	lw	$t0	-44($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-72($fp)
#BECOM	$t43		cA
	lw	$t0	-72($fp)
	sw	$t0	-44($fp)
#JMP		lable_45	
	j	lable_45
#LAB		lable_44	
lable_44:
#VARRAY	49	26	array
	li	$t0	49
	sw	$t0	116($t9)
#VPARA	27	dient	
	li	$t2	27
#CALL	dient		
	jal	dient
	nop
#VARRAY	49	0	array
	li	$t0	49
	sw	$t0	12($t9)
#VPARA	1	dient	
	li	$t2	1
#CALL	dient		
	jal	dient
	nop
#BECOM	97		cA
	li	$t0	97
	sw	$t0	-44($fp)
#BECOM	0		index
	li	$t0	0
	sw	$t0	-8($fp)
#LAB		lable_47	
lable_47:
#LEQ	index	25	$t44
	lw	$t0	-8($fp)
	li	$t1	25
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JNE	$t44	lable_48	
	bne	$t0	1	lable_48
#JMP		lable_50	
	j	lable_50
#LAB		lable_49	
lable_49:
#PLUS	index	1	$t45
	lw	$t0	-8($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-80($fp)
#BECOM	$t45		index
	lw	$t0	-80($fp)
	sw	$t0	-8($fp)
#JMP		lable_47	
	j	lable_47
#LAB		lable_50	
lable_50:
#VARRAY	cA	index	array
	lw	$t1	-8($fp)
	mul	$t1	$t1	4
	addi	$t1	$t1	12
	add	$t1	$t1	$t9
	lw	$t0	-44($fp)
	sw	$t0	0($t1)
#PLUS	cA	1	$t46
	lw	$t0	-44($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-84($fp)
#BECOM	$t46		cA
	lw	$t0	-84($fp)
	sw	$t0	-44($fp)
#JMP		lable_49	
	j	lable_49
#LAB		lable_48	
lable_48:
#VARRAY	49	26	array
	li	$t0	49
	sw	$t0	116($t9)
#VPARA	27	dient	
	li	$t2	27
#CALL	dient		
	jal	dient
	nop
#BECOM	1		index
	li	$t0	1
	sw	$t0	-8($fp)
#LAB		lable_51	
lable_51:
#VPARA	index	fib	
	lw	$t2	-8($fp)
#CALL	fib		$t47
	jal	fib
	nop
	sw	$v0	-88($fp)
#PRINTF	  	$t47	
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	1
	lw	$a0	-88($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PLUS	index	1	$t48
	lw	$t0	-8($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-92($fp)
#BECOM	$t48		index
	lw	$t0	-92($fp)
	sw	$t0	-8($fp)
#LEQ	index	10	$t49
	lw	$t0	-8($fp)
	li	$t1	10
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JEQ	$t49	lable_51	
	bne	$t0	0	lable_51
#PRINTF	  		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#VPARA	cc1	relation	
	lw	$t2	428($t9)
#VPARA	cc1	relation	
	lw	$t3	428($t9)
#CALL	relation		
	jal	relation
	nop
#VPARA	cc1	relation	
	lw	$t2	428($t9)
#VPARA	cc2	relation	
	lw	$t3	432($t9)
#CALL	relation		
	jal	relation
	nop
#VPARA	cc3	relation	
	lw	$t2	436($t9)
#VPARA	cc2	relation	
	lw	$t3	432($t9)
#CALL	relation		
	jal	relation
	nop
#CALL	kongkong		
	jal	kongkong
	nop
#VPARA	cc1	toInt	
	lw	$t2	428($t9)
#CALL	toInt		$t50
	jal	toInt
	nop
	sw	$v0	-100($fp)
#PRINTF		$t50	
	li	$v0	1
	lw	$a0	-100($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#VPARA	50	toInt	
	li	$t2	50
#CALL	toInt		$t51
	jal	toInt
	nop
	sw	$v0	-104($fp)
#PRINTF		$t51	
	li	$v0	1
	lw	$a0	-104($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PRINTF	   		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#CALL	printASCII		
	jal	printASCII
	nop
#PRINTF	   		
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
	li	$v0	11
	li	$a0	32
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#VPARA	1	sum	
	li	$t2	1
#VPARA	100	sum	
	li	$t3	100
#CALL	sum		$t52
	jal	sum
	nop
	sw	$v0	-108($fp)
#PRINTF		$t52	
	li	$v0	1
	lw	$a0	-108($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#BECOM	CA		ca
	lw	$t0	424($t9)
	sw	$t0	-40($fp)
#LAB		lable_52	
lable_52:
#PRINTF		ca	
	li	$v0	11
	lw	$a0	-40($fp)
	syscall
li $v0, 4 # system call code for print_str
la $a0, NewLine
syscall # print the string
#PLUS	ca	1	$t53
	lw	$t0	-40($fp)
	li	$t1	1
	add	$t0	$t0	$t1
	sw	$t0	-112($fp)
#BECOM	$t53		ca
	lw	$t0	-112($fp)
	sw	$t0	-40($fp)
#LEQ	ca	122	$t54
	lw	$t0	-40($fp)
	li	$t1	122
	slt	$t0	$t1	$t0
	li	$t1	1
	sub	$t0	$t1	$t0
#JEQ	$t54	lable_52	
	bne	$t0	0	lable_52
__FEND_LAB_10:
	lw	$ra	-4($fp)
	add	$sp	$fp	$0
	lw	$fp	($fp)
	li	$v0	10
	syscall
