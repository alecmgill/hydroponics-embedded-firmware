/*
 * heater_driver.h
 *
 *  Created on: Feb 7, 2022
 *      Author: Alec
 */

#ifndef INC_HEATER_DRIVER_H_
#define INC_HEATER_DRIVER_H_
#include "stm32f2xx_hal.h"
#include "main.h"
#include "sensors.h"
void heatOn();
void heatCoolOff();
void coolOn();



#endif /* INC_HEATER_DRIVER_H_ */
