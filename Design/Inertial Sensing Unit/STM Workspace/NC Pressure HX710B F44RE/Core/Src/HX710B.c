/*
 * HX710B.c
 *
 *  Created on: Apr 5, 2023
 *      Author: Dutah/Thiruvarulselvan K
 */
#include "main.h"

#include "HX710B.h"

enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz;

uint8_t shiftIn(GPIO_TypeDef* dataPort, uint16_t dataPin, GPIO_TypeDef* clockPort, uint16_t clockPin, uint8_t bitOrder) {
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		HAL_GPIO_WritePin(clockPort,clockPin, 1);
		if (bitOrder == LSBFIRST)
			value |= HAL_GPIO_ReadPin(dataPort,dataPin) << i;
		else
			value |= HAL_GPIO_ReadPin(dataPort,dataPin) << (7 - i);
		HAL_GPIO_WritePin(clockPort, clockPin, 0);
	}
	return value;
}

unsigned long readHX() {

  // pulse clock line to start a reading
  for (char i = 0; i < HX_MODE; i++) {
    HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin, 1);
    HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin, 0);
  }

  // wait for the reading to finish
  while (HAL_GPIO_ReadPin(OUT_GPIO_Port,OUT_Pin)) {}

  // read the 24-bit pressure as 3 bytes using SPI
  byte data[3];
  for (byte j = 3; j--;) {
    data[j] = shiftIn(OUT_GPIO_Port, OUT_Pin, SCK_GPIO_Port, SCK_Pin, MSBFIRST);
  }

  data[2] ^= 0x80;  // see note

  // shift the 3 bytes into a large integer
  long result=0;
  result += (long)data[2] << 16;
  result += (long)data[1] << 8;
  result += (long)data[0];

  return result;
}
