/*
 * TIM3.h
 *
 *  Created on: May 19, 2024
 *      Author: ADMIN
 */

#ifndef INC_TIM3_H_
#define INC_TIM3_H_

#include <stm32f4xx.h>

#include "TIM2.h"

extern int done;
extern int checker_1;
extern int Tim3_initializing;

extern int sys_stuck;
extern int break_wait_for;

void Tim3_Init(void);
void TIM3_IRQHandler(void);
void Tim3_wait_for_timeout();
void Tim3_esp_not_stuck();

#endif /* INC_TIM3_H_ */
