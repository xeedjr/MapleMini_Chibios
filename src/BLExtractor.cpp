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
	state_table[2].ev_type = Events::kProtoMsg;
	state_table[2].fun = std::bind(&BLExtractor::proto_message,
								this,
								std::placeholders::_1);
	bl_main.SetNewState(state_table, 3);

/*	thread_ID = osThreadCreate(osThread(BLExtractor_Thread), this);
	if (thread_ID == nullptr) {
		/// Error
		chSysHalt("Error create thread");
	}
	thread_ID->p_name = "BLExtractor_Thread";

	si7021_.init(&I2CD1);

	/// create period read humidity sensor data
	timer0 = osTimerCreate(osTimer(timer0_handle), osTimerPeriodic, (void *)this);
	if (timer0 == nullptr) {
		/// Error
		chSysHalt("Error create timer0");
	}
	osTimerStart (timer0, 100);
*/
}

BLExtractor::~BLExtractor() {
	// TODO Auto-generated destructor stub
}

void BLExtractor::change_fan_speed(Events::Event* message) {
	curr_fan_speed_ = message->change_fan_speed.speed;
	if (message->change_fan_speed.speed == 0) {
		/// Off
		rele1->Set(true);
	} else {
		/// On
		rele1->Set(false);
	}

	/// response execute code
	mb_comunication->send_resp_OK(Commands_change_speed_tag);
}

void BLExtractor::proto_message(Events::Event* message) {
	MBMessage msg = MBMessage_init_zero;
	mb_comunication->decode_message(msg,
									message->proto_msg.message,
									message->proto_msg.len);
	switch(msg.which_interface) {
	case MBMessage_commands_tag:
		switch(msg.interface.commands.which_cmd) {
		case Commands_change_speed_tag:
			{
				ev_pair.ev_type = Events::kChangeFanSpeed;
				ev_pair.events.change_fan_speed.speed = msg.interface.commands.cmd.change_speed.fan_speed;
				put_event(ev_pair);
			}
			break;
		}
		break;
	}
}

void BLExtractor::read_measurements(Events::Event* message) {
	float temperature = 0, hummidity = 0;
	float percent = 0.5;

	si7021_.read_temperature(temperature);
	si7021_.read_humidity(hummidity);

	if (((hummidity < (humidity_ - 0.9)) || ((humidity_ + 0.9) < hummidity)) ||
		((temperature < (temperature_ - 0.5)) || ((temperature_ + 0.5) < temperature))) {
		/// values changed
		/// Store
		humidity_ = hummidity;
		temperature_ = temperature;

		/// Send notification
		mb_comunication->send_notification_temp_humidity(humidity_,
				temperature_,
				curr_fan_speed_);
	}
}

void BLExtractor::put_event(Events ev) {
	put_event_pair.ev_type = ev.ev_type;
	put_event_pair.ev_data = ev.events;
	bl_main.PutEvent(put_event_pair);
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
