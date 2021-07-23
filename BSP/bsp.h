/*
 * bsp.h
 *
 *  Created on: 23 jun. 2021
 *      Author: Usuario
 */

#ifndef BSP_H_
#define BSP_H_

#include "stm32f4xx_hal.h"

#include "DWT_delay.h"
#include "flash.h"
#include "led.h"
#include "dht11.h"
#include "ky_022.h"
#include "ky_005.h"


void BSP_Init(void);
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void CONSOLE_SendMessage(uint8_t* Buf, uint16_t Len);

void Error_Handler(void);
#endif /* BSP_H_ */
