/*
 * ESP8226.h
 *
 *  Created on: Jul 29, 2021
 *      Author: 39569
 */

#ifndef INC_ESP8226_H_
#define INC_ESP8226_H_

#include "stm32f1xx_hal.h"


#define ESP_USART huart2 //define the uart group used by ESP8266
#define ESP_BUFFER_SIZE USART2_RxBUFFER_SIZE // define the buffer size
#define ESP_RxBuffer USART2_RxBuffer

void ESP_Send_cmd(char cmd[],uint32_t waittime, uint32_t delay);
void ESP_Send_Data_TT(uint8_t* data, uint16_t len);
void ESP_Send_Data_4Bytes(uint8_t* data);
void ESP_Init(uint8_t mode);
void ESP_Init_TT();



#endif /* INC_ESP8226_H_ */
