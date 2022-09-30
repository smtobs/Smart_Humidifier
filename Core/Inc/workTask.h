/*
 * workTask.h
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#ifndef INC_WORKTASK_H_
#define INC_WORKTASK_H_

//#include "main.h"
#include "common.h"
#include "workTaskConfig.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "ringbuffer.h"

osThreadId 		workTaskHandle;
osSemaphoreId 	workHandle;

extern osSemaphoreId envSemHandle;
extern osMessageQId envQueueHandle;
extern osPoolId     Pool_ID;

ring_buffer_t ring_buffer;

void workTaskInit(osPriority Priority);

#endif /* INC_WORKTASK_H_ */
