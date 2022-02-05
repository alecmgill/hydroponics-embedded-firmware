/*
 * nutrient_pH_driver.h
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */

#ifndef INC_NUTRIENT_PH_DRIVER_H_
#define INC_NUTRIENT_PH_DRIVER_H_

#include "stm32f2xx_hal.h"

void doseWater(double acid_ml, double base_ml, double nutrient_ml);
double calcPhUpDose();      // following dose methods return a double in milliliters.
double calcPhDownDose();
double calcNutrientDose();

#endif /* INC_NUTRIENT_PH_DRIVER_H_ */
