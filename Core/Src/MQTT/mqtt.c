#include "mqtt.h"


char rx_buffer[500],tx_buffer[150];

uint16_t ProtocolNameLength;
uint16_t ClientIDLength;

uint8_t connect = 0x10,publishCon = 0x30,subscribeCon = 0x82;
char *protocolName = "MQTT";
uint8_t level = 0x04;
uint8_t flag = 0x02;   // 02--> sifresiz
uint16_t keepAlive =60;
uint16_t packetID = 0x01;
uint8_t Qos = 0x00;
char *clientID1 = "Topuz1";
char *clientID2 = "Topuz2";

extern TIM_HandleTypeDef htim7;

void Wifi_Connect(char *SSID ,char *Password)
{
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),1000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CWQAP\r\n",strlen("AT+CWQAP\r\n"),1000);
	HAL_Delay(1000);
//	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+RST\r\n",strlen("AT+RST\r\n"),100);
//	HAL_Delay(5000);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,Password),1000);

}
#if (0)
void Connect_Broker(char *Ip ,char *Port)
{
	char msg[128] = {0,};

	snprintf(msg, sizeof(msg), "AT+MQTTCONN=%d,\"%s\",%d,%d\r\n", 0, "192.168.219.100", 1883, 1);
	HAL_UART_Transmit(&_WIFI_USART, (uint8_t *)msg, strlen(msg), 1000);

	HAL_TIM_Base_Start_IT(&htim7);
	osSemaphoreWait(WifiSemHandle, osWaitForever);
}
#elif (1)
void Connect_Broker(char *Ip ,char *Port)
{
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPMUX=0\r\n",strlen("AT+CIPMUX=0\r\n"),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",Ip,Port),5000);
	HAL_Delay(2000);

	//connect packet

	ProtocolNameLength = strlen(protocolName);
	ClientIDLength     = strlen(clientID1);
	uint8_t Remainlength;
	Remainlength = 2+ProtocolNameLength+6+ClientIDLength;
	uint16_t length = sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),(char)ProtocolNameLength,protocolName,(char)level,(char)flag,(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),(char)ClientIDLength,clientID1);

	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),(char)ProtocolNameLength,protocolName,(char)level,(char)flag,(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),(char)ClientIDLength,clientID1),5000);

	isConnectBroker = true;
}
#else
void Connect_Broker(char *Ip ,char *Port)
{
	uint8_t Remainlength;
	uint16_t length;

#if (0)
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPMUX=0\r\n",strlen("AT+CIPMUX=0\r\n"),1000);
	osDelay(100);
#else
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPMUX=1\r\n",strlen("AT+CIPMUX=1\r\n"),1000);
	osDelay(100);
#endif
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),1000);
	osDelay(100);

	//HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",Ip,Port),5000);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSTART=%d,\"TCP\",\"%s\",%s\r\n", 0, Ip, Port),5000);
	//sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n",LinkId,RemoteIp,RemotePort,TimeOut);
	osDelay(2000);

	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSTART=%d,\"TCP\",\"%s\",%s\r\n", 1, Ip, Port),5000);
	osDelay(2000);
	//connect packet

	ProtocolNameLength = strlen(protocolName);
	ClientIDLength     = strlen(clientID1);

	Remainlength = 2+ProtocolNameLength+6+ClientIDLength;
	length = sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",
												(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),
												(char)ProtocolNameLength,protocolName,(char)level,(char)flag,
												(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),
												(char)ClientIDLength,clientID1);

	//HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),1000);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d,%d\r\n", 0, length),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",
												(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),
												(char)ProtocolNameLength,protocolName,(char)level,(char)flag,
												(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),
												(char)ClientIDLength,clientID1),5000);
	osDelay(1000);


	ProtocolNameLength = strlen(protocolName);
	ClientIDLength     = strlen(clientID2);

	Remainlength = 2+ProtocolNameLength+6+ClientIDLength;
	length = sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",
												(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),
												(char)ProtocolNameLength,protocolName,(char)level,(char)flag,
												(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),
												(char)ClientIDLength,clientID2);

	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d,%d\r\n", 1, length),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",
											(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),
											(char)ProtocolNameLength,protocolName,(char)level,(char)flag,
											(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),
											(char)ClientIDLength,clientID2),5000);
	osDelay(1000);


	isConnectBroker = true;
}
#endif
void Subscribe(char *topic)
{
	uint16_t TopicLength = strlen(topic);
	uint8_t RemainLength = 2+2+TopicLength+1; // packetIDlength(2) + topiclengthdata(2)+topiclength+Qos
	uint16_t length = sprintf(tx_buffer,"%c%c%c%c%c%c%s%c",(char)subscribeCon,(char)RemainLength,(char)(packetID << 8),(char)packetID,(char)(TopicLength << 8),(char)TopicLength,topic,(char)Qos);



	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),1000);
	//HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d,%d\r\n", 0, length),1000);
	osDelay(100);
	//HAL_TIM_Base_Start_IT(&htim7);
	//osSemaphoreWait(WifiSemHandle, osWaitForever);
	//Wifi_RxClear();

	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%c%c%s%c",(char)subscribeCon,(char)RemainLength,(char)(packetID << 8),(char)packetID,(char)(TopicLength << 8),(char)TopicLength,topic,(char)Qos),5000);

	//Wifi_RxClear();
	//HAL_TIM_Base_Start_IT(&htim7);
	//osSemaphoreWait(WifiSemHandle, osWaitForever);

}

