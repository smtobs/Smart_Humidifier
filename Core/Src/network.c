/*
 * network.c
 *
 *  Created on: Sep 26, 2022
 *      Author: bsoh
 */
#include "network.h"
#include "ringbuffer.h"
#include "util.h"
#include "Wifi.h"

static uint8_t Fd = 1;
//static uint8_t tcpFd = 1;

extern Wifi_t Wifi;

uint8_t socket(int type, NETWORK_IFACE iface, socketInfo *info)
{
	uint8_t sockfd;

	/* Check TCP Or UDP */
	if (type == SOCK_STREAM)
	{
		sockfd = Fd++;
	}
	else if (type == SOCK_DGRAM)
	{
		sockfd = Fd++;
	}
	else
	{
		return FAIL;
	}

	/* 오버 플로우 체크 */
	if (sockfd >= 256)
	{
		return FAIL;
	}

	switch (iface)
	{
		case WIFI_IFACE:
			info->iface = WIFI_IFACE;
			info->socket = sockfd;
			break;

		default:
			return FAIL;
	}
	return SUCCESS;
}

bool connect(socketInfo *info, const char *ip, const char *port)
{
	const int timeout = 5000;

	switch (info->iface)
	{
		case WIFI_IFACE:
			return WifiTcpIpTcpConnection(info->socket, ip, port, timeout);

		default :
			break;
	}
	return false;
}

bool send(socketInfo *info, uint8_t *msg, uint16_t len)
{

	if (info->socket == 0)
	{
		return false;
	}
	else
	{
		switch (info->iface)
		{
			case WIFI_IFACE:
				return WifiSend(info->socket, len, msg);

			default :
				break;
		}
	}
	return false;
}

int read(socketInfo *info, unsigned char *buf, int nbytes)
{
	ring_buffer_t data;
//	if (sockfd >= )
//	{
//		return -1;
//	}
	memcpy(&data, &Wifi.wifi_ring_buffer[info->socket], sizeof(ring_buffer_t));
	for (int i=0; i<nbytes && (!ring_buffer_is_empty(&data)); i++)
	{
		ring_buffer_dequeue(&data, &buf[i]);
	}
	return nbytes;
}
