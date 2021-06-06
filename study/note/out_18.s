	.text
	.data
	.globl	a
a:	.long	0
	.data
	.globl	b
b:	.long	0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp

	movq	$34, %r8
	movzbl	b(%rip), %r9
	movl	%r8d, b(%rip)  # b = 34, r8d 表示取低32位
	
	movzbl	a(%rip), %r10
	movl	%r8d, a(%rip)  # a =  34
	movzbl	a(%rip), %r8

	movq	%r8, %rdi 
	call	printint  # print a
	movq	%rax, %r9

	movzbl	b(%rip), %r8

	movq	%r8, %rdi
	call	printint  # print b
	movq	%rax, %r9

	movq	$0, %r8
	movl	%r8d, %eax
	jmp	L1
L1:
	popq     %rbp
	ret
