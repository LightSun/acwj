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
	.comm	i,8,8       # i is 8 bytes
	.comm	j,1,1		# j is 1 byte
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%%rsp, %%rbp
	movq	$20, %r8
	movb	%r8b, j(%rip)  # j = 20
	movzbq	j(%rip), %r8   # 64位传送
	movq	%r8, %rdi 
	call	printint       # print j

	movq	$10, %r8         # i = 10 
	movq	%r8, i(%rip)
	movq	i(%rip), %r8     
	movq	%r8, %rdi		 # print i
	call	printint

	movq	$1, %r8       	 # i = 1
	movq	%r8, i(%rip)
L1:
	movq	i(%rip), %r8
	movq	$5, %r9
	cmpq	%r9, %r8         # if i > 5 to L2
	jg	L2
	movq	i(%rip), %r8
	movq	%r8, %rdi
	call	printint

	movq	i(%rip), %r8    
	movq	$1, %r9
	addq	%r8, %r9        # i = i + 1
	movq	%r9, i(%rip)
	jmp	L1
L2:
	movq	$253, %r8      # j = 253
	movb	%r8b, j(%rip)
L3:
	movzbq	j(%rip), %r8   
	movq	$2, %r9
	cmpq	%r9, %r8      # if j == 2 to L4
	je	L4
	movzbq	j(%rip), %r8
	movq	%r8, %rdi
	call	printint

	movzbq	j(%rip), %r8
	movq	$1, %r9
	addq	%r8, %r9       # j = j + 1
	movb	%r9b, j(%rip)
	jmp	L3
L4:
	movl $0, %eax
	popq     %rbp
	ret
