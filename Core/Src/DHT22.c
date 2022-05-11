/*
 * DHT22.c
 *
 *  Created on: Jul 28, 2021
 *      Author: 39569
 */

#include "DHT22.h"
#include "tim.h"
#include <string.h>
#include <stdio.h>

static GPIO_TypeDef* wire_Port;
static uint16_t wire_Pin;
static GPIO_InitTypeDef GPIO_InitStruct;
//uint8_t Hum_byte1, Hum_byte2, Temp_byte1, Temp_byte2, err;
uint16_t Sum;


void DHT22_Init(void){
	wire_Port = DHT_PORT;
	wire_Pin = DHT_PIN;
	HAL_GPIO_WritePin(wire_Port, wire_Pin, GPIO_PIN_SET);// pull up wire
}

static void gpio_output(){
	GPIO_InitStruct.Pin = wire_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(wire_Port, &GPIO_InitStruct);
}

static void gpio_input(){
	GPIO_InitStruct.Pin = wire_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(wire_Port, &GPIO_InitStruct);
}

static uint8_t DHT22_Verify_Data(uint64_t data){
	uint8_t chksum = data & 0xff;
	uint8_t sum = 0;
	for (int i=0;i<4;i++){
		sum += (uint64_t)(data>>8*(i+1)) & 0xff;
	}
	if (chksum!=sum) return 1; else return 0;
}

void ONE_WIRE_Start(){
	gpio_output();
	HAL_GPIO_WritePin(wire_Port, wire_Pin, GPIO_PIN_RESET);
	delay_us(1000);
	HAL_GPIO_WritePin(wire_Port, wire_Pin, GPIO_PIN_SET);
	delay_us(30);
	gpio_input();
}
uint8_t ONE_WIRE_Response(){
	uint8_t waitTime = 0;
	waitTime = 100;//响应超时时间设定
	while(HAL_GPIO_ReadPin(wire_Port, wire_Pin))//检测响应信号，传感器拉低80us
	{
		if((waitTime--) > 0) delay_us(1);
		else return DHT_ERR_RESPONSE_FAILED;
	}
	waitTime = 100;//响应超时时间设定
	while((!HAL_GPIO_ReadPin(wire_Port, wire_Pin)))//传感器将总线拉高80us，标志马上进入数据传输
	{
		if((waitTime--) > 0) delay_us(1);
		else return DHT_ERR_RESPONSE_FAILED;
	}

	return 0; // correct response
}



uint8_t DHT22_Get_Data(uint16_t *temp,uint16_t *humi){

	uint64_t data = DHT22_ReadData();
	if(data<0xff){
		return (uint8_t)data; //return error
	}
	if (DHT22_Verify_Data(data)) return DHT_ERR_DATA_CHKSUM_FAILED; //数据位校验

	*humi = (uint64_t)(data >> 24) & 0x00ffff;//去掉前16位湿度数据和后8位校验数据，保留16位温度数据
	*temp = (uint64_t)(data >> 8) & 0x00ffff;
	return 0;

}




uint64_t DHT22_ReadData(void)
{
	uint8_t waitTime = 0;//响应等待时间
	uint16_t i = 0;//循环变量
	uint64_t data = 0;//数据存储变量
	ONE_WIRE_Start();//传感器唤醒

	if (ONE_WIRE_Response()) return DHT_ERR_RESPONSE_FAILED;

	for(i = 0;i < 40;i ++)//获取40bit数据
	{
		waitTime = 100;//响应超时时间设定
		while(HAL_GPIO_ReadPin(wire_Port, wire_Pin))//等待50us低电平间隙到来
		{
			if((waitTime--) > 0) delay_us(1); else return DHT_ERR_READ_BIT_FAILED;
		}
		waitTime = 50;//响应超时时间设定
		while(!HAL_GPIO_ReadPin(wire_Port, wire_Pin))//间隙结束，等待数据高电平到来
		{
			if((waitTime--) > 0) delay_us(1); else return DHT_ERR_READ_BIT_FAILED;
		} //高电平保持26~28us表示低电平，保持70us表示高电平。

		delay_us(30);//这里等待30us,越过28us（但不能大于28+50）,直接判断是不是1
		if(HAL_GPIO_ReadPin(wire_Port, wire_Pin))//读取当前总线的状态
		{ data <<= 1;//右移1位空出末位
		  data |= 1;//把1添入末位
		}
		else data <<= 1; //右移1位空出末位，末位就是0
	}
	gpio_output();//设为输出
	HAL_GPIO_WritePin(wire_Port, wire_Pin, GPIO_PIN_SET);//拉高，释放总线
	return data;
}


void DHT22_THMsg(uint16_t Temp, uint16_t Humi, char THMsg[30]){
 	char TempChar[9], HumiChar[9];
 	char defaultMsg[31] = "Temp:00000000, Humi:00000000\r\n"; //5+8+2+5+8+2 Temp = xxxx
 	sprintf(TempChar, "%8d", Temp);
 	sprintf(HumiChar, "%8d", Humi);

 	strcpy(THMsg,defaultMsg);

 	for (int i=0;i<8;i++){
 		THMsg[i+5] = TempChar[i];
 		THMsg[i+15+5] = HumiChar[i];
 	}
}






/////////////////////////////////



