/*
 * RTC_driver.c
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#include "main.h"
#include "sensors.h"
#include "RTC_driver.h"
#include "light_driver.h"


void getTime(void)
{
	HAL_RTC_GetTime(&hrtc, &sTime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate,RTC_FORMAT_BIN);
}

void setTimeDate(uint8_t month, uint8_t day, uint8_t year, uint8_t hours, uint8_t min, uint8_t sec)
{
	sTime.Hours = hours;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)Error_Handler();

	sDate.WeekDay = 0x01;
	sDate.Month = month;
	sDate.Date = day;
	sDate.Year = year;
	if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)Error_Handler();
}


