/*
 * ESP8266Parser.cpp
 *
 *  Created on: 12 но€б. 2016 г.
 *      Author: Bogdan
 */

#include <ESP8266Parser.h>

#include "cmsis_os.h"
#include "ch.h"
#include "hal.h"

static ESP8266Parser* p = nullptr;

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

  (void)uartp;

  chSysLockFromISR();

  chSysUnlockFromISR();
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e) {

  (void)uartp;
  (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c) {

  (void)uartp;
  (void)c;

/*  chSysLockFromISR();
  rx_char = c;
  pxMBFrameCBByteReceived(  );
  chSysUnlockFromISR();
*/}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(UARTDriver *uartp) {

  (void)uartp;
  chSysLockFromISR();
  p->process();
  chSysUnlockFromISR();
}

/*
 * UART driver configuration structure.
 */
static UARTConfig uart_cfg_1 = {
  txend1,
  txend2,
  rxend,
  rxchar,
  rxerr,
  115200,
  0,
  0,
  0
};


ESP8266Parser::ESP8266Parser() {
	// TODO Auto-generated constructor stub
	p = this;
	uartStart(&UARTD2, &uart_cfg_1);
	uartStartReceive(&UARTD2, 1, &recv_c);
}

ESP8266Parser::~ESP8266Parser() {
	// TODO Auto-generated destructor stub
}

void ESP8266Parser::process() {

/*	if (ESP8266Parser::recv_c == '\n') {
		if (last_c == '\r') {
			/// enter condition last command ends new starts
			index = 0;
		}
	} else {
		/// need analyse maybe command
		switch(recv_c) {
		case
		}
	}
*/
	buffer[index++] =

	last_c = recv_c;
	uartStartReceive(&UARTD2, 1, &recv_c);
}
