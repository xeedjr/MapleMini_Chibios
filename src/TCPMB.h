/*
 * TCPMB.h
 *
 *  Created on: 04 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#ifndef TCPMB_H_
#define TCPMB_H_

#include <functional>
#include "cmsis_os.h"
#include "MailBox.h"
#include "timer.h"


class TCPMB {

public:
	TCPMB();
	virtual ~TCPMB();

	uint16_t network_device_read (void);
	uint16_t network_device_send (void);
	static void TCPMB_Thread (void const *argument);
	void Thread (void);
	void send_event (int32_t signals) ;
	void send_event_incoming_packet ();
	static void TCPMB_periodic_timer_cb(void const *argument);
	static void TCPMB_arp_timer_cb(void const *argument);

	osThreadId thread_ID = {0};
	osThreadDef(TCPMB_Thread, osPriorityNormal, 512);
	osTimerId periodic_timer = {0};
	osTimerDef(periodic_handle, TCPMB_periodic_timer_cb);
	osTimerId arp_timer = {0};
	osTimerDef(arp_handle, TCPMB_arp_timer_cb);

	//struct timer periodic_timer, arp_timer;
	enum ThtrEv {
		kPeriodix = 	(1 << 0),
		kARP = 			(1 << 1),
		kIncominPacket = (1 << 2),
	};
};

#endif /* TCPMB_H_ */
