

#  include "CheckIR.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

#undef DEBUG

#define PACKET_SIZE 256
#define IR_MODULE_ID "WiJET2"

/* three seconds */
#define TIMEOUT 3

int timeout=0;

void sigalrm(int sig)
{
	timeout=1;
}

const char *read_string(int fd)
{
	static char buffer[PACKET_SIZE+1]="";
	char *end;
	static int ptr=0;
	ssize_t ret;
		
	if(ptr>0)
	{
		memmove(buffer,buffer+ptr,strlen(buffer+ptr)+1);
		ptr=strlen(buffer);
		end=strchr(buffer,'\n');
	}
	else
	{
		end=NULL;
	}
	alarm(TIMEOUT);
	while(end==NULL)
	{
		if(PACKET_SIZE<=ptr)
		{
			fprintf(stderr,"Error: bad packet\n");
			ptr=0;
			return(NULL);
		}
		ret=read(fd,buffer+ptr,PACKET_SIZE-ptr);

		if(ret<=0 || timeout)
		{
			if(timeout)
			{
				fprintf(stderr,"Error: timeout\n");
			}
			else
			{
				alarm(0);
			}
			ptr=0;
			return(NULL);
		}
		buffer[ptr+ret]=0;
		ptr=strlen(buffer);
		end=strchr(buffer,'\n');
	}
	alarm(0);timeout=0;

	end[0]=0;
	ptr=strlen(buffer)+1;
#ifdef DEBUG
	printf("buffer: -%s-\n",buffer);
#endif
	return(buffer);
}

enum packet_state
{
	P_BEGIN,
	P_MESSAGE,
	P_STATUS,
	P_DATA,
	P_N,
	P_DATA_N,
	P_END
};

int send_packet(int fd,const char *packet)
{
	int done,todo;
	const char *string,*data;
	//char *endptr;
	enum packet_state state;
	int status,n;
	unsigned long data_n=0;

	todo=strlen(packet);
	data=packet;
	while(todo>0)
	{
		done=write(fd,(void *) data,todo);
		if(done<0) {
			fprintf(stderr,"Error: could not send packet\n");
			return(-1);
		}
		data+=done;
		todo-=done;
	}

	/* get response */
	status=0;
	state=P_BEGIN;
	n=0;
	while(1)
	{
		string=read_string(fd);
		if(string==NULL) return(-1);
		switch(state)
		{
		case P_BEGIN:
			if(strcasecmp(string,"BEGIN")!=0) {
				continue;
			}
			state=P_MESSAGE;
			break;
		case P_MESSAGE:
			if(strncasecmp(string,packet,strlen(string))!=0 ||
			   strlen(string)+1!=strlen(packet)) {
				state=P_BEGIN;
				continue;
			}
			state=P_STATUS;
			break;
		case P_STATUS:
			if(strcasecmp(string,"SUCCESS")==0) {
				status=0;
			} else if(strcasecmp(string,"END")==0) {
				status=0;
				return(status);
			} else if(strcasecmp(string,"ERROR")==0) {
				fprintf(stderr,"Error: command failed: %s", packet);
				status=-1;
			} else {
				goto bad_packet;
			}
			state=P_DATA;
			break;
		case P_DATA:
			if(strcasecmp(string,"END")==0)	{
				return(status);
			} else if(strcasecmp(string,"DATA")==0)	{
				state=P_N;
				break;
			}
			goto bad_packet;
		case P_N:
			errno=0;
			if(strcasecmp(string, IR_MODULE_ID) == 0) {
				status = 0;
			}
			state = P_END;
			break;
		case P_DATA_N:
			fprintf(stderr,"DATA: %s\n",string);
			n++;
			if((unsigned long)n==data_n) state=P_END;
			break;
		case P_END:
			if(strcasecmp(string,"END")==0)
			{
				return(status);
			}
			goto bad_packet;
			break;
		}
	}
 bad_packet:
	fprintf(stderr,"Error: bad return packet\n");
	return(-1);
}

int checkIR()
{
        char *lircd=LIRCD;
	struct sockaddr_un addr;
	int fd;
	char buffer[PACKET_SIZE+1];
	struct sigaction act;

	act.sa_handler = sigalrm;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;           /* we need EINTR */
	sigaction(SIGALRM,&act,NULL);

	/* Create a new socket and connect to lircd daemon */
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, lircd);
	fd=socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd == -1) {
		fprintf(stderr,"Error: could not open socket\n");
		return(EXIT_FAILURE);
	}
	if(connect(fd,(struct sockaddr *)&addr,sizeof(addr)) == -1) {
		fprintf(stderr,"Error: could not connect to socket\n");
		close(fd);
		return(EXIT_FAILURE);
	}

	/* Send the command */
	sprintf(buffer, "ID\n");
        if(send_packet(fd, buffer) == -1) {
		close(fd);
                return(EXIT_FAILURE);
        }

	close(fd);
	return(EXIT_SUCCESS);
}

#ifdef APPLICATION

int main(int argc,char **argv)
{
	if (checkIR() == EXIT_SUCCESS)
		printf("OK\n");
}

#endif
