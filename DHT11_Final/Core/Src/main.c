#include "main.h"

#include <stdint.h>
#include <stdbool.h>
#include <stm32f4xx.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "SysConfig.h"
#include "TIM2.h"
#include "TIM3.h"
#include "DHT11.h"

#include "Usart6.h"
#include "ESP8266.h"

int checker_1;
int checker_2;
int done = 0;
char char_checker;
char char_checker_2;

int sys_stuck = 0;
int break_wait_for = 0;

unsigned int sevsegnum = 0;
int sevseg_data = 3192;

float RH_whole = 0;
float RH_dec = 0;
float Temp_whole = 0;
float Temp_dec = 0;
float RH_Temp_checksum = 0;

float heat_index;
float Temp_comb;
float RH_comb;

int dataIndex = 0;
int receptionDone = 0;
char dataBuffer[300];

int Tim3_initializing = 1;

int main(void){

	checker_1 = 1997;
	sys_config();
	Tim2_Init();
	Tim3_Init();

	delay_s(5);
	checker_1 = 2997;

	Usart6_Init();
	sevseg_init();

//	ESP8266_Init("_WiFi@UPD", "");
//	ESP8266_Init("PLDTHOMEFIBRXFGxc", "PLDTWIFIae7dL");
//	ESP8266_Init("iPhone (8)", "bl3ugh@93");
	ESP8266_Init("owo", "mamamopassword");
//	ESP8266_Init("EEE192-429", "EEE192_Room429");
//	ESP8266_Init("iPhone (8)", "randompassword");

	while (1){

		break_wait_for = 0;
//		break_wait_for = 1;

		TIM4->DIER &= ~(1<<0);

		Tim3_wait_for_timeout();

		DHT11_Start();
		DHT11_Response();
		DHT11_Read();

		Tim3_esp_not_stuck();

		TIM4->DIER |= (1<<0);

		int heat_index_int = DHT11_HI_calc();
		sevseg_data = heat_index_int;

		if(heat_index_int >= 42){
			alert(0);
		} else if (heat_index_int < 42 && heat_index_int >= 33){
			alert(1);
		} else {
			alert(2);
		}

		ESP8266_Send_Data("BLH8QO6SFC28OYXO", 1, Temp_whole);
		if (!break_wait_for) {delay_s(20);}
		ESP8266_Send_Data("BLH8QO6SFC28OYXO", 2, RH_whole);
		if (!break_wait_for) {delay_s(20);}
		ESP8266_Send_Data("BLH8QO6SFC28OYXO", 3, heat_index_int);
		if (!break_wait_for) {delay_s(60);}

	}

}

void TIM4_Init(void){
	RCC->APB1ENR |= (1<<2); // Enable clock for TIM4
	TIM4->PSC = 1600-1; // Set PSC+1 = 160000
	TIM4->ARR = 4; // Set timer to reset after CNT = 100
	TIM4->DIER |= (1<<0); // Enable timer interrupt generation
	NVIC->IP[TIM4_IRQn] = (6 << 4); // Set priority to 2
	NVIC->ISER[TIM4_IRQn >> 5] |= (1 << (TIM4_IRQn % 32)); // Enable interrupt
	TIM4->SR &= ~(1<<0);
	TIM4->EGR |= (1<<0);
	TIM4->CR1 |= (1<<0); // enable timer
}

void sevseg_init(void){
	TIM4_Init();
	RCC->AHB1ENR |= (1<<0);
	RCC->AHB1ENR |= (1<<1);
	RCC->AHB1ENR |= (1<<2);
	//PA0 1st digit
	GPIOA->MODER &= ~(1<<1);
	GPIOA->MODER |= (1<<0);
	GPIOA->OTYPER &= ~(1<<0);
	GPIOA->ODR |= (1<<0);
	// PB5 2nd digit
	GPIOB->MODER &= ~(1<<11);
	GPIOB->MODER |= (1<<10);
	GPIOB->OTYPER &= ~(1<<5);
	GPIOB->ODR |= (1<<5);
	//PB4 3rd digit
	GPIOB->MODER &= ~(1<<9);
	GPIOB->MODER |= (1<<8);
	GPIOB->OTYPER &= ~(1<<4);
	GPIOB->ODR |= (1<<4);
	// PA1 4th digit
	GPIOA->MODER &= ~(1<<3);
	GPIOA->MODER |= (1<<2);
	GPIOA->OTYPER &= ~(1<<1);
	GPIOA->ODR |= (1<<1);
	// PB3 a
	GPIOB->MODER &= ~(1<<7);
	GPIOB->MODER |= (1<<6);
	GPIOB->OTYPER &= ~(1<<3);
	GPIOB->ODR &= ~(1<<3);
	// PA8 b
	GPIOA->MODER &= ~(1<<17);
	GPIOA->MODER |= (1<<16);
	GPIOA->OTYPER &= ~(1<<8);
	GPIOA->ODR &= ~(1<<8);
	// PC0 c
	GPIOC->MODER &= ~(1<<1);
	GPIOC->MODER |= (1<<0);
	GPIOC->OTYPER &= ~(1<<0);
	GPIOC->ODR &= ~(1<<0);
	// PC1 d
	GPIOC->MODER &= ~(1<<3);
	GPIOC->MODER |= (1<<2);
	GPIOC->OTYPER &= ~(1<<1);
	GPIOC->ODR &= ~(1<<1);
	// PB0 e
	GPIOB->MODER &= ~(1<<1);
	GPIOB->MODER |= (1<<0);
	GPIOB->OTYPER &= ~(1<<0);
	GPIOB->ODR &= ~(1<<0);
	// PB10 f
	GPIOB->MODER &= ~(1<<21);
	GPIOB->MODER |= (1<<20);
	GPIOB->OTYPER &= ~(1<<10);
	GPIOB->ODR &= ~(1<<10);
	// PA4 g
	GPIOA->MODER &= ~(1<<9);
	GPIOA->MODER |= (1<<8);
	GPIOA->OTYPER &= ~(1<<4);
	GPIOA->ODR &= ~(1<<4);
	//PC9 red
	GPIOC->MODER &= ~(1<<19);
	GPIOC->MODER |= (1<<18);
	GPIOC->OTYPER &= ~(1<<1);
	GPIOC->ODR &= ~(1<<9);
	//PB8 yellow
	GPIOB->MODER &= ~(1<<17);
	GPIOB->MODER |= (1<<16);
	GPIOB->OTYPER &= ~(1<<1);
	GPIOB->ODR &= ~(1<<8);
	//PB9 green
	GPIOB->MODER &= ~(1<<19);
	GPIOB->MODER |= (1<<18);
	GPIOB->OTYPER &= ~(1<<1);
	GPIOB->ODR &= ~(1<<9);
}

