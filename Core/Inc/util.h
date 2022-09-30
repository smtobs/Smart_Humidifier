/*
 * util.h
 *
 *  Created on: 2022. 9. 11.
 *      Author: bsoh
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include "main.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;

#define MAX_DEBUG_BUFF   2048
#define DEBUG_UART       huart1

#define DEBUG_PRINT(X)   _debugPrint(X, __FILE__, __FUNCTION__, __LINE__)

void _debugPrint(char *, char *, const char*, int );

#endif /* INC_UTIL_H_ */
