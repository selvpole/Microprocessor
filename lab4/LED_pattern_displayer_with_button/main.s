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
	.equ GPIOB_ODR, 0x48000414
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_IDR, 0x48000810
	.equ X, 10000
	.equ Y, 24

main:
	bl GPIO_init
	movs r0, #0	// count
	movs r6, #0 // button
	movs r8, #0 // run_loop y/n
	ldr r1, =GPIOB_ODR
loop:
	// TODO: Write the display pattern into leds variable
	cmp r0, #0	// r0 = counter
	IT eq
	moveq r1, 0x1	// 0001

	cmp r0, #1
	IT eq
	moveq r1, 0x3	// 0011

	cmp r0, #2
	IT eq
	moveq r1, 0x6	// 0110

	cmp r0, #3
	IT eq
	moveq r1, 0xC	// 1100

	cmp r0, #4
	IT eq
	moveq r1, 0x8	// 1000

	cmp r0, #5
	IT eq
	moveq r1, 0xC	// 1100

	cmp r0, #6
	IT eq
	moveq r1, 0x6	// 0110

	cmp r0, #7
	IT eq
	moveq r1, 0x3	// 0011

	add r0, r0, #1	// count++

	cmp r0, #8	// count reset
	IT eq
	moveq r0, #0

	bl displayLED
	bl delay
	b loop

// TODO: Initial LED GPIO pins as output
GPIO_init:
	// Enable PB, PC in AHB2 clock
	ldr r0, =RCC_AHB2ENR
	movs r1, 0x6
	str r1, [r0]

	/* Configure PB */
	// configure PB3, PB4, PB5, PB6 as output pins
	ldr r0, =GPIOB_MODER
	ldr r1, [r0]
	and r1, 0xFFFFC03F
	movs r2, 0x1540
	orrs r1, r1, r2
	str r1, [r0]

	// configure OtypeR(default as Push-Pull), no need to set

	// configure Ospeed as high speed mode(10)
	ldr r0, =GPIOB_OSPEEDR
	movs r1, 0x2A80
	strh r1, [r0]

	/* Configure PC */
	// configure PC13(user button) as input pins
	ldr r0, =GPIOC_MODER
	ldr r1, [r0]
	and r1, 0xF3FFFFFF
	str r1, [r0]

	// configure PC13_PUPDR as pull-up mode
	ldr r0, =GPIOC_PUPDR
	movs r1, 0xF7FFFFFF
	str r1, [r0]

	bx lr

// Display LED by leds
displayLED:
	ldr r2, =GPIOB_ODR
	lsl r1, #3
	strh r1, [r2]
	bx lr

// TODO: write a delay 1 sec function
delay:	// 1cycle=0.25uS => 1sec=4*10^6cycle
	// cycles : 2 + (2 + (1 + 3)*Y - 1 + (1 + 3))*X - 1 + (button trigger cycle) + 1
	ldr r3, =X	// 2 cycle, X=10000
L1:	ldr r4, =Y	// 2 cycle, Y=24
L2:	subs r4, #1	// 1 cycle
	bne L2	// 3 cycle
	subs r3, #1	// 1 cycle
	bne L1	// 3 cycle
	// button trigger
	ldr r5, =GPIOC_IDR
	ldr r5, [r5]
	lsrs r5, r5, #13	// if GPIOC13 = 1 (button is pushed)
	beq switch_stat
	cmp r8, #1
	beq delay
	bx lr		// 1 cycle

switch_stat:
	eor r8, r8, #1
	b delay
