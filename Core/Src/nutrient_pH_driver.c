/*
 * nutrient_pH_driver.c
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */
#include "nutrient_pH_driver.h"
#include "main.h"



void doseWater(double acid_ml, double base_ml, double nutrient_ml)
{
	int ph_down_steps = calc_dose_steps(acid_ml);
	int ph_up_steps = calc_dose_steps(base_ml);
	int nutrient_steps = calc_dose_steps(nutrient_ml);
	step(nutrient_steps,ph_up_steps, ph_down_steps);
}
double calcPhUpDose()      // following dose methods return a double in milliliters.
{

}
double calcPhDownDose()
{

}
double calcNutrientDose()
{

}
