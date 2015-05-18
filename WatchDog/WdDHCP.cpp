
#include "WatchDog.h"
/**
 *      The methods of WatchDog class 
 *
 *  	@author     Howard Xu
 *	@version        2.5
 */

#define DHCP_CONF       "/etc/dhcpd.conf"

#define MODE	"operationMode"

#define IPMODE	"IPMODE"	// 0: FIXED 1:DHCP  in scan.txt

#define DHCPS	"DHCPS"
#define IPADDR	"IPADDR"
#define IPMASK	"IPMASK"
#define GATEWAY	"GATEWAY"

#define DSTART	"DSTART"
#define DEND	"DEND"
#define DTIME	"DTIME"
#define PDNS	"PDNS"
#define ADNS	"ADNS"

#define DLTIME	"default-lease-time"
#define DNS	"domain-name-servers"
#define SUBNET	"subnet"
#define RANGE	"range"
#define ROUTER	"routers"

int WatchDog::dhcpSvr()
{
	struct in_addr iaddr, naddr;
	getProperty gP(gP.P_SCAN);
	if (gP.getInt(IPMODE) > 0) return 0;	// DHCP client myself
	resolvDNS();
	system("pkill dhcpd");  // down first
	gP.init(gP.P_OTC);
	if (gP.getInt(DHCPS) == 0) return 0;	// disabled DHCPD
	LdConf dlist;
	int i = dlist.loadFromFile(DHCP_CONF);
	while (i > 0) {
		char buffer[200], *p;
		i = dlist.getIndex(DNS);
		if (i < 0) break;		
		p = gP.getStr(PDNS);
		if (strlen(p) > 8) {
			sprintf(buffer, "option domain-name-servers %s", p);
		 	p = gP.getStr(ADNS);
			if (strlen(p) > 8) {
				strcat(buffer, ", ");
				strcat(buffer, p);
			}
			strcat(buffer, ";\n");
			dlist.chgLine(i, buffer);
		}
		int l = gP.getInt(DTIME) * 60;		// Seconds
		if (l > 0) {
			i = dlist.getIndex(DLTIME);
			if (i != -1) {
				sprintf(buffer, "%s %d;\n", DLTIME, l);
				dlist.chgLine(i, buffer);
			}
		}
		i = dlist.getIndex(SUBNET);
		if (i < 0) break;
		if (inet_aton(gP.getStr(IPADDR), &iaddr)==0) break;
	 	if (inet_aton(gP.getStr(IPMASK), &naddr)==0) break;
		iaddr.s_addr = iaddr.s_addr & naddr.s_addr;
		sprintf(buffer, "subnet %s netmask %s {\n", inet_ntoa(iaddr), gP.getStr(IPMASK));
		dlist.chgLine(i, buffer);
		p = gP.getStr(IPADDR);
                i = strlen(p) -1;
		while(i-- > 0) if(p[i] == '.') { p[i] = 0; break; }
		i = dlist.getIndex(RANGE);
		if (i < 0) break;
		sprintf(buffer, "  range  %s.%s %s.%s;\n", p,gP.getStr(DSTART), p, gP.getStr(DEND));  
		dlist.chgLine(i, buffer);
		i = dlist.getIndex(ROUTER);
		sprintf(buffer, "  option routers %s;\n", gP.getStr(GATEWAY));
		dlist.saveToFile(DHCP_CONF);

		gP.init(gP.P_WLAN);
		if (gP.getInt(MODE) == 0)
			system("dhcpd br0 2>/dev/null");
		else
			system("dhcpd ath0 2>/dev/null");
		return 0;
	}
	return -1;
}

#define RESOLV_CONF	"/etc/resolv.conf"

#define NAMESERVER	"nameserver"

void WatchDog::resolvDNS()
{
	int i;
	char buffer[40], *p;
        getProperty gP(gP.P_OTC);
	LdConf rlist;
	rlist.loadFromFile(RESOLV_CONF);
	p = gP.getStr(PDNS);
        if (*p != 0) {
                sprintf(buffer, "%s %s\n", NAMESERVER, p);
		i = rlist.getIndex(NAMESERVER);
		if (i != -1) 
			rlist.chgLine(i, buffer);
		else
			rlist.setLine(buffer);
	}
	p = gP.getStr(ADNS);
        if (*p != 0) {
		sprintf(buffer, "%s %s\n", NAMESERVER, p);
		i = rlist.getIndex(NAMESERVER);
		i = rlist.getIndex(NAMESERVER, OneLine::MASK, i+1);
		if (i != -1)
                        rlist.chgLine(i, buffer);
                else
                        rlist.setLine(buffer);
	}
	rlist.saveToFile(RESOLV_CONF); 
}
