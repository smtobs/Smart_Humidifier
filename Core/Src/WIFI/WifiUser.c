
#include "Wifi.h"

//#######################################################################################
void Wifi_UserInit(void)
{
	Wifi_SetMode(WifiMode_Station);
  //Wifi_SetMode(WifiMode_SoftAp); 

  //while (Wifi_Station_ConnectToAp("L70_9587","33559366",NULL) == false);
	while (Wifi_Station_ConnectToAp("U+NetC568","P91352@4BB",NULL) == false);
	//while (Wifi_Station_ConnectToAp("bsoh123","123456789as",NULL) == false);
 
    
}
//#######################################################################################
void  Wifi_UserProcess(void)
{
#if (1)
  static uint8_t last=0;
  if(strstr(Wifi.MyIP,"0.0.0.0")!=NULL)
  {    
    if(last!=1)
	{
		
	}
    last=1;
  }
  else
  {   
    if(last!=2)
    {
      Wifi_TcpIp_StartTcpConnection(0,Wifi.MyGateWay,34500,10);
    	//Wifi_TcpIp_StartTcpConnection(0, "192.168.219.103", 34500, 10);
    }
    last=2;
  }
#else
  Wifi_TcpIp_StartTcpConnection(0, "192.168.219.103", 34500, 10);
#endif
}
//#######################################################################################
void  Wifi_UserGetUdpData(uint8_t LinkId,uint16_t DataLen,uint8_t *Data)
{
  Wifi_TcpIp_SendDataUdp(LinkId,2,(uint8_t*)"OK");
}
//#######################################################################################
void  Wifi_UserGetTcpData(uint8_t LinkId,uint16_t DataLen,uint8_t *Data)
{
  Wifi_TcpIp_SendDataTcp(LinkId,2,(uint8_t*)"OK");
}
//#######################################################################################
