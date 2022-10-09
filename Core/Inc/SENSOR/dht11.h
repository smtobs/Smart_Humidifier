/*
 * dht11.h
 *
 *  Created on: Sep 29, 2022
 *      Author: bsoh
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"

#define DHT11_SUCCESS         1
#define DHT11_ERROR_CHECKSUM  2
#define DHT11_ERROR_TIMEOUT   3

typedef struct DHT11_Dev {
	uint8_t temparature;
	uint8_t humidity;
	GPIO_TypeDef* port;
	uint32_t pin;
} DHT11_Dev;

DHT11_Dev dht11Dev;

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

void dht11Init(DHT11_Dev *);
void dht11Start (DHT11_Dev *);
uint8_t dht11CheckResponse (DHT11_Dev *);
uint8_t dht11Read (DHT11_Dev *);
#endif /* INC_DHT11_H_ */
