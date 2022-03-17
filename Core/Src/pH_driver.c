/*
 * pH_driver.c
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#include "main.h"
#include "pH_driver.h"
#include "sensors.h"
#include "cmsis_os.h"
double mili_Voltage_Buf = 0;

double ph_voltage_average = 0;

double value;
double ph_voltage_buffer[30] = {0};
double pH_low_cal = 3047.0, pH_mid_cal = 2939.0, pH_high_cal= 2852.0, low_ph_solution = 4.0, mid_ph_solution = 6.86, High_ph_solution = 9.18;
double slope = 0, ph_Value_Buf = 0;

double convert_ph(float voltage_mV)	// converts voltage to pH value based on three point calibration
{
	if(voltage_mV>pH_mid_cal)
	{
		slope = (mid_ph_solution-low_ph_solution)/(pH_mid_cal-pH_low_cal);	// if the solution is Acidic calculate the slope of the (calibration_ph_Value_Buf vs calibration_milimvoltage) line  bases on calibration parameters
		return (slope*(voltage_mV - pH_low_cal)+low_ph_solution);
	}
	else if(voltage_mV<=pH_mid_cal)
	{
		slope = (High_ph_solution-mid_ph_solution)/(pH_high_cal-pH_mid_cal);// if the solution is Basic calculate the slope bases just like we did above but for the higher range
		return (slope*(voltage_mV - pH_mid_cal)+mid_ph_solution);
	}
	return 0;
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


