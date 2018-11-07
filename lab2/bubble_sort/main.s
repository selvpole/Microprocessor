	.syntax unified
	.cpu cortex-m4

.data
	arr1: .byte 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
	arr2: .byte 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC
.text
	.global main
	.equ len, 8

main:
	ldr r0, =arr1
	bl sort
	ldr r0, =arr2
	bl sort
L: b L

sort:
	movs r7, lr
	movs r4, r0
	add r4, r4, len
loop1:
	sub r4, r4, #1
	movs r3, r0
	cmp r4, r0
	bne loop2
	bx r7
loop2:
	ldrb r1, [r3]
	add r3, r3, #1
	ldrb r2, [r3]
	cmp r1, r2
	bgt swap
	cmp r3, r4
	beq loop1
	b loop2
swap:
	strb r1, [r3]
	sub r3, r3, #1
	strb r2, [r3]
	add r3, r3, #1
	cmp r3, r4
	beq loop1
	b loop2

