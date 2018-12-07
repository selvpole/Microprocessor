#include "../inc/stm32l476xx.h"
#include <stdio.h>
#include <stdlib.h>
extern void GPIOinit();
extern void delay_1s();

#define SET_REG(REG,SELECT,VAL) {((REG)=(REG&(~(SELECT)))|(VAL));};

void SystemClock_Config(int clock_speed_Mhz){
	int pn=0, pm=0, pr=0;
	switch( clock_speed_Mhz ){
		// formula: 4/pr*pn/pm (MHz)
		case 1:
			pr=3;pn=8;;pm=0;
			break;
        case 6:
			pr=3;pn=12;pm=0; // 4/8*12/1 = 6
			break;
		case 10:
			pr=3;pn=40;pm=0; // 4/8*20/1 = 10
			break;
		case 16:
            pr=3;pn=32;pm=0; // 4/8*32/1 = 16
            break;
        case 40:
            pr=3;pn=80;pm=1; // 4/8*80/1 = 40
            break;
        default: // Same as input source
        	pr=3;pn=8;pm=1;
        	break;
	}

	/* RCC MSI */

	// Config MSI
	SET_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_MSI);
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI) ;

	// Config PLLON
	/* formula for clock generation
	*====================================================
	*	f(VCO clock) = f(PLL clock input) * (PLLN / PLLM)
	*   f(PLL_R) = f(VCO clock) / PLLR
	*====================================================
	*/
	//Select MSI as PLLCLK source
	RCC->CR &= ~RCC_CR_PLLON;
	while((RCC->CR&(~RCC_CR_PLLRDY))==0);
	SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, RCC_PLLCFGR_PLLSRC_MSI);
	while((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_MSI)==0);

	// PLLR: Main PLL division(bit[26:25]) => (2,4,6,8)
	SET_REG( RCC->PLLCFGR, RCC_PLLCFGR_PLLR, (pr&3)<<25 );
	// PLLN: division fractor(bit[14:8]) => (8 ~ 86)
	SET_REG( RCC->PLLCFGR, RCC_PLLCFGR_PLLN, (pn&127)<<8);
	// PLLM: division denominator(bit[6:4]) => (1 ~ 8)
	SET_REG( RCC->PLLCFGR, RCC_PLLCFGR_PLLM, (pm&7)<<4 );


	//Enable PLLR
	SET_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLREN, RCC_PLLCFGR_PLLREN);
	// Enable PLL CLK
	RCC->CR  |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	// Change clock back to PLL
	SET_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

int user_press_buttom(){
	int k=0, flag_debounce;
	if((GPIOC->IDR & 0b0010000000000000) == 0){
		k = 5000;
		while(k>0){
			flag_debounce = GPIOC->IDR & 0b0010000000000000;
			k--;
		}
		if(flag_debounce == 0){
			return 1;
		}
	}
	return 0;
}

int main(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIO_init();
	// Open MSI
	RCC->CR |= RCC_CR_MSION;
	while((RCC->CR & RCC_CR_MSIRDY)==0);
	SystemClock_Config(1);
	int clock_type[5]={1,6,10,16,40};
	int i=0;
	while(1){
		if(user_press_buttom()){
			i = (i+1)%5;
			SystemClock_Config(clock_type[i]);
		}
		GPIOA->BSRR = (1<<5); // set PA5 to 1
		delay_1s();
		if(user_press_buttom()){
			i = (i+1)%5;
			SystemClock_Config(clock_type[i]);
		}
		GPIOA->BRR = (1<<5); // set PA5 to 0
		delay_1s();
	}
}
