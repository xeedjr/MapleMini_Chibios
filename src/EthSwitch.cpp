/*
 * EthSwitch.cpp
 *
 *  Created on: 28 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#include <string.h>
#include "EthSwitch.h"
#include "TCPMB.h"
#include "enc28j60.h"

extern ENC28J60Driver enc28j60;
extern std::unique_ptr<TCPMB> tvp_mb;

EthSwitch::EthSwitch() {
	// TODO Auto-generated constructor stub
	thread_ID = osThreadCreate(osThread(EthSwitch_Thread), this);
	if (thread_ID == nullptr) {
		/// Error
		chSysHalt("Error create thread");
	}
	thread_ID->p_name = "TCPMB_Thread";
}

EthSwitch::~EthSwitch() {
	// TODO Auto-generated destructor stub
}

void EthSwitch::Thread (void) {
	osEvent evt;

	while(1) {
	    // wait for a signal
	    evt = osSignalWait (0, osWaitForever);
	    if (evt.status == osEventSignal)  {
	        // handle event status
	    	if ((evt.value.signals & kIncominPacketNIF) > 0) {
	    		uint16_t len =  enc28j60PacketReceive(&enc28j60, temp_packet, sizeof(temp_packet));

	    		if (enc28j60ReadPktCnt(&enc28j60) > 0) {
	    			/// is more than 1 packet
	    			NetIfNewPacket();
	    		}

	    		if (len > 0) {
		    		/// process
	    			std::shared_ptr<uint8_t> p;
	    			if (len < 256) {
	    				p = small_packets_pool.get();
	    				if (p == nullptr) {
	    					return;
	    				}
	    				memcpy(p.get(), temp_packet, len);
	    			} else {
	    				p = big_packets_pool.get();
	    				if (p == nullptr) {
	    					return;
	    				}
	    				memcpy(p.get(), temp_packet, len);
	    			}

	    			tvp_mb->put_incoming_packet(p, len);
	    		}
			}
	    	if ((evt.value.signals & kOutPacketNIF) > 0) {
	    		if (out_packets_queue_count > 0) {
	    			Packet packet;
	    			out_packets_queue.pop(packet);
					out_packets_queue_count--;

					enc28j60PacketSend(&enc28j60, packet.p.get(), packet.len);
	    		};
			}
	    }
	}
}

void EthSwitch::EthSwitch_Thread (void const *argument) {
	EthSwitch* p = (EthSwitch*)(argument);

	p->Thread();
}

void EthSwitch::NetIfNewPacket(void) {
	osSignalSet(thread_ID,
					kIncominPacketNIF);
}

void EthSwitch::NetIfSendPacket(uint8_t* buffer, uint16_t len) {
	std::shared_ptr<uint8_t> p;
	if (len > 0) {
		if (len < 256) {
			p = small_packets_pool.get();
			if (p == nullptr) {
				return;
			}
			memcpy(p.get(), buffer, len);
		} else {
			p = big_packets_pool.get();
			if (p == nullptr) {
				return;
			}
			memcpy(p.get(), buffer, len);
		}
		Packet packet = {len, p};
		out_packets_queue.push(packet);
		out_packets_queue_count++;

		osSignalSet(thread_ID,
						kOutPacketNIF);
	};
}
