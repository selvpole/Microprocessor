	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	leds: .byte 0
.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OTYPE, 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_IDR, 0x48000410	// PB3, PB4, PB5, PB6
	.equ GPIOA_MODER, 0x48000000
	.equ GPIOA_OSPEEDR, 0x48000008
	.equ GPIOA_ODR, 0x48000014	// PA6, PA7, PA8, PA9
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_IDR, 0x48000810	// PC13
	.equ X, 1
	.equ Y, 1000000
	.equ password, 0xF	// 0110

main:
	bl GPIO_init
loop:
	ldr r9, =X
L1:	ldr r10, =Y
L2:	subs r10, #1
	bne L2
	subs r9, #1
	bne L1	// 3 cycle
DT: ldr r4, =GPIOB_IDR
	ldr r4, [r4]
	lsr r3, r4, #6
	and r3, #1
	lsr r2, r4, #5
	and r2, #1
	lsr r1, r4, #4
	and r1, #1
	lsr r0, r4, #3
	and r0, #1

	orr	r4, r0, r1
	and r5, r5, r2
	eor r6, r1, r0
	orr	r3, r3, r5	//
	lsl r3, r3, #8
	eor r2, r4, r2	//
	lsl r2, r2, #7
	eor r1, r7, #1	//
	lsl r1, r1, #6
	eor r0, r0, #1	//
	lsl r0, r0, #5
	add r0, r1
	add r0, r2
	add r0, r3
	// display
	ldr r2, =GPIOA_ODR
	strh r0, [r2]

	b loop

// TODO: Initial LED GPIO pins as output
GPIO_init:
	// Enable PA, PB, PC in AHB2 clock
	ldr r0, =RCC_AHB2ENR
	movs r1, 0x3
	str r1, [r0]

	/* Configure PB */
	// configure PB3, PB4, PB5, PB6 as input pins
	ldr r0, =GPIOB_MODER
	ldr r1, [r0]
	and r1, 0xFFFFC03F
	movs r2, 0x0000
	orrs r1, r1, r2
	str r1, [r0]

	// configure OtypeR(default as Push-Pull), no need to set

	// configure Ospeed as high speed mode(10)
	ldr r0, =GPIOB_OSPEEDR
	movs r1, 0x2A80
	strh r1, [r0]

	/* Configure PA */
	// configure PA5, PA6, PA7, PA8 as ouput pins
	ldr r0, =GPIOA_MODER
	ldr r1, [r0]
	and r1, 0xFFFC03FF
	movs r2, 0x5700
	add r2, 0xB0000
	orrs r1, r1, r2
	str r1, [r0]

	// configure Ospeed as high speed mode(10)
	ldr r0, =GPIOA_OSPEEDR
	movs r1, 0x5700
	add r2, 0xB0000
	strh r1, [r0]


	bx lr
