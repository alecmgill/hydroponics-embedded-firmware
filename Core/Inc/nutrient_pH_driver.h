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
double calcPhUpDose(float TDS, float pH);      // following dose methods return a double in milliliters.
double calcPhDownDose(float TDS, float pH);
double calcNutrientDose(float TDS, float pH);

#endif /* INC_NUTRIENT_PH_DRIVER_H_ */
