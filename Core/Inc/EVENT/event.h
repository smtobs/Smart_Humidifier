/*
 * event.h
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#ifndef INC_EVENT_H_
#define INC_EVENT_H_

#include "common.h"

//#define BUZZER_ON                0x0001
//#define BUZZER_OFF               0x0002
//#define HUM_ON                   0x0004
//#define HUM_OFF                  0x0008
//#define UPDATE_SENSOR_VALUE      0x0010

#define BUZZER_ON                1
#define BUZZER_OFF               2
#define HUM_ON                   3
#define HUM_OFF                  4
#define UPDATE_SENSOR_VALUE      5
#define UART_RECEIVE             6
#define WIFI_SEND_REP            7

typedef struct
{
	int temp;
	int humid;
}ENV_MSG;

typedef enum
{
	OFF,
	ON,
}PRODUCT_STATUS;

PRODUCT_STATUS          productStatus;

#endif /* INC_EVENT_H_ */
