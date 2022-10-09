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
#include "dht11.h"
#include "event.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

osThreadId 		sensorTaskHandle;
osSemaphoreId 	sensorHandle;

extern osSemaphoreId sensorSemHandle;
extern osSemaphoreId envSemHandle;
extern osMessageQId envQueueHandle;
extern osPoolId     Pool_ID;

DHT11_Dev dht11Dev;
void sensorInit(osPriority);


#endif /* INC_SENSOR_H_ */
