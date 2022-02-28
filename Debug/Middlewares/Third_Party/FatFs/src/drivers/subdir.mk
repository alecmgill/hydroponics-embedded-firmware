################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FatFs/src/drivers/usbh_diskio.c 

OBJS += \
./Middlewares/Third_Party/FatFs/src/drivers/usbh_diskio.o 

C_DEPS += \
./Middlewares/Third_Party/FatFs/src/drivers/usbh_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FatFs/src/drivers/%.o: ../Middlewares/Third_Party/FatFs/src/drivers/%.c Middlewares/Third_Party/FatFs/src/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -std=gnu11 -g -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-FatFs-2f-src-2f-drivers

clean-Middlewares-2f-Third_Party-2f-FatFs-2f-src-2f-drivers:
	-$(RM) ./Middlewares/Third_Party/FatFs/src/drivers/usbh_diskio.d ./Middlewares/Third_Party/FatFs/src/drivers/usbh_diskio.o

.PHONY: clean-Middlewares-2f-Third_Party-2f-FatFs-2f-src-2f-drivers

