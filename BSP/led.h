/*
 * led.h
 *
 *  Created on: 21 jun. 2021
 *      Author: Usuario
 */

#ifndef LED_H_
#define LED_H_

#include "bsp.h"

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t pin;
} GPIOLED_TypeDef;

void LED_Init(void);
void LED_On(void *led);
void LED_Off(void *led);
void LED_Toggle(void *led);


#endif /* LED_H_ */
