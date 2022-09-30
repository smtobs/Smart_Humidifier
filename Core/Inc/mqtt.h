/*
 * mqtt.h
 *
 *  Created on: 2022. 9. 8.
 *      Author: v7dd1
 */

#ifndef INC_MQTT_H_
#define INC_MQTT_H_

#include "WifiConfig.h"
#include "Wifi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

//#include "usart.h"
//#include "gpio.h"
#include "main.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void Connect_Broker(char *Ip ,char *Port);
void Subscribe(char *topic);
void publish(char *topic, char *message);

#endif /* INC_MQTT_H_ */
