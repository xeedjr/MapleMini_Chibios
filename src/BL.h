/*
 * BL.h
 *
 *  Created on: 15 окт. 2016 г.
 *      Author: Bogdan
 */

#ifndef BL_H_
#define BL_H_

#include <functional>
#include "cmsis_os.h"
#include "MailBox.h"

class BL {

public:
	class Events {
	public:
		enum EventType {
			kOpenRele = 0,
			kSensor
		} ev_type;
		union Event {
			struct OpenRele {
				bool open_close;
			} open_rele;
			struct Sensor {
				bool sensor_state;
				uint8_t sensor_id;
			} sensor;
		} events;
	};

	void put_event(Events ev);
private:
	MailBox<Events, 3> events_queue_;
	static void BL_Thread (void const *argument);
	void Thread (void);

	osThreadId thread_ID = {0};
	osThreadDef(BL_Thread, osPriorityNormal, 256);

	struct EventPair{
		Events::EventType ev_type;
		std::function<void(Events::Event)>  fun;
	};
	std::array<BL::EventPair, 2> bl_ev;
	void open_rele(Events::Event msg);
	void sensor(Events::Event msg);
public:
	BL();
	virtual ~BL();
};

#endif /* BL_H_ */
