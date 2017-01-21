/*
 * SensorButton.h
 *
 *  Created on: 06 но€б. 2016 г.
 *      Author: Bogdan
 */

#ifndef SENSORBUTTON_H_
#define SENSORBUTTON_H_

#include <functional>

#include "cmsis_os.h"
#include "hal.h"

class SensorButton {
	bool is_pushed_old_state_ = false;
	ioportid_t port_;
	uint8_t pin_;
	std::function<void(void)> pushed_cb_;
	std::function<void(void)> released_cb_;
	std::function<void(void)> timer_cb_;
    std::function<void(void)> ext_interrupt_;
	static void static_timer_handler_(void const *argument);
	void timer_handler_();
	void event_process_();
public:
	SensorButton(ioportid_t port,
				uint8_t pin,
				std::function<void(void)> pushed_cb,
				std::function<void(void)> released_cb);
	virtual ~SensorButton();

	void event();
};

#endif /* SENSORBUTTON_H_ */
