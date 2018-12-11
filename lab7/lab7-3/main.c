#include "../inc/stm32l476xx.h"
#include "keypad.h"

void GPIO_init_AF() {
	// TODO: Initial GPIO pin as alternate function for buzzer.
	// choose PB4_AF2 => TIM3_CH1
	RCC->AHB2ENR = RCC->AHB2ENR | 0x2;
	GPIOB->MODER &= ~GPIO_MODER_MODE4;
	GPIOB->MODER |= GPIO_MODER_MODE4_1;  // set to the alternative function mode
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL4_1; // => 0010: TIM3_CH1
}

void Timer_init(TIM_TypeDef *timer) {
	// TODO: Initialize timer
	// Use TIM3
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	timer->ARR = (uint32_t)99;
	timer->PSC = (uint32_t)9999;
	timer->EGR = TIM_EGR_UG;
}

void PWM_channel_init(TIM_TypeDef *timer) {
	// TODO: Initialize timer PWM channel
	// Use CH1

	// due to ARR == 100
	timer->CCR1 = 50;

	// set bit[6:4] = 011, wave flip if CNT == CCR1
	// timer->CCMR1 &= 0xFF8F;
	timer->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;

	timer->CR1 |= TIM_CR1_ARPE;

	// Capture Compare 1 Output Enable
	timer->CCER |= TIM_CCER_CC1E;

	timer->EGR = TIM_EGR_UG;
}


int main() {
	TIM_TypeDef	*timer = TIM3;
	GPIO_init_AF();
	keypad_init();
	Timer_init(timer);
	PWM_channel_init(timer);
	// TODO: Scan the keypad and use PWM to send the corresponding frequency square wave to buzzer.
	while (1) {
		int value = keypad_scan();
		if(value == -1) {
			timer->CR1 &= ~TIM_CR1_CEN;
		}
		else if (value == 1) {
			timer->CCR1 = (timer->CCR1+5)<90 ? timer->CCR1+5 : 90;
		}
		else if (value == 2){

			timer->CCR1 = (timer->CCR1+5)>10 ? timer->CCR1-5 : 10;
		}
		else {
			timer->PSC = 10000/(uint32_t)value - 1;
			timer->CR1 |= TIM_CR1_CEN;
		}
	}

	return 0;
}
