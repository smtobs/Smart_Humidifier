/*
 * event_loop.c
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#include "event_loop.h"

extern osEventFlagsId_t eventFlagHandle;

void eventLoopTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	uint32_t eventFlag;

	DEBUG_PRINT("eventLoopTask..");

  /* Infinite loop */
  for (;;)
  {
	    eventFlag = osEventFlagsWait(eventFlagHandle, 0x00FF, osFlagsWaitAny, osWaitForever);
		{
				switch (eventFlag)
				{
					case WIFI_SEND_REP:
						DEBUG_PRINT("WIFI_SEND_REP");
						ring_buffer_queue(&ring_buffer, (unsigned char)WIFI_SEND_REP);
						break;

					case UART_RECEIVE:
						DEBUG_PRINT("UART_RECEIVE");
						ring_buffer_queue(&ring_buffer, (unsigned char)UART_RECEIVE);
						break;

					case UPDATE_SENSOR_VALUE:
						ring_buffer_queue(&ring_buffer, (unsigned char)UPDATE_SENSOR_VALUE);
						break;

					case HUM_ON:
						ring_buffer_queue(&ring_buffer, (unsigned char)HUM_ON);
						break;

					case HUM_OFF:

						ring_buffer_queue(&ring_buffer, (unsigned char)HUM_OFF);
						break;

					default:
						break;
				}
		}
		osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}
