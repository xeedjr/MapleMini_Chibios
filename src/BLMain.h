/*
 * BLMain.h
 *
 *  Created on: 07 џэт. 2017 у.
 *      Author: Bogdan
 */

#ifndef BLMAIN_H_
#define BLMAIN_H_

#include <functional>
#include "MailBox.h"

template <typename event_enum_T, typename event_message_T>
class BLMain {
public:
	struct StateTable {
		event_enum_T ev_type;
		std::function<void(event_message_T*)>  fun;
	};
	struct EventPair {
		event_enum_T ev_type;
		event_message_T ev_data;
	};
private:
	MailBox<EventPair, 3> events_queue_;
	StateTable* state_table = nullptr;
	uint32_t state_table_len = 0;
public:
	BLMain() {};
	~BLMain() {};

	class SetState {
		StateTable* table_ = nullptr;
		uint16_t len_ = 0;
		BLMain* p_ = nullptr;
		SetState(BLMain* p,
				StateTable* table,
				uint16_t len) {
			p_ = p;
			table_ = GetCurrentState(len_);
			p_->SetNewState(table, len);
		}
		~SetState() {
			p_->SetNewState(table_, len_);
		}
	};

	void SetNewState (StateTable* table, uint16_t len) {
		state_table = table;
		state_table_len = len;
	}
	StateTable* GetCurrentState (uint16_t& len) {
		len = state_table_len;
		return state_table;
	}

	void PutEvent (EventPair ev) {
		events_queue_.push(ev);
	}
	void PutEventI (EventPair ev) {
		events_queue_.pushI(ev);
	}

	void GeneralWait (void) {
		EventPair ev;
		events_queue_.pop(ev);

		if (state_table != nullptr) {
			for (int i = 0; i < state_table_len; i++) {
				if (state_table[i].ev_type == ev.ev_type) {
					state_table[i].fun(&ev.ev_data);
				}
			}
		};
	}

	void Iterate (void) {
		GeneralWait();
	}
};

#endif /* BLMAIN_H_ */
