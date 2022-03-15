/*
 * nutrient_pH_driver.c
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */
#include "nutrient_pH_driver.h"
#include "main.h"
#include "hydro_control_loop.h"
double pH_up_volume = 0;
double pH_down_volume = 0;
double nutrient_volume = 0;
void doseWater(double acid_ml, double base_ml, double nutrient_ml)
{
	int ph_down_steps = calc_dose_steps(acid_ml);
	int ph_up_steps = calc_dose_steps(base_ml);
	int nutrient_steps = calc_dose_steps(nutrient_ml);
	step(nutrient_steps,ph_up_steps, ph_down_steps);
}
//max_pH_up_dose = 1.0, max_pH_down_dose = 1.0, max_nutrient_dose = 50
double d  = 0;
double pH_up_dose_factor = 1000, pH_down_dose_factor = 1000, nutrient_dose_factor = 1000;
double calcPhUpDose(double pH_set_val, double TDS_set_val)      // following dose methods return a double in milliliters.
{
	pH_up_volume =  0; // set dose to the minimum
	for(d = 0; (d < pH_set_val-pH) && (pH_up_volume < max_pH_up_dose); d += 0.01) 		 pH_up_volume += (double) max_pH_up_dose/pH_up_dose_factor;
	return pH_up_volume;
}

double calcPhDownDose(double pH_set_val, double TDS_set_val)
{
	pH_down_volume =  0;
	for(d = 0; (d < pH-pH_set_val) && (pH_down_volume < max_pH_down_dose); d += 0.01) 	 pH_down_volume += (double) max_pH_down_dose/pH_down_dose_factor;
	return pH_down_volume;
}

double calcNutrientDose(double pH_set_val, double TDS_set_val)	// calculates nutrient dosage based on how far we are away from the set-point
{
	nutrient_volume =  0;
	for(int i = 0; (i < TDS_set_val - TDS) && (nutrient_volume < max_nutrient_dose); i++) nutrient_volume += (double) max_nutrient_dose/nutrient_dose_factor;
	return nutrient_volume;
}

double Start_pH = 0;
double Start_Nutrient = 0;
void calibrateDosage(char what_to_cal) // goal: Calculate the pH/nutrient change given a set volume. This will allow us to reduce over/undershooting without PID
{
	Start_pH = pH;
	Start_Nutrient = TDS;
	if(what_to_cal == 'u')	   doseWater(0,0.1,0);	// 'u' = pH up, 'd' = pH down, 'u' = nutrient up,
	else if(what_to_cal == 'd')doseWater(0.1,0,0);
	else if(what_to_cal == 'n')doseWater(0,0,3);
	waitForWaterToStabilize();
	if(what_to_cal == 'u')      pH_up_dose_factor    = (pH-Start_pH)*10000;
	else if(what_to_cal == 'd') pH_down_dose_factor  = (Start_pH-pH)*11000;
	else if(what_to_cal == 'n') nutrient_dose_factor = (TDS-Start_Nutrient)*50;
}
