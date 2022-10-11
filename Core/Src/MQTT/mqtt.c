#include "mqtt.h"
#include "util.h"
#include "ringbuffer.h"
#include "event.h"


char *clientID1 = "Topuz1";

s_mqtt mqtt =
{
		.msg        = {0,},
		.status       = 0,
		.topicString  = MQTTString_initializer,
		.subTopic = "/sub/smart/humidifier",
		.pubTopic = "/pub/smart/humidifier",
};

extern TIM_HandleTypeDef     htim7;
extern ring_buffer_t         wifi_ring_buffer;
extern osSemaphoreId 	     workHandle;
extern osThreadId 		     eventLoopTaskHandle;

int transport_getdata(uint8_t *buf, int buflen)
{
	int i;

	for (i=0; i<buflen && (!ring_buffer_is_empty(&wifi_ring_buffer)); i++)
	{
		ring_buffer_dequeue(&wifi_ring_buffer, (char *)(&buf[i]));
	}

	return buflen;
}

int Connect_Broker()
{
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	unsigned char buffer[128];
	int buflen = sizeof(buffer);
	char tx_buffer[256] = {0,};
	int length;

	uint8_t responMsg = 0xff;
	uint8_t sessionPresent = 0;
	uint8_t connack_rc = 0;

	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPMUX=0\r\n",strlen("AT+CIPMUX=0\r\n"),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", HOST_IP, HOST_PORT),5000);
	HAL_Delay(2000);

	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = clientID1;
	connectData.keepAliveInterval = 60 * 2;

	length = MQTTSerialize_connect(buffer, buflen, &connectData);

	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),1000);
	HAL_Delay(100);

	Wifi_RxClear();
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)buffer, length, 1000);
	HAL_Delay(100);

	responMsg = MQTTPacket_read(buffer, length, transport_getdata);
	if ((responMsg != CONNACK) || (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buffer, length) != 1 || connack_rc != 0))
	{
		isConnectBroker = false;

		return CONNECT_FAIL;
	}
	isConnectBroker = true;
	return CONNECT_SUCCESS;
}

void mqttMessageHandling(const unsigned char *payLoad, const int payloadLen)
{
	char message[128]={0,};

	snprintf(message, sizeof(message), "%.*s", payloadLen, payLoad);

	if (!strncmp(message, "RUN", sizeof("RUN")))
	{
		DEBUG_PRINT("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		osSignalSet(eventLoopTaskHandle, BUZZER_ON | HUM_ON);
	}
	else if (!strncmp(message, "STOP", sizeof("STOP")))
	{
		DEBUG_PRINT("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		osSignalSet(eventLoopTaskHandle, BUZZER_OFF | HUM_OFF);
	}
	DEBUG_PRINT(message);
}

void PacketProcessing()
{
	unsigned char buffer[128];
	int buflen = sizeof(buffer);
	int messageType;

	while (true)
	{
		unsigned char retained, dup;
		unsigned char* payload_in;
		unsigned short msgid, submsgid;
		int payloadlen_in, qos, subcount;
		MQTTString receivedTopic;

		messageType = MQTTPacket_read(buffer, buflen, transport_getdata);
		switch (messageType)
		{
			case SUBACK:
				MQTTDeserialize_suback(&submsgid, 1, &subcount, &qos, buffer, buflen);
				Wifi_RxClear();
				break;

			case PUBLISH:
				if (MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
						&payload_in, &payloadlen_in, buffer, buflen) == 1)
				{
					mqttMessageHandling(payload_in, payloadlen_in);
					Wifi_RxClear();
				}
				return;

			default :
				Wifi_RxClear();
				return;
		}
		osSemaphoreRelease(workHandle);
		osDelay(1000);
	}
}

int awaitSubscribes()
{
	char tx_buffer[256] = {0,};
	int req_qos = 0, len;
	unsigned char buffer[128];
	uint32_t waitTime = 500;
	uint8_t atCommandResult;

	MQTTString topic = MQTTString_initializer;
	topic.cstring = mqtt.subTopic;

	do
	{
		osDelay(1);
	}while (mqtt.status != 0);

	len = MQTTSerialize_subscribe(buffer, sizeof(buffer), 0, 1, 1, &topic, &req_qos);
	if (len <= 0)
	{
		DEBUG_PRINT("len <= 0");
		goto end;
	}

	Wifi_RxClear();
	HAL_UART_Transmit(&_WIFI_USART, (uint8_t *)tx_buffer, sprintf(tx_buffer,"AT+CIPSEND=%d\r\n", len), 100);

	if (Wifi_WaitForString(waitTime, &atCommandResult, 2, "OK", "ERROR") == false)
	{
		DEBUG_PRINT("Time Out !");
		goto end;
	}

	Wifi_RxClear();
	HAL_UART_Transmit(&_WIFI_USART, (uint8_t *)buffer, len, 100);

	if (Wifi_WaitForString(waitTime, &atCommandResult, 2, "OK", "ERROR") == false)
	{
		DEBUG_PRINT("Time Out !");
		goto end;
	}
	osDelay(100);

	PacketProcessing();

	end :
	osSemaphoreRelease(workHandle);
	osDelay(1000);
	return 0;
}

void ayncPublish(char *message)
{
	if (mqtt.status == 0)
	{
		char tx_buffer[128] = {0,};

		if (!message || !(*message))
		{
			return;
		}
		memset(mqtt.msg, 0x0, sizeof(mqtt.msg));

		mqtt.topicString.cstring = mqtt.pubTopic;
		mqtt.length = MQTTSerialize_publish(mqtt.msg, sizeof(mqtt.msg), 0, 0, 0, 0,
				mqtt.topicString, (unsigned char *)message, strlen(message));

		if (osSemaphoreWait(workHandle, osWaitForever) == osOK)
		{
			mqtt.status = 1;

			Wifi_RxClear();
			HAL_UART_Transmit(&_WIFI_USART, (uint8_t *)tx_buffer, sprintf(tx_buffer,"AT+CIPSEND=%d\r\n", mqtt.length), 100);
			HAL_TIM_Base_Start_IT(&htim7);
		}
	}
	return;
}
