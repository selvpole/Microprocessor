#include "stm32l476xx.h"

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

int main(){
	GPIO_init();
	max7219_init();
	int student_id[] = {0,6,1,6,4,2,1};

	for(int i=0; i<7; i++){
			max7219_send(7-i, student_id[i]);
	}
}
