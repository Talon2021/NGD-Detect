#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/route.h>
#include <sys/mman.h>
#include "ip.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_arp.h>

#define IPQUADSIZ (16)
#define IFNAME "eth0"
#define SYSTEM_NETWORK_CONF "/etc/network/interfaces"

#ifndef TRACE_IP

#define TRACE_IP //printf
#endif
/*
 *      Display an IP address in readable format.
 */

#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

int set_default_gw(char *if_name, char *address);

int set_numeric_ip(char *if_name, struct sockaddr_in *sockaddr, ADDR_SET_OPS operation);

int get_default_gw(char *if_name, char *address);


/******************************************************************
****                  Global variables                         ****
******************************************************************/
char szVersion_IP_20080928[] = "1.2.0.1293 Build " __DATE__ " " __TIME__;

int IsIface(char *szLine, char *ifname, int *pEntered)
{
	char *pIface = NULL;

	pIface = szLine;
	while (*pIface != '\0' && isspace(*pIface))
	{
		pIface++;
	}

	if (0 == strncmp(pIface, "iface", 5))
	{
		pIface += 5;
		while (*pIface != '\0' && isspace(*pIface))
		{
			pIface++;
		}
		// 进入了该网卡的配置
		if (0 == strncmp(pIface, ifname, strlen(ifname)))
		{
			*pEntered = 1;
			return 1;
		}
		// 离开了该网卡的配置
		else if (1 == *pEntered)
		{
			*pEntered = 0;
			return 0;
		}
	}
	return 0;
}

static int TestInterface(char *path)
{
	
	struct stat buf;
	int fd;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		return 0;
	fstat(fd, &buf);
	close(fd);
	return buf.st_size;
}


int Replace_Section_In_File(char *FileName, char *ifname, char *SectionPattern, char *NewValue)
{
	TRACE_IP("\n..IP.C....Replace_Section_In_File()......begin............\n");
	FILE *fp_new;

	FILE *fp_bak;

	char system_cmd[255];

	char bakFileName[255];

	char line_buffer[512];

	/*Step-1 : copy FileName to FileName-bak */
	sprintf(bakFileName, "%s-bak", FileName);
	sprintf(system_cmd, "mv %s %s", FileName, bakFileName);
	system(system_cmd);
	
	
	/*Step-2 : Lookup SectionPattern in FileName and Do "Replace" */
	/*Step-2-1 : Create New File */
	fp_new = NULL;
	fp_new = fopen(FileName, "w");
	if (fp_new == NULL)
	{
		TRACE_IP("Create File %s Failed...\n", FileName);
		sprintf(system_cmd, "mv %s %s", bakFileName, FileName);
		system(system_cmd);
		return -1;
	}

	fp_bak = NULL;
	fp_bak = fopen(bakFileName, "r");
	if (fp_bak == NULL)
	{
		TRACE_IP("Can't open File %s\n", bakFileName);
		sprintf(system_cmd, "mv %s %s", bakFileName, FileName);
		system(system_cmd);
		return -1;
	}
	fgets(line_buffer, 512, fp_bak);
	int nReplace = 0;

	while (!feof(fp_bak))
	{
		int ret = IsIface(line_buffer, ifname, &nReplace);
		if (ret || !nReplace || strstr(line_buffer, "#") != NULL || 
			((strstr(line_buffer, SectionPattern) == NULL) || (strstr(line_buffer, "hwaddress") != NULL)))
		{						//Just Copy to new file
			fwrite(line_buffer, strlen(line_buffer), 1, fp_new);
		}
		else
		{
			//First Replace and then write to new file
			sprintf(line_buffer, "%s%s\n", SectionPattern, NewValue);
			fwrite(line_buffer, strlen(line_buffer), 1, fp_new);
		}
		fgets(line_buffer, 512, fp_bak);
	}

	fclose(fp_bak);
	fclose(fp_new);

	if (TestInterface(FileName) == 0 && TestInterface(bakFileName))
	{
		TRACE_IP("[set ip failed]ifname = %s,SectionPattern = %s,NewValue = %s\n",ifname,SectionPattern,NewValue);
		sprintf(system_cmd, "mv %s %s", bakFileName, FileName);
		system(system_cmd);
	}
	else if (TestInterface(FileName) == 0 && TestInterface(bakFileName) == 0)
	{
		fp_new = fopen(FileName, "w");
		TRACE_IP("[set ip failed 1]ifname = %s,SectionPattern = %s,NewValue = %s\n",ifname,SectionPattern,NewValue);
		if (fp_new == NULL)
		{
			TRACE_IP("3 Create File %s Failed...\n", FileName);
			return -1;
		}

		sprintf(line_buffer,"#INTERFACE_BEGIN#\n\
auto lo\n\
iface lo inet loopback\n\
    \n\
auto eth0\n\
iface eth0 inet static\n\
address 192.168.1.11\n\
network 192.168.1.1\n\
netmask 255.255.255.0\n\
broadcast 192.168.1.255\n\
gateway 192.168.1.1\n\
\n\
auto wlan0\n\
iface wlan0 inet static\n\
address 192.168.2.11\n\
network 192.168.2.1\n\
netmask 255.255.255.0\n\
broadcast 192.168.2.255\n\
gateway 192.168.2.1\n\
pre-up /etc/network/wifi-pre\n\
down /etc/network/wifi-pre\n\
#INTERFACE_END#\n");
		fwrite(line_buffer, strlen(line_buffer), 1, fp_new);
		fclose(fp_new);		
		
	}
	return 0;
}


