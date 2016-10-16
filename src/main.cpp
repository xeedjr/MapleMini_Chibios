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
#include "ReleGPIO.h"
#include "Button.h"

using namespace chibios_rt;

std::unique_ptr<MBComunication> mb_comunication;
std::unique_ptr<BL> bl;
std::unique_ptr<ReleGPIO> rele1;
std::unique_ptr<Button> button1;

#if HAL_USE_EXT
static EXTConfig extcfg = {
  { {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
	{EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, NULL},
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
	if (mb_comunication) {
		return mb_comunication->eMBRegHoldingCB(pucRegBuffer,
											usAddress,
											usNRegs,
											eMode);
	} else {
	    return MB_ENOREG;
	}
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
  //System::init();

  BaseThread::sleep(S2ST(2));

	eMBInit( MB_RTU, 1, 1, 115200, MB_PAR_NONE );

	xMBPortPollThreadInit();

	rele1.reset(new ReleGPIO(USER_RELE1_PORT, USER_RELE1));
	bl.reset(new BL);
	mb_comunication.reset(new MBComunication);
	button1.reset(new Button(USER_BUTTON_PORT,
								USER_BUTTON,
								[](){
									BL::Events ev;
									ev.ev_type = BL::Events::kOpenRele;
									ev.events.open_rele.open_close = true;
									bl->put_event(ev);
								},
								[](){
									BL::Events ev;
									ev.ev_type = BL::Events::kOpenRele;
									ev.events.open_rele.open_close = false;
									bl->put_event(ev);
								}));

	  extcfg.channels[7].cb = [](EXTDriver *extp, expchannel_t channel){
										//System::lockFromIsr();
										button1->event();
										//System::unlockFromIsr();
								  };
	  extStart(&EXTD1, &extcfg);

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
