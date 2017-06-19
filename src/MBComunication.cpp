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
#include "pb_decode.h"
#include "pb_encode.h"
#include "MBMessage.pb.h"
#include "TCPMB.h"

extern std::unique_ptr<BLExtractor> bl;
extern std::unique_ptr<TCPMB> tvp_mb;

MBComunication::MBComunication() {
}

MBComunication::~MBComunication() {
	// TODO Auto-generated destructor stub
}

void MBComunication::put_out_packet(uint8_t* buff, uint16_t len) {
	if (out_packet__len_ == 0) {
		memcpy(out_packet_,
				buff,
				len);
		out_packet__len_ = len;
	};
}

uint8_t MBComunication::get_in_packet(uint8_t* buff) {

	uint8_t temp = 0;
	memcpy(buff,
			in_packet_,
			in_packet__len_);
	temp = in_packet__len_;
	in_packet__len_ = 0;

	return temp;
}
eMBException
MBComunication::eMBFuncPacket( UCHAR * pucFrame, USHORT * usLen ) {
	/// copy input message
	if (*usLen > MB_PDU_DATA_OFF) {
		if (in_packet__len_ == 0) {
			/// buffer free
			in_packet__len_ = *usLen - MB_PDU_DATA_OFF;
			memcpy(in_packet_,
					&pucFrame[MB_PDU_DATA_OFF],
					in_packet__len_);
			tvp_mb->send_event_incoming_packet();
		};
	};

	/// fill out packet if exist
	if (out_packet__len_ != 0) {
		memcpy(&pucFrame[MB_PDU_DATA_OFF],
				out_packet_,
				out_packet__len_);
		*usLen = ( USHORT )( MB_PDU_DATA_OFF + out_packet__len_ );
		out_packet__len_ = 0;
	} else {
		/// send 0 bytes
		*usLen = ( USHORT )( MB_PDU_DATA_OFF + out_packet__len_ );
	};

    return MB_EX_NONE;
}
