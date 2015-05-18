#include "AutoIpProcess.h"


AutoIpProcess::AutoIpProcess()
{
	myNetIpAddr = 0;
	myNetMask = 0;
	psmNetMask = 0;
	memset(myNetMAC, 0, 6);

	state = 0;
	typeFlag = 0;       //init
	setDelta(0, 0);
	
	moderatorIP = 0;
	grantedIP = 0;

	m_Log = new LogTrace("AutoIP");
	setLog();

	m_wlan = new myWlan(WLAN_CONFIG);

        Config();
}

AutoIpProcess::~AutoIpProcess()
{
	m_continue = false;
	m_ScanUDP.Close();
}

/**
 *      setLog method
 */
void AutoIpProcess::setLog()
{
        char* f = NULL;
        getProperty gP(gP.P_OTC);
        int i = LogTrace::ERROR;
        if (gP.getInt("MODULES") & 2) {		// ST 1: AI 2: MD 4:  WD 8
		i =  gP.getInt("DEBUG");
                if (i != 0) f = "/www/log/autoip";
	}
        m_Log->setLog(i, f);
}

void AutoIpProcess::Config()
{
// WiJET.E is the default device name, it will be changed to CYNALYNX-s# 
#define DEFDEVNAME	"WiJET.E"
#define OTC_COFIG	"/etc/otc_defaults.conf"
#define	GATEWAY		"GATEWAY"
#define	DNAME		"DNAME"
#define AUTOIP		"AIPENABLED"

	LdProp clist;
	clist.loadFromFile(OTC_COFIG);
	currentGatway = inet_addr(clist.getValue(GATEWAY));
	strcpy(deviceName, clist.getValue(DNAME));
	if (strcmp(deviceName, DEFDEVNAME) == 0) {
		char *p = m_shm.getSNumber();
		if (strlen(p) != 0) {
			sprintf(deviceName, "CYNALYNX-%s", p);
			clist.setValue(DNAME, deviceName);
			clist.saveToFile(OTC_COFIG);
		}
	}
	char cmd[80];
        sprintf(cmd, "hostname %s", deviceName);
        system(cmd);

	m_autoIpMode = clist.getIntValue(AUTOIP) ? true: false;
	m_count = 0;
	m_Log->log(LogTrace::INFO, "Device Name %s Mode %d", deviceName, m_autoIpMode);
}

void AutoIpProcess::init(int n)
{
	// 1:	br0:0 	- br0 alias
	// 2:	br0	- bridge
	// 3:	eth0:0	- eth0 alias
	// 4:	eth0	- eht0
	// 5:   ath0:0	- ath0 alias
	// 6:   ath0	- ath0

	if (n > 4)
		strcpy(myIfName, n == 5? "ath0:0":"ath0");
	else if (n > 2)
		strcpy(myIfName, n == 3? "eth0:0":"eth0");
	else
		strcpy(myIfName, n == 1? "br0:0":"br0");
}

int AutoIpProcess::start()
{
#define CH_CHECK_PERIOD 2
        int m, n, sock, msock;
        fd_set fds;
        struct timeval tmout;

        if (!m_ModeratorTCP.OpenServer(WIJET_PORT)) {
		m_Log->log(LogTrace::ERROR, "Can't start Moderator service");
		return -1;
	}
	m_Log->log(LogTrace::INFO, "Start AutoIP service (%s)", myIfName);
        m_ScanUDP.Open(UDP_PORT_RECV);
        accessModeratorVariables(true);
        sock = m_ScanUDP.GetSocketID();

	RetrieveIP(sock);
//	GetIPAddress(myIPaddr, sock);
//	GetMacAddress(myNetMAC, sock);
        m_continue = true;

        while (m_continue) {
	        msock = m_ModeratorTCP.GetSocketID();
        	m = sock > msock? sock: msock;

                FD_ZERO(&fds);
                FD_SET(sock, &fds);
                FD_SET(msock, &fds);
                tmout.tv_sec = CH_CHECK_PERIOD;
                tmout.tv_usec = 0;

                n = select(m+1, &fds, NULL, NULL, &tmout);
		if (m_continue) {
                if (n < 0) {
			if ( m_count++ > 5) {
                        m_Log->log(LogTrace::ERROR,"Select Error in AutoIP\n");
                        return -1;
			}
			usleep(100);
                }
                if (n > 0) {
                	if (FD_ISSET(sock, &fds))
                        	ProcessMsg();
                	else if (FD_ISSET(msock, &fds))
                        	ProcessModerator();
		}
		}
        }
        return 0;
}

void AutoIpProcess::ProcessMsg()
{
	int n = m_ScanUDP.Recv((char *)&m_Message, 2*sizeof(CARD16), MSG_PEEK);
 m_Log->log(LogTrace::INFO, "Process message type %x ---------------->", m_Message.cmd);

	if (n > 0) {
		n = this->CheckChanel();
//	m_Log->log(LogTrace::INFO, "Process message type %x", m_Message.cmd);
		if (n == 0) {
		        switch (m_Message.cmd) {
            		case DISCOVERY_REQ:
				n = this->HandleDiscovery();
			break;
			case IPADDR_CONFIRM:
				n = this->HandleIpConfirm();
			break;
			case IPADDR_OFFER:
				n = this->HandleIpOffer();
			break;
			case RECONNECT_REQ:
				n = this->HandleReconnect();
			break;
			case RELEASE_NOTIFY:
				n = this->HandleRelease();
			break;
			case QUERYONLY_REQ:
				n = this->HandleQuery();
			break;
			default:
				n =  m_ScanUDP.Recv((char *)&m_Message, sizeof(m_Message), 0); 
				m_Log->log(LogTrace::WARN, "Unsupported message type %d", m_Message.cmd);
			break;
			}
		}
		else {	// illegal channel
			m_Log->log(LogTrace::WARN, "Illegal Channel");
                                n =  m_ScanUDP.Recv((char *)&m_Message, sizeof(m_Message), 0);
		}
	}
	else {	// error
		m_Log->log(LogTrace::ERROR,"Fail to receive messaage");
	}  
}

