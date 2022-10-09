#include "Wifi.h"
#include "WifiConfig.h"
//#include "MQTTPacket.h"
//#include "mqttclient.h"
#include "mqtt.h"
#include "common.h"

Wifi_t	Wifi;

extern char rx_buffer[500];

//#########################################################################################################
bool Wifi_SendRaw(uint8_t *data,uint16_t len)
{
	if(len <= _WIFI_TX_SIZE)
	{
		memcpy(Wifi.TxBuffer,data,len);
		if(HAL_UART_Transmit(&_WIFI_USART,data,len,100) == HAL_OK)
			return true;
		else
			return false;
	}
	else
		return false;
}

bool Wifi_SendString(char *data)
{
	return Wifi_SendRaw((uint8_t*)data,strlen(data));
}

bool Wifi_SendStringAndWait(char *data,uint16_t DelayMs)
{
	if(Wifi_SendRaw((uint8_t*)data,strlen(data))==false)
		return false;
	osDelay(DelayMs);
	return true;
}

bool Wifi_WaitForString(uint32_t TimeOut_ms,uint8_t *result,uint8_t CountOfParameter,...)
{
	
	if(result == NULL)
		return false;
	if(CountOfParameter == 0)
		return false;

	*result=0;

  va_list tag;
	va_start (tag,CountOfParameter);
	char *arg[CountOfParameter];
	for(uint8_t i=0; i<CountOfParameter ; i++)
		arg[i] = va_arg (tag, char *);	
  va_end (tag);
	
		
	//////////////////////////////////	
	for(uint32_t t=0 ; t<TimeOut_ms ; t+=50)
	{
		osDelay(50);
		for(uint8_t	mx=0 ; mx<CountOfParameter ; mx++)
		{
			if(strstr((char*)Wifi.RxBuffer,arg[mx])!=NULL)
			{
				*result = mx+1;
				return true;
			}				
		}				
	}
	// timeout
	return false;
	
}
//#########################################################################################################
bool Wifi_ReturnString(char *result,uint8_t WantWhichOne,char *SplitterChars)
{
	if(result == NULL) 
		return false;
	if(WantWhichOne==0)
		return false;

	char *str = (char*)Wifi.RxBuffer;
	

	str = strtok (str,SplitterChars);
	if(str == NULL)
	{
		strcpy(result,"");
		return false;
	}
	while (str != NULL)
  {
    str = strtok (NULL,SplitterChars);
		if(str != NULL)
			WantWhichOne--;
		if(WantWhichOne==0)
		{
			strcpy(result,str);
			return true;
		}
  }
	strcpy(result,"");
	return false;	
}

//#########################################################################################################
bool Wifi_ReturnStrings(char *InputString,char *SplitterChars,uint8_t CountOfParameter,...)
{
	if(CountOfParameter == 0)
		return false;
	va_list tag;
	va_start (tag,CountOfParameter);
	char *arg[CountOfParameter];
	for(uint8_t i=0; i<CountOfParameter ; i++)
		arg[i] = va_arg (tag, char *);	
  va_end (tag);
	
	char *str;
	str = strtok (InputString,SplitterChars);
	if(str == NULL)
		return false;
	uint8_t i=0;
	while (str != NULL)
  {
    str = strtok (NULL,SplitterChars);
		if(str != NULL)
			CountOfParameter--;
		strcpy(arg[i],str);
		i++;
		if(CountOfParameter==0)
		{
			return true;
		}
  }
	return false;	
	
}
//#########################################################################################################
bool	Wifi_ReturnInteger(int32_t	*result,uint8_t WantWhichOne,char *SplitterChars)
{
	if((char*)Wifi.RxBuffer == NULL)
		return false;
	if(Wifi_ReturnString((char*)Wifi.RxBuffer,WantWhichOne,SplitterChars)==false)
		return false;
	*result = atoi((char*)Wifi.RxBuffer);
	return true;
}
//#########################################################################################################

