/*
 * BackupRegister.h
 *
 *  Created on: 12 џэт. 2017 у.
 *      Author: Bogdan
 */

#ifndef BACKUPREGISTER_H_
#define BACKUPREGISTER_H_

#include <stdint.h>

class BackupRegister {
public:
	BackupRegister();
	virtual ~BackupRegister();

	void write(uint8_t reg, uint16_t val);
	uint16_t read(uint8_t reg);
};

extern BackupRegister stm32_backup_registers;

#endif /* BACKUPREGISTER_H_ */
