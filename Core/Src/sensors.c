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
#include "TDS_driver.h"
#include "ADC_driver.h"
extern int getMedianNum();

uint8_t Temp_byte1, Temp_byte2;
uint16_t TEMP;
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


#define VREF 3.3      			// analog reference voltage(Volt) of the ADC
#define SCOUNT  30           	// sum of sample point
int nutrientBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int nutrientBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;
int analogBufferTemp[SCOUNT];

float readWaterTDS()
{
	  // Get nutrient level
	for(int i = 0; i<SCOUNT+10; i++)
	{
		get_nutrient_ph_value();
		if(i>9)nutrientBuffer[i-10] = nutrient_ph_values[1];   // skip first 10 values since they are way off (pH: position 0 Nutrient: position 1)
	    HAL_Delay(10);
	}

   for(copyIndex=0;copyIndex<SCOUNT;copyIndex++) analogBufferTemp[copyIndex]= nutrientBuffer[copyIndex];
   averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)0.000805664; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
   float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
   float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
   tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value

	//voltsTDS = (raw*0.000805664); // .000805664 volts per 1 ADC value
	return tdsValue;
}

void get_nutrient_ph_value()
{
	nutrient_ph_values[0] = get_ADC_values();
	nutrient_ph_values[1] = get_ADC_values();
}