bool	Wifi_ReturnFloat(float	*result,uint8_t WantWhichOne,char *SplitterChars)
{	
	if((char*)Wifi.RxBuffer == NULL)
		return false;
	if(Wifi_ReturnString((char*)Wifi.RxBuffer,WantWhichOne,SplitterChars)==false)
		return false;
	*result = atof((char*)Wifi.RxBuffer);
	return true;
}
//#########################################################################################################
void Wifi_RemoveChar(char *str, char garbage)
{
	char *src, *dst;
	for (src = dst = str; *src != '\0'; src++)
	{
		*dst = *src;
		if (*dst != garbage)
			dst++;
	}
	*dst = '\0';
}
//#########################################################################################################
void Wifi_RxClear(void)
{
	memset(Wifi.RxBuffer,0,_WIFI_RX_SIZE);
	Wifi.RxIndex=0;	
  HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);
}
//#########################################################################################################
void Wifi_TxClear(void)
{
	memset(Wifi.TxBuffer,0,_WIFI_TX_SIZE);
}
//#########################################################################################################
void Wifi_RxCallBack(void)
{
  //+++ at command buffer
  if(Wifi.RxIsData==false)                                              
  {
    Wifi.RxBuffer[Wifi.RxIndex] = Wifi.usartBuff;
    if(Wifi.RxIndex < _WIFI_RX_SIZE)
      Wifi.RxIndex++;
  }
  //--- at command buffer
  //+++  data buffer
  else                                                                  
  {
    if( HAL_GetTick()-Wifi.RxDataLastTime > 50)
      Wifi.RxIsData=false;
    //+++ Calculate Data len after +IPD
    if(Wifi.RxDataLen==0)
    {
      //+++ Calculate Data len after +IPD ++++++ Multi Connection OFF
      if (Wifi.TcpIpMultiConnection==false)
      {
        Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp] = Wifi.usartBuff;
        Wifi.RxIndexForDataTmp++;
        if(Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp-1]==':')
        {
          Wifi.RxDataConnectionNumber=0;
          Wifi.RxDataLen=atoi((char*)&Wifi.RxBufferForDataTmp[1]);
        }
      }
      //--- Calculate Data len after +IPD ++++++ Multi Connection OFF
      //+++ Calculate Data len after +IPD ++++++ Multi Connection ON
      else
      {
        Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp] = Wifi.usartBuff;
        Wifi.RxIndexForDataTmp++;
        if(Wifi.RxBufferForDataTmp[2]==',')
        {
          Wifi.RxDataConnectionNumber=Wifi.RxBufferForDataTmp[1]-48;
        }
        if((Wifi.RxIndexForDataTmp>3) && (Wifi.RxBufferForDataTmp[Wifi.RxIndexForDataTmp-1]==':'))
          Wifi.RxDataLen=atoi((char*)&Wifi.RxBufferForDataTmp[3]);
      }
      //--- Calculate Data len after +IPD ++++++ Multi Connection ON
    }
    //--- Calculate Data len after +IPD
    //+++ Fill Data Buffer
    else  
    {      
      Wifi.RxBufferForData[Wifi.RxIndexForData] = Wifi.usartBuff;
      if(Wifi.RxIndexForData < _WIFI_RX_FOR_DATA_SIZE)
        Wifi.RxIndexForData++;
      if( Wifi.RxIndexForData>= Wifi.RxDataLen)
      {
        Wifi.RxIsData=false;         
        Wifi.GotNewData=true;
      }
    }
    //--- Fill Data Buffer    
  }           
  //--- data buffer
	HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);
  //+++ check +IPD in At command buffer
  if(Wifi.RxIndex>4)
  {
    if( (Wifi.RxBuffer[Wifi.RxIndex-4]=='+') && (Wifi.RxBuffer[Wifi.RxIndex-3]=='I') && (Wifi.RxBuffer[Wifi.RxIndex-2]=='P') && (Wifi.RxBuffer[Wifi.RxIndex-1]=='D'))
    {
      memset(Wifi.RxBufferForDataTmp,0,sizeof(Wifi.RxBufferForDataTmp));
      Wifi.RxBuffer[Wifi.RxIndex-4]=0;
      Wifi.RxBuffer[Wifi.RxIndex-3]=0;
      Wifi.RxBuffer[Wifi.RxIndex-2]=0;
      Wifi.RxBuffer[Wifi.RxIndex-1]=0;
      Wifi.RxIndex-=4;
      Wifi.RxIndexForData=0;
      Wifi.RxIndexForDataTmp=0;
      Wifi.RxIsData=true;
      Wifi.RxDataLen=0;  
      Wifi.RxDataLastTime = HAL_GetTick();      
    }
  }
  //--- check +IPD in At command buffer  
}

