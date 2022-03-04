/*
 * nutrient_pH_driver.h
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */

#ifndef INC_NUTRIENT_PH_DRIVER_H_
#define INC_NUTRIENT_PH_DRIVER_H_

#include "stm32f2xx_hal.h"
#include "peristaltic_driver.h"
void doseWater(float acid_ml, float base_ml, float nutrient_ml);
double calcPhUpDose(double pH_set_val, double TDS_set_val);      // following dose methods return a double in milliliters.
double calcPhDownDose(double pH_set_val, double TDS_set_val);
double calcNutrientDose(double pH_set_val, double TDS_set_val);

#endif /* INC_NUTRIENT_PH_DRIVER_H_ */
