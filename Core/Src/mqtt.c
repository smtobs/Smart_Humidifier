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
char *clientID = "Topuz";

void Connect_Broker(char *Ip ,char *Port)
{
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPMUX=0\r\n",strlen("AT+CIPMUX=0\r\n"),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",Ip,Port),5000);
	osDelay(2000);

	//connect packet

	ProtocolNameLength = strlen(protocolName);
	ClientIDLength     = strlen(clientID);
	uint8_t Remainlength;
	Remainlength = 2+ProtocolNameLength+6+ClientIDLength;
	uint16_t length = sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),(char)ProtocolNameLength,protocolName,(char)level,(char)flag,(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),(char)ClientIDLength,clientID);

	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%s%c%c%c%c%c%c%s",(char)connect,(char)Remainlength,(char)(ProtocolNameLength << 8),(char)ProtocolNameLength,protocolName,(char)level,(char)flag,(char)(keepAlive << 8),(char)keepAlive,(char)(ClientIDLength << 8),(char)ClientIDLength,clientID),5000);


}
void Subscribe(char *topic)
{
	uint16_t TopicLength = strlen(topic);
	uint8_t RemainLength = 2+2+TopicLength+1; // packetIDlength(2) + topiclengthdata(2)+topiclength+Qos
	uint16_t length = sprintf(tx_buffer,"%c%c%c%c%c%c%s%c",(char)subscribeCon,(char)RemainLength,(char)(packetID << 8),(char)packetID,(char)(TopicLength << 8),(char)TopicLength,topic,(char)Qos);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"AT+CIPSEND=%d\r\n",length),1000);
	osDelay(100);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer,sprintf(tx_buffer,"%c%c%c%c%c%c%s%c",(char)subscribeCon,(char)RemainLength,(char)(packetID << 8),(char)packetID,(char)(TopicLength << 8),(char)TopicLength,topic,(char)Qos),5000);

}
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
