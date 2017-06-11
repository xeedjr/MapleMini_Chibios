/*
 * MBComunication.h
 *
 *  Created on: 14 окт. 2016 г.
 *      Author: Bogdan
 */

#ifndef MBCOMUNICATION_H_
#define MBCOMUNICATION_H_

#include "cmsis_os.h"
#include "mb.h"
#include "MailBox.h"
#include "MBMessage.pb.h"
#include "BLExtractor.h"

class MBComunication {
public:

private:
	uint8_t in_packet_[256] = {0};
	uint8_t in_packet__len_ = 0;
	uint8_t out_packet_[256] = {0};
	uint8_t out_packet__len_ = 0;

    BLExtractor::Events ev;
public:
	MBComunication();
	virtual ~MBComunication();

	eMBException
	eMBFuncPacket( UCHAR * pucFrame, USHORT * usLen );

	void send_notification_temp_humidity(float hum,
										float temp,
										uint32_t speed);
	void send_resp_OK(uint8_t cmd_tag);
	int decode_message(MBMessage& message,
						pb_byte_t* buffer,
						uint8_t len);
	void put_message_for_send(MBMessage& message);
	void put_out_packet(uint8_t* buff, uint16_t len);
	uint8_t get_in_packet(uint8_t* buff);
};

#endif /* MBCOMUNICATION_H_ */
