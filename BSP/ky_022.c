/*
 * ky_022.c
 *
 *  Created on: 24 jun. 2021
 *      Author: Usuario
 */

#include "ky_022.h"

KY022_TypeDef ky_022  = {GPIOB, GPIO_PIN_5};

uint32_t receiveIRCode(void) {
	uint8_t count;
	uint32_t code=0;

	 // Wait the pin to go low
	  while (HAL_GPIO_ReadPin(ky_022.port, ky_022.pin));

		  /* The START Sequence begin here
	   * there will be a pulse of 9ms LOW and
	   * than 4.5 ms space (HIGH)
	   */
	  while (!(HAL_GPIO_ReadPin (ky_022.port, ky_022.pin)));  // wait for the pin to go high.. 9ms LOW

	  while ((HAL_GPIO_ReadPin (ky_022.port, ky_022.pin)));  // wait for the pin to go low.. 4.5ms HIGH

	  /* START of FRAME ends here*/

	  /* DATA Reception
	   * We are only going to check the SPACE after 562.5us pulse
	   * if the space is 562.5us, the bit indicates '0'
	   * if the space is around 1.6ms, the bit is '1'
	   */

	  for (int i=0; i<32; i++)
	  {
		  count=0;

		  while (!(HAL_GPIO_ReadPin (ky_022.port, ky_022.pin))); // wait for pin to go high.. this is 562.5us LOW

		  while ((HAL_GPIO_ReadPin (ky_022.port, ky_022.pin)))  // count the space length while the pin is high
		  {
			  count++;
			  Delay_us(100);
		  }

		  if (count > 12) // if the space is more than 1.2 ms
		  {
			  code |= (1UL << (31-i));   // write 1
		  }

		  else code &= ~(1UL << (31-i));  // write 0
	  }

		return code;
}
