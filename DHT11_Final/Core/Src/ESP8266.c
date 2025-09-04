#include "ESP8266.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

void ESP8266_Init(char *ssid, char *password){

	int ESP_response = 0;
	char data_to_send[100];

	USART6_Text_Write("AT+RST\r\n");
	Tim3_wait_for_timeout();
	delay_s(10);
//	while(!ESP_response){ESP_response = USART6_Wait_For("WIFI GOT IP\r\n", 1, 1);}
	Tim3_esp_not_stuck();

	USART6_Text_Write("AT\r\n");
	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For("OK\r\n", 1, 1);}
	Tim3_esp_not_stuck();

	USART6_Text_Write("AT+CWMODE=1\r\n");
	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For("OK\r\n", 1, 1);}
	Tim3_esp_not_stuck();

	sprintf(data_to_send, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
	USART6_Text_Write(data_to_send);
	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For("OK\r\n", 1, 1);}
	Tim3_esp_not_stuck();

	USART6_Text_Write("AT+CIPMUX=0\r\n");
	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For("OK\r\n", 1, 1);}
	Tim3_esp_not_stuck();

}

void ESP8266_Send_Data(char *writeAPIKey, int fieldNumber, int value){

	int ESP_response = 0;
	char data_to_send[300];
	char data_to_send_two[300];

	USART6_Text_Write("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n");
	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For("OK\r\n", 0, 1);}
	Tim3_esp_not_stuck();

	sprintf(data_to_send, "GET /update?api_key=%s&field%d=%u\r\n", writeAPIKey, fieldNumber, value);

	sprintf(data_to_send_two, "AT+CIPSEND=%d\r\n", strlen(data_to_send));
	USART6_Text_Write(data_to_send_two);
	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For(">", 0, 1);}
	Tim3_esp_not_stuck();

	USART6_Text_Write(data_to_send);
	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For("SEND OK\r\n", 0, 0);}
	Tim3_esp_not_stuck();

	ESP_response = 0;
	Tim3_wait_for_timeout();
	while(!ESP_response && !break_wait_for){ESP_response = USART6_Wait_For("CLOSED", 0, 1);}
	Tim3_esp_not_stuck();

}

