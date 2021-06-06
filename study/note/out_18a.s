	.text
	.data
	.globl	a
a:	.long	0
	.data
	.globl	b
b:	.quad	0
	.data
	.globl	c
c:	.byte	0
	.data
	.globl	d
d:	.quad	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp

	leaq	a(%rip), %r8  # &a
	movq	b(%rip), %r9
	movq	%r8, b(%rip)  # b = &a

	movq	$15, %r8
	movq	b(%rip), %r9
	movq	(%r9), %r9  	# *b
	movq	%r8, (%r9)		# *b = 15
	movzbl	a(%rip), %r8

	movq	%r8, %rdi
	call	printint  		# print a
	movq	%rax, %r9

	leaq	c(%rip), %r8  	# &c
	movq	d(%rip), %r9			
	movq	%r8, d(%rip)	# d = &c

	movq	$16, %r8
	movq	d(%rip), %r9
	movzbq	(%r9), %r9    	#	*d = 16 
	movb	%r8b, (%r9)

	movzbq	c(%rip), %r8

	movq	%r8, %rdi
	call	printint 		 # print c
	movq	%rax, %r9

	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq     %rbp
	ret