int set_if_flags(char *if_name, short flags)
{
//	TRACE_IP("\n..IP.C....set_if_flags()......begin............\n");

	struct ifreq ifr;

//  struct sockaddr_in sockaddr;
	int sock, ret = 0;

	if ((!if_name) || (!flags))
		return EFAULT;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return errno;

	strncpy(ifr.ifr_name, if_name, IFNAMSIZ);

	if ((ret = ioctl(sock, SIOCGIFFLAGS, &ifr)) < 0)
	{
		ret = errno;
		goto out;
	}

	ifr.ifr_flags |= flags;

	if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
	{
		ret = errno;
	}

  out:

	close(sock);
	return ret;

}

/* Resolve a hostname (or dotted quad style address) into a sockaddr struct */

int resolve(struct sockaddr_in *sockaddr, char *host_name)
{

	struct hostent *host;

	if ((!sockaddr) || (!host_name))
		return EFAULT;

	if (!(host = gethostbyname(host_name)))
	{

		switch (h_errno)
		{

			case HOST_NOT_FOUND:
				return ENOENT;

			case TRY_AGAIN:
				return EAGAIN;

			default:
				return EINVAL;
		}
	}

	sockaddr->sin_family = host->h_addrtype;
	memcpy(&sockaddr->sin_addr, host->h_addr, host->h_length);

	return 0;
}


/*
*Resolve a IP string into a sockaddr struct 
* 2019 10 31 zhengg add
*/
int resolve_ip(struct sockaddr_in *sockaddr, char *host_name)
{
	int ipaddr = 0;
	sockaddr->sin_family = AF_INET;
	
	ipaddr = inet_addr(host_name);

	memcpy(&sockaddr->sin_addr, &ipaddr, 4);

	return 0;
}



