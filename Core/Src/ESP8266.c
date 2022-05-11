/*
 * ESP8266.c
 *
 *  Created on: Jul 29, 2021
 *      Author: 39569
 */



#include "ESP8226.h"
#include "usart.h"
#include "string.h"

uint8_t ESP_RxCount = 0;

volatile uint8_t recv_end_flag=0;              //接收完成标记位
volatile uint8_t rx_len=0;                     //接收数据长度

uint8_t AT_Reset[] = "AT+RST\r\n";

uint8_t AT_CWSAP_DEF_Server[] = "AT+CWSAP_DEF=\"TCP_Server\",\"12345678\",5,4\r\n";  // wifi to open
uint8_t AT_CIPServer[] = "AT+CIPSERVER=1,8080\r\n"; //server port to open


uint8_t AT_CWJAP_DEF_Station[] = "AT+CWJAP_DEF=\"Tenda_26AE60\",\"wuxidefoe\"\r\n"; // wifi to connect
uint8_t AT_CIPSTART[] = "AT+CIPSTART=\"TCP\",\"192.168.0.196\",8080\r\n"; // TCP server address and port

void ESP_Send_cmd(char cmd[],uint32_t waittime, uint32_t delay){
	uint8_t len = strlen(cmd);
	HAL_UART_Transmit(&ESP_USART ,(uint8_t*)cmd,len,waittime); //set esp mode
	HAL_Delay(delay);
}

void ESP_Send_Data_TT(uint8_t* data, uint16_t len){
	//send data by transparent transmission
	for (uint16_t i=0;i<len;i++){
		ESP_RxBuffer[i] = data[i];
	}
	HAL_UART_Transmit_DMA(&ESP_USART ,(uint8_t*)ESP_RxBuffer,len); //send to usart2
}

void ESP_Send_Data_4Bytes(uint8_t* data){
	//send data once
	ESP_Send_cmd("AT+CIPMODE=0\r\n",1000,10);
	ESP_Send_cmd("AT+CIPSEND=4\r\n",1000,10);
	for (uint16_t i=0;i<4;i++){
		ESP_RxBuffer[i] = data[i];
	}
	HAL_UART_Transmit_DMA(&ESP_USART ,(uint8_t*)ESP_RxBuffer,4); //send to usart2
}



void ESP_Init_TT(){
	// init transparent transmission
	ESP_Send_cmd("AT+CIPMODE=1\r\n",1000,100); //set CIP mode
	ESP_Send_cmd("AT+CIPSEND\r\n",1000,100); //start transparent transmission
}

void ESP_Init(uint8_t mode){

	if (mode==2){
		// AP mode
		HAL_UART_Receive_DMA(&ESP_USART,(uint8_t*)ESP_RxBuffer,ESP_BUFFER_SIZE); //enable DMA receiving

		ESP_Send_cmd("+++",1000,1000); // close transparent transmission

		ESP_Send_cmd("AT+CWMODE=2\r\n",1000,100);
		//
		HAL_UART_DMAStop(&ESP_USART); //
		HAL_UART_Transmit(&ESP_USART ,(uint8_t*)AT_Reset,sizeof(AT_Reset),2000); //reset 8266
		HAL_UART_Receive(&ESP_USART,(uint8_t*)ESP_RxBuffer,ESP_BUFFER_SIZE,1000);
		HAL_Delay(1000);

		HAL_UART_Receive_DMA(&ESP_USART,(uint8_t*)ESP_RxBuffer,ESP_BUFFER_SIZE); //enable DMA receiving
		//
		ESP_Send_cmd("AT+CWSAP_DEF=\"TCP_Server\",\"12345678\",5,4\r\n",1000,100);
		//
		ESP_Send_cmd("AT+CIPMUX=1\r\n",1000,200);
		//
		ESP_Send_cmd("AT+CIPSERVER=1,8080\r\n",1000,100);

	}
	else if (mode==1){
		// station mode
		HAL_UART_Receive_DMA(&ESP_USART,(uint8_t*)ESP_RxBuffer,ESP_BUFFER_SIZE); //enable DMA receiving

		ESP_Send_cmd("+++",1000,1000); // close transparent transmission

		ESP_Send_cmd("AT+CWMODE=1\r\n",1000,100);
		//
		HAL_UART_DMAStop(&ESP_USART); //
		HAL_UART_Transmit(&ESP_USART ,(uint8_t*)AT_Reset,sizeof(AT_Reset),2000); //reset 8266
		HAL_UART_Receive(&ESP_USART,(uint8_t*)ESP_RxBuffer,ESP_BUFFER_SIZE,1000);
		HAL_Delay(2000);

		HAL_UART_Receive_DMA(&ESP_USART,(uint8_t*)ESP_RxBuffer,ESP_BUFFER_SIZE); //enable DMA receiving
		//
		ESP_Send_cmd("AT+CWJAP_DEF=\"Tenda_26AE60\",\"wuxidefoe\"\r\n",1000,10000); // connect to wifi
		//
		ESP_Send_cmd("AT+CIPMUX=0\r\n",1000,100); //single connect
		ESP_Send_cmd("AT+CIPSTART=\"TCP\",\"192.168.0.196\",8080\r\n",1000,1000); //connect TCP server

	}

}





