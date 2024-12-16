#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "INet.h"
#include "sdk_log.h"
#include "ip.h"
#include "common.h"
static const char *INet_CfgSection = "INet_Cfg";

static const char *INet_DHCP_key = "DHCP_Enable";

static const char *INet_IP_key = "LocalIP";

static const char *INet_SubnetMask_key = "LocalSubnetMask";

#ifdef NET_WLAN
static const char *IFNAME = "wlan0";
#elif defined(NET_ETHERNET)
static const char *IFNAME = "eth0";
#endif
static const char *INet_Gateway_key = "LocalGateway";

static const char *INet_Gateway_Default_s = "192.168.110.1";

static const char *INet_SubnetMask_Default_s = "255.255.255.0";

static const char *INet_IP_Default_s = "192.168.110.11";

static const char *RESOLV_CONF = "/etc/resolv.conf";

static const char *DNS_Default = "8.8.8.8";

static const char *INet_Dns_Major_key = "MajorDns";

static const char *INet_Dns_Minor_key = "MinorDns";

INet *INet::m_instance = 0;

static int subMaskWillBeChanged=0;

int INet::LoadParam()
{
	int port, getPort;
	int ret = 0;
	char mac[MAX_MAC_STR_LEN] = {0};
	char ip[MAX_IP_STR_LEN] = {0};
    char gateway[MAX_IP_STR_LEN] = {0};
    char netmask[MAX_IP_STR_LEN] = {0};
	char major_dns[MAX_IP_STR_LEN] = {0};
	char lastSetValue[MAX_IP_STR_LEN] = {0};
	char buf[256] = {0};

	int mac0, mac1, mac2, mac3, mac4, mac5;
    CConfig *pCfg = CConfig::GetInstance();

    m_bDHCP_Enable = pCfg->GetValue(INet_CfgSection, INet_DHCP_key, (long)0);

	m_pCConfig->GetValue(INet_CfgSection, INet_IP_key, (char *)INet_IP_Default_s, ip, MAX_IP_STR_LEN);
	if (!IsValidIP(ip))
	{							
		ERROR(" load cfg--  ip is invalid(%s)\n", ip);
		strcpy(ip, INet_IP_Default_s);
	}
	NET_SetIPAddress(ip);
	pCfg->GetValue(INet_CfgSection, INet_SubnetMask_key, (char *)INet_SubnetMask_Default_s, netmask, MAX_IP_STR_LEN);
	if (!IsValidIP(netmask))
	{
		ERROR(" load cfg--  subnetmask is invalid(%s)\n", netmask);
		strcpy(netmask, INet_SubnetMask_Default_s);
	}

	ret = NET_SetSubnetMask(netmask);
	

	pCfg->GetValue(INet_CfgSection, INet_Gateway_key, (char *)INet_Gateway_Default_s, gateway, MAX_IP_STR_LEN);
	if (!IsValidIP(gateway))
	{
		ERROR("load cfg -- gateway is invalid(%s)\n", gateway);
		strcpy(gateway, INet_Gateway_Default_s);
	}

	ret = NET_SetGateway(gateway);

	NET_GetMacAddress(mac);

	sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5);
	m_sMAC_addr[0] = mac0;
	m_sMAC_addr[1] = mac1;
	m_sMAC_addr[2] = mac2;
	m_sMAC_addr[3] = mac3;
	m_sMAC_addr[4] = mac4;
	m_sMAC_addr[5] = mac5;

	pCfg->GetValue(INet_CfgSection, INet_Dns_Major_key, (char *)DNS_Default, major_dns, MAX_IP_STR_LEN);
	if(!IsValidIP(major_dns))
	{
		ERROR("load cfg -- gateway is invalid(%s)\n", gateway);
		strcpy(major_dns, DNS_Default);
	}
	NET_SetDns(major_dns, NULL);
	
    return 0;
}

int INet::IsValidIP(const char *ipaddr)
{
    unsigned int ip;

	ip = inet_addr(ipaddr);
	if ((unsigned int) (-1) == ip || 0 == ip)
	{
		return 0;
	}
	return 1;
}

