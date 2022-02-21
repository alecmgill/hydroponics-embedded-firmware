################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ADC_driver.c \
../Core/Src/File_Handling.c \
../Core/Src/RTC_driver.c \
../Core/Src/TDS_driver.c \
../Core/Src/calibrate_system.c \
../Core/Src/fan_driver.c \
../Core/Src/heater_driver.c \
../Core/Src/hydro_control_loop.c \
../Core/Src/light_driver.c \
../Core/Src/main.c \
../Core/Src/nutrient_pH_driver.c \
../Core/Src/pH_driver.c \
../Core/Src/peristaltic_driver.c \
../Core/Src/sensors.c \
../Core/Src/stm32f2xx_hal_msp.c \
../Core/Src/stm32f2xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f2xx.c \
../Core/Src/water_temp_driver.c 

OBJS += \
./Core/Src/ADC_driver.o \
./Core/Src/File_Handling.o \
./Core/Src/RTC_driver.o \
./Core/Src/TDS_driver.o \
./Core/Src/calibrate_system.o \
./Core/Src/fan_driver.o \
./Core/Src/heater_driver.o \
./Core/Src/hydro_control_loop.o \
./Core/Src/light_driver.o \
./Core/Src/main.o \
./Core/Src/nutrient_pH_driver.o \
./Core/Src/pH_driver.o \
./Core/Src/peristaltic_driver.o \
./Core/Src/sensors.o \
./Core/Src/stm32f2xx_hal_msp.o \
./Core/Src/stm32f2xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f2xx.o \
./Core/Src/water_temp_driver.o 

C_DEPS += \
./Core/Src/ADC_driver.d \
./Core/Src/File_Handling.d \
./Core/Src/RTC_driver.d \
./Core/Src/TDS_driver.d \
./Core/Src/calibrate_system.d \
./Core/Src/fan_driver.d \
./Core/Src/heater_driver.d \
./Core/Src/hydro_control_loop.d \
./Core/Src/light_driver.d \
./Core/Src/main.d \
./Core/Src/nutrient_pH_driver.d \
./Core/Src/pH_driver.d \
./Core/Src/peristaltic_driver.d \
./Core/Src/sensors.d \
./Core/Src/stm32f2xx_hal_msp.d \
./Core/Src/stm32f2xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f2xx.d \
./Core/Src/water_temp_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F207xx -c -I../Core/Inc -I../Drivers/STM32F2xx_HAL_Driver/Inc -I../Drivers/STM32F2xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F2xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/Third_Party/FatFs/src/drivers -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ADC_driver.d ./Core/Src/ADC_driver.o ./Core/Src/File_Handling.d ./Core/Src/File_Handling.o ./Core/Src/RTC_driver.d ./Core/Src/RTC_driver.o ./Core/Src/TDS_driver.d ./Core/Src/TDS_driver.o ./Core/Src/calibrate_system.d ./Core/Src/calibrate_system.o ./Core/Src/fan_driver.d ./Core/Src/fan_driver.o ./Core/Src/heater_driver.d ./Core/Src/heater_driver.o ./Core/Src/hydro_control_loop.d ./Core/Src/hydro_control_loop.o ./Core/Src/light_driver.d ./Core/Src/light_driver.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/nutrient_pH_driver.d ./Core/Src/nutrient_pH_driver.o ./Core/Src/pH_driver.d ./Core/Src/pH_driver.o ./Core/Src/peristaltic_driver.d ./Core/Src/peristaltic_driver.o ./Core/Src/sensors.d ./Core/Src/sensors.o ./Core/Src/stm32f2xx_hal_msp.d ./Core/Src/stm32f2xx_hal_msp.o ./Core/Src/stm32f2xx_it.d ./Core/Src/stm32f2xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f2xx.d ./Core/Src/system_stm32f2xx.o ./Core/Src/water_temp_driver.d ./Core/Src/water_temp_driver.o

.PHONY: clean-Core-2f-Src

