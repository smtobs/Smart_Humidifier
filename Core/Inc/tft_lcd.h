/*
 * tft_lcd.h
 *
 *  Created on: Sep 24, 2022
 *      Author: v7dd1
 */

#ifndef INC_TFT_LCD_H_
#define INC_TFT_LCD_H_

#include "common.h"
#include "tft_lcd_config.h"

//#include "stm32f429i_discovery_lcd.h"
//#include "stm32f429i_discovery_ts.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

osThreadId 		lcdTaskHandle;
osSemaphoreId 	lcdHandle;

void tftLcdInit(osPriority);
//void lcdTask(void const *);


#endif /* INC_TFT_LCD_H_ */
