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

void readWaterTemp(void);
void readWaterTDS();
void get_nutrient_ph_value();
int  readWaterLevel();
void calibratePhProbe();
void readPH();
void getFiveSamples(int delay_enable);

#endif /* INC_SENSORS_H_ */
