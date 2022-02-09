/*
 * water_temp_driver.h
 *
 *  Created on: Feb 6, 2022
 *      Author: Alec
 */

#ifndef INC_WATER_TEMP_DRIVER_H_
#define INC_WATER_TEMP_DRIVER_H_

#include "stm32f2xx_hal.h"
#include "main.h"
#include "sensors.h"
uint32_t DWT_Delay_Init(void);
uint8_t DS18B20_Start (void);
void DS18B20_Write (uint8_t data);
uint8_t DS18B20_Read (void);
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DWT_Delay_us(volatile uint32_t au32_microseconds);

#endif /* INC_WATER_TEMP_DRIVER_H_ */
