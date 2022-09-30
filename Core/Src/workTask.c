/*
 * workTask.c
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */
#include "workTask.h"
#include "event.h"

#define RING_BUFF_SUCCESS 1
#define RING_BUFF_ERROR   0

//lwrb_t ringBuff;
extern int e_flag;


void _workTask(void const * argument)
{
	char buf;
	int cnt;
	char debug[32] = {0,};

	ring_buffer_init(&ring_buffer);

	DEBUG_PRINT("_workTask... init !!!!");
	osDelay(10);

	for (;;)
	{
		for(cnt = 0; ring_buffer_dequeue(&ring_buffer, &buf) > 0; cnt++)
		{
			/* Do something with buf... */
			int32_t evn=0;

			evn = (int32_t)buf;
			snprintf(debug, sizeof(debug), "dev:[%lu]\r\n", evn);

			switch (evn)
			{
				case BUZZER_ON:
				case BUZZER_OFF:
				case HUM_ON:
				case HUM_OFF:
					HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
					DEBUG_PRINT(debug);
					break;

				default:
					break;
			}
		}
	    osDelay(1);
	}
}

void workTaskInit(osPriority Priority)
{
	osSemaphoreDef(workHandle);
	workHandle = osSemaphoreCreate(osSemaphore(workHandle), 1);
	osThreadDef(workTaskName, _workTask, Priority, 0, _WORK_TASK_SIZE);
	workTaskHandle = osThreadCreate(osThread(workTaskName), NULL);
}
