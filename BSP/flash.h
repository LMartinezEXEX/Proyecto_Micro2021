/*
 * flash.h
 *
 *  Created on: 19 jul. 2021
 *      Author: Usuario
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "bsp.h"

uint32_t Flash_Write_Data (uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords);
void Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords);

#endif /* FLASH_H_ */
