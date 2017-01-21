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
#include "Bootloader.h"

using namespace chibios_rt;

Bootloader bootloader_main;

/*
 * Application entry point.
 */
int main(void) {

	bootloader_main.start();

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  bootloader_main.init();

  while (true) {
	  bootloader_main.poll();
  }

  return 0;
}

// ----------------------------------------------------------------------------
