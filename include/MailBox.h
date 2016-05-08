/*
 * MailBox.h
 *
 *  Created on: 20 марта 2016 г.
 *      Author: Bogdan
 */

#ifndef MAILBOX_H_
#define MAILBOX_H_

#include <string.h>

#include "ch.hpp"

template <typename T, int N>
class MailBox
{
private:
 chibios_rt::Mailbox<msg_t, N> mail_box_;
 chibios_rt::Mailbox<msg_t, N> free_mail_box_;
 std::array<T, N> buffers;

 public:
	MailBox() {
		for (int i = 0; i < N; i++) {
			free_mail_box_.post(i, TIME_INFINITE);
		}
	}

	void pop(T& item)
	{
		msg_t index;
		mail_box_.fetch(&index, TIME_INFINITE);
		item = buffers.at(index);
		free_mail_box_.post(index, TIME_INFINITE);
	}

	void push(T& item)
	{
		msg_t index;
		free_mail_box_.fetch(&index, TIME_INFINITE);
		buffers.at(index) = item;
		mail_box_.post(index, TIME_INFINITE);
	}

	void pushI(T& item)
	{
		msg_t index;
		free_mail_box_.fetchI(&index);
		buffers.at(index) = item;
		mail_box_.postI(index);
	}
};

#endif /* MAILBOX_H_ */
