/*
 * user_functions.c
 *
 *  Created on: 29 но€б. 2016 г.
 *      Author: Bogdan
 */

#include "hal.h"

// Flash configuration
#define MAIN_PROGRAM_START_ADDRESS              (uint32_t)0x08005000
#define MAIN_PROGRAM_PAGE_NUMBER                20
#define NUM_OF_PAGES                            256

#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)

bool UserStartCondition() {
	return true;
}

void UserStartApplication() {
	typedef void (*pFunction)(void);
	pFunction Jump_To_Application = nullptr;
	uint32_t jumpAddress = 0;

	port_disable();
	  /* Initialization of the vector table and priority related settings.*/
	  //SCB->VTOR = MAIN_PROGRAM_START_ADDRESS - (uint32_t)0x08000000;
	//SCB->VTOR = MAIN_PROGRAM_START_ADDRESS;
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, MAIN_PROGRAM_START_ADDRESS);
	SCB->VTOR = NVIC_VectTab_FLASH | (MAIN_PROGRAM_START_ADDRESS & (uint32_t)0x1FFFFF80);

	jumpAddress = *((__IO uint32_t*) (MAIN_PROGRAM_START_ADDRESS + 4));
	Jump_To_Application = (pFunction) jumpAddress;
	__set_MSP(*(__IO uint32_t*) MAIN_PROGRAM_START_ADDRESS);
	Jump_To_Application();
}
