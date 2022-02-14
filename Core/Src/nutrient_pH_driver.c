/*
 * nutrient_pH_driver.c
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */
#include "nutrient_pH_driver.h"
#include "main.h"



void doseWater(float acid_ml, float base_ml, float nutrient_ml)
{
	int ph_down_steps = calc_dose_steps(acid_ml);
	int ph_up_steps = calc_dose_steps(base_ml);
	int nutrient_steps = calc_dose_steps(nutrient_ml);
	step(nutrient_steps,ph_up_steps, ph_down_steps);
}

double calcPhUpDose(float TDS, float pH)      // following dose methods return a double in milliliters.
{
	//if(pH > (pH_set_point+0.8))
	return 2.0;
}

double calcPhDownDose(float TDS, float pH)
{
	return 0;
}

double calcNutrientDose(float TDS, float pH)
{
	return 0;
}
