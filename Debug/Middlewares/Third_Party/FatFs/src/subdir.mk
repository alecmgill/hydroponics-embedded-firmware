################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FatFs/src/diskio.c \
../Middlewares/Third_Party/FatFs/src/ff.c \
../Middlewares/Third_Party/FatFs/src/ff_gen_drv.c 

OBJS += \
./Middlewares/Third_Party/FatFs/src/diskio.o \
./Middlewares/Third_Party/FatFs/src/ff.o \
./Middlewares/Third_Party/FatFs/src/ff_gen_drv.o 

C_DEPS += \
./Middlewares/Third_Party/FatFs/src/diskio.d \
./Middlewares/Third_Party/FatFs/src/ff.d \
./Middlewares/Third_Party/FatFs/src/ff_gen_drv.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FatFs/src/%.o: ../Middlewares/Third_Party/FatFs/src/%.c Middlewares/Third_Party/FatFs/src/subdir.mk
	arm-none-eabi-gcc "$<" -std=gnu11 -g -c -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-FatFs-2f-src

clean-Middlewares-2f-Third_Party-2f-FatFs-2f-src:
	-$(RM) ./Middlewares/Third_Party/FatFs/src/diskio.d ./Middlewares/Third_Party/FatFs/src/diskio.o ./Middlewares/Third_Party/FatFs/src/ff.d ./Middlewares/Third_Party/FatFs/src/ff.o ./Middlewares/Third_Party/FatFs/src/ff_gen_drv.d ./Middlewares/Third_Party/FatFs/src/ff_gen_drv.o

.PHONY: clean-Middlewares-2f-Third_Party-2f-FatFs-2f-src

