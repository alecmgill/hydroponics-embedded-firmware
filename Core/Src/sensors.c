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


void convertWaterTDS() // Get nutrient level
{TDS_voltage = 0;
	int j = 0;
	for(j = 0; j < 38; j++)
	{
		TDS_voltage += TDS_computation_buffer[j];
	}
	TDS_voltage = (double)TDS_voltage/j;
	TDS_voltage = TDS_voltage*(double)0.000805664; 																					 // read the analog value more stable by averaging and convert to voltage value
	compensationCoefficient=1.0+0.02*(water_temp-25.0);   																								    		 //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
	compensationVolatge=TDS_voltage/compensationCoefficient;  																									 //temperature compensation
	TDS =(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds
}

char micro_reset = 'y';
int pH_val_buf[55] = {0};
void get_DMA_nutrient_ph_value()	// gets nutrient and ph values from the ADC DMA buffer and place it in a buffer
{
	valid_value_pH = 0;
	valid_value_TDS = 0;
	DMA_pH_sample_avg = 0;
	taskENTER_CRITICAL();
	{
		for(int j = 0; j<79; j++ )
		{
			if(j%2 == 0)//&& nutrient_ph_values[j] < 80)
			{
				TDS_computation_buffer[valid_value_TDS] = nutrient_ph_values[j];
				valid_value_TDS++;
			}
			else if(j%2 != 0 && nutrient_ph_values[j] != 0)
			{
				pH_val_buf[valid_value_pH] = nutrient_ph_values[j];
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

void convertPH()
{
	mili_voltage = ((DMA_pH_sample_avg/4096.0)*3.3)*1000;
	pH = convert_ph(mili_voltage);
}


int runs_after = 0;
double previous_temp = 0;
char get_init_temp = 'n';
char getSensorValues()
{


	readWaterTemp();
	if(get_init_temp == 'n')
	{
		previous_temp = water_temp;
		get_init_temp = 'y';
	}
/*	while(micro_reset == 'y') // if our first run check to see if the pH sensor Erroneous value if so
	{
		if(pH > 14 || runs_after < 3)
		{
			get_nutrient_ph_value(); // sample sensors before taking data. This is because the first run will sometimes give erroneous values
			osDelay(1000);
			readWaterTDS();
			readPH();
			if(pH <= 14 ) runs_after++;
		}
		else micro_reset ='n';
	}*/

	for(int checkSamples = 0; checkSamples<num_sensor_samples+3; checkSamples++)			   // sample TDS and PH every half second for 30 times
	{
		//osDelay(100);
		get_DMA_nutrient_ph_value();
				//osDelay(10);
		convertWaterTDS();
				//osDelay(10);
		convertPH();

		if(checkSamples >= 3) // Sometimes the first 3 samples are invalad so lets just skip them
		{
			 sample_array_TDS[checkSamples-3] = TDS;
			 sample_array_pH[checkSamples-3] = pH;
			 osDelay(10);
		}
		else  osDelay(100);
		TDS = 0;
		pH = 0;
	}
//	TDS = 0;
//	pH = 0;

	for(int get_average = 0; get_average<num_sensor_samples; get_average++)			   // sample TDS and PH every half second for 30 times
	{
		TDS += sample_array_TDS[get_average];
		pH += sample_array_pH[get_average];
	}
	TDS = TDS/num_sensor_samples;
	pH  = pH/num_sensor_samples;
	runs_after = 0;


	if(water_temp > 100 || previous_temp-water_temp > 3 || water_temp-previous_temp > 3) return 'n'; // handle invalid temp reading which happen from time to time

	previous_temp = water_temp;
	waterTempControl();
	return 'y';
}


