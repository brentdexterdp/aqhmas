#include "Usart6.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

void Usart6_Init(){

	RCC->AHB1ENR |= (1<<0);

	GPIOA->MODER &= ~(1<<22);
	GPIOA->MODER |= (1<<23);

	GPIOA->MODER &= ~(1<<24);
	GPIOA->MODER |= (1<<25);

//	GPIOA->OTYPER &= ~(1 << 11);	// PA11(TX) push-pull
//	GPIOA->OSPEEDR |= (0b11 << 22);	// PA11(TX) high-speed mode

//	GPIOA->PUPDR &= ~(0b11 << 24); // PA12(RX) pulldown
//	GPIOA->PUPDR |=  (0b10 << 24);

	GPIOA->AFR[1] = (136<<12); // set both PA11 and PA12 to AFRH

	RCC->APB2ENR |= (1<<5);	// Enable USART6 peripheral
	RCC->APB2RSTR |= (1 << 5);	// Reset the whole peripheral
	RCC->APB2RSTR &= ~(1 << 5);

	USART6->CR1 &= ~(0b11 << 2); // disable USART6 RX and TX for now
	USART6->CR1 |=  (1 << 13); // enable USART6

	USART6->BRR &= ~(0x0000FFFF);
	USART6->BRR |= (0x0000008B);

	USART6->CR1 &= ~(1 << 15);	// OVER8 = 0
	USART6->CR2 &= ~(0b11 << 12);	// One (1) stop bit
	USART6->CR1 &= ~(1 << 10);	// No parity

	USART6->CR2 &= ~(1 << 11);
	USART6->CR1 &= ~(1 << 12);
	USART6->CR3 &= ~(0b11 << 8);

	NVIC->IP[71] = (4 << 4) & 0xff; // Set priority to 2
	NVIC->ISER[2] = (1 << 7); // position 71, ISER 2, bit 7

	USART6->CR1 |= (0b1011<<2);

}

void USART6_IRQHandler(void){

	if(USART6->SR & 1<<5){

		dataBuffer[dataIndex] = USART6->DR;
		dataIndex += 1;

		if(dataIndex == 300){
			dataIndex = 0;
		}

	}

}

char USART6_Read(void){

	while(!(USART6->SR & (1<<5))){}
	return USART6->DR;

}

void USART6_Write(char ch){

	while(!(USART6->SR & (1<<7)));
	USART6->DR = ch;
	while(!(USART6->SR & (1<<6)));

}

void USART6_Text_Write(char *text){

	int text_index = 0;
	int text_length = strlen(text);

	while(text_index < text_length){
		USART6_Write(text[text_index]);
		text_index++;
	}

}

void USART6_Clear_Buffer(){

	for(int i=0; i<300; i++){dataBuffer[i] = 0;}
	dataIndex = 0;
	delay_ms(200);

}

int USART6_Wait_For(char *response, int delay_further, int clear_buffer){

	int dataBufferIndex = 0;
	int responseIndex = 0;
	int responseMatched = 0;

	while(response[responseIndex] != dataBuffer[dataBufferIndex]){dataBufferIndex++;}

	while(dataBuffer[dataBufferIndex] != '\0'){

		while(response[responseIndex] == dataBuffer[dataBufferIndex]){

			char_checker = dataBuffer[dataBufferIndex];
			char_checker_2 = response[responseIndex];
			delay_ms(200);

			if(delay_further){delay_s(1);}

			responseIndex++;
			dataBufferIndex++;

			if(dataBuffer[dataBufferIndex] == '\0' || response[responseIndex] == '\0'){

				responseMatched = 1;
				break;
			}

		}

		if(responseMatched){
			break;
		} else{
			dataBufferIndex++;
		}

	}

	if(responseMatched && clear_buffer){USART6_Clear_Buffer();}

	return responseMatched;

}
