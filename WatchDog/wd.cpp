

#include "WatchDog.h"

WatchDog* myWatchDog;

void handler(int sig)
{
	myWatchDog->process(sig);
}

int main (int argc, char **argv)
{
	myWatchDog = new WatchDog();
	for (int i=1; i<=SIGTERM; i++)
		signal(i, handler);
	if(myWatchDog->init(argc) == 0) 
		myWatchDog->start();
	else
		system("reboot");         // REBOOT
	return 0;
}

