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
			kOpenRele = 0
		} ev_type;
		union Event {
			struct OpenRele {
				bool open_close;
			} open_rele;
		} events;
	};

	void put_event(Events ev);
private:
	MailBox<Events, 3> events_queue_;
	void Thread (void);

	osThreadId thread_ID;
	static void BL_Thread (void const *argument) {
		BL* p = (BL*)(argument);

		p->Thread();
	}
	osThreadDef(BL_Thread, osPriorityNormal, 128);

	struct EventPair{
		Events::EventType ev_type;
		std::function<void(Events::Event)>  fun;
	};
	std::array<BL::EventPair, 1> bl_ev;
	void open_rele(Events::Event msg);
public:
	BL();
	virtual ~BL();
};

#endif /* BL_H_ */
