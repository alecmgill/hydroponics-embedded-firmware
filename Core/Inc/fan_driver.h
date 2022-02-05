/*
 * fan_driver.h
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */

#ifndef INC_FAN_DRIVER_H_
#define INC_FAN_DRIVER_H_
#include "fan_driver.h"
#include "main.h"

void fanOn(TIM_HandleTypeDef sys,TIM_HandleTypeDef plant,TIM_HandleTypeDef heat_cool);

void fanOff(TIM_HandleTypeDef sys,TIM_HandleTypeDef plant,TIM_HandleTypeDef heat_cool);

void setFanSpeed(TIM_HandleTypeDef sys,TIM_HandleTypeDef plant,TIM_HandleTypeDef heat_cool, float sys_fan,float plant_fan,float heat_cool_fan);

#endif /* INC_FAN_DRIVER_H_ */
