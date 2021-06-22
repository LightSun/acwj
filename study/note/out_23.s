	.data
	.globl	a
a:	.long	0
	.data
	.globl	b
b:	.long	0
	.data
	.globl	c
c:	.long	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-16,%rsp			# Lower stack pointer by 16

	movq	$10, %r8
	movl	%r8d, -12(%rbp)	 	# z is at offset -12

	movq	$20, %r8
	movl	%r8d, -8(%rbp)		 # y is at offset -8

	movq	$30, %r8
	movb	%r8b, -4(%rbp)		 # x is at offfset -4

	movq	$5, %r8
	movl	%r8d, a(%rip)		 # a has a global label

	movq	$15, %r8			 # b has a global label
	movl	%r8d, b(%rip)

	movq	$25, %r8
	movl	%r8d, c(%rip)

	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	addq	$16,%rsp
	popq  %rbp
	ret
