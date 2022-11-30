#ifndef	_WIFICONFIG_H
#define	_WIFICONFIG_H
#include "usart.h"

extern UART_HandleTypeDef huart5;

#define		_WIFI_USART                    huart5

#define		_WIFI_RX_SIZE                  512
#define		_WIFI_RX_FOR_DATA_SIZE         1024
#define		_WIFI_TX_SIZE                  256
#define		_WIFI_TASK_SIZE                512


#define		_WIFI_WAIT_TIME_LOW            5000
#define		_WIFI_WAIT_TIME_MED            10000
#define		_WIFI_WAIT_TIME_HIGH           25000
#define		_WIFI_WAIT_TIME_VERYHIGH       60000


#define USE_WIFI

#endif
