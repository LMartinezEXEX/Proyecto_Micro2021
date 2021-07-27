/*
 * dht11.c
 *
 *  Created on: 21 jun. 2021
 *      Author: Usuario
 */

#include "dht11.h"

void DHT11_Start(void);
uint8_t DHT11_Check_Status(void);
uint8_t DHT11_Read(void);

DHT11_TypeDef dht11 = {GPIOA, GPIO_PIN_6};

void DHT11_Start(void) {

	Set_Pin_Output(dht11.port, dht11.pin);  		// set the pin as output
	HAL_GPIO_WritePin(dht11.port, dht11.pin, 0);   	// pull the pin low
	Delay_us(18000);   								// wait for 18ms
	HAL_GPIO_WritePin(dht11.port, dht11.pin, 1);
	Delay_us(20);
	Set_Pin_Input(dht11.port, dht11.pin);    		// set as input
}

uint8_t DHT11_Check_Status(void) {
	uint8_t Response = 0;
	Delay_us(40);
	if (!(HAL_GPIO_ReadPin(dht11.port, dht11.pin))) {

		Delay_us(80);

		if ((HAL_GPIO_ReadPin(dht11.port, dht11.pin)))
			Response = 1;
		else
			Response = -1;
	}
	while ((HAL_GPIO_ReadPin(dht11.port, dht11.pin)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_Read(void) {
	uint8_t i = 0, j;
	for (j = 0; j < 8; j++) {
		while (!(HAL_GPIO_ReadPin(dht11.port, dht11.pin)));   	// wait for the pin to go high

		Delay_us(40);   										// wait for 40 us

		if (!(HAL_GPIO_ReadPin(dht11.port, dht11.pin)))   		// if the pin is low
		{
			i &= ~(1 << (7 - j));   							// write 0
		} else
			i |= (1 << (7 - j));  								// if the pin is high, write 1

		while ((HAL_GPIO_ReadPin(dht11.port, dht11.pin)));  	// wait for the pin to go low
	}
	return i;
}

uint8_t DHT11_getTemp(void){
	uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
	uint16_t SUM;

	DHT11_Start();

	if (!DHT11_Check_Status())
		Error_Handler();

	Rh_byte1 = DHT11_Read();
	Rh_byte2 = DHT11_Read();
	Temp_byte1 = DHT11_Read();
	Temp_byte2 = DHT11_Read();

	SUM = DHT11_Read();

	return Temp_byte1;
}

uint8_t DHT11_getHR(void) {
	uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
	uint16_t SUM;

	DHT11_Start();

	if (!DHT11_Check_Status())
		Error_Handler();

	Rh_byte1 = DHT11_Read();
	Rh_byte2 = DHT11_Read();
	Temp_byte1 = DHT11_Read();
	Temp_byte2 = DHT11_Read();

	SUM = DHT11_Read();

	return Rh_byte1;
}

