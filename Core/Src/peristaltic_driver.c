/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : peristaltic_driver.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include "peristaltic_driver.h"
#include "main.h"
//extern TIM_HandleTypeDef step_timer;

void delay_Stepper(uint16_t us)
{
		__HAL_TIM_SET_COUNTER(&htim2,0);  			 // set the counter value a 0
		while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // wait for the counter to reach the us input in the parameter
}
double steps = 0;
int calc_dose_steps(double miliLiters)	// takes a dose in mls and returns the number of steps for that volume
{
	if	   (miliLiters > 0 && miliLiters <= 1)	steps = (1/0.221)*(miliLiters+0.2012)*(1745);
	else if(miliLiters > 1 && miliLiters <= 2)	steps = (1/0.221)*(miliLiters+0.2012)*(1790);
	else if(miliLiters > 2 && miliLiters <= 3)  steps = (1/0.221)*(miliLiters+0.2012)*(1980);
	else if(miliLiters > 3 && miliLiters <= 4)  steps = (1/0.221)*(miliLiters+0.2012)*(1980);
	else if(miliLiters > 4 && miliLiters <= 5)  steps = (1/0.221)*(miliLiters+0.2012)*(1980);
	else if(miliLiters > 5 && miliLiters <= 30) steps = (1/0.221)*(miliLiters+0.2012)*(1980);
	else if(miliLiters == 0) steps = 0;
	else steps = (1/0.221)*(miliLiters+0.2012)*(1915);
	return (int)steps;
}

void disablePumps()	// disable all pumps
{
	HAL_GPIO_WritePin(GPIOE,nutrient_enable_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE,ph_down_enable_Pin,GPIO_PIN_SET);
 	HAL_GPIO_WritePin(GPIOE,ph_up_enable_Pin,GPIO_PIN_SET);

}

void enablePumps(int nutrient_steps, int ph_up_steps, int ph_down_steps)	// enable only the pumps that are going to dose
{
	if(nutrient_steps>0)HAL_GPIO_WritePin(GPIOE,nutrient_enable_Pin,GPIO_PIN_RESET);		// enable nutrient pump
	if(ph_down_steps>0)	HAL_GPIO_WritePin(GPIOE,ph_down_enable_Pin,GPIO_PIN_RESET);			// enable ph down pump
	if(ph_up_steps>0)	HAL_GPIO_WritePin(GPIOE,ph_up_enable_Pin,GPIO_PIN_RESET);			// enable ph up pump
}

void step(int nutrient_steps, int ph_up_steps, int ph_down_steps)
{
	int most_steps = nutrient_steps;										// find the largest dose in steps. Will be used in step loop
	if(ph_up_steps>most_steps) most_steps 	= ph_up_steps;
	if(ph_down_steps>most_steps) most_steps = ph_down_steps;
	enablePumps(nutrient_steps,ph_up_steps,ph_down_steps);					// enable only the pumps that are going to dose
	//HAL_TIM_Base_Start(&htim2);
	for(int i = 0; i<most_steps; i++)										// step each pump their respective number of steps.
	{
		if(i<nutrient_steps) HAL_GPIO_TogglePin(GPIOE, nutrient_pump_Pin);
		if(i<ph_up_steps)	 HAL_GPIO_TogglePin(GPIOE, ph_up_pump_Pin);
		if(i<ph_down_steps)	 HAL_GPIO_TogglePin(GPIOE, ph_down_pump_Pin);
		 osDelay(40);
		if(i<nutrient_steps) HAL_GPIO_TogglePin(GPIOE, nutrient_pump_Pin);
		if(i<ph_up_steps)	 HAL_GPIO_TogglePin(GPIOE, ph_up_pump_Pin);
		if(i<ph_down_steps)	 HAL_GPIO_TogglePin(GPIOE, ph_down_pump_Pin);
		 osDelay(40);
	}
	disablePumps();
}