int GetIpAddress(char *host_name)
{
	int ip1, ip2, ip3, ip4;

	sscanf(host_name, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	return ((ip1 << 24) | (ip2 << 16) | (ip3 << 8) | (ip4));
}

/* A helper function to set one of the interfcace addresses,
   be it the IP, netmask or broadcast. It also sets the out_address
   to the dotted quad form of the address. out_address must be a least 
   of IPQUADSIZ length. */

int set_if_address(char *if_name, char *in_address, ADDR_SET_OPS operation)
{

//  struct ifreq ifr;
	struct sockaddr_in sockaddr;
//	TRACE_IP("\n..IP.C....set_if_address()......begin............\n");
	int ret = 0;

	char gateway[20];

	char netmask[20];

	char ip[20];

	int ad_gateway, ad_netmask, ad_ip;

/*
	if (SET_NETMASK == operation)
	{
		TRACE_IP("set_if_address(): %s, netmask = %s\n", if_name, in_address);
	}
	else if (SET_BROADCAST == operation)
	{
		TRACE_IP("set_if_address(): %s, broadcast = %s\n", if_name, in_address);
	}
	else if (SET_ADDRESS == operation)
	{
		TRACE_IP("set_if_address(): %s, ip = %s\n", if_name, in_address);
	}
	else if (SET_GATEWAY == operation)
	{
		TRACE_IP("set_if_address(): %s, gateway = %s\n", if_name, in_address);
	}
*/

	if ((!if_name) || (!in_address))
	{
		ret = EFAULT;
		goto out;
	}

	if (operation == SET_GATEWAY)
	{
		ret = set_default_gw(if_name, in_address);
	}
	else
	{
 // 	TRACE_IP("\n..IP.C---------- set_if_address ---------operation != SET_GATEWAY---------\n");
		set_if_flags(if_name, IFF_UP);

		if ((ret = resolve_ip(&sockaddr, in_address)) != 0)
			goto out;

		if ((ret = set_numeric_ip(if_name, &sockaddr, operation)) != 0)
			goto out;
	}

	switch (operation)
	{
		case SET_ADDRESS:
			{
				char buffer[256];
				Replace_Section_In_File(SYSTEM_NETWORK_CONF, if_name, "address ", in_address);
				snprintf(buffer, 255, "route add default %s", if_name);
				system(buffer);
				break;
			}
		case SET_NETMASK:
			Replace_Section_In_File(SYSTEM_NETWORK_CONF, if_name, "netmask ", in_address);
			break;
		case SET_GATEWAY:
			Replace_Section_In_File(SYSTEM_NETWORK_CONF, if_name, "gateway ", in_address);
			break;
		case SET_BROADCAST:
			Replace_Section_In_File(SYSTEM_NETWORK_CONF, if_name, "broadcast ", in_address);
			break;
		default:
			break;
	}
	
	get_if_address(if_name, gateway, GET_GATEWAY);
	
	get_if_address(if_name, netmask, GET_NETMASK);
	
	get_if_address(if_name, ip, GET_ADDRESS);

	ad_gateway = GetIpAddress(gateway);

	ad_netmask = GetIpAddress(netmask);

	ad_ip = GetIpAddress(ip);



	TRACE_IP("\n....IP.C..set_if_address()....ip = %s,netmask = %s,gateway = %s\n", ip, netmask, gateway);
	if ((ad_netmask & ad_gateway) != (ad_netmask & ad_ip))
	{
		TRACE_IP("\n....IP.C......set_if_address.........diffrent ad_ip = %d,ad_netmask = %d,ad_gateway = %d\n\n", ad_ip, ad_netmask, ad_gateway);
		ret = set_default_gw(if_name, ip);
	}
	else
	{
		TRACE_IP("\n....IP.C......set_if_address..........same ad_ip = %d,ad_netmask = %d,ad_gateway = %d.................\n\n", ad_ip, ad_netmask, ad_gateway);
		ret = set_default_gw(if_name, gateway);
	}


  out:
	return ret;
}

int set_numeric_ip(char *if_name, struct sockaddr_in *sockaddr, ADDR_SET_OPS operation)
{
	TRACE_IP("\n..IP.C---------- set_numeric_ip--------begin--------\n");
	struct ifreq ifr;

	int sock, ret = 0;

	if ((!if_name) || (!sockaddr))
	{
		ret = EFAULT;
		goto out;
	}

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ret = errno;
		goto out;
	}

	memcpy(&ifr.ifr_addr, sockaddr, sizeof (struct sockaddr));
	strncpy(ifr.ifr_name, if_name, IFNAMSIZ);

	if ((ret = ioctl(sock, operation, &ifr)) != 0)
	{
		ret = errno;
	}

	close(sock);

  out:
	return ret;
}

int get_arp_entry(const char *dev, const char *ip,unsigned char *mac)
{
    int sfd, saved_errno, ret;
    //unsigned char aucupmac[18] = "";
    struct arpreq arp_req;
    struct sockaddr_in *sin;

    sin = (struct sockaddr_in *)&(arp_req.arp_pa);

    memset(&arp_req, 0, sizeof(arp_req));
    sin->sin_family = AF_INET;
    inet_pton(AF_INET, ip, &(sin->sin_addr));
    strncpy(arp_req.arp_dev, dev, IFNAMSIZ-1);

    sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd < 0)
	{
		return -1;
	}
    ret = ioctl(sfd, SIOCGARP, &arp_req);
    if (ret < 0) {
		printf("Get ARP entry failed \n");
		close(sfd);
		return -1;
    }
    if (arp_req.arp_flags & ATF_COM) {
        //mac = (unsigned char *)arp_req.arp_ha.sa_data;
		memcpy(mac, (unsigned char *)arp_req.arp_ha.sa_data, 6);
		printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		ret = 0;
    } else {
        printf("MAC: Not in the ARP cache.\n");
		ret = -1;
    }
	close(sfd);
	return ret;
}
/* A helper function to get one of the interfcace addresses,
   be it the IP, netmask or broadcast in dotted quad notation.
   address must point to a buffer of at least DQUADSIZ size */

