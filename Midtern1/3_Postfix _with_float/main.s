	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	operand_stack_buttom: .zero 12
	operand_stack_pointer: .float 3.14, 2.7, 9.8
	operators: .asciz "+*"

.text
	.global main

main:
	bl init
	bl push_val
	ldr r0, =operators
	mov r1, #0
	b cal
push_val:
	ldr sp, =operand_stack_buttom
	add sp, #12
	ldr r0, =operand_stack_pointer
	vldr s0, [r0]
	vldr s1, [r0,#4]
	vldr s2, [r0,#8]
	vpush {s0,s1,s2}
	add r1, r0, #-1
	bx lr
cal:
	vpop {s0, s1}
	ldrb r2, [r0,r1]
	cmp r2, 0x2B // +
	beq add
	cmp r2, 0x2A // *
	beq mul
check:
	add r1, #1
	cmp r1, #2
	bne cal
	vpop {s0}
	b program_end
add:
	vadd.f32 s0, s1
	vpush {s0}
	b check
mul:
	vmul.f32 s0, s1
	vpush {s0}
	b check
program_end:
	b program_end
init:
	LDR.W R0, =0xE000ED88
	//Read CPACR
	LDR R1, [R0]
	//Set bits 20-23 to enable CP10 and CP11 coprocessors
	ORR R1, R1, #(0xF << 20)
	//Write back the modified value to the CPACR
	STR R1, [R0]
	// wait for store to complete
	DSB
	// reset pipeline now the FPU is enabled
	ISB
	bx lr
