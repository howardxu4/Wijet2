
/**
 *	The State class maintain the state system 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#include "Control.h"


/**
 *	The constructor
 *
 */
Control::Control()
{
        m_Log = new LogTrace("WiJET-2");
        setLog();
	m_myState = NULL;
}

/**
 *	The destructor
 *
 */
Control::~Control()
{
	if (m_myState != NULL)
		delete m_myState;
}

/**
 *      setLog method
 */
void Control::setLog()
{
        char* f = NULL;
        getProperty gP(gP.P_OTC);
        int i = LogTrace::ERROR;
        if (gP.getInt("MODULES") & 1) {		// ST 1: AI 2: MD 4:  WD 8
		i = gP.getInt("DEBUG");
                if (i != 0) f = "/www/log/state";
	}
        m_Log->setLog(i, f);
}

/**
 *	init method
 *
 */
int Control::init()
{
	int rtn;
	rtn = m_service.OpenServer(WIJET2_PORT);
	if (!rtn) {
            m_Log->log(LogTrace::ERROR, "Open server fail\n");
            return -1;
	}
	else m_myState = new State(m_Log);
	return 0;
}

/**
 *      begin method
 */
void Control::begin()
{
        if (m_myState != NULL)
                m_myState->init();
}

/**
 *	processMsg 
 */
void processMsg(SocketTCP* myClient, State* myState, unsigned long IP)
{
        int rtn;
        char buffer[250];
        memset(buffer, 0, 250);
        myClient->SetRcvTime(2000);
        rtn = myClient->Recv(buffer, 250);
        if (rtn > 0) 
	if (!myState->chkState()) {
		myState->m_Log->log(LogTrace::ERROR,"No WatchDog there"); 
	}
	else if (ntohl(IP) == INADDR_LOOPBACK) {
	myState->m_Log->log(LogTrace::INFO, "Inner process %s", buffer);
        if (strcmp(buffer, "AskIP") == 0) {        // AutoIP
                sprintf(buffer, "%u", myState->getIP());
                myClient->Send(buffer, strlen(buffer)+1);
                usleep(1000);
        }
        else if (strcmp(buffer, "Local") == 0)          // Banner
                myState->setState(State::S_LOCAL, true);
        else if (strcmp(buffer, "Banner") == 0) {       // Default
                myState->setState(State::S_IDLE, true);
                myState->setIP(0);
        }
        else if (strstr(buffer, "WiJET") != NULL) {     // VNC Viewer
                unsigned int ip;
                buffer[5] = 0;          // WiJET_peerd_address
                sscanf(&buffer[6], "%u", &ip);
                myState->setState(State::S_WIJET, true);
                myState->setIP(ip);
        }
        else if (strstr(buffer, "DeskTop") != NULL) {   // MdPlay
                sprintf(buffer, "%u_%d_%d", myState->getWindow(),
                myState->getScreenWidth(), myState->getScreenHeight());
                myClient->Send(buffer, strlen(buffer)+1);
                usleep(1000);
        }
        else if (strstr(buffer, "Window") != NULL) {    // VNC desktop
                unsigned int wd;
                int wx, wy;
                buffer[6] = 0;          // WiJET_desktop_window
                sscanf(&buffer[7], "%u_%d_%d", &wd, &wx, &wy);
                myState->setWindow(wd);
                myState->setScreenWidth(wx);
                myState->setScreenHeight(wy);
        }
        else if (strcmp(buffer, "Remote") == 0)         // X11VNC
                myState->setState(State::S_CNTRL, true);
        else if (strcmp(buffer, "IrCmd") == 0) 		// IR Cmd
		myState->procIR();
        else if (strcmp(buffer, "stop") == 0)
                printf("STOP!!!\n");
        else
                myState->m_Log->log(LogTrace::ERROR, "Receive nothing");
	}
	else {
	MsgHeader myMsgHeader(buffer);
	if (myMsgHeader.checkMsgHeader()) {
		switch(myMsgHeader.getCommand()) {
		case PSM2WiJETSetReverse:
			int wx, wy, wr;
                	sscanf(&buffer[MsgHeader::getMsgBodyStart()+7], "%u_%u %u", &wx, &wy, &wr);
                	if (wr == 0 && wx == 1024)
                        	system("xrandr -s 1024x768");
                	myState->setBack(wr);
                	myState->setIP(IP);
                	myState->setState(State::S_SVER, true);
		break;
		case PSM2WiJETSetPass:
			myState->setPass();
		break;
		case MAC2WiJETuMount:		// Added for MAC special
			system("umount -f /mnt1 2> /dev/null");
		break;
		default:
			myState->m_Log->log(LogTrace::ERROR, "This is an invalid messasge");
		break;
		}
	}
	else 
#ifdef  COMPITABLITY
	{
        int n = 0;
        myState->m_Log->log(LogTrace::DEBUG,"IP %x: %s", IP, &buffer[n]);
        if (strstr(&buffer[n], "Server") != NULL) {    // PSM
                int wx, wy, wr;
                sscanf(&buffer[n+7], "%u_%u %u", &wx, &wy, &wr);
                if (wr == 0 && wx == 1024)
                        system("xrandr -s 1024x768");
                myState->setBack(wr);
                myState->setIP(IP);
                myState->setState(State::S_SVER, true);
        }
        else if (strcmp(&buffer[n], "Pass") == 0)           // PSM
                myState->setPass();
        else
                myState->m_Log->log(LogTrace::ERROR, "Receive Wrong message");
	}
#else
	myState->m_Log->log(LogTrace::ERROR, "Receive Wrong message %s %s", buffer, &buffer[13]);
#endif
	}
        delete myClient;
}

