	.text
	.text
	.globl	fred
	.type	fred, %function
fred:
	push	{fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	mov	r4, #20
	mov	r0, r4
	b	L1
L1:
	sub	sp, fp, #4
	pop	{fp, pc}
	.align	2
	.comm	result,4,4 		# int variable
	.text
	.globl	main
	.type	main, %function
main:
	push	{fp, lr}		 # Save the frame and stack pointers
	add	fp, sp, #4			 # Add sp+4 to the frame pointer
	sub	sp, sp, #8			 # Lower the stack pointer by 8
	str	r0, [fp, #-8]	     # Save the argument as a local var?.	 fp -=8 then ro = fp

	mov	r5, #10
	mov	r0, r5
	bl	printint      # print 10

	mov	r5, r0
	mov	r4, #15
	mov	r0, r4
	bl	fred

	mov	r4, r0
	ldr	r3, .L2+0
	str	r4, [r3]  # load result as '.L2+0'. store r4 to [r3]

	ldr	r3, .L2+0
	ldr	r4, [r3]
	mov	r0, r4   # result = r4 = 20

	bl	printint  # print result
	mov	r4, r0      
	mov	r4, #15
	mov	r0, r4  # r0 = 15
	bl	fred    # fred(15)

	mov	r4, r0
	mov	r5, #10
	add	r5, r4, r5  # fred(15) + 10 = 20 + 10 = 30
	mov	r0, r5
	bl	printint    # print 30
	mov	r5, r0
	mov	r4, #0
	mov	r0, r4   # r0 = r4 =0
	b	L2       # exit func
L2:
	sub	sp, fp, #4
	pop	{fp, pc}  # Pop the frame and stack pointers
	.align	2
.L2:
	.word result
.L3:
# B或BL指令引起处理器转移到“子程序名”处开始执行
# https://blog.csdn.net/weixin_34242819/article/details/91598293