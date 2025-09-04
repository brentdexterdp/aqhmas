#include <stdint.h>
#include <stm32f4xx.h>
#include <TIM2.h>
#include <TIM3.h>
#include <Usart6.h>
#include <ESP8266.h>

unsigned int sevsegnum = 0;
int sevseg_data = 3192;
int checker_1 = 0;
int checker_2 = 0;
int checker_3 = 0;
int TIM4counter = 0;
int done = 0;
int Tim3_initializing = 1;
int dataIndex = 0;
char dataBuffer[300];
int data2Index = 0;
char data2Buffer[300];
char char_checker;
char char_checker_2;
int PM25 = -1;

int curbufind = 0;
int found = 0;

int sys_stuck = 0;
int break_wait_for = 0;

void TIM4_Init(void){
	RCC->APB1ENR |= (1<<2); // Enable clock for TIM4
	TIM4->PSC = 1600-1; // Set PSC+1 = 160000
	TIM4->ARR = 4; // Set timer to reset after CNT = 100
	TIM4->DIER |= (1<<0); // Enable timer interrupt generation
	NVIC->IP[TIM4_IRQn] = (5 << 4); // Set priority to 2
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

void Usart1_Init(){

	RCC->AHB1ENR |= (1<<0);

	GPIOA->MODER &= ~(1<<18); // PA9
	GPIOA->MODER |= (1<<19);

	GPIOA->MODER &= ~(1<<20); // PA10
	GPIOA->MODER |= (1<<21);

	//GPIOA->AFR[1] = (136<<12); // set both PA9 and PA10 to AFRH
	GPIOA->AFR[1] &= ~(0x00000FF0);
	GPIOA->AFR[1] |=  (0x00000770);

	RCC->APB2ENR |= (1<<4);	// Enable USART1 peripheral
	RCC->APB2RSTR |= (1 << 4);	// Reset the whole peripheral
	RCC->APB2RSTR &= ~(1 << 4);

	USART1->CR1 &= ~(0b11 << 2); // disable USART1 RX and TX for now
	USART1->CR1 |=  (1 << 13); // enable USART1

	USART1->BRR &= ~(0x0000FFFF);
	USART1->BRR |= (0x00000683);

	USART1->CR1 &= ~(1 << 15);	// OVER8 = 0
	USART1->CR2 &= ~(0b11 << 12);	// One (1) stop bit
	USART1->CR1 &= ~(1 << 10);	// No parity

	USART1->CR2 &= ~(1 << 11);
	USART1->CR1 &= ~(1 << 12);
	USART1->CR3 &= ~(0b11 << 8);

	NVIC->IP[37] = (2 << 4) & 0xff; // Set priority to 2
	NVIC->ISER[1] = (1 << 5); // position 71, ISER 1, bit 5

	USART1->CR1 |= (0b1011<<2);

}

void USART1_IRQHandler(void){

	if(USART1->SR & 1<<5){

		data2Buffer[data2Index] = USART1->DR;
		data2Index += 1;
		if (data2Index >= 300){
			data2Index = 0;
		}
	}

}

void readPM25(void){
	curbufind = 0;
	found = 0;
	while (!found){
		if (data2Buffer[curbufind] == 66 && curbufind < 200){
			if (data2Buffer[curbufind + 1] == 77) {
				found = 1;
				//(buf[8] << 8) + buf[9];
				PM25 = (data2Buffer[curbufind + 8] << 8) + data2Buffer[curbufind + 9];
				}
			}
		curbufind++;
		if (curbufind >= 300){
			curbufind = 0;
		}
	}
	sevseg_data = PM25;

	if (PM25 <= 25) {
		alert(2);
	} else if (PM25 <= 35) {
		alert(1);
	} else {
		alert(0);
	}
}

int main (void){
	sevseg_init();
	Tim2_Init();
	Tim3_Init();
	Usart6_Init();
	Usart1_Init();


//	for (int i = 0; i < 15; i++){
//				readPM25();
//				delay_s(1);
//			}


//	ESP8266_Init("EEE192-429", "EEE192_Room429");
//	ESP8266_Init("_WiFi@UPD", "");
	ESP8266_Init("owo", "mamamopassword");
//	ESP8266_Init("iPhone (8)", "bl3ugh@93");
//	ESP8266_Init("PLDTHOMEFIBRXFGxc", "PLDTWIFIae7dL");

	while(1){

		break_wait_for = 0;

		checker_1 = 2;
		ESP8266_Send_Data("PV6OUPDQYB5A6QS4", 1, sevseg_data);
		checker_1 = 3;
		for (int i = 0; i < 20; i++){
			readPM25();
			delay_s(1);
		}
	}

}


