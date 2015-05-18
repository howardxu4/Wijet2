#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>

// Include the necessary Motif Toolkit header files 
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>

#include <pthread.h>
#include "../myNetLib/myNetWK.h"

static char *names[] = {
        "Internet Browswer    ",
//	"Media Player         ", 
	"Configuration        ",
	"Back to Banner Page  ",
       0
  };

enum {  ITERNET_BROWSER = 0,
//	VIDEO_PLAYER,
        CONFIGURATION,
        EXIT_PROGRAM
        };

static int values[] = {
	ITERNET_BROWSER,
//      VIDEO_PLAYER,
        CONFIGURATION,
        EXIT_PROGRAM
        };

enum {	XINEAPP,
	FIREFOX,
	OTHER
};
	
static char *pnames[] = {
	"xine",
	"firefox",
	0
};

#define  USBDIR	"/mnt"

Widget fileDialog, errorDialog;
char cmdline[200];
int scrnX = 800;
int scrnY = 600;

void cleanAll()
{
        char cmd[200];
        int i = 0;
        while (pnames[i]) {
                sprintf(cmd,"pkill %s", pnames[i++]);
                system(cmd);
        }
}

void *invoke_application(void *name)
{
        // tell control ceter
        // clean process of internet browser or video player
        cleanAll();
        // start program
        if (name != NULL)
                system((char*)name);
        sleep(1);
        pthread_exit(0);
}

void setThread(char* cmdline)
{
        pthread_t myThread;
        pthread_create( &myThread, NULL, invoke_application, (void*)cmdline);
}

void Open()
{
	// mount usb and set path to there
	XmString cdir;
	sprintf(cmdline, "umount -f %s; mount /dev/sda1 %s;", USBDIR, USBDIR);
	system(cmdline);
	sleep(1); 
	cdir = XmStringCreateSimple(USBDIR);
	XtVaSetValues(fileDialog, XmNdirectory, cdir, NULL);
	XmStringFree(cdir);
	XtManageChild(fileDialog);
}

void Cancel(Widget w, XtPointer a, XtPointer b)
{
	XtUnmanageChild(fileDialog);
}

void OpenFile(Widget w, XtPointer a, XtPointer b)
{
	FILE *fp;
	char *filename;
	XmString filename_sel_text, err_mess;

	// Put the name of the file to be opened in to the variable filename 
	XtVaGetValues(fileDialog, XmNtextString, &filename_sel_text, NULL);
	XmStringGetLtoR(filename_sel_text, XmSTRING_DEFAULT_CHARSET, &filename);
	XmStringFree(filename_sel_text);

	if ((fp = fopen(filename, "ro")) == NULL) { // Oh, can't read that one
		err_mess = XmStringCreateSimple("Can't open that file");
		XtVaSetValues(errorDialog, XmNmessageString, err_mess, NULL);
		XmStringFree(err_mess);
		XtManageChild(errorDialog);
	}
	else { // Ok, read it 
		char msg[80];
		struct stat stat_buf;
		off_t file_size;
		int isDir;
		fstat(fileno(fp), &stat_buf);
		file_size = stat_buf.st_size;
		isDir =  (stat_buf.st_mode & S_IFDIR);
		fclose(fp);
		XtUnmanageChild(fileDialog);

		// check file location and type dir:file to run Xine
		if (strncmp(filename, USBDIR, 4) != 0) {
			sprintf(msg, "Selected file %s must be under %s", filename, USBDIR);
			err_mess = XmStringCreateSimple(msg);
			XtVaSetValues(errorDialog, XmNmessageString, err_mess, NULL);
			XmStringFree(err_mess);
			XtManageChild(errorDialog);
		}
		else {
#if 0
                        sprintf(msg, "Selected file %s size %d is Dir %d", filename, (int)file_size, isDir);
                        err_mess = XmStringCreateSimple(msg);
                        XtVaSetValues(errorDialog, XmNmessageString, err_mess, NULL);
                        XmStringFree(err_mess);
                        XtManageChild(errorDialog);
#else
			sprintf(cmdline, "%s '%s%s' -V xxmc -f", pnames[XINEAPP], isDir?"dvd:/":" ", filename);
			setThread(cmdline);
#endif
		}
  	}
  	free(filename);
}

