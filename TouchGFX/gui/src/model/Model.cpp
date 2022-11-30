#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "event.h"
#include "main.h"

extern osMessageQId      envQueueHandle;
extern osEventFlagsId_t  eventFlagHandle;
extern PRODUCT_STATUS    productStatus;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	if (sensorUpdatePeriod >= 10)
	{
		ENV_MSG envData;

		if ( (osMessageQueueGet(envQueueHandle, &envData, NULL, 1)) == osOK)
		{
			modelListener->UpdateTemp((envData.temp));
			modelListener->UpdateHum((envData.humid));
		}
		modelListener->ChkUpdateOnOffButton(productStatus == ON ? true : false);
		sensorUpdatePeriod = 0;
	}
	sensorUpdatePeriod++;
}

void Model::updateHumidifierControlState(bool state)
{
	if (state == true)
	{
		osEventFlagsSet(eventFlagHandle, HUM_ON);
	}
	else
	{
		osEventFlagsSet(eventFlagHandle, HUM_OFF);
	}
}
