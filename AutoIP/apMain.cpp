#include "AutoIpProcess.h"

AutoIpProcess* myAutoIP;

void handler(int sig)
{
        switch(sig) {
        case SIGTERM:
        case SIGABRT:
		myAutoIP->stop();
	break;
	default:
		myAutoIP->process(sig);
	break;
	}	
}
int main(int n, char *a[])
{
	myAutoIP = new AutoIpProcess();
	myAutoIP->init(n);
        for (int i=1; i<=SIGTERM; i++)
                signal(i, handler);
	if(myAutoIP->start() == -1)
		printf("Network Error\n");
        return 0;
}

