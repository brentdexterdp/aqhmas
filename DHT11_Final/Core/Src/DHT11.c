/*
 * DHT11.c
 *
 *  Created on: May 6, 2024
 *      Author: ADMIN
 */

#include "DHT11.h"
#include <math.h>

void DHT11_Start() {

	GPIOB->MODER |= (1 << 12); // output mode
	GPIOB->MODER &= ~(1 << 13);
	GPIOB->ODR &= ~(1 << 6);
	delay_us(18000);
	GPIOB->ODR |= (1 << 6);
	delay_us(40);

	GPIOB->MODER &= ~(1 << 12); // input mode
	GPIOB->MODER &= ~(1 << 13);
}

void DHT11_Response(void){

	while(GPIOB_IDR_Pin(6)){}
	delay_us(80);
	while(!GPIOB_IDR_Pin(6)){}
	delay_us(80);

}

int DHT11_Read_Eight_Bit(void){

	int temp = 0;

	for(int i = 0; i<8; i++){

		while(!GPIOB_IDR_Pin(6)){}
		delay_us(40);

		if(!GPIOB_IDR_Pin(6)){
			temp &= ~(1<<(7-i));
		} else{
			temp |= (1<<(7-i));
		}

		while(GPIOB_IDR_Pin(6)){}

	}

	return temp;

}

void DHT11_Read(){

	RH_whole = DHT11_Read_Eight_Bit();
	RH_dec = DHT11_Read_Eight_Bit();
	Temp_whole = DHT11_Read_Eight_Bit();
	Temp_dec = DHT11_Read_Eight_Bit();
	RH_Temp_checksum = DHT11_Read_Eight_Bit();

}

int DHT11_HI_calc(){

	float c1 = -42.379;
	float c2= 2.04901523;
	float c3 = 10.14333127;
	float c4 = -0.22475541;
	float c5 = -6.83783*pow(10,-3);
	float c6 = -5.481717*pow(10,-2);
	float c7 = 1.22874*pow(10,-3);
	float c8 = 8.5282*pow(10,-4);
	float c9 = -1.99*pow(10,-6);

	Temp_comb = Temp_whole + (Temp_dec/10);
	Temp_comb = (Temp_comb*9/5) + 32;
	RH_comb = RH_whole + (RH_dec/10);

	heat_index = c1+c2*Temp_comb+c3*RH_comb+c4*Temp_comb*RH_comb+c5*pow(Temp_comb,2)+c6*pow(RH_comb,2)+c7*pow(Temp_comb,2)*RH_comb+c8*Temp_comb*pow(RH_comb,2)+c9*pow(Temp_comb,2)*pow(RH_comb,2);
	heat_index = (heat_index-32)*5/9;

	return (int)heat_index;

}

