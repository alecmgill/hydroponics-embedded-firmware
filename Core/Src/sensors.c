/*
 * sensors.c
 *
 *  Created on: Feb 6, 2022
 *      Author: Alec
 */

//uint8_t check=2, temp_l, temp_h;
//uint16_t temp;
//float temperature;
#include "sensors.h"
#include "main.h"
#include"string.h"
#include "water_temp_driver.h"



uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;
double Temperature = 0;
float Humidity = 0;
uint8_t Presence = 0;
double readWaterTemp(void)
{
	 DWT_Delay_Init();

	 Set_Pin_Output(water_temp_GPIO_Port, water_temp_Pin);   // set the pin as output
	 HAL_GPIO_WritePin (water_temp_GPIO_Port, water_temp_Pin, 1);
	 DWT_Delay_us(100);
	 Presence = DS18B20_Start();

	  DWT_Delay_us (300);
	  DS18B20_Write (0xCC);  // skip ROM
	  DS18B20_Write (0x44);  // convert t
	  DWT_Delay_us (800000);

	  Presence = DS18B20_Start();
	  DWT_Delay_us (300);
	  DS18B20_Write (0xCC);  // skip ROM
	  DS18B20_Write (0xBE);  // Read Scratch-pad

	  Temp_byte1 = DS18B20_Read();
	  Temp_byte2 = DS18B20_Read();
	  TEMP = (Temp_byte2<<8)|Temp_byte1;
	  Temperature = (float)TEMP/16;
	  return Temperature;
}







