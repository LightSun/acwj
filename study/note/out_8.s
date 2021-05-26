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
	.comm	i,8,8
	.comm	j,8,8

	movq	$6, %r8
	movq	%r8, i(%rip) # i = 6
	movq	$12, %r8
	movq	%r8, j(%rip)  # j = 12
	movq	i(%rip), %r8
	movq	j(%rip), %r9
	cmpq	%r9, %r8	 # Compare %r8-%r9, i.e. i-j
	jge	L1				 # Jump to L1 if i >= j
	movq	i(%rip), %r8
	movq	%r8, %rdi   # print i
	call	printint
	jmp	L2				# skip the else code
L1:
	movq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint  # print j
L2:
	movl	$0, %eax
	popq	%rbp
	ret
