	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	leds: .byte 0
.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400 // 3 4 5 6
	.equ GPIOB_OSPEEDR, 0x48000408 // 10 => high speed
	.equ GPIOB_IDR, 0x48000410	// PB3, PB4, PB5, PB6 INPUT
	.equ GPIOA_MODER, 0x48000000 // 6 7 8 9
	.equ GPIOA_OTYPER, 0x48000004 // push-pull => 0
	.equ GPIOA_OSPEEDR, 0x48000008 // 10 => high speed
	.equ GPIOA_PUPDR, 0x4800000C // pull up => 01
	.equ GPIOA_ODR, 0x48000014 // PA6, PA7, PA8, PA9 OUTPUT
	.equ X, 1
	.equ Y, 400000

main:
	bl GPIO_INIT
	movs r4, #0	// count
	movs r3, #0	// light up
loop:
	add r4, #1
	cmp r4, #4
	it eq
	moveq r3, #1
	cmp r4, #5
	it eq
	bleq reset_cnt

	bl delay
	b loop

reset_cnt:
	movs r4, #0
	movs r3, #0
	bx lr

GPIO_INIT:
	/* AHB2 */
	ldr r0, =RCC_AHB2ENR
	movs r1, 0x3
	str r1, [r0]

	/* GPIOB */
	// MODER 3 4 5 6, input => 00
	ldr r0, =GPIOB_MODER
	ldr r1, [r0]
	and r1, 0xFFFFC03F
	str r1, [r0]

	// OSPEEDR
	ldr r0, =GPIOB_OSPEEDR
	ldr r1, [r0]
	and r1, 0xFFFFC03F
	orr r1, 0x00002A80
	str r1, [r0]

	/* GPIOA */
	// MODER 6 7 8 9, output => 01
	ldr r0, =GPIOA_MODER
	ldr r1, [r0]
	and r1, 0xFFF00FFF
	orr r1, 0x00055000
	str r1, [r0]

	// OSPEEDR
	ldr r0, =GPIOA_OSPEEDR
	ldr r1, [r0]
	and r1, 0xFFF00FFF
	orr r1, 0x000AA000
	str r1, [r0]

	// PUPDR
	ldr r0, =GPIOA_PUPDR
	ldr r1, [r0]
	and r1, 0xFFF00FFF
	orr r1, 0x00055000
	str r1, [r0]

	bx lr

delay:	// 1 sec
	ldr r1, =X
L1:
	ldr r2, =Y
L2:
	/* Detect */
	ldr r5, =GPIOB_IDR
	ldr r6, [r5]
	lsl r6, #3
	sub r6, 0b11000000
	cmp r3, #1
	it eq
	moveq r6, #0
	ldr r5, =GPIOA_ODR
	str r6, [r5]

	subs r2, #1
	bne L2
	subs r1, #1
	bne L1

	bx lr
