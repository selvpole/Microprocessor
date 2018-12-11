/* keypad.c */
#include "../inc/stm32l476xx.h"
/* output */
#define X0 6 // PA6
#define X1 7 // PA7
#define X2 8 // pAB
#define X3 9 // PA9
/* input */
#define Y0 11 // PB11
#define Y1 12 // PB12
#define Y2 13 // PB13
#define Y3 14 // PB14
unsigned int x_pin[4] = {X0, X1, X2, X3}; // GPIOA as output
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3}; // GPIOB as input

int Table[4][4] =  {{261,293,329,0},
					{349,392,440,0},
					{493,523,  0,0},
					{1,0,2,0}};

void keypad_init() {
	/* Enable GPIOA, GPIOB */
	RCC->AHB2ENR = RCC->AHB2ENR | 0b11;
	/*=========================== GPIOA ===========================*/
	// MODER: output mode (01): 6 7 8 9
	GPIOA->MODER = GPIOA->MODER & 0xFFF00FFF;
	GPIOA->MODER = GPIOA->MODER | 0x00055000;
	// OSPEEDR: medium speed (01)
	GPIOA->OSPEEDR = GPIOA->OSPEEDR & 0xFFF00FFF;
	GPIOA->OSPEEDR = GPIOA->OSPEEDR | 0x00055000;
	// PUPDR: pull up (01)
	GPIOA->PUPDR = GPIOA->PUPDR & 0xFFF00FFF;
	GPIOA->PUPDR = GPIOA->PUPDR | 0xFFF55000;
	/*=========================== GPIOB ===========================*/
	// MODER: input mode (00): 11 12 13 14
	GPIOB->MODER = GPIOB->MODER & 0xC03FFFFF;
	// OSPEEDR: medium speed (01)
	GPIOB->OSPEEDR = GPIOB->OSPEEDR & 0xC03FFFFF;
	GPIOB->OSPEEDR = GPIOB->OSPEEDR | 0x15400000;
	// PUPDR: pull down (10)
	GPIOB->PUPDR = GPIOB->PUPDR & 0xC03FFFFF;
	GPIOB->PUPDR = GPIOB->PUPDR | 0x2A800000;
}

int keypad_scan() {
	int k, flag_keypad, flag_debounce, flag_keypad_r;
	// detect next button
	GPIOA->ODR = GPIOA->ODR | (0b1111 << 6);
	while(1){
		flag_keypad=GPIOB->IDR & (0b1111<<11);
		if(flag_keypad!=0){ // check the button is pressed
			k=45000;
			while(k!=0){ // check the debounce
				flag_debounce = GPIOB->IDR&(0b1111 << 11);
				k--;
			}
			if(flag_debounce!=0){
				for(int i=0; i<4; i++){ // row
					GPIOA->ODR = (GPIOA->ODR&0xFFFFFC3F)|(1<<x_pin[i]);
					for(int j=0; j<4; j++){ // column
						flag_keypad_r = GPIOB->IDR&(1<<y_pin[j]);
						if(flag_keypad_r != 0){
							return Table[i][j];
						}
					}
				}
			}
		}
		return -1;
	}
}

