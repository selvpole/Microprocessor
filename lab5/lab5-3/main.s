	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	user_stack: .zero 512
	f1: .word 0
	f2: .word 1
.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR,0x4800040C
	.equ GPIOB_BSRR, 0x48000418 // set
	.equ GPIOB_BRR, 0x48000428 // reset
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_OSPEEDR, 0x48000808
	.equ GPIOC_IDR, 0x48000810
	.equ DATA, 0b1000 // PB3
	.equ LOAD, 0b10000 // PB4
	.equ CLK, 0b100000 // PB5
	.equ DECODE_MODE, 0x09 // 0xX9
	.equ DISPLAY_TEST, 0x0F // 0xXF
	.equ SCAN_LIMIT, 0x0B // 0xXB
	.equ INTENSITY, 0x0A // 0xXA
	.equ SHUTDOWN, 0x0C // 0xXC
	.equ LIMIT, 99999999
	.equ X, 100
	.equ Y, 10000

main:
	ldr sp, =user_stack
	add sp, #512
	bl GPIO_init
	bl max7219_init
	mov r9, #1 // start
fib_init:
	ldr r1, =f1
	ldr r2, =f2
	mov r3, #0 // r3 = counter
	mov r4, #0 // fib 1
	mov r5, #1 // fib 2
loop:
	bl debounce
	b loop

// display r4
Display_Val:
	ldr r6, =LIMIT
	cmp r4, r6
	bgt overflow
	push {r4, r5}
	mov r7, #0
	mov r8, #10
set_num:
	// address
	add r0, r7, #1
	// display value
	udiv r5, r4, r8 // div
	mul r5, r8
	sub r1, r4, r5
	bl MAX7219Send
	add r7, r7, #1
	udiv r4, r8
	cmp r4, #0
	bne set_num
add_space:
	cmp r7, #8
	itt eq
	popeq {r4, r5}
	bxeq lr

	add r0, r7, #1
	mov r1, 0xF
	bl MAX7219Send
	add r7, r7, #1
	b add_space
overflow: // print -1
	mov r0, 0x1 // 1
	mov r1, 0x1
	bl MAX7219Send
	mov r0, 0x2
	mov r1, 0xA // -
	bl MAX7219Send
	mov r7, #2
	b add_space

// debounce
debounce:
	push {lr}
	ldr r4, =GPIOC_IDR
	ldr r1, =X
L1:
	ldr r2, =Y
L2:
	subs r2, #1
	bne L2
	// check button on
	ldr r5, [r4]
	lsrs r5, #13
	beq press_button
	// check end
	subs r1, #1
	bne L1
	// no detect
	pop {lr}
	bx lr
press_button:
	ldr r1, =X
l1:
	ldr r2, =Y
l2:
	subs r2, #1
	bne l2
	// check button off
	ldr r5, [r4]
	lsrs r5, #13
	beq fib
	// check end
	subs r1, #1
	bne l1
	mov r4, 0x0
	mov r5, 0x0
	bl Display_Val

	// press out time
out_time:
	// check button off
	ldr r5, [r4]
	lsrs r5, #13
	beq out_time
	// check end
	b fib_init
fib:
	mov r6, r4
	mov r4, r5
	add r5, r4, r6
	bl Display_Val
	pop {lr}
	bx lr


MAX7219Send:
	push {r4-r9}
	lsl r0, #8
	add r0, r1
	ldr r4, =DATA
	ldr r5, =LOAD
	ldr r6, =CLK
	ldr r7, =GPIOB_BSRR
	ldr r8, =GPIOB_BRR
	mov r7, #16
MAX7219Send_loop:
	sub r7, #1
	mov r9, #1
	lsl r9, r7
	tst r9, r0
	beq set_zero
	str r4, [r7] // Data = 1
	b after_set
set_zero:
	str r4, [r8] // Data = 0
after_set:
	str r6, [r8] // CLK = 0
	str r6, [r7] // CLK = 1
	cmp r7, #0
	bne MAX7219Send_loop
	str r5, [r8] // Load = 0
	ldr r5, [r7] // Load = 1
	pop {r4-r9}
	bx lr
GPIO_init:
	// enable PB and PC
	ldr r0, =RCC_AHB2ENR
	mov r1, #6
	str r1, [r0]

	/* GPIOB */
	// MODER: set PB3, PB4, PB5 as output mode
	ldr r0, =GPIOB_MODER
	ldr r1, [r0]
	and r1, 0xFFFFF03F
	mov r2, 0x540
	orr r1, r2
	str r1, [r0]

	// OSPEEDR
	ldr r0, =GPIOB_OSPEEDR
	ldr r1, [r0]
	and r1, 0xFFFFF03F
	mov r2, 0xA80
	orr r1, r2
	str r1, [r0]

	// PUPDR
	ldr r0, =GPIOB_PUPDR
	ldr r1, [r0]
	and r1, 0xFFFFF03F
	mov r2, 0x540
	orr r1, r2
	str r1, [r0]

	/* GPIOC */
	// MODER
	ldr r0, =GPIOC_MODER
	ldr r1, [r0]
	and r1, 0xF3FFFFFF
	str r1, [r0]

	// OSPEEDR
	ldr r0, =GPIOB_OSPEEDR
	ldr r1, [r0]
	and r1, 0xF3FFFFFF
	mov r2, 0x08000000
	orr r1, r2
	str r1, [r0]
	bx lr
max7219_init:
	push {lr}
	ldr r0, =DECODE_MODE
	mov r1, 0xFF // B decoder for digits 7-0
	bl MAX7219Send
	ldr r0, =DISPLAY_TEST
	mov r1, 0x0 // normal operation
	bl MAX7219Send
	ldr r0, =SCAN_LIMIT
	mov r1, 0x7 // scan all digits
	bl MAX7219Send
	ldr r0, =INTENSITY
	mov r1, 0x5
	bl MAX7219Send
	ldr r0, =SHUTDOWN
	mov r1, 0x1 // normal operation
	bl MAX7219Send
	pop {lr}
	bx lr

