/**
  ******************************************************************************
  * File Name          : STM32TouchController.cpp
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* USER CODE BEGIN STM32TouchController */

#include <STM32TouchController.hpp>
#include "stm32f429i_discovery_ts.h"
#include "main.h"
#include "string.h"
#include "stdio.h"
//#include <cstring.h>

extern UART_HandleTypeDef huart1;

void STM32TouchController::init()
{
    /**
     * Initialize touch controller and driver
     *
     */
	BSP_TS_Init(240, 320);
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    /**
     * By default sampleTouch returns false,
     * return true if a touch has been detected, otherwise false.
     *
     * Coordinates are passed to the caller by reference by x and y.
     *
     * This function is called by the TouchGFX framework.
     * By default sampleTouch is called every tick, this can be adjusted by HAL::setTouchSampleRate(int8_t);
     *
     */
    TS_StateTypeDef TS_State;
    BSP_TS_GetState(&TS_State);
    if (TS_State.TouchDetected)
    {
    	//char buf[128] = {0,};

        x = TS_State.X;
        y = TS_State.Y;

        //x = 240 - x;
        y = 320 - y;

        //snprintf(buf, sizeof(buf), "x=%lu y=%lu\r\n", x, y);
        //HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);
        //DEBUG_PRINT(buf);

        return true;
    }

    //DEBUG_PRINT("return false");
    return false;
}

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
