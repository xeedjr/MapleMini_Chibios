/*
 * BLExtractor.cpp
 *
 *  Created on: 15 окт. 2016 г.
 *      Author: Bogdan
 */

#include <memory>
#include <string.h>
#include "BLExtractor.h"
#include "ReleGPIO.h"
#include "MBComunication.h"

extern std::unique_ptr<ReleGPIO> rele1;
extern std::unique_ptr<MBComunication> mb_comunication;


BLExtractor::BLExtractor() {
	state_table[0].ev_type = Events::kChangeFanSpeed;
	state_table[0].fun = std::bind(&BLExtractor::change_fan_speed,
								this,
								std::placeholders::_1);
	state_table[1].ev_type = Events::kReadMeasurements;
	state_table[1].fun = std::bind(&BLExtractor::read_measurements,
								this,
								std::placeholders::_1);
	bl_main.SetNewState(state_table, 2);

	thread_ID = osThreadCreate(osThread(BLExtractor_Thread), this);
	if (thread_ID == nullptr) {
		/// Error
		chSysHalt("Error create thread");
	}
	thread_ID->p_name = "BLExtractor_Thread";

	si7021_.init(&I2CD1);

	/// create period read humidity sensor data
	osTimerId timer0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)this);
	osTimerStart (timer0, 100);
}

BLExtractor::~BLExtractor() {
	// TODO Auto-generated destructor stub
}

void BLExtractor::change_fan_speed(Events::Event* message) {
	if (message->change_fan_speed.speed == 0) {
		/// Off
		rele1->Set(true);
	} else {
		/// On
		rele1->Set(false);
	}
}


void BLExtractor::read_measurements(Events::Event* message) {
	float temperature = 0, hummidity = 0;
	si7021_.read_temperature(temperature);
	si7021_.read_humidity(hummidity);

	humidity_ = hummidity;
	temperature_ = temperature;

	mb_comunication->send_notification_temp_humidity(humidity_,
			temperature_,
			0);
}

void BLExtractor::put_event(Events ev) {
	BL::EventPair ev_pair;
	ev_pair.ev_type = ev.ev_type;
	ev_pair.ev_data = ev.events;
	bl_main.PutEvent(ev_pair);
}

void BLExtractor::BLExtractor_Thread (void const *argument) {
	BLExtractor* p = (BLExtractor*)(argument);

	p->Thread();
}

void BLExtractor::Thread (void) {
	while(1) {
		bl_main.Iterate();
	}
}

void BLExtractor::BLExtractor_Timer (void const *argument) {
	BLExtractor* p = (BLExtractor*)(argument);

	p->Timer();
}

void BLExtractor::Timer (void) {
	BL::EventPair ev_pair;
	ev_pair.ev_type = Events::kReadMeasurements;
	bl_main.PutEventI(ev_pair);
}
