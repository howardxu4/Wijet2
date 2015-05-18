#include <stdio.h>
#include <stdlib.h>

#include "../myNetLib/myNetWK.h"

int main(int argc, char* argv[])
{
	int port = 0;
	int protocol = 0;
	char *ip = NULL;
	char *msg = NULL;
	bool echo = false;
	if (argc > 1) {
    		for(int i = 1; i < argc; i++) {
			if(!strcmp(argv[i], "-i")) {
				ip = argv[++i];
			}
			else if(!strcmp(argv[i], "-p")) {
				port = atoi(argv[++i]);
			}
			else if(!strcmp(argv[i], "-t")){
				if(!strcmp(argv[++i], "udp"))
					protocol = Sender::PROT_UDP;
				else if (strstr(argv[i], "mul"))
					protocol = Sender::PROT_MUL;
			}
			else if(!strcmp(argv[i], "-r")) 
				echo = true;
			else
				msg = argv[i];
		}
	}
	if (msg == NULL || port == 0) {
		fprintf(stderr, "Usage: %s {-p port} [-i ip] [-t protocol] [-r] msg\n", argv[0]);
	}
	else
	{
		Sender mySender(port);
		if (ip != NULL) mySender.SetIP(ip, INADDR_LOOPBACK); 
		if (mySender.Send(msg, strlen(msg)) >0) {
			if (echo) {
				char buf[100];
				if (mySender.Echo((char*)&buf[0], 100)> 0)
					printf("recv resp : %s\n", buf);
			}
			usleep(400);
		}
		else
			fprintf(stderr,"Open socket fails on port %d\n",
				port);
		return 0;
        }
	return -1;
}

