################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../NES/Src/BUS.c \
../NES/Src/CPU.c \
../NES/Src/Catridge.c \
../NES/Src/PPU.c 

C_DEPS += \
./NES/Src/BUS.d \
./NES/Src/CPU.d \
./NES/Src/Catridge.d \
./NES/Src/PPU.d 

OBJS += \
./NES/Src/BUS.o \
./NES/Src/CPU.o \
./NES/Src/Catridge.o \
./NES/Src/PPU.o 


# Each subdirectory must supply rules for building sources it contributes
NES/Src/%.o NES/Src/%.su: ../NES/Src/%.c NES/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/STM32/Workspace/dendy/LCD/Inc" -I"E:/STM32/Workspace/dendy/NES/Inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-NES-2f-Src

clean-NES-2f-Src:
	-$(RM) ./NES/Src/BUS.d ./NES/Src/BUS.o ./NES/Src/BUS.su ./NES/Src/CPU.d ./NES/Src/CPU.o ./NES/Src/CPU.su ./NES/Src/Catridge.d ./NES/Src/Catridge.o ./NES/Src/Catridge.su ./NES/Src/PPU.d ./NES/Src/PPU.o ./NES/Src/PPU.su

.PHONY: clean-NES-2f-Src

