

#include "WatchDog.h"
#include <pthread.h>

/**
 *      The WatchDog class 
 *
 *  	@author     Howard Xu
 *	@version        2.5
 */

void* syscall(void *p)
{
	system((char*)p);
	pthread_exit(0);
}

void* invoke(void *p)
{
        WatchDog* wd = (WatchDog*)p;
        wd->loop();
        pthread_exit(0);
}

WatchDog::WatchDog()
{
        m_burn = false;
	m_mount = false;
        m_continue = 1;
        m_Log = new LogTrace("WatchDog");
        setLog();
}

WatchDog::~WatchDog()
{
	if (m_continue == 0) {
		// setLED(6);
		setWiJET(false);		// stop wijet
		setNetwork(false);		// stop network
	}
	else {
		m_Log->log(LogTrace::ERROR, "System will shutdown!");
		system("shutdown -g 1");
	} 
}

/**
 *      setLog method
 */
void WatchDog::setLog()
{
        char* f = NULL;
        getProperty gP(gP.P_OTC);
        int i = LogTrace::ERROR;
        if (gP.getInt("MODULES") & 8) { 	// ST 1: AI 2: MD 4:  WD 8
		i = gP.getInt("DEBUG");
                if (i != 0) f = "/www/log/watchdog";
	}
        m_Log->setLog(i, f);
}

void WatchDog::callSys(char *cmd)
{
        pthread_t myThread;
        pthread_create( &myThread, NULL, syscall, (void*)cmd);	
}

void WatchDog::dump()
{
        printf("State = %d\n", m_shmm->State);
        printf("Debug = %d\n", m_shmm->Debug);
        printf("moderatorIP = %lu\n", m_shmm->moderatorIP);
        printf("grantedIP = %lu\n", m_shmm->grantedIP);
        printf("Mode = %d\n", m_shmm->Mode);
        printf("changes = %d\n", m_shmm->Changes);
        printf("status = %d\n", m_shmm->status);
	printf("burnTimes=%d\n", m_shmm->BurnTimes);
	printf("SNumber=%s\n", m_shmm->SNumber);
}

bool WatchDog::ckHW()
{
	char SN[10];
	strcpy(SN, "000000");
	ShMemory shm;
	if (checkHW(SN) != 0) {
		m_Log->log(LogTrace::ERROR, "Check Hardware DOM fails");
                return false;
	}
	shm.setSNumber(SN);
	return true;
}

int WatchDog::init(int n)
{
        ShMemory shm;
        m_shmm = shm.getShmp();
        if (m_shmm == NULL) {
                m_Log->log(LogTrace::ERROR, "Shared Memory module fails");
                return false;
        }
	m_shmm->SNumber[0] = 0;		// initial to empty
	if (n == 1 && checkIR() != EXIT_SUCCESS) {
		m_Log->log(LogTrace::ERROR, "Check Hardware IR fails");
		if (checkIR() != EXIT_SUCCESS) {
			m_Log->log(LogTrace::ERROR, "Check Hardware IR fails");
			if (checkIR() != EXIT_SUCCESS) {
				m_Log->log(LogTrace::ERROR, "IR fatal failure");
                		return -1;
			}
		}
	}
	
        initBurn();
	updtXorg();
	updtXine();
        setXwindow(true);
	
#ifdef USEDEBUG
	dump();
#endif
	return 0;
}

void WatchDog::start()
{
	pthread_t myThread;
	pthread_create( &myThread, NULL, invoke, (void*)this);

// check application periodically
m_Log->log(LogTrace::DEBUG,"check application periodically");
	int count =  m_shmm->BurnTimes;
	while(m_continue) {
		sleep(5);
		if (m_burn) setBurn( ++count ); 
// Check APPs
	}
	if (m_burn) setBurn( ++count, true );
}

void WatchDog::loop()
{
// check system periodically
m_Log->log(LogTrace::DEBUG," check system periodically");
	int count = 0;
	while(m_continue) {
		sleep(5);
		count++;
		if (!m_shmm->SNumber[0] && !(count%5))	// Do it each 5th time 
			if (!ckHW()) {
				m_Log->log(LogTrace::ERROR," check hardware failed");
				if (count > 15) 
					process(ShMemory::M_REST);
				else if (count > 10)
				callSys( "xwinfo \"The hardware checking failed;CYNALYNX can't perform correctly;If this is found again;the system will be shutdown soon\" 2");
			}
			else tellOther("aip", ShMemory::M_CONF);
		sysCheck();
	}
}

