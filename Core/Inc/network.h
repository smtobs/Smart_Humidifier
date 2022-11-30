/*
 * network.h
 *
 *  Created on: 2022. 10. 28.
 *      Author: bsoh
 */

#ifndef INC_NETWORK_H_
#define INC_NETWORK_H_

#include "wifi.h"
#define SOCK_STREAM   1
#define SOCK_DGRAM    2

typedef enum _NETWORK_IFACE
{
	WIFI_IFACE,
}NETWORK_IFACE;

typedef struct _socketInfo
{
	NETWORK_IFACE iface;
	uint8_t socket;
}socketInfo;

uint8_t socket(int, NETWORK_IFACE, socketInfo *);
bool connect(socketInfo *, const char *, const char *);
bool send(socketInfo *, uint8_t *, uint16_t);
int read(socketInfo *, unsigned char *, int);

#endif /* INC_NETWORK_H_ */
