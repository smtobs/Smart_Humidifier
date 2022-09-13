/*
 * lcd.c
 *
 *  Created on: Sep 12, 2022
 *      Author: bsoh
 */
#include "lcd.h"

osThreadId 		lcdTaskHandle;
osSemaphoreId 	lcdHandle;

static uint16_t frame_buffer[76800];

void lcdInit(osPriority	Priority)
{
	osSemaphoreDef(lcdHandle);
	lcdHandle = osSemaphoreCreate(osSemaphore(lcdHandle), 1);
	osThreadDef(lcdTaskName, lcdTask, Priority, 0, _LCD_TASK_SIZE);
	lcdTaskHandle = osThreadCreate(osThread(lcdTaskName), NULL);
}

void lcdTask(void const * argument)
{
	  TS_StateTypeDef  TS_State;

	  static uint32_t x = 0, y = 0;
	  uint8_t buffer_str[50]={0,};

	  BSP_LCD_Init();
	  BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, (uint32_t)frame_buffer);
	  BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
	  BSP_LCD_DisplayOn();
	  BSP_LCD_Clear(LCD_COLOR_WHITE);

	  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());


	  BSP_LCD_SetBackColor(RGB_LCD_COLOR_WHITE);
	  BSP_LCD_SetTextColor(RGB_LCD_COLOR_BLACK);
	  BSP_LCD_SetFont(&LCD_20_FONT);
	  BSP_LCD_DisplayStringAt(0,150, (uint8_t*)"Welcome", CENTER_MODE);
	  BSP_LCD_SetFont(&LCD_16_FONT);
	  BSP_LCD_DisplayStringAt(0,180, (uint8_t*)"STM32", CENTER_MODE);
	  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	  while (1)
	  {
		    BSP_TS_GetState(&TS_State);
		    if ((TS_State.TouchDetected))
		    {
		      x = TS_State.X;
		      y = TS_State.Y;
		      sprintf((char *)buffer_str, "X=%d   ",(int)x);
		      BSP_LCD_DisplayStringAt(0,50, (uint8_t*)buffer_str, LEFT_MODE);
		      memset(buffer_str,0,50);

		      sprintf((char *)buffer_str, "Y=%d   ",(int)y);
		      BSP_LCD_DisplayStringAt(0,100, (uint8_t*)buffer_str, LEFT_MODE);
		      memset(buffer_str,0,50);
		    }
	  }
}
