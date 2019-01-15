#include "ds18b20.h"

int conv_time[4] = {94,188,375,750};
/* Send ConvT through OneWire with resolution
 * param:
 *   OneWire: send through this
 *   resolution: temperature resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
int DS18B20_ConvT(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	OneWire_WriteByte(OneWire,0x44);
	while(OneWire_ReadBit(OneWire) == 0);
	return 0;
}

/* Read temperature from OneWire
 * param:
 *   OneWire: send through this
 *   destination: output temperature
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
uint8_t DS18B20_Read(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	OneWire_WriteByte(OneWire, 0xBE);
	int ratio = 12 - resolution, tmp = 1;
	while(ratio--)	tmp *= 2;
	uint8_t lsb = OneWire_ReadByte(OneWire) & ~(tmp - 1);
	uint8_t msb = OneWire_ReadByte(OneWire);

	return lsb + (msb << 8);
}

/* Set resolution of the DS18B20
 * param:
 *   OneWire: send through this
 *   resolution: set to this resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
uint8_t DS18B20_SetResolution(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	OneWire_WriteByte (OneWire, 0x4E); /* issue the write scratchpad command */
	OneWire_WriteByte (OneWire, 100); /* write the TH register */
	OneWire_WriteByte (OneWire, 0); /* write the TL register */
	OneWire_WriteByte (OneWire, 0x1F+((resolution - 9)<<5));	/* write the configuration register */
//	delay_ms(conv_time[resolution-9]);
	return 0;
}

/* Check if the temperature conversion is done or not
 * param:
 *   OneWire: send through this
 * retval:
 *    0 -> OK
 *    1 -> Not yet
 */
uint8_t DS18B20_Done(OneWire_t* OneWire) {
	// TODO
	return 0;
}
