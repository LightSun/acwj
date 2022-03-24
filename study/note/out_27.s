	.text
	.globl	param8
	.type	param8, @function
param8:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	addq	$-32,%rsp
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L1
L1:
	addq	$32,%rsp
	popq  %rbp
	ret
	.globl	param5
	.type	param5, @function
param5:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	addq	$-32,%rsp
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L2
L2:
	addq	$32,%rsp
	popq  %rbp
	ret
	.globl	param2
	.type	param2, @function
param2:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	addq	$-32,%rsp
	movq	$3, %r10
	movl	%r10d, -12(%rbp)
	movq	$4, %r10
	movl	%r10d, -16(%rbp)
	movq	$5, %r10
	movl	%r10d, -20(%rbp)
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movslq	-12(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movslq	-16(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movslq	-20(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L3
L3:
	addq	$32,%rsp
	popq  %rbp
	ret
	.globl	param0
	.type	param0, @function
param0:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$-32,%rsp
	movq	$1, %r10
	movl	%r10d, -4(%rbp)
	movq	$2, %r10
	movl	%r10d, -8(%rbp)
	movq	$3, %r10
	movl	%r10d, -12(%rbp)
	movq	$4, %r10
	movl	%r10d, -16(%rbp)
	movq	$5, %r10
	movl	%r10d, -20(%rbp)
	movslq	-4(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movslq	-8(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movslq	-12(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movslq	-16(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movslq	-20(%rbp), %r10
	movq	%r10, %rdi
	call	printint
	movq	%rax, %r11
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L4
L4:
	addq	$32,%rsp
	popq  %rbp
	ret
