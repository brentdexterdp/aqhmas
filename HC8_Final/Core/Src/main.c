#include "main.h"
#include "stdint.h"
#include "stdio.h"
#include "stm32f4xx.h"

#include "TIM2.h"
#include "TIM3.h"
#include "ESP8266.h"
#include "Usart6.h"

// For ESP01 Bare-metal

char dataBuffer[300];
int Tim3_initializing = 1;
int esp_at_reset = 1;

int dataIndex = 0;

int checker_1;
char char_checker;
char char_checker_2;

// For ESP01 Bare-metal

// For testing purposes, delete in actual deployment

//int _write(int file, char *ptr, int len)
// {
//	 int DataIdx;
//	 for (DataIdx = 0; DataIdx < len; DataIdx++)
//	 {
//		 ITM_SendChar(*ptr++);
//	 }
//	 return len;
// }

// variables
volatile unsigned int capture = 0;
volatile unsigned int ctr_overflow = 0;
int done = 0;
int read_positive = 0;

unsigned int sevsegnum = 0;
int sevseg_data = 3192;
int first = 5;

int sys_stuck = 0;
int break_wait_for = 0;

unsigned int current_captured;
unsigned int last_captured;
float timespan;
float timespan_ms;
unsigned int first_edge_captured;
float ctr_freq;
float ctr_period;
int ppm_value;

volatile unsigned int str_overflow;
unsigned int str_current_captured;

// functions
void GPIO_Init(void);
void IC_Init(void);
void sevseg_init(void);
void sevseg_display(int input);
void alert(int alert);

int main(void)
{
  /* USER CODE BEGIN 1 */
//	unsigned int current_captured;
//	unsigned int last_captured;
	//float timespan;
	//float timespan_ms;
//	unsigned int first_edge_captured;
//	float ctr_freq;
//	float ctr_period;
//	int ppm_value;
	//float time_elapsed;
	//int start_timer;

	capture = 0; // set to 1 if a capture occurs
	ctr_overflow = 0; // number of overflows
	current_captured = 0; // copy current counter value
	last_captured = 0;
	first_edge_captured = 0; // set to 1 after first capture so second capture knows pulse can be measured
	ctr_freq = SystemCoreClock / 250; // TIM1 counter frequency based on 84 MHz clock
	ctr_period = 1 / ctr_freq; // TIM1 counter period

	Usart6_Init();
	sevseg_init();

	//printf("test print\n");

	Tim2_Init();
	Tim3_Init();

	checker_1 = 1997;
	delay_s(5);
	checker_1 = 2997;

	//ESP8266_Init("GlobeAtHome_2171B_2.4", "jZGtG7sZ");
//	ESP8266_Init("_WiFi@UPD", "");
//	ESP8266_Init("EEE192-429", "EEE192_Room429");
//	ESP8266_Init("iPhone (8)", "bl3ugh@93");
//	ESP8266_Init("PLDTHOMEFIBRXFGxc", "PLDTWIFIae7dL");
	ESP8266_Init("owo", "mamamopassword");

	//printf("Connected\n");

	//int counter = 0;

//	start_timer = 1;

	//checker_1 = 1995;

	GPIO_Init();
	IC_Init();

  while (1)
  {

//	  if (start_timer == 1){
//		  Tim3_wait_for_timeout();
//		  start_timer = 0;
//	  }

	  break_wait_for = 0;

	  if(capture){
		  	  	str_current_captured = current_captured;
	  			//current_captured = TIM1->CCR2;

	  			if (first_edge_captured == 0){ // first time capturing, set first_edge_captured to 1
	  				first_edge_captured = 1;
	  			}
	  			else if (first_edge_captured == 1){ // first_edge_captured is 1 thus start recording frequency
	  				first_edge_captured = 0;
	  				str_overflow = ctr_overflow;
	  				timespan = ctr_period * (str_current_captured - last_captured + (str_overflow*(1 + 65535))); // record the timespan between the two captures
	  				timespan_ms = timespan * 1000;
	  				ppm_value = 5*(timespan_ms - 2);
	  				if (first != 0){
	  					first -= 1;
	  					checker_1 = ppm_value;
	  					//sevseg_data = 487;
	  					//alert(2);
	  					//ESP8266_Send_Data("IDM6L0ASEGKNYV0F", 1, 487);
	  					delay_s(20);
	  					//HAL_Delay(15);
	  					last_captured = str_current_captured;
	  					capture = 0;
	  					ctr_overflow = 0;
	  					continue;
	  				}
	  				//printf("%d PPM\n", ppm_value);
	  				//ESP8266_Send_Data("IDM6L0ASEGKNYV0F", 1, ppm_value);
	  				//delay_s(20);
	  				//printf("%d Counter\n", counter);
	  				//counter += 1;

	  				// time_elapsed = ((TIM3->CNT)*(TIM3->PSC))/SystemCoreClock;

	  				checker_1 = ppm_value;
	  				sevseg_data = ppm_value;
	  				if (ppm_value <= 1000){
	  					alert(2);
	  				}
	  				else if ((ppm_value > 1000) && (ppm_value <= 4000)){
	  					alert(1);
	  				}
	  				else{
	  					alert(0);
	  				}
	  				ESP8266_Send_Data("IDM6L0ASEGKNYV0F", 1, ppm_value);

//	  				if (counter == 0){
//	  					alert(0);
//	  					counter += 1;
//	  				}
//	  				else if (counter == 1){
//	  					alert(1);
//	  					counter += 1;
//	  				}
//	  				else if (counter == 2){
//	  					alert(2);
//	  					counter = 0;
//	  				}
	  				if (!break_wait_for) {delay_s(20);}
	  				//start_timer = 1;
	  					//counter = 0;

	  				//delay_s(15);
	  				//HAL_Delay(15);

	  				last_captured = str_current_captured;
	  				IC_Init();
	  				continue;

	  			}

	  			last_captured = str_current_captured;

	  			capture = 0;
	  			ctr_overflow = 0;

	  }

  }

}

