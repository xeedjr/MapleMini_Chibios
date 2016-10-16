/*
 * MBComunication.cpp
 *
 *  Created on: 14 окт. 2016 г.
 *      Author: Bogdan
 */
#include <string.h>
#include <memory>
#include "MBComunication.h"
#include "BL.h"

extern std::unique_ptr<BL> bl;

MBComunication::MBComunication() {
	mb_thread_ID = osThreadCreate(osThread(MBComunication_Thread), this);
}

MBComunication::~MBComunication() {
	// TODO Auto-generated destructor stub
}

bool MBComunication::HoldingCheack (uint16_t usAddress,
									uint16_t usNRegs,
									uint16_t& cheack_register) {
	uint16_t int_Address = ((uint8_t*)&registers_.holding - (uint8_t*)&cheack_register) + 1;

	if (usAddress <= int_Address &&
		int_Address < (usAddress + usNRegs))
		return true;
}

void MBComunication::Thread (void) {
	while(1) {
		Events ev;
		events_queue_.pop(ev);

		switch(ev.ev_type) {
		case Events::kHoldingRegisterWrite:
			if (HoldingCheack(ev.events.holding_register_write.usAddress(),
								ev.events.holding_register_write.usNRegs(),
								registers_.holding.rele1)) {
				/// need open rele
				BL::Events ev;
				ev.ev_type = BL::Events::kOpenRele;
				if (registers_.holding.rele1 == 1) {
					ev.events.open_rele.open_close = true;
				} else {
					ev.events.open_rele.open_close = false;
				}
				bl->put_event(ev);
			}
			break;
		}
	}
}

eMBErrorCode
MBComunication::eMBRegHoldingCB( UCHAR * pucRegBuffer,
								USHORT usAddress,
								USHORT usNRegs,
								eMBRegisterMode eMode ) {
	switch(eMode) {
	case MB_REG_READ:
		memcpy(pucRegBuffer, &((uint16_t*)&registers_.holding)[usAddress - 1], usNRegs);
		return MB_ENOERR;
	case MB_REG_WRITE:
		memcpy(&((uint16_t*)&registers_.holding)[usAddress - 1], pucRegBuffer, usNRegs * sizeof(uint16_t));
		Events ev;
		ev.ev_type = Events::kHoldingRegisterWrite;
		ev.events.holding_register_write.set(usAddress,
												usNRegs);
		events_queue_.push(ev);
		return MB_ENOERR;
	}

    return MB_ENOREG;

}
