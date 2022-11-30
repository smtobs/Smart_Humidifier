#include "mqtt.h"
#include "util.h"
#include "ringbuffer.h"
#include "event.h"
#include "network.h"

bool sendingPublisher;

s_mqtt mqttPublish, mqttSubscribe;

extern Wifi_t Wifi;
extern osEventFlagsId_t  eventFlagHandle;

int publish_transport_getdata(uint8_t *buf, int buflen)
{
#if (1)
	for (int i=0; i<buflen && (!ring_buffer_is_empty(&Wifi.wifi_ring_buffer[0])); i++)
	{
		ring_buffer_dequeue(&Wifi.wifi_ring_buffer[0], (unsigned char *)(&buf[i]));
	}
	return buflen;
#else
	return read(mqttPublish.network.socket, buf, buflen);
#endif
}

int subscribe_transport_getdata(uint8_t *buf, int buflen)
{
#if (1)
	for (int i=0; i<buflen && (!ring_buffer_is_empty(&Wifi.wifi_ring_buffer[0])); i++)
	{
		ring_buffer_dequeue(&Wifi.wifi_ring_buffer[0], (unsigned char *)(&buf[i]));
	}
	return buflen;
#else
	return read(mqttPublish.network.socket, buf, buflen);
#endif
}

bool mqttInit()
{
	mqttPublish.pubTopic = "/pub/smart/humidifier";
	mqttPublish.subTopic = "/sub/smart/humidifier";

	mqttSubscribe.pubTopic = "/pub/smart/humidifier";
	mqttSubscribe.subTopic = "/sub/smart/humidifier";

	if ((socket(SOCK_STREAM, WIFI_IFACE, &mqttPublish.network)) == FAIL)
	{
		DEBUG_PRINT("socket Fail !");
		return false;
	}
	wifiRxClear(mqttPublish.network.socket);

	mqttSubscribe.network.socket = mqttPublish.network.socket;
	mqttPublish.getfp   = publish_transport_getdata;
	mqttSubscribe.getfp = subscribe_transport_getdata;

	return true;
}

int connectBroker(s_mqtt *mqtt)
{
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	unsigned char buffer[128];
	int buflen = sizeof(buffer);
	int length;

	uint8_t responMsg = 0xff;
	uint8_t sessionPresent = 0;
	uint8_t connack_rc = 0;

	connect(&mqtt->network, HOST_IP, HOST_PORT);

	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = "Topuz1";
	connectData.keepAliveInterval = 60 * 2;

	length = MQTTSerialize_connect(buffer, buflen, &connectData);

	send(&mqtt->network, buffer, length);
	osDelay(100);

	//responMsg = MQTTPacket_read(buffer, length, mqtt->getfp);
	responMsg = MQTTPacket_read(buffer, length, publish_transport_getdata);

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
		DEBUG_PRINT("mqttMessageHandling [RUN] !");
		osEventFlagsSet(eventFlagHandle, HUM_ON);

	}
	else if (!strncmp(message, "STOP", sizeof("STOP")))
	{
		DEBUG_PRINT("mqttMessageHandling [STOP] !");
		osEventFlagsSet(eventFlagHandle, HUM_OFF);
	}
}

enum msgTypes PacketProcessing(s_mqtt *mqtt)
{
	unsigned char buffer[128];
	int buflen = sizeof(buffer);
	int messageType;

	unsigned char retained, dup;
	unsigned char* payload_in;
	static unsigned short msgid=1;
	int payloadlen_in, qos=0, subcount, len;
	MQTTString receivedTopic;

	messageType = MQTTPacket_read(buffer, buflen, subscribe_transport_getdata);
	switch (messageType)
	{
		case SUBACK:
			MQTTDeserialize_suback(&msgid, 1, &subcount, &qos, buffer, buflen);
			wifiRxClear(mqtt->network.socket);

			DEBUG_PRINT("SUBACK");
			break;

		case PUBLISH:
			DEBUG_PRINT("PUBLISH");
			if (MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buffer, buflen) == 1)
			{
				mqttMessageHandling(payload_in, payloadlen_in);

				wifiRxClear(mqtt->network.socket);
				if ((len = MQTTSerialize_ack(buffer, buflen, PUBACK, 0, msgid)) != 0)
				{
					send(&mqtt->network, buffer, len);
				}
			}
			break;

		default :
			//DEBUG_PRINT("default");
			break;
	}
	return messageType;
}

int subscribe(s_mqtt *mqtt)
{
	int req_qos = 0, len;
	unsigned char buffer[128];
	MQTTString topic = MQTTString_initializer;
	topic.cstring = mqttSubscribe.subTopic;

	len = MQTTSerialize_subscribe(buffer, sizeof(buffer), 0, 1, 1, &topic, &req_qos);
	if (len <= 0)
	{
		DEBUG_PRINT("len <= 0");
		return false;
	}
	send(&mqtt->network, (uint8_t *)buffer, len);

	return true;
}

void publish(char *message)
{
	if (!message || !(*message))
	{
		DEBUG_PRINT("publish message NULL");
		return;
	}
	memset(mqttPublish.msg, 0x0, sizeof(mqttPublish.msg));

	mqttPublish.topicString.cstring = mqttPublish.pubTopic;
	mqttPublish.length = MQTTSerialize_publish(mqttPublish.msg, sizeof(mqttPublish.msg), 0, 0, 0, 0,
											   mqttPublish.topicString, (unsigned char *)message, strlen(message));

	sendingPublisher = true;

	send(&mqttPublish.network, mqttPublish.msg, mqttPublish.length);
	return;
}
