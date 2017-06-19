/*
 * TCPClient.h
 *
 *  Created on: 13 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include "uip.h"

#include "PBFunct.h"
#include "BLMain.h"
#include "TCPMB.h"
#include "MailBox.h"
#include "PBFunct.h"

class TCPClient {
public:
	TCPClient();
	virtual ~TCPClient();

	uint8_t uuid[16] = {0};

	uint8_t send_tcp_buff[100];
	uint16_t send_tcp_buff_len = 0;
	uint8_t recv_tcp_buff[100];
	uint16_t recv_tcp_buff_len = 0;

	struct uip_conn* conn = nullptr;
	void TCPMB_conn_result(TCPMB::Result result);
	int Send_proto_packet(PBFunct::PBPacketShrdPtr packet);

	uint8_t size;
	MailBox<PBFunct::PBPacketShrdPtr, 3> send_proto_packets_queue_;
	enum StateSend {
		kSendID,
		kSendData,
		kSendReady,
	} state_send = kSendID;
	PBFunct::PBPacketShrdPtr curent_send_packet;

	struct Ev {
		enum ThtrEv {
			kInitConnection,
			kConnClose 			,
			kConnDataIs		,
			kIncominPacket		,
			kTCPDataSend		,
			kConnOk 	,
			kSendOk 	,
			kSendDeviceId,
			kSendProtoPacket,
			kSendProtoPacketFromQueue,
		} ev;
		union {
			uint8_t u1;
			void* p;
		} data ;
		PBFunct::PBPacketShrdPtr packet;
	};
	MailBox<Ev, 3> events_queue_;
	static void TCPClient_Thread (void const *argument);
	void Thread (void);
	osThreadId thread_ID = {0};
	osThreadDef(TCPClient_Thread, osPriorityNormal, 512);

	uint8_t in_buffer[256];
	uint16_t received_size = 0;
private:

};

#endif /* TCPCLIENT_H_ */
