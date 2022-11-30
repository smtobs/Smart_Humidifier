/*
 * util.c
 *
 *  Created on: 2022. 9. 11.
 *      Author: bsoh
 */

#include "util.h"

char debugBuff[MAX_DEBUG_BUFF] = {0,};

void _debugPrint(char *debugMsg, char *file, const char *func, int line)
{
	int n;

	n = snprintf(debugBuff, MAX_DEBUG_BUFF,
				//"File : %s \r\nFunction : %s \r\nLine : %d \r\nDebug : %s\r\n\r\n", file, func, line, debugMsg);
				"Debug : %s\r\n\r\n", debugMsg);
	if ( n < 0 || n >= MAX_DEBUG_BUFF )

	{
		HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)"debugPrint Error!!!\r\n", strlen("debugPrint Error!!!r\n"), 100);
	}
	else
	{
		HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)debugBuff, n, 100);
	}
	memset(debugBuff, 0x0, MAX_DEBUG_BUFF);
}


