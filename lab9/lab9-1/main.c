#include "LCD.h"

int pos=0b0;

void SystemClock_Config(){
	// Turn on MSI
	RCC->CR |= RCC_CR_MSION;
	while((RCC->CR&RCC_CR_MSIRDY)==0); // 4MHZ
	// switch SYSCLK to MSI
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_MSI;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI);
	// PreScale
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV4;

	//use the clock from processor
	SysTick->CTRL |= 0x4; //
	SysTick->LOAD = (uint32_t)299999; // 0.3 second
	//system interrupt happens for every 8000000 cpu cycles, that is the peroid of 2 second
	SysTick->CTRL |= 0x7; //processor clock, turn on all

}

void pos_inc(){
	pos= pos+0b1;
	if(pos > 0x0F && pos < 0x40)
		pos = 0x40;
	else if(pos > 0x4F)
		pos = 0x0;
}

void delay(){
	for(int i=0; i<1000; i++);
}

void SysTick_Handler(){
	LCD_write(0x01,0); // clear screen
	delay();
	LCD_write(0x80+pos,0); // set DDRAM address
	LCD_write(0x31,1); // "1"
	pos_inc();
	LCD_write(0x80+pos,0); // set DDRAM address
	LCD_write(0x37,1); // "7"
}

int main(){
	LCD_init();
	SystemClock_Config();
	while(1);
	return 0;
}
