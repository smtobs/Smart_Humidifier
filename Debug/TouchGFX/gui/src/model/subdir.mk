################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/gui/src/model/Model.cpp 

OBJS += \
./TouchGFX/gui/src/model/Model.o 

CPP_DEPS += \
./TouchGFX/gui/src/model/Model.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/gui/src/model/Model.o: ../TouchGFX/gui/src/model/Model.cpp TouchGFX/gui/src/model/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/EVENT" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/gui/include -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Drivers/BSP/STM32F429I-Discovery" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -MMD -MP -MF"TouchGFX/gui/src/model/Model.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

