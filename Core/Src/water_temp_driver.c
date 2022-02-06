/*
 * water_temp_driver.c
 *
 *  Created on: Feb 6, 2022
 *      Author: Alec
 */
#include "main.h"
#include "sensors.h"
#include "water_temp_driver.h"

uint32_t DWT_Delay_Init(void)
{
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT)
    {
       return 0; /*clock cycle counter started*/
    }
    else
    {
      return 1; /*clock cycle counter not started*/
    }
}

 void DWT_Delay_us(volatile uint32_t au32_microseconds)
{
  uint32_t au32_initial_ticks = DWT->CYCCNT;
  uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
  au32_microseconds *= au32_ticks;
  while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}


uint8_t DS18B20_Start (void)
{
	uint8_t Response = 0;
	Set_Pin_Output(water_temp_GPIO_Port, water_temp_Pin);   // set the pin as output
	HAL_GPIO_WritePin (water_temp_GPIO_Port, water_temp_Pin, 0);  // pull the pin low
	DWT_Delay_us (480);   // delay according to datasheet

	Set_Pin_Input(water_temp_GPIO_Port, water_temp_Pin);    // set the pin as input
	DWT_Delay_us (80);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin (water_temp_GPIO_Port, water_temp_Pin))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	else Response = -1;

	DWT_Delay_us (400); // 480 us delay totally.

	return Response;
}

void DS18B20_Write (uint8_t data)
{
	Set_Pin_Output(water_temp_GPIO_Port, water_temp_Pin);  // set as output

	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			Set_Pin_Output(water_temp_GPIO_Port, water_temp_Pin);  // set as output
			HAL_GPIO_WritePin (water_temp_GPIO_Port, water_temp_Pin, 0);  // pull the pin LOW
			DWT_Delay_us (5);  // wait for 1 us

			Set_Pin_Input(water_temp_GPIO_Port, water_temp_Pin);  // set as input
			DWT_Delay_us (65);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			Set_Pin_Output(water_temp_GPIO_Port, water_temp_Pin);
			HAL_GPIO_WritePin (water_temp_GPIO_Port, water_temp_Pin, 0);  // pull the pin LOW
			DWT_Delay_us (65);  // wait for 60 us

			Set_Pin_Input(water_temp_GPIO_Port, water_temp_Pin);
		}
	}
}

uint8_t DS18B20_Read (void)
{
	uint8_t value=0;

	Set_Pin_Input(water_temp_GPIO_Port, water_temp_Pin);

	for (int i=0;i<8;i++)
	{
		Set_Pin_Output(water_temp_GPIO_Port, water_temp_Pin);   // set as output

		HAL_GPIO_WritePin (water_temp_GPIO_Port, water_temp_Pin, 0);  // pull the data pin LOW
		DWT_Delay_us (5);  // wait for > 1us

		Set_Pin_Input(water_temp_GPIO_Port, water_temp_Pin);  // set as input
		if (HAL_GPIO_ReadPin (water_temp_GPIO_Port, water_temp_Pin))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		DWT_Delay_us (50);  // wait for 60 us
	}
	return value;
}


void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
