	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	user_stack: .zero 128
	expr_result: .word 0
.text
	.global main
	postfix_expr:	.asciz "-100 10 20 + - 10 +"
	.align 0x2

main:
	ldr r0, =postfix_expr
	ldr sp, =user_stack
	add sp, #128
	add r1, r0, #-1
	movs r3, #0
	movs r8, #10
postfix:
	add r1, r1, #1
	ldrb r2, [r1]
	cmp r2, 0x2b	// +
	beq plus
	cmp r2, 0x2d	// -
	beq check
	cmp r2, 0x20	// " "
	beq push
atoi:
	mul r3, r8
	sub r2, r2, #48
	add r3, r3, r2
	b postfix
plus:
	pop {r5, r6}
	add r5, r5, r6
	push {r5}
	add r1, r1, #1
	ldrb r2, [r1]
	cmp r2, 0x20
	beq postfix
	b store
	b postfix
check:
	add r1, r1, #1
	ldrb r2, [r1]
	cmp r2, 0x20
	beq sub
	cmp r2, 0x00
	beq sub
neg:
	movs r3, #-1
	sub r2, r2, #48
	mul r2, r3
	b postfix

sub:
	pop {r5, r6}
	sub r5, r5, r6
	push {r5}
	cmp r2, 0x20
	beq postfix
	b store
push:
	push {r3}
	movs r3, #0
	b postfix

store:
	pop {r5}
	ldr r0, =expr_result
	str r5, [r0]
L:
	b L


