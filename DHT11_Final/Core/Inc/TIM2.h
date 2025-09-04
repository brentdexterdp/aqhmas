/*
 * TIM2.h
 *
 *  Created on: May 6, 2024
 *      Author: ADMIN
 */

#ifndef INC_TIM2_H_
#define INC_TIM2_H_

#include <stm32f4xx.h>

extern int done;

void Tim2_Init(void);
void TIM2_IRQHandler(void);
void delay_us(int us_delay);
void delay_ms(int ms_delay);
void delay_s(int s_delay);
void delay_ms_pseudo(int delay);

#endif /* INC_TIM2_H_ */
