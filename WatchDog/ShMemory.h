

/**
 *      The ShMemory class
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#if !defined(WIJET2_SHAREDMEMORY_H_INCLUDED_DEFINE_)
#define WIJET2_SHAREDMEMORY_H_INCLUDED_DEFINE_

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>

#include "../LoadFile/LdEnv.h"

// #define myDEBUG

struct shareData
{
// Control
	int State;
// Debug
	int Debug;
// AIP
	unsigned long moderatorIP;
	unsigned long grantedIP;
// Wireless
	int Mode;
// Config
	int Changes;	
// Vds
	int status;
// WatchDog
	int BurnTimes;
	char SNumber[10];
};


class ShMemory
{
public:	inline ShMemory() {
		m_shmp = NULL;
		LdEnv Env;
		key_t key = ftok(Env.getPath() , 2);
        	int shmid = shmget(key, sizeof(shareData), IPC_CREAT | 0666);
        	if (shmid >= 0) m_shmp = (shareData*)shmat(shmid, NULL, 0);
#ifdef myDEBUG
		printf("KEY = %x \n", key);
		printf("SHMID = %d \n", shmid);
		if (m_shmp == NULL) printf("Null Shared memory\n");
		else printf("SM key %d ID %d Memory %lX\n", key, shmid, (unsigned int*)m_shmp);
#endif
	};
	inline virtual ~ShMemory(){};
	
	inline void tellWD(int n) {   
		char cmd[40];
		sprintf(cmd, "pkill -%d wd", n);
		system(cmd);
	};
	inline void setState(int state) { m_shmp->State = state; };
	inline int getState() { return m_shmp->State; };
	inline void setDebug(int debug) { m_shmp->Debug = debug; };
	inline int getDebug() { return m_shmp->Debug; };
	inline void setModeratorIP(unsigned long ip) { m_shmp->moderatorIP = ip; };
	inline unsigned long getModeratorIP() { return m_shmp->moderatorIP; };
	inline void setGrantedIP(unsigned long ip) { m_shmp->grantedIP = ip; };
	inline unsigned long getGrantedIP() { return m_shmp->grantedIP; };
	inline void setMode(int mode) { m_shmp->Mode = mode; };
	inline void setChanges(int change) { m_shmp->Changes = change; };
	inline void setStatus(int status) { m_shmp->status = status; };
	inline void setBTimes(int btimes) { m_shmp->BurnTimes = btimes; };
	inline int getBTimes() { return m_shmp->BurnTimes; };
	inline void setSNumber(char* sn) { memcpy(m_shmp->SNumber,sn, 10); };
	inline char* getSNumber() { return m_shmp->SNumber; };
	inline shareData* getShmp() { return m_shmp; }; 

	enum {	
		M_STAT = SIGHUP,	// 1
		M_AIP =  SIGINT,	// 2
		M_VDS =  SIGQUIT,	// 3
		M_CONF = SIGILL,	// 4
		M_DEBG = SIGTRAP,	// 5
		M_STOP = SIGABRT,	// 6
		M_APPL = SIGBUS,        // 7
		M_SYST = SIGFPE,        // 8
		M_KILL = SIGKILL,	// 9	(kill)
		M_REST = SIGUSR1,	// 10
		M_MISC = SIGSEGV,	// 11
		M_WRLS = SIGUSR2,	// 12
		M_LOG =  SIGPIPE,	// 13
		M_NTHN = SIGALRM,	// 14	(nothing)
		M_TERM = SIGTERM	// 15
	};
		
private:
	shareData* m_shmp;
};


#endif // !defined(WIJET2_SHAREDMEMORY_H_INCLUDED_DEFINE_)
