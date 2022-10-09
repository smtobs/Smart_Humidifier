/*
 * sensor.c
 *
 *  Created on: Sep 17, 2022
 *      Author: bsoh
 */
#include "sensor.h"
#include "event_loop.h"

extern DHT11_Dev dht11Dev;
extern osThreadId eventLoopTask;
extern osThreadId defaultTaskHandle;
extern osThreadId eventLoopTaskHandle;

void sensorTask(void const * argument)
{
	  uint8_t status = 0;

	  dht11Init(&dht11Dev);
	  DEBUG_PRINT("sensorTask init...");

	  while (1)
	  {
		  //dht11Start(&dht11Dev);
		  //status = dht11CheckResponse(&dht11Dev);

		  //if (status == DHT11_SUCCESS)
		  {
#if (0)
			  	if ( ((Rh_byte1 = dht11Read(&dht11Dev)) == 255) ||
			  			((Rh_byte2 = dht11Read(&dht11Dev)) == 255) ||
						((Temp_byte1 = dht11Read(&dht11Dev)) == 255) ||
						((Temp_byte2 = dht11Read(&dht11Dev)) == 255) ||
						((SUM = dht11Read(&dht11Dev)) == 255) )
			  	{
			  		goto error;
			  	}
#endif
#if (0)
				Rh_byte1 = dht11Read(&dht11Dev);
				Rh_byte2 = dht11Read(&dht11Dev);
				Temp_byte1 = dht11Read(&dht11Dev);
				Temp_byte2 = dht11Read(&dht11Dev);
				SUM = dht11Read(&dht11Dev);
#endif

#if (1)
				Rh_byte1 = (uint8_t)72;
				Temp_byte1 = (uint8_t)26;
#endif
				dht11Dev.temparature = Temp_byte1;
				dht11Dev.humidity    = Rh_byte1;

				osSemaphoreRelease(sensorSemHandle);
				osSignalSet(eventLoopTaskHandle, UPDATE_SENSOR_VALUE);
				osDelay(1000);
		  }
		 // else
		  {
			 // error:
			 // osDelay(100);
		  }
	  }
}

void sensorInit(osPriority Priority)
{
	osSemaphoreDef(sensorHandle);
	sensorHandle = osSemaphoreCreate(osSemaphore(sensorHandle), 1);
	osThreadDef(sensorTaskName, sensorTask, Priority, 0, _SENSOR_TASK_SIZE);
	sensorTaskHandle = osThreadCreate(osThread(sensorTaskName), NULL);
}

