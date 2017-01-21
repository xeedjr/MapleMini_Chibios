/*
 * MBComunication.cpp
 *
 *  Created on: 14 окт. 2016 г.
 *      Author: Bogdan
 */
#include <string.h>
#include <memory>
#include "MBComunication.h"
#include "BLExtractor.h"

extern std::unique_ptr<BLExtractor> bl;

MBComunication::MBComunication() {
	mb_thread_ID = osThreadCreate(osThread(MBComunication_Thread), this);
	mb_thread_ID->p_name = "MBComunication_Thread";
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

void MBComunication::put_event(Events ev) {
	events_queue_.push(ev);
}

void MBComunication::Thread (void) {
	while(1) {
		Events ev;
		events_queue_.pop(ev);

		switch(ev.ev_type) {
		case Events::kHoldingRegisterWrite:
			if (HoldingCheack(ev.events.holding_register_write.usAddress(),
								ev.events.holding_register_write.usNRegs(),
								registers_.holding.fan_speed)) {
				/// need change fan speed
				BLExtractor::Events ev;
				ev.ev_type = BLExtractor::Events::kChangeFanSpeed;
				ev.events.change_fan_speed.speed = registers_.holding.fan_speed;
				bl->put_event(ev);
			}
			break;
		case Events::kSensorState:
//			registers_.holding.sensor1 = ev.events.sensor_state.state;
			break;
		case Events::kUpdateParameters:
			Conv conv;
			registers_.holding.fan_speed = ev.events.update_parameters.fan_speed;
			conv.f = ev.events.update_parameters.humidity;
			registers_.holding.humidity_lo = conv.hr.lo;
			registers_.holding.humidity_hi = conv.hr.hi;
			conv.f = ev.events.update_parameters.temperature;
			registers_.holding.temperature_hi = conv.hr.hi;
			registers_.holding.temperature_lo = conv.hr.lo;
			conv.f = ev.events.update_parameters.humidity_level_on;
			registers_.holding.humidity_level_on_hi = conv.hr.hi;
			registers_.holding.humidity_level_on_lo = conv.hr.lo;
			conv.f = ev.events.update_parameters.humidity_level_off;
			registers_.holding.humidity_level_off_hi = conv.hr.hi;
			registers_.holding.humidity_level_off_lo = conv.hr.lo;
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
		memcpy(pucRegBuffer, &((uint16_t*)&registers_.holding)[usAddress - 1], usNRegs * sizeof(uint16_t));
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
