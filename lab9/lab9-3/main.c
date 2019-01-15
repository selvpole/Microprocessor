#include "LCD.h"
#include "ds18b20.h"
#include <string.h>

char font[] = {
  0x0E, 0x0E, 0x0E, 0x05, 0x1F, 0x14, 0x07, 0x1D,
  0x1C, 0x1C, 0x1D, 0x06, 0x1C, 0x07, 0x1D, 0x00
};

OneWire_t OneWire;
int t_res = 11;
int mod = 0x0;
int mod_c = 0x0;

int pos=0b0;

void SystemClock_Config(){
	SysTick->CTRL |= 0x4; //
	SysTick->LOAD = (uint32_t)1199999; // 0.3 second
//	//system interrupt happens for every 8000000 cpu cycles, that is the peroid of 2 second
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

int stridx(int p){
	return p/0x40*16 + p%0x40;
}

int int_len (int n)
{
	int len = 0;
	while (n > 0) {
		n /= 10;
		++len;
	}
	return len;
}

void write_str_to_LCD (const char *s){
	LCD_write (0x01, 0);
	delay();
	int len = strlen(s);

	while (stridx(pos) < len) {
		LCD_write (0x80 + pos, 0);	/* set up the DD RAM address */
		LCD_write (s[stridx(pos)] - 0, 1);
		pos_inc();
	}
	if (len == stridx(pos)) {
		// LCD_write (0x01, 0); /* clear screen */
		pos = 0;
	}
}

void temp_to_str (char *s, int num)
{
	memset(s, 0, sizeof(s));
	int _int = num >> 4;
	int _float = (num & 0x0F) * 10000 / 16;

	int len;
	int tmp = _int;
	for (len = 0; tmp > 0; len++) tmp /= 10;
	for (int i = len - 1; i >= 0; i--) {
		s[i] = _int % 10 + '0';
		_int /= 10;
	}
	s[len] = '.';

	for (int i = len + 4; i >= len + 1; i--) {
		s[i] = _float % 10 + '0';
		_float /= 10;
	}
	s[len + 5] = '\0';
}

void write_temp_to_LCD (void)
{
	int	temp;
	char ch[32];

//	OneWire_Reset (&OneWire);
//	OneWire_SkipROM (&OneWire);
//	DS18B20_ConvT (&OneWire, t_res);

	OneWire_Reset(&OneWire);
	OneWire_SkipROM(&OneWire);
	temp = DS18B20_Read(&OneWire, t_res);

	temp_to_str(ch, temp);
	write_str_to_LCD(ch);
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
		mod_c = (mod_c + 1) % 10;
		if (mod_c == 0)
			write_temp_to_LCD ();
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
	return (deb>=10000);
}

int main(){
	btn_init();
	LCD_init();
	SystemClock_Config();
	create_font();
	OneWire_Init(&OneWire, GPIOC, 4);
	OneWire_Reset(&OneWire);
	OneWire_SkipROM(&OneWire);
	DS18B20_SetResolution(&OneWire, t_res);
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
