/*
** Copyright (C) 2003 Daniel Caujolle-Bert <segfault@club-internet.fr>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
* $Id: muxine.c,v 1.3 2004/12/27 19:48:03 miguelfreitas Exp $
*/

#include "myXine.h"

/**
 *	global variablesa - used in callback methods
 */

static myGX	gx;

LogTrace*	m_log = NULL;

void myXineSetLog(LogTrace* log)
{
	m_log = log;
}

void printOut(int level, char* format,...)
{
	char message[512];
	va_list ap;
        va_start(ap, format);
	vsprintf(message, format, ap);
	if (m_log == NULL) {
		if (level) fprintf(stderr, "%s\n",  message);
		else printf("%s", message);
	}
	else 
		m_log->log(level?LogTrace::ERROR:LogTrace::INFO, message);
}

/**
 *      myXineSvr - the server of control
 */
void* myXineSvr(void *p)
{
        SocketTCP myTCP;
        if(!myTCP.OpenServer(XINE_SVR_PORT)) {
                printOut(1, "Open server fails on port %d", XINE_SVR_PORT);
        }
	else {
	printOut(0, "Xine Server starting..."); 

        while(gx.running == 1) {
                struct sockaddr_in addr;
                char buffer[250];
                int rtn;
        try {
                SocketTCP* myClient =  myTCP.Accept(&addr);
                printOut(1,"New Client Addr %u Port %d\n", *(UINT*)(&addr.sin_addr), addr.sin_port);
                rtn = myClient->Recv(buffer, 250);
                if (rtn > 0) {
                        buffer[rtn] = 0;
			printOut(0, "Recv: %s", buffer);
                        if (strstr(buffer, "stop"))
                                xine_stop (gx.stream);
                        else if (strstr(buffer, "speed")) {
//STOP:PAUSE:RESUME:SLOW:FAST:EXIT
                                int speed = atoi(&buffer[6]);
                                xine_set_param(gx.stream, XINE_PARAM_SPEED, speed);                        }
			else if (strstr(buffer, "query")) {
				int status = xine_get_status(gx.stream);
				sprintf(buffer, "Status = %d\n", status);
				printOut(0, buffer);
				myClient->Send(buffer, strlen(buffer)+1);
			}
                        else {
                                xine_stop (gx.stream);
                                printOut(0, "STOP: %s", buffer);
                                gx.running = -1;
                        }
                }
		myClient->Close();
                delete myClient;
        }
        catch(...) {
                printOut(1,"exception!!");
        }
        }	// end of while
	myTCP.Close();
	}	// end of else
        pthread_exit(0);
}

/*
 *      dest_size_cb - call back required
 */
static void dest_size_cb(void *data, int video_width, int video_height, double video_pixel_aspect,
			 int *dest_width, int *dest_height, double *dest_pixel_aspect)  {

  *dest_width        = gx.width;
  *dest_height       = gx.height;
  *dest_pixel_aspect = gx.pixel_aspect;
}

/*
 *      frame_output_cb - call back required
 */
static void frame_output_cb(void *data, int video_width, int video_height,
			    double video_pixel_aspect, int *dest_x, int *dest_y,
			    int *dest_width, int *dest_height, 
			    double *dest_pixel_aspect, int *win_x, int *win_y) {
  *dest_x            = 0;
  *dest_y            = 0;
  *win_x             = gx.xpos;
  *win_y             = gx.ypos;
  *dest_width        = gx.width;
  *dest_height       = gx.height;
  *dest_pixel_aspect = gx.pixel_aspect;
}

/*
 *	event_listener - call back required
 */
static void event_listener(void *user_data, const xine_event_t *event) {
  switch(event->type) { 
  case XINE_EVENT_UI_PLAYBACK_FINISHED:
    printOut(0, "FINISH: 0");
    gx.running = 0;
    break;
  }
}
  
int myXineInit(int on)
{
  char              configfile[2048];

  if(!XInitThreads()) {
    printOut(0, "XInitThreads() failed");
    return -1;
  }

  gx.xine = xine_new();
  sprintf(configfile, "%s%s", xine_get_homedir(), "/.xine/config");
  xine_config_load(gx.xine, configfile);
  if (on)
  	xine_engine_set_param(gx.xine, XINE_ENGINE_PARAM_VERBOSITY,1);
  xine_init(gx.xine);

  printOut(0, "myXine: Initialized");
  return 0;
}

