/*
 * mqtt.h
 *
 *  Created on: 2022. 9. 8.
 *      Author: bsoh
 */

#ifndef INC_MQTT_H_
#define INC_MQTT_H_

#include "WifiConfig.h"
#include "Wifi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "main.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "MQTTPacket.h"
#include "network.h"

//extern socketInfo network;

typedef struct
{
	unsigned char msg[128];
	int length;
	int status;
	char *subTopic;
	char *pubTopic;
	MQTTString topicString;
	socketInfo network;
	int (*getfp)(uint8_t *, int);
}s_mqtt;

#define HOST_IP              "192.168.162.227"
#define HOST_PORT            "1883"
#define CONNECT_FAIL         0
#define CONNECT_SUCCESS      1

bool mqttInit();
int connectBroker(s_mqtt *);
void publish(char *);
int subscribe(s_mqtt *);
enum msgTypes PacketProcessing(s_mqtt *mqtt);

bool isConnectBroker;

#endif /* INC_MQTT_H_ */
