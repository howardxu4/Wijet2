
/**
 *	The State class 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#if !defined(WIJET2_STATE_H_INCLUDED_DEFINE_)
#define WIJET2_STATE_H_INCLUDED_DEFINE_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "../LogTrace/LogTrace.h"
#include "../myNetLib/myNetWK.h"
#include "../myNetLib/cmdMsgs.h"
#include "../WatchDog/ShMemory.h"
#include "../LoadFile/getProperty.h"

class State  
{
public:
	State(LogTrace* log=NULL);
	virtual ~State();
	void init();
	void procIR();
	void setConnectivity(bool mode=true);
	int setState(int state, bool kill=false);
	inline int getState() { return m_state; }
	inline void setIP(unsigned int ip) {  m_IP = ip; }
	inline unsigned int getIP() { return m_IP; }
	inline void setWindow(unsigned int wd) { m_Window = wd; }
	inline unsigned int getWindow() { return m_Window; }
	inline void setScreenWidth(int x) { m_ScreenWidth = x; }
	inline int getScreenWidth() { return m_ScreenWidth; }
	inline void setScreenHeight(int y) { m_ScreenHeight = y; }
	inline int getScreenHeight() { return m_ScreenHeight; }
	inline void setBack(int back) { m_back = back; }
	inline void setPass() { m_pass = 1; }
	inline bool chkState() { return m_state == m_shm.getState(); }

	enum {
		S_IDLE = 0,
                S_LOCAL,
		S_WIJET,
		S_CNTRL,
		S_SVER,
		S_VWER,
		S_EXIT
	};
	
	LogTrace*       m_Log;
	char		m_cmd[200];

private:
	void CommandPSM(char cmd);
	void informPSM();

	int		m_state;
	int		m_pass;
	int		m_back;			// back to 
        unsigned int    m_IP;                 	// connected PC IP
        unsigned int    m_Window;             	// VNC desktop
        int   		m_ScreenWidth;
        int    		m_ScreenHeight;
	ShMemory	m_shm;			// Shared Memory	
};

#endif // !defined(WIJET2_STATE_H_INCLUDED_DEFINE_)
