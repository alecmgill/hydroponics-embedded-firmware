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
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_Pin,GPIO_PIN_RESET);			// set to heat
	osDelay(200);
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_enable_Pin,GPIO_PIN_RESET);	// enable the heater
}
void heatCoolOff()
{
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_enable_Pin,GPIO_PIN_SET);	// disable the heater
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_Pin,GPIO_PIN_SET);			// turn off active relays so not to waste power... This also sets the heater/cooler to the cool mode however, since the heater power-supply is disabled no cooling will take place
}
void coolOn()
{
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_Pin,GPIO_PIN_SET);		// set to cool
	osDelay(200);
	HAL_GPIO_WritePin(GPIOE,water_heat_cool_enable_Pin,GPIO_PIN_RESET);	// enable the cooler
}
