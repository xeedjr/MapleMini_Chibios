/*
 * Servo.cpp
 *
 *  Created on: 03 но€б. 2016 г.
 *      Author: Bogdan
 */

#include "Servo.h"

Servo::Servo() {
	// TODO Auto-generated constructor stub

}

Servo::~Servo() {
	// TODO Auto-generated destructor stub
}

void Servo::Set(int8_t degree) {
	uint16_t deg = 0;

	if (degree != 0) {
		deg = 750 + (int32_t)((float)degree * 5.27); //2,7 like in documentation
	} else {
		deg = 750;
	}
	pwmEnableChannel(pwmp_, ch_, PWM_PERCENTAGE_TO_WIDTH(pwmp_, deg)); ///< 3%
}
