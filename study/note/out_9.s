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
	movq	$1, %r8
	movq	%r8, i(%rip)     # i = 1
L1:
	movq	i(%rip), %r8
	movq	$10, %r9
	cmpq	%r9, %r8 		# if i <= 10
	jg	L2					# greater than,jump to L2
	movq	i(%rip), %r8
	movq	%r8, %rdi       # print i 
	call	printint
	movq	i(%rip), %r8
	movq	$1, %r9        # add 1 to i
	addq	%r8, %r9
	movq	%r9, i(%rip)
	jmp	L1				  # and loop back.
L2:
	movl	$0, %eax
	popq	%rbp
	ret