void tellService()
{
#ifndef USINGINTERNAL
	system("sdmsg Banner");
#else
        SocketTCP tcp;
        if (tcp.Open(WIJET2_PORT, htonl(INADDR_LOOPBACK)) != 0) {
                if(tcp.Send("Banner", 6) < 1)
                         fprintf(stderr, "Send fails\n");
                tcp.Close();
        }
        else  fprintf(stderr, "error to open socket???\n");
#endif
}

void callback(Widget w,  void  *client,  XtPointer call) {
	int i = *(int*)client;

	switch(i) {		
        case ITERNET_BROWSER:
	sprintf(cmdline, "%s -height %d -width %d file:///usr/share/doc/HTML/index.html", pnames[FIREFOX], scrnX, scrnY);
        setThread(cmdline);
	break;

//	case VIDEO_PLAYER:
//	Open();
//	break;

        case CONFIGURATION:
	sprintf(cmdline, "%s -height %d -width %d http://localhost:8888", pnames[FIREFOX], scrnX, scrnY);
        setThread(cmdline);
	break;

        case EXIT_PROGRAM:
	default:
        cleanAll();
//	printf("Exit this program and go back to banner page\n");
	tellService();
	system("sdmsg Banner");
	sleep(2);
	exit(0);
	}
}

void CtrlBox(int argc,  char **argv) {
	XtAppContext app;
       	Widget 	toplevel, box, temp;
       	Widget   buttons[10];
       	int  	nbuttons;
        char*	exitPoint = names[EXIT_PROGRAM];
	if (argc > 1) {
		argv[0] = "Remote";
		if (argc > 2) exitPoint = "Back to PC desktop    ";
	}
	else argv[0] = "Local";
       	argc = 1;

       	toplevel =  XtVaAppInitialize(&app,"CtrlBox",NULL, 0, &argc, argv, NULL, NULL);

       	box = XtVaCreateManagedWidget("box",xmRowColumnWidgetClass, toplevel, NULL);

       	nbuttons = 0;
       	while(names[nbuttons] != 0) {
		if (nbuttons == EXIT_PROGRAM )
		buttons[nbuttons] = XtVaCreateManagedWidget(exitPoint,
                        xmPushButtonWidgetClass, box, NULL, 0);
		else
		buttons[nbuttons] = XtVaCreateManagedWidget(names[nbuttons],
			xmPushButtonWidgetClass, box, NULL, 0);
		XtAddCallback(buttons[nbuttons],XmNactivateCallback, callback,
			&values[nbuttons]);
	    nbuttons++;
	}

	// Create a popup error message dialog; no cancel, no help button 
	errorDialog = XmCreateErrorDialog(box, "errorDialog", NULL, 0);
	temp = XmMessageBoxGetChild(errorDialog, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(temp);
	temp = XmMessageBoxGetChild(errorDialog, XmDIALOG_HELP_BUTTON);
	XtUnmanageChild(temp);

	// Create a popup dialog to get the filename; no help button 
	fileDialog = XmCreateFileSelectionDialog(box, "fileDialog", NULL, 0);
	XtVaSetValues(fileDialog, XmNwidth, 400, NULL);
  	temp = XmFileSelectionBoxGetChild(fileDialog, XmDIALOG_HELP_BUTTON);
  	XtUnmanageChild(temp);

  	XtAddCallback(fileDialog, XmNokCallback, OpenFile, NULL);
	XtAddCallback(fileDialog, XmNcancelCallback, Cancel, NULL);

	XtManageChildren(buttons,nbuttons);
	XtRealizeWidget(toplevel);
        XMoveWindow (XtDisplay(toplevel), XtWindow(toplevel), 0, 0);
	XWarpPointer(XtDisplay(box), None, XtWindow(box), 0, 0, 0, 0, 400, 300);
	XtAppMainLoop(app);
}

int main(int argc,  char **argv) {
	cleanAll();
	CtrlBox(argc, argv);
	return 0;
}
