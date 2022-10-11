################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.c 

C_DEPS += \
./Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.d 

OBJS += \
./Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.o: ../Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.c Drivers/BSP/Components/mfxstm32l152/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Middlewares/MQTTPacket/src" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/EVENT" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/TFT_LCD" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/WIFI" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/CJSON" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/RINGBUFFER" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/SENSOR" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Core/Inc/MQTT" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/gui/include -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Drivers/BSP/Components" -I"C:/Users/v7dd1/STM32CubeIDE/workspace_1.6.1/2022_10_09_commit/SmartHumidfier/Drivers/BSP/STM32F429I-Discovery" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

