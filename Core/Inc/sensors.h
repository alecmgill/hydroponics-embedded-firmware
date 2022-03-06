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
extern char done_sampling;  // global variable: tells the tasks that the sensor averaging is complete
char new_sample;			// global variable tells the water temp task when there is a new sample is ready
double readWaterTemp(void);
double readWaterTDS();
void get_nutrient_ph_value();
int readWaterLevel();
void calibratePhProbe();
double readPH();
void getFiveSamples(int delay_enable);

#endif /* INC_SENSORS_H_ */
