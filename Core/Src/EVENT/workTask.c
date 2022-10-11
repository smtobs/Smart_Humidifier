/*
 * workTask.c
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */
#include "workTask.h"
#include "event.h"
#include "dht11.h"
#include "mqtt.h"
#include "Wifi.h"

#include "cJSON.h"

extern DHT11_Dev dht11Dev;
extern int e_flag;
extern bool isConnectBroker;
//osThreadId 		WifiTaskHandle;
extern osSemaphoreId 	WifiSemHandle;
PRODUCT_STATUS          productStatus;

static char *createJsonMessage()
{
	char value[8] = {0,};
	char *string = NULL;
	cJSON *name  = NULL;
	cJSON *monitor = cJSON_CreateObject();

    if (monitor == NULL)
    {
        goto end;
    }

    /* Temp */
    snprintf(value, sizeof(value), "%u", dht11Dev.temparature);
    name = cJSON_CreateString(value);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "temp", name);


    memset(value, 0x0, sizeof(value));
    /* Humidity */
    snprintf(value, sizeof(value), "%u", dht11Dev.humidity);
    name = cJSON_CreateString(value);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "humidity", name);

    memset(value, 0x0, sizeof(value));
    /* Humidity */
    snprintf(value, sizeof(value), "%d", productStatus);
    name = cJSON_CreateString(value);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "status", name);


    string = cJSON_Print(monitor);
    if (string == NULL)
    {
        DEBUG_PRINT("Failed to print monitor.");
    }

    end:
        cJSON_Delete(monitor);
        return string;
}

static void messageForwardServer()
{
	char *message = NULL;

	message = createJsonMessage();
	ayncPublish(message);

	if (message)
	{
		free(message);
	}

}

osStatus messageForwardLcdTask(ENV_MSG *envData)
{
	return osMessagePut(envQueueHandle, (uint32_t)envData, 10);
}

void sensorEventHandling()
{
	ENV_MSG envData;

	if (osSemaphoreWait(sensorSemHandle, 1) == osOK)
	{
		envData.humid    = (int)dht11Dev.humidity;
		envData.temp     = (int)dht11Dev.temparature;

		if ( (messageForwardLcdTask(&envData) == osOK) && (isConnectBroker == true) )
		{
			messageForwardServer(&dht11Dev);
		}
	}
}

void _workTask(void const * argument)
{
	//ENV_MSG envData;
	char buf;
	int cnt;
	char debug[32] = {0,};

	ring_buffer_init(&ring_buffer);

	DEBUG_PRINT("_workTask... init !!!!");

	for (;;)
	{
		for (cnt = 0; ring_buffer_dequeue(&ring_buffer, &buf) > 0; cnt++)
		{
			/* Do something with buf... */
			int32_t evn=0;
			evn = (int32_t)buf;
			snprintf(debug, sizeof(debug), "dev:[%lu]\r\n", evn);

			switch (evn)
			{
				case UPDATE_SENSOR_VALUE:
					sensorEventHandling();
					break;

				case BUZZER_ON:
				case BUZZER_OFF:
					/* ToDo */
					break;
				case HUM_ON:
					productStatus = ON;
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, SET);
					break;
				case HUM_OFF:
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, RESET);
					productStatus = OFF;
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
