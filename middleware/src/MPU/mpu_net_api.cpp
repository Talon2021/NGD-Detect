#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mpu_net_api.h"
#include "INet.h"
#include "sdk_log.h"

//调用网络管理器执行动作
#define MPU_NET_CALL(x)															\
do																				\
{																				\
	int nRet;																	\
																				\
	INet *pMgr = INet::GetInstance();											\
																				\
	if (NULL != pMgr)															\
	{																			\
		nRet = pMgr->x;															\
		if (0 != nRet)													        \
		{																		\
			ERROR("%s, nRet = %d\n", #x, nRet);									\
		}																		\
	}																			\
	else																		\
	{																			\
		ERROR("Get INet instance fail!\n");										\
		nRet = -1;									                            \
	}																			\
																				\
	return nRet;																\
}																				\
while (0)

#define MPU_NET_GET(DataType, DefaultValue, Function)							\
do																				\
{																				\
	DataType retValue;															\
																				\
	INet *pMgr = INet::GetInstance();											\
																				\
	if (NULL != pMgr)														    \
	{																			\
		retValue = pMgr->Function;												\
	}																			\
	else																		\
	{																			\
		ERROR("Get INet instance fail!\n");								        \
		retValue = DefaultValue;												\
	}																			\
																				\
	return retValue;															\
}																				\
while (0)

int NET_Init()
{
    int ret = 0;
    INet *pMgr = INet::GetInstance();
    if (NULL == pMgr)
	{
		ERROR("Get INet instance fail!\n");
		ret = -1;
	}
    return ret;
}

int MPU_NET_SetIPAddress(const char *pszIPAddress)
{
   MPU_NET_CALL(NET_SetIPAddress(pszIPAddress));
}

char *MPU_NET_GetIPAddress(char *pszIPAddress)
{
    MPU_NET_GET(char *, pszIPAddress, NET_GetIPAddress(pszIPAddress));
}

int MPU_NET_SetGateway(const char *pszGateway)
{
    MPU_NET_CALL(NET_SetGateway(pszGateway));
}

char *MPU_NET_GetGateway(char *pszGateway)
{
    MPU_NET_GET(char *, pszGateway, NET_GetGateway(pszGateway));
}

int MPU_NET_SetSubnetMask(const char *pszSubnetMask)
{
    MPU_NET_CALL(NET_SetSubnetMask(pszSubnetMask));
}

char *MPU_NET_GetSubnetMask(char *pszSubnetMask)
{
    MPU_NET_GET(char *, pszSubnetMask, NET_GetSubnetMask(pszSubnetMask));
}

char *MPU_NET_GetMacAddress(char *pszMacAddress)
{
    MPU_NET_GET(char *, pszMacAddress, NET_GetMacAddress(pszMacAddress));
}

int MPU_NET_SetDns(char *strDns0, char *strDns1)
{
    MPU_NET_CALL(NET_SetDns(strDns0, strDns1));
}

int MPU_NET_GetDns(char *strDns0, char *strDns1)
{
    MPU_NET_GET(int, 0, NET_GetDns(strDns0, strDns1));
}

int MPU_NET_GetDhcpEnable()
{
    MPU_NET_GET(int, 0, NET_GetDhcpEnable());
}
