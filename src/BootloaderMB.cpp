/*
 * BootloaderMB.cpp
 *
 *  Created on: 04 дек. 2016 г.
 *      Author: Bogdan
 */

#include "BootloaderMB.h"

Bootloader_MB* bootloader_mb_p = nullptr;

#define REG_HOLDING_START 1

Bootloader_MB::Bootloader_MB() {
	// TODO Auto-generated constructor stub
	bootloader_mb_p = this;
}

Bootloader_MB::~Bootloader_MB() {
	// TODO Auto-generated destructor stub
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	return bootloader_mb_p->eMBRegHoldingCB(pucRegBuffer, usAddress, usNRegs, eMode);
}

eMBErrorCode
Bootloader_MB::eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + (sizeof(holding_reg_)/2) ) )
    {
        iRegIndex = ( int )( usAddress - REG_HOLDING_START );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( uint8_t ) ( ((uint16_t*)&holding_reg_)[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( uint8_t ) ( ((uint16_t*)&holding_reg_)[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
            	((uint16_t*)&holding_reg_)[iRegIndex] = *pucRegBuffer++ << 8;
            	((uint16_t*)&holding_reg_)[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

void Bootloader_MB::init() {

	eMBInit( MB_RTU, 1, 1, 115200, MB_PAR_NONE );

	eMBEnable();
}

void Bootloader_MB::poll() {

	eMBPoll();

	if (holding_reg_.pageSize > 0) {
		/// we have data for write in write it
		linearFlashProgramStart(&flash_helper);
		linearFlashProgram(&flash_helper,
							holding_reg_.pageAddress,
							holding_reg_.pageBuffer,
							holding_reg_.pageSize);
		linearFlashProgramFinish(&flash_helper);
		holding_reg_.pageSize = 0;
	}
}