int myXineWindow(int x, int y, int w, int h, Window parent)
{
  gx.q.openConnection (getenv("DISPLAY"));
  if (!gx.q.getDisplay()) {
    printOut(1, "XOpenDisplay() failed.");
    return -1;
  }

  gx.q.setParent(parent);

  if (w <= 10 || h <= 10) {
    gx.xpos = gx.ypos = 0;
    gx.width = DisplayWidth(gx.q.getDisplay(), gx.q.getScreen());
    gx.height = DisplayHeight(gx.q.getDisplay(), gx.q.getScreen());
  }
  else {
    gx.xpos = x;
    gx.ypos = y;
    gx.width = w;
    gx.height = h;
  }

  gx.q.initWindow(gx.xpos, gx.ypos, gx.width, gx.height, INPUT_MOTION);

  gx.pixel_aspect = gx.q.getAspect();
  printOut(0, "myXine: Open Window");
  return 0;
}

int myXineDriver(char* vodriver, char* aodriver)
{
  char         *vo_driver    = "auto";
  char         *ao_driver    = "auto";

  x11_visual_t      vis;

  vis.display           = gx.q.getDisplay();
  vis.screen            = gx.q.getScreen();
  vis.d                 = gx.q.getWindow();
  vis.dest_size_cb      = dest_size_cb;
  vis.frame_output_cb   = frame_output_cb;
  vis.user_data         = NULL;

  if (vodriver != NULL) vo_driver = vodriver;
  if (aodriver != NULL) ao_driver = aodriver;

  if((gx.vo_port = xine_open_video_driver(gx.xine,
       vo_driver, XINE_VISUAL_TYPE_X11, (void *) &vis)) == NULL) {
    printOut(1, "I'm unable to initialize '%s' video driver. Giving up.", vo_driver);
    return -1;
  }

  gx.ao_port     = xine_open_audio_driver(gx.xine , ao_driver, NULL);

  gx.stream      = xine_stream_new(gx.xine, gx.ao_port, gx.vo_port);
  gx.event_queue = xine_event_new_queue(gx.stream);
  xine_event_create_listener_thread(gx.event_queue, event_listener, NULL);

  printOut(0, "Created listener thread for A %s V %s", ao_driver, vo_driver);
  xine_gui_send_vo_data(gx.stream, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) gx.q.getWindow());
  xine_gui_send_vo_data(gx.stream, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);

  printOut(0, "myXine: Driver installed");
  return 0;
}

int myXinePlay(char* mrl)
{
  pthread_t myTread;
  if(!xine_open(gx.stream, mrl)) {
    printOut(1, "Unable to open mrl '%s'", mrl);
    return 0;
  }
  printOut(0, "Open mrl '%s'", mrl);

//  xine_set_param(gx.stream, XINE_PARAM_AV_OFFSET, 0);
//  xine_set_param(gx.stream, XINE_PARAM_SPU_OFFSET, 0);

  gx.q.showWindow(gx.xpos, gx.ypos, gx.width, gx.height);

  if (!xine_play(gx.stream, 0, 0)) {
    printOut(1, "Unable to play mrl '%s'", mrl);
    return 0;
  }
  printOut(0, "Set RUNNING...");
  gx.running = 1;
  pthread_create( &myTread, NULL, myXineSvr, NULL);

  
  while(gx.running == 1) {
    XEvent xevent;
    int    got_event;
    XLockDisplay(gx.q.getDisplay());
    got_event = XCheckMaskEvent(gx.q.getDisplay(), INPUT_MOTION, &xevent);
    XUnlockDisplay(gx.q.getDisplay());

    if( !got_event ) {
      xine_usec_sleep(20000);
      continue;
    }

    switch(xevent.type) {

    case Expose:
      if(xevent.xexpose.count != 0)
        break;
      xine_gui_send_vo_data(gx.stream, XINE_GUI_SEND_EXPOSE_EVENT, &xevent);
      break;
    case ConfigureNotify:
      {
        XConfigureEvent *cev = (XConfigureEvent *) &xevent;
        Window           tmp_win;

        gx.width  = cev->width;
        gx.height = cev->height;

        if((cev->x == 0) && (cev->y == 0)) {
          XLockDisplay(gx.q.getDisplay());
          XTranslateCoordinates(gx.q.getDisplay(), cev->window,
                                DefaultRootWindow(cev->display),
                                0, 0, &gx.xpos, &gx.ypos, &tmp_win);
          XUnlockDisplay(gx.q.getDisplay());
        }
        else {
          gx.xpos = cev->x;
          gx.ypos = cev->y;
        }
      }
      break;
     case KeyPress:
        {
        XKeyEvent *kev = (XKeyEvent*)&xevent;
        if (kev->keycode == 24 || kev->keycode == 9)  // q | esc
                gx.running = 0;
        }
        break;
    }
  }
  printOut(0, "End of loop!!!");
  return 0;
}

