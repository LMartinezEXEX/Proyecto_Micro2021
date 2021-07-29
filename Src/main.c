/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "Queue.h"
#include "Semphr.h"
#include "Timers.h"

// Leds declaration
void *ledRed;
void *ledBlue;
void *ledOrange;
void *ledGreen;

// ON/OFF : 2200084830
// VOL+ : 2200085085
// VOL- : 2200085340

void prvInitErrorTimerCallback(TimerHandle_t xTimer);

void vPeriodic_get_HR(void *pvParameters);
void vCONSOLE_SerialPrint(void *pvParameters);
void vManage_Actions(void *pvParameters);
void vIR_code_get(void *pvParameters);
void vIR_code_send(void *pvParameters);
void vIR_code_receive(void *pvParameters);
void vCONSOLE_SerialReceive(void *pvParameters);
void vFlashSaver(void *pvParameters);


static QueueHandle_t xHR_to_Comm_Queue = NULL, xHRManager_to_Comm_Queue = NULL, xHR_to_HRManager_Queue = NULL, xHRManager_to_CodeManager_Queue = NULL, xCodeManager_to_AudioCommander_Queue = NULL, xReciever_Queue = NULL, xNewCodeSav_to_Comm_Queue = NULL, xReceiver_to_NewCodeSav_Queue = NULL, xReceiver_to_Comm_Queue = NULL;
static QueueSetHandle_t xCommQueueSet = NULL;

static SemaphoreHandle_t xCodesSem = NULL, xNewCodeSem = NULL, xFlashSaveSem = NULL, xWaitingResponseSem = NULL;

uint32_t codes_pointer = 0;
uint32_t CODE_INDEX_HR_UP = 0;
uint32_t CODE_INDEX_HR_DOWN = 1;

codesStruct_TypeDef codesStruct;

