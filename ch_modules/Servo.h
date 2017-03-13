/*
 * Servo.h
 *
 *  Created on: 03 но€б. 2016 г.
 *      Author: Bogdan
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "hal.h"

/*
 * #if HAL_USE_PWM
PWMConfig pwmcfg = {
  100000,                                    // 10kHz PWM clock frequency.
  2000,                                    // Initial PWM period 20ms.
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

/*	  pwmStart(&PWMD3, &pwmcfg);
	  servo.reset(new Servo);
	  servo->Init(&PWMD3, 3);

	  servo->Set(-90);
	  servo->Set(0);
	  servo->Set(90);
*/


class Servo {
	PWMDriver *pwmp_ = nullptr;
	uint8_t ch_ = 0xFF;
public:
	Servo();
	void Init(PWMDriver *pwmp,
			uint8_t ch) {
		pwmp_ = pwmp;
		ch_ = ch;
	}
	void Set(int8_t degree);
	virtual ~Servo();
};

#endif /* SERVO_H_ */
