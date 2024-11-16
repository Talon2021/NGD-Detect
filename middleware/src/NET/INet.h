#ifndef _INET_H_
#define _INET_H_

#pragma once

#define MAX_IP_STR_LEN 16
#define MAX_MAC_STR_LEN 18

#include "CConfig.h"

class INet
{
private:
    int m_bDHCP_Enable;

    static INet *m_instance;

    pthread_mutex_t m_writeLock;

    char m_sLocalIP[MAX_IP_STR_LEN];

    char m_sGateWay[MAX_IP_STR_LEN];

    char m_sSubNetMask[MAX_IP_STR_LEN];

    char m_sMAC_addr[MAX_MAC_STR_LEN];

    char m_sDns0_addr[MAX_IP_STR_LEN];

    char m_sDns1_addr[MAX_IP_STR_LEN];
    CConfig *m_pCConfig;
    int LoadParam();

    //!\brief 检查IP是否合法
	//!\param ipaddr IP地址
	//!\return 合法1,不合法0
    int IsValidIP(const char *ipaddr);

    int IsValidGW(const char *ipaddr);
public:
    static INet *GetInstance();
    static void Release();
    int Init();
    int UnInit();
    INet(/* args */);
    ~INet();

    int NET_SetIPAddress(const char *pszIPAddress);

    char *NET_GetIPAddress(char *pszIPAddress);
    
    int NET_SetGateway(const char *pszGateway);

    char *NET_GetGateway(char *pszGateway);

    int NET_SetSubnetMask(const char *pszSubnetMask);

    char *NET_GetSubnetMask(char *pszSubnetMask);

    char *NET_GetMacAddress(char *pszMacAddress);

    int NET_SetDns(char *strDns0, char *strDns1);

    int NET_GetDns(char *strDns0, char *strDns1);

    int NET_GetDhcpEnable();
    
};










#endif