
#include "XwinObject.h"


void DEBUG (char *ui) { 
#ifdef OUTPUT
fprintf (stderr, ui); fflush(stderr);
#endif
}

#define DELTA 	3
#define CHWIDTH	9
#define CHEIGHT	18
 
int l = 0;		// max length
int c = 0;		// color
int p = 1;		// max line
char   display[512];	// display content


void updateScreen(XwinObject &q)
{
	int clr = c==0?q.Green:c==1?q.Yellow:q.Red;
	int fclr = c==0?q.Blue:c==1?q.Red:q.White;
        q.displayRect(DELTA, DELTA, 2*DELTA+l*CHWIDTH, 2+CHEIGHT*p, clr);
	char *t = &display[0];
	for(int i=1; i<=p; i++) {
		q.displayString(DELTA+2, i*CHEIGHT,t,fclr, clr);
		if (i<p) t += strlen(t)+1;
	}
	DEBUG("EVENT: update screen\n");
}

int processEvent(XwinObject &q)
{
        XEvent x_event;
        int    got_event;

            XLockDisplay(q.getDisplay());
            got_event = XCheckMaskEvent(q.getDisplay(), INPUT_MOTION, &x_event);
            XUnlockDisplay(q.getDisplay());

            if (got_event) {
                switch (x_event.type) {

                     case CreateNotify:
	                DEBUG("Create EVENT\n");
                     break;
                     case Expose:
                        updateScreen(q);
                     break;
		     case ConfigureNotify:
			DEBUG("Configure Notify\n");
		     break;
                     case KeyPress:
                        {
                        char s[30];
                        XKeyEvent *ke = (XKeyEvent*)&x_event;
                        sprintf(s, "Key -> %x %d\n", ke->keycode, ke->keycode);
                        DEBUG(s);
                        switch(ke->keycode) {
                            case 24:    // q
                                exit(0);
                            break;
                        }
                     }
                     break;
                }
                return 0;
        }
        return 1;
}

void parse()
{
	char *t = &display[0];
	int i = 0;
	while(*t != 0) {
		if (*t == ';') {
			if (l < i) l = i;
			p++;
			*t = 0;		
			i = 0;	
		}
		else i++;
		t++;
	}
	if (l < i) l = i;
}

int main (int argc, char **argv)
{
	XwinObject q;
	int x, y, n;

	/* default display */
        memset (display, 0, sizeof (display));
        snprintf (display, sizeof (display), ":0.0");

        q.openConnection (display);
        if (!q.getDisplay()) {
                fprintf (stderr, "Can't open display %s!\n", display);
                return 0;
        }

	if (argc == 1) 
		sprintf(display, "Usage: %s text [color]; text -- display information;    separate line with semi-colon; color -- 0 | 1 | 2 (Green, Yellow, Red);    default is Green.", argv[0]);
	else strcpy(display, argv[1]);
	if (argc > 2) c = atoi(argv[2]);

	q.initWindow(0, 0, 1, 1, INPUT_MOTION, false);
	x = q.getScreenWidth();
	y = q.getScreenHeight();
	parse();
	if (p == 1)
	q.showWindow(x-50-l*CHWIDTH,y-60,
		4*DELTA+l*CHWIDTH, CHEIGHT+2*DELTA +4);
	else
	q.showWindow((x-l*CHWIDTH)/2,(y-CHEIGHT)/2,
		 4*DELTA+l*CHWIDTH, p*CHEIGHT+2*DELTA +4);
	updateScreen(q);
	DEBUG("flush: InLOOP:\n");

	n = p*2 + l*p/4;	
	if (n > 20) n = 15;
	else if(n > 10) n = 10;

        while (n) {
	    if ( processEvent(q) != 1) {
            
            }
	    else {
		n--;
		sleep(1);
	    }
        }

	DEBUG("Close Xwindow connection\n");
	q.closeWinObject();
	exit(0);
        return 0;
}

