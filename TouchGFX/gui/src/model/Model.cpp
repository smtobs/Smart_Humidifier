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

			modelListener->UpdateTemp((envData->temp));
			modelListener->UpdateHum((envData->humid));
		}
		//sensorUpdatePeriod = 0;
	}
	//sensorUpdatePeriod++;
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