int get_if_address(char *if_name, char *address, ADDR_GET_OPS operation)
{

TRACE_IP("\n..IP.C---------- get_if_address--------begin--------\n");
	struct ifreq ifr;

	//struct sockaddr_in sockaddr;
	int sock, ret;

	u_char *ptr;

	if ((!if_name) || (!address))
		return EFAULT;
	if (operation == GET_GATEWAY)
	{
		ret = get_default_gw(if_name, address);
		return ret;
	}
	
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return errno;

	strncpy(ifr.ifr_name, if_name, IFNAMSIZ);

	if (ioctl(sock, operation, &ifr) >= 0)
	{
		ret = 0;
		if (operation == GET_MAC)
		{
			ptr = (u_char *) & ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
			sprintf(address, "%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
		}
		else
		{
			struct sockaddr_in *sadr = (struct sockaddr_in *) &ifr.ifr_addr;

			snprintf(address, IPQUADSIZ, "%d.%d.%d.%d", NIPQUAD(sadr->sin_addr));
		}

	}
	else
	{

		ret = errno;
	}

	close(sock);


	return ret;
}

int get_default_gw(char *if_name, char *address)
{
TRACE_IP("\n..IP.C---------- get_default_gw--------begin--------\n");
	char buffer[255];

	FILE *fp;

	char *szRet;

	char *szSubString;

	int nRet;

	//Initial value 
	sprintf(address, "0.0.0.0");
	nRet = EFAULT;

	//Lookup gateway in SYSTEM_NETWORK_CONF
	fp = fopen(SYSTEM_NETWORK_CONF, "r");
	if (fp == NULL)
		return EFAULT;

	memset(buffer, 0, 255);
	szRet = fgets(buffer, 255, fp);
	int nEntered = 0;

	while ((!feof(fp)) && (szRet != NULL))
	{
		int ret = IsIface(szRet, if_name, &nEntered);

		if (!ret && nEntered && ((szSubString = (strstr(buffer, "gateway "))) != NULL) && (strstr(buffer, "#") == NULL))
		{
			sscanf(szSubString, "gateway %s", address);
			nRet = 0;
			break;				//Exit while 
		}

		memset(buffer, 0, 255);
		szRet = fgets(buffer, 255, fp);
	}

	fclose(fp);
	return nRet;

}

int set_default_gw(char *if_name, char *address)
{
	char system_cmd[255];

	if ((!if_name) || (!address))
		return EFAULT;

	memset(system_cmd, 0, 255);
	sprintf(system_cmd, "route del default dev %s", if_name);
	system(system_cmd);
	system(system_cmd);
	
	sprintf(system_cmd, "route add default gw %s dev %s", address, if_name);
	system(system_cmd);

	return 0;
}

/*
void main ()
{
char address_local[255];
get_if_address ("eth0", address_local, GET_ADDRESS);
printf ("local ip is %s\n", address_local);

get_if_address(IFNAME, address_local, GET_NETMASK) ;
printf ("local netmask is %s\n", address_local);	

set_if_address ("eth0", "192.168.1.98", SET_ADDRESS);
set_if_address ("eth0", "255.255.0.0", SET_NETMASK);
printf ("OKOK...\n");
get_if_address(IFNAME, address_local, GET_NETMASK) ;

printf ("local netmask is %s\n", address_local);	
set_if_address ("eth0", "192.168.1.6", SET_GATEWAY);
//get_if_address ("eth0", address_local, GET_ADDRESS);
//printf ("local ip is %s\n", address_local);


get_if_address ("eth0", address_local, GET_GATEWAY);

//get_default_gw ("eth0", address_local);
printf ("local gw is %s\n", address_local);
//set_default_gw ("eth0", "192.168.1.1");

	
}

*/
