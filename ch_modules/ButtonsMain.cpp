/*
 * ButtonsMain.cpp
 *
 *  Created on: 05 но€б. 2016 г.
 *      Author: Bogdan
 */

#include <ButtonsMain.h>

ButtonsMain::ButtonsMain() {
	thread_ID = osThreadCreate(osThread(ButtonsMain_thread), NULL);
	if (thread_ID == NULL) {
		//abort();
	}
	thread_ID->p_name = "mb_thread";
}

ButtonsMain::~ButtonsMain() {
	// TODO Auto-generated destructor stub
}

void ButtonsMain::thread() {

}

/** @brief !!! Shall be in  chSysLockFromISR
 *
 */
void ButtonsMain::event(ButtonsMainEnum button) {

}
