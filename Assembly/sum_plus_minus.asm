.text
	li $v0, 5
	syscall
	move $s0, $v0
continue:
	beq $t0, $s0, finish
	add $t0, $t0, 1
	li $v0, 5
	syscall
	move $t1, $v0
	beq $t2, 1, minus
	add $t3, $t3, $t1
	li $t2, 1
	j continue
minus:
	sub $t3, $t3, $t1
	li $t2, 0
	j continue

finish:
	move $a0, $t3
	li $v0, 1
	syscall
	
	li $v0, 10
	syscall
