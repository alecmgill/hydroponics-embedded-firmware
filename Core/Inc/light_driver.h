/*
 * light_driver.h
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#ifndef INC_LIGHT_DRIVER_H_
#define INC_LIGHT_DRIVER_H_
#include "stm32f2xx_hal.h"
#include "main.h"
#include "sensors.h"
#include "RTC_driver.h"
extern int day_or_night;

void lightOn();
void lightOff();
void setLightCyle(uint8_t start_hour, uint8_t start_min, uint8_t end_hours, uint8_t end_min);
void checkLightCycle();

#endif /* INC_LIGHT_DRIVER_H_ */
