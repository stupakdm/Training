.data
	w: .ascii "\n"
.text
	li $t2, 0x10010004
	#lb $t0, ($t2)
	#sb $t0, 0x1000
	li $v0, 5
	syscall
	move $s0, $v0

input:
	beq $t0, $s0, check
	add $t0, $t0, 1
	li $v0, 5
	syscall
	sw $v0, ($t2)
	addi $t2, $t2, 4
	j input
check:
	beqz $s0, finish
	subi $t2, $t2, 4
	sub $s0, $s0, 1
	lw $t1, ($t2)
	div $t3, $t1, 2
	mfhi $t3
	beqz $t3, print
	j check
print:
	move $a0, $t1
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, w
	syscall
	j check

finish:
	li $v0, 10
	syscall
	
