#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "event.h"
#include "main.h"

extern osThreadId    defaultTaskHandle;
extern osMessageQId  envQueueHandle;
extern osSemaphoreId envSemHandle;
extern osPoolId      Pool_ID;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
#if (1)
	if (sensorUpdatePeriod >= 100)
	{
		ENV_MSG *envData;
		osEvent event;
		char buf[128] = {0,};

		event = osMessageGet(envQueueHandle, osWaitForever);

		if (event.status == osEventMessage)
		{
			envData = (ENV_MSG *)event.value.p;
			//DEBUG_PRINT(pMsq->temp);

			snprintf(buf, sizeof(buf), "temp:%d\r\n", envData->temp);
			HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);

			memset(buf, 0x0, sizeof(buf));
			snprintf(buf, sizeof(buf), "humid:%d\r\n", envData->humid);
			HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);

			//osPoolFree(Pool_ID, envData);
			modelListener->UpdateTemp((envData->temp));
			modelListener->UpdateHum((envData->humid));
		}
		sensorUpdatePeriod = 0;
	}
	sensorUpdatePeriod++;
#endif
}

void Model::updateHumidifierControlState(bool state)
{
	if (state == true)
	{
		osSignalSet(defaultTaskHandle, BUZZER_ON | HUM_ON);
	}
	else
	{
		osSignalSet(defaultTaskHandle, BUZZER_OFF | HUM_OFF);
	}
}
