/*
 * EthSwitch.h
 *
 *  Created on: 28 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#ifndef ETHSWITCH_H_
#define ETHSWITCH_H_

#include "Pool.h"
#include "MailBox.h"

class EthSwitch {
	Pool<1, 1500> big_packets_pool;
	Pool<5, 256> small_packets_pool;
	uint8_t temp_packet[1500];

	enum ThtrEv {
		kIncominPacketNIF = (1 << 0),
		kOutPacketNIF = (1 << 1),
	};

	static void EthSwitch_Thread (void const *argument);
	void Thread (void);
	osThreadId thread_ID = {0};
	osThreadDef(EthSwitch_Thread, osPriorityNormal, 512);

	struct Packet {
		uint16_t len;
		std::shared_ptr<uint8_t> p;
	};
	MailBox<Packet, 5> out_packets_queue;
	uint8_t out_packets_queue_count = 0;

public:
	EthSwitch();
	virtual ~EthSwitch();

	void NetIfNewPacket(void);
	void NetIfSendPacket(uint8_t* buffer, uint16_t len);

};

#endif /* ETHSWITCH_H_ */
