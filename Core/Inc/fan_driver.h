/*
 * fan_driver.h
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */

#ifndef INC_FAN_DRIVER_H_
#define INC_FAN_DRIVER_H_

#include "main.h"

void fanOn();

void fanOff();

void setFanSpeed(float sys_fan,float plant_fan,float heat_cool_fan);

#endif /* INC_FAN_DRIVER_H_ */
