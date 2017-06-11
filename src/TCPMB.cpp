/*
 * TCPMB.cpp
 *
 *  Created on: 04 θώνÿ 2017 γ.
 *      Author: Bogdan
 */
#include <memory>

#include "TCPMB.h"
#include "uip.h"
#include "timer.h"
#include "uip_arp.h"
#include "MBComunication.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

extern std::unique_ptr<MBComunication> mb_comunication;

struct uip_eth_addr local = {{UIP_ETHADDR0,
					  UIP_ETHADDR1,
					  UIP_ETHADDR2,
					  UIP_ETHADDR3,
					  UIP_ETHADDR4,
					  UIP_ETHADDR5}};

TCPMB::TCPMB() {
	// TODO Auto-generated constructor stub

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

void TCPMB_application_cb(void) {

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
	    	if ((evt.value.signals & TCPMB::kARP) > 0) {
				uip_arp_timer();
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

void TCPMB::TCPMB_periodic_timer_cb(void const *argument) {
	TCPMB* p = (TCPMB*)(argument);
	p->send_event(TCPMB::kPeriodix);
}
void TCPMB::TCPMB_arp_timer_cb(void const *argument) {
	TCPMB* p = (TCPMB*)(argument);
	p->send_event(TCPMB::kARP);
}

