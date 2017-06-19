/*
 * PBFunct.cpp
 *
 *  Created on: 12 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#include "PBFunct.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "MBMessage.pb.h"

PBFunct::~PBFunct() {
	// TODO Auto-generated destructor stub
}

PBFunct::PBFunct() {
	// TODO Auto-generated constructor stub
	PBPacketPool_Id = osPoolCreate (osPool (PBPacketPool));
	if (PBPacketPool_Id == NULL)  {
		chSysHalt("Error create PBPacketPool_Id");
	}
}

PBFunct::PBPacketShrdPtr
PBFunct::get_buff() {
	PBPacket *addr_p;
	PBPacketShrdPtr addr;

	if ((addr_p = (PBPacket *)osPoolAlloc (PBPacketPool_Id)) != nullptr) {
		/// good
	    addr.reset(addr_p,
	    			std::bind(&PBFunct::_free_buff,
								this,
								std::placeholders::_1));
	} else {
		addr.reset(addr_p);
	}

    return addr;
}

void PBFunct::_free_buff(PBPacket* pb_pack) {
	osStatus   status;

    status = osPoolFree (PBPacketPool_Id, pb_pack);
    if (status==osOK)  {
      // handle status code
    	return ;
    }
    return;
}

PBFunct::PBPacketShrdPtr
PBFunct::send_notification_temp_humidity(float hum,
												float temp,
												uint32_t speed) {
	MBMessage msg = MBMessage_init_zero;
	msg.interface.notifications.notif.change_parameters.fan_speed = speed;
	msg.interface.notifications.notif.change_parameters.humidity = hum;
	msg.interface.notifications.notif.change_parameters.temperature = temp;
	msg.which_interface = MBMessage_notifications_tag;
	msg.interface.notifications.which_notif = Notifications_change_parameters_tag;

	return put_message_for_send(msg);
}

PBFunct::PBPacketShrdPtr
PBFunct::send_resp_OK(uint8_t cmd_tag) {
	MBMessage msg = MBMessage_init_zero;
	msg.which_interface = MBMessage_responses_tag;
	msg.interface.responses.which_resp = Responses_exec_result_tag;
	msg.interface.responses.on_command = cmd_tag;
	msg.interface.responses.resp.exec_result.result = Result::Result_OK;

	return put_message_for_send(msg);
}

int PBFunct::decode_message(MBMessage& message,
							PBFunct::PBPacketShrdPtr packet) {
    /* This is the buffer where we will store our message. */
    bool status = false;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(packet->packet_u.s.data,
    											packet->packet_u.s.len);

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, MBMessage_fields, &message);

    /* Check for errors... */
    if (!status)
    {
        return -1;
    }
    return 1;
}

PBFunct::PBPacketShrdPtr
PBFunct::put_message_for_send(MBMessage& message) {
	auto p = get_buff();
	if (p != nullptr) {
		/* This is the buffer where we will store our message. */
		bool status = false;

		/* Encode our message */

		/* Create a stream that will write to our buffer. */
		pb_ostream_t stream = pb_ostream_from_buffer(p->packet_u.s.data,
													PBPACKET_SIZE - 4);

		/* Now we are ready to encode the message! */
		status = pb_encode(&stream,
							MBMessage_fields,
							&message);

		/* Then just check for any errors.. */
		if (!status)
		{
			return nullptr;
		}

		p->packet_u.s.len = stream.bytes_written;
	};

	return p;
}

