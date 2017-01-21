/*
 * ButtonsMain.h
 *
 *  Created on: 05 но€б. 2016 г.
 *      Author: Bogdan
 */

#ifndef BUTTONSMAIN_H_
#define BUTTONSMAIN_H_

#include "cmsis_os.h"
#include "Button.h"

//#define MAX_BUTTONS_NUM 5

enum ButtonsMainEnum {
	kMainButton = 0,
	kConunt
};

class ButtonsMain {
	std::array<Button*, kMainButton> buttons_;
	//std::array<Button*, kMainButton> buttons_;
	osThreadId thread_ID = {0};
	static void ButtonsMain_thread (void const *argument) {

		while(1) {

		}
	}
	osThreadDef(ButtonsMain_thread, osPriorityNormal, 50);

	void thread();
public:
	ButtonsMain();
	virtual ~ButtonsMain();

	void add(ButtonsMainEnum button, Button* buttonp) {
		buttons_.at(button) = buttonp;
	};
	void event(ButtonsMainEnum button);
};

#endif /* BUTTONSMAIN_H_ */
