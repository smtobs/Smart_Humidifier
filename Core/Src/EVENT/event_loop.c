/*
 * event_loop.c
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#include "event_loop.h"
#include "dht11.h"

void eventLoopTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	osEvent evt;
	int32_t i;
	DEBUG_PRINT("eventLoopTask..");
  /* Infinite loop */
  for (;;)
  {
		evt = osSignalWait(0x00FF, 50);
		if (evt.status == osEventSignal)
		{
			for (i=0; i<=5; i++)
			{
				switch ((evt.value.signals) & (1U << i))
				{
					case UPDATE_SENSOR_VALUE:
						ring_buffer_queue(&ring_buffer, (char)(evt.value.signals) & (1U << i));
						break;

					case BUZZER_ON:
					case BUZZER_OFF:
					case HUM_ON:
					case HUM_OFF:

						ring_buffer_queue(&ring_buffer, (char)(evt.value.signals) & (1U << i));
						break;

					default:
						break;
				}
			}
		}
		osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

void eventLoopInit(osPriority Priority)
{
	osSemaphoreDef(eventLoopHandle);
	eventLoopHandle = osSemaphoreCreate(osSemaphore(eventLoopHandle), 1);
	osThreadDef(eventLoopTaskName, eventLoopTask, Priority, 0, _EVENT_LOOP_TASK_SIZE);
	eventLoopTaskHandle = osThreadCreate(osThread(eventLoopTaskName), NULL);
}
