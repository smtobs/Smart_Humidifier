/*
 * sensor.c
 *
 *  Created on: Sep 17, 2022
 *      Author: bsoh
 */
#include "sensor.h"

#define MAX_AVERAGE_FILTER     5

bool readEth01dvSensor(sensorDev *sensor)
{
	uint32_t val=0;
	int temp=0, humid=0, cont;

	/* 평균  */
	for (cont=0; cont<MAX_AVERAGE_FILTER; cont++)
	{
		/* Temp */
		HAL_ADC_Start(&hadc1);
		val = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		temp += (int)(-66.875 + 218.75 * val / 1024);

		/* humidity */
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		val = HAL_ADC_GetValue(&hadc1);
		humid += (int)(-12.5 + 125 * val / 1024);
	}
	sensor->temparature = temp  / MAX_AVERAGE_FILTER;
	sensor->humidity    = humid / MAX_AVERAGE_FILTER;
	return true;
}
