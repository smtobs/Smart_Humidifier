/*
 * eventHandlerTask.c
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */
#include "eventHandlerTask.h"
#include "event.h"
#include "mqtt.h"
#include "Wifi.h"
#include "cJSON.h"
#include "sensor.h"
#include "buzzer.h"

extern s_mqtt mqttPublish;
extern TIM_HandleTypeDef htim2;
extern bool             isConnectBroker;
extern osTimerId        updateSensorTimerHandle;
extern PRODUCT_STATUS   productStatus;

void sensorEventHandling();

static char *createJsonMessage(sensorDev *sensor)
{
	static unsigned long seq = 0;

	char value[8] = {0,};
	char *string = NULL;
	cJSON *name  = NULL;
	cJSON *monitor = cJSON_CreateObject();

    if (monitor == NULL)
    {
        goto end;
    }

    /* Temp */
    snprintf(value, sizeof(value), "%lu", seq++);
    name = cJSON_CreateString(value);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "sequence", name);


    memset(value, 0x0, sizeof(value));
    /* Temp */
    snprintf(value, sizeof(value), "%u", sensor->temparature);
    name = cJSON_CreateString(value);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "temp", name);


    memset(value, 0x0, sizeof(value));
    /* Humidity */
    snprintf(value, sizeof(value), "%u", sensor->humidity);
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

static void messageForwardBroker(sensorDev *sensor)
{
	char *message = NULL;

	message = createJsonMessage(sensor);
	publish(message);

	if (message)
	{
		free(message);
	}
}

osStatus messageForwardLcdTask(ENV_MSG *envData)
{
	return osMessageQueuePut(envQueueHandle, envData, 0, 10);
}

void sensorEventHandling()
{
	ENV_MSG envData;
	sensorDev sensor;
	char debug[128] = {0,};

	readEth01dvSensor(&sensor);

	snprintf(debug, sizeof(debug), "sensor.temparature [%d]", sensor.temparature);
	//DEBUG_PRINT(debug);

	envData.humid    = sensor.humidity;
	envData.temp     = sensor.temparature;
	if ( (messageForwardLcdTask(&envData) == osOK) && (isConnectBroker == true) )
	{
		messageForwardBroker(&sensor);
	}
}

void humidifierOn()
{
	productStatus = ON;

	/* humidifier I/O */
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, SET);

	/* FET ON */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, SET);

	/* LED */
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, SET);

	humidifierOnBuzzerSound();
}

void humidifierOff()
{
	productStatus = OFF;

	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, RESET);

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, RESET);

	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, RESET);

}

void receiveHandlling()
{
	PacketProcessing(&mqttPublish);
}

void _eventHandler(void const * argument)
{
	unsigned char index;
	int cnt;
	char debug[32] = {0,};

	DEBUG_PRINT("eventHandler init !!!!");

	buzzerInit(&htim2, TIM_CHANNEL_1);
	osDelay(500);

	humidifierOn();

	ring_buffer_init(&ring_buffer);

	osTimerStart(updateSensorTimerHandle, 500);

	for (;;)
	{
		for (cnt = 0; ring_buffer_dequeue(&ring_buffer, &index) > 0; cnt++)
		{
			int32_t evn = (int32_t)index;
			snprintf(debug, sizeof(debug), "dev:[%lu]\r\n", evn);
			//DEBUG_PRINT(debug);
			switch (index)
			{
				case HUM_ON:
					humidifierOn();
					break;

				case HUM_OFF:
					humidifierOff();
					break;

				case UPDATE_SENSOR_VALUE:
					sensorEventHandling();
					break;

				case UART_RECEIVE:
					receiveHandlling();
					break;

				default :
					break;
			}
		}
	    osDelay(1);
	}
}