#define CONNECTION_KEEPALIVE_S 60UL
#define KEEPALIVE_INTERVAL 20

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
extern int32_t MQTT_Socket;

void WifiTask(void const * argument)
{
#if (1)


#if (1)
	DEBUG_PRINT("Start WiFi Task !");

	HAL_UART_Transmit(&huart1, (uint8_t *)"hello\r\n", (uint16_t)strlen("hello\r\n"), 1000);


	osDelay(1000);
	Wifi_SendString("AT\r\n");
 	Wifi_SetRfPower(82);


	HAL_UART_Transmit(&huart1, (uint8_t *)"hello1\r\n", (uint16_t)strlen("hello1\r\n"), 1000);


    Wifi_TcpIp_GetMultiConnection();
    Wifi_TcpIp_Close(0);
    Wifi_TcpIp_Close(1);
    Wifi_TcpIp_Close(2);
    Wifi_TcpIp_Close(3);
    Wifi_TcpIp_Close(4);
    Wifi_TcpIp_SetMultiConnection(true);

    HAL_UART_Transmit(&huart1, (uint8_t *)"hello2\r\n", (uint16_t)strlen("hello2\r\n"), 1000);

	Wifi_GetMode();

	HAL_UART_Transmit(&huart1, (uint8_t *)"hello3\r\n", (uint16_t)strlen("hello3\r\n"), 1000);


	Wifi_Station_DhcpIsEnable();

	HAL_UART_Transmit(&huart1, (uint8_t *)"hello4\r\n", (uint16_t)strlen("hello4\r\n"), 1000);

	Wifi_UserInit();

	HAL_UART_Transmit(&huart1, (uint8_t *)"hello5\r\n", (uint16_t)strlen("hello5\r\n"), 1000);
#endif

	//Wifi_TcpIp_StartTcpConnection(0, "192.168.219.103", 1883, 500);
	//HAL_UART_Receive_IT(&_WIFI_USART,(uint8_t *)rx_buffer,500);

	//Wifi_Connect("U+NetC568","P91352@4BB");
	//osDelay(5000);

	Connect_Broker("192.168.219.100", "1883");
	//osSemaphoreRelease(WifiSemHandle);
	HAL_UART_Transmit(&huart1, (uint8_t *)"Connect_Broker\r\n", (uint16_t)strlen("Connect_Broker\r\n"), 1000);

	osDelay(1000);
	Subscribe("demoTopic2017");

	while (1)
	{
		//memset(rx_buffer, 0x0, sizeof(rx_buffer));
		//HAL_UART_Receive_IT(&_WIFI_USART,(uint8_t *)rx_buffer,500);
		osDelay(1);
		//DEBUG_PRINT((char *)rx_buffer);
		//DEBUG_PRINT(Wifi.usartBuff);
	}

#else
	unsigned char buffer[128]={0,};
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	int len=0;
	int rc = 0;

	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	char* payload = "mypayload";
	int payloadlen = strlen(payload);

	osDelay(3000);
	//Wifi_SendStringAndWait("AT\r\n",1000);
	Wifi_SendString("AT\r\n");
 	Wifi_SetRfPower(82);
 	HAL_UART_Transmit(&huart1, (uint8_t *)"hllow\r\n", (uint16_t)strlen("hllow\r\n"), 1000);
    Wifi_TcpIp_GetMultiConnection();
    Wifi_TcpIp_Close(0);
    Wifi_TcpIp_Close(1);
    Wifi_TcpIp_Close(2);
    Wifi_TcpIp_Close(3);
    Wifi_TcpIp_Close(4);
    Wifi_TcpIp_SetMultiConnection(true);

	Wifi_GetMode();
	Wifi_Station_DhcpIsEnable();

	HAL_UART_Transmit(&huart1, (uint8_t *)"hllow444\r\n", (uint16_t)strlen("hllow444\r\n"), 1000);

	Wifi_UserInit();

	Wifi_TcpIp_StartTcpConnection(0, "192.168.219.103", 1883, 500);
	HAL_UART_Transmit(&huart1, (uint8_t *)"hllow555\r\n", (uint16_t)strlen("hllow555\r\n"), 1000);

	// Populate the connect struct.

	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = "TemperaturePublisher";
	connectData.keepAliveInterval = 60;

	len = MQTTSerialize_connect(buffer, sizeof(buffer), &connectData);

	Wifi_TcpIp_SendDataTcp(0, (uint16_t)len, (uint8_t *)buffer);
	HAL_Delay(10000);

	HAL_UART_Transmit(&huart1, (uint8_t *)"hllow666\r\n", (uint16_t)strlen("hllow666\r\n"), 1000);

	topicString.cstring = "test";
	len = MQTTSerialize_subscribe(buffer, sizeof(buffer), 0, msgid, 1, &topicString, &req_qos);
	Wifi_TcpIp_SendDataTcp(0, (uint16_t)len, (uint8_t *)buffer);
	HAL_Delay(10000);


#if (0)
	if (len > 0)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"hllow666\r\n", (uint16_t)strlen("hllow666\r\n"), 1000);
		HAL_UART_Transmit(&huart1, buffer, len, 1000);
		HAL_UART_Transmit(&huart1, (uint8_t *)"hllow666\r\n", (uint16_t)strlen("hllow666\r\n"), 1000);
	}
	else
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"hllow777\r\n", (uint16_t)strlen("hllow777\r\n"), 1000);
	}
