/*
 * MBComunication.cpp
 *
 *  Created on: 14 окт. 2016 г.
 *      Author: Bogdan
 */
#include <string.h>
#include <memory>
#include "MBComunication.h"
#include "mbframe.h"
#include "BLExtractor.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "MBMessage.pb.h"

extern std::unique_ptr<BLExtractor> bl;

MBComunication::MBComunication() {
}

MBComunication::~MBComunication() {
	// TODO Auto-generated destructor stub
}

void MBComunication::send_notification_temp_humidity(float hum,
														float temp,
														uint32_t speed) {
	MBMessage msg = MBMessage_init_zero;
	msg.interface.notifications.notif.change_parameters.fan_speed = 99;
	msg.interface.notifications.notif.change_parameters.humidity = 50.1;
	msg.interface.notifications.notif.change_parameters.temperature = 33.5;
	msg.which_interface = MBMessage_notifications_tag;
	msg.interface.notifications.which_notif = Notifications_change_parameters_tag;

	put_message_for_send(msg);
}

void MBComunication::put_message_for_send(MBMessage& message) {
    /* This is the buffer where we will store our message. */
    bool status = false;

    /* Encode our message */

	/* Create a stream that will write to our buffer. */
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

	/* Now we are ready to encode the message! */
	status = pb_encode(&stream, MBMessage_fields, &message);

	/* Then just check for any errors.. */
	if (!status)
	{
		return ;
	}

	if (out_message_len_ == 0) {
		memcpy(out_messages_, buffer, stream.bytes_written);
		out_message_len_ = stream.bytes_written;
	}
}

eMBException
MBComunication::eMBFuncPacket( UCHAR * pucFrame, USHORT * usLen ) {
    //memcpy( &pucFrame[MB_PDU_DATA_OFF], &ucMBSlaveID[0], ( size_t )usMBSlaveIDLen );
    //*usLen = ( USHORT )( MB_PDU_DATA_OFF + usMBSlaveIDLen );

	/// copy input message
	if (*usLen > MB_PDU_DATA_OFF) {
		BLExtractor::Events ev;
		ev.ev_type = BLExtractor::Events::kProtoMsg;
		memcpy(ev.events.proto_msg.message,
				&pucFrame[MB_PDU_DATA_OFF],
				*usLen - MB_PDU_DATA_OFF);
		ev.events.proto_msg.len = (uint8_t)(*usLen - MB_PDU_DATA_OFF);
		bl->put_event(ev);
	};

	/// fill out packet if exist
	if (out_message_len_ != 0) {
		memcpy(&pucFrame[MB_PDU_DATA_OFF], out_messages_, out_message_len_);
		*usLen = ( USHORT )( MB_PDU_DATA_OFF + out_message_len_ );
		out_message_len_ = 0;
	};

    return MB_EX_NONE;
}
