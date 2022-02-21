/*
 * hydro_control_loop.c
 *
 *  Created on: Feb 9, 2022
 *      Author: Alec
 */

#include "hydro_control_loop.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "File_Handling.h"
#include "fatfs.h"
#include "usb_host.h"
#include "stm32f2xx_hal.h"
#include "calibrate_system.h"
//double pH_up_dose = 0, pH_down_dose = 0, nutrient_dose = 0;
//float nutrient_set_point = 500.0, pH_set_point = 6.5, water_temp_set_point = 20.0, water_temp = 0, TDS = 0, pH = 0;
float  water_temp = 0, TDS = 0, pH = 0;
int runOnce = 1;


extern void calibrateSys();


void systemControl()
{
	if(runOnce == 1)// && write_times == 2)
	{
		runOnce = 0;
		fanOn();
		setTimeDate(0x01, 0x08, 0x22, 0x19, 0x09, 0x00); // MUST BE HEX BUT NOT CONVERTED i,e,(the 22 day of the month is represented as 0x22 NOT 0x16) (month, day, year, hours, min, sec)
		setLightCyle(19, 9, 19, 10); 			   		 // MUST BE INT (start hour, start min, start sec, end hour, end min)


		/*pH_up_dose 	  = calcPhUpDose(TDS,pH);
		pH_down_dose  = calcPhDownDose(TDS,pH);
		nutrient_dose = calcNutrientDose(TDS,pH);*/




	//	doseWater(2,0,0);//pH_down_dose, pH_up_dose, nutrient_dose);		// step the pump motors (pHDown,pHup,nutrient_dose)

	}
/*water_temp	  = readWaterTemp();			// get sensor data
		TDS 		  = readWaterTDS();
		pH 		 	  = readPH();*/
	//checkLightCycle();						    // check if its time to turn lights on or off
	if(write_times != 2)calibrateSys();




}
