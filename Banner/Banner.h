

/**
 *      The Banner class
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#if !defined(WIJET2_BANNER_H_INCLUDED_DEFINE_)
#define WIJET2_BANNER_H_INCLUDED_DEFINE_

#include "../LogTrace/LogTrace.h"
#include "../LoadFile/LdFlist.h"
#include "../LoadFile/LdEnv.h"
#include "../myNetLib/myNetWK.h"
#include "../XwinObj/XwinObject.h"
#include "../AutoIP/PsmProtocol.h"

class Banner
{
public:	Banner();
	virtual ~Banner();
	int start();
	void loop();
	inline void stop() { m_continue= 0; };
	inline int getStat() { return m_continue; };
        void tellService();
	int splash();
	void updtShow(bool f=false);
	void updateBurn();
private:
	void updateInfo();
	void updateScreen();
	void changePic(char *fname);
	int processEvent();
	int queryIpMac(char *IP, char *MAC);
	inline int getDuration() { int l = m_flist.getDuration(); 
		return (l<0?86400:l==0?5:l); }
        XwinObject 	m_win;
        LdFlist 	m_flist;
	LdProp          m_slist;
	LdProp		m_vlist;
	int		m_continue;
	int 		m_count;
	LogTrace*	m_Log;
	bool		m_show;
	bool		m_splash;
	int		m_otc;		// special for invoke xterm
	bool		m_ethonly;	// ethernet only
};

#endif	// !defined(WIJET2_BANNER_H_INCLUDED_DEFINE_)


