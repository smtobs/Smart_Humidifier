/*
 * event_loop.h
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#ifndef INC_EVENT_LOOP_H_
#define INC_EVENT_LOOP_H_

#include "main.h"
#include "common.h"
#include "event.h"
#include "event_loop_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "ringbuffer.h"

osThreadId 		     eventLoopTaskHandle;
osSemaphoreId 	     eventLoopHandle;

extern ring_buffer_t ring_buffer;

void eventLoopTask(void const *);

#endif /* INC_EVENT_LOOP_H_ */
