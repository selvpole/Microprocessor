#include "LCD.h"
#include <string.h>

const char str[] = "Help! This is HELL!!";
const int len = strlen(str);

char font[] = {
  0x0E,
  0x0E,
  0x0E,
  0x05,
  0x1F,
  0x14,
  0x07,
  0x1D,
  0x1C,
  0x1C,
  0x1D,
  0x06,
  0x1C,
  0x07,
  0x1D,
  0x00
};

int mod = 0x0;

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

int stridx(int pos){
	return pos / 0x40 * 16 + pos % 0x40;
}

void write_str_to_LCD(const char *s){
	if (stridx (pos) == len) {
		LCD_write (0x01, 0);	/* clear screen */
		pos = 0;	/* reset the position */
	} else {
		LCD_write (0x80 + pos, 0);	/* set up the DD RAM address */
		LCD_write (s[stridx (pos)], 1);	/* write a character from the string */
		pos_inc ();
	}
}

void SysTick_Handler(){
	switch(mod){
	case 0:
		LCD_write(0x01,0); // clear screen
		delay();
		LCD_write(0x80+pos,0); // set DDRAM address
		LCD_write(0x00,1);
		pos_inc();
		LCD_write(0x80+pos,0); // set DDRAM address
		LCD_write(0x01,1);
		break;
	case 1:
		write_str_to_LCD(str);
		break;
	}
}

void create_font(){
	delay();
	for(int num=0; num<2; num++)
		for(int row=0; row<8; row++){
			LCD_write(0x40 + (num<<3) + row, 0);
			LCD_write(font[(num<<3) + row], 1);
		}
}

void btn_init ()
{
	RCC->AHB2ENR |= 0x4;
	GPIOC->MODER &= ~GPIO_MODER_MODE13;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED13;
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED13_0;
}

int btn_pressed ()
{
	int	deb = 0;
	while (!(GPIOC->IDR & (GPIO_IDR_ID13)) && deb++ < 10000);
	return !(GPIOC->IDR & (GPIO_IDR_ID13));
}

int main(){
	LCD_init();
	SystemClock_Config();
	create_font();
	btn_init();
	while(1){
		if(btn_pressed()){
			while(btn_pressed());
			mod = (mod+1)%2;
			pos = 0x0;
			LCD_write(0x01,0); // clear screen
			delay();
		}
	}
	return 0;
}
