/*
 * ky_022.h
 *
 *  Created on: 24 jun. 2021
 *      Author: Usuario
 */

#ifndef KY_022_H_
#define KY_022_H_

#include "bsp.h"

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} KY022_TypeDef;

uint32_t receiveIRCode(void);

#endif /* KY_022_H_ */
