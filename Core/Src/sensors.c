/*
 * sensors.c
 *
 *  Created on: Feb 6, 2022
 *      Author: Alec
 */

#include "sensors.h"
#include "main.h"
#include "water_temp_driver.h"
#include "TDS_driver.h"
#include "ADC_driver.h"
#include "pH_driver.h"
#include "cmsis_os.h"
#include "hydro_control_loop.h"

extern int getMedianNum();

uint8_t Temp_byte1, Temp_byte2,Presence = 0;
uint16_t TEMP;

int TDS_computation_buffer[80] = {0}, valid_value_pH  = 0, valid_value_TDS  = 0;
double TDS_voltage = 0, compensationCoefficient = 0, compensationVolatge = 0, DMA_pH_sample_avg = 0, mili_voltage, Humidity = 0;

void readWaterTemp()
{
	taskENTER_CRITICAL();
	{
		 DWT_Delay_Init();
		 HAL_GPIO_WritePin (water_temp_GPIO_Port, water_temp_Pin, 1);
		 DWT_Delay_us(100);
		 Presence = DS18B20_Start();

		 DWT_Delay_us (300);
		 DS18B20_Write (0xCC);  // skip ROM
		 DS18B20_Write (0x44);  // convert t

		 Presence = DS18B20_Start();
		 DWT_Delay_us (300);
		 DS18B20_Write (0xCC);  // skip ROM
		 DS18B20_Write (0xBE);  // Read Scratch-pad

		 Temp_byte1 = DS18B20_Read();
		 Temp_byte2 = DS18B20_Read();
		 TEMP = (Temp_byte2<<8)|Temp_byte1;
		 water_temp = (double)TEMP/16;
	} taskEXIT_CRITICAL();
}


void readWaterTDS() // Get nutrient level
{
	TDS_voltage = getMedianNum(TDS_computation_buffer,80)*(double)0.000805664; 																					 // read the analog value more stable by averaging and convert to voltage value
	compensationCoefficient=1.0+0.02*(water_temp-25.0);   																								    		 //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
	compensationVolatge=TDS_voltage/compensationCoefficient;  																									 //temperature compensation
	TDS =(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds
}


void get_nutrient_ph_value()	// gets nutrient and ph values from the ADC DMA buffer and place it in a buffer
{
	valid_value_pH = 0;
	valid_value_TDS = 0;
	DMA_pH_sample_avg = 0;
	taskENTER_CRITICAL();
	{
		for(int j = 0; j<80; j++ )
		{
			if(j%2 == 0 )//&& nutrient_ph_values[j] < 80)
			{
				TDS_computation_buffer[valid_value_TDS] = nutrient_ph_values[j];
				valid_value_TDS++;
			}
			if(j%2 != 0)
			{
				DMA_pH_sample_avg += nutrient_ph_values[j];
				valid_value_pH++;
			}
		}
	}
	taskEXIT_CRITICAL();
	DMA_pH_sample_avg = DMA_pH_sample_avg/valid_value_pH;
}

int readWaterLevel() // 1=good 0=low
{
	return HAL_GPIO_ReadPin (water_level_GPIO_Port, water_level_Pin);
}
void calibratePhProbe()
{

}


void readPH()
{
	mili_voltage = ((DMA_pH_sample_avg/4096.0)*3.3)*1000;
	pH = convert_ph(mili_voltage);
}


void getSensorValues()
{

	for(int checkSamples = 0; checkSamples<num_sensor_samples; checkSamples++)			   // sample TDS and PH every half second for 30 times
	{
		get_nutrient_ph_value();
		readWaterTDS();
		readPH();

		sample_array_TDS[checkSamples] = TDS;
		sample_array_pH[checkSamples] = pH;
		osDelay(10);
	}
	for(int checkSamples = 0; checkSamples<num_sensor_samples; checkSamples++)			   // sample TDS and PH every half second for 30 times
	{
		TDS += sample_array_TDS[checkSamples];
		pH += sample_array_pH[checkSamples];
	}
	readWaterTemp();
	TDS = TDS/num_sensor_samples;
	pH  = pH/num_sensor_samples;
	waterTempControl();
}


