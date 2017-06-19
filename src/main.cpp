//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <new>

#include "ch.hpp"
#include "hal.h"
#include "cmsis_os.h"
#include "mb.h"
#include "uip.h"

#include "MBComunication.h"
#include "BLExtractor.h"
#include "ReleGPIO.h"
#include "Button.h"
#include "ButtonsMain.h"
#include "SensorButton.h"
#include "Servo.h"
#include "ESP8266Parser.h"
#include "BackupRegister.h"
#include "TCPMB.h"
#include "TCPClient.h"
#include "PBFunct.h"

using namespace chibios_rt;

std::unique_ptr<MBComunication> mb_comunication;
uint8_t mb_comunication_r[sizeof(MBComunication)];
std::unique_ptr<BLExtractor> bl;
uint8_t bl_r[sizeof(BLExtractor)];
std::unique_ptr<ReleGPIO> rele1;
uint8_t rele1_r[sizeof(ReleGPIO)];
//std::unique_ptr<ESP8266Parser> esp;
//uint8_t esp_r[sizeof(ESP8266Parser)];
std::unique_ptr<TCPMB> tvp_mb;
uint8_t tvp_mb_r[sizeof(TCPMB)];
std::unique_ptr<TCPClient> tcp_client;
uint8_t tcp_client_r[sizeof(TCPClient)];
std::unique_ptr<PBFunct> pb_funct;
uint8_t pb_funct_r[sizeof(PBFunct)];

/*
 * I2C1 config.
 */
static const I2CConfig i2cfg1 = {
    OPMODE_I2C,
    400000,
    FAST_DUTY_CYCLE_2,
};


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

  stm32_backup_registers.write(1, 1);
  	uint16_t address = stm32_backup_registers.read(1);
  	uint8_t address1 = address & 0x0F;
	eMBInit( MB_RTU, address1, 1, 115200, MB_PAR_NONE );
	eMBRegisterCB( 25, eMBFuncPacket);

	xMBPortPollThreadInit();

	pb_funct.reset(new (pb_funct_r) PBFunct);

	rele1.reset(new (rele1_r) ReleGPIO(USER_RELE1_PORT, USER_RELE1));
	bl.reset(new (bl_r) BLExtractor);
	mb_comunication.reset(new (mb_comunication_r) MBComunication);

//	  esp.reset(new ESP8266Parser);

	eMBEnable();

	i2cStart(&I2CD1, &i2cfg1);

	tvp_mb.reset(new (tvp_mb_r) TCPMB());
	tcp_client.reset(new (tcp_client_r) TCPClient);

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
