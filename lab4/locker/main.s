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
	.equ GPIOA_ODR, 0x48000014	// PA6, PA7
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_IDR, 0x48000810	// PC13
	.equ X, 10000
	.equ Y, 24
	.equ password, 0xF	// 0110

main:
	bl GPIO_init
	movs r0, #0	// count
	movs r6, #0 // button
	movs r8, #0 // run_loop y/n
	ldr r7, =password	// r7 = password
	lsl r7, r7, #3	// PB6 PB5 PB4 PB3
loop:
	// TODO: Write the display pattern into leds variable
	bl debounce
	b loop

// TODO: Initial LED GPIO pins as output
GPIO_init:
	// Enable PA, PB, PC in AHB2 clock
	ldr r0, =RCC_AHB2ENR
	movs r1, 0x7
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
	// configure PA6, PA7 as input pins
	ldr r0, =GPIOA_MODER
	ldr r1, [r0]
	and r1, 0xFFFF0FFF
	movs r2, 0x5000
	orrs r1, r1, r2
	str r1, [r0]

	// configure Ospeed as high speed mode(10)
	ldr r0, =GPIOA_OSPEEDR
	movs r1, 0x5000
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

check_pass:
	// send 1 to the DIP switch
	ldr r2, =GPIOA_ODR	// PA6 send 1 to switch
	movs r1, #1
	lsl r1, #6
	strh r1, [r2]
	bl debounce	// delay 1 sec
	ldr r2, =GPIOB_IDR	// input from DIP switch
	ldr r2, [r2]
	cmp r2, r7
	ite eq
	moveq r9, #3	// r9 for counting the blink time
	movne r9, #1

// Display LED
displayLED:
	ldr r2, =GPIOA_ODR
	movs r1, #1
	lsl r1, #7	// PA7 output 1 to LEDs
	strh r1, [r2]
	bl debounce	// count 1 sec
	movs r1, #0
	strh r1, [r2]	// RB8 output 0
	bl debounce	// count 1 sec
	subs r9, r9, #1
	bne displayLED
	b loop

// TODO: write a delay 1 sec function
debounce:	// 1cycle=0.25uS => 1sec=4*10^6cycle
	// cycles : 2 + (2 + (1 + 3)*Y - 1 + (1 + 3))*X - 1 + (button trigger cycle) + 1
	ldr r3, =X	// 2 cycle, X=10000
L1:	ldr r4, =Y	// 2 cycle, Y=24
L2:	subs r4, #1	// 1 cycle
	bne L2	// 3 cycle
	subs r3, #1	// 1 cycle
	bne L1	// 3 cycle
BT: // button trigger
	ldr r5, =GPIOC_IDR
	ldr r5, [r5]
	lsrs r5, r5, #13	// if GPIOC13 = 1 (button is pushed)
	beq check_pass
	bx lr		// 1 cycle
