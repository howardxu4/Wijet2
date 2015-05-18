

/**
 *      The myXine class
 *
 *      @author     Howard Xu
 *      @version        2.5
 */

#include "myXine.h"
#include "../LogTrace/LogTrace.h"

/*
 *      main - Main program
 */
int main(int argc, char **argv) {
  int          i, n=0;
  char         *vo_driver    = "auto";
  char         *ao_driver    = "auto";
  char         *mrl = NULL;
  int          rect[4] = { 300, 100, 510, 400 };
  Window       parent = 0;
  LogTrace*    m_Log = new LogTrace("MyXinePlay");
  m_Log->setLog(LogTrace::ERROR);   
  
  if(argc <= 1) {
    printf("%s: specify one mrl\n", argv[0]);
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
      else if (!strcmp(argv[i], "-P")) {
	parent = atol(argv[++i]);
      }
      else if (!strcmp(argv[1], "-D")) {
	n = 1;
      }
      else if((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")) || argv[i][0] == '-' ) {
        printf("Options:\n");
        printf("  -A <ao name>           Audio output plugin name (default = alsa).\n");
        printf("  -V <vo name>           Video output plugin name (default = Xv).\n");
        printf("  -G <WxH[+X+Y]>         geometry <WxH[+X+Y]>\n");
	printf("  -P <window> 		 parent winodw\n");
	printf("  -D			 dubug info turn on\n");
        return 0;
      }
      else {
        mrl = argv[i];
      }
    }
  }
  else
    mrl = argv[1];
#if 0
  return myXine(rect[0], rect[1], rect[2], rect[3], mrl, vo_driver, ao_driver);
#else
printf("***********************\n");

  myXineSetLog(m_Log);

  m_Log->log(LogTrace::INFO, "Start..");
  if (myXineInit(n) < 0)
     return 0;

  if (myXineWindow(rect[0], rect[1], rect[2], rect[3]) < 0)
    return 0;

  if (myXineDriver(vo_driver, ao_driver) < 0)
    return 0;

  myXinePlay(mrl);

  myXineCleanUp();

/*
  if (myXineWindow(0, 0, 0, 0) < 0)
    return 0;

  if (myXineDriver(vo_driver, ao_driver) < 0)
    return 0;

  myXinePlay(mrl);

  myXineCleanUp();
*/
   myXineExit();
  m_Log->log(LogTrace::INFO, "Done");

  return 1;


#endif
}

/*
 * Local variables:
 * compile-command: "gcc -Wall -O2 `xine-config --cflags` `xine-config --libs` -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm -o mx myMain.cpp myxine.cpp"
 * End:
 */

