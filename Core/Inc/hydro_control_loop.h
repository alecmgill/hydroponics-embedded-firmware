/*
 * hydro_control_loop.h
 *
 *  Created on: Feb 9, 2022
 *      Author: Alec
 */

#ifndef INC_HYDRO_CONTROL_LOOP_H_
#define INC_HYDRO_CONTROL_LOOP_H_
#include "main.h"
#include "nutrient_pH_driver.h"
#include "RTC_driver.h"
#include "fan_driver.h"
#include "pH_driver.h"
#include "TDS_driver.h"
extern float nutrient_set_point, pH_set_point, water_temp_set_point;
void systemControl();

#endif /* INC_HYDRO_CONTROL_LOOP_H_ */
