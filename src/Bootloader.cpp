/*
 * Bootloader.cpp
 *
 *  Created on: 29 но€б. 2016 г.
 *      Author: Bogdan
 */

#include "Bootloader.h"

bool UserStartCondition();
void UserStartApplication();

Bootloader::Bootloader() {
	// TODO Auto-generated constructor stub

}

Bootloader::~Bootloader() {
	// TODO Auto-generated destructor stub
}

void Bootloader::start() {

	if (UserStartCondition()) {
		UserStartApplication();
	}
}

void Bootloader::init() {

	bootloader_mb_.init();
}

void Bootloader::poll() {

	bootloader_mb_.poll();
}
