#include "../inc/stm32l476xx.h"
#include "keypad.h"

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(int, int);

TIM_TypeDef	*timer = TIM3;
int counter;
int clock_run = 0;

void GPIO_init_AF() {
	// TODO: Initial GPIO pin as alternate function for buzzer.
	// PC7 as timer
	RCC->AHB2ENR |= 0x4;
	GPIOC->MODER &= ~GPIO_MODER_MODE7;
	GPIOC->MODER |= GPIO_MODER_MODE7_1;  // set to the alternative function mode
	GPIOC->AFR[0] |= GPIO_AFRL_AFSEL7_1; // (TIM3_CH2)
}

void Timer_init(TIM_TypeDef *timer) {
	// TODO: Initialize timer
	// Use TIM3
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	timer->ARR = (uint32_t)99;
	timer->PSC = (uint32_t)152;
	timer->EGR = TIM_EGR_UG;
}

void PWM_channel_init(TIM_TypeDef *timer) {
	// TODO: Initialize timer PWM channel
	// Use CH1
	// duty_cycle = 50
	timer->CCR2 = 50;

	// set bit[6:4] = 011, wave flip if CNT == CCR1
	// timer->CCMR1 &= 0xFF8F;
	timer->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0;

	timer->CR1 |= TIM_CR1_ARPE;

	// Capture Compare 2 Output Enable
	timer->CCER |= TIM_CCER_CC2E;

	timer->EGR = TIM_EGR_UG;
}

void SystemClock_Config() {
	SysTick->CTRL |= 0x6;
	SysTick->LOAD = 3999999;
	SysTick->CTRL |= 0x7;
}

void EXTI_config() {
	// enable
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// external interrupt to GPIOs
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	// set up the interrupt mask register EXTI_IMR1
	EXTI->IMR1 = 0;
	EXTI->IMR1 |= EXTI_IMR1_IM13;

	// set the rising-triggered interrupt
	EXTI->RTSR1 |= EXTI_RTSR1_RT13;
}

void NVIC_config() {
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 1);
	NVIC_SetPriority(SysTick_IRQn, 0);
}

int display(int data)
{
	int i=0;
	int digit=8;
	if(data==0)
		max7219_send(1, 0);
	else{
		for(i=1;i<=8;i++)
		{
			digit = data%10;
			data/=10;
			if(digit == 0 && data == 0)
				digit = 15; // space
			max7219_send(i, digit);
		}
	}
	return 0;
}

void SysTick_Handler(void) {
	// counter plus 1 every second
	display(counter);
	counter--;
	if (counter == 0) {
		display(0);
		timer->CR1 |= TIM_CR1_CEN;
		SysTick->CTRL |= ~0x7; // close SysTick
	}
}

void EXTI15_10_IRQHandler(void) {
	timer->CR1 &= ~TIM_CR1_CEN;
	counter = 0;
	clock_run = 0;
	// clear pending
	EXTI->PR1 = 1 << 13;

	return;
}


int main() {
	GPIO_init();
	max7219_init();
	GPIO_init_AF();
	keypad_init();
	Timer_init(timer);
	PWM_channel_init(timer);
	SystemClock_Config();
	EXTI_config();
	NVIC_config();

	display(0);
	counter = 0;
	while (1) {
		GPIOA->ODR |= (0b1111 << 6);
		counter = keypad_scan();
		if(counter>0) {
			SysTick->CTRL |= 0x7;
			clock_run=1;
		}
		while(clock_run);
	}

	return 0;
}
