/*
 * dht11.c
 *
 *  Created on: Sep 29, 2022
 *      Author: bsoh
 */

#include "dht11.h"

__STATIC_INLINE void polDelayUs(volatile uint32_t microseconds)
{
	/* Go to number of cycles for system */
	microseconds *= (SystemCoreClock / 10000000);

	/* Delay till end */
	while (microseconds--);
}

void setPinOutput (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void setPinInput (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*********************************** DHT11 FUNCTIONS ********************************************/
void dht11Init(DHT11_Dev *dev)
{
	dev->port = DHT11_GPIO_Port;
	dev->pin = DHT11_Pin;
}

void dht11Start (DHT11_Dev *dev)
{
	setPinOutput (dev->port, dev->pin); // set the pin as output

	HAL_GPIO_WritePin (dev->port, dev->pin, GPIO_PIN_RESET);   // pull the pin low
	HAL_Delay(18);   // wait for 18ms

    HAL_GPIO_WritePin (dev->port, dev->pin, GPIO_PIN_SET);   // pull the pin high
	polDelayUs(20);   // wait for 20us

	setPinInput(dev->port, dev->pin);    // set as input
}

uint8_t dht11CheckResponse (DHT11_Dev *dev)
{
	uint8_t response = 0;
	polDelayUs(40);
	if (!(HAL_GPIO_ReadPin (dev->port, dev->pin)))
	{
		polDelayUs(80);
		if ((HAL_GPIO_ReadPin (dev->port, dev->pin))) response = DHT11_SUCCESS;
		else response = DHT11_ERROR_TIMEOUT; // 255
	}
	while ((HAL_GPIO_ReadPin (dev->port, dev->pin)));   // wait for the pin to go low

	return response;
}

uint8_t dht11Read (DHT11_Dev *dev)
{
	uint8_t i,j;
	int cnt = 0;

	for (j=0;j<8;j++)
	{
#if (0)
		while (!(HAL_GPIO_ReadPin (dev->port, dev->pin)))   // wait for the pin to go high
		{
			if (cnt >= 5000000)
			{
				return 255;
			}
			polDelayUs(1);
			cnt++;
		}
#else
		while (!(HAL_GPIO_ReadPin (dev->port, dev->pin)));   // wait for the pin to go high
#endif

		polDelayUs(40);   // wait for 40 us

		if (!(HAL_GPIO_ReadPin (dev->port, dev->pin)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else
		{
			i|= (1<<(7-j));  // if the pin is high, write 1
		}

		cnt=0;
#if (0)
		while ((HAL_GPIO_ReadPin (dev->port, dev->pin)))  // wait for the pin to go low
		{
			if (cnt >= 5000000)
			{
				return 255;
			}
			polDelayUs(1);
			cnt++;
		}
#else
		while ((HAL_GPIO_ReadPin (dev->port, dev->pin)));  // wait for the pin to go low
#endif
	}
	return i;
}
