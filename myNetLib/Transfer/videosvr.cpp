#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../myNetWK.h"

typedef struct {
	SocketTCP *tcp;
	char	  *ip;
} myArgs;

#define myXine "/home/wijet2/myXine/mx"

void* startClient(void *ma)
{
        int rtn;
        char buffer[125], cmd[250];
        SocketTCP *myClient = ((myArgs*)ma)->tcp;
        char *IP = ((myArgs*)ma)->ip;
        myClient->SetRcvTime(5000);
        rtn = myClient->Recv(buffer, 250);
        if (rtn > 0) {
                buffer[rtn] = 0;
		if (strstr(buffer,":"))		// full mrl
			sprintf(cmd, "%s %s\n", myXine, buffer);
		else
                	sprintf(cmd, "%s tcp://%s:%s\n", myXine, IP, buffer);
                fprintf(stderr, cmd);
                system(cmd);
        } 
        delete myClient;
        printf("Done \n");
        pthread_exit(0);
} 


int main(int n, char *a[])
{
	SocketTCP myTCP;
	int port = 10000;
	if ( n > 1) port = atoi(a[1]);
	printf("Video server start on port: %d\n", port);
	if(!myTCP.OpenServer(port)) {
		fprintf(stderr, "Open server fail\n");
		return -2;
	}
	while(1) {
		pthread_t myTread;
       		struct sockaddr_in addr;
       		myArgs ma;
	try {
		ma.tcp = myTCP.Accept(&addr);
                ma.ip = inet_ntoa(addr.sin_addr);
                printf("New Client Addr %u %s Port %d  Family %d\n", *(UINT*)(&addr.sin_addr), ma.ip, addr.sin_port, addr.sin_family);
		pthread_create( &myTread, NULL, startClient, (void*)&ma);
		printf("Start...\n");
	}
	catch(...) {
		fprintf(stderr,"exception!!\n");
		return -1;
	}
	}
	return 0;
}

