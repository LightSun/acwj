	.text
	.data
	.globl	a
	.data
	.globl	b
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
b:	.quad	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp

	movq	$12, %r8
	leaq	b(%rip), %r9  	# & b 
	movq	$3, %r10		
	salq	$2, %r10		# << 2. eg: 3 * 4
	addq	%r9, %r10 		# add to b's address
	movq	%r8, (%r10)		# b[3] = 12

	leaq	b(%rip), %r8	# &b
	movq	$3, %r9
	salq	$2, %r9 		# << 2. 3 * 4
	addq	%r8, %r9		# b's address += 3*4
	movq	(%r9), %r9		# r9 = b[3]
	movl	%r9d, a(%rip)   # a = r9
	movzbl	a(%rip), %r8

	movq	%r8, %rdi
	call	printint	# print a
	movq	%rax, %r9

	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq     %rbp
	ret
