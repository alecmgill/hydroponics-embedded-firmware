/*
 * fan_driver.c
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */
#include "fan_driver.h"


double system_fan_sp = 5.5;
double plant_fan_sp = 3.5;
double heat_cool_fan_sp = 2.5; // set default fan speed values
void fanOn()
{
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);	// start heater cooler fan control
   TIM1->ARR = 2879;							// sets the PWM frequency of 25Mhz
   if(heat_cool_fan_sp == 0)  TIM1->CCR1  = 0;
   else TIM1->CCR1 = (TIM1->ARR)/heat_cool_fan_sp;	// sets the fan speed
   TIM1->CNT = 20500;

   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);		// start system fan control
   TIM1->ARR = 2879;							// sets the PWM frequency of 25Mhz
   if(system_fan_sp == 0) TIM1->CCR1  = 0;
   else TIM1->CCR3 = (TIM1->ARR)/system_fan_sp;		// sets the fan speed
   TIM1->CNT = 20500;

   HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);	// start plant fan control
   TIM10->ARR = 2879;							// sets the PWM frequency of 25Mhz
   if(plant_fan_sp == 0) TIM10->ARR = 0;
   else TIM10->CCR1 = (TIM10->ARR)/plant_fan_sp;		// sets the fan speed
   TIM10->CNT = 20500;
}

void fanOff()
{
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);	// stop heat_cool fan control
   TIM1->ARR = 2879;							// sets the PWM frequency of 25Mhz
   TIM1->CCR1 = 0;								// sets the fan speed
   TIM1->CNT = 20500;

   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);		// stop system fan control
   TIM1->ARR = 2879;							// sets the PWM frequency of 25Mhz
   TIM1->CCR3 = 0;								// sets the fan speed
   TIM1->CNT = 20500;

   HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);	// stop plant fan control
   TIM10->ARR = 2879;							// sets the PWM frequency of 25Mhz
   TIM10->CCR1 = 0;								// sets the fan speed
   TIM10->CNT = 20500;

}
void setFanSpeed(float sys_fan,float plant_fan,float heat_cool_fan)
{
	system_fan_sp = sys_fan;
	plant_fan_sp = plant_fan;
	heat_cool_fan_sp = heat_cool_fan;
	fanOn();
}
