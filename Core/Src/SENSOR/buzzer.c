/*
 * buzzer.c
 *
 *  Created on: Nov 8, 2022
 *      Author: bsoh
 */


#include "buzzer.h"
#include "cmsis_os2.h"

TIM_HandleTypeDef *hBuzzTim;
uint32_t buzzChannel;

void buzzerInit(TIM_HandleTypeDef *htim, uint32_t Channel)
{
	hBuzzTim = htim;
	buzzChannel = Channel;

	humidifierOnBuzzerSound();
	humidifierOnBuzzerSound();
}

void humidifierOnBuzzerSound(void)
{
	buzzerStartFreq();

	buzzerGenFreq(1000);
	osDelay(100);

	buzzerGenFreq(2000);
	osDelay(100);

	buzzerGenFreq(3000);
	osDelay(100);

	buzzerStoptFreq();
}

void buzzerSpeedButtonSound(void)
{
	buzzerStartFreq();
	buzzerGenFreq(3000);
	osDelay(100);
	buzzerGenFreq(4000);
	osDelay(100);
	buzzerGenFreq(5000);
	osDelay(100);
	buzzerStoptFreq();
}

void buzzerStartFreq(void)
{
	HAL_TIM_PWM_Start(hBuzzTim, buzzChannel);
	//__HAL_TIM_SET_COUNTER(hBuzzTim, 0); //cnt 0으로 초기화
}

void buzzerStoptFreq(void)
{
	HAL_TIM_PWM_Stop(hBuzzTim, buzzChannel);
}

void buzzerGenFreq(uint16_t freq)
{
	/* Frequncy */
	uint16_t arr = (100000/freq) - 1;

	/* Duty */
	uint16_t ccr = arr/2;

	__HAL_TIM_SET_AUTORELOAD(hBuzzTim, arr);
	__HAL_TIM_SET_COMPARE(hBuzzTim, buzzChannel, ccr);
}

void buzzerBlinkSound(void)
{
	buzzerStartFreq();
	buzzerGenFreq(1000);
	osDelay(100);
	buzzerStoptFreq();
}
