/*
 * TIM2.c
 *
 *  Created on: May 6, 2024
 *      Author: ADMIN
 */

#include "TIM2.h"

void Tim2_Init(void){

	RCC->APB1ENR |= (1<<0); // Enable clock for TIM2
	TIM2->PSC = 16-1; // Set PSC+1 = 16000

	TIM2->ARR = 1; // Set timer to reset after CNT = 1 or 1 us

	TIM2->DIER |= (1<<0); // Enable timer interrupt generation

	NVIC->IP[28] = (3 << 4) & 0xff; // Set priority to 3
	NVIC->ISER[0] = (1 << 28); // Enable interrupt
	TIM2->SR &= ~(1<<0);

	TIM2->CR1 &= ~(1<<0); // Disable timer, for now

}

void TIM2_IRQHandler(void){

	done = 1;

	TIM2->CR1 &= ~(1<<0);
	TIM2->SR &= ~(1<<0); // Clear UIF update interrupt flag

}

void delay_us(int us_delay){

	TIM2->ARR = us_delay;
	done = 0;
	TIM2->CR1 |= (1<<0);

	while(!done){}

}

void delay_ms(int ms_delay){

	for(int i=0; i<=ms_delay; i++){
		delay_us(1000);
	}

}

void delay_s(int s_delay){

	for(int i=0; i<=s_delay; i++){
		delay_ms(1000);
	}

}

void delay_ms_pseudo(int delay) {

	// delay program by given time in milliseconds
	int i;
	for (; delay > 0; delay--) {
		for (i = 0; i < 2657; i++);
	}

}

