	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	result: .word 0
	user_stack:	.zero 256
.text
	.global main
	m: .word 0x4E	// 78
	n: .word 0x34	// 52
main:
	ldr sp, =user_stack
	add sp, sp, #256
	ldr r0, =m
	ldr r0, [r0]
	ldr r1, =n
	ldr r1, [r1]
	push {r0, r1}
	bl gcd
	ldr r0, =result
	str r5, [r0]
	b program_end
gcd:
	pop {r0, r1}
	push {lr}
	cmp r0, r1
	beq equal
	bgt greater
	blt less
equal:
	pop {lr}
	movs r5, r0
	bx lr
greater:
	sub r0, r0, r1
	push {r1, r0}
	bl gcd
	pop {lr}
	bx lr
less:
	sub r1, r1, r0
	push {r0, r1}
	bl gcd
	pop {lr}
	bx lr
program_end:
	b program_end