#if (0)
void publish(char *topic, char *message)
{

	uint16_t topiclength = strlen(topic);
	uint8_t remainlength = 2+topiclength+strlen(message);
	int length = sprintf(tx_buffer,"%c%c%c%c%s%s",(char)publishCon,(char)remainlength,(char)(topiclength << 8),(char)topiclength,topic,message);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),100);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%s%s",(char)publishCon,(char)remainlength,(char)(topiclength << 8),(char)topiclength,topic,message),5000);
	osDelay(100);
}
#elif (1)
void publish(char *topic, char *message)
{

	if (!message || !(*message))
	{
		return;
	}
	uint16_t topiclength = strlen(topic);
	uint8_t remainlength = 2+topiclength+strlen(message);

	int length = sprintf(tx_buffer,"%c%c%c%c%s%s",(char)publishCon,(char)remainlength,(char)(topiclength << 8),(char)topiclength,topic,message);

	Wifi_RxClear();
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),100);
	//HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d,%d\r\n",0, length),100);
	HAL_TIM_Base_Start_IT(&htim7);
	osSemaphoreWait(WifiSemHandle, osWaitForever);

	Wifi_RxClear();
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%s%s",(char)publishCon,(char)remainlength,(char)(topiclength << 8),(char)topiclength,topic,message),5000);
	HAL_TIM_Base_Start_IT(&htim7);
	osSemaphoreWait(WifiSemHandle, osWaitForever);

}
#else
void publish(char *topic, char *message)
{
	int LinkId=0; // Todo

	//osSemaphoreWait(WifiSemHandle, osWaitForever);

	uint16_t topiclength = strlen(topic);
	uint8_t remainlength = 2+topiclength+strlen(message);

	memset(tx_buffer, 0x0, sizeof(tx_buffer));

	int length = sprintf(tx_buffer,"%c%c%c%c%s%s", (char)publishCon, (char)remainlength, (char)(topiclength << 8), (char)topiclength,topic ,message);

	do
	{
		Wifi_RxClear();

		if (Wifi.TcpIpMultiConnection == false)
		{
			 sprintf((char*)Wifi.TxBuffer, "AT+CIPSEND=%d\r\n", length);
		}
		else
		{
			sprintf((char*)Wifi.TxBuffer,"AT+CIPSEND=%d,%d\r\n", LinkId, length);
		}

		if (Wifi_SendString((char*)Wifi.TxBuffer) == false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"TxBuffer)==false\r\n", strlen("TxBuffer)==false\r\n"), 1000);
			break;
		}

		HAL_TIM_Base_Start_IT(&htim7);
		osSemaphoreWait(WifiSemHandle, osWaitForever);

		Wifi_RxClear();
		Wifi_SendRaw((uint8_t*)tx_buffer, length);

		//Wifi_SendRaw((uint8_t*)tx_buffer, sprintf(tx_buffer,"%c%c%c%c%s%s",
			//								(char)publishCon, (char)remainlength, (char)(topiclength << 8), (char)topiclength, topic, message));
		//Wifi_SendRaw((uint8_t*)tx_buffer, sprintf(tx_buffer,"%c%c%c%c%s%s",
			//								(char)publishCon, (char)remainlength, (char)(topiclength << 8), (char)topiclength, topic, message));

		/* CallBack... */
		HAL_TIM_Base_Start_IT(&htim7);
		osSemaphoreWait(WifiSemHandle, osWaitForever);

	} while (0);
}

#endif



#if (0)

ool  Wifi_TcpIp_SendDataTcp(uint8_t LinkId,uint16_t dataLen,uint8_t *data)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		if(Wifi.TcpIpMultiConnection==false)
		{
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSENDBUF=%d\r\n",dataLen);
			//sprintf((char*)Wifi.TxBuffer,"AT+CIPSEND=%d\r\n",dataLen);
		}
		else
		{
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSENDBUF=%d,%d\r\n",LinkId,dataLen);
			//sprintf((char*)Wifi.TxBuffer,"AT+CIPSEND=%d,%d\r\n",LinkId,dataLen);
		}

		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"TxBuffer)==false\r\n", strlen("TxBuffer)==false\r\n"), 1000);
			break;
		}

		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"OK ERROR\r\n", strlen("OK ERROR\r\n"), 1000);
			break;
		}

		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,3,">","ERROR","busy")==false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"ERROR busy\r\n", strlen("ERROR busy\r\n"), 1000);
			break;
		}

#if (0)
		if(result > 1)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"result\r\n", strlen("result\r\n"), 1000);
			break;
		}
#endif
		Wifi_RxClear();
		Wifi_SendRaw(data,dataLen);

		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"Wifi_SendRaw result ERROR\r\n", strlen("Wifi_SendRaw result ERROR\r\n"), 1000);
			break;
		}
		returnVal=true;
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
#endif
