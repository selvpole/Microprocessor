#include "../inc/stm32l476xx.h"
#define TIME_SEC 12.7
extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(char a , char b);
extern void Display(int data);
#define SET_REG(REG, SELECT, VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));};

TIM_TypeDef *timer;
int num_digs = 8;

int display(int data)
{
	int i=0;
	int digit;
	if(data>99999999)
		return -1;
	for(i=1;i<=num_digs;i++)
	{
		digit = data%10;
		data/=10;
		if(digit == 0 && data == 0 && i>3)
			digit = 15; // space
		if(i == 3)
			digit |= 0x80; // add '.'
		max7219_send(i, digit);
	}
	return 0;
}

void Timer_init(TIM_TypeDef *timer){
	//Initialize timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // 4MHz
	// TIM2_PSC = 1
	timer->PSC = 0;
	// TIM_ARR = 10000.00
	timer->ARR = 10000;
}

void Timer_start(TIM_TypeDef *timer){
	timer->CR1 |= TIM_CR1_CEN;
}

void count(){
	int time = 0;
	while(1){	//Polling the timer count and do lab requirements
		if(time >= TIME_SEC*100) break;
		// if reload, time++, display(); close flag
		if((timer->SR&TIM_SR_UIF) == 1){ // status register update interrupt
			time++;
			display(time);
			TIM2->SR &= ~TIM_SR_UIF;
		}
	}
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
	GPIO_init();
	max7219_init();
	timer = TIM2;
	Timer_init(timer);
	Timer_start(timer);
	display(0);
	count();
	while(1){
		if(user_press_buttom()){
			count();
		}
	}
	return 0;
}
