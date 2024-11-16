#ifndef _INC_MPU_NET_API_H_
#define _INC_MPU_NET_API_H_

int NET_Init();

int MPU_NET_SetIPAddress(const char *pszIPAddress);

char *MPU_NET_GetIPAddress(char *pszIPAddress);

int MPU_NET_SetGateway(const char *pszGateway);

char *MPU_NET_GetGateway(char *pszGateway);

int MPU_NET_SetSubnetMask(const char *pszSubnetMask);

char *MPU_NET_GetSubnetMask(char *pszSubnetMask);

char *MPU_NET_GetMacAddress(char *pszMacAddress);

int MPU_NET_SetDns(char *strDns0, char *strDns1);

int MPU_NET_GetDns(char *strDns0, char *strDns1);

int MPU_NET_GetDhcpEnable();


#endif