

/**
 *	The Control class 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#if !defined(WIJET2_CONTROL_H_INCLUDED_DEFINE_)
#define WIJET2_CONTROL_H_INCLUDED_DEFINE_


#include "../LogTrace/LogTrace.h"
#include "../myNetLib/myNetWK.h"
#include "State.h"
#include <pthread.h>

class Control
{
public:
	Control();
	virtual ~Control();
	int init();
	int start();
	void stop();
	void begin();
	inline void process(int n) { if (n == SIGTRAP) setLog();
		if (n == SIGUSR1) stopLog(); }

private:
	void setLog();
	inline void stopLog() { if (m_Log) m_Log->stopLog(); };
	SocketTCP m_service;
	State*	  m_myState;
	int	  m_continue;
        LogTrace*       m_Log;

};

#endif // !defined(WIJET2_CONTROL_H_INCLUDED_DEFINE_)
