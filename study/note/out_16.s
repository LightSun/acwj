	.text
	.comm	d,4,4
	.comm	f,4,4
	.comm	e,8,8
	.comm	a,4,4
	.comm	b,4,4
	.comm	c,4,4
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp

	movq	$3, %r8
	movl	%r8d, b(%rip) 		# 将r8的4个字节传送给b变量. b = 3
	movq	$5, %r8    
	movl	%r8d, c(%rip) 		# c = 5
	movzbl	b(%rip), %r8      	
	movzbl	c(%rip), %r9
	movq	$10, %r10
	imulq	%r9, %r10 			# r10 = c * 10 (windows 和 linux 汇编差异。src and dst 相反)

	addq	%r8, %r10           # r10 += b
	movl	%r10d, a(%rip)      # 将r10的4个字节 赋值给 a. a = r10
	movzbl	a(%rip), %r8

	movq	%r8, %rdi
	call	printint      # print a
	movq	%rax, %r9

	movq	$12, %r8
	movl	%r8d, d(%rip)
	movzbl	d(%rip), %r8  # r8 = d = 12

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	leaq	d(%rip), %r8  # get the addr of d . r8 = &d
	movq	%r8, e(%rip)  # e = &d
	movq	e(%rip), %r8
	movq	(%r8), %r8    # r8 = *e
	movl	%r8d, f(%rip) 
	movzbl	f(%rip), %r8  # f = r8 =*e

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq     %rbp
	ret
