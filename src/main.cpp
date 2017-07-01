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
#include "uip.h"
#include "enc28j60.h"

#include "ReleGPIO.h"
#include "Button.h"
#include "ButtonsMain.h"
#include "SensorButton.h"
#include "Servo.h"
#include "BackupRegister.h"
#include "TCPMB.h"
#include "EthSwitch.h"

using namespace chibios_rt;

std::unique_ptr<TCPMB> tvp_mb;
uint8_t tvp_mb_r[sizeof(TCPMB)];
std::unique_ptr<EthSwitch> eth_switch;
uint8_t eth_switch_r[sizeof(EthSwitch)];


/*
 * Low speed SPI configuration (281.250kHz, CPHA=0, CPOL=0, MSb first).
 */
static SPIConfig enc28j60_spicfg = {
  NULL,
  ENC28J60_CS_PORT,
  ENC28J60_CS,
  SPI_CR1_BR_1
};
static ENC28J60Config enc28j60_config = {
		&SPID1,
		&enc28j60_spicfg,
		ENC28J60_RESET_PORT,
		ENC28J60_RESET
};
ENC28J60Driver enc28j60;

static void enc28j60Interrupt(EXTDriver *extp, expchannel_t channel) {

  (void)extp;
  (void)channel;

  chSysLockFromISR();

  eth_switch->NetIfNewPacket();

  chSysUnlockFromISR();
}

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, enc28j60Interrupt},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};



uint8_t packet[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF , 0xFF , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x08 , 0x06 , 0x00 , 0x01 , 0x08 , 0x00 , 0x06 , 0x04 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00};

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

	enc28j60ObjectInit(&enc28j60);
	tvp_mb.reset(new (tvp_mb_r) TCPMB());

	eth_switch.reset(new (eth_switch_r) EthSwitch());

	enc28j60Start(&enc28j60, &enc28j60_config);
	enc28j60EnablePromiscuous(&enc28j60, true);
	extStart(&EXTD1, &extcfg);

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
