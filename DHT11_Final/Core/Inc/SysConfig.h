/*
 * SysConfig.h
 *
 *  Created on: May 6, 2024
 *      Author: ADMIN
 */

#ifndef INC_SYSCONFIG_H_
#define INC_SYSCONFIG_H_

#include <stm32f4xx.h>

void sys_config(void);
int GPIOB_IDR_Pin(int pin_number);

#endif /* INC_SYSCONFIG_H_ */