#endif
	//Wifi_TcpIp_SendDataTcp(0, (uint16_t)strlen("Hi"), (uint8_t *)"Hi");
	//HAL_UART_Transmit(&huart1, (uint8_t *)"hllow666\r\n", (uint16_t)strlen("hllow666\r\n"), 1000);
#endif
	while (1)
	{

	}

	while(1)
	{	
		Wifi_GetMyIp();	
		if((Wifi.Mode==WifiMode_SoftAp) || (Wifi.Mode==WifiMode_StationAndSoftAp))
		{
			Wifi_SoftAp_GetConnectedDevices();
		}

		Wifi_TcpIp_GetConnectionStatus();
		Wifi_RxClear();

		for(uint8_t i=0; i< 100; i++)
		{
#if (1)
			for(uint8_t ii=0; ii<5 ; ii++)
				HAL_UART_Transmit(&huart1, (uint8_t *)Wifi.TcpIpConnections[ii].RemoteIp, (uint16_t)strlen(Wifi.TcpIpConnections[ii].RemoteIp), 1000);
#endif
		  if( Wifi.GotNewData==true)
		  {
			Wifi.GotNewData=false;
			for(uint8_t ii=0; ii<5 ; ii++)
			{
#if (1)
				char test[64] = {0,};

				sprintf(test, "Wifi.TcpIpConnections[%u].Type=[%s]\r\n", ii, Wifi.TcpIpConnections[ii].Type);
				HAL_UART_Transmit(&huart1, (uint8_t *)test, (uint16_t)strlen(test), 1000);
#endif
			  if((strstr(Wifi.TcpIpConnections[ii].Type,"UDP")!=NULL) && (Wifi.RxDataConnectionNumber==Wifi.TcpIpConnections[ii].LinkId))
				Wifi_UserGetUdpData(Wifi.RxDataConnectionNumber,Wifi.RxDataLen,Wifi.RxBufferForData);
			  if((strstr(Wifi.TcpIpConnections[ii].Type,"TCP")!=NULL) && (Wifi.RxDataConnectionNumber==Wifi.TcpIpConnections[ii].LinkId))
				Wifi_UserGetTcpData(Wifi.RxDataConnectionNumber,Wifi.RxDataLen,Wifi.RxBufferForData);
			}
		  }
		  osDelay(10);
		}
		HAL_UART_Transmit(&huart1, (uint8_t *)"userProcess\r\n", (uint16_t)strlen("userProcess\r\n"), 1000);
		Wifi_UserProcess();
	}
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void WifiInit(osPriority	Priority)
{
	HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);
	Wifi_RxClear();
	Wifi_TxClear();
	osSemaphoreDef(WifiSemHandle);
	WifiSemHandle = osSemaphoreCreate(osSemaphore(WifiSemHandle), 1);
	osThreadDef(WifiTaskName, WifiTask, Priority, 0, _WIFI_TASK_SIZE);
	WifiTaskHandle = osThreadCreate(osThread(WifiTaskName), NULL);	
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool	Wifi_Restart(void)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+RST\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;			
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
//#########################################################################################################
bool	Wifi_DeepSleep(uint16_t DelayMs)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+GSLP=%d\r\n",DelayMs);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;			
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_FactoryReset(void)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+RESTORE\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;			
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_Update(void)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIUPDATE\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(1000*60*5,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;			
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################
bool	Wifi_SetRfPower(uint8_t Power_0_to_82)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+RFPOWER=%d\r\n",Power_0_to_82);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;			
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool Wifi_SetMode(WifiMode_t	WifiMode_)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWMODE_CUR=%d\r\n",WifiMode_);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;			
		Wifi.Mode = WifiMode_;
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool Wifi_GetMode(void)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWMODE_CUR?\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;			
		if(Wifi_ReturnInteger((int32_t*)&result,1,":"))
			Wifi.Mode = (WifiMode_t)result ;
		else
			Wifi.Mode = WifiMode_Error;
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
//#########################################################################################################
bool Wifi_GetMyIp(void)
{	
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIFSR\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		sscanf((char*)Wifi.RxBuffer,"AT+CIFSR\r\r\n+CIFSR:APIP,\"%[^\"]",Wifi.MyIP);
    sscanf((char*)Wifi.RxBuffer,"AT+CIFSR\r\r\n+CIFSR:STAIP,\"%[^\"]",Wifi.MyIP);			
    
    
    Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPSTA?\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;	
    
    char *str=strstr((char*)Wifi.RxBuffer,"gateway:");
    if(str==NULL)
      break;
    if(Wifi_ReturnStrings(str,"\"",1,Wifi.MyGateWay)==false)
      break;    
    
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool	Wifi_Station_ConnectToAp(char *SSID,char *Pass,char *MAC)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		if(MAC==NULL)
		{
			sprintf((char*)Wifi.TxBuffer,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",SSID,Pass);
			//sprintf((char*)Wifi.TxBuffer,"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,Pass);
		}
		else
		{
			sprintf((char*)Wifi.TxBuffer,"AT+CWJAP_CUR=\"%s\",\"%s\",\"%s\"\r\n",SSID,Pass,MAC);
			//sprintf((char*)Wifi.TxBuffer,"AT+CWJAP=\"%s\",\"%s\",\"%s\"\r\n",SSID,Pass,MAC);
		}
		if (Wifi_SendString((char*)Wifi.TxBuffer)==false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"111\r\n", (uint16_t)strlen("111\r\n"), 1000);
			break;
		}
		if (Wifi_WaitForString(_WIFI_WAIT_TIME_VERYHIGH,&result,3,"\r\nOK\r\n","\r\nERROR\r\n","\r\nFAIL\r\n")==false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"ERROR!!!!!!!!!!!!!\r\n", (uint16_t)strlen("ERROR!!!!!!!!!!!!!\r\n"), 1000);
			break;
		}

#if (1)
		if( result > 1)
		{
			//HAL_UART_Transmit(&huart1, (uint8_t *)"222\r\n", (uint16_t)strlen("222\r\n"), 1000);
			break;
		}
#endif
		returnVal=true;	
	}while(0);

	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_Station_Disconnect(void)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWQAP\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;				
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_Station_DhcpEnable(bool Enable)
{         
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWDHCP_CUR=1,%d\r\n",Enable);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		Wifi.StationDhcp=Enable;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_Station_DhcpIsEnable(void)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWDHCP_CUR?\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		if(Wifi_ReturnInteger((int32_t*)&result,1,":")==false)
			break;
		switch(result)
		{
			case 0:
				Wifi.StationDhcp=false;
				Wifi.SoftApDhcp=false;				
			break;
			case 1:
				Wifi.StationDhcp=false;
				Wifi.SoftApDhcp=true;				
			break;
			case 2:
				Wifi.StationDhcp=true;
				Wifi.SoftApDhcp=false;				
			break;
			case 3:
				Wifi.StationDhcp=true;
				Wifi.SoftApDhcp=true;				
			break;			
		}
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_Station_SetIp(char *IP,char *GateWay,char *NetMask)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPSTA_CUR=\"%s\",\"%s\",\"%s\"\r\n",IP,GateWay,NetMask);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		Wifi.StationDhcp=false;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################

