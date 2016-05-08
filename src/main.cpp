//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "ch.hpp"
#include "hal.h"

using namespace chibios_rt;


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

  /*
   * Serves timer events.
   */
  while (true) {
//	palSetPad(GPIOD, GPIOD_LED5);       /* Orange.  */
	BaseThread::sleep(MS2ST(500));
//	palClearPad(GPIOD, GPIOD_LED5);     /* Orange.  */
	BaseThread::sleep(MS2ST(500));
//    if (palReadPad(GPIOA, GPIOA_BUTTON)) {
//
///    };
  }

  return 0;
}

// ----------------------------------------------------------------------------
