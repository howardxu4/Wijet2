#if !defined(_AUTOIP_PROCESS_WIJET_DISCOVERY_DEFINED)
#define _AUTOIP_PROCESS_WIJET_DISCOVERY_DEFINED

#include "../myNetLib/myNetWK.h"
#include "../myNetLib/cmdMsgs.h"

#include <linux/route.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>

#include "../Wlans/myWlan.h"
#include "../LogTrace/LogTrace.h"
#include "../WatchDog/ShMemory.h"
#include "PsmProtocol.h"

class AutoIpProcess
{
public:
	AutoIpProcess();
	~AutoIpProcess();
	void setLog();
	void init(int n);
	int start();
	inline void stop( ) { m_continue = false; };
	inline void process(int sig) { sig==SIGTRAP? setLog() : 
		sig==SIGUSR1? m_Log->stopLog() : Config(); m_count=0; };

private:
	inline void setDelta(int x, int y) { m_deltax=x; m_deltay=y; };
	inline void getDelta(int *x, int *y) { *x=m_deltax; *y=m_deltay; };

	void Config();
        void ProcessMsg();
        void  ProcessModerator();

	int SetDefaultGateway(struct in_addr * gw, int del);
	char* safe_strncpy(char *dst, char *src, int size);
	int SetIpUsing(int skfd, char *name, int c, unsigned long ip);
	int GetIPAddress(unsigned char *addr, int sock);
	int GetMacAddress(unsigned char *macAddr, int sock);
	void RetrieveIP(int sock);

	int GetPreviousBurnTime(char *log_dom_sn);
	Bool accessModeratorVariables(Bool isRead);
	int CheckMessage(unsigned char *buf, int n);
	int QueryIP();

	int CheckChanel();
	int HandleDiscovery();
	int HandleIpConfirm();
	int HandleIpOffer();
	int HandleReconnect();
	int HandleRelease();
	int HandleQuery();

private:
	SocketUDP m_ScanUDP;
	ipHandshake m_Message;

	SocketTCP m_ModeratorTCP;
	unsigned long      moderatorIP;          // +++++++++howard 021305
	unsigned long      grantedIP;
	unsigned long 	   currentGatway;

	char     deviceName[DEVICE_NAME_MAX_LEN+1];
	char     *desktopName;
	unsigned char	 myIPaddr[16];
	unsigned char 	 myMask[16];
	unsigned char	 myMACaddr[18];

	CARD32   myNetIpAddr;
	CARD32   myNetMask;
	CARD32   psmNetMask;
	CARD8    myNetMAC[6];

	CARD16   state;
	CARD16   typeFlag;       //init to assuming same subnet as PSM

	int	m_deltax;
	int	m_deltay;
	bool	m_autoIpMode;	
	int 	m_count;	// error ignore
        LogTrace*       m_Log;
	bool		m_continue;
	char		myIfName[10];
	myWlan*		m_wlan;
	ShMemory        m_shm;
};
#endif	//_AUTOIP_PROCESS_WIJET_DISCOVERY_DEFINED