void WatchDog::setLED(int n)
{
	enum { 	LED_OFF,
		LED_GREEN,
		LED_YELLOW,
		LED_RED
	};
	int i = LED_OFF;						
	char cmd[40];
	switch (m_shmm->State) {
	case 0:		// IDLE
		i = LED_GREEN;			
	break;
	case 1:		// LOCAL
		i = LED_YELLOW;
	break;
	case 6:		// EXIT
	break;
	default:	// CONNECT
		i = LED_RED;
	break;
	}
	sprintf(cmd, "setled %d", i);
	system(cmd);
}

void WatchDog::setBurn(int n, bool last)
{
#define BURN_FILE	"/etc/burn.tst"
	FILE *fp = fopen(BURN_FILE, "w");
	if (fp != NULL) {
		fprintf(fp, "BurnTimes=%d\n", n);
		fclose(fp);
		m_shmm->BurnTimes = n;
		if (last) system( "date >> /etc/burn.tst");
		if (m_shmm->State == 0)
			tellOther("bn", ShMemory::M_DEBG);
	}
}

void WatchDog::initBurn()
{
	m_shmm->BurnTimes = 0;
        FILE *fp = fopen(BURN_FILE, "r");
        if (fp != NULL) {
                fscanf(fp, "BurnTimes=%d", &m_shmm->BurnTimes);
                fclose(fp);
        }
}

void WatchDog::sysCheck()
{
// Check USB 
	struct stat fstat;
	if (stat("/dev/sda1", &fstat) < 0) {
		if (m_mount & 1) usbMount(1, false);
	}
	else if ((m_mount & 1) == 0) usbMount(1, true);
 	if (stat("/dev/sdb1", &fstat) < 0) {
                if (m_mount & 2) usbMount(2, false);
        }
        else if ((m_mount & 2) == 0) usbMount(2, true);

// Check Memory
}

void WatchDog::process(int n)
{
	switch(n) {
		case ShMemory::M_STAT:
		m_shmm->State = m_shmm->Changes;
		m_Log->log(LogTrace::DEBUG, "Process %d State = %d", n,  m_shmm->State);
		setLED(m_shmm->State);
		break;
		case ShMemory::M_AIP:
                if (m_shmm->State == 0)
                        tellOther("bn", ShMemory::M_CONF);
		break;
		case ShMemory::M_VDS:
		break;
		case ShMemory::M_CONF:
                m_Log->log(LogTrace::DEBUG, "Process %d Changes = %d", n,  m_shmm->Changes);
                // State::S_IDLE and BANNERSHOW changes
                if (m_shmm->State == 0 && (m_shmm->Changes & 1))
                        tellOther("bn", ShMemory::M_CONF);
                // AIPENABLED changes
                if (m_shmm->Changes & 2)
                        tellOther("aip", ShMemory::M_CONF);
		// XORG.CONF
		if (m_shmm->Changes & 4)
			updtXorg();
		// XINE config
		if (m_shmm->Changes & 8)
			updtXine();
		break;
		case ShMemory::M_DEBG:
		if (m_shmm->Changes & 1) m_burn = true;
		if (m_shmm->Changes & 8) {  // clean 
			m_Log->stopLog();
			tellOther("aip", SIGUSR1);
			tellOther("vds", SIGUSR1);
			tellOther("w2", SIGUSR1);
			system("rm -f /www/log/*");
		}
		if (m_shmm->Changes & 14) { // changes
			setLog();
			tellOther("aip", n);
			tellOther("vds", n);
			tellOther("w2", n);
		}
		break;
		case ShMemory::M_REST:
		if (m_shmm->Changes & 1) system("reset");
		system("xwinfo \"System is shutting down...\" 1");
		setLED(6);
		system("ka 1");		// REBOOT
		m_Log->log(LogTrace::DEBUG, "Process %d to Reboot system", n);
		stop();
		break;
		case ShMemory::M_MISC:
		break;
		case ShMemory::M_WRLS:
		m_Log->log(LogTrace::DEBUG, "process WPA setup");
		// wpa auto setup
		// system("startUp WPA UPDATE &");
		break;
		case ShMemory::M_LOG:
		uploadIMG();		// process upload image
		break;
		case ShMemory::M_APPL:
		m_Log->log(LogTrace::DEBUG, "process DHCP service setup");
//		if (m_shmm->Changes == 0) setNetwork(true);     // from xinit
		dhcpSvr();		// update conf | up
//		if (m_shmm->Changes == 0) setWiJET(true);	// from xinit
		break;
		case ShMemory::M_STOP:
		case ShMemory::M_TERM:
		m_Log->log(LogTrace::DEBUG, "Process %d to Stop WatchDog\n", n);
			stop(); 		
		default:
		break;
	}
	m_shmm->Changes = 0;
}

