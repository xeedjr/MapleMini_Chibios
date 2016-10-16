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

extern std::unique_ptr<ReleGPIO> rele1;

BL::BL() {
	bl_ev[0].ev_type = Events::kOpenRele;
	bl_ev[0].fun = std::bind(&BL::open_rele,
								this,
								std::placeholders::_1);

	thread_ID = osThreadCreate(osThread(BL_Thread), this);
}

BL::~BL() {
	// TODO Auto-generated destructor stub
}

void BL::open_rele(Events::Event msg) {

	rele1->Set(msg.open_rele.open_close);
}

void BL::put_event(Events ev) {
	events_queue_.push(ev);
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
