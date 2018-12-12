#include "../inc/stm32l476xx.h"
int tim=0;

void SystemClock_Config(){
	// Turn on HSI
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR&RCC_CR_HSIRDY)==0); // 16MHZ
	// switch SYSCLK to HSI
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

	//use the clock from processor
	SysTick->CTRL |= 0x00000004; //
	SysTick->LOAD = (uint32_t)31999999; //unsigned int 32 bit counter 16000000 (2s interrupt once)
	//system interrupt happens for every 8000000 cpu cycles, that is the peroid of 2 second
	SysTick->CTRL |= 0x00000007; //processor clock, turn on all

}

void SysTick_Handler(void){
	tim++;
	if(tim==2){
		GPIOB->BRR = GPIO_BRR_BR3;
	}
	if(tim==3){
		GPIOB->BSRR = GPIO_BSRR_BS3;
		tim = 0;
	}
}

void led_init(){
	/* Enable GPIOB */
	RCC->AHB2ENR |= 0x2;

	/* Set PA3 as output mode */
	GPIOB->MODER &= ~GPIO_MODER_MODE3;
	GPIOB->MODER |= GPIO_MODER_MODE3_0;

	/* Set PA3 as high speed mode */
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED3;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3_1;

	/* PUPDR pull up*/
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD3;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD3_0;
}


int main(){
	SystemClock_Config();
	led_init();
	while(1){};
	return 0;
}
