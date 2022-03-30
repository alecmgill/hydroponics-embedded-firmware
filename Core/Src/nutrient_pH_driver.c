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
char cont_cal_additions_PH_up = 'n',cont_cal_additions_PH_down = 'n',cont_cal_additions_nutrient = 'n';
//int cont_additions = 0; // 0: (first cal run) we need to figure out what cal dosage we need based on the TDS level. 2: (fine tuning the cal dosage) add 0.1ml to pH dose and see if that changed the waters pH enough to calaculate a dose. 3: signifies we are/need to dosing water via calc methods above)

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
	//cont_additions = 0;
	pH_up_volume = (pH_up_ml_per_val_change)*(pH_set_point-pH);
	return pH_up_volume;
}

double calcPhDownDose(double pH_set_val, double TDS_set_val)
{
//cont_additions = 0;
	pH_down_volume = (pH_down_ml_per_val_change)*(pH-pH_set_point);
	return pH_down_volume;
}

double calcNutrientDose(double pH_set_val, double TDS_set_val)	// calculates nutrient dosage based on how far we are away from the set-point
{
	//cont_additions = 0;
	nutrient_volume = (nutrient_ml_per_val_change)*(nutrient_set_point - TDS);
	return nutrient_volume;
}

double Start_pH = 0;
double Start_Nutrient = 0;
double pH_up_cal_dosage = 0.05, pH_down_cal_dosage = 0.05, nutrient_up_cal_dosage = 10;
char calibrating = 'n';

char calibrateDosage(char what_to_cal,double Cal_pH, double p_bounds_set, double n_bounds_set)	// goal: Calculate the pH/nutrient change given a set volume. This will allow us to reduce over/undershooting without PID
{
//	getSensorValues();

	Start_pH = Cal_pH;
	Start_Nutrient = TDS;

	// NEED TO ACCOUNT FOR what_to_cal
	if(TDS > 300 && TDS <= 500)// && calibrating == 'y')
	{
		if(		what_to_cal == 'd' && cont_cal_additions_PH_down == 'n') pH_down_cal_dosage = 0.2;
		else if(what_to_cal == 'u' && cont_cal_additions_PH_up == 'n')   pH_up_cal_dosage = 0.2;
	}
	else if(TDS > 500 && TDS < 700) // && calibrating != 'y')
	{
		if(what_to_cal == 'd' && cont_cal_additions_PH_down == 'n') pH_down_cal_dosage = 0.4;
		else if(what_to_cal == 'u' && cont_cal_additions_PH_up == 'n') pH_up_cal_dosage = 0.4;
	}
	else if(TDS >= 700)//&& calibrating != 'y')
	{
		if(what_to_cal == 'd' && cont_cal_additions_PH_down == 'n')  pH_down_cal_dosage = 0.5;
		else if(what_to_cal == 'u' && cont_cal_additions_PH_up == 'n') pH_up_cal_dosage = 0.5;
	}
	else  //calibrating != 'n')
	{
		if(what_to_cal == 'd' &&  cont_cal_additions_PH_down == 'n') 	 pH_down_cal_dosage = 0.1;
		else if(what_to_cal == 'u' && cont_cal_additions_PH_up == 'n' )  pH_up_cal_dosage = 0.1;
		else if(what_to_cal == 'n' && cont_cal_additions_nutrient == 'n')nutrient_up_cal_dosage = 5;
	}
	if(what_to_cal == 'u')
	{
		doseWater(0,pH_up_cal_dosage,0);// if we are close to our pH set point dose with a very small amount to not over-dose the water.
		total_pH_up_ml_cal_step += pH_up_cal_dosage;
	}
	else if(what_to_cal == 'd')
	{
		doseWater(pH_down_cal_dosage,0,0);// if we are close to our pH set point dose with a very small amount to not over-dose the water.
		total_pH_down_ml_cal_step += pH_down_cal_dosage;
	}
	else if(what_to_cal == 'n')
	{
		doseWater(0,0,nutrient_up_cal_dosage);
		total_nutrient_ml_cal_step += nutrient_up_cal_dosage;
	}
	waitForWaterToStabilize();

	if(what_to_cal == 'u' && (pH+p_bounds_set) > pH_set_point)			   return 'n';
	else if(what_to_cal == 'd' && (pH-p_bounds_set) < pH_set_point) 	   return 'n';
	else if(what_to_cal == 'n' && (TDS+n_bounds_set) > nutrient_set_point) return 'n';
	// NEED to make sure we did not reach our required pH or nutrient
	else if(what_to_cal == 'u' && pH-Start_pH <= 0.03)
	{
		cont_cal_additions_PH_up = 'y';
		calibrating = 'y';
		pH_up_cal_dosage += 0.1;								// if we are close to our pH set point dose with a very small amount to not over-dose the water.
		return 'n';
	}
	else if(what_to_cal == 'd' && Start_pH-pH <= 0.03)
	{
		cont_cal_additions_PH_down = 'y';
		//cont_additions = 2;
		calibrating = 'y';
		pH_down_cal_dosage += 0.1;	// if we are close to our pH set point dose with a very small amount to not over-dose the water.
		return 'n';
	}
	else if(what_to_cal == 'n' && TDS-Start_Nutrient < 3 )
	{
		cont_cal_additions_nutrient = 'y';
		//cont_additions = 2;
		calibrating = 'y';
		nutrient_up_cal_dosage += 3;
		return 'n';
	}
	else if(what_to_cal == 'u')
	{
		pH_up_ml_per_val_change  =  (pH_up_cal_dosage/((pH-Start_pH)-0.02)); // constant is used to slightly underestimate the max amount of pH solution we dose so not to overshoot
		cont_cal_additions_PH_up = 'n';
		//pH_up_dose_factor    = 90/(pH-Start_pH);
		calibrating = 'n';
	//	cont_additions = 3;
		return 'y';
	}
	else if(what_to_cal == 'd')
	{
		cont_cal_additions_PH_down = 'n';
		pH_down_ml_per_val_change  = (pH_down_cal_dosage/((Start_pH-pH)-0.02));
		calibrating = 'n';
		//cont_additions = 3;
		return 'y';
	}
	else if(what_to_cal == 'n')
	{
		cont_cal_additions_nutrient = 'n';
		nutrient_ml_per_val_change = (nutrient_up_cal_dosage/((TDS-Start_Nutrient)-1.1));
		calibrating = 'n';
//		cont_additions = 3;
		return 'y';
	}
	return 'n';
}
