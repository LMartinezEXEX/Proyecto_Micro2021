/*
 * ky_005.c
 *
 *  Created on: 12 jul. 2021
 *      Author: Usuario
 */

#include "ky_005.h"

void IRcarrier(unsigned int IRtimemicroseconds);

KY005_TypeDef ky_005 = {GPIOB, GPIO_PIN_9};

void KY005_Init(void){
	Set_Pin_Output(ky_005.port, ky_005.pin);
}

void sendIRCode(uint32_t code) {
	IRcarrier(9000);
	Delay_us(4500);

	for(int i = 0; i < 32; i++) {
		IRcarrier(BITtime);

		if(code & 0x80000000)
			Delay_us(3 * BITtime);
		else
			Delay_us(BITtime);

		code <<= 1;
	}

	IRcarrier(BITtime);
}

void IRcarrier(unsigned int IRtimemicroseconds)
{
  for(int i=0; i < (IRtimemicroseconds / 26); i++)
    {
	HAL_GPIO_WritePin(ky_005.port, ky_005.pin, 1);   	//turn on the IR LED
	Delay_us(13);          								//delay for 13us (9us + digitalWrite), half the carrier frequnecy
	HAL_GPIO_WritePin(ky_005.port, ky_005.pin, 0);    	//turn off the IR LED
	Delay_us(13);          								//delay for 13us (9us + digitalWrite), half the carrier frequnecy
    }
}
