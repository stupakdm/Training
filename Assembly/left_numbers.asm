.data
	w: .word 0x10010000
.text
start:
	li $v0, 5
	syscall
	move $t0, $v0
	lw $s0, w
	addi $s0, $s0, 4
enter:
	beq $t0, $t1, digits
	li $v0, 5
	syscall
	sw $v0, ($s0)
	addi $s0, $s0, 4
	addi $t1, $t1, 1
	j enter
digits:
	lw $s0, w
	move $t1, $zero
	beq $t1 ,$t0, finish
	addi $t1 ,$t1, 1
	addi $s0, $s0, 4
	lw $t3, ($s0)
	jal left_num_1
left_num_1:
	subi $ra, $ra, 20
	sw $ra, ($sp)
left_num:
	div $t3, $t3, 10
	beq $t3, $zero, jump_back
	move $t4, $t3
	j left_num 
jump_back:
	abs $t4, $t4
	move $a0, $t4
	li $v0, 1
	syscall
	lw $ra, ($sp)
	jr $ra
finish:
	li $v0, 10
	syscall
	
