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

class MBComunication {
public:

private:
	uint8_t out_messages_[256] = {0};
	uint8_t out_message_len_ = 0;
    uint8_t buffer[256] = {0};

public:
	MBComunication();
	virtual ~MBComunication();

	eMBException
	eMBFuncPacket( UCHAR * pucFrame, USHORT * usLen );

	void send_notification_temp_humidity(float hum,
										float temp,
										uint32_t speed);
	void put_message_for_send(MBMessage& message);
};

#endif /* MBCOMUNICATION_H_ */
