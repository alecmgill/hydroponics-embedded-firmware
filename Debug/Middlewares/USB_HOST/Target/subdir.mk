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
	arm-none-eabi-gcc "$<" -std=gnu11 -g -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-USB_HOST-2f-Target

clean-Middlewares-2f-USB_HOST-2f-Target:
	-$(RM) ./Middlewares/USB_HOST/Target/usbh_conf.d ./Middlewares/USB_HOST/Target/usbh_conf.o ./Middlewares/USB_HOST/Target/usbh_platform.d ./Middlewares/USB_HOST/Target/usbh_platform.o

.PHONY: clean-Middlewares-2f-USB_HOST-2f-Target