//#########################################################################################################

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool  Wifi_SoftAp_GetConnectedDevices(void)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWLIF\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		Wifi_RemoveChar((char*)Wifi.RxBuffer,'\r');
    Wifi_ReturnStrings((char*)Wifi.RxBuffer,"\n,",10,Wifi.SoftApConnectedDevicesIp[0],Wifi.SoftApConnectedDevicesMac[0],Wifi.SoftApConnectedDevicesIp[1],Wifi.SoftApConnectedDevicesMac[1],Wifi.SoftApConnectedDevicesIp[2],Wifi.SoftApConnectedDevicesMac[2],Wifi.SoftApConnectedDevicesIp[3],Wifi.SoftApConnectedDevicesMac[3],Wifi.SoftApConnectedDevicesIp[4],Wifi.SoftApConnectedDevicesMac[4]);
		for(uint8_t i=0 ; i<6 ; i++)
    {
      if( (Wifi.SoftApConnectedDevicesIp[i][0]<'0') || (Wifi.SoftApConnectedDevicesIp[i][0]>'9'))
        Wifi.SoftApConnectedDevicesIp[i][0]=0;      
      if( (Wifi.SoftApConnectedDevicesMac[i][0]<'0') || (Wifi.SoftApConnectedDevicesMac[i][0]>'9'))
        Wifi.SoftApConnectedDevicesMac[i][0]=0;      
    }
    
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################
bool  Wifi_SoftAp_Create(char *SSID,char *password,uint8_t channel,WifiEncryptionType_t WifiEncryptionType,uint8_t MaxConnections_1_to_4,bool HiddenSSID)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWSAP=\"%s\",\"%s\",%d,%d,%d,%d\r\n",SSID,password,channel,WifiEncryptionType,MaxConnections_1_to_4,HiddenSSID);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		  
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		  
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool  Wifi_TcpIp_GetConnectionStatus(void)
{
	osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPSTATUS\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		
		char *str = strstr((char*)Wifi.RxBuffer,"\nSTATUS:");
    if(str==NULL)
      break;
    str = strchr(str,':');
    str++;
    for(uint8_t i=0 ; i<5 ;i++)
      Wifi.TcpIpConnections[i].status=(WifiConnectionStatus_t)atoi(str);
    str = strstr((char*)Wifi.RxBuffer,"+CIPSTATUS:");
    for(uint8_t i=0 ; i<5 ;i++)
    {
      sscanf(str,"+CIPSTATUS:%d,\"%3s\",\"%[^\"]\",%d,%d,%d",(int*)&Wifi.TcpIpConnections[i].LinkId,Wifi.TcpIpConnections[i].Type,Wifi.TcpIpConnections[i].RemoteIp,(int*)&Wifi.TcpIpConnections[i].RemotePort,(int*)&Wifi.TcpIpConnections[i].LocalPort,(int*)&Wifi.TcpIpConnections[i].RunAsServer);
      str++;
      str = strstr(str,"+CIPSTATUS:");
      if(str==NULL)
        break;
    }
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################
bool  Wifi_TcpIp_Ping(char *PingTo)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+PING=\"%s\"\r\n",PingTo);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_MED,&result,3,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
    if(Wifi_ReturnInteger((int32_t*)&Wifi.TcpIpPingAnswer,2,"+")==false)
      break;    
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool  Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPMUX=%d\r\n",EnableMultiConnections);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;				
    Wifi.TcpIpMultiConnection=EnableMultiConnections;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################
