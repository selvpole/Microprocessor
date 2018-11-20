#include "stm32l476xx.h" //TODO: define your gpio pi n
#define X0 6
#define X1 7
#define X2 8
#define X3 9
#define Y0 11
#define Y1 12
#define Y2 13
#define Y3 14
unsigned int x_pin[4] = {X0, X1, X2, X3}; // GPIOA 6 7 8 9
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3}; // GPIOB 11 12 13 14

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char adr, unsigned char data);

int output=0;
int Table[4][4] = {{1,2,3,10},{4,5,6,11},{7,8,9,12},{15,0,14,13}};

/* TODO: initial keypad gpio pin, X as output and Y as input */
void keypad_init() {
	/*=========================== AHB2 ============================*/
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
/* TODO: scan keypad value return: >=0: key pressed_value -1: no key_press */
int keypad_scan() {
	int flag_keypad, flag_debounce, k, flag_keypad_r;
	// reset to detect next button
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
			GPIOA->ODR=GPIOA->ODR|0b1111<<6; //set PA6,7,8,9(row) high
		}
		else {
			return -1;
		}
	}
}

void display(int data){
	if(data==-1){
		for(int i=1; i<=8; i++)
			max7219_send(i,0xF);
	}
	else{
		for(int i=1; i<=8; i++){
			if(data > 0)
				max7219_send(i,data%10);
			else{
				if(i==1)
					max7219_send(1,0);
				else
					max7219_send(i,0xF);
			}
			data /= 10;
		}
	}
}

int main(){
	GPIO_init();
	keypad_init();
	max7219_init();
	display(0);
	while (1) {
		int keypad_value = keypad_scan();

		if (keypad_value != -1){
			display(keypad_value);
		}
		else
			display(-1);
	}
}
