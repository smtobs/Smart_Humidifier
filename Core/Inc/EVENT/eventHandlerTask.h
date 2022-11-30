/*
 * workTask.h
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#ifndef INC_WORKTASK_H_
#define INC_WORKTASK_H_

#include <eventHandlerTaskConfig.h>
#include "common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "ringbuffer.h"

osThreadId 		     workTaskHandle;
osSemaphoreId 	     workHandle;
extern osSemaphoreId sensorSemHandle;

extern osSemaphoreId envSemHandle;
extern osMessageQId envQueueHandle;

ring_buffer_t ring_buffer;

void _eventHandler(void const * argument);

#endif /* INC_WORKTASK_H_ */