void vPrintCodes(void);
uint8_t ucIndex_In_Bound(uint32_t index);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* HARDWARE INITALIZATION*/
	BSP_Init();

	/**/

	/* CODES STRUCTURE INITALIZATION */
	uint32_t *pCodeStruct = &codesStruct;

	Flash_Read_Data(USER_FLASH_MEMORY, pCodeStruct, MAX_CODES + 3);

	/**/

	BaseType_t xErrorHappendFreeRTOS = pdFALSE;

	/* QUEUE INITALIZATION */

	xHR_to_Comm_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(uint8_t));
	xHRManager_to_Comm_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(action_TypeDef));
	xNewCodeSav_to_Comm_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(uint32_t));
	xReceiver_to_Comm_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(userConfig_TypeDef));

	xCommQueueSet = xQueueCreateSet(4 * QUEUE_DEFAULT_SIZE);

	xQueueAddToSet(xHR_to_Comm_Queue, xCommQueueSet);
	xQueueAddToSet(xHRManager_to_Comm_Queue, xCommQueueSet);
	xQueueAddToSet(xNewCodeSav_to_Comm_Queue, xCommQueueSet);
	xQueueAddToSet(xReceiver_to_Comm_Queue, xCommQueueSet);

	xHR_to_HRManager_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(uint8_t));
	xHRManager_to_CodeManager_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(action_TypeDef));
	xCodeManager_to_AudioCommander_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(uint32_t));

	xReciever_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(receive_message_TypeDef));

	xReceiver_to_NewCodeSav_Queue = xQueueCreate(QUEUE_DEFAULT_SIZE, sizeof(BaseType_t));


	if (	xHR_to_Comm_Queue == NULL || xHRManager_to_Comm_Queue == NULL || xCommQueueSet == NULL ||
			xReciever_Queue == NULL || xHR_to_HRManager_Queue == NULL || xHRManager_to_CodeManager_Queue == NULL ||
			xCodeManager_to_AudioCommander_Queue == NULL || xReciever_Queue == NULL	|| xNewCodeSav_to_Comm_Queue == NULL ||
			xReceiver_to_NewCodeSav_Queue == NULL)
		xErrorHappendFreeRTOS = pdTRUE;

	/**/

	/* SEMAPHORE INITALIZATION */

	xCodesSem = xSemaphoreCreateMutex();

	xNewCodeSem = xSemaphoreCreateBinary();

	xFlashSaveSem = xSemaphoreCreateBinary();

	xWaitingResponseSem = xSemaphoreCreateBinary();

	if(xCodesSem == NULL || xNewCodeSem == NULL || xFlashSaveSem == NULL || xWaitingResponseSem == NULL)
		xErrorHappendFreeRTOS = pdTRUE;

	/**/

	/* TASK CREATION*/

	BaseType_t xHR, xHRManager, xCommunicator, xCodeManager, xAudioCommander, xNewCodeSaver, xReceiver, xFlashSaver;

	xHR 			= xTaskCreate(vPeriodic_get_HR, "HR", 100, NULL, 1, NULL);
	xHRManager 		= xTaskCreate(vManage_Actions, "HRManager", 100, NULL, 2, NULL);
	xReceiver		= xTaskCreate(vCONSOLE_SerialReceive, "Receiver", 100, NULL, 3, NULL);
	xCommunicator 	= xTaskCreate(vCONSOLE_SerialPrint, "Communicator", 300, NULL, 4, NULL);
	xCodeManager 	= xTaskCreate(vIR_code_get, "CodeManager", 100, NULL, 4, NULL);
	xNewCodeSaver	= xTaskCreate(vIR_code_receive, "NewCodeSaver", 100, NULL, 5, NULL);
	xAudioCommander = xTaskCreate(vIR_code_send, "AudioCommander", 100, NULL, 6, NULL);
	xFlashSaver		= xTaskCreate(vFlashSaver, "FlashSaver", 100, NULL, 7, NULL);

	if (	xHR == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY || xHRManager == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY ||
			xCommunicator == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY || xCodeManager == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY ||
			xNewCodeSaver == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY || xReceiver == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY ||
			xAudioCommander == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY || xFlashSaver == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
		xErrorHappendFreeRTOS = pdTRUE;

	/**/

	/*SOFTWARE TIMERS INITALIZATION*/

	if (xErrorHappendFreeRTOS) {
		TimerHandle_t xAutoReloadTimer = NULL;

		xAutoReloadTimer = xTimerCreate("INIT ERRORS!", pdMS_TO_TICKS(500), pdTRUE, 0, prvInitErrorTimerCallback);

		if (xAutoReloadTimer == NULL) {
			LED_On(ledRed);
			while(1);
		}

		if(xTimerStart(xAutoReloadTimer, 0) != pdTRUE) {
			LED_On(ledRed);
			while(1);
		}
	}

  /* Start scheduler */
	vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  for(;;);
}

void prvInitErrorTimerCallback(TimerHandle_t xTimer) {
	LED_Toggle(ledRed);
}

void vPeriodic_get_HR(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();

	uint8_t ucHR;

	BaseType_t xWaitingResponse;

	for(;;){
		xWaitingResponse = xQueuePeek((QueueHandle_t) xWaitingResponseSem, (void *) NULL, (TickType_t) NULL) == pdTRUE;

		if(!xWaitingResponse){
			vTaskSuspendAll();
			{
				ucHR = DHT11_getHR();
			}
			xTaskResumeAll();

			xQueueSendToBack(xHR_to_Comm_Queue, &ucHR, 0);
			xQueueSendToBack(xHR_to_HRManager_Queue, &ucHR, 0);
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5000));
	}

	vTaskDelete( NULL );
}