void GPIO_Init(void){
	RCC->AHB1ENR |= (1 << 0); // enable GPIOA peripheral clock

	// insert bit configuration for PA9

	// set PA9 to alternate function mode
	GPIOA->MODER |= (1 << 19);
	GPIOA->MODER &= ~(1 << 18);

	// use AF01 for PA9
	GPIOA->AFR[1] &= ~(1 << 7);
	GPIOA->AFR[1] &= ~(1 << 6);
	GPIOA->AFR[1] &= ~(1 << 5);
	GPIOA->AFR[1] |= (1 << 4);

	// set as no pull-up, pull-down
	GPIOA->PUPDR &= ~(1 << 19);
	GPIOA->PUPDR &= ~(1 << 18);
}

void IC_Init(void){

	capture = 0;
	ctr_overflow = 0;
	read_positive = 1;

	RCC->APB2RSTR |= (1 << 0);
	RCC->APB2RSTR &= ~(1 << 0);

	// insert code here to enable clock of TIM1
	RCC->APB2ENR |= (1 << 0);

	// insert code here to set counter in edge - aligned mode
	TIM1->CR1 &= ~(1 << 6);
	TIM1->CR1 &= ~(1 << 5);

	// insert code here to set counting direction as up - counting
	TIM1->CR1 &= ~(1 << 4);

	TIM1->PSC = 249; // set clock prescaler
	TIM1->ARR = 0xFFFF; // set ARR to max

	// insert code here to set direction of channel ’n’ as input
	// Used channel 2 due to PA9 AF01 being TIM1_CH2
	TIM1->CCMR1 &= ~(1 << 9);
	TIM1->CCMR1 |= (1 << 8);

	// insert code here to program the input filter as follows :
	// set sampling frequency to timer clock frequency
	// set input filter duration to N = 4
	// bits 15 to 12 for the IC2F bits, 0010 to satisfy above requirements
	TIM1->CCMR1 &= ~(1 << 15);
	TIM1->CCMR1 &= ~(1 << 14);
	TIM1->CCMR1 |= (1 << 13);
	TIM1->CCMR1 &= ~(1 << 12);

	// insert code here to program the input prescaler as follows :
	// no prescaler to capture each transition
	// set IC2PSC bits to 00
	TIM1->CCMR1 &= ~(1 << 11);
	TIM1->CCMR1 &= ~(1 << 10);

	// insert code here to set active transition as rising edge
	// use 11 for non-inverted / rising edge
	//TIM1->CCER |= (1 << 5);
	//TIM1->CCER |= (1 << 7);

	// insert code here to enable capture for channel ’n’
	TIM1->CCER |= (1 << 4);

	TIM1->EGR |= (1 << 0); // generate an update
	TIM1->CR1 |= (1 << 0); // enable counter
	TIM1->DIER |= (1 << 2); // enable interrupt for timer 1
	TIM1->DIER |= (1 << 0); // enable update interrupt
	TIM1->SR &= ~(1 << 2); // clear CCIF
	TIM1->SR &= ~(1 << 0); // clear UIF

	// set interrupt priority
	NVIC->IP[TIM1_CC_IRQn] = (2 << 4);
	NVIC->IP[TIM1_UP_TIM10_IRQn] = (3 << 4);

	// enable interrupt
	NVIC->ISER[TIM1_CC_IRQn >> 5] |= (1 << (TIM1_CC_IRQn % 32));
	NVIC->ISER[TIM1_UP_TIM10_IRQn >> 5] |= (1 << (TIM1_UP_TIM10_IRQn % 32));

	TIM1->CCER &= ~(1 << 7);
	TIM1->CCER &= ~(1 << 5);

}

void TIM1_CC_IRQHandler(void){

	if(TIM1->SR & (1 << 2)){
		capture = 1;
		current_captured = TIM1->CCR2;
		TIM1->SR &= ~(1 << 2); // clear CCIF
		if (read_positive == 1){
			read_positive = 0;
			TIM1->CCER |= (1 << 5);
		}
		else{
			read_positive = 1;
			TIM1->CCER &= ~(1 << 5);
		}
	}
}

void TIM1_UP_TIM10_IRQHandler(void){
	if(TIM1->SR & (1 << 0)){
		ctr_overflow++;
		TIM1->SR &= ~(1 << 0); // clear UIF
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
	TIM4->SR &= ~(1<<0); // Clear UIF update interrupt flag
}