void sevseg_display(int input){
	if (input > 9999){
		input = 9999;
	}else if (input < 0){
		input = 0;
	}
	int thou = (input / 1000) % 10;
	int hund = (input / 100) % 10;
	int tens = (input / 10) % 10;
	int ones = (input / 1) % 10;

		int digit = 0;
		switch (sevsegnum) {
		case 0:
			digit = thou;
			GPIOA->ODR &= ~(1<<0); //1
			GPIOB->ODR |= (1<<5); //2
			GPIOB->ODR |= (1<<4); //3
			GPIOA->ODR |= (1<<1); //4
			sevsegnum = 3;
		break;

		case 1:
			digit = hund;
			GPIOA->ODR |= (1<<0); //1
			GPIOB->ODR &= ~(1<<5); //2
			GPIOB->ODR |= (1<<4); //3
			GPIOA->ODR |= (1<<1); //4
			sevsegnum = 0;
		break;

		case 2:
			digit = tens;
			GPIOA->ODR |= (1<<0); //1
			GPIOB->ODR |= (1<<5); //2
			GPIOB->ODR &= ~(1<<4); //3
			GPIOA->ODR |= (1<<1); //4
			sevsegnum = 1;
		break;

		case 3:
			digit = ones;
			GPIOA->ODR |= (1<<0); //1
			GPIOB->ODR |= (1<<5); //2
			GPIOB->ODR |= (1<<4); //3
			GPIOA->ODR &= ~(1<<1); //4
			sevsegnum = 2;
		break;
		}
		switch(digit) {
		case 0: //abcdef
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR |= (1<<1); //d
			GPIOB->ODR |= (1<<0); //e
			GPIOB->ODR |= (1<<10); //f
			GPIOA->ODR &= ~(1<<4); //g
		break;

		case 1:
			GPIOB->ODR &= ~(1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR &= ~(1<<1); //d
			GPIOB->ODR &= ~(1<<0); //e
			GPIOB->ODR &= ~(1<<10); //f
			GPIOA->ODR &= ~(1<<4); //g
		break;

		case 2:
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR &= ~(1<<0); //c
			GPIOC->ODR |= (1<<1); //d
			GPIOB->ODR |= (1<<0); //e
			GPIOB->ODR &= ~(1<<10); //f
			GPIOA->ODR |= (1<<4); //g
		break;

		case 3:
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR |= (1<<1); //d
			GPIOB->ODR &= ~(1<<0); //e
			GPIOB->ODR &= ~(1<<10); //f
			GPIOA->ODR |= (1<<4); //g
		break;

		case 4:
			GPIOB->ODR &= ~(1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR &= ~(1<<1); //d
			GPIOB->ODR &= ~(1<<0); //e
			GPIOB->ODR |= (1<<10); //f
			GPIOA->ODR |= (1<<4); //g
		break;

		case 5:
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR &= ~(1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR |= (1<<1); //d
			GPIOB->ODR &= ~(1<<0); //e
			GPIOB->ODR |= (1<<10); //f
			GPIOA->ODR |= (1<<4); //g
		break;

		case 6:
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR &= ~(1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR |= (1<<1); //d
			GPIOB->ODR |= (1<<0); //e
			GPIOB->ODR |= (1<<10); //f
			GPIOA->ODR |= (1<<4); //g
		break;

		case 7:
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR &= ~(1<<1); //d
			GPIOB->ODR &= ~(1<<0); //e
			GPIOB->ODR &= ~(1<<10); //f
			GPIOA->ODR &= ~(1<<4); //g
		break;

		case 8:
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR |= (1<<1); //d
			GPIOB->ODR |= (1<<0); //e
			GPIOB->ODR |= (1<<10); //f
			GPIOA->ODR |= (1<<4); //g
		break;

		case 9:
			GPIOB->ODR |= (1<<3); //a
			GPIOA->ODR |= (1<<8); //b
			GPIOC->ODR |= (1<<0); //c
			GPIOC->ODR |= (1<<1); //d
			GPIOB->ODR &= ~(1<<0); //e
			GPIOB->ODR |= (1<<10); //f
			GPIOA->ODR |= (1<<4); //g
		break;
		}
		for (int j=0; j<1000; j++){
		}

	//GPIOA->ODR |= (1<<0); //1
	//GPIOB->ODR |= (1<<4); //2
	//GPIOB->ODR |= (1<<5); //3
	//GPIOC->ODR |= (1<<0); //4
}

void alert(int alert){
	// 0 for green, 1 for yellow, 2 for red
	switch (alert){
	case 0:
		GPIOC->ODR &= ~(1<<9);
		GPIOB->ODR &= ~(1<<8);
		GPIOB->ODR |= (1<<9);
		break;

	case 1:
		GPIOC->ODR &= ~(1<<9);
		GPIOB->ODR |= (1<<8);
		GPIOB->ODR &= ~(1<<9);
		break;

	case 2:
		GPIOC->ODR |= (1<<9);
		GPIOB->ODR &= ~(1<<8);
		GPIOB->ODR &= ~(1<<9);
		break;
	}
}

void TIM4_IRQHandler(void){
	sevseg_display(sevseg_data);
	TIM4->SR &= ~(5<<0); // Clear UIF update interrupt flag
}
