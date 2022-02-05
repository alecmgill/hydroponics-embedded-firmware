/*
 * fan_driver.c
 *
 *  Created on: Feb 5, 2022
 *      Author: Alec
 */
#include "fan_driver.h"


float system_fan_sp = 3.5;
float plant_fan_sp = 3.5;
float heat_cool_fan_sp = 3.5; // set default fan speed values
void fanOn(TIM_HandleTypeDef sys,TIM_HandleTypeDef plant,TIM_HandleTypeDef heat_cool)
{

   HAL_TIM_PWM_Start(&heat_cool, TIM_CHANNEL_1);	// start heater cooler fan control
   TIM1->ARR = 2879;								// sets the PWM frequency of 25Mhz
   TIM1->CCR1 = (TIM1->ARR)/heat_cool_fan_sp;			// sets the fan speed
   TIM1->CNT = 20500;

   HAL_TIM_PWM_Start(&sys, TIM_CHANNEL_1);			// start system fan control
   TIM3->ARR = 1439;								// sets the PWM frequency of 25Mhz
   TIM3->CCR1 = (TIM3->ARR)/system_fan_sp;				// sets the fan speed
   TIM3->CNT = 20500;
   TIM1->PSC = 0;

   HAL_TIM_PWM_Start(&plant, TIM_CHANNEL_1);		// start plant fan control
   TIM3->ARR = 1439;								// sets the PWM frequency of 25Mhz
   TIM3->CCR1 = (TIM3->ARR)/plant_fan_sp;				// sets the fan speed
   TIM3->CNT = 20500;
   TIM1->PSC = 0;
}

void fanOff(TIM_HandleTypeDef sys,TIM_HandleTypeDef plant,TIM_HandleTypeDef heat_cool)
{

   HAL_TIM_PWM_Start(&heat_cool, TIM_CHANNEL_1);// stop heat_cool fan control
   TIM1->ARR = 2879;							// sets the PWM frequency of 25Mhz
   TIM1->CCR1 = 0;								// sets the fan speed
   TIM1->CNT = 20500;

   HAL_TIM_PWM_Start(&sys, TIM_CHANNEL_1);		// stop system fan control
   TIM3->ARR = 1439;							// sets the PWM frequency of 25Mhz
   TIM3->CCR1 = 0;								// sets the fan speed
   TIM3->CNT = 20500;
   TIM1->PSC = 0;

   HAL_TIM_PWM_Start(&plant, TIM_CHANNEL_1);	// stop plant fan control
   TIM3->ARR = 1439;							// sets the PWM frequency of 25Mhz
   TIM3->CCR1 = 0;								// sets the fan speed
   TIM3->CNT = 20500;
   TIM1->PSC = 0;
}

void setFanSpeed(TIM_HandleTypeDef sys,TIM_HandleTypeDef plant,TIM_HandleTypeDef heat_cool, float sys_fan,float plant_fan,float heat_cool_fan)
{
	system_fan_sp = sys_fan;
	plant_fan_sp = plant_fan;
	heat_cool_fan_sp = heat_cool_fan;
	fanOn(sys,plant,heat_cool);
}
