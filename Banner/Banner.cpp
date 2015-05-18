
#include "Banner.h"
#include "../WatchDog/ShMemory.h"
/**
 *      The LdFlist class load the banner file list
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

// #define USINGDEBUG
void DEBUG (char *ui) {
#ifdef USINGDEBUG
	 fprintf (stderr, ui); fflush(stderr);
#endif
}

/**
 *      The constructor
 *
 */
Banner::Banner()
{
	m_Log = new LogTrace("Banner");
	m_Log->setLog(LogTrace::OFF);
	m_continue = 1;
	m_splash = false;
	m_ethonly = false;
}

Banner::~Banner()
{
	stop();
}

#define myUDP_PORT_RECV UDP_PORT_SEND           // reverse from AUTOIP
#define myUDP_PORT_SEND UDP_PORT_RECV

int Banner::queryIpMac(char* IP, char *MAC)
{
	int n = 0;
	SocketUDP myRecv, mySend;
	if (myRecv.Open(myUDP_PORT_RECV, htonl(INADDR_LOOPBACK)) != 0
	&& mySend.Open(0, htonl(INADDR_LOOPBACK)) != 0) {
		myRecv.SetRcvTime(500);
		mySend.SetSndTime(500);
		char msg[80];
		msg[0] = 0x01;
		msg[1] = 0x06;
		*((unsigned int*) &msg[2]) = htonl(INADDR_LOOPBACK);
		*((unsigned int*) &msg[6]) = 0xFF000000;
		if(mySend.Connect(myUDP_PORT_SEND, htonl(INADDR_LOOPBACK)) 
		&& mySend.Send(msg, 10) > 0) 
			if (myRecv.Recv(msg, 80)> 0) {
				ipHandshake *sMsg = (ipHandshake*)&msg[0];
				sprintf(IP, "%s", inet_ntoa(*(struct in_addr *)(&sMsg->drp.r.ipAddr[0])));
				int i, j;
				char *p = (char*)&sMsg->drp.r.mac[0]; 
       				for (i = 0, j = 0; i< 6; i++, j=j+3) 
            				sprintf(&MAC[j], "%2.2X:", p[i] & 0xFF);
				MAC[17] = 0;
				n = ntohs(sMsg->drp.t.type);
				if (n == 65535) n = -1;
                         	m_Log->log(LogTrace::INFO, "Received from AutoIP %s [%d] ", sMsg->drp.e.dName, n);
			}
	}
	if (n == 0) {
	        strcpy(IP, "169.254.98.170");
        	strcpy(MAC, "00:11:2F:4B:FE:7E");
		m_Log->log(LogTrace::ERROR,"Can't communicate with AutoIP");
	}
	return n;
}

#define VER	"FIRMWARE_VER"
#define	SSID	"SSID"
#define CHANNEL	"Channel"
#define RFDETECT	"RF activities detected"
#define INTERFERENCE	"Interference"
#define MODE	"Mode"
#define SECURITY	"Security"
#define IPADDR	"IPADDR"

void Banner::updateBurn()
{
	ShMemory shm;
	int n = shm.getBTimes();
	if (n > 0) {
		char str[40];
		n = n * 5;	// seconds
		sprintf(str, "BurnTimes=%02d:%02d:%02d", n/3600, n/60%60, n%60);
		m_win.displayString(100, 82,  str, m_win.Green, m_win.Black);
	}
}