void vCONSOLE_SerialPrint(void *pvParameters) {
	QueueSetMemberHandle_t xHandle;

	uint8_t ucHR;
	uint8_t pucPrintBuf[80];
	action_TypeDef xAction;
	uint32_t ulIR_Code;
	userConfig_TypeDef xConfig;
	BaseType_t xPrintCodes;

	for(;;){

		xHandle = xQueueSelectFromSet(xCommQueueSet, portMAX_DELAY);

		if( xHandle == (QueueSetMemberHandle_t) xHR_to_Comm_Queue) {
			xQueueReceive(xHandle, &ucHR, 0);

			sprintf(pucPrintBuf, "HR es: %u%%\r\n", ucHR);
			CONSOLE_SendMessage(pucPrintBuf, strlen((char *) pucPrintBuf));
		}
		else if (xHandle == (QueueSetMemberHandle_t) xHRManager_to_Comm_Queue){
			xQueueReceive(xHandle, &xAction, 0);

			switch(xAction){
			case HR_UP:
				memcpy(pucPrintBuf, "La HR% subio!\r\n", sizeof("La HR% subio!\r\n"));
				break;

			case HR_DOWN:
				memcpy(pucPrintBuf, "La HR% bajo!\r\n", sizeof("La HR% bajo!\r\n"));
				break;

			default:
				break;
			}

			CONSOLE_SendMessage(pucPrintBuf, strlen((char *) pucPrintBuf));
		}
		else if(xHandle == (QueueSetMemberHandle_t) xNewCodeSav_to_Comm_Queue){
			xQueueReceive(xHandle, &ulIR_Code, 0);

			sprintf(pucPrintBuf, "Codigo: %lu, envia Y para guardar o otra para descartar\r\n", ulIR_Code);
			CONSOLE_SendMessage(pucPrintBuf, strlen((char *) pucPrintBuf));
		}
		else if(xHandle == (QueueSetMemberHandle_t) xReceiver_to_Comm_Queue){
			xQueueReceive(xHandle, &xConfig, 0);

			xPrintCodes = pdFALSE;

			switch(xConfig){
			case ILLEGAL:
				memcpy(pucPrintBuf, "Comando incorrecto\r\n", sizeof("Comando incorrecto\r\n"));
				break;
			case NEW_CP:
				memcpy(pucPrintBuf, "¡CP actualizado!\r\n", sizeof("¡CP actualizado!\r\n"));
				break;
			case BAD_CP:
				memcpy(pucPrintBuf, "CP fuera de rango\r\n", sizeof("CP fuera de rango\r\n"));
				break;
			case NEW_INDEX:
				memcpy(pucPrintBuf, "¡Indice actualizado!\r\n", sizeof("¡Indice actualizado!\r\n"));
				break;
			case BAD_INDEX:
				memcpy(pucPrintBuf, "Indice fuera de rango\r\n", sizeof("Indice fuera de rango\r\n"));
				break;
			case NEW_CODE:
				memcpy(pucPrintBuf, "¡Codigo guardado!\r\n", sizeof("¡Codigo guardado!\r\n"));
				break;
			case DISCARD_CODE:
				memcpy(pucPrintBuf, "Codigo descartado\r\n", sizeof("Codigo descartado\r\n"));
				break;
			case RESET_STRUCT:
				memcpy(pucPrintBuf, "¡Reseteado completo!\r\n", sizeof("¡Reseteado completo!\r\n"));
				break;
			case CODES:
				xPrintCodes = pdTRUE;
				break;
			default:
				break;
			}

			if(!xPrintCodes)
				CONSOLE_SendMessage(pucPrintBuf, strlen((char *) pucPrintBuf));

			else{
				xSemaphoreTake(xCodesSem, portMAX_DELAY);
				{
					vPrintCodes();
				}
				xSemaphoreGive(xCodesSem);
			}
		}
		else {
			LED_On(ledBlue);
		}

	}

	vTaskDelete( NULL );
}

void vManage_Actions(void *pvParameters) {
	uint8_t ucHR, ucLast_HR = 0;
	action_TypeDef xAction;

	for(;;) {

		xQueueReceive(xHR_to_HRManager_Queue, &ucHR, portMAX_DELAY);

		if(!ucLast_HR) {
			ucLast_HR = ucHR;
			continue;
		}

		if(ucHR >= ucLast_HR + HR_SENSIBILITY) {
			xAction = HR_UP;
			ucLast_HR = ucHR;
			xQueueSendToBack(xHRManager_to_Comm_Queue, &xAction, 0);
			xQueueSendToBack(xHRManager_to_CodeManager_Queue, &xAction, 0);
		}
		else if(ucHR <= ucLast_HR - HR_SENSIBILITY) {
			xAction = HR_DOWN;
			ucLast_HR = ucHR;
			xQueueSendToBack(xHRManager_to_Comm_Queue, &xAction, 0);
			xQueueSendToBack(xHRManager_to_CodeManager_Queue, &xAction, 0);
		}

	}

	vTaskDelete( NULL );

}

