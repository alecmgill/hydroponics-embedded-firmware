################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/USB_HOST/App/usb_host.c 

OBJS += \
./Middlewares/USB_HOST/App/usb_host.o 

C_DEPS += \
./Middlewares/USB_HOST/App/usb_host.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/USB_HOST/App/%.o: ../Middlewares/USB_HOST/App/%.c Middlewares/USB_HOST/App/subdir.mk
	arm-none-eabi-gcc "$<" -std=gnu11 -g -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-USB_HOST-2f-App

clean-Middlewares-2f-USB_HOST-2f-App:
	-$(RM) ./Middlewares/USB_HOST/App/usb_host.d ./Middlewares/USB_HOST/App/usb_host.o

.PHONY: clean-Middlewares-2f-USB_HOST-2f-App

