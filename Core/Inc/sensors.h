/*
 * sensors.h
 *
 *  Created on: Feb 6, 2022
 *      Author: Alec
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_
#include "stm32f2xx_hal.h"
#include "main.h"
#include "sensors.h"

double readWaterTemp(void);


float readWaterTDS();

#endif /* INC_SENSORS_H_ */