void vIR_code_get(void *pvParameters){

	action_TypeDef xAction;
	uint32_t ulIR_Code;

	for(;;){

		xQueueReceive(xHRManager_to_CodeManager_Queue, &xAction, portMAX_DELAY);

		xSemaphoreTake(xCodesSem, portMAX_DELAY);
		{

			switch(xAction){
			case HR_UP:
				ulIR_Code = codesStruct.codes_arr[codesStruct.CODE_INDEX_HR_UP];
				break;
			case HR_DOWN:
				ulIR_Code = codesStruct.codes_arr[codesStruct.CODE_INDEX_HR_DOWN];
				break;
			default:
				break;
			}

			xQueueSendToBack(xCodeManager_to_AudioCommander_Queue, &ulIR_Code, 0);

		}
		xSemaphoreGive(xCodesSem);
	}

	vTaskDelete( NULL );
}

void vIR_code_send(void *pvParameters){

	uint32_t ulIR_Code;

	for(;;){

		xQueueReceive(xCodeManager_to_AudioCommander_Queue, &ulIR_Code, portMAX_DELAY);

		sendIRCode(ulIR_Code);
		HAL_Delay(500);
		sendIRCode(ulIR_Code);

	}

	vTaskDelete( NULL );
}

void vIR_code_receive(void *pvParameters){

	uint32_t ulIR_Code;
	BaseType_t xResponse;

	for(;;){

		xSemaphoreTake(xNewCodeSem, portMAX_DELAY);

		ulIR_Code = receiveIRCode();

		xQueueSendToBack(xNewCodeSav_to_Comm_Queue, &ulIR_Code, 0);

		xSemaphoreGive(xWaitingResponseSem);

		xQueueReceive(xReceiver_to_NewCodeSav_Queue, &xResponse, portMAX_DELAY);

		if (xResponse) {
			xSemaphoreTake(xCodesSem, portMAX_DELAY);
			{

				codesStruct.codes_arr[codesStruct.codes_pointer] = ulIR_Code;

				xSemaphoreGive(xFlashSaveSem);
			}
			xSemaphoreGive(xCodesSem);
		}

	}

	vTaskDelete( NULL );
}

