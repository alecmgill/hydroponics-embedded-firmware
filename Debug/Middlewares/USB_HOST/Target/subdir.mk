################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/USB_HOST/Target/usbh_conf.c \
../Middlewares/USB_HOST/Target/usbh_platform.c 

OBJS += \
./Middlewares/USB_HOST/Target/usbh_conf.o \
./Middlewares/USB_HOST/Target/usbh_platform.o 

C_DEPS += \
./Middlewares/USB_HOST/Target/usbh_conf.d \
./Middlewares/USB_HOST/Target/usbh_platform.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/USB_HOST/Target/%.o: ../Middlewares/USB_HOST/Target/%.c Middlewares/USB_HOST/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F207xx -c -I../Core/Inc -I../Drivers/STM32F2xx_HAL_Driver/Inc -I../Drivers/STM32F2xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F2xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/Third_Party/FatFs/src/drivers -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-USB_HOST-2f-Target

clean-Middlewares-2f-USB_HOST-2f-Target:
	-$(RM) ./Middlewares/USB_HOST/Target/usbh_conf.d ./Middlewares/USB_HOST/Target/usbh_conf.o ./Middlewares/USB_HOST/Target/usbh_platform.d ./Middlewares/USB_HOST/Target/usbh_platform.o

.PHONY: clean-Middlewares-2f-USB_HOST-2f-Target

