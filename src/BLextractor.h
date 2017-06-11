/*
 * BLExtractor.h
 *
 *  Created on: 15 окт. 2016 г.
 *      Author: Bogdan
 */

#ifndef BLEXTRACTOR_H_
#define BLEXTRACTOR_H_

#include <functional>
#include "cmsis_os.h"
#include "MailBox.h"
#include "BLMain.h"
#include "SI7021.h"

class BLExtractor {

public:
	class Events {
	public:
		enum EventType {
			kChangeFanSpeed = 0,
			kMeasure,
			kUpdateValuesInMBRegisters,
			kReadMeasurements,
			kProtoMsg,
		} ev_type;

		union Event {
			struct ChangeFanSpeed {
				uint16_t speed;
			} change_fan_speed;
			struct ProtoMsg {
				uint8_t message [256];
				uint8_t len;
			} proto_msg;
		} events;
	};

	void put_event(Events ev);
private:
	static void BLExtractor_Thread (void const *argument);
	void Thread (void);

	osThreadId thread_ID = {0};
	osThreadDef(BLExtractor_Thread, osPriorityNormal, 256);

	static void BLExtractor_Timer (void const *argument);
	void Timer (void);
	osTimerId timer0;
	osTimerDef(timer0_handle, BLExtractor_Timer);

	SI7021 si7021_;

	uint16_t curr_fan_speed_ = 0;
	float humidity_ = 0;
	float temperature_ = 0;
	float humidity_level_on_ = 60;
	float humidity_level_off_ = 40;


public:
	BLExtractor();
	virtual ~BLExtractor();

	typedef BLMain<Events::EventType, Events::Event> BL;
	BL bl_main;

	BL::StateTable state_table[3];
	BL::EventPair put_event_pair;
	Events ev_pair;

	void change_fan_speed(Events::Event*);
	void proto_message(Events::Event*);
	void read_measurements(Events::Event* message);

};

#endif /* BLEXTRACTOR_H_ */
