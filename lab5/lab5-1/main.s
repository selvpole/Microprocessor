	.syntax unified
	.cpu cortex-m4
	.thumb
.data       // 0     1     2     3     4     5     6     7     8     9     A     b     C     d     E     F
	arr: .byte 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47
	user_stack: .zero 512
.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR,0x4800040C
	.equ GPIOB_BSRR, 0x48000418 // set
	.equ GPIOB_BRR, 0x48000428 // reset
	.equ DATA, 0b1000 // PB3
	.equ LOAD, 0b10000 // PB4
	.equ CLK, 0b100000 // PB5
	.equ DECODE_MODE, 0x09 // 0xX9
	.equ DISPLAY_TEST, 0x0F // 0xXF
	.equ SCAN_LIMIT, 0x0B // 0xXB
	.equ INTENSITY, 0x0A // 0xXA
	.equ SHUTDOWN, 0x0C // 0xXC
	.equ X, 1
	.equ Y, 1000000

main:
	ldr sp, =user_stack
	add sp, #512
	BL   GPIO_init
	BL   max7219_init
	ldr r9, =arr
	mov r2, #0
Display0toF: //TODO: Display 0 to F at first digit on 7-SEG LED. Display one per second. BX LR
	mov r0, #1
	ldrb r1, [r9, r2]
	bl MAX7219Send
	bl Delay
	add r2, #1
	cmp r2, #16
	bne Display0toF
	mov r2, #0
	b Display0toF
GPIO_init: //TODO: Initialize three GPIO pins as output for max7219 DIN, CS and CLK BX LR
 	/* AHB2 */
	ldr r0, =RCC_AHB2ENR
	movs r1, #2
	str r1, [r0]

	/* GPIOA */
	// MODER output 3 4 5
	ldr r0, =GPIOB_MODER
	ldr r1, [r0]
	and r1, 0xFFFFF03F
	movs r2, 0x540
	orr r1, r2
	str r1, [r0]

	// SPEEDR high speed 10
	ldr r0, =GPIOB_OSPEEDR
	ldr r1, [r0]
	and r1, 0xFFFFF03F
	movs r2, 0xA80
	orr r1, r2
	str r1, [r0]

	// PUPDR pull up 01
	ldr r0, =GPIOB_PUPDR
	ldr r1, [r0]
	and r1, 0xFFFFF03F
	movs r2, 0x540
	orr r1, r2
	str r1, [r0]
	bx lr
MAX7219Send:    //input parameter: r0 is ADDRESS , r1 is DATA //TODO: Use this function to send a message to max7219 BX LR
	push {r0-r9}
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
	pop {r0-r9}
	bx lr
max7219_init: //TODO: Initialize max7219 registers BX LR
	push {lr}
	ldr r0, =DECODE_MODE
	mov r1, 0x0 // 0x00 => no decode for digit 7-0
	bl MAX7219Send
	ldr r0, =DISPLAY_TEST
	mov r1, 0x0 // 0 => normal operation
	bl MAX7219Send
	ldr r0, =SCAN_LIMIT
	mov r1, 0x0 // 0xX0 => display digit 0 only
	bl MAX7219Send
	ldr r0, =INTENSITY
	mov r1, 0x0 // 0xX0 => 1/32
	bl MAX7219Send
	ldr r0, =SHUTDOWN
	mov r1, 0x1 // 1 => normal operation
	bl MAX7219Send
	pop {lr}
	bx lr
Delay: //TODO: Write a delay 1sec function BX LR
	push {r1, r2}
	ldr r1, =X
L1:
	ldr r2, =Y
L2:
	subs r2, #1
	bne L2
	subs r1, #1
	bne L1
	pop {r1, r2}
	bx lr
