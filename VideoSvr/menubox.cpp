#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

XtAppContext application_context;

#define MAX_NAME_ITEM_SIZE	5
static  char * menu_item_names[MAX_NAME_ITEM_SIZE] = {
        "Internet Browswer", "Video Player", "Configuration",
        "Presentation", "Exit program",
    };

enum {	ITERNET_BROWSER = 0,
	VIDEO_PLAYER,
	CONFIGURATION,
	PRESENTATION,
	EXIT_PROGRAM
	};

void *invoke_application(void *name)
{
	// tell control ceter
	// clean process of internet browser or video player
	// start program
	system((char*)name);
        pthread_exit(0);
}

void do_menu_selection(Widget item_selected, XtPointer unused1, XtPointer unused2) 
{
  pthread_t myThread;
  unsigned int i;
  char * name = XtName(item_selected);
  for (i = 0; i < MAX_NAME_ITEM_SIZE; i++)
	if (strcmp(name, menu_item_names[i]) == 0)
		break;
	
  switch(i) {
	case ITERNET_BROWSER:
	pthread_create( &myThread, NULL, invoke_application, (void*)"/usr/bin/netscape -geometry 1024x768+0+0");
	break;
	case VIDEO_PLAYER:
	pthread_create( &myThread, NULL, invoke_application, (void*)"/usr/bin/xine");
	break;
	case CONFIGURATION:
	printf("goto configuration page\n");
	break;
	case PRESENTATION:
	printf("switch to presentaion\n");
	break;
	default:
    		XtDestroyApplicationContext(application_context);
    		exit(0);
	break;
  }
}

String fallback_resources[] = { 
    "*menuButton.label: Click me to start",
    NULL,
};

int main(int argc, char **argv) {

    int i;
    Widget top_level, menu_button, menu, menu_element;
    
    top_level = XtVaAppInitialize(&application_context, "testmenu", NULL, 0,
			    &argc, argv, fallback_resources, 
			    NULL);
    
    menu_button = XtCreateManagedWidget("menuButton", menuButtonWidgetClass,
					top_level, NULL, 0);
    
    menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, menu_button, 
			      NULL, 0);
    
    for (i = 0; i < (int)XtNumber(menu_item_names) ; i++) {
      menu_element = XtCreateManagedWidget(menu_item_names[i],
					   smeBSBObjectClass,
					   menu, NULL, 0);
      XtAddCallback(menu_element, XtNcallback, do_menu_selection, NULL);
	
    }
    
    XtRealizeWidget(top_level);
    XtAppMainLoop(application_context);
    return 0;
}


