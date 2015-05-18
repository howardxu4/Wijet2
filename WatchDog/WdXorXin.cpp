
#include "WatchDog.h"
/**
 *      The methods of WatchDog class 
 *
 *  	@author     Howard Xu
 *	@version        2.6
 */

#define XORG_CONF       "/etc/X11/xorg.conf"

// VDEVICE=0               # Auto | CRT | TV
// TVMODE=0                # NTSC | PAL
// AUDIOTYPE=0             # Stero 2.0| 2.1 | Surround 5.0| 5.1| Pass Through
// COLORDEPTH=1            # Medium 16 bit | Highest 24 bit

#define VDEVICE         "VDEVICE"
#define TVMODE		"TVMODE"
#define COLORDEPTH	"COLORDEPTH"

void WatchDog::updtXorg()
{
        int i, j, k;
        char line[100];
        LdConf  clist;
        getProperty gP(gP.P_OTC);

        clist.loadFromFile(XORG_CONF);
        clist.parSection();
        i = clist.getIndex("\"Device\"", OneLine::GSTR, 0);
        if (i > 0) {
		int n = gP.getInt(VDEVICE);
                j = clist.getIndex("ActiveDevice", OneLine::ALLS, i);
                if (j > 0) {
			sprintf(line, "\tOption      \"ActiveDevice\" \"%s\"\n", n==0?"CRT,TV": n==1?"CRT":"TV");
                        clist.chgLine(j, line);
			m_Log->log(LogTrace::DEBUG, "[%d] %s", j, clist.getLine(j));
                }
                j = clist.getIndex("TVType", OneLine::ALLS, i);
                if (j > 0) {
                        k = gP.getInt(TVMODE);
                        sprintf(line, "\tOption      \"TVType\" \"%s\"\n", k?"PAL":"NTSC");
                        clist.chgLine(j, line);
                        if (n != 2) clist.changeType(j);
			m_Log->log(LogTrace::DEBUG, "[%d] %s", j, clist.getLine(j));
                }
                j = clist.getIndex("TVOutput", OneLine::ALLS, i);
                if (j > 0) {
                        sprintf(line, "\tOption      \"TVOutput\" \"S-Video\"\n");
                        clist.chgLine(j, line);
                        if (n != 2) clist.changeType(j);
			m_Log->log(LogTrace::DEBUG, "[%d] %s", j, clist.getLine(j));
                }
        }
        i = clist.getIndex("\"Screen\"", OneLine::GSTR, 0);
        if (i > 0) {
                j = clist.getIndex("DefaultDepth", OneLine::ALLS, i);
                if (j > 0) {
                        k = gP.getInt(COLORDEPTH);
                        sprintf(line, "\tDefaultDepth     %d\n",  k?24:16);
                        clist.chgLine(j, line);
			m_Log->log(LogTrace::DEBUG, "[%d] %s", j, clist.getLine(j));
                }

        }
        clist.saveToFile(XORG_CONF);
}

#define XINE_CONF       "/root/.xine/config"

#define AUDIOTYPE       "AUDIOTYPE"

char* opts[] = {
        "Stereo 2.0",
        "Stereo 2.1",
        "Surround 5.0",
        "Surround 5.1",
        "Pass Through",
        NULL
};

void WatchDog::updtXine()
{
        int i, k;
        char line[100];
        LdConf  clist;

        getProperty gP(gP.P_OTC);
        clist.loadFromFile(XINE_CONF);
        i = clist.getIndex("audio.output.speaker_arrangement", OneLine::ALLS);
        if (i > 0) {
                k = gP.getInt(AUDIOTYPE);
		if (strstr(clist.getLine(i), opts[k]) == NULL) {
                sprintf(line, "audio.output.speaker_arrangement:%s",  opts[k]);
                clist.chgLine(i, line);
		m_Log->log(LogTrace::DEBUG, "[%d] %s", i, clist.getLine(i));
                clist.saveToFile(XINE_CONF);
		}
        }
	else m_Log->log(LogTrace::ERROR, "Can't find audto.output.speaker");
}
