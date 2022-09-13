/*
 * lcd.h
 *
 *  Created on: Sep 12, 2022
 *      Author: bsoh
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdio.h>
#include "main.h"

#include "lcdConfig.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"

void lcdInit(osPriority);
void lcdTask(void const *);

#endif /* INC_LCD_H_ */
