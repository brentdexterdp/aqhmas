/*
 * DHT11.h
 *
 *  Created on: May 6, 2024
 *      Author: ADMIN
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include <stm32f4xx.h>
#include "Tim2.h"

extern float RH_whole;
extern float RH_dec;
extern float Temp_whole;
extern float Temp_dec;
extern float RH_Temp_checksum;
extern int checker_1;

extern float heat_index;
extern float Temp_comb;
extern float RH_comb;

void DHT11_Start();
void DHT11_Response(void);
int DHT11_Read_Eight_Bit(void);
void DHT11_Read();
int DHT11_HI_calc();

#endif /* INC_DHT11_H_ */
