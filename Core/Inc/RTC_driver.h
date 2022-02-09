/*
 * RTC_driver.h
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#ifndef INC_RTC_DRIVER_H_
#define INC_RTC_DRIVER_H_
#include "stm32f2xx_hal.h"
#include "light_driver.h"
#include "main.h"

void getTime(void);
void setTimeDate(uint8_t month, uint8_t day, uint8_t year, uint8_t hours, uint8_t min, uint8_t sec);

#endif /* INC_RTC_DRIVER_H_ */