void vCONSOLE_SerialReceive(void *pvParameters){
	userConfig_TypeDef xConfig;

	receive_message_TypeDef xPack;
	uint8_t *pucBuffer;
	uint32_t ulLen;

	uint32_t ucNEW_codes_pointer;
	uint32_t ucNEW_HR_index;

	BaseType_t xResponse;

	for(;;){

		xQueueReceive(xReciever_Queue, &xPack, portMAX_DELAY);

		pucBuffer = xPack.Buf;
		ulLen = xPack.Len;

		if (ulLen >= 4 && !strncmp((char *) pucBuffer, "CP ", 3)) {
			ucNEW_codes_pointer = atoi((char *) &xPack.Buf[3]);

			if (ucIndex_In_Bound(ucNEW_codes_pointer)) {

				xSemaphoreTake(xCodesSem, portMAX_DELAY);
				{
					codesStruct.codes_pointer = ucNEW_codes_pointer;

					xSemaphoreGive(xFlashSaveSem);
				}
				xSemaphoreGive(xCodesSem);

				xConfig = NEW_CP;

			} else
				xConfig = BAD_CP;

		} else if(ulLen >= 5 && !strncmp((char *) pucBuffer, "HR", 2) && (pucBuffer[2] == '+' || pucBuffer[2] == '-') &&  pucBuffer[3] == ' '){

			ucNEW_HR_index = atoi((char *) &xPack.Buf[4]);

			if(ucIndex_In_Bound(ucNEW_HR_index)){

				xSemaphoreTake(xCodesSem, portMAX_DELAY);
				{
					if (pucBuffer[2] == '+') {
						codesStruct.CODE_INDEX_HR_UP = ucNEW_HR_index;
					} else {
						codesStruct.CODE_INDEX_HR_DOWN = ucNEW_HR_index;
					}

					xSemaphoreGive(xFlashSaveSem);
				}
				xSemaphoreGive(xCodesSem);

				xConfig = NEW_INDEX;

			} else
				xConfig = BAD_INDEX;

		}else if(ulLen == 5 && !strncmp((char *) pucBuffer, "reset", 5)){

			xSemaphoreTake(xCodesSem, portMAX_DELAY);
			{
				for(int i = 0; i < MAX_CODES; i++)
					codesStruct.codes_arr[i] = 0;

				codesStruct.CODE_INDEX_HR_UP = 0;
				codesStruct.CODE_INDEX_HR_DOWN = 0;
				codesStruct.codes_pointer = 0;

				xSemaphoreGive(xFlashSaveSem);
			}
			xSemaphoreGive(xCodesSem);

			xConfig = RESET_STRUCT;

		}else if(ulLen == 1 && xSemaphoreTake(xWaitingResponseSem, 0)){
			if(pucBuffer[0] == 'y' || pucBuffer[0] == 'Y'){
				xResponse = pdTRUE;

				xConfig = NEW_CODE;
			}
			else {
				xResponse = pdFALSE;

				xConfig = DISCARD_CODE;
			}

			xQueueSendToBack(xReceiver_to_NewCodeSav_Queue, &xResponse, 0);

		} else if(ulLen == 5 && !strncasecmp((char *) pucBuffer, "codes", 5)){
			xConfig = CODES;

		} else
			xConfig = ILLEGAL;


		xQueueSendToBack(xReceiver_to_Comm_Queue, &xConfig, 0);
	}

	vTaskDelete( NULL );
}

void vFlashSaver(void *pvParameters){
	uint32_t *pCodeStruct = &codesStruct;

	for(;;){

		xSemaphoreTake(xFlashSaveSem, portMAX_DELAY);

		Flash_Write_Data(USER_FLASH_MEMORY, pCodeStruct, MAX_CODES + 3);

	}

	vTaskDelete( NULL );

}

/* USER CODE BEGIN 4 */
void SW_PressEvent(void){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(xNewCodeSem, &xHigherPriorityTaskWoken);
	LED_Toggle(ledGreen);
}

void CONSOLE_ReceiveMessageCallback(volatile uint8_t *buf, uint32_t len){

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	receive_message_TypeDef xPack;

	memcpy(xPack.Buf, buf, len);
	xPack.Len = len;

	xQueueSendToBackFromISR(xReciever_Queue, &xPack, &xHigherPriorityTaskWoken);

}

uint8_t ucIndex_In_Bound(uint32_t index){
	return (0 <= index && index < MAX_CODES);
}

void vPrintCodes(void){
	uint8_t buf[55];
	uint8_t HR_UP_IndexBuf[10] = "<~~ HR+";
	uint8_t HR_DOWN_IndexBuf[10] = "<~~ HR-";
	uint8_t CPBuf[10] = "<~~ CP";
	for(int i = 0; i < MAX_CODES; i++){
		sprintf(buf, "COMMANDO %i: %lu", i, codesStruct.codes_arr[i]);

		if(i == codesStruct.codes_pointer)
			strcat(buf, CPBuf);
		if(i == codesStruct.CODE_INDEX_HR_UP)
			strcat(buf, HR_UP_IndexBuf);
		if(i == codesStruct.CODE_INDEX_HR_DOWN)
			strcat(buf, HR_DOWN_IndexBuf);

		strcat(buf, "\r\n");
		CONSOLE_SendMessage(buf, strlen((char *) buf));
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
