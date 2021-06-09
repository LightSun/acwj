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
	leaq	b(%rip), %r9
	movq	$3, %r10
	salq	$2, %r10
	addq	%r9, %r10
	movq	%r8, (%r10)
	leaq	b(%rip), %r8
	movq	$3, %r9
	salq	$2, %r9
	addq	%r8, %r9
	movq	(%r9), %r9
	movl	%r9d, a(%rip)
	movzbl	a(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9
	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq     %rbp
	ret
