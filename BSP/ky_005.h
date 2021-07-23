/*
 * ky_005.h
 *
 *  Created on: 12 jul. 2021
 *      Author: Usuario
 */

#ifndef KY_005_H_
#define KY_005_H_

#include "bsp.h"

#define BITtime		562

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} KY005_TypeDef;

void KY005_Init(void);

void sendIRCode(uint32_t code);

#endif /* KY_005_H_ */
