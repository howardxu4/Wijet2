#include "AutoIpProcess.h"


#define GATEWAY_IP sizeof(unsigned long)
unsigned long currentGatway;

int AutoIpProcess::SetDefaultGateway(struct in_addr * gw, int del)
{
    struct sockaddr_in addr;
    struct rtentry route;
    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&addr,0,sizeof(addr));
    memset(&route,0,sizeof(route));
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = INADDR_ANY;
    memcpy(&route.rt_dst, &addr, sizeof(addr));
    memcpy(&route.rt_genmask, &addr, sizeof(addr));
    addr.sin_addr = *gw;
    memcpy(&route.rt_gateway, &addr, sizeof(addr));

    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_metric = 0;
    route.rt_dev = NULL;
    int n = ioctl(s, del?SIOCDELRT:SIOCADDRT, &route);
    if(n != 0) {
    	m_Log->log(LogTrace::ERROR, "setting default gateway failed %08X   %s   %s  [%d] %s", *(unsigned int *)gw, inet_ntoa(*gw), del?"Del":"Add", errno, strerror(errno));
        return -1;
    }
    return 0;
}

char* AutoIpProcess::safe_strncpy(char *dst, char *src, int size)
{
    dst[size-1] = '\0';
    return strncpy(dst,src,size-1);
}

int AutoIpProcess:: SetIpUsing(int skfd, char *name, int c, unsigned long ip)
{
    struct ifreq ifr;
    struct sockaddr_in sin;

    safe_strncpy(ifr.ifr_name, &name[0], IFNAMSIZ);
    memset(&sin, 0, sizeof(struct sockaddr));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip;
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
    if (ioctl(skfd, c, &ifr) < 0)
        return -1;
    return 0;
}

int AutoIpProcess::GetIPAddress(unsigned char *addr, int sock)
{
        struct ifreq ifr;

        strcpy(ifr.ifr_name,  myIfName);
        ifr.ifr_addr.sa_family = AF_INET;
        ioctl(sock, SIOCGIFADDR, &ifr);
        strcpy((char*)addr, inet_ntoa(*(struct in_addr *)&ifr.ifr_addr.sa_data[2]));
	m_Log->log(LogTrace::INFO,"IP address of %s = %s\n", myIfName, addr);
        return 0;
}

int AutoIpProcess::GetMacAddress(unsigned char *macAddr, int sock)
{
        struct ifreq ifr;
        int i, j;

        strcpy(ifr.ifr_name, myIfName);
        ifr.ifr_addr.sa_family = AF_INET;

        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
            return -1;

	char s[30];
        for (i = 0, j = 0; i< 6; i++, j=j+2) {
            macAddr[i] = ifr.ifr_hwaddr.sa_data[i];
 	    sprintf(&s[j], "%2.2X", ifr.ifr_hwaddr.sa_data[i] & 0xFF);
	}
	m_Log->log(LogTrace::INFO,"MAC of %s = %s", myIfName, s);

        return 0;
}

void AutoIpProcess::RetrieveIP(int sock)
{
	struct ifreq ifr;
	int i, j;
	
	safe_strncpy((char *)&ifr.ifr_name[0], (char *)&myIfName[0], IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
		 m_Log->log(LogTrace::ERROR,"Failed on IP address retrieve!");
	memcpy((char *)&myNetIpAddr, (char *)&ifr.ifr_addr.sa_data[2], 4);
	sprintf((char*)myIPaddr, "%s", inet_ntoa(*(struct in_addr *)&ifr.ifr_addr.sa_data[2]));

	ioctl(sock, SIOCGIFNETMASK, &ifr);
	memcpy((char *)&myNetMask, (char *)&ifr.ifr_addr.sa_data[2], 4);
	sprintf((char*)myMask, "%s", inet_ntoa(*(struct in_addr *)&ifr.ifr_addr.sa_data[2]));
	ioctl(sock, SIOCGIFHWADDR, &ifr);
	for (i = 0, j = 0; i< 6; i++, j=j+2) {
		myNetMAC[i] = ifr.ifr_hwaddr.sa_data[i];
		sprintf((char*)&myMACaddr[j], "%2.2X", ifr.ifr_hwaddr.sa_data[i] & 0xFF);
	}
}

