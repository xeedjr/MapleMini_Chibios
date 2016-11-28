/*
 * ESP8266Parser.h
 *
 *  Created on: 12 но€б. 2016 г.
 *      Author: Bogdan
 */

#ifndef ESP8266PARSER_H_
#define ESP8266PARSER_H_

#include <stdint.h>

class ESP8266Parser {
public:
	ESP8266Parser();
	virtual ~ESP8266Parser();

	void process();
	char recv_c = 0;
	char last_c = 0;
	uint16_t index = 0;
	char buffer[20];
};

#endif /* ESP8266PARSER_H_ */
