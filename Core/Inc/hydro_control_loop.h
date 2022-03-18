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

extern double water_temp, TDS, pH;
extern double max_pH_up_dose, max_pH_down_dose, max_nutrient_dose, pH_set_point, nutrient_set_point, sample_array_TDS[30], sample_array_pH[30];
extern int num_sensor_samples;
void floatToString(double FP_NUM);
void write_balance_data_file(char buffer_full);
void add_data_to_array();
void appendInitialConditions();
void balancePhAndNutrient();
void resetStabilityVars();
void isStabalized();
int waitForWaterToStabilize();
void waterTempControl();
void systemControl();

#endif /* INC_HYDRO_CONTROL_LOOP_H_ */
