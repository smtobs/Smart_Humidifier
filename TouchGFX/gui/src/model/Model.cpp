#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "event.h"
#include "main.h"

extern osThreadId    defaultTaskHandle;
extern osThreadId    eventLoopTaskHandle;
extern osMessageQId  envQueueHandle;
extern osSemaphoreId envSemHandle;
extern osPoolId      Pool_ID;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
#if (1)
	//sensorUpdatePeriod=0;
	//if (sensorUpdatePeriod >=100)
	{

		osEvent event;
		event = osMessageGet(envQueueHandle, 1);

		if (event.status == osEventMessage)
		{
			ENV_MSG *envData;


			char buf[128] = {0,};

			envData = (ENV_MSG *)event.value.p;
			//DEBUG_PRINT(pMsq->temp);

#if (0)
			snprintf(buf, sizeof(buf), "temp:%d\r\n", envData->temp);
			HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);

			memset(buf, 0x0, sizeof(buf));
			snprintf(buf, sizeof(buf), "humid:%d\r\n", envData->humid);
			HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);
#endif

			//osPoolFree(Pool_ID, envData);
			modelListener->UpdateTemp((envData->temp));
			modelListener->UpdateHum((envData->humid));
		}
		//sensorUpdatePeriod = 0;
	}
	//sensorUpdatePeriod++;
#endif
}

void Model::updateHumidifierControlState(bool state)
{
	if (state == true)
	{
		osSignalSet(eventLoopTaskHandle, BUZZER_ON | HUM_ON);
	}
	else
	{
		osSignalSet(eventLoopTaskHandle, BUZZER_OFF | HUM_OFF);
	}
}
