/*
 * SysConfig.c
 *
 *  Created on: May 6, 2024
 *      Author: ADMIN
 */

#include "SysConfig.h"

void sys_config(void) {

	RCC->AHB1ENR |= (1 << 1);

	GPIOB->MODER |= (1 << 12); // output mode
	GPIOB->MODER &= ~(1 << 13);

	GPIOB->OTYPER &= ~(1 << 6); // push-pull

	GPIOB->OSPEEDR &= ~(1 << 12); // low-speed
	GPIOB->OSPEEDR &= ~(1 << 13);

	GPIOB->PUPDR &= ~(1 << 12); // no pull-up, pull-down
	GPIOB->PUPDR &= ~(1 << 13);

}

int GPIOB_IDR_Pin(int pin_number){

	int temp = GPIOB->IDR;

	return (temp/(1<<pin_number))%2;

}
