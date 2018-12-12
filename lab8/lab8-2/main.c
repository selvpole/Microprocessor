#include "../inc/stm32l476xx.h"
#include "keypad.h"

void led_init(){
	/* Enable GPIOA */
	RCC->AHB2ENR |= 0x1;

	/* Set PA2 as output mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE5;
	GPIOA->MODER |= GPIO_MODER_MODE5_0;

	/* Set PA2 as high speed mode */
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1;

	/* PUPDR pull up*/
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD5_0;
}

//void Sysclk_config (void)
//{
//	/* turn on HSI */
//	RCC->CR |= RCC_CR_HSION;
//	while ((RCC->CR & RCC_CR_HSIRDY)==0);
//	/* switch the SYSCLK to HSI */
//	RCC->CFGR &= ~RCC_CFGR_SW;
//	RCC->CFGR |= RCC_CFGR_SW_HSI;
//	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
//	/* set the HPRE */
//	RCC->CFGR &= ~RCC_CFGR_HPRE;
//	RCC->CFGR |= RCC_CFGR_HPRE_DIV16;
//
//	//use the clock from processor
//	SysTick->CTRL |= 0x00000004; //
//	SysTick->LOAD = (uint32_t)499999; //(0.5s interrupt once)
//	//system interrupt happens for every 8000000 cpu cycles, that is the peroid of 2 second
//	SysTick->CTRL |= 0x00000007; //processor clock, turn on all
//}

void EXTI_config(){ // PB:11 12 13 14
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// 11
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR3_EXTI11_PB
					   | SYSCFG_EXTICR4_EXTI12_PB
					   | SYSCFG_EXTICR4_EXTI13_PB
					   | SYSCFG_EXTICR4_EXTI14_PB;

	// mask
	EXTI->IMR1 |= EXTI_IMR1_IM11
				| EXTI_IMR1_IM12
				| EXTI_IMR1_IM13
				| EXTI_IMR1_IM14;
	// rising trigger
	EXTI->RTSR1 |= EXTI_RTSR1_RT11
				 | EXTI_RTSR1_RT12
				 | EXTI_RTSR1_RT13
				 | EXTI_RTSR1_RT14;
}

void NVIC_config(){
	NVIC_EnableIRQ (EXTI15_10_IRQn);
	NVIC_SetPriority (EXTI15_10_IRQn, (uint32_t)0);
}

void EXTI15_10_IRQHandler(){
	int key_value = keypad_scan();
	while (key_value--) {
		for (int j = 0; j < 50000; j++);
			GPIOA->BSRR = GPIO_BSRR_BS5;
		for (int j = 0; j < 50000; j++);
			GPIOA->BRR = GPIO_BRR_BR5;
	}
	GPIOA->ODR = GPIOA->ODR | (0b1111 << 6);

	// clear pending, or interrupt will be triggered continually
	EXTI->PR1 &= ~EXTI_PR1_PIF11;
	EXTI->PR1 &= ~EXTI_PR1_PIF12;
	EXTI->PR1 &= ~EXTI_PR1_PIF13;
	EXTI->PR1 &= ~EXTI_PR1_PIF14;
}

//void SysTick_Handler(void){
//	if(key_value > 0 && light_up == 1){
//		GPIOA->BSRR = GPIO_BSRR_BS5;
//		light_up = 0;
//		key_value--;
//	}
//	else if(key_value > 0 && light_up == 0){
//		GPIOA->BRR = GPIO_BRR_BR5;
//		light_up = 1;
//	}
//	else if(key_value==0){
//		GPIOA->BRR = GPIO_BRR_BR5;
//		light_up = 1;
//	}
//}


int main(){
//	Sysclk_config();
	led_init();
	keypad_init();
	EXTI_config();
	NVIC_config();
	GPIOA->ODR = GPIOA->ODR | (0b1111 << 6);
	while(1){}

	return 0;
}
