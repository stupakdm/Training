.data
	a: .float 123.456
	b: .float 654.321
	_2: .float 2
.text
	lwc1 $f0, a
	lwc1 $f1, b
	lwc1 $f2, _2
	add.s $f3, $f2, $f1,
	div.s $f8, $f3, $f2
	mov.d $f12, $f8
	li $v0, 3
	syscall