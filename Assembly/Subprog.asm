.data
	ping: .asciiz "Ping\n"
.text
	jal subr
	la $s1, subr
	jalr $s1
	li $t1, 123
	addi $sp, $sp, -4
	sw $t1, ($sp)
	addi $t2, $t1, 100
	addi $sp, $sp, -4
	sw $t2, ($sp)
	la $a0, ping
	li $v0, 4
	syscall
	li $v0, 10
	syscall
subr:
	la $a0, ping
	jr $ra
	
