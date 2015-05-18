#include "videosvr.h"

typedef struct {
	SocketTCP *tcp;
	char	  *ip;
} myArgs;

#define myXineStr "/home/wijet2/myXine/mx"

// take out the geometry and saperate the mrl

void parseBuffer(int r[], char* buffer)
{
  char *p = strstr(buffer, "-G");
  if (p != NULL) {
	int i = 1;
	char *q = p+3;
	while (*q == ' ' && *q) q++;
	if (*q) {
		while(*(q+i) != ' ' && *(q+i)) i++;
		*(q+i) = 0;
		parse_geometry(q, r); 	
	}
	
        if (p - buffer > 4) {  // mrl is first
		while(*(--p) == ' ') *p = 0; 
	}
	else {
		p = q+i+1;
		while(*p== ' ' && *p) p++;
		q = buffer;
		while ((*q++ = *p++) != 0);
	}
  }
}

void* startClient(void *ma)
{
        int rtn;
        char buffer[125], cmd[250];
        SocketTCP *myClient = ((myArgs*)ma)->tcp;
        char *IP = ((myArgs*)ma)->ip;
        myClient->SetRcvTime(5000);
        rtn = myClient->Recv(buffer, 250);
        if (rtn > 0) {
		int r[4] = { 200, 100, 300, 250 };
                while(buffer[rtn-1] <= ' ') rtn--;
                buffer[rtn] = 0;
		if (strstr(buffer,":"))	{	// full mrl
			sprintf(cmd, "%s %s", myXineStr, buffer);
			printf("'%s'\n", cmd);
			parseBuffer(r, buffer);
#if 1
			if (myXineWindow(r[0], r[1], r[2],r[3]) == 0){
				myXineDriver(NULL, NULL);
				myXinePlay(buffer);
				myXineCleanUp();
			}
#else
			myXine(r[0], r[1], r[2],r[3], buffer, NULL, NULL);
#endif
		}
		else {
                	sprintf(cmd, "%s tcp://%s:%s\n", myXineStr, IP, buffer);
                	fprintf(stderr, cmd);
                	system(cmd);
		}
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
	if (myXineInit() < 0) {
		fprintf(stderr, "Xine initialization fails\n");
		return -3;
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

