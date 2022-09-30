/*
 * event.h
 *
 *  Created on: Sep 30, 2022
 *      Author: bsoh
 */

#ifndef INC_EVENT_H_
#define INC_EVENT_H_

#include "common.h"

#define BUZZER_ON       0x0001
#define BUZZER_OFF      0x0002
#define HUM_ON          0x0004
#define HUM_OFF         0x0008

typedef enum
{
	E_BUZZER_ON,
	E_BUZZER_OFF,
	E_HUM_ON,
	E_HUM_OFF
}eventState;

typedef struct
{
	//uint8_t temp;
	//uint8_t humid;
	int temp;
	int humid;
}ENV_MSG;

//ENV_MSG *envData;

#endif /* INC_EVENT_H_ */
