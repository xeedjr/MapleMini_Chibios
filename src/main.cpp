//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "ch.hpp"
#include "hal.h"
#include "cmsis_os.h"
#include "mb.h"
#include "MBComunication.h"
#include "BL.h"
#include "BLExtractor.h"
#include "ReleGPIO.h"
#include "Button.h"
#include "ButtonsMain.h"
#include "SensorButton.h"
#include "Servo.h"
#include "ESP8266Parser.h"
#include "BackupRegister.h"

using namespace chibios_rt;

std::unique_ptr<MBComunication> mb_comunication;
std::unique_ptr<BLExtractor> bl;
std::unique_ptr<ReleGPIO> rele1;
std::unique_ptr<ESP8266Parser> esp;

/*
 * I2C1 config.
 */
static const I2CConfig i2cfg1 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_2,
};

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	if (mb_comunication) {
		return mb_comunication->eMBRegHoldingCB(pucRegBuffer,
											usAddress,
											usNRegs,
											eMode);
	} else {
	    return MB_ENOREG;
	}
}

eMBException
eMBFuncPacket( UCHAR * pucFrame, USHORT * usLen )
{
	if (mb_comunication) {
		return mb_comunication->eMBFuncPacket(pucFrame,
												usLen);
	} else {
	    return MB_EX_NONE;
	}

    return MB_EX_NONE;
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  osKernelInitialize();

  //stm32_backup_registers.write(1, 1);
  	uint16_t address = stm32_backup_registers.read(1);
  	uint8_t address1 = address & 0x0F;
	eMBInit( MB_RTU, address1, 1, 115200, MB_PAR_NONE );
	eMBRegisterCB( 25, eMBFuncPacket);

	xMBPortPollThreadInit();

	rele1.reset(new ReleGPIO(USER_RELE1_PORT, USER_RELE1));
	bl.reset(new BLExtractor);
	mb_comunication.reset(new MBComunication);

//	  esp.reset(new ESP8266Parser);

	eMBEnable();

	i2cStart(&I2CD1, &i2cfg1);

  /*
   * Serves timer events.
   */
  while (true) {
	palSetPad(INDICATE_PORT, INDICATE);       /* Orange.  */
	osDelay(500);
	palClearPad(INDICATE_PORT, INDICATE);     /* Orange.  */
	osDelay(500);
  }

  return 0;
}

// ----------------------------------------------------------------------------
