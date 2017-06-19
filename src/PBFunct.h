/*
 * PBFunct.h
 *
 *  Created on: 12 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#ifndef PBFUNCT_H_
#define PBFUNCT_H_

#include "PBFunct.h"
#include <stdint.h>
#include <memory>
#include "cmsis_os.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "MBMessage.pb.h"

#define PBPACKET_SIZE	100

class PBFunct {
public:
	virtual ~PBFunct();
	PBFunct();

	typedef struct {
		union {
			struct {
				uint32_t len;
				uint8_t  data[];
			} s;
			uint8_t buff[PBPACKET_SIZE];
		} packet_u;
	} PBPacket;
	typedef std::shared_ptr<PBPacket> PBPacketShrdPtr;

	PBPacketShrdPtr send_notification_temp_humidity(float hum,
										float temp,
										uint32_t speed);
	PBPacketShrdPtr send_resp_OK(uint8_t cmd_tag);
	int decode_message(MBMessage& message,
								PBFunct::PBPacketShrdPtr packet);
	PBPacketShrdPtr
	put_message_for_send(MBMessage& message);



#define NUM_PACKETS  5
	//osPoolDef (PBPacketPool, 3, PBPacket);
	PBPacket  os_pool_buf_PBPacketPool[NUM_PACKETS];                                   \
	memory_pool_t os_pool_obj_PBPacketPool;                                    \
	osPoolDef_t os_pool_def_PBPacketPool = {                                    \
	  (NUM_PACKETS),                                                                     \
	  sizeof (PBPacket),                                                            \
	  (memory_pool_t *)&os_pool_obj_PBPacketPool,                                              \
	  (void *)&os_pool_buf_PBPacketPool[0]                                            \
	};
	osPoolId PBPacketPool_Id;
	PBPacketShrdPtr get_buff();
	void _free_buff(PBPacket*);
};

#endif /* PBFUNCT_H_ */
