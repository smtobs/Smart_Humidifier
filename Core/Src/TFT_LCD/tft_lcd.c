/*
 * tft_lcd.c
 *
 *  Created on: Sep 24, 2022
 *      Author: bsoh
 */


#include "tft_lcd.h"
#include "ili9341/ili9341.h"
#include "Common/lcd.h"

extern void TouchGFX_Task(void const * argument);

static LCD_DrvTypeDef* LcdDrv;

void LcdDrvStart()
{
	LcdDrv = &ili9341_drv;
	LcdDrv->Init();
	LcdDrv->DisplayOff();
}

