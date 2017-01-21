/*
 * BL.cpp
 *
 *  Created on: 15 окт. 2016 г.
 *      Author: Bogdan
 */

#include <memory>
#include <string.h>
#include "BL.h"
#include "ReleGPIO.h"
#include "MBComunication.h"

extern std::unique_ptr<ReleGPIO> rele1;
extern std::unique_ptr<MBComunication> mb_comunication;

BL::BL() {
	bl_ev[0].ev_type = Events::kOpenRele;
	bl_ev[0].fun = std::bind(&BL::open_rele,
								this,
								std::placeholders::_1);
	bl_ev[1].ev_type = Events::kSensor;
	bl_ev[1].fun = std::bind(&BL::sensor,
								this,
								std::placeholders::_1);

	thread_ID = osThreadCreate(osThread(BL_Thread), this);
	if (thread_ID == nullptr) {
		/// Error
		chSysHalt("Error create thread");
	}
	thread_ID->p_name = "BL_Thread";
}

BL::~BL() {
	// TODO Auto-generated destructor stub
}

void BL::open_rele(Events::Event msg) {

	rele1->Set(msg.open_rele.open_close);
}

void BL::sensor(Events::Event msg) {
	MBComunication::Events ev;

	ev.ev_type = MBComunication::Events::kSensorState;
	ev.events.sensor_state.state = msg.sensor.sensor_state;
	ev.events.sensor_state.id = msg.sensor.sensor_id;

	mb_comunication->put_event(ev);
}

void BL::put_event(Events ev) {
	events_queue_.push(ev);
}

void BL::BL_Thread (void const *argument) {
	BL* p = (BL*)(argument);

	p->Thread();
}

void BL::Thread (void) {
	while(1) {
		Events ev;
		events_queue_.pop(ev);

		for (int i = 0; i < bl_ev.size(); i++) {
			if (bl_ev[i].ev_type == ev.ev_type) {
				bl_ev[i].fun(ev.events);
			}
		}

	}
}
