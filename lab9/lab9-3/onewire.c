#include "onewire.h"

TIM_TypeDef	*timer = TIM2;

//void timer_init(){
//	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
//	/* timer init */
//	timer->PSC = (uint32_t) 0;	/* assuming that the sysclk is 4 MHz */
//	timer->ARR = (uint32_t) 0;
//	timer->EGR = TIM_EGR_UG;
//}

void delay_us(int us){
	for(int i=0; i<us; i++);
}

/* use TIM2 to wait for the delay time */
void delay_ms(int ms){
	delay_us (ms * 1000);
}

/* Init OneWire Struct with GPIO information
 * param:
 *   OneWire: struct to be initialized
 *   GPIOx: Base of the GPIO DQ used, e.g. GPIOA
 *   GPIO_Pin: The pin GPIO DQ used, e.g. 5
 */
void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin){
	OneWireStruct->GPIOx = GPIOx;
	OneWireStruct->GPIO_Pin = GPIO_Pin;
}

/* Send reset through OneWireStruct
 * Please implement the reset protocol
 * param:
 *   OneWireStruct: wire to send
 * retval:
 *    0 -> Reset OK
 *    1 -> Reset Failed
 */
uint8_t OneWire_Reset(OneWire_t* OneWireStruct){
	uint32_t pin = OneWireStruct->GPIO_Pin;
	/* line output low and wait for 480 us */
	OneWireStruct->GPIOx->MODER &= ~(0x3 << pin * 2);
	OneWireStruct->GPIOx->MODER |= 0x1 << pin * 2;
	// set voltage low
	OneWireStruct->GPIOx->BRR |= 0x1 << pin;
	delay_us (480);
	/* release line (set as input) and wait for 70 us */
	OneWireStruct->GPIOx->MODER &= ~(0x3 << pin * 2);
	delay_us (70);
	/* wait until the bit is pulled low */
	while(OneWireStruct->GPIOx->IDR & (0x1 << pin));
	/* delay for 410 us */
	delay_us (410);
}

/* Write 1 bit through OneWireStruct
 * Please implement the send 1-bit protocol
 * param:
 *   OneWireStruct: wire to send
 *   bit: bit to send
 */
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit){
	uint32_t pin = OneWireStruct->GPIO_Pin;
	// set output mode
	OneWireStruct->GPIOx->MODER &= ~(0x3 << pin * 2);
	OneWireStruct->GPIOx->MODER |= (0x1 << pin * 2);
	// set voltage low
	OneWireStruct->GPIOx->BRR |= 0x1 << pin;
	delay_us(3);
	if(bit)
		OneWireStruct->GPIOx->MODER &= ~(0x3 << pin * 2);
	delay_us(60);
	OneWireStruct->GPIOx->MODER &= ~(0x3 << pin * 2);
}

/* Read 1 bit through OneWireStruct
 * Please implement the read 1-bit protocol
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct) {
	uint32_t pin = OneWireStruct->GPIO_Pin;
	delay_us(2);
	// set output mode
	OneWireStruct->GPIOx->MODER &= ~(0x3 << pin * 2);
	OneWireStruct->GPIOx->MODER |= (0x1 << pin * 2);
	// set vlotage low
	OneWireStruct->GPIOx->BRR |= 0x1 << pin;
	delay_us(3);
	// set input mode
	OneWireStruct->GPIOx->MODER &= ~(0x3 << (pin * 2));
	uint8_t ret = (OneWireStruct->GPIOx->IDR >> pin) & 0x1;
	delay_us(60);
	return ret;
}

/* A convenient API to write 1 byte through OneWireStruct
 * Please use OneWire_WriteBit to implement
 * param:
 *   OneWireStruct: wire to send
 *   byte: byte to send
 */
void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte) {
	for (int i = 0; i < 8; i++)
		OneWire_WriteBit(OneWireStruct, (byte>>i) & 0x1);
}

/* A convenient API to read 1 byte through OneWireStruct
 * Please use OneWire_ReadBit to implement
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct) {
	uint8_t ret = 0;
	for(int i=0; i<8; i++)
		ret |= OneWire_ReadBit(OneWireStruct) << i;
	return ret;
}

/* Send ROM Command, Skip ROM, through OneWireStruct
 * You can use OneWire_WriteByte to implement
 */
void OneWire_SkipROM(OneWire_t* OneWireStruct) {
	OneWire_WriteByte(OneWireStruct, 0xCC);
}
