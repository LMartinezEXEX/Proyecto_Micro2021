/*
 * dht11.h
 *
 *  Created on: 21 jun. 2021
 *      Author: Usuario
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "bsp.h"

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} DHT11_TypeDef;

uint8_t DHT11_getTemp(void);
uint8_t DHT11_getHR(void);

#endif /* DHT11_H_ */