bool  Wifi_TcpIp_GetMultiConnection(void)
{
  
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPMUX?\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;				
    if(Wifi_ReturnInteger((int32_t*)&result,1,":")==false)
      break;
    Wifi.TcpIpMultiConnection=(bool)result;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################
bool  Wifi_TcpIp_StartTcpConnection(uint8_t LinkId,char *RemoteIp,uint16_t RemotePort,uint16_t TimeOut)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPSERVER=1,%d\r\n",RemotePort);

		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		Wifi_RxClear();
    if(Wifi.TcpIpMultiConnection==false)
      sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n",RemoteIp,RemotePort,TimeOut);
    else
      sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n",LinkId,RemoteIp,RemotePort,TimeOut);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH,&result,3,"OK","CONNECT","ERROR")==false)
			break;
		if(result == 3)
			break;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool  Wifi_TcpIp_StartUdpConnection(uint8_t LinkId,char *RemoteIp,uint16_t RemotePort,uint16_t LocalPort)
{ 
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
    if(Wifi.TcpIpMultiConnection==false)
      sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=\"UDP\",\"%s\",%d,%d\r\n",RemoteIp,RemotePort,LocalPort);
    else
      sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=%d,\"UDP\",\"%s\",%d,%d\r\n",LinkId,RemoteIp,RemotePort,LocalPort);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH,&result,3,"OK","ALREADY","ERROR")==false)
			break;
		if(result == 3)
			break;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool  Wifi_TcpIp_Close(uint8_t LinkId)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
    if(Wifi.TcpIpMultiConnection==false)
      sprintf((char*)Wifi.TxBuffer,"AT+CIPCLOSE\r\n");
    else
      sprintf((char*)Wifi.TxBuffer,"AT+CIPCLOSE=%d\r\n",LinkId);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool  Wifi_TcpIp_SetEnableTcpServer(uint16_t PortNumber)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
    if(Wifi.TcpIpMultiConnection==false)
    {
      sprintf((char*)Wifi.TxBuffer,"AT+CIPMUX=1\r\n");
      if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
        break;
      if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
        break;    
      Wifi.TcpIpMultiConnection=true;
      Wifi_RxClear();      
    }
    else
      sprintf((char*)Wifi.TxBuffer,"AT+CIPSERVER=1,%d\r\n",PortNumber);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool  Wifi_TcpIp_SetDisableTcpServer(uint16_t PortNumber)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
    sprintf((char*)Wifi.TxBuffer,"AT+CIPSERVER=0,%d\r\n",PortNumber);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;		
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;	
}
//#########################################################################################################
bool  Wifi_TcpIp_SendDataUdp(uint8_t LinkId,uint16_t dataLen,uint8_t *data)
{
  osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
    if(Wifi.TcpIpMultiConnection==false)
      sprintf((char*)Wifi.TxBuffer,"AT+CIPSERVER=0\r\n");
    else
      sprintf((char*)Wifi.TxBuffer,"AT+CIPSEND=%d,%d\r\n",LinkId,dataLen);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,">","ERROR")==false)
			break;
		if(result == 2)
			break;		
    Wifi_RxClear();
    Wifi_SendRaw(data,dataLen);
    if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		returnVal=true;	
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;	
  
}
//#########################################################################################################
bool  Wifi_TcpIp_SendDataTcp(uint8_t LinkId,uint16_t dataLen,uint8_t *data)
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


bool Wifi_TcpIp_StartMqttConnection(uint8_t LinkId,uint16_t dataLen,uint8_t *data)
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

		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"CONNACK")==false)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"CONNACK ERROR\r\n", strlen("CONNACK ERROR\r\n"), 1000);
			break;
		}
		returnVal=true;
	}while(0);
	osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart3.Instance)
    {
    	Wifi_RxCallBack();
    }
}
//#########################################################################################################
