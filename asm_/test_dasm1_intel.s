	.file	"test_dasm1.c"
	.intel_syntax noprefix
	.text
	.def	printf;	.scl	3;	.type	32;	.endef
printf:
	push	rbp
	mov	rbp, rsp
	push	rbx
	sub	rsp, 56
	mov	QWORD PTR 16[rbp], rcx
	mov	QWORD PTR 24[rbp], rdx
	mov	QWORD PTR 32[rbp], r8
	mov	QWORD PTR 40[rbp], r9
	lea	rax, 24[rbp]
	mov	QWORD PTR -32[rbp], rax
	mov	rbx, QWORD PTR -32[rbp]
	mov	ecx, 1
	mov	rax, QWORD PTR __imp___acrt_iob_func[rip]
	call	rax
	mov	r8, rbx
	mov	rdx, QWORD PTR 16[rbp]
	mov	rcx, rax
	call	__mingw_vfprintf
	mov	DWORD PTR -20[rbp], eax
	mov	eax, DWORD PTR -20[rbp]
	mov	rbx, QWORD PTR -8[rbp]
	leave
	ret
	.def	__main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
.LC0:
	.ascii "result: \0"
.LC1:
	.ascii "%s %d\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
main:
	push	rbp
	mov	rbp, rsp
	sub	rsp, 64
	mov	DWORD PTR 16[rbp], ecx
	mov	QWORD PTR 24[rbp], rdx
	call	__main
	mov	DWORD PTR -4[rbp], 2
	mov	DWORD PTR -8[rbp], 3
	mov	DWORD PTR -12[rbp], 5
	mov	DWORD PTR -16[rbp], 8
	mov	DWORD PTR -20[rbp], 3
	mov	eax, DWORD PTR -8[rbp]
	imul	eax, DWORD PTR -12[rbp]
	mov	DWORD PTR -8[rbp], eax
	mov	eax, DWORD PTR -8[rbp]
	add	eax, 2
	mov	DWORD PTR -4[rbp], eax
	mov	eax, DWORD PTR -16[rbp]
	cdq
	idiv	DWORD PTR -20[rbp]
	mov	DWORD PTR -8[rbp], eax
	mov	eax, DWORD PTR -8[rbp]
	sub	DWORD PTR -4[rbp], eax
	mov	eax, DWORD PTR -4[rbp]
	mov	r8d, eax
	lea	rdx, .LC0[rip]   # 使用RIP，是因为RIP是保存了当前的程序的指针，加上偏移，就可以索引到静态变量的空间
	lea	rcx, .LC1[rip] 
	call	printf
	mov	eax, 0         # eax复位
	leave              # 将栈指针指向帧指针,pop备份的指针到rbp = pop rbp 
	ret                # return 
	.ident	"GCC: (Rev2, Built by MSYS2 project) 10.3.0"
	.def	__mingw_vfprintf;	.scl	2;	.type	32;	.endef
