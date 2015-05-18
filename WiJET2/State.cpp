

/**
 *	The State class maintain the state system 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#include "State.h"

#include <pthread.h>

char TOOLDIR[80];

/**
 *	The constructor
 *
 */
State::State(LogTrace* log)
{
	LdEnv Env;
	strcpy(TOOLDIR, Env.getPath());
	m_IP = 0;
	m_state = S_EXIT;
	m_pass = 0;
	if (log!=NULL) m_Log = log; 
	else m_Log = new LogTrace("State");
	m_Log->log(LogTrace::INFO, "Path = %s", TOOLDIR);
	m_shm.setState(m_state);
}

/**
 *	The destructor
 *
 */
State::~State()
{
	setState(S_EXIT);
	delete m_Log;
}

/**
 *	init method
 *
 */
void State::init()
{
	m_ScreenWidth = 800;
	m_ScreenHeight = 600;
	m_Log->log(LogTrace::INFO, "start %s ...", "WiJET2");
	setState(S_IDLE, true);
}

void *invoke_application(void *name)
{
	char cmd[80];
	sprintf(cmd, "%s/%s", TOOLDIR, (char*)name);
        system(cmd);
        pthread_exit(0);
}

void kill_application(char* app)
{
	char cmd[80];
	sprintf(cmd, "pkill %s", app);
	system(cmd);
	// double check the killing
	sprintf(cmd, "kill -9 `pgrep %s` 2 >/dev/null", app);
	system(cmd);
}

/**
 *	setState method
 *
 *	@param	state
 */
int State::setState(int state, bool kill)
{
    pthread_t myThread;
	char *msg;
	if (state == S_IDLE && state == m_state) return m_state;
	if (kill) {
		if(m_state == S_EXIT || m_state == S_IDLE || state == S_SVER || state == S_VWER) 
			kill_application("bn");
		if(m_state == S_LOCAL || m_state == S_CNTRL) {
			kill_application("ctl");
			kill_application("x11vnc");
			kill_application("xvkbd");
                	kill_application("xine");       // Add on the last
			system("umount -f /mnt1 2> /dev/null");
		}
	}
	switch(state) {
		case S_LOCAL:		// receive from Banner
		pthread_create( &myThread, NULL, invoke_application, (void*)"ctl");
		msg = "Local Control";
		break;
		case S_WIJET:		// receive from vncviewer
		kill_application("xine");		// Secured
		msg = "WiJET";
		break;
		case S_CNTRL:		// receive from x11vnc
		if (m_back)
		pthread_create( &myThread, NULL, invoke_application, (void*)"ctl 1 2");
		else
                pthread_create( &myThread, NULL, invoke_application, (void*)"ctl 1");
		msg = "Remote Control";
		break;
		case S_SVER:		// receive from PSM
		sprintf(m_cmd, "x11vnc -q -nopw -nolookup -connect %s", inet_ntoa(*((in_addr*)&m_IP)));
		pthread_create( &myThread, NULL, invoke_application, (void*)m_cmd);
		msg = "SERVER";
		break;
		case S_VWER:		// receive from Control
                pthread_create( &myThread, NULL, invoke_application, (void*)"bn");
		msg = "VIEWER";
		break;
		case S_EXIT:
		msg = "EXIT";
		break;
		case S_IDLE:		// receive from everyone
		default:	
		if (m_state == S_CNTRL) {
			informPSM();	// Inform PSM to close VncViewer
			if (m_back)	// Back to PC Desktop
				setConnectivity();
		}
                system("xrandr -s 800x600");
		if ((m_state == S_CNTRL && m_back) || m_pass)
		pthread_create( &myThread, NULL, invoke_application, (void*)"bn splash");
		else {
//			system("xrandr -s 800x600");
			pthread_create( &myThread, NULL, invoke_application, (void*)"bn");
		}
		m_pass = 0;
		msg = "Banner Page";
		break;
	}
        m_Log->log(LogTrace::INFO, "switch to %s", msg);
	m_state = state;
	m_shm.setChanges(state);	// Let WatchDod to change for security 
	m_shm.tellWD(ShMemory::M_STAT);
	return (m_state);
}

void State::procIR()
{
    pthread_t myThread;
	m_Log->log(LogTrace::INFO, "Process IR on state %d", m_state);
	if (m_state == S_IDLE)
		setState(S_LOCAL, true);
	else if(m_state == S_LOCAL || m_state == S_CNTRL) {
	// if file open or browser and no xvkbd then invok xvkbd
		pthread_create( &myThread, NULL, invoke_application, (void*)"sftkbd");

	}
}

// communication with connected PSM


void State::CommandPSM(char cmd)
{

#define	OTCPSM_PORT	34302

#define MSG_BODY	12

        if (m_IP != 0) {
	MsgHeader myMsgHeader(COMANDSETPSM2PSM);
        char msg[MSG_BODY];
        Sender mySender(OTCPSM_PORT);
        mySender.SetIP(inet_ntoa(*((in_addr*)&m_IP)), INADDR_LOOPBACK);

	myMsgHeader.setCommand(cmd, m_IP);
	memcpy(msg, myMsgHeader.getMsgHeader(), myMsgHeader.getMsgHeaderLength());
	*(short*)&msg[myMsgHeader.getMsgHeaderLength()] = 0;
        if(mySender.Send(msg, MSG_BODY) <= 0)
                m_Log->log(LogTrace::INFO, "Failure in sending close to %s",
                        inet_ntoa(*((in_addr*)&m_IP)));
        }
}

void State::setConnectivity(bool mode)
{
	CommandPSM(mode?PSM2PSMEstablish:PSM2PSMDisconnect);
}

void State::informPSM()
{
	CommandPSM(WiJET2PSMCommandInfo);
}

