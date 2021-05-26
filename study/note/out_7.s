	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	.comm	x,8,8
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8   	 # Perform %r8 - %r9, i.e. 7 - 9
	setl	%r9b		 # Set %r9b to 1 if 7 is less than 9
	andq	$255,%r9	 # Remove all other bits in %r9
	movq	%r9, x(%rip) # Save the result in x	
	movq	x(%rip), %r8
	movq	%r8, %rdi   # rdi 目的变址 寄存器
	call	printint    # Print x out
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	setle	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$9, %r9
	cmpq	%r9, %r8
	setne	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	sete	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setge	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$7, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setle	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$9, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setg	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$9, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setge	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	$9, %r8
	movq	$7, %r9
	cmpq	%r9, %r8
	setne	%r9b
	andq	$255,%r9
	movq	%r9, x(%rip)
	movq	x(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movl	$0, %eax
	popq	%rbp
	ret
	# 栈指针 (%rsp) 和 基址指针 (%rbp)

	# 在bss段申请一段命名空间,该段空间的名称叫symbol, 长度为length. Ld连接器在连接会为它留出空间. bss在运行时才会分配
	# .comm	fred,8,8  # Declare fred
	# .comm	jim,8,8   # Declare jim