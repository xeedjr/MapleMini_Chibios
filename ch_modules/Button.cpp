/*
 * Button.cpp
 *
 *  Created on: 20 марта 2016 г.
 *      Author: Bogdan
 */

#include "Button.h"

Button::Button(ioportid_t port,
		uint8_t pin,
		std::function<void(void)> pushed_cb,
		std::function<void(void)> released_cb) {
	port_ = port;
	pin_ = pin;
	pushed_cb_ = pushed_cb;
	released_cb_ = released_cb;

	timer_id = osTimerCreate(osTimer(timer_), osTimerOnce, this);
}

Button::~Button() {
}

void Button::event_process_() {
	switch(is_pushed_old_state_) {
	case false: /// un pushed state
	    if (palReadPad(port_, pin_) == 0) {
	    	/// pushed GPIOA_BUTTON
	    	/// received new state need send event and change old state
	    	is_pushed_old_state_ = true;
	    	/// start timer
	    	//timer_armed_ = true;
	    	osTimerStart(timer_id, 100);
	    	if (pushed_cb_)
	    		pushed_cb_();
	    };
		break;
	case true: /// pushed state
	    if (palReadPad(port_, pin_) == 1) {
	    	/// un pushed GPIOA_BUTTON
	    	is_pushed_old_state_ = false;
	    	/// start timer
	    	//timer_armed_ = true;
	    	osTimerStart(timer_id, 100);
	    	if (released_cb_)
	    		released_cb_();
	    };
		break;
	}
}

void Button::static_timer_handler_(void const *argument) {
	chSysLockFromISR();
	((Button*)(argument))->timer_handler_();
	chSysUnlockFromISR();
}

void Button::timer_handler_() {
	timer_cb_();
	//timer_armed_ = false;
	//event_process_();
}

/** @brief !!! Shall be in  chSysLockFromISR
 *
 */
void Button::event() {
	//if (!timer_armed_) {
		//event_process_();
	//};
}
