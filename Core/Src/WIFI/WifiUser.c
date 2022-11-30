
#include "Wifi.h"

//#######################################################################################
void Wifi_UserInit(void)
{
	Wifi_SetMode(WifiMode_Station);
	while (Wifi_Station_ConnectToAp("bsoh123","123456789as",NULL) == false);
}
