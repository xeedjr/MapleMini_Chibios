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

	void put_out_packet(uint8_t* buff, uint16_t len);
	uint8_t get_in_packet(uint8_t* buff);
};

#endif /* MBCOMUNICATION_H_ */
