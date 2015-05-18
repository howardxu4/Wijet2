#include "Banner.h"
#include <pthread.h>

Banner myBanner;

void* invoke(void *p)
{
	Banner* bn = (Banner*)p;
	bn->loop();
	pthread_exit(0);
}

void handler(int sig)
{
        switch(sig) {
        case SIGTERM:
        case SIGABRT:
		myBanner.stop();
		break;
	case SIGTRAP:
		myBanner.updateBurn();
		break;
	default:
		myBanner.updtShow(true);
	break;
	}
}

int main (int argc, char **argv)
{
        pthread_t myThread;
	int n;
        for (int i=1; i<=SIGTERM; i++)
                signal(i, handler);
	if (argc > 1) n = myBanner.splash();
	else  n = myBanner.start();
	if(n > 0) {
		pthread_create( &myThread, NULL, invoke, (void*)&myBanner);
		n = 1;
		while(true) {
			sleep(2);
			n = myBanner.getStat();
			if(n != 1) break;
		}
		if (n == 2) myBanner.tellService();
		myBanner.stop();
	}
}

