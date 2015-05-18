

/**
 *      The myXine class
 *
 * 	@author     Howard Xu
 *      @version        2.5
 */

#if !defined(_MYXINE_API_INCLUDED_DEFINED)
#define _MYXINE_API_INCLUDED_DEFINED

#include "../XwinObj/XwinObject.h"
#include "../myNetLib/myNetWK.h"
#include "../myNetLib/Sender.h"
#include "../LogTrace/LogTrace.h"

#include <xine.h>
#include <xine/xineutils.h>

#define XINE_SVR_PORT	6789

typedef struct {
	int 		xpos, ypos, width, height;
	double 		pixel_aspect;
	int    		running;
	XwinObject      q;
	xine_t              *xine;
	xine_stream_t       *stream;
	xine_video_port_t   *vo_port;
	xine_audio_port_t   *ao_port;
	xine_event_queue_t  *event_queue;
} myGX;

extern void myXineSetLog(LogTrace* log);

extern int myXineInit(int on=0);
extern int myXineWindow(int x, int y, int w, int h, Window parent=0);
extern int myXineDriver(char *vo, char *ao);
extern int myXinePlay(char *mrl);
extern void myXineCleanUp();
extern void myXineExit();

extern int myXine(int x, int y, int w, int h, char* mrl, char* vo_driver, char* ao_driver);

extern int parse_geometry(char *geomstr, int rect[]);

#endif	// _MYXINE_API_INCLUDED_DEFINED

