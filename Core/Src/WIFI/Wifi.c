#include "Wifi.h"
#include "WifiConfig.h"
#include "mqtt.h"
#include "common.h"
#include "network.h"
#include "event.h"

extern s_mqtt mqttPublish;
extern s_mqtt mqttSubscribe;

extern osEventFlagsId_t eventFlagHandle;

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
	for(uint32_t t=0 ; t<TimeOut_ms ; t+=1)
	{
		osDelay(1);
		for(uint8_t	mx=0 ; mx<CountOfParameter ; mx++)
		{
			if(strstr((char*)Wifi.RxBuffer, arg[mx])!=NULL)
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
void Wifi_RxClear()
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
    	//ring_buffer_queue(&Wifi.wifi_ring_buffer[Wifi.RxDataConnectionNumber], (char)(Wifi.usartBuff));
    	//Wifi.RxBufferForData[Wifi.RxIndexForData] = Wifi.usartBuff;

        if(Wifi.RxIndexForData < _WIFI_RX_FOR_DATA_SIZE)
        {
          ring_buffer_queue(&Wifi.wifi_ring_buffer[0], Wifi.usartBuff);
          Wifi.RxIndexForData++;
        }
        if( Wifi.RxIndexForData>= Wifi.RxDataLen)
        {
          Wifi.RxIsData=false;
          Wifi.GotNewData=true;

          if (isConnectBroker == true)
          {
        	  osEventFlagsSet(eventFlagHandle, UART_RECEIVE);
          }
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

void WifiInit()
{
	//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, SET);
	osDelay(1000);

	HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);

	for (int i=0; i<3; i++)
	{
		ring_buffer_init((&Wifi.wifi_ring_buffer[i]));
	}
	Wifi_TxClear();

	DEBUG_PRINT("Start WiFi Task !");

	if (mqttInit() == false)
	{
		DEBUG_PRINT("mqtt INIT Fail!!!");
	}

	Wifi_SendString("AT\r\n");
 	Wifi_SetRfPower(82);

    Wifi_TcpIp_GetMultiConnection();
    Wifi_TcpIp_Close(0);
    Wifi_TcpIp_Close(1);
    Wifi_TcpIp_Close(2);
    Wifi_TcpIp_Close(3);
    Wifi_TcpIp_Close(4);
    Wifi_TcpIp_SetMultiConnection(true);
    DEBUG_PRINT("MultiConnection true !");

    //HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, SET);
	Wifi_GetMode();
	DEBUG_PRINT("Get WiFi..");

	Wifi_Station_DhcpIsEnable();
	DEBUG_PRINT("DhcpIsEnable..");

	Wifi_UserInit();
	DEBUG_PRINT("Wi-Fi Connect");

	do { osDelay(1); } while (connectBroker(&mqttPublish) == CONNECT_FAIL);
	DEBUG_PRINT("mqttPublish Connect Broker !");

	do { osDelay(1); } while (subscribe(&mqttPublish) != true);
	DEBUG_PRINT("subscribe!");

	while (1)
	{
		osDelay(1);
	}
}

bool Wifi_SetRfPower(uint8_t Power_0_to_82)
{
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool Wifi_SetMode(WifiMode_t WifiMode_)
{
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
	return returnVal;		
}

//#########################################################################################################
bool Wifi_GetMode(void)
{
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;
}
//#########################################################################################################
bool Wifi_GetMyIp(void)
{	
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool	Wifi_Station_ConnectToAp(char *SSID,char *Pass,char *MAC)
{
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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

		if( result > 1)
		{
			break;
		}
		returnVal=true;	
	}while(0);

	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}

//#########################################################################################################
bool	Wifi_Station_DhcpEnable(bool Enable)
{         
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_Station_DhcpIsEnable(void)
{
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool	Wifi_Station_SetIp(char *IP,char *GateWay,char *NetMask)
{
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}

//#########################################################################################################
bool  Wifi_SoftAp_Create(char *SSID,char *password,uint8_t channel,WifiEncryptionType_t WifiEncryptionType,uint8_t MaxConnections_1_to_4,bool HiddenSSID)
{
  //osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		  
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool  Wifi_TcpIp_GetConnectionStatus(void)
{
	//osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################
bool  Wifi_TcpIp_Ping(char *PingTo)
{
 // osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}
//#########################################################################################################
bool  Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections)
{
 // osSemaphoreWait(WifiSemHandle,osWaitForever);
	uint8_t result;
	uint8_t txBuffer[128] = {0,};
	bool		returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)txBuffer,"AT+CIPMUX=%d\r\n",EnableMultiConnections);
		if (Wifi_SendString((char*)txBuffer) == false)
		{
			DEBUG_PRINT("false");
			break;
		}
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR") == false)
		{
			DEBUG_PRINT("Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2, OK,ERROR) == false");
			break;
		}
		Wifi.TcpIpMultiConnection=EnableMultiConnections;
		returnVal=true;	
	}while(0);
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################
bool  Wifi_TcpIp_GetMultiConnection(void)
{
  
 // osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;			
}
//#########################################################################################################

//#########################################################################################################
bool  Wifi_TcpIp_Close(uint8_t LinkId)
{
  //osSemaphoreWait(WifiSemHandle,osWaitForever);
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
	//osSemaphoreRelease(WifiSemHandle);
	return returnVal;		
}

bool WifiTcpIpTcpConnection(uint8_t socket, const char *ip, const char *port, uint16_t timeOut)
{
	char tx_buffer[128] = {0,};

	char dsg[128] = {0,};
	snprintf(dsg, sizeof(dsg), "WifiTcpIpTcpConnection[%d]", socket);

	DEBUG_PRINT(dsg);

	wifiRxClear(socket);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)"AT+CIPSERVER=1,%d\r\n",strlen("AT+CIPSERVER=1,%d\r\n"), 1000);
	osDelay(100);

	if (Wifi.TcpIpMultiConnection == false)
	{
		HAL_UART_Transmit(&_WIFI_USART, (uint8_t *)tx_buffer, sprintf(tx_buffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", ip, port), timeOut);
	}
	else
	{
		HAL_UART_Transmit(&_WIFI_USART, (uint8_t *)tx_buffer, sprintf(tx_buffer,"AT+CIPSTART=%d,\"TCP\",\"%s\",%s\r\n", socket, ip, port), timeOut);
	}
	wifiRxClear(socket);
	osDelay(2000);

	//DEBUG_PRINT(socket);
	return true;
}

bool WifiSend(uint8_t socket, uint16_t length, uint8_t *data)
{
	char tx_buffer[128] = {0,};

	wifiRxClear(socket);
	if (Wifi.TcpIpMultiConnection == false)
	{
		HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer, sprintf(tx_buffer,"AT+CIPSEND=%d\r\n", length), 1000);
	}
	else
	{
		HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)tx_buffer, sprintf(tx_buffer,"AT+CIPSEND=%d,%d\r\n", socket, length), 1000);
	}
	osDelay(20);

	wifiRxClear(socket);
	HAL_UART_Transmit(&_WIFI_USART,(uint8_t *)data, length, 1000);

	return true;
}

void wifiRxClear(uint8_t socket)
{
	memset(Wifi.RxBuffer,0,_WIFI_RX_SIZE);
	Wifi.RxIndex=0;

	ring_buffer_init(&Wifi.wifi_ring_buffer[0]);
	ring_buffer_init(&Wifi.wifi_ring_buffer[socket]);
	HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart5.Instance)
    {
    	Wifi_RxCallBack();
    }
}
//#########################################################################################################
