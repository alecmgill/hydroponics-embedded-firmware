/*
 * ADC_driver.c
 *
 *  Created on: Feb 6, 2022
 *      Author: Alec
 */
#include "main.h"
#include "ADC_driver.h"
#include "sensors.h"

float get_ADC_values(int pH_or_nutrients)
{
		HAL_ADC_Start(&hadc2);
		HAL_ADC_PollForConversion(&hadc2, 100);
		return HAL_ADC_GetValue(&hadc2);//Read value
}
