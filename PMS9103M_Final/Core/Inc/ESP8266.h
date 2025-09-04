/*
 * ESP8266.h
 *
 *  Created on: May 18, 2024
 *      Author: ADMIN
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_

#include <stm32f4xx.h>
#include "Usart6.h"
#include "TIM2.h"
#include "TIM3.h"

extern int dataIndex;
extern int receptionDone;
extern char dataBuffer[300];

extern int checker_1;
extern char char_checker;
extern char char_checker_2;

extern int sys_stuck;
extern int break_wait_for;

void ESP8266_Init(char *SSID, char *PASSWORD);
void ESP8266_Send_Data(char *writeAPIKey, int fieldNumber, int value);

#endif /* INC_ESP8266_H_ */
