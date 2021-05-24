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
	.comm	fred,8,8  # Declare fred
	.comm	jim,8,8   # Declare jim
	movq	$5, %r8
	movq	fred, %r8(%rip)  # fred = 5
	movq	$12, %r8
	movq	jim, %r8(%rip)   # jim = 12
	movq	fred(%rip), %r8
	movq	jim(%rip), %r9
	addq	%r8, %r9        # fred + jim
	movq	%r9, %rdi
	call	printint
	movl	$0, %eax
	popq	%rbp
	ret
