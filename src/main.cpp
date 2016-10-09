//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "ch.hpp"
#include "hal.h"
#include "cmsis_os.h"
#include "mb.h"

using namespace chibios_rt;


#if HAL_USE_EXT
static EXTConfig extcfg = {
  {
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, NULL},
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
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};
#endif


eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	if (eMode == MB_REG_READ) {
		*pucRegBuffer = 1;
		return MB_ENOERR;
	}
    return MB_ENOREG;
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
  System::init();

  BaseThread::sleep(S2ST(2));

	eMBInit( MB_RTU, 1, 1, 115200, MB_PAR_NONE );

	xMBPortPollThreadInit();

	eMBEnable();

  /*
   * Serves timer events.
   */
  while (true) {
	palSetPad(INDICATE_PORT, INDICATE);       /* Orange.  */
	BaseThread::sleep(MS2ST(500));
	palClearPad(INDICATE_PORT, INDICATE);     /* Orange.  */
	BaseThread::sleep(MS2ST(500));
  }

  return 0;
}

// ----------------------------------------------------------------------------
