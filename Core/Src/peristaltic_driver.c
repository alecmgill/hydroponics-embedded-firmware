/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Stepper.c
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

void delay_Stepper(uint16_t us,TIM_HandleTypeDef h)
{
		__HAL_TIM_SET_COUNTER(&h,0);  			 // set the counter value a 0
		while (__HAL_TIM_GET_COUNTER(&h) < us);  // wait for the counter to reach the us input in the parameter
}

void Step(int stepNumber, int Speed, TIM_HandleTypeDef h)
{

		for(stepNumber>0; stepNumber--;)
		{
			HAL_GPIO_TogglePin(GPIOE, ph_up_pump_Pin);
			delay_Stepper(Speed,h);
			HAL_GPIO_TogglePin(GPIOE, ph_up_pump_Pin);
			delay_Stepper(Speed,h);
		}
}


int Dose(double miliLiters)
{
	double steps = 0;
	if	   (miliLiters > 0 && miliLiters <= 1)	steps = (1/0.221)*(miliLiters+0.2012)*(1700);
	else if(miliLiters > 1 && miliLiters <= 2)	steps = (1/0.221)*(miliLiters+0.2012)*(1740);
	else if(miliLiters > 2 && miliLiters <= 3)  steps = (1/0.221)*(miliLiters+0.2012)*(1793);
	else if(miliLiters > 3 && miliLiters <= 4)  steps = (1/0.221)*(miliLiters+0.2012)*(1793);
	else if(miliLiters > 4 && miliLiters <= 5)  steps = (1/0.221)*(miliLiters+0.2012)*(1893);
	else if(miliLiters > 5 && miliLiters <= 30) steps = (1/0.221)*(miliLiters+0.2012)*(1900);
	else steps = (1/0.221)*(miliLiters+0.2012)*(1915);
	return (int)steps;
}
void disablePump()
{
	 HAL_GPIO_WritePin(GPIOE, ph_up_enable_Pin, GPIO_PIN_SET); // Disable the stepper motor
}
void enablePump()
{
	HAL_GPIO_WritePin(GPIOE, ph_up_enable_Pin, GPIO_PIN_RESET);		// Enable the stepper motor
}
