/*
 * TCPClient.cpp
 *
 *  Created on: 13 θώνÿ 2017 γ.
 *      Author: Bogdan
 */
#include <memory>

#include "TCPClient.h"
#include "TCPMB.h"
#include "BLExtractor.h"

extern std::unique_ptr<TCPMB> tvp_mb;
extern std::unique_ptr<PBFunct> pb_funct;
extern std::unique_ptr<BLExtractor> bl;
extern void get_96bit_uuid_array(uint8_t* buff);

TCPClient::TCPClient() {

	get_96bit_uuid_array(uuid);

	thread_ID = osThreadCreate(osThread(TCPClient_Thread), this);
	if (thread_ID == nullptr) {
		/// Error
		chSysHalt("Error create thread");
	}
	thread_ID->p_name = "TCPClient Thread";

	Ev ev;
	ev.ev = Ev::kInitConnection;
	events_queue_.push(ev);
}

TCPClient::~TCPClient() {

}

void TCPClient::TCPMB_conn_result(TCPMB::Result result) {
	Ev ev;

	switch(result.ev) {
	case TCPMB::Result::kClosed:
		ev.ev = Ev::kConnClose;
		break;
	case TCPMB::Result::kConnectOk:
		ev.ev = Ev::kConnOk;
		break;
	case TCPMB::Result::kReceivedData:
		ev.ev = Ev::kIncominPacket;
		break;
	case TCPMB::Result::kSendOk:
		ev.ev = Ev::kSendOk;
		break;
	}

	ev.data.p = result.conn;
	events_queue_.push(ev);
}

int TCPClient::Send_proto_packet(PBFunct::PBPacketShrdPtr packet) {
	Ev ev;

	ev.ev = Ev::kSendProtoPacket;
	ev.packet = packet;

	if (events_queue_.push(ev) < 0) {
		/// error
		return -1;
	};
	return 0;
}

void TCPClient::Thread (void) {
	Ev ev;

	while(1) {
	    // wait for a signal
		events_queue_.pop(ev);

		switch(ev.ev) {
		case Ev::kInitConnection:
			{
				if (conn == nullptr) {
					conn = tvp_mb->connect(192, 168, 56, 90,
									1112,
									std::bind(&TCPClient::TCPMB_conn_result,
												this,
												std::placeholders::_1));

					tvp_mb->read_data(conn, in_buffer, &received_size);

					state_send = kSendID;
				};
			}
			break;
		case Ev::kConnClose:
			{
				tvp_mb->close(conn);
				conn = nullptr;
				if (curent_send_packet != nullptr) {
					curent_send_packet = nullptr;
				};
				state_send = kSendID;
				ev.ev = Ev::kInitConnection;
				events_queue_.push(ev);
			}
			break;
		case Ev::kConnOk:
			{
				ev.ev = Ev::kSendDeviceId;
				events_queue_.push(ev);
			}
			break;
		case Ev::kSendDeviceId :
			{
				state_send = kSendID;
				tvp_mb->send_data(conn, uuid, sizeof(uuid));
			}
			break;
		case Ev::kSendProtoPacketFromQueue :
			{
				if (size > 0){
					/// send
					if (state_send == kSendReady) {
						PBFunct::PBPacketShrdPtr q;
						state_send = kSendData;
						send_proto_packets_queue_.pop(q);
						size--;
						curent_send_packet = q;
						if(tvp_mb->send_data(conn, q->packet_u.buff, q->packet_u.s.len + 4) < 0) {
							/// error send
							chSysHalt("kSendProtoPacketFromQueue send error");
						};
					}
				};
			}
			break;
		case Ev::kIncominPacket :
			{
				PBFunct::PBPacketShrdPtr p;

				/// process data in in_buffer
				if ((p = pb_funct->get_buff()) != nullptr) {
					memcpy(p->packet_u.buff,
							in_buffer,
							received_size);
				}

				if ((p->packet_u.s.len + 4) != (received_size)) {
					/// fragment wroong
					ev.ev = Ev::kConnClose;
					events_queue_.push(ev);
					break;
				}

				received_size = 0;
				tvp_mb->read_data(conn, in_buffer, &received_size);

				/// looks good
				BLExtractor::Events ev_pair;
				ev_pair.ev_type = BLExtractor::Events::kProtoMsg;
				ev_pair.events.packet = p;
				bl->put_event(ev_pair);
			}
			break;
		case Ev::kSendOk :
			{
				switch(state_send) {
				case kSendID:
					state_send = kSendReady;
					ev.ev = Ev::kSendProtoPacketFromQueue;
					events_queue_.push(ev);
					break;
				case kSendData:
					curent_send_packet = nullptr;
					state_send = kSendReady;
					ev.ev = Ev::kSendProtoPacketFromQueue;
					events_queue_.push(ev);
					break;
				}
			}
			break;
		case Ev::kSendProtoPacket :
			{
				if (state_send != kSendID) {
					if (size < 3) {
						PBFunct::PBPacketShrdPtr q;
						q = ev.packet;
						send_proto_packets_queue_.push(q);
						size++;

						ev.ev = Ev::kSendProtoPacketFromQueue;
						events_queue_.push(ev);
					} else {

					};
				} else {

				}
			}
			break;


		}
	}
}

void TCPClient::TCPClient_Thread (void const *argument) {
	TCPClient* p = (TCPClient*)(argument);

	p->Thread();
}

