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
double pH_up_ml_per_val_change = 0, pH_down_ml_per_val_change = 0, nutrient_ml_per_val_change = 0;
double calcPhUpDose(double pH_set_val, double TDS_set_val)      // following dose methods return a double in milliliters.
{
	pH_up_volume = (pH_up_ml_per_val_change)*(pH_set_point-pH);
	return pH_up_volume;
}

double calcPhDownDose(double pH_set_val, double TDS_set_val)
{
	pH_down_volume = (pH_down_ml_per_val_change)*(pH-pH_set_point);
	return pH_down_volume;
}

double calcNutrientDose(double pH_set_val, double TDS_set_val)	// calculates nutrient dosage based on how far we are away from the set-point
{
	nutrient_volume = (nutrient_ml_per_val_change)*(nutrient_set_point - TDS);

	return nutrient_volume;
}

double Start_pH = 0;
double Start_Nutrient = 0;
double pH_up_cal_dosage = 0.1, pH_down_cal_dosage = 0.1, nutrient_up_cal_dosage = 5;
char calibrating = 'n';
char calibrateDosage(char what_to_cal,double Cal_pH, double p_bounds_set, double n_bounds_set)	// goal: Calculate the pH/nutrient change given a set volume. This will allow us to reduce over/undershooting without PID
{
//	getSensorValues();
	Start_pH = Cal_pH;
	Start_Nutrient = TDS;
	if(TDS > 300 && TDS <= 500 && calibrating != 'y')
	{
		pH_down_cal_dosage = 0.2;
		pH_up_cal_dosage = 0.2;

	}
	else if(TDS > 500 && TDS < 700 && calibrating != 'y')
	{
		pH_down_cal_dosage = 0.4;
		pH_up_cal_dosage = 0.4;
	}
	else if(TDS >= 700 && calibrating != 'y')
	{
		pH_down_cal_dosage = 0.5;
		pH_up_cal_dosage = 0.5;
	}
	if(what_to_cal == 'u')		doseWater(0,pH_up_cal_dosage,0);// if we are close to our pH set point dose with a very small amount to not over-dose the water.
	else if(what_to_cal == 'd')	doseWater(pH_down_cal_dosage,0,0);// if we are close to our pH set point dose with a very small amount to not over-dose the water.
	else if(what_to_cal == 'n') doseWater(0,0,nutrient_up_cal_dosage);
	waitForWaterToStabilize();

	if(what_to_cal == 'u' && (pH+p_bounds_set) > pH_set_point)			   return 'n';
	else if(what_to_cal == 'd' && (pH-p_bounds_set) < pH_set_point) 	   return 'n';
	else if(what_to_cal == 'n' && (TDS+n_bounds_set) > nutrient_set_point) return 'n';
	// NEED to make sure we did not reach our required pH or nutrient
	else if(what_to_cal == 'u' && pH-Start_pH <= 0.03)
	{
		calibrating = 'y';
		pH_up_cal_dosage += 0.1;								// if we are close to our pH set point dose with a very small amount to not over-dose the water.
		return 'n';
	}
	else if(what_to_cal == 'd' && Start_pH-pH <= 0.03)
	{
		calibrating = 'y';
		pH_down_cal_dosage += 0.1;	// if we are close to our pH set point dose with a very small amount to not over-dose the water.
		return 'n';
	}
	else if(what_to_cal == 'n' && TDS-Start_Nutrient < 5 )
	{
		calibrating = 'y';
		nutrient_up_cal_dosage += 3;
		return 'n';
	}
	else if(what_to_cal == 'u')
	{
		pH_up_ml_per_val_change  =  (pH_up_cal_dosage/((pH-Start_pH)-0.01)); // constant is used to slightly underestimate the max amount of pH solution we dose so not to overshoot
		//pH_up_dose_factor    = 90/(pH-Start_pH);
		calibrating = 'n';
		return 'y';
	}
	else if(what_to_cal == 'd')
	{
		pH_down_ml_per_val_change  = (pH_down_cal_dosage/((Start_pH-pH)-0.01));
		calibrating = 'n';
		return 'y';
	}
	else if(what_to_cal == 'n')
	{
		nutrient_ml_per_val_change = (nutrient_up_cal_dosage/(Start_Nutrient-TDS));
		calibrating = 'n';
		return 'y';
	}
	return 'n';
}
