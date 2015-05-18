

/**
 *      The myWlan class
 *
 *      @author     Howard Xu
 *      @version        2.5
 */

#if !defined(_MYWLAN_API_INCLUDED_DEFINED)
#define _MYWLAN_API_INCLUDED_DEFINED

#include "wlan_common.h"
#include "wlan_advanced.h"
#include "../LogTrace/LogTrace.h"
#include "../myNetLib/myNetWK.h"
#include "../LoadFile/getProperty.h"
#include "../LoadFile/LdConf.h"

using namespace std;

#define WLAN_CONFIG             "/etc/wlan.conf"
#define WJ_START_SCAN_FILE      "/etc/scan.txt"
#define OTC_CONFIG		"/etc/otc_defaults.conf"

class myWlan
{
public:
	myWlan(char* file=NULL);
	~myWlan();
	int init(char* file);
	int setScan();
	int startUp();
	int wpaSetup();
	bool isAvailable();
	int checkChannel();
	
	inline bool isRadioOn() {
		if (isAvailable()) return m_wlan->isRadioOn();
		else return false;
	};
	inline int getStandard() {
		if (isAvailable()) return m_wlan->getStandard();
		else return IEEE_80211G; 
	};
	inline int getMode() {
		if (isAvailable()) return m_wlan->getMode();
		else return MODE_MANAGED;
	};
	inline int getWepEnabled() {
		if (isAvailable()) return m_wlan->getWepEnabled();
		else return 0;
	};
	inline int getUseKeyIndex() {
		if (isAvailable()) return m_wlan->getUseKeyIndex();
		else return 0;
	};
	inline WepKeyT* getWepKeys() {
		if (isAvailable()) return m_wlan->getWepKeys();
		else return NULL;
	};	
	inline int getAuthOption() {
		if (isAvailable()) return m_wlan->getAuthOption();
		else return AUTH_OPEN;
	};
	inline int getChannel() {
		if (isAvailable()) return m_wlan->getChannel();
		else return 6;
	};
	inline char* getSsid() {
		if (isAvailable()) return m_wlan->getSsid();
		else return "";
	};
	inline void setAll() {
		if (isAvailable()) m_wlan->SetAll();
	};
	inline int getIntValue(char *name) {
		if (isAvailable()) return m_wlan->m_wlist.getIntValue(name);
		return 0;
	}
private:

#define	WLANIFNAME	"ath0"

	wlan_common* 	m_wlan;
	wlan_advanced* 	m_wa;
	LogTrace*       m_Log;
};


#endif // _MYWLAN_API_INCLUDED_DEFINED

