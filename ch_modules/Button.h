/*
 * Button.h
 *
 *  Created on: 20 марта 2016 г.
 *      Author: Bogdan
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <functional>

#include "cmsis_os.h"
#include "ch.hpp"
#include "hal.h"


class Button {
	osTimerDef(timer_, Button::static_timer_handler_);
	osTimerId timer_id = 0;
	//bool timer_armed_ = false;

	//chibios_rt::Timer timer_;
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
	Button(ioportid_t port,
			uint8_t pin,
			std::function<void(void)> pushed_cb,
			std::function<void(void)> released_cb);
	~Button();

	void set_event_cb(std::function<void(void)> timer_cb,
					  std::function<void(void)> ext_interrupt) {
		timer_cb_ = timer_cb;
		ext_interrupt_ = ext_interrupt;
	};

	void event();
};

#endif /* BUTTON_H_ */
