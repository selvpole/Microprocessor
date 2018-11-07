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

	// "+" = 0x2B
	// "-" = 0x2D
	// " " = 0x20
	// "/0" = 0x00
main:
	ldr r0, =postfix_expr
	ldr sp, =user_stack
	add sp, sp, #128
	add r1, r0, #-1	// r1 = counter
	movs r4, #0
	movs r8, #10
postfix:
	add r1, r1, #1
	ldrb r3, [r1] 	// r3 = str[i]
	cmp r3, 0x2B	// if(str[i] == "+") => plus
	beq add
	cmp r3, 0x2D	// if(str[i] == "-") => minus
	beq check
	cmp r3, 0x20	// if(str[i] == " ") => push num
	beq push
	b atoi			// else atoi
check:
	add r2, r1, #1	// r2 = i+1
	ldrb r2, [r2]	// r2 = str[i+1]
	cmp r2, 0x20	// if(str[i+1] == " ")
	beq sub			//	str[i] = sub
	b neg			// else	str[i] = neg
add:
	pop {r5, r6}
	add r5, r5, r6
	push {r5}
	movs r5, #0
	add r1, r1, #1
	ldrb r2, [r1]	// r2 = str[i+1]
	cmp r2, 0x20	// if(str[i+1] == " ")
	beq postfix
	b store
sub:
	pop {r5, r6}
	sub r5, r6, r5
	push {r5}
	movs r5, #0
	add r1, r1, #1
	ldrb r2, [r1]	// r2 = str[i+1]
	cmp r2, 0x20	// if(str[i+1] == " ")
	beq postfix
	b store
atoi:
	mul r5, r5, r8	// num = num*10
	sub r3, r3, #48
	add r5, r5, r3
	b postfix
neg:
	movs r5, #-1
	sub r2, r2, 48
	mul r5, r5, r2
	add r1, r1, #1
	b postfix
push:
	push {r5}
	movs r5, #0
	b postfix
store:
	ldr r0, =expr_result
	pop {r5}
	str r5, [r0]
program_end:
	B program_end
