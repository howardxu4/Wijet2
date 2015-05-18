

/**
 *	The State class maintain the state system 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#include "Control.h"

Control* myControl;

void handler(int sig)
{
        switch(sig) {
        case SIGTERM:
        case SIGABRT:
                myControl->stop();
        break;
        default:
                myControl->process(sig);
        break;
	}
}

int main(int n, char* a[])
{
	myControl = new Control();
        if(myControl->init() == 0) {
		for (int i=1; i<=SIGTERM; i++)
                	signal(i, handler);		
		myControl->begin();
		if (myControl->start() == -1)
			printf("Network Error\n");
	}
        else printf("Check!\n");
	return 0;	
}
