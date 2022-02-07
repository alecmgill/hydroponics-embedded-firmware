/*
 * heater_driver.c
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#include "stm32f2xx_hal.h"
#include "main.h"
#include "sensors.h"
#include "heater_driver.h"
void heatOn()
{
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_Pin,GPIO_PIN_SET);			// set to heat
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_enable_Pin,GPIO_PIN_SET);	// enable the heater
}
void heatOff()
{
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_enable_Pin,GPIO_PIN_RESET);	// disable the heater
}
void coolOn()
{
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_Pin,GPIO_PIN_RESET);		// set to cool
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_enable_Pin,GPIO_PIN_SET);	// enable the cooler
}
void coolOff()
{
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_enable_Pin,GPIO_PIN_RESET);	// disable the cooler
}
