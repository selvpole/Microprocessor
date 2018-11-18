	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	operand_stack_buttom: .zero 12
	operand_stack_pointer: .float 3.14, 2.7, 9.8
	operators: .asciz "+*"

.text
	.global main

main:
	ldr sp, =operand_stack_buttom
	add sp, #12
	ldr r0, =operand_stack_pointer
	vldr.F32 s1, =operand_stack_pointer
	vpush {s1}
	add r0, #1
	vldr s0, [r0]
	vpush {s0}
	add r0, #1
	vldr s0, [r0]
	vpush {s0}
	add r1, r0, #-1
	ldr r0, =operators

postfix:

