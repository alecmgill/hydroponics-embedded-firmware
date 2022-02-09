/*
 * pH_driver.c
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#include "main.h"
#include "pH_driver.h"

float mili_Voltage,ph_Value = 0;
int phBuffer[30];
int phAverage = 0;
int j = 0;
float value;

float voltage_mV, volt_avg_len = 30, pH_low_cal = 2016.5, pH_mid_cal = 1527.5, pH_high_cal= 1115.1;

float read_voltage()
{
	phAverage = 0;
	ph_Value = 0;
	for(int i = 0; i<32; i++)
	{
		get_nutrient_ph_value();
		value = nutrient_ph_values[0];
		if(i>1) phBuffer[i-2] = value;   // skip first 2 samples for accuracy
		HAL_Delay(1);
	}
	for(j=0;j<30;j++) ph_Value = phBuffer[j] + ph_Value;
	phAverage = ph_Value/30;
	mili_Voltage = ((phAverage/4096.0)*3.3)*1000;
	return mili_Voltage;
}

float convert_ph(float voltage_mV)	// converts voltage to pH value based on three point calibration
{
	if(voltage_mV>pH_mid_cal) 		// if the solution is basic
	{
		float slope = (7.0-4.0)/((pH_mid_cal-1500.0)/3.0 - (pH_low_cal-1500.0)/3.0);
		float intercept =  7.0 - slope*(pH_mid_cal-1500.0)/3.0;
		float phValue = slope*(voltage_mV-1500.0)/3.0+intercept;  //y = k*x + b
		return phValue;
	}
	if(voltage_mV<pH_mid_cal) 		// if the solution is acidic
	{
		float slope = (7.0-4.0)/((pH_mid_cal-1500.0)/3.0 - (pH_low_cal-1500.0)/3.0);
		float intercept =  7.0 - slope*(pH_mid_cal-1500.0)/3.0;
		float phValue = slope*(voltage_mV-1500.0)/3.0+intercept;  //y = k*x + b
		return phValue;
	}
}


void cal_mid(float voltage_mV)
{
  pH_mid_cal = voltage_mV;
}

void cal_low(float voltage_mV)
{
  pH_low_cal = voltage_mV;
}

void cal_high(float voltage_mV)
{
  pH_high_cal = voltage_mV;
}

void cal_clear() {
  pH_mid_cal = 1500;
  pH_low_cal = 2030;
  pH_high_cal = 975;
}


