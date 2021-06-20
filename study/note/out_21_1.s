	.text
	.data
	.globl	a
a:	.byte	0
	.data
	.globl	b
b:	.byte	0
	.data
	.globl	c
c:	.byte	0
	.data
	.globl	d
d:	.long	0
	.data
	.globl	e
e:	.long	0
	.data
	.globl	f
f:	.long	0
	.data
	.globl	g
g:	.quad	0
	.data
	.globl	h
h:	.quad	0
	.data
	.globl	i
i:	.quad	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp

	movq	$5, %r8
	movb	%r8b, b(%rip)	# b = 5
	movq	$7, %r8
	movb	%r8b, c(%rip)	# c = 7

	movzbq	c(%rip), %r9    # 
	incb	c(%rip)			# move c and c++

	addq	%r8, %r9		# 
	movb	%r9b, a(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9       # print a

	movq	$5, %r8
	movl	%r8d, e(%rip)
	movq	$7, %r8
	movl	%r8d, f(%rip)
	movslq	f(%rip), %r9	#
	incl	f(%rip)
	addq	%r8, %r9
	movl	%r9d, d(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	movq	$5, %r8
	movq	%r8, h(%rip)
	movq	$7, %r8
	movq	%r8, i(%rip)
	movq	i(%rip), %r9
	incq	i(%rip)
	addq	%r8, %r9
	movq	%r9, g(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	movzbq	b(%rip), %r8
	decb	b(%rip)
	addq	%r8, %r9
	movb	%r9b, a(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	movslq	e(%rip), %r8
	decl	e(%rip)
	addq	%r8, %r9
	movl	%r9d, d(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	movq	h(%rip), %r8
	decq	h(%rip)
	addq	%r8, %r9
	movq	%r9, g(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	incb	b(%rip)
	movzbq	b(%rip), %r8
	addq	%r8, %r9
	movb	%r9b, a(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	incl	e(%rip)
	movslq	e(%rip), %r8
	addq	%r8, %r9
	movl	%r9d, d(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	incq	h(%rip)
	movq	h(%rip), %r8
	addq	%r8, %r9
	movq	%r9, g(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	decb	c(%rip)
	movzbq	c(%rip), %r9
	imulq	%r8, %r9
	movb	%r9b, a(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	decl	f(%rip)
	movslq	f(%rip), %r9
	imulq	%r8, %r9
	movl	%r9d, d(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	decq	i(%rip)
	movq	i(%rip), %r9
	imulq	%r8, %r9
	movq	%r9, g(%rip)

	movq	%r8, %rdi
	call	printint
	movq	%rax, %r9

	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq     %rbp
	ret
