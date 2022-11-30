/*
 * sensor.h
 *
 *  Created on: Sep 17, 2022
 *      Author: bsoh
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

//#include "main.h"
#include "common.h"
#include "sensorConfig.h"
#include "event.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "stdbool.h"

extern ADC_HandleTypeDef hadc1;

osThreadId 		sensorTaskHandle;
osSemaphoreId 	sensorHandle;

typedef struct {
	int temparature;
	int humidity;
}sensorDev;

bool readEth01dvSensor(sensorDev *sensor);

#endif /* INC_SENSOR_H_ */
