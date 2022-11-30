/*
 * buzzer.h
 *
 *  Created on: Nov 8, 2022
 *      Author: bsoh
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"

void buzzerInit(TIM_HandleTypeDef *htim, uint32_t Channel);
void humidifierOnBuzzerSound(void);
void buzzerSpeedButtonSound(void);
void buzzerStartFreq(void);
void buzzerStoptFreq(void);
void buzzerGenFreq(uint16_t freq);
void buzzerBlinkSound(void);

#endif /* INC_BUZZER_H_ */
