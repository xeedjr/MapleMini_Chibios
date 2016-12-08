/*
 * Bootloader.h
 *
 *  Created on: 29 но€б. 2016 г.
 *      Author: Bogdan
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "BootloaderMB.h"

class Bootloader {
	Bootloader_MB bootloader_mb_;
public:
	Bootloader();
	virtual ~Bootloader();

	void start();
	void init();
	void poll();
};

#endif /* BOOTLOADER_H_ */
