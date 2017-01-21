/*
 * BackupRegister.cpp
 *
 *  Created on: 12 џэт. 2017 у.
 *      Author: Bogdan
 */

#include <BackupRegister.h>
#include "hal.h"

BackupRegister stm32_backup_registers;

BackupRegister::BackupRegister() {
	// TODO Auto-generated constructor stub

}

BackupRegister::~BackupRegister() {
	// TODO Auto-generated destructor stub
}

void BackupRegister::write(uint8_t reg, uint16_t val) {
	BKP_TypeDef* bkp = BKP;
	switch(reg) {
	case 1:
		bkp->DR1 = val;
		break;
	case 2:
		bkp->DR2 = val;
		break;
	case 3:
		bkp->DR3 = val;
		break;
	}
}

uint16_t BackupRegister::read(uint8_t reg) {
	BKP_TypeDef* bkp = BKP;
	switch(reg) {
	case 1:
		return bkp->DR1;
		break;
	case 2:
		return bkp->DR2;
		break;
	case 3:
		return bkp->DR3;
		break;
	}
}
