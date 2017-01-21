/*
 * SI7021.cpp
 *
 *  Created on: 07 џэт. 2017 у.
 *      Author: Bogdan
 */

#include "hal.h"

#include <SI7021.h>

void SI7021::read_humidity(float& humidity) {
	  i2cAcquireBus(&I2CD1);
	  msg_t status = MSG_OK;
	  /* buffers */
	  static uint8_t si7021_rx_data[8];
	  static uint8_t si7021_tx_data[8];

	  si7021_tx_data[0] =  0xE5;
	  status = i2cMasterTransmitTimeout(i2cp_, 0b1000000,
			  si7021_tx_data, 1, si7021_rx_data, 2, MS2ST(1000));
	  uint16_t humidity_code = 0;
	  humidity_code = (si7021_rx_data[0] << 8) + si7021_rx_data[1];
	  humidity = ((125 * float(humidity_code)) / 65536) - 6;

	  i2cReleaseBus(&I2CD1);
}

void SI7021::read_temperature(float& temperature) {
	  i2cAcquireBus(&I2CD1);
	  msg_t status = MSG_OK;
	  /* buffers */
	  static uint8_t si7021_rx_data[8];
	  static uint8_t si7021_tx_data[8];

	  si7021_tx_data[0] =  0xE3;
	  status = i2cMasterTransmitTimeout(i2cp_, 0b1000000,
			  si7021_tx_data, 1, si7021_rx_data, 2, MS2ST(1000));
	   int16_t temperature_code = 0;
	  temperature_code = (si7021_rx_data[0] << 8) + si7021_rx_data[1];
	  temperature = ((175.72 * float(temperature_code)) / 65536) - 46.85;

	  i2cReleaseBus(&I2CD1);
}