int INet::IsValidGW(const char *ipaddr)
{
    unsigned int ip;
    unsigned int ip1, ip2, ip3, ip4;
    sscanf(ipaddr, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
    if (ip1 == 255 || ip2 == 255 )
    {
        return 0;
    }	

	ip = inet_addr(ipaddr);
	if ((unsigned int) (-1) == ip || 0 == ip)
	{
		return 0;
	}
	return 1;
}

INet *INet::GetInstance()
{
	if (m_instance == 0)
	{		
		printf("---------Inet.cpp-----Net::init()---start!!!!!!!!------\n");

		m_instance = new INet;

		if (0 != m_instance)
		{
			m_instance->Init();
		}
		else
		{
		  return NULL;
		}
	}
	return m_instance;
}

void INet::Release()
{
	if (m_instance != 0)
	{
		delete m_instance;

		m_instance = 0;
	}
}

int INet::Init()
{
	m_pCConfig = CConfig::GetInstance();
	if (NULL == m_pCConfig)
	{
		ERROR("NULL == m_pCConfig\n");
		UnInit();
		return -1;
	}
	pthread_mutex_init(&m_writeLock, NULL);

	//LoadParam();

    return 0;
}

int INet::UnInit()
{
    return 0;
}

INet::INet()
{
    m_bDHCP_Enable = 0;
}

INet::~INet()
{
    UnInit();
}

int INet::NET_SetIPAddress(const char *pszIPAddress)
{
	int ret = 0;
	pthread_mutex_lock(&m_writeLock);
	if (NULL == pszIPAddress)
	{
		ERROR("NULL == pszIPAddress\n");
		pthread_mutex_unlock(&m_writeLock);
		return -1;
	}
	if (!IsValidIP(pszIPAddress))
	{
		ERROR("invalid ip =%s\n", pszIPAddress);
		pthread_mutex_unlock(&m_writeLock);
		return -1;
	}

	int first = atoi(pszIPAddress);
    if((0 == first) || (127 == first) || (first > 223))
    {
        ERROR("ip first is invalid: %s\n", pszIPAddress);
		pthread_mutex_unlock(&m_writeLock);
        return -1;
    }

	if (strncmp(m_sLocalIP, pszIPAddress, MAX_IP_STR_LEN) == 0)
	{
		ERROR("The same IP set\n");
		pthread_mutex_unlock(&m_writeLock);
		return 0;
	}

	ret = set_if_address((char *) IFNAME, (char *) pszIPAddress, SET_ADDRESS);
	if (ret)
	{
		ERROR("Set ipaddress failed (%d) pszIPAddress =%s \n", ret, pszIPAddress);
		pthread_mutex_unlock(&m_writeLock);
		return -1;
	}
	char szBroadcastAddr[MAX_IP_STR_LEN];

	get_if_address((char *) IFNAME, szBroadcastAddr, GET_BROADCAST);
	set_if_address((char *) IFNAME, szBroadcastAddr, SET_BROADCAST);

	strcpy(m_sLocalIP, pszIPAddress);
	CConfig *pCfg = CConfig::GetInstance();
	pCfg->SetValue(INet_CfgSection, INet_IP_key, m_sLocalIP);
    pthread_mutex_unlock(&m_writeLock);
	return ret;
}

char *INet::NET_GetIPAddress(char *pszIPAddress)
{
	int ret;

	if (NULL == pszIPAddress)
	{
		ERROR("NULL==pszIPAddress\n");
		return NULL;
	}

	ret = get_if_address((char *) IFNAME, (char *) pszIPAddress, GET_ADDRESS);
	if (ret)
	{
		*pszIPAddress = '\0';
		ERROR("Get ipaddress failed (%d) \n", ret);
		return NULL;
	}

	strcpy(m_sLocalIP, pszIPAddress);
	
	return pszIPAddress;
}

int INet::NET_SetGateway(const char *pszGateway)
{
    int ret;
	//不管DHCP是否打开，均认为是设静态
	if (NULL == pszGateway)
	{
		ERROR("NULL == pszGateway\n");
		return -1;
	}
	//检查IP是否合法
	if (!IsValidIP(pszGateway))
	{
		ERROR("nvalid ip =%s\n", pszGateway);
		return -1;
	}
	
	if (!IsValidGW(pszGateway))
	{
		ERROR("invalid ip =%s\n", pszGateway);
		return -1;
	}
	ret = set_if_address((char *) IFNAME, (char *) pszGateway, SET_GATEWAY);
	if (ret)
	{
		ERROR("set_if_address<SET_GATEWAY> fail(%d), Gateway = %s\n", ret, pszGateway);
		return -1;
	}
	strcpy(m_sGateWay, pszGateway);
	m_pCConfig->SetValue(INet_CfgSection, INet_Gateway_key, m_sGateWay);

    return 0;
}

char *INet::NET_GetGateway(char *pszGateway)
{
    int ret;

	if (NULL == pszGateway)
	{
		ERROR("NULL == pszGateway\n");
		return NULL;
	}

	ret = get_if_address((char *) IFNAME, pszGateway, GET_GATEWAY);
	if (ret)
	{
		*pszGateway = '\0';
		ERROR("get_if_address<GET_GATEWAY> fail(%d)\n", ret);
		return NULL;
	}

	return pszGateway;
}

int INet::NET_SetSubnetMask(const char *pszSubnetMask)
{
	int ret;
	//static int first = 1;
	if (NULL == pszSubnetMask)
	{
		ERROR("NULL == pszSubnetMask\n");
		return -1;
	}
	
	if (!IsValidIP(pszSubnetMask))
	{
		ERROR("invalid ip =%s\n", pszSubnetMask);
		return -1;
	}

	ret = set_if_address((char *) IFNAME, (char *) pszSubnetMask, SET_NETMASK);
	if (ret)
	{
		ERROR("Set subnetMask failed (%d) pszSubnetMask =%s \n", ret, pszSubnetMask);		
		subMaskWillBeChanged = 0;
		return -1;
	}
	strcpy(m_sSubNetMask, pszSubnetMask);

	m_pCConfig->SetValue(INet_CfgSection, INet_SubnetMask_key, m_sSubNetMask);

	// if(!first)
	// {
	// 	saveSubMaskBak(m_sSubNetMask);
	// }
	// first = 0;

	subMaskWillBeChanged = 0;

	return 0;
}

char *INet::NET_GetSubnetMask(char *pszSubnetMask)
{
    int ret;

	if (NULL == pszSubnetMask)
	{
		ERROR("NULL==pszSubnetMask\n");
		return NULL;
	}

	ret = get_if_address((char *) IFNAME, (char *) pszSubnetMask, GET_NETMASK);
	if (ret)
	{
		*pszSubnetMask = '\0';
		ERROR("Get SubnetMask failed (%d) \n", ret);
		return NULL;
	}

	return pszSubnetMask;
}

char *INet::NET_GetMacAddress(char *pszMacAddress)
{
    int ret;

	if (NULL == pszMacAddress)
	{
		ERROR("NULL==pszMacAddress\n");
		return NULL;
	}

	ret = get_if_address((char *) IFNAME, (char *) pszMacAddress, GET_MAC);
	if (ret)
	{
		ERROR("Set MacAddress failed (%d) pszMacAddress =%s \n", ret, pszMacAddress);
		return NULL;
	}

	return pszMacAddress;
}

int INet::NET_SetDns(char *strDns0, char *strDns1)
{
	char line[256];
	char command[256];
	if(strDns0 == NULL && strDns1 == NULL)
	{
		ERROR("NULL==dns\n");
		return -1;
	}
	pthread_mutex_lock(&m_writeLock);
	FILE *file = fopen(RESOLV_CONF, "r");
	FILE *tmp = fopen("/tmp/resolv.conf.tmp", "w");
	if (!file || !tmp) {
        ERROR("Error opening files");
		pthread_mutex_unlock(&m_writeLock);
       return -1;
    }
	while (fgets(line, sizeof(line), file)) {
        if (strstr(line, IFNAME) == NULL) {
            fputs(line, tmp);
        }
    }
	fclose(file);

	if(strDns0)
	{
		fprintf(tmp, "nameserver %s # %s\n", strDns0, IFNAME);
		
	}
	if(strDns1)
	{
		fprintf(tmp, "nameserver %s # %s\n", strDns1, IFNAME);
	}

	fclose(tmp);
	
	snprintf(command, sizeof(command), "cat /tmp/resolv.conf.tmp > %s",RESOLV_CONF);
	mysystem(command);
	if(strDns0)
	{
		strcpy(m_sDns0_addr, strDns0);
		m_pCConfig->SetValue(INet_CfgSection, INet_Dns_Major_key, m_sDns0_addr);
	}
	if(strDns1)
	{
		strcpy(m_sDns1_addr, strDns1);
		m_pCConfig->SetValue(INet_CfgSection, INet_Dns_Minor_key, m_sDns1_addr);
	}
	pthread_mutex_unlock(&m_writeLock);
    return 0;
}

int INet::NET_GetDns(char *strDns0, char *strDns1)
{
	if(strDns0 == NULL && strDns1 == NULL)
	{
		ERROR("dns buff is NULL\n");
		return -1;
	}
	int dns_count = 0;
	int ret = -1;
	char line[256];
	FILE *file = fopen(RESOLV_CONF, "r");
    if (!file) {
        ERROR("Failed to open resolv.conf");
        return -1;
    }
	pthread_mutex_lock(&m_writeLock);

	while (fgets(line, sizeof(line), file))
	{
		if (strstr(line, IFNAME) == NULL)
		{
			continue;
		} 
		if (strncmp(line, "nameserver ", 10) == 0)
		{
			char *dns_addr = line + 11;
			while (*dns_addr == ' ') dns_addr++;
			if(dns_count == 0 && strDns0)
			{
				strncpy(strDns0, dns_addr, MAX_IP_STR_LEN);
				strDns0[strcspn(strDns0, "#")-1] = '\0';
			}
			else if(dns_count == 1 && strDns1)
			{
				strncpy(strDns1, dns_addr, MAX_IP_STR_LEN);
				strDns0[strcspn(strDns0, "#")-1] = '\0';
			}
			dns_count++;
		}
	}
	fclose(file);
	pthread_mutex_unlock(&m_writeLock);
	if(dns_count > 0)
	{
		ret = 0;
	}
    return ret;
}

int INet::NET_GetDhcpEnable()
{
    return m_bDHCP_Enable;
}
