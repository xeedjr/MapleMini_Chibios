/*
 * TCPMB.h
 *
 *  Created on: 04 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#ifndef TCPMB_H_
#define TCPMB_H_

#include <functional>
#include <memory>
#include "cmsis_os.h"
#include "MailBox.h"
#include "timer.h"
#include "uip.h"

class TCPMB {
	std::shared_ptr<uint8_t> packet_incom;
	uint16_t packet_incom_len = 0;
public:
	TCPMB();
	virtual ~TCPMB();

	void put_incoming_packet (std::shared_ptr<uint8_t> p, uint16_t len);

	uint16_t network_device_read (void);
	uint16_t network_device_send (void);
	static void TCPMB_Thread (void const *argument);
	void Thread (void);
	void send_event (int32_t signals) ;
	void send_event_incoming_packet ();
	static void TCPMB_periodic_timer_cb(void const *argument);
	static void TCPMB_arp_timer_cb(void const *argument);
	void application_cb(void);

	void connected(void);
	void closed(void);
	void acked(void);
	void newdata(void);
	void senddata(void);

	osThreadId thread_ID = {0};
	osThreadDef(TCPMB_Thread, osPriorityNormal, 512);
	osTimerId periodic_timer = {0};
	osTimerDef(periodic_handle, TCPMB_periodic_timer_cb);
	osTimerId arp_timer = {0};
	osTimerDef(arp_handle, TCPMB_arp_timer_cb);

	//struct timer periodic_timer, arp_timer;
	enum ThtrEv {
		kPeriodix = 	(1 << 0),
		kPeriodicARP =  (1 << 1),
		kIncominPacket = (1 << 2),
		kTCPDataSendOrPoll  = (1 << 3),
	};

	struct Result {
		enum {
			kConnectOk = 1,
			kClosed = 2,
			kSendOk = 3,
			kReceivedData = 4,
		} ev;
		struct uip_conn* conn;
		uint16_t len;
	};
	std::array<std::function<void(TCPMB::Result result)>,
				UIP_CONNS> result_cb_conn_id;

	struct uip_conn* connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port, std::function<void(Result result)> result_cb);
	int close(struct uip_conn* conn);
	int send_data(struct uip_conn*,
					uint8_t* buff,
					uint16_t len);
	int read_data(struct uip_conn* conn,
						uint8_t* buff,
						uint16_t* len);
};

#endif /* TCPMB_H_ */
