################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f207zgtx.s 

OBJS += \
./Core/Startup/startup_stm32f207zgtx.o 

S_DEPS += \
./Core/Startup/startup_stm32f207zgtx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -DDEBUG -c -I"C:/Users/Alec/STM32CubeIDE/HydroProject/.git" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/.settings" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Core" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Debug" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Drivers" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/FATFS" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/Third_Party" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/App" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/Target" -I"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Inc" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_core.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/App/usb_host.c" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/App/usb_host.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/Target/usbh_conf.c" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/Target/usbh_conf.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/Target/usbh_platform.c" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/USB_HOST/Target/usbh_platform.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_ctlreq.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_def.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_ioreq.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/usbh_pipes.h" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c" -include"C:/Users/Alec/STM32CubeIDE/HydroProject/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f207zgtx.d ./Core/Startup/startup_stm32f207zgtx.o

.PHONY: clean-Core-2f-Startup

