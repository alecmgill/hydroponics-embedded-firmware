/*
 * light_driver.c
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#include "main.h"
#include "sensors.h"
#include "light_driver.h"

RTC_TimeTypeDef light_on_time = {0};
RTC_DateTypeDef light_on_date = {0};
RTC_TimeTypeDef light_off_time = {0};
RTC_DateTypeDef light_off_date = {0};
int day_or_night = 0; // 0=night 1=day

void lightOn()
{
	HAL_GPIO_WritePin(GPIOE,grow_light_Pin,GPIO_PIN_SET);		// turn on grow light
	day_or_night = 1;											// it just turned to daytime so set day status
}

void lightOff()
{
	HAL_GPIO_WritePin(GPIOE,grow_light_Pin,GPIO_PIN_RESET);		// turn off grow light
	day_or_night = 0;											// it just turned to night time so set night status
}

void setLightCyle(uint8_t start_hour, uint8_t start_min, uint8_t end_hours, uint8_t end_min)
{
	light_on_time.Hours = start_hour;
	light_on_time.Minutes = start_min;

	light_off_time.Hours = end_hours;
	light_off_time.Minutes = end_min;
}

void checkLightCycle() // checks to see if its time to turn lights on or off
{
	if((sTime.Hours == light_on_time.Hours) && (sTime.Minutes == light_on_time.Minutes) && day_or_night == 0)        lightOn();
	else if((sTime.Hours == light_off_time.Hours) && (sTime.Minutes == light_off_time.Minutes) && day_or_night == 1) lightOff();
}
