	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	user_stack: .zero 512
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
	.equ X, 10
	.equ Y, 100000

main:
	ldr sp, =user_stack
	add sp, #512
	bl GPIO_init
	bl max7219_init
	mov r9, #1 // start
fib_init:
	mov r3, #0 // r3 = counter
	mov r4, #0 // fib 1
	mov r5, #1 // fib 2
	bl Display_Val
loop:
	bl debounce
	b loop

// display r4
Display_Val:
	push {r4,r5,lr}
	ldr r6, =LIMIT
	cmp r4, r6
	bgt overflow
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
	popeq {r4, r5, lr}
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
	push {r4-r11,lr}
	ldr r0, =GPIOC_IDR
	ldr r1, =X
L1:
	ldr r2, =Y
L2:
	subs r2, #1
	bne L2
	// check button on
	ldr r6, [r0]
	lsrs r6, #13
	beq press_button
	// check end
	subs r1, #1
	bne L1
	// no detect
	pop {r4-r11,lr}
	bx lr
press_button:
	ldr r1, =X
l1:
	ldr r2, =Y
l2:
	subs r2, #1
	bne l2
	// check button off
	ldr r6, [r0]
	lsrs r6, #13
	bne fib
	// check end
	subs r1, #1
	bne l1
	mov r4, 0x0
	mov r5, 0x0
	bl Display_Val

	// press out time
out_time:
	// check button off
	ldr r6, [r0]
	lsrs r6, #13
	beq out_time
	// check end
	pop {r4-r11,lr}
	b fib_init
fib:
	pop {r4-r11}
	mov r6, r4
	mov r4, r5
	add r5, r6
	bl Display_Val
	pop {lr}
	bx lr


MAX7219Send:
	push {r0-r11}
	lsl r0, #8
	add r0, r1 // r0 = XXXX 0000(address) 00000000(data)
	ldr r1, =GPIOB_MODER
	ldr r2, =LOAD
	ldr r3, =DATA
	ldr r4, =CLK
	ldr r5, =GPIOB_BSRR
	ldr r6, =GPIOB_BRR
	mov r7, #16 // r7 = i
max7219send_loop:
	mov r8, #1
	sub r9, r7, #1
	lsl r8, r8, r9 // r8 = mask
	str r4, [r6] // CLK = 0
	tst r0, r8
	beq bit_not_set
	str r3, [r5] // DATA = 1
	b if_done
bit_not_set:
	str r3, [r6] // DATA = 0
if_done:
	str r4, [r5] // CLK = 1
	subs r7, #1 // i--
	bgt max7219send_loop // i > 0
	str r2, [r6] // Load = 0
	str r2, [r5] // Load = 1
	pop {r0-r11}
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
