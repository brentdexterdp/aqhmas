
#include "TIM3.h"

void Tim3_Init(void){

	RCC->APB1ENR |= (1<<1); // Enable clock for TIM3
	TIM3->PSC = 62500-1; // Set PSC+1 = 16000

	TIM3->ARR = 16384; // Set timer to reset after CNT = 100

	TIM3->DIER |= (1<<0); // Enable timer interrupt generation

	NVIC->IP[29] = (1 << 4) & 0xff; // Set priority to 1
	NVIC->ISER[0] = (1 << 29); // Enable interrupt

	TIM3->SR &= ~(1<<0);
	TIM3->CR1 &= ~(1<<0); // Disable timer, for now

}

void TIM3_IRQHandler(void){

	while(1){

		if(Tim3_initializing){
			Tim3_initializing = 0;
			TIM3->SR &= ~(1<<0);
			break;
		} else {

			sys_stuck += 1;
			break_wait_for = 1;
			TIM3->SR = 0;

			if (sys_stuck == 10){NVIC_SystemReset();}

			break;

		}

	}

}

void Tim3_wait_for_timeout(){

	TIM3->ARR = 16384;
	TIM3->CR1 |= (1<<0);

}

void Tim3_esp_not_stuck(){

	TIM3->CR1 &= ~(1<<0);
	TIM3->SR &= ~(1<<0);
	TIM3->CNT = 0;

}