#ifdef	USINGTHREAD
typedef struct {
        SocketTCP *tcp;
        unsigned long ip;
        State     *state;
} myArgs;

void* startClient(void *mst)
{
	myArgs *ma = (myArgs*)mst;
	State *myState = ma->state;
	SocketTCP *myClient = ma->tcp;
	unsigned long IP = ma->ip;
	myState->m_Log->log(LogTrace::INFO, "Start thread to receive from %x", IP);
	processMsg(myClient, myState, IP);
	delete ma;
	pthread_exit(0);
}

/**
 *	start method
 *
 */
int  Control::start()
{
	m_continue = 1;
	m_Log->log(LogTrace::INFO,"Service START......");
	while(m_continue) {
		pthread_t myThread;
		struct sockaddr_in addr;
		myArgs *ma = new myArgs;
        	try {
			try {
					ma->tcp = m_service.Accept(&addr);
			}
			catch(...) {
			m_Log->log(LogTrace::ERROR, "Accept Failure!");
			if (!m_service.OpenServer(WIJET2_PORT)) {
                		m_Log->log(LogTrace::ERROR, "Open server fail\n");
                		return -1;
        		}
			delete ma;
			continue;
			}
			ma->ip = *(unsigned long*)&(addr.sin_addr);
			m_Log->log(LogTrace::INFO, "New Client Addr %u %x Port %d", *(UINT*)(&addr.sin_addr), ma->ip, addr.sin_port);
			ma->state = m_myState;
			pthread_create( &myThread, NULL, startClient, (void*)ma);
        	}
		catch(...) {
			m_Log->log(LogTrace::ERROR,"exception!!");
			return -1;
		}
	}
	m_service.Close();
	m_Log->log(LogTrace::INFO,"Service SHUTDOWN!\n");
	return (0);
}
#else

/**
 *      start method
 *
 */
int  Control::start()
{
        m_continue = 1;
        m_Log->log(LogTrace::INFO,"Service START......");
        while(m_continue) {
                struct sockaddr_in addr;
		SocketTCP* tcp;
		unsigned long ip;
                try {
                        try {
                        	tcp = m_service.Accept(&addr);
                        }
                        catch(...) {
                        m_Log->log(LogTrace::ERROR, "Accept Failure!");
                        if (!m_service.OpenServer(WIJET2_PORT)) {
                                m_Log->log(LogTrace::ERROR, "Open server fail\n");
                                return -1;
                        }
                        continue;
                        }
                        ip = *(unsigned long*)&(addr.sin_addr);
                        m_Log->log(LogTrace::INFO, "New Client Addr %x %x Port %d", *(UINT*)(&addr.sin_addr), ntohl(ip), addr.sin_port);
			
			processMsg(tcp, m_myState, ip);
                }
                catch(...) {
                        m_Log->log(LogTrace::ERROR,"exception!!");
			m_service.Close();
                        return -1;
                }
        }
        m_service.Close();
        m_Log->log(LogTrace::INFO,"Service SHUTDOWN!\n");
        return (0);
}

#endif

/**
 *	stop method
 */
void Control::stop()
{
	SocketTCP tcp;
	m_continue = 0;
	m_Log->log(LogTrace::INFO,"Stop Control");
	if (tcp.Open(WIJET2_PORT, htonl(INADDR_LOOPBACK)) != 0) {
		m_Log->log(LogTrace::INFO,"Stop Control loop");
		tcp.Send("stop", 4);
		tcp.Close();
	}
	else m_Log->log(LogTrace::ERROR,"Open socket fails!");
}
