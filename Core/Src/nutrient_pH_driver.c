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
void doseWater(float acid_ml, float base_ml, float nutrient_ml)
{
	int ph_down_steps = calc_dose_steps(acid_ml);
	int ph_up_steps = calc_dose_steps(base_ml);
	int nutrient_steps = calc_dose_steps(nutrient_ml);
	step(nutrient_steps,ph_up_steps, ph_down_steps);
}
//max_pH_up_dose = 1.0, max_pH_down_dose = 1.0, max_nutrient_dose = 50
double d  = 0;
double calcPhUpDose(double pH_set_val, double TDS_set_val)      // following dose methods return a double in milliliters.
{
	pH_up_volume =  0; // set dose to the minimum

	for(d = 0; (d < pH_set_val-pH) && (pH_up_volume < max_pH_up_dose) && (TDS < 150); d += 0.01)			     	  pH_up_volume += (double) max_pH_up_dose/1300.0;
	for(d = 0; (d < pH_set_val-pH) && (pH_up_volume < max_pH_up_dose) && (TDS >= 150) && (TDS < 300); d += 0.01) 	  pH_up_volume += (double) max_pH_up_dose/600.0;
	for(d = 0; (d < pH_set_val-pH) && (pH_up_volume < max_pH_up_dose) && (TDS >= 300) && (TDS < 600); d += 0.01) 	  pH_up_volume += (double) max_pH_up_dose/120.0;
	for(d = 0; (d < pH_set_val-pH) && (pH_up_volume < max_pH_up_dose) && (TDS >= 600); d += 0.01) 				 	  pH_up_volume += (double) max_pH_up_dose/50.0;
	return pH_up_volume;
}

double calcPhDownDose(double pH_set_val, double TDS_set_val)
{
	pH_down_volume =  0;

	for(d = 0; (d < pH-pH_set_val) && (pH_down_volume < max_pH_down_dose) && (TDS < 150); d += 0.01)			      pH_down_volume += (double) max_pH_down_dose/1300.0;
	for(d = 0; (d < pH-pH_set_val) && (pH_down_volume < max_pH_down_dose) && (TDS >= 150) && (TDS < 300); d += 0.01)  pH_down_volume += (double) max_pH_down_dose/600.0;
	for(d = 0; (d < pH-pH_set_val) && (pH_down_volume < max_pH_down_dose) && (TDS >= 300) && (TDS < 600); d += 0.01)  pH_down_volume += (double) max_pH_down_dose/120.0;
	for(d = 0; (d < pH-pH_set_val) && (pH_down_volume < max_pH_down_dose) && (TDS >= 600); d += 0.01)		  		  pH_down_volume += (double) max_pH_down_dose/50.0;
	return pH_down_volume;
}

double calcNutrientDose(double pH_set_val, double TDS_set_val)	// calculates nutrient dosage based on how far we are away from the set-point
{
	nutrient_volume =  0;
	if(		(TDS_set_val - TDS)  > 7.5)								 for(int i = 0; (i < TDS_set_val - TDS) && (nutrient_volume < max_nutrient_dose); i++)     nutrient_volume += (double) max_nutrient_dose/40.0;
	else if((TDS_set_val - TDS) <= 7.5 && (TDS_set_val - TDS) > 2.5 )for(int i = 0; (i < TDS_set_val - TDS) && (nutrient_volume < max_nutrient_dose); i++)	  nutrient_volume += (double) max_nutrient_dose/45.0;
	else if((TDS_set_val - TDS) <= 2.5) 						     for(int i = 0; (i < TDS_set_val - TDS) && (nutrient_volume < max_nutrient_dose); i++)	  nutrient_volume += (double) max_nutrient_dose/50.0;

	return nutrient_volume;
}