void myXineCleanUp()
{
	if (gx.running >= 0) {
//		Sender mySender(XINE_SVR_PORT);
//		mySender.Send("exit", 4);
		usleep(40000);
	}
	xine_close(gx.stream);
	xine_event_dispose_queue(gx.event_queue);
	xine_dispose(gx.stream);
	if(gx.ao_port)
		xine_close_audio_driver(gx.xine, gx.ao_port);
	xine_close_video_driver(gx.xine, gx.vo_port);

	gx.q.closeWinObject();
	printOut(0, "myXine: Cleaned up");
}

/**
 *	myXineExit method
 */
void myXineExit()
{ 
	xine_exit(gx.xine);
}

/**
 *	myXine	- the API of Xine player
 *
 *	@param x - postion x
 *	@param y - positon y
 *	@param w - width
 *	@param h - height
 *	@param mrl - media resource locator
 *	@param vo_driver - default auto
 *	@param ao_driver - default auto
 */
int myXine(int x, int y, int w, int h, char* mrl, char* vo_driver, char* ao_driver)
{
  if (myXineInit() < 0)
     return 0;

  if (myXineWindow(x, y, w, h) < 0)
    return 0;

  if (myXineDriver(vo_driver, ao_driver) < 0)
    return 0;


  myXinePlay(mrl);

  myXineCleanUp();

  myXineExit();
  return 1;
}

/*
 *	parse_geometry 
 *
 *	@param	geomstr - WxH+x+y
 *	@param  rect - geometry holder
 */
int parse_geometry(char *geomstr, int rect[])
{
  unsigned int w, h;
  int  x, y, ret;
  if ((ret = XParseGeometry(geomstr, &x, &y, &w, &h))) {
    if(ret & XValue)
        rect[0] = x;
    if(ret & YValue)
        rect[1] = y;
    if(ret & WidthValue)
        rect[2] = w;
    if(ret & HeightValue)
        rect[3] = h;
    return 1;
  }
  return 0;
}

#ifdef  USING_MAIN

/*
 *	main - Main program
 */
int main(int argc, char **argv) {
  int          i;
  char         *vo_driver    = "auto";
  char         *ao_driver    = "auto";
  char         *mrl = NULL;
  int	       rect[4] = { 300, 100, 510, 400 };

  if(argc <= 1) {
    printf("specify one mrl\n");
    return 0;
  }

  if(argc > 1) {
    for(i = 1; i < argc; i++) {
      if(!strcmp(argv[i], "-V")) {
        vo_driver = argv[++i];
      }
      else if(!strcmp(argv[i], "-A")) {
        ao_driver = argv[++i];
      }
      else if(!strcmp(argv[i], "-G")){
        if(!parse_geometry(argv[++i], rect)) {
           printf("Bad geomtry '%s'\n", argv[i-1]);
           return 0;
        }
      }
      else if((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")) || argv[i][0] == '-' ) {
        printf("Options:\n");
        printf("  -A <ao name>           Audio output plugin name (default = alsa).\n");
        printf("  -V <vo name>           Video output plugin name (default = Xv).\n");
        printf("  -G <WxH[+X+Y]>         geometry <WxH[+X+Y]>\n");
        return 0;
      }
      else {
        mrl = argv[i];
      }
    }
  }
  else
    mrl = argv[1];

  return myXine(rect[0], rect[1], rect[2], rect[3], mrl, vo_driver, ao_driver);

  return 1;
}

/*
 * Local variables:
 * compile-command: "gcc -Wall -O2 `xine-config --cflags` `xine-config --libs` -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm -o muxine muxine.c"
 * End:
 */
#endif

