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

#include "main.h"
#include "stm32f2xx_hal.h"

void delay_Stepper(uint16_t us,TIM_HandleTypeDef h);
void step(int nutrient_steps, int ph_up_steps, int ph_down_steps, TIM_HandleTypeDef h);
int  calc_dose_steps(double miliLiters);
void disablePump();
void enablePump();
