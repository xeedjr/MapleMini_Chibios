/*
 * ReleGPIO.cpp
 *
 *  Created on: 16 окт. 2016 г.
 *      Author: Bogdan
 */

#include "ReleGPIO.h"

ReleGPIO::ReleGPIO(ioportid_t port,
					uint8_t pin) {
	port_ = port;
	pin_ = pin;
	Set(false);
}

ReleGPIO::~ReleGPIO() {
	// TODO Auto-generated destructor stub
}

void ReleGPIO::Set(bool open_close) {
	if (open_close == true) {
		palSetPad(port_, pin_);       /* Orange.  */
	} else {
		palClearPad(port_, pin_);     /* Orange.  */
	};
}
