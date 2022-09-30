/*
 * sensor.c
 *
 *  Created on: Sep 17, 2022
 *      Author: bsoh
 */
#include "sensor.h"

void sensorTask(void const * argument)
{
	  //ENV_MSG envData;
	ENV_MSG envData;
	  uint8_t status = 0;
	  char buf[128] = {0,};

	  dht11Init(&dht11Dev);
	  DEBUG_PRINT("init....");

	  while (1)
	  {
		  dht11Start(&dht11Dev);
		  status = dht11CheckResponse(&dht11Dev);

		  if (status == DHT11_SUCCESS)
		  {
			  //envData = (ENV_MSG *)osPoolAlloc(Pool_ID);
			  //pMsgQ = (MessageQ *)osPoolAlloc (Pool_ID);

			  Rh_byte1 = dht11Read(&dht11Dev);
			  Rh_byte2 = dht11Read(&dht11Dev);
			  Temp_byte1 = dht11Read(&dht11Dev);
			  Temp_byte2 = dht11Read(&dht11Dev);
			  SUM = dht11Read(&dht11Dev);

			  dht11Dev.temparature = Temp_byte1;
			  dht11Dev.humidity    = Rh_byte1;

			  envData.temp  = (int)dht11Dev.temparature;
			  envData.humid = (int)dht11Dev.humidity;

			  //snprintf(buf, sizeof(buf), "start temp:[%u] humid[%u]\r\n", dht11Dev.temparature, dht11Dev.humidity);
			 // DEBUG_PRINT(buf);
			  //memset(buf, 0x0, sizeof(buf));



			 if (osMessagePut(envQueueHandle, (uint32_t)(&envData), osWaitForever) == osOK)
			  //if (osMessagePut(envQueueHandle, ttt, osWaitForever) == osOK)
			 {
				  /* ToDo */
				 //DEBUG_PRINT("osMessagePut OK");
#if (1)
				 // snprintf(buf, sizeof(buf), "temp:[%d] humid[%d]\r\n", envData.temp, envData.humid);
				  //DEBUG_PRINT(buf);
				  //memset(buf, 0x0, sizeof(buf));
#else
				snprintf(buf, sizeof(buf), "sensor task : size:%d\r\n", sizeof(ENV_MSG));
				HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);
#endif
				//goto OK;
				// osSemaphoreRelease(envSemHandle);
			 }

			  osDelay(1000);
		  }
		  else
		  {
			  osDelay(100);
		  }
	  }
	  while (1)
	  {
		  //OK:
		  osDelay(1);
	  }
}

void sensorInit(osPriority Priority)
{
	osSemaphoreDef(sensorHandle);
	sensorHandle = osSemaphoreCreate(osSemaphore(sensorHandle), 1);
	osThreadDef(sensorTaskName, sensorTask, Priority, 0, _SENSOR_TASK_SIZE);
	sensorTaskHandle = osThreadCreate(osThread(sensorTaskName), NULL);
}

