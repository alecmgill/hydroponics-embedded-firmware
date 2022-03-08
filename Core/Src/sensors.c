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
char done_sampling = 'y'; // global variable: tells the tasks that the sensor averaging is complete
char new_sample = 'n';	  // global variable tells the water temp task when there is a new sample is ready
uint8_t Temp_byte1, Temp_byte2,Presence = 0;
uint16_t TEMP;
double Temperature = 0;
double Humidity = 0;

double readWaterTemp(void)
{

	taskENTER_CRITICAL();
	{

	 DWT_Delay_Init();

	 //Set_Pin_Output(water_temp_GPIO_Port, water_temp_Pin);   // set the pin as output
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
	 Temperature = (double)TEMP/16;
	}
	taskEXIT_CRITICAL();

	 return Temperature;
}




double DMA_TDS_sample_avg = 0;
double DMA_pH_sample_avg = 0;

#define samples  30           	// sum of sample point
int nutrientBuffer[samples],analogBufferTemp[samples], copyIndex = 0;  // store the analog value in the array, read from ADC
double averageVoltage = 0,tdsValue = 0,temperature = 25,compensationCoefficient = 0,compensationVolatge = 0;
int TDS_index = 0;
char nutrient_buffer_ready = 'n';

double readWaterTDS() // Get nutrient level
{
	averageVoltage = DMA_TDS_sample_avg * (double)0.000805664; 																					 // read the analog value more stable by averaging and convert to voltage value
	compensationCoefficient=1.0+0.02*(temperature-25.0);   																								    		 //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
	compensationVolatge=averageVoltage/compensationCoefficient;  																									 //temperature compensation
	tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds
	return tdsValue;
	return -10; // else if we make it here there is no new sample ready so return -10 to signify we need to check again

}
double whtvalue  = 0;
void get_nutrient_ph_value()	// gets nutrient and ph values from the ADC and places it in a buffer
{
	whtvalue = 0;
	DMA_TDS_sample_avg = 0;
	DMA_pH_sample_avg = 0;
	taskENTER_CRITICAL();
	{
		for(int j = 0; j<30; )
		{
			if(j%2 == 0 && nutrient_ph_values[j] < 80)
			{
				DMA_TDS_sample_avg += nutrient_ph_values[j]; // even so get pH
			}
			else if(j%2 == 0)
			{
				whtvalue = nutrient_ph_values[j];
			}
			if(j%2 != 0)
			{
				DMA_pH_sample_avg += nutrient_ph_values[j];
			}
			j++;
		}
	}
	taskEXIT_CRITICAL();

	DMA_pH_sample_avg = DMA_pH_sample_avg/15;
	DMA_TDS_sample_avg =  DMA_TDS_sample_avg/15;

	//nutrient_ph_values[0] = get_ADC_values(1);
//	nutrient_ph_values[1] = get_ADC_values(0);

}

int readWaterLevel() // 1=good 0=low
{
	return HAL_GPIO_ReadPin (water_level_GPIO_Port, water_level_Pin);
}
void calibratePhProbe()
{

}
double mili_voltage,ph_value = 0;
double readPH()
{

	mili_voltage = ((DMA_pH_sample_avg/4096.0)*3.3)*1000;
	ph_value = convert_ph(mili_voltage);
	return ph_value;
}

double TDS_check = 0, pH_check = 0;
int fiveSamples = 0;

void getSensorValues(int delay_enable)
{


	//osThreadSuspend(WaterTempControHandle);

	//done_sampling = 'n';
	get_nutrient_ph_value();


	TDS = readWaterTDS();
	pH =  readPH();


	water_temp = readWaterTemp();
	new_sample = 'y';
/*
	for(fiveSamples = 0; fiveSamples<5;)
	{
		//pH_check 	=  readPH();
		//TDS_check	=  readWaterTDS();
		if(retrieved_ADC_values == 'y')// && TDS_check > 0)// && pH_check != -1 )
		{
			get_nutrient_ph_value();

			TDS 		  += readWaterTDS();	// get sensor data
			pH 		 	  += readPH();
			water_temp	  += readWaterTemp();
			if(delay_enable == 1) osDelay(100);
			fiveSamples++;
			retrieved_ADC_values = 'n';
		}
	}
	if(fiveSamples >= 5)
	{
		water_temp	  = water_temp/5;							    // get sensor data so we can tell if its time to do pH up or pH down
		TDS 		  = TDS/5;
		pH 		 	  = pH/5;
	}*/

	 //osDelay(1000);
	//new_sample = 'y';
	//osThreadResume(WaterTempControHandle);
}


