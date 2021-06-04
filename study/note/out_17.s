	.text
	.data
	.globl	d
d:	.long	0
	.data
	.globl	f
f:	.long	0
	.data
	.globl	e
e:	.quad	0
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
	movq	$3, %r8
	movl	%r8d, b(%rip)
	movq	$5, %r8
	movl	%r8d, c(%rip)
	movzbl	b(%rip), %r8
	movzbl	c(%rip), %r9
	movq	$10, %r10
	imulq	%r9, %r10
	addq	%r8, %r10
	movl	%r10d, a(%rip)
	movzbl	a(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$12, %r8
	movl	%r8d, d(%rip)
	movzbl	d(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	leaq	d(%rip), %r8
	movq	%r8, e(%rip)
	movq	e(%rip), %r8
	movq	(%r8), %r8
	movl	%r8d, f(%rip)
	movzbl	f(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq     %rbp
	ret
