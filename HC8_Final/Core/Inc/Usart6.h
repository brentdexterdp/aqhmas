/*
 * Usart6.h
 *
 *  Created on: May 18, 2024
 *      Author: ADMIN
 */

#ifndef INC_USART6_H_
#define INC_USART6_H_

#include <stm32f4xx.h>

#include "Usart6.h"
#include "TIM2.h"
#include "TIM3.h"

extern int checker_1;
extern int esp_at_reset;

extern int dataIndex;
extern int receptionDone;
extern char dataBuffer[300];

extern char char_checker;
extern char char_checker_2;

void Usart6_Init();
char USART6_Read(void);
void USART6_Write(char ch);
void USART6_Text_Write(char *text);
void USART6_Clear_Buffer();
int USART6_Wait_For(char *response, int delay_further, int clear_buffer);

#endif /* INC_USART6_H_ */
