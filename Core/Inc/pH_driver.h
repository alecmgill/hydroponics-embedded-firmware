/*
 * pH_driver.h
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#ifndef INC_PH_DRIVER_H_
#define INC_PH_DRIVER_H_
#include "main.h"
#include "stm32f2xx_hal.h"
float read_voltage();

float convert_ph(float voltage_mV);

void cal_mid(float voltage_mV);

void cal_low(float voltage_mV);

void cal_high(float voltage_mV);
void cal_clear();



#endif /* INC_PH_DRIVER_H_ */
