/*
 * DHT22.h
 *
 *  Created on: Jul 28, 2021
 *      Author: 39569
 */

#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include "stm32f1xx_hal.h"
// pin
#define DHT_PORT GPIOA
#define DHT_PIN GPIO_PIN_6
// error codes
#define DHT_ERR_RESPONSE_FAILED 1
#define DHT_ERR_READ_BIT_FAILED 2
#define DHT_ERR_DATA_CHKSUM_FAILED 3
// function prototypes
void DHT22_Init(); //initialization function
void ONE_WIRE_Start(); //Start communication
uint8_t ONE_WIRE_Response(); //Sensor response for start command
void DHT22_THMsg(uint16_t Temp, uint16_t Humi, char THMsg[30]); //generate message string
uint8_t DHT22_Get_Data(uint16_t *temp,uint16_t *humi); //function available for user to use
uint64_t DHT22_ReadData(void); // read original data

#endif /* INC_DHT22_H_ */

