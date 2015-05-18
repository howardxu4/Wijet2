#include "ConfigCommon.h"
#include "ConfigTools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEVICENAME      "DNAME"
#define IPADDR          "IPADDR"
#define IPMASK          "IPMASK"
#define GATEWAY         "GATEWAY"

#define	SSID		"SSID"
#define	CHANNEL		"Channel"
#define	MODE		"Mode"
#define RFDETECT	"RF activities detected"
#define INTERF		"Interference"

static char glbStr[4][60];
enum {
	MAC,
	IP,
	PSM,
	MODERATOR
	};


#define myUDP_PORT_RECV UDP_PORT_SEND           // reverse from AUTOIP
#define myUDP_PORT_SEND UDP_PORT_RECV

int getIP()
{
        SocketTCP myTCP;
        char buf[40];
        unsigned int n = 0;
        myTCP.Open(WIJET2_PORT, htonl(INADDR_LOOPBACK));
        strcpy(buf, "AskIP");
        if (myTCP.Send(buf, strlen(buf) +1) > 0) {
                if (myTCP.Recv(buf, 40) > 0) {
                        sscanf(buf, "%u", &n);
                }
        }
	return n;
}

void getInfo()
{
	char *sIP = &glbStr[IP][0];
	char *sMAC = &glbStr[MAC][0];

	unsigned long ip;
        int n = 0;
        SocketUDP myRecv, mySend;

// default value for testing only	
	strcpy(sIP, "169.254.98.170");
	strcpy(sMAC, "00:11:2D:4C:FB:7A");
        strcpy(glbStr[MODERATOR], " ");
        strcpy(glbStr[PSM], " ");

        if (myRecv.Open(myUDP_PORT_RECV, htonl(INADDR_LOOPBACK)) != 0
        && mySend.Open(0, htonl(INADDR_LOOPBACK)) != 0) {
                myRecv.SetRcvTime(500);
                mySend.SetSndTime(500);
                char msg[80];
                msg[0] = 0x01;
                msg[1] = 0x06;
                *((unsigned int*) &msg[2]) = htonl(INADDR_LOOPBACK);
                *((unsigned int*) &msg[6]) = 0xFF000000;
                if(mySend.Connect(myUDP_PORT_SEND, htonl(INADDR_LOOPBACK))
                && mySend.Send(msg, 10) > 0)
                        if (myRecv.Recv(msg, 80)> 0) {
                                ipHandshake *sMsg = (ipHandshake*)&msg[0];
                                sprintf(sIP, "%s", inet_ntoa(*(struct in_addr *)(&sMsg->drp.r.ipAddr[0])));
                                int i, j;
                                char *p = (char*)&sMsg->drp.r.mac[0];
                                for (i = 0, j = 0; i< 6; i++, j=j+3)
                                        sprintf(&sMAC[j], "%2.2X:", p[i] & 0xFF);                                
				sMAC[17] = 0;
				ip = *(unsigned long*)(&sMsg->drp.e.dName[DEVICE_NAME_MAX_LEN-4]);
				if (ip != 0)
				strcpy(glbStr[MODERATOR], inet_ntoa(*(struct in_addr *)(&sMsg->drp.e.dName[DEVICE_NAME_MAX_LEN-4])));

				ip = getIP();
				if (ip != 0)
				strcpy(glbStr[PSM], inet_ntoa(*(struct in_addr *)(&ip)));
				
                                n = 1;
                        }
        }
        if (n == 0) {
//                printf("ERROR: Can't communicate with AutoIP");
        }
}
 
int cgiMain() {
	ConfigCommon cfg;
	getProperty gPc(gPc.P_OTC);
	getProperty gPs(gPs.P_SCAN);
	int i = gPs.getInt(INTERF);
	getInfo();
	
	cfg.putHeader("Pegasus Wireless CYNALYNX - Information");
	cfg.startPage("Information","The information on this page shows the current system status and settings.");
	cfg.putSubtitle("Current connectivity information");
	cfg.dispRow("Radio MAC address", glbStr[MAC]);
	cfg.dispRow("Current IP address", glbStr[IP]);
	cfg.dispRow("Current Channel", gPs.getStr(CHANNEL));
	if (i == 0)
	cfg.dispRow("RF activities detected", gPs.getStr(RFDETECT), true);
	else
	cfg.dispRow("RF activities detected", cfg.getFValue("%s &nbsp;&nbsp;&nbsp;</font> <font class=f1 color=red>WARNING:Interference",  gPs.getStr(RFDETECT)), true);
	cfg.skipRow();
	cfg.dispRow("Moderator IP", glbStr[MODERATOR]);
	cfg.dispRow("PSM IP address", glbStr[PSM]);
	cfg.skipRow();
	cfg.putSubtitle("Current configuration setting");
	cfg.dispRow("Device name", gPc.getStr(DEVICENAME));
	cfg.dispRow("SSID", gPs.getStr(SSID));
	cfg.dispRow("Radio Mode", gPs.getStr(MODE)); 
	cfg.skipRow();
	cfg.dispRow("IP address", gPc.getStr(IPADDR)); 
	cfg.dispRow("Subnet mask", gPc.getStr(IPMASK)); 
	cfg.dispRow("Default gateway", gPc.getStr(GATEWAY)); 
	cfg.endPage();
	return 0;
}

