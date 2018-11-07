	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global main
	.equ N, 20

main:
	ldr r0, =N
	cmp r0, #100
	bgt overN
	cmp r0, #1
	blt overN
	movs r1, #0
	movs r2, #1
	bl fib

L:
	b L

fib:
	adds r4, r1, r2
	bvs overflow
	movs r1, r2
	movs r2, r4
	sub r0, #1
	cmp r0, #0
	beq L
	b fib

overN:
	movs r4, #-1
	b L

overflow:
	movs r4, #-2
	b L
