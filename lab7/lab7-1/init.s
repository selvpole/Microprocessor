	.syntax unified
	.cpu cortex-m4
	.thumb
.text
	.global GPIO_init
	.global delay_1s
	.equ GPIOA_MODER, 0x48000000
	.equ GPIOA_OSPEEDR, 0x48000008
	.equ GPIOA_PUPDR, 0x4800000C
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_OSPEEDR, 0x48000808 // 13
	.equ X, 1000000
	.equ Y, 1

GPIO_init:
	/* GPIOA */
	// MODER 5 ouput: 01
	ldr r0, =GPIOA_MODER
	ldr r1, [r0]
	and r1, 0xFFFFF3FF
	orr r1, 0x00000400
	str r1, [r0]

	// SPEEDR high speed 10
	ldr r0, =GPIOA_OSPEEDR
	ldr r1, [r0]
	and r1, 0xFFFFF3FF
	orr r1, 0x00000800
	str r1, [r0]

	// PUPDR pull up: 01
	ldr r0 , =GPIOA_PUPDR
	ldr r1, [r0]
	and r1, 0xFFFFF3FF
	orr r1, 0x00000400
	str r1, [r0]

	/* GPIOC */
	// MODER 13 input: 00
	ldr r0, =GPIOC_MODER
	ldr r1, [r0]
	and r1, 0xF3FFFFFF
	str r1, [r0]

	// SPEEDR high speed 10
	ldr r0, =GPIOC_OSPEEDR
	ldr r1, [r0]
	and r1, 0xF3FFFFFF
	movs r2, 0x8000000
	orr r1, r2
	str r1, [r0]
	bx lr

delay_1s:
	ldr r3, =Y
L1:
	ldr r4, =X
L2:
	subs r4, #1
	bne L2
	subs r3, #1
	bne L1
	bx lr
