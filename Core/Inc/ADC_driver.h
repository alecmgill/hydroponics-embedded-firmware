/*
 * ADC_driver.h
 *
 *  Created on: Feb 6, 2022
 *      Author: Alec
 */

#ifndef INC_ADC_DRIVER_H_
#define INC_ADC_DRIVER_H_

#include "stm32f2xx_hal.h"
#include "main.h"
#include "sensors.h"

float get_ADC_values();

void get_nutrient_ph_value();

#endif /* INC_ADC_DRIVER_H_ */
