	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	user_stack: .zero 512

.text
	.global main
	.equ N, 40

main:
	ldr sp, =user_stack
	add sp, sp, #512
	ldr r0, =N
	movs r8, #0
	push {r0}
	bl stair_climb1
	pop {r8}
end:
	b end

stair_climb1:
	pop {r0}
	push {lr}

	// check if neg
	cmp r0, #0
	blt neg

	// check if eqaul 0
	cmp r0, #0
	beq init

	// check if equal 1
	cmp r0, #1
	beq init


	sub r1, r0, #1
	sub r2, r0,#2
	push {r1, r2}
	bl stair_climb1
	bl stair_climb2
	pop {r3, r4}
	add r3, r3, r4
	pop {lr}
	push {r3}
	bx lr

stair_climb2:
	pop {r3}
	pop {r0}
	push {r3}
	push {lr}

	// check if neg
	cmp r0, #0
	blt neg

	// check if eqaul 0
	cmp r0, #0
	beq init

	// check if equal 1
	cmp r0, #1
	beq init


	sub r1, r0, #1
	sub r2, r0, #2
	push {r1, r2}
	bl stair_climb1
	bl stair_climb2
	pop {r3, r4}
	add r3, r3, r4
	pop {lr}
	push {r3}
	bx lr

init:
	movs r5, #1
	pop {lr}
	push {r5}
	bx lr

neg:
	movs r8, #-1
	b end
