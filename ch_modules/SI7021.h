/*
 * SI7021.h
 *
 *  Created on: 07 џэт. 2017 у.
 *      Author: Bogdan
 */

#ifndef SI7021_H_
#define SI7021_H_

#include "hal.h"

class SI7021 {
	I2CDriver *i2cp_ = nullptr;
public:
	SI7021(I2CDriver *i2cp) {
		i2cp_ = i2cp;
	};
	SI7021() {};
	virtual ~SI7021() {};
	void init(I2CDriver *i2cp) {
		i2cp_ = i2cp;
	};

	void read_humidity(float& humidity);
	void read_temperature(float& temperature);
};

#endif /* SI7021_H_ */
