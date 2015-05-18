
/**
 *      The WatchDog class
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#if !defined(WIJET2_WATCHDOG_H_INCLUDED_DEFINE_)
#define WIJET2_WATCHDOG_H_INCLUDED_DEFINE_

#include "../LogTrace/LogTrace.h"
#include "../myNetLib/myNetWK.h"
#include "../LoadFile/getProperty.h"
#include "../LoadFile/LdConf.h"
#include "../LoadFile/LdMagic.h"

#include "ShMemory.h"

class WatchDog
{
public:	WatchDog();
	virtual ~WatchDog();
	void setLog();
	int init(int n);
	void start();
	void loop();
	void process(int n);
	inline void stop() { m_continue= 0; };

private:
	bool ckHW();
	void setLED(int n);
	void initBurn();
	void setBurn(int n, bool last=false);
	int dhcpSvr();
	void resolvDNS();
	void uploadIMG();
	void updtXorg();
	void updtXine();
	void sysCheck();
	void usbMount(int n, bool f);
	void initUSB(int n);
	void callSys(char *cmd);
	void dump();
	inline void setXwindow(bool up) {
		if (up) system("xinit 2>/dev/null &");
	}
	inline void setNetwork(bool up) {
		if (up) system("st");
		else system("dn");
	}
	inline void setWiJET(bool up) {
		if (up) system("ct");
		else system("ka");
	}
        inline void tellOther(char *nm, int n) {
                char cmd[60];
                sprintf(cmd, "pkill -%d %s", n, nm);
                system(cmd);
        };
	inline void stopLog() { if(m_Log) m_Log->stopLog(); };

	int		m_continue;
	LogTrace*	m_Log;
	shareData*	m_shmm;
	bool		m_burn;
	int		m_mount;	// 0: None, 1: sda1, 2: sdb1
};

extern int checkHW(char* sn);
extern int checkIR();

#endif	// !defined(WIJET2_WATCHDOG_H_INCLUDED_DEFINE_)


