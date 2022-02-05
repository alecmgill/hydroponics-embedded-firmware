/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ph_up_pump_Pin GPIO_PIN_2
#define ph_up_pump_GPIO_Port GPIOE
#define ph_down_pump_Pin GPIO_PIN_3
#define ph_down_pump_GPIO_Port GPIOE
#define nutrient_pump_Pin GPIO_PIN_4
#define nutrient_pump_GPIO_Port GPIOE
#define ph_up_enable_Pin GPIO_PIN_5
#define ph_up_enable_GPIO_Port GPIOE
#define ph_down_enable_Pin GPIO_PIN_6
#define ph_down_enable_GPIO_Port GPIOE
#define plant_fan_speed_Pin GPIO_PIN_6
#define plant_fan_speed_GPIO_Port GPIOF
#define water_temp_Pin GPIO_PIN_0
#define water_temp_GPIO_Port GPIOA
#define water_ph_Pin GPIO_PIN_4
#define water_ph_GPIO_Port GPIOA
#define system_fan_speed_Pin GPIO_PIN_6
#define system_fan_speed_GPIO_Port GPIOA
#define nutrient_level_Pin GPIO_PIN_5
#define nutrient_level_GPIO_Port GPIOC
#define nutrient_enable_Pin GPIO_PIN_7
#define nutrient_enable_GPIO_Port GPIOE
#define water_heat_cool_Pin GPIO_PIN_8
#define water_heat_cool_GPIO_Port GPIOE
#define heat_cool_fan_speed_Pin GPIO_PIN_9
#define heat_cool_fan_speed_GPIO_Port GPIOE
#define grow_light_Pin GPIO_PIN_10
#define grow_light_GPIO_Port GPIOE
#define water_pump_enable_Pin GPIO_PIN_11
#define water_pump_enable_GPIO_Port GPIOE
#define water_level_Pin GPIO_PIN_1
#define water_level_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
