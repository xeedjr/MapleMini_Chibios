/*
 * TCPMB.cpp
 *
 *  Created on: 04 θώνÿ 2017 γ.
 *      Author: Bogdan
 */
#include <memory>
#include <string>
#include <stdio.h>

#include "TCPMB.h"
#include "uip.h"
#include "timer.h"
#include "uip_arp.h"
#include "MBComunication.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

extern std::unique_ptr<MBComunication> mb_comunication;

static TCPMB* local_p = nullptr;

struct uip_eth_addr local = {{UIP_ETHADDR0,
					  UIP_ETHADDR1,
					  UIP_ETHADDR2,
					  UIP_ETHADDR3,
					  UIP_ETHADDR4,
					  UIP_ETHADDR5}};

TCPMB::TCPMB() {
	// TODO Auto-generated constructor stub

	local_p = this;

	uip_ipaddr_t ipaddr;

	/// create period read humidity sensor data
	periodic_timer = osTimerCreate(osTimer(periodic_handle), osTimerPeriodic, (void *)this);
	if (periodic_timer == nullptr) {
		/// Error
		chSysHalt("Error create periodic_timer");
	}
	arp_timer = osTimerCreate(osTimer(arp_handle), osTimerPeriodic, (void *)this);
	if (arp_timer == nullptr) {
		/// Error
		chSysHalt("Error create arp_timer");
	}

	uip_init();

	uip_setethaddr(local);

	uip_ipaddr(ipaddr, 192,168,56,115);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 192,168,56,1);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);

	uip_ipaddr(&ipaddr, 192,168,56,90);

	thread_ID = osThreadCreate(osThread(TCPMB_Thread), this);
	if (thread_ID == nullptr) {
		/// Error
		chSysHalt("Error create thread");
	}
	thread_ID->p_name = "TCPMB_Thread";

	osTimerStart (periodic_timer, 500);
	osTimerStart (arp_timer, 10000);
}

TCPMB::~TCPMB() {
	// TODO Auto-generated destructor stub
}

struct uip_conn* TCPMB::connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4,
								uint16_t port,
								std::function<void(Result )> result_cb) {
	uip_ipaddr_t ipaddr;
	//uint8_t ip1, ip2, ip3, ip4;
	struct uip_conn* conn;

	//sscanf(ip, "%u.%u.%u.%u", &ip1, &ip2, &ip3, &ip4);
	uip_ipaddr(ipaddr, ip1, ip2, ip3, ip4);

	conn = uip_connect(&ipaddr, HTONS(port));
	conn->appstate.conn_fl = 0;
	for(int i = 0; i < UIP_CONNS; i++) {
		if (&uip_conns[i] == conn) {
			conn->appstate.conn_id = i;
			result_cb_conn_id[i] = result_cb;
		}
	}

	return conn;
}

int TCPMB::close(struct uip_conn* conn) {
	conn->appstate.conn_fl = 3;
	send_event(TCPMB::kTCPDataSendOrPoll);
	return 1;
}

int TCPMB::send_data(struct uip_conn* conn,
				uint8_t* buff,
				uint16_t len) {
	if (conn->appstate.send_len > 0) {
		//// we cant perform send
		return -1;
	}

	if (len > UIP_BUFSIZE) {
		//// we cant perform send
		return -1;
	}

	conn->appstate.send_buff = buff;
	conn->appstate.send_len = len;

	send_event(TCPMB::kTCPDataSendOrPoll);

	return len;
}

int TCPMB::read_data(struct uip_conn* conn,
					uint8_t* buff,
					uint16_t* len) {
	if (conn->appstate.recv_len != nullptr) {
		//// we cant perform send
		return 0;
	}

	conn->appstate.recv_buff = buff;
	conn->appstate.recv_len = len;

	if (uip_stopped(conn)) {
		uip_restart();
	}

	return 1;
}

void TCPMB::connected(void) {
	TCPMB::Result res;
	res.ev = TCPMB::Result::kConnectOk;
	res.conn = uip_conn;

	result_cb_conn_id[uip_conn->appstate.conn_id](res);
}

void TCPMB::closed(void) {
	TCPMB::Result res;
	res.ev = TCPMB::Result::kClosed;
	res.conn = uip_conn;

	result_cb_conn_id[uip_conn->appstate.conn_id](res);
}

void TCPMB::acked(void) {
	TCPMB::Result res;

	uip_conn->appstate.send_len = 0;

	res.ev = TCPMB::Result::kSendOk;
	res.conn = uip_conn;

	result_cb_conn_id[uip_conn->appstate.conn_id](res);
}

void TCPMB::newdata(void) {
	TCPMB::Result res;
	res.ev = TCPMB::Result::kReceivedData;
	res.conn = uip_conn;

	if (uip_conn->appstate.recv_buff == nullptr ||
		uip_conn->appstate.recv_len == nullptr) {
		/// pointers null
		uip_stop();
		return ;
	}

	if (*(uip_conn->appstate.recv_len) > 0) {
		/// buffer full cant fill
		uip_stop();
		return ;
	}

	if (uip_datalen() > 0) {
		memcpy(uip_conn->appstate.recv_buff,
				uip_appdata,
				uip_datalen());
		*(uip_conn->appstate.recv_len) = uip_datalen();
		uip_conn->appstate.recv_buff = nullptr;
		uip_conn->appstate.recv_len = nullptr;

		result_cb_conn_id[uip_conn->appstate.conn_id](res);
	};
}