void Banner::updateInfo()
{
        int width, height;
        m_win.getWindowWH(&width, &height, 0);	// erase = false
	int channel;
        int x,  y = 30, w = 760;
        char str[40], buf[40], buf1[40];

        x = (width - w)/2;
	channel = queryIpMac(buf, buf1);

	if (m_ethonly) {
	m_win.displayRect(x, y-18, w, 50, m_win.Black);
        sprintf(str, "Device=%s ", m_slist.getValue(SSID));
        m_win.displayString(x+10, y, str, m_win.White, m_win.Black);
        sprintf(str, "IP=%s", m_slist.getValue(IPADDR));
        m_win.displayString(x+470, y, str, m_win.White, m_win.Black);
        sprintf(str, "Version:%s", m_vlist.getValue(VER));
        m_win.displayString(x+640,y, str, m_win.Green, m_win.Black);
        y +=20;
        sprintf(str, "Mode=%s ", m_slist.getValue(MODE));
        m_win.displayString(x+10,y,str, m_win.White, m_win.Black);
        sprintf(str, "aliasIP=%s", buf);
        m_win.displayString(x+360,y,str, m_win.Red, m_win.Black);
        sprintf(str, "MAC=%s", buf1);
        m_win.displayString(x+570,y,str, m_win.White, m_win.Black);

	}
	else {
	m_win.displayRect(x, y-18, w, 60, m_win.Black);
	sprintf(str, "SSID=%s ", m_slist.getValue(SSID)); 
        m_win.displayString(x+10, y, str, m_win.White, m_win.Black);
	sprintf(str, "IP=%s", m_slist.getValue(IPADDR));
	m_win.displayString(x+470, y, str, m_win.White, m_win.Black);
	sprintf(str, "Version:%s", m_vlist.getValue(VER));
        m_win.displayString(x+640,y, str, m_win.Green, m_win.Black);
	y +=16;
	if (channel > 0) 
	sprintf(str, "Channel=%d ", channel); // m_slist.getValue(CHANNEL));
	else sprintf(str, "Channel="); 
	m_win.displayString(x+10,y,str, m_win.White, m_win.Black);
	if (channel <=0 )m_win.displayString(x+80,y,"Scanning",m_win.Yellow, m_win.Black); 
	sprintf(str, "Mode=%s ", m_slist.getValue(MODE)); 
	m_win.displayString(x+180,y,str, m_win.White, m_win.Black);
	sprintf(str, "aliasIP=%s", buf);
        m_win.displayString(x+360,y,str, m_win.Red, m_win.Black);
	sprintf(str, "MAC=%s", buf1);
	m_win.displayString(x+570,y,str, m_win.White, m_win.Black);

	y +=16;
	sprintf(str, "RF activities detected on channels(%s)", m_slist.getValue(RFDETECT)); 
        m_win.displayString(x+10,y,str, m_win.White, m_win.Black);
	
	if (m_slist.getIntValue(INTERFERENCE) == 1) {
		sprintf(str, "WARNING:Interference");
        	m_win.displayString(x+540,y,str, m_win.Red, m_win.Black);
	}
	else {
		sprintf(str, "Interference None");
		m_win.displayString(x+540,y,str, m_win.White, m_win.Black);
	}
	if (m_slist.getIntValue(SECURITY) == 0) 
                m_win.displayRect(x+740, y-10, 10, 10, m_win.Green);
	else
        	m_win.displayRect(x+740, y-10, 10, 10, m_win.Yellow);
	}
}

void Banner::updateScreen()
{
        int width, height;
        m_win.getWindowWH(&width, &height, 1);  // erase = true
        m_win.displayImage(width, height, 1);   // center = true
	if (m_show) updateInfo();
        m_win.movePointer(1400, 1000);  // outside of window

}

void Banner::changePic(char *fname)
{
	m_win.loadImageFromFile(fname);
	updateScreen();
}

int Banner::processEvent()
{
        XEvent x_event;
        int    got_event;

        XLockDisplay(m_win.getDisplay());
        got_event = XCheckMaskEvent(m_win.getDisplay(), INPUT_MOTION, &x_event);
        XUnlockDisplay(m_win.getDisplay());

        if (got_event) {
            switch (x_event.type) {

                 case CreateNotify:
			DEBUG("CreateNotify EVENT\n");
                 break;
                 case Expose:
			DEBUG("Expose EVENT\n");
                        updateScreen();
                 break;
                 case KeyPress:
                 {
                	char s[30];
                        XKeyEvent *ke = (XKeyEvent*)&x_event;
                        sprintf(s, "Key -> %x %d\n", ke->keycode, ke->keycode);
                        DEBUG(s);
                        switch(ke->keycode) {
			case 18:			// W - 9
			m_otc = 1;
				break;
			case 13:			// I - 4
			m_otc = m_otc == 1? 2:0; 
				break;
			case 14:			// J - 5
			m_otc = m_otc == 2? 3:0;
				break;
			case 12:			// E - 3
			m_otc = m_otc == 3? 4:0;	
                                break;
			case 17:			// T - 8
			if (m_otc == 4) {
				system("xterm &");
				return -1;
			}
			break;
			case 9:                         // Esc
                                return 2;
                        break;
#ifdef	DEVELOP
			case 24:    // q
				return -1;
                        case 33:    // p
                                DEBUG("change pic\t");

                        	if(m_flist.nextFile() > 1) {
                       		m_count = getDuration();
                        	changePic(m_flist.getFilename());
                  		}
                  	break;
                 	case 46:    // l
                                DEBUG("chnage to local control\n");
				return 2;
                        break;
#endif
			default:
			m_otc = 0;
			break;
                   	}
                 }
                 break;
  		 case ButtonPress:
			DEBUG("Button Clicked %d\n");
		 break;
		 case MotionNotify:
		 break;
		 default:
		 {
		//	char s [30];
		//	sprintf(s, "Type of %d Event\n", x_event.type);
		//	DEBUG(s);
		 }
            }
            return 0;
        }
        return 1;
}

#define BANNERSHOW      "BNSHOW"
#define OTC_CONFIG      "/etc/otc_defaults.conf"

