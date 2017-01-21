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
#include "ButtonsMain.h"
#include "SensorButton.h"
#include "Servo.h"
#include "ESP8266Parser.h"

using namespace chibios_rt;

std::unique_ptr<MBComunication> mb_comunication;
std::unique_ptr<BL> bl;
std::unique_ptr<ReleGPIO> rele1;
std::unique_ptr<Button> button1;
std::unique_ptr<ButtonsMain> buttons_main;
std::unique_ptr<SensorButton> sensor_button;
std::unique_ptr<Servo> servo;
std::unique_ptr<ESP8266Parser> esp;


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

#if HAL_USE_PWM
PWMConfig pwmcfg = {
  100000,                                    /* 10kHz PWM clock frequency.   */
  2000,                                    /* Initial PWM period 20ms.       */
  NULL,
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0,
#if STM32_PWM_USE_ADVANCED
  0
#endif
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

  osDelay(500);

	eMBInit( MB_RTU, 1, 1, 115200, MB_PAR_NONE );

	xMBPortPollThreadInit();

	rele1.reset(new ReleGPIO(USER_RELE1_PORT, USER_RELE1));
	bl.reset(new BL);
	mb_comunication.reset(new MBComunication);
/*	button1.reset(new Button(USER_BUTTON_PORT,
								USER_BUTTON,
								[](){
									BL::Events ev;
									ev.ev_type = BL::Events::kSensor;
									ev.events.sensor.sensor_state = true;
									ev.events.sensor.sensor_id = 1;
									bl->put_event(ev);
								},
								[](){
									BL::Events ev;
									ev.ev_type = BL::Events::kSensor;
									ev.events.sensor.sensor_state = false;
									ev.events.sensor.sensor_id = 1;
									bl->put_event(ev);
								}));

	  buttons_main.reset(new ButtonsMain);
	  buttons_main->add(kMainButton, &(*button1));

	  extcfg.channels[7].cb = [](EXTDriver *extp, expchannel_t channel){
		  	  	  	  	  	  	  	  	chSysLockFromISR();
		  	  	  	  	  	  	  	  	buttons_main->event(kMainButton);
										chSysUnlockFromISR();
								  };
*/
	sensor_button.reset(new SensorButton(USER_BUTTON_PORT,
									USER_BUTTON,
									[](){
										BL::Events ev;
										ev.ev_type = BL::Events::kSensor;
										ev.events.sensor.sensor_state = true;
										ev.events.sensor.sensor_id = 1;
										bl->put_event(ev);
									},
									[](){
										BL::Events ev;
										ev.ev_type = BL::Events::kSensor;
										ev.events.sensor.sensor_state = false;
										ev.events.sensor.sensor_id = 1;
										bl->put_event(ev);
									}));
	  extcfg.channels[7].cb = [](EXTDriver *extp, expchannel_t channel){
		  	  	  	  	  	  	  	  	//chSysLockFromISR();
										sensor_button->event();
										//chSysUnlockFromISR();
								  };

/*	  pwmStart(&PWMD3, &pwmcfg);
	  servo.reset(new Servo);
	  servo->Init(&PWMD3, 3);

	  servo->Set(-90);
	  servo->Set(0);
	  servo->Set(90);
*/
	  extStart(&EXTD1, &extcfg);

	  esp.reset(new ESP8266Parser);

	eMBEnable();

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
