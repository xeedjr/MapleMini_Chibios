/*
 * GetUUID.cpp
 *
 *  Created on: 18 ���� 2017 �.
 *      Author: Bogdan
 */

#include <stdint.h>
#include <string.h>

void get_96bit_uuid_array(uint8_t* buff) {
	memcpy(buff,
			(void*)(0x1FFFF7E8),
			12);
}


