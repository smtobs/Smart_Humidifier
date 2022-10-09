/*
 * event.h
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#ifndef INC_EVENT_H_
#define INC_EVENT_H_

#include "common.h"

#define BUZZER_ON                0x0001
#define BUZZER_OFF               0x0002
#define HUM_ON                   0x0004
#define HUM_OFF                  0x0008
#define UPDATE_SENSOR_VALUE      0x0010

typedef struct
{
	int temp;
	int humid;
}ENV_MSG;

#endif /* INC_EVENT_H_ */