void TCPMB::senddata(void) {
	static char *bufptr, *lineptr;
	static int buflen, linelen;
	uint16_t dev_id = 1;

	bufptr = (char*)uip_appdata;
	buflen = 0;

	if(buflen + uip_conn->appstate.send_len < uip_mss()) {
		if (uip_conn->appstate.send_len > 0){
			memcpy(bufptr,
					uip_conn->appstate.send_buff,
					uip_conn->appstate.send_len);
			bufptr += uip_conn->appstate.send_len;
			buflen += uip_conn->appstate.send_len;
		};
	} else {

	}

	uip_send(uip_appdata, buflen);
}

void TCPMB::application_cb(void) {
	if(uip_connected()) {
		switch(uip_conn->appstate.conn_fl) {
		case 0:
			uip_conn->appstate.conn_fl = 1;
			connected();
			break;
		case 3:
			uip_close();
			break;
		}
	}

	if(uip_closed() ||
		uip_aborted() ||
		uip_timedout()) {
		closed();
	}

	if(uip_acked()) {
		acked();
	}

	if(uip_newdata()) {
		newdata();
	}

	if(uip_rexmit() ||
		uip_newdata() ||
		uip_acked() ||
		uip_connected() ||
		uip_poll()) {
		senddata();
	}
}

u16_t TCPMB::network_device_read (void) {
	return mb_comunication->get_in_packet(uip_buf);
}

u16_t TCPMB::network_device_send (void) {
	mb_comunication->put_out_packet(uip_buf,
									uip_len);
	uip_len = 0;
}

void TCPMB::Thread (void) {
	osEvent evt;

	while(1) {
	    // wait for a signal
	    evt = osSignalWait (0, osWaitForever);
	    if (evt.status == osEventSignal)  {
	        // handle event status
	    	if ((evt.value.signals & TCPMB::kIncominPacket) > 0) {
				uip_len = network_device_read();
				if(uip_len > 0) {
					if(BUF->type == htons(UIP_ETHTYPE_IP)) {
						uip_arp_ipin();
						uip_input();
						/* If the above function invocation resulted in data that
						should be sent out on the network, the global variable
						uip_len is set to a value > 0. */
						if(uip_len > 0) {
							uip_arp_out();
							network_device_send();
						}
					} else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
						uip_arp_arpin();
						/* If the above function invocation resulted in data that
						should be sent out on the network, the global variable
						uip_len is set to a value > 0. */
						if(uip_len > 0) {
							network_device_send();
						}
					}
				}
			}
	    	if ((evt.value.signals & TCPMB::kPeriodix) > 0) {
				int i;
				for(i = 0; i < UIP_CONNS; i++) {
					uip_periodic(i);
					/* If the above function invocation resulted in data that
					should be sent out on the network, the global variable
					uip_len is set to a value > 0. */
					if(uip_len > 0) {
						uip_arp_out();
						network_device_send();
					}
				}

				#if UIP_UDP
				for(i = 0; i < UIP_UDP_CONNS; i++) {
					uip_udp_periodic(i);
					/* If the above function invocation resulted in data that
					should be sent out on the network, the global variable
					uip_len is set to a value > 0. */
					if(uip_len > 0) {
						uip_arp_out();
						network_device_send();
					}
				}
				#endif /* UIP_UDP */

			}
	    	if ((evt.value.signals & TCPMB::kPeriodicARP) > 0) {
				uip_arp_timer();
			}
	    	if ((evt.value.signals & TCPMB::kTCPDataSendOrPoll) > 0) {
				int i;
				for(i = 0; i < UIP_CONNS; i++) {
					uip_conn = &uip_conns[i];
					uip_poll_conn(uip_conn);
					/* If the above function invocation resulted in data that
					should be sent out on the network, the global variable
					uip_len is set to a value > 0. */
					if(uip_len > 0) {
						uip_arp_out();
						network_device_send();
					}
				}
			}
	    }
	}
}

void TCPMB::TCPMB_Thread (void const *argument) {
	TCPMB* p = (TCPMB*)(argument);

	p->Thread();
}

void TCPMB::send_event (int32_t signals) {
	osSignalSet(thread_ID, signals);
}

void TCPMB::send_event_incoming_packet () {
	osSignalSet(thread_ID,
				TCPMB::kIncominPacket);
}

void TCPMB_application_cb(void) {
	local_p->application_cb();
}

void TCPMB::TCPMB_periodic_timer_cb(void const *argument) {
	TCPMB* p = (TCPMB*)(argument);
	p->send_event(TCPMB::kPeriodix);
}
void TCPMB::TCPMB_arp_timer_cb(void const *argument) {
	TCPMB* p = (TCPMB*)(argument);
	p->send_event(TCPMB::kPeriodicARP);
}

