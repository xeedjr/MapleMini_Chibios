/*
 * ReleGPIO.h
 *
 *  Created on: 16 окт. 2016 г.
 *      Author: Bogdan
 */

#ifndef RELEGPIO_H_
#define RELEGPIO_H_

#include "hal.h"

class ReleGPIO {
	ioportid_t port_;
	uint8_t pin_;
public:
	ReleGPIO(ioportid_t port,
			uint8_t pin);
	virtual ~ReleGPIO();
	void Set(bool open_close);
};

#endif /* RELEGPIO_H_ */
