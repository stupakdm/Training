.data
	.word 0x76543210
	.half 0x2468
	.word 0x0011
	.byte 1
	.half 0x2222
	.word 0x76543210
	.half 0x2222, 0x3333
	.byte 5,7,9
	.align 2
	.byte 1,3,5
	.align 1
	.byte 7
	.align 1	#«адает сдвиг в пам€ти на определенное количество 
	.byte 9

.text
	li $v0, 5
	syscall
	move $t0, $v0
	bgez $t0, morezero
	mul $t0, $t0, -1
morezero:
	beqz $t0, finish
	div $t0,$t0,10
	mfhi $t2
	add $t1, $t1, $t2
	j morezero
finish:
	move $a0, $t1
	li $v0, 1
	syscall
	
	li $v0, 10
	syscall
