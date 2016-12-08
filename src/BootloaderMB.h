/*
 * BootloaderMB.h
 *
 *  Created on: 04 дек. 2016 г.
 *      Author: Bogdan
 */

#ifndef BOOTLOADERMB_H_
#define BOOTLOADERMB_H_

#include "stdint.h"
#include "mb.h"
#include "helper.h"

class Bootloader_MB {
	struct HoldingReg {
		uint32_t pageAddress;
		uint16_t pageSize;
		uint16_t pageBuffer[128];
	} holding_reg_;

	struct LinearFlashing flash_helper;
public:
	Bootloader_MB();
	virtual ~Bootloader_MB();

	eMBErrorCode
	eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
	                 eMBRegisterMode eMode );
	void init();
	void poll();
};

#endif /* BOOTLOADERMB_H_ */