void Banner::updtShow(bool f)
{
	if (m_splash) m_show = false;
	else {
        m_show = true;
	LdProp clist;
        if (clist.loadFromFile(OTC_CONFIG) == 0) {
                 m_Log->log(LogTrace::ERROR,"Fail to load config file %s\n", OTC_CONFIG);
        }
        else if (clist.getIntValue(BANNERSHOW) == 0) m_show = false;
	if (f) changePic(m_flist.getFilename());
	}
}

int Banner::start()
{

#define VERSION		"/etc/version"
#define BANNER_LIST     "/etc/banners.txt"
#define SCAN_TXT	"/etc/scan.txt"

        char          display[128];

	if (m_vlist.loadFromFile(VERSION) == 0) {
		m_Log->log(LogTrace::ERROR,"Fail to load version file %s\n", VERSION);
	}
	m_slist.noComment(SSID);
	if (m_slist.loadFromFile(SCAN_TXT) == 0) {
		m_Log->log(LogTrace::ERROR,"Fail to load scan text %s\n", SCAN_TXT);
	}
        if (m_flist.loadList(BANNER_LIST) == 0){
		m_Log->log(LogTrace::ERROR,"Fail to load file list %s\n", BANNER_LIST);
                return -1;
        }
	m_Log->log(LogTrace::INFO, "open file OK");

	if (strcmp(m_slist.getValue(MODE), "Ethernet") == 0)
		m_ethonly = true;

	// default display 
        memset (display, 0, sizeof (display));
        snprintf (display, sizeof (display), ":0.0");

        m_win.openConnection (display);
        if (!m_win.getDisplay()) {
		m_Log->log(LogTrace::ERROR, "Can't open display %s!\n", display);
                return -2;
        }

	m_win.initWindow(0, 0, m_win.getScreenWidth(), m_win.getScreenHeight(),
		INPUT_MOTION);

	m_Log->log(LogTrace::INFO, "Screen W: %d H: %d", m_win.getScreenWidth(), m_win.getScreenHeight());

        m_win.loadImageFromFile(m_flist.getFilename());
	if (m_win.getXImage() == NULL) {
                m_Log->log(LogTrace::ERROR,"Load image fails\n");
        	return -3;
	}
	updtShow();

        m_count = getDuration();
	return m_count;
}

void Banner::loop()
{
	m_win.showWindow(0, 0, m_win.getScreenWidth(), m_win.getScreenHeight());
	m_Log->log(LogTrace::DEBUG, "InLOOP:");
	
        while (m_continue) {
	    int n = processEvent();
            if ( n != 1 ){
                 if (n < 0) m_continue = 0;
		 if (n > 1) {  // special return
			m_continue = n;
			break;
		 }	 
            }
	    else {
		usleep(1000000); 
		m_count--;
		if (m_count <=0) {
                	if(m_flist.nextFile() > 1) {
                		m_count = getDuration();
                		changePic(m_flist.getFilename());
			}
		}
		else if(m_count%5 == 4 && m_show) updateInfo();
	    }
        }
}

void Banner::tellService()
{
	SocketTCP tcp;
	try {
        if (tcp.Open(WIJET2_PORT, htonl(INADDR_LOOPBACK)) != 0) {
                if(tcp.Send("Local", 5) < 1)
			 m_Log->log(LogTrace::ERROR, "Send fails");
		else
			 m_Log->log(LogTrace::INFO, "Sending ...(Local)");
		sleep(1);
		tcp.Close();
	}
        else  m_Log->log(LogTrace::ERROR, "error to open socket???");
	}
	catch(...) {
		m_Log->log(LogTrace::ERROR,"Exception!");
	}
}

int Banner::splash()
{	
        char          display[128];

#define	SPLASH_LIST	"/etc/splash.txt"
	m_splash = true;

        if (m_flist.loadList(SPLASH_LIST) == 0){
                m_Log->log(LogTrace::ERROR,"Fail to load file list %s\n", SPLASH_LIST);
                return -1;
        }
        m_Log->log(LogTrace::INFO, "open file OK");

        // default display
        memset (display, 0, sizeof (display));
        snprintf (display, sizeof (display), ":0.0");

        m_win.openConnection (display);
        if (!m_win.getDisplay()) {
                m_Log->log(LogTrace::ERROR, "Can't open display %s!\n", display);
                return -2;
        }

        m_win.initWindow(0, 0, m_win.getScreenWidth(), m_win.getScreenHeight(),
                INPUT_MOTION);
        m_win.loadImageFromFile(m_flist.getFilename());
        if (m_win.getXImage() == NULL) {
                m_Log->log(LogTrace::ERROR,"Load image fails\n");
                return -3;
        }
        m_count = getDuration();
        return m_count;

}
