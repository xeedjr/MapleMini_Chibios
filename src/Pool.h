/*
 * Pool.h
 *
 *  Created on: 28 θώνÿ 2017 γ.
 *      Author: Bogdan
 */

#ifndef POOL_H_
#define POOL_H_

#include <array>
#include <memory>

#include "cmsis_os.h"

template <int N, int ELEMENT_SIZE>
class Pool {
private:

	osMessageQId		mesasage_ID;
	const msg_t message_buff[N] = {0};                        \
	mailbox_t mailbox_ID;                                    \
	const osMessageQDef_t message_def = {                            \
	  (N),                                                               \
	  sizeof (unsigned char),                                                             \
	  (mailbox_t *)&mailbox_ID,                                          \
	  (void *)&message_buff[0]                                        \
	};

	uint8_t pool[N][ELEMENT_SIZE];

	void _free_buff(uint8_t* pb_pack) {

		osMessagePut(mesasage_ID, (uint32_t)pb_pack, osWaitForever);

	    return;
	}
public:
	Pool(){
		mesasage_ID = osMessageCreate(&message_def, NULL);
	    if ( 0 == mesasage_ID )
	    {
	    	/// error

	    }
		for (int i = 0; i < N; i++) {
			//free_mail_box_.post(i, TIME_INFINITE);
			osMessagePut(mesasage_ID, (uint32_t)&pool[i][0], osWaitForever);
		}
	}
	~Pool(){}

	std::shared_ptr<uint8_t>
	get(){
		uint8_t *addr_p;
		std::shared_ptr<uint8_t> addr;

		osEvent  peEvent_;
		peEvent_ = osMessageGet(mesasage_ID, TIME_IMMEDIATE);
	    if( osEventMessage != peEvent_.status)
	    {
	    	/// error
	    	addr.reset(addr_p);
	    } else {
	    	/// good
	    	addr_p = (uint8_t*)peEvent_.value.v;
	    	addr.reset(addr_p,
						std::bind(&Pool<N, ELEMENT_SIZE>::_free_buff,
									this,
									std::placeholders::_1));
	    }

	    return addr;
	}
};

#endif /* POOL_H_ */
