/*
 * MBComunication.h
 *
 *  Created on: 14 окт. 2016 г.
 *      Author: Bogdan
 */

#ifndef MBCOMUNICATION_H_
#define MBCOMUNICATION_H_

#include "cmsis_os.h"
#include "mb.h"
#include "MailBox.h"

class MBComunication {
public:
	class Events {
	public:
		enum {
			kHoldingRegisterWrite = 0,
			kSensorState,
		} ev_type;
		union Event {
			class HoldingRegisterWrite {
				uint16_t usAddress_;
				uint16_t usNRegs_;
			public:
				void set(uint16_t usAddress,
						uint16_t usNRegs) {
					usAddress_ = usAddress;
					usNRegs_ = usNRegs;
				}
				uint16_t usAddress() {
					return usAddress_;
				}
				uint16_t usNRegs() {
					return usNRegs_;
				}
			} holding_register_write;
			struct kSensorState {
				bool state;
				uint8_t id;
			} sensor_state;
		} events;
	};
private:
	MailBox<Events, 3> events_queue_;
	void Thread (void);

	osThreadId mb_thread_ID = {0};
	static void MBComunication_Thread (void const *argument) {
		MBComunication* p = (MBComunication*)(argument);

		p->Thread();
	}
	osThreadDef(MBComunication_Thread, osPriorityNormal, 256);

	struct Registers {
		struct Holding {
			uint16_t rele1;
			uint16_t sensor1;
		} holding;
	} registers_;

	bool HoldingCheack (uint16_t usAddress,
						uint16_t usNRegs,
						uint16_t& cheack_register);
public:
	MBComunication();
	virtual ~MBComunication();

	eMBErrorCode
	eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
	                 eMBRegisterMode eMode );

	void put_event(Events ev);
};

#endif /* MBCOMUNICATION_H_ */
