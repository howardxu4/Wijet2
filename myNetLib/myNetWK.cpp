#include "myNetWK.h"

//#include "cmdMsgs.h"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <io.h>

/////////////////// CHANGE HERE TO SWICH COMPILE OPTION ////////////////

#ifdef USING_THIS_FOR_TEST_MODERATOR		

#include <iostream.h>

#include <shellapi.h>

#define OTCPSM_PORT	34302
#define MSG_SIZE 256


typedef struct s_availMacIp {
	unsigned short     sel;
	char      mac[6];
	char      ipA[4];
	char      dName[31];
} availMacIp;


/////////////////////////////////////////////////////////////////////////////
// The Command Helper Arrays  

enum CMD_INDEX {CMD_HELP, CMD_CLEAN, CMD_WAKEUP, CMD_GETSEL,
			CMD_CONNECT, COM_GETCONN, COM_DISCONN, 
			COM_CLOSE, COM_EXIT, 
			CMD_LAUCH, CMD_PLAY, CMD_OPEN, CMD_RUN, CMD_LAST };

char* cmdString[] = {"help","quit","wakeup","getselectunit",
					"establish","getconnectlist","release", 
					"close", "exit",
					"launchppt", "playvideo", "openfile", "runscript", ""};

UCHAR cmdMessage[] = {0, 0, PSM2PSMControl, PSM2PSMGetUnitSelection, 
			PSM2PSMEstablish,PSM2PSMGetConnectedUnits,PSM2PSMDisconnect,
			PSM2PSMExit,PSM2PSMExitDaemon,
			PSM2PSMLaunchPPT,PSM2PSMPlayVideo,PSM2PSMOpenFile,PSM2PSMRunScript };


int FillMessage(UCHAR cmd, char *para, ULONG ip, char *buf)
{
	MsgHeader myMsgHeader(COMANDSETPSM2PSM); 
	int n = myMsgHeader.getMsgHeaderLength();
	myMsgHeader.setCommand(cmd, ip);
	memcpy(buf, myMsgHeader.getMsgHeader(), n);
	buf[n] = (UCHAR)strlen(para) + 1;
	strcpy(&buf[n+1], para);
	return n + buf[n] + 1;
}

BOOL SendToPSM(char* msg, int size, ULONG ip)
{
	SocketTCP myClient;
	if (myClient.Open(OTCPSM_PORT, ip)) {
		myClient.SetRcvTime(5000);
		if (myClient.Send(msg, size) > 0) {
			memset(msg, 0, MSG_SIZE);
			if (myClient.Recv(msg, MSG_SIZE) > 0) {
				return true;
			}
		}
	}
	sprintf(msg, "IO error: network no response");
	return false;
}

int ParserCmd(char *msg)
{
	int i, j, k;
	char cmdLine[MSG_SIZE];
	k = strlen(msg);
	for(i=j=0; i<=k; i++) {
		if (msg[i] == ' ' && j==0) continue;
		cmdLine[j++] = tolower(msg[i]);
	}
	for (i = CMD_HELP; i < CMD_LAST; i++)
		if (strncmp(cmdLine, cmdString[i], strlen(cmdString[i])) == 0) 
			return i;
	return -1;
}

void showHelp()
{
		printf("                         COMMAND ONLINE HELP\n\n");
		printf("Quit                                    :quit this application\n");
		printf("WakeUp                   [ip]           :wakeup the PSM\n");	
		printf("GetSelectUnit            [ip]           :get unit selection list\n");	
		printf("Establish                [ip]           :establish a session\n");	
		printf("GetConnectList           [ip]           :get connected unit list\n");	
		printf("Release                  [ip]           :release the session\n");	
		printf("Close                    [ip]           :close the PSM\n");	
		printf("Exit                     [ip]           :exit the PSM\n");	
		printf("LaunchPPT      {ppt}     [ip]           :launch PPT\n");	
		printf("PlayVideo     {video}    [ip]           :play video\n");	
		printf("OpenFile       {file}    [ip]           :open file\n");	
		printf("RunScript     {script}   [ip]           :run script\n");	
		printf("HELP                                    :show this help\n");
		printf(" ----- case sensitive is ignored \n");
		printf(" ----- { } filename is required  \n");
		printf(" ----- [ ] ip is optional  default:127.0.0.1 \n\n");
		
}

BOOL PaserArg(int cmd, char *msg, char **para, char **ip)
{
	char *szIP, *szPara; 
	szPara = &msg[strlen(cmdString[cmd])];
	if (cmd > COM_EXIT) {
		while(*szPara == ' ') szPara++;
		if (*szPara == '"') {
			*szPara++ = ' ';
			szIP = szPara;
			while(*szIP) {
				if (*szIP == '"') {
					*szIP = ' ';
					if (*(szIP+1)) {
						*szIP = 0;
						szIP++;
					}
					break;
				}
				szIP++;
			}
		}
		else {
			szIP = szPara;
			while(*szIP) {
				if (*szIP == ' ') {
					if (*(szIP+1)) {
						*szIP = 0;
						szIP++;
					}
					break;
				}
				szIP++;
			}
		}
		if (szPara == szIP) {
			printf("The parameter is required in this command: %s\n", cmdString[cmd]);
			return false;
		}
	}
	else szIP = szPara;
	while(*szIP == ' ') szIP++;
	*para = szPara;
	*ip = szIP;
	return true;
}

BOOL ExecCmdInput(int cmd, char *szPara, char *szIP)
{
	BOOL rtn = false;
	char buf[MSG_SIZE];
	ULONG ip = htonl(INADDR_LOOPBACK);
	if (strlen(szIP) > 0) ip = inet_addr(szIP);
	SocketICMP myPing;
	if (ip ==  htonl(INADDR_LOOPBACK) || myPing.Ping(ip) ) {
		memset(buf, 0, MSG_SIZE);
		int n = FillMessage(cmdMessage[cmd], szPara, ip, buf);
		rtn = SendToPSM(buf, n, ip);
		if (cmd == CMD_GETSEL || cmd == COM_GETCONN) {
			n = *(int*)buf;
			availMacIp *array = (availMacIp*)&buf[sizeof(int)];
			for(int i=0; i<n; i++) {
				char listLine[80];
				sprintf(listLine, inet_ntoa(*(struct in_addr *)&(array->ipA[0])));
				for(int j=strlen(listLine); j<18; j++) 
					listLine[j] = ' ';
				strcpy(&listLine[18], (char *)array->dName);
				printf("%s\n", listLine);	
				array++;
			}
			sprintf(buf, "%s get %d entries", cmdString[cmd], n);
		}
		printf("%s\n", buf);
	}
	else 
		printf( "Unreachable IP address (%x), please input correct one", ip);
	return rtn;
}

#ifdef NNNNNNooooooooooooCOMMENT

void main(int n, char *a[])
{
	if (n == 1) printf("Usage %s  file [dir [para]] \n", a[0]);
	else {
		char s[20];
		char *file, *dir, *para;
		dir = para = "";
		file = a[1];
		if (n > 2) dir = a[2];
		if (n > 3) para = a[3];
		ShellExecute(NULL, NULL, file, para, dir, SW_SHOW);
		printf("Is this work?");
		cin.getline(s, 20);

	}
}

#endif

int main(int n, char *a[])
{
	int rtn = 0;
	int cmd = CMD_HELP;
	char msg[MSG_SIZE];
	char *Para, *IP;
	SocketSystem mySockSys;
	Para = IP = "";
	if (n > 1) {
		cmd = ParserCmd(a[1]);
		if (cmd > CMD_CLEAN) {
			if (cmd > COM_EXIT)
				if (n == 2) {
					printf("The parameter is required in this command: %s\n",cmdString[cmd]);
					rtn = -1;
				}
				else {
					Para = a[2];
					if (n > 3) IP = a[3];
				}
			else if (n > 2)
				IP = a[2];
			if (!ExecCmdInput(cmd, Para, IP))
				rtn = -1;			
		}
		else {
			printf("\nUsage:  Moderator cmd [arg...]\n");
			printf("the detail about cmd and arg please refer to the following information:\n\n");  
			showHelp();
		}
	}
	else do {
		if(cmd > CMD_CLEAN) {
			BOOL ok = PaserArg(cmd, msg, &Para, &IP);
			if (ok)
				ok = ExecCmdInput(cmd, Para, IP);
			if (!ok)
				printf( "Fail on command call \n\n");	
		}
		else if(cmd != CMD_CLEAN)
			showHelp();
		cin.getline(msg, MSG_SIZE);
		cmd = ParserCmd(msg);
	} while (cmd != CMD_CLEAN);
	return rtn;
}


#else

/////////////////////////////////////////////////////////////////////////////
// The Net Lib testing program  

#define TCP_TEST_PORT	9800
#define BUFFER_SIZE		1024

// server test
void server_test()
{
	SocketTCP myTCP;
	struct sockaddr_in addr;
	bool p;
/*
	Create();
	Bind(TCP_TEST_PORT, INADDR_ANY); 
	Listen(5);
*/
	p = myTCP.OpenServer(TCP_TEST_PORT);
	printf("Server start working... (return %d)\n", p);
	try {
		while(1) {
			char buf[BUFFER_SIZE];
			SocketTCP* myClient = myTCP.Accept(&addr);
			printf("New Client Addr %d %s Port %d  Family %d\n",
				addr.sin_addr, inet_ntoa(addr.sin_addr), addr.sin_port, addr.sin_family);
			myClient->SetKeepAlive(1);
			myClient->SetRcvTime(2000);
			memset(&addr, 0, sizeof(addr));
			while(1) {
				if (myClient->Recv(buf, BUFFER_SIZE) >0) {
					printf("%s\n", buf);
					myClient->Send(buf, strlen(buf));
					if (buf[0] == '1') break;
				}
				else break;
			}
			printf("Client %d closed\n", myClient->GetSocketID());
		}
	}
	catch(...) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
		printf("something are wrong!\n");
	}
}

// client test
void client_test(ULONG uip)
{
	SocketTCP myTCP;
	bool p;
	int  i, error;
	char buffer[BUFFER_SIZE];
	ULONG 	ip = htonl(INADDR_LOOPBACK);
	SocketICMP myPing;
	if (uip != htonl(INADDR_LOOPBACK)) {
	if (myPing.Ping(uip)) ip = uip;
	else printf("The IP %x is not reachable switch to localhost  ********\n", uip);
	}
	printf("connect to %x  %u origIP(%x)\n", ip, ip, uip);
	memset(buffer, NULL, BUFFER_SIZE);
/*
	Create();
//	Bind(0, INADDR_LOOPBACK);
	Connect(TCP_TEST_PORT, ip);
*/
	p = myTCP.Open(TCP_TEST_PORT,ip);
	printf("open client return %d\n", p);
	p = myTCP.SetKeepAlive(5);
	printf("Set Keep Alive option return %d\n", p);
	for(i =0; i<3;i++) {
		error = myTCP.Send("hellow Socket", 15);
		printf("Client start sending... (Send %d)\n", error);
		error = myTCP.Recv(buffer, BUFFER_SIZE);
		printf("Client start recive... (Recv %d)\n", error);
		if (error > 0) buffer[error] = 0;
		printf("receive data -> %s\n", buffer);	
	}
	error = myTCP.Send("01 that's all", 14);
	printf("Client End sending... (Send %d)\n", error);
	error = myTCP.Recv(buffer, BUFFER_SIZE);
	printf("Client start recive... (Recv %d)  %s\n",
		error, myTCP.GetSockName());
	
}

// simple test
void simple_test()
{
	SocketICMP myPing;
	NICAccess myNIC;
	SocketBase myBase;
	ULONG ip = 16777343; // 127.0.0.1
	int  i, error;
	bool p;

	error = myNIC.getNumMaskIP();
	for (i=0; i<error; i++)
		printf("NIC total %d (%d) IP: %x  MASK %x\n", error, i, 
			myNIC.getIPAddr(i), myNIC.getMaskAddr(i));
	p = myPing.Ping(ip);
	printf("ping %ld returns %d\n", ip, p);
	ip = 12345678;

	p = myPing.Ping(ip);
	printf("ping %ld returns %d\n", ip, p);
	ip = INADDR_LOOPBACK;
	p = myPing.Ping(ip);
	printf("ping loopback %ld returns %d\n", ip, p);
	ip = htonl(ip);
	p = myPing.Ping(ip);
	printf("ping LoopBack %ld returns %d\n", ip, p);
	ip = myBase.GetAddress("192.168.11.50");
	p = myPing.Ping(ip);
	printf("ping 192.168.11.50 %ld returns %d\n", ip, p);
	for (i=0; i<error; i++) 
		printf("Ping (%d) IP: %x  return %d\n", i, 
			myNIC.getIPAddr(i), 
			myPing.Ping(myNIC.getIPAddr(i)));
	myBase.Create(SOCK_STREAM);
	printf("Socket = %d Host name: %s\n", myBase.GetSocketID(), myBase.GetSockName());
	printf("\n\nCost: INADDR_LOOPBACK %d %x\n", INADDR_LOOPBACK,INADDR_LOOPBACK);
	printf("Cost: INADDR_BROADCAST %d %x\n", INADDR_BROADCAST,INADDR_BROADCAST);
	printf("Cost: INADDR_ANY %d %x\n", INADDR_ANY,INADDR_ANY);
	printf("Cost: INADDR_NONE %d %x\n", INADDR_NONE,INADDR_NONE);
}

// WiJet UDP receive test
#define UDP_PORT_RECV   30700
#define UDP_PORT_SEND	30600
void receive_test(ULONG rip)
{
	const int time = 100000;
	bool p;
	char buf[BUFFER_SIZE];
	int error;
	struct sockaddr_in raddr;
	SocketUDP sock2;
	ULONG 	ip = INADDR_ANY;
	SocketICMP myPing;
	if (rip != INADDR_ANY) {
	if (myPing.Ping(rip)) ip = rip;
	else  
		printf("The IP %x is not reachable switch to localhost  ********\n", rip);
	}
	printf("connect to %x  %u origIP(%x)\n", ip, ip, rip);
/*
	Create();
	Bind(UDP_PORT_RECV, INADDR_ANY);
*/
	p = sock2.Open(UDP_PORT_RECV, ip);
	printf("Open returns %d port %d IP %x\n", p, UDP_PORT_RECV, ip);
	p = sock2.SetRcvTime(time);
	printf("SetRcvTime %ld returns %d\n", time, p);

	while (1) {
	error = sock2.RecvFrom(buf, BUFFER_SIZE, &raddr);
	if(error != 0 && error != SOCKET_ERROR) {
		struct sockaddr_in *paddr = (sockaddr_in*)&raddr;
		buf[error] = 0;
		printf("OK  recv = %s\n", buf);
		printf("RADDR %d  %s  PORT %d  FAMILY %d\n", paddr->sin_addr,
			inet_ntoa(paddr->sin_addr), paddr->sin_port, paddr->sin_family);
	}
	else
		printf("Receive error %d\n", error);
	}
}

void sending_test(ULONG uip)
{
	const int one = 1;
	bool p;
	int error;
	char buf[BUFFER_SIZE];
	SocketUDP socka;
	ULONG 	ip = INADDR_BROADCAST;
	SocketICMP myPing;
	if (uip != INADDR_BROADCAST) {
	if (myPing.Ping(uip)) ip = uip;
	else  
		printf("The IP %x is not reachable switch to localhost  ********\n", uip);
	}
	printf("connect to %x  %u origIP(%x)\n", ip, ip, uip);
/*
	Create();
	Bind(0,INADDR_ANY);
*/

	p = socka.Open();
	printf("Open returns %d\n", p);
	p = socka.SetBroadcast();
	printf("SetBroadcast returns %d\n", p);
	sprintf(buf, "The 1st message go through the broadcast UDP");
	p = socka.SendTo(UDP_PORT_RECV, ip, buf, strlen(buf)+1);
	if (!p) printf("Error Code = %d\n",WSAGetLastError()); 
	printf("sendto %s return %d\n", buf, p);
	sprintf(buf, "Can you see that?");
	p = socka.Connect(UDP_PORT_RECV, ip);
	printf("connect return %d\n", p);
	error = socka.Send(buf, strlen(buf)+1);
	if (error <=0) printf("Error Code = %d\n",WSAGetLastError()); 
	printf("send %s return %d\n", buf, error);

}

#define MULTICAST_PORT		5600
#define MULTICAST_GROUP_IP "234.5.6.8"

void mc_receive_test()
{
	const int time = 100000;
	ULONG ip;
	bool p;
	char buf[BUFFER_SIZE];
	struct sockaddr_in raddr;
	int error;
	MultiCastUDP myMUDP;

	ip = myMUDP.GetAddress(MULTICAST_GROUP_IP);

//	Create();	
//	Bind(MULTICAST_PORT);
//	SetLoop(1);
//	SetTTL(32);

	p = myMUDP.Open(MULTICAST_PORT, 1);
	printf("Open %s returns %d\n", MULTICAST_GROUP_IP, p);

	p = myMUDP.Join(ip);
	printf("Join returns %d\n", p);
	p = myMUDP.SetRcvTime(time);
	printf("SetRcvTime returns %d\n", p);
	while (1) {
		printf("start receiving data ...\n");
		error = myMUDP.RecvFrom(buf, BUFFER_SIZE, &raddr);
		if(error != 0 && error != SOCKET_ERROR)
			printf("OK  recv: %s", buf);
		else
			printf("Receive error %d\n", error);
	}
}

void mc_send_test()
{
//	const int time = 100000;
	ULONG ip;
	bool p;
	int error;
	char buf[BUFFER_SIZE];
	MultiCastUDP myMUDP;
	ip = myMUDP.GetAddress(MULTICAST_GROUP_IP);
 
//	Create();	
//	Bind(MULTICAST_PORT);
//	SetLoop(1);
//	SetTTL(32);

	p = myMUDP.Open(MULTICAST_PORT);
	printf("Open %s returns %d\n", MULTICAST_GROUP_IP, p);

//	p = myMUDP.Join(ip);
//	printf("Join returns %d\n", p);
	sprintf(buf, "Hello Multicasting ... \n");
	printf("sending %s", buf);
	p = myMUDP.SendTo(MULTICAST_PORT, ip, buf, strlen(buf)+1);
	if(p)
		printf("OK\n");
	else
		printf("Sending error %d\n", WSAGetLastError());
	p = myMUDP.Connect(MULTICAST_PORT, ip);
	printf("Connect return %d\n", p);
	error = myMUDP.Send(buf, strlen(buf)+1);
	if (error <=0) printf("Error Code = %d\n",WSAGetLastError()); 
	printf("send %s return %d\n", buf, error);
	
}

#define TOTALSIZE 1113088

void m_server()
{
	SocketTCP myTCP;
	MultiCastUDP myMUDP;
	ULONG ip = myMUDP.GetAddress(MULTICAST_GROUP_IP);
	fd_set fds;
	int fd, n, len, nbytes, error, j;
	int msock, csock, maxd;
	struct timeval timeout;
	bool p;
	char buf[BUFFER_SIZE];
	struct sockaddr_in raddr;
	error = nbytes = j = 0;

	p = myMUDP.Open(MULTICAST_PORT, 1);
	printf("Open %s returns %d\n", MULTICAST_GROUP_IP, p);

	p = myMUDP.Join(ip);
	printf("Join returns %d\n", p);

	p = myTCP.OpenServer(TCP_TEST_PORT);
	printf("Server start working... (return %d)\n", p);
	SocketTCP* myClient = myTCP.Accept(NULL);
	printf("Connected a TCP client...\n");

	fd = open("test1.vob", _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE );
	if (fd < 0) printf("Failed to open test1.vob\n");
	else printf("The file test1.vob is opened\n");

	msock = myMUDP.GetSocketID();
	csock = myClient->GetSocketID();
	maxd = (csock > msock)? csock: msock;
	while(1) {
		FD_ZERO(&fds);
		FD_SET((UINT)msock, &fds);
		FD_SET((UINT)csock, &fds);
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		n = select(maxd+1, &fds, NULL, NULL, &timeout);
    
		if (n > 0) {
			if (FD_ISSET(msock, &fds)) {
				len = myMUDP.RecvFrom(buf, BUFFER_SIZE, &raddr);
				// sequence number and crc check
				// update statistics
				if (len > 0) {         // get something
					write(fd, buf, len);
					nbytes += len;
				} else if (len == 0) { // closed or end of stream
					close(fd);
					break;
				}
				else { // error recvfrom
					printf("Failed to recvfrom\n");
					break;
				}
			} // multicast socket

			if (FD_ISSET(csock, &fds)) {
				len = myClient->Recv(buf,BUFFER_SIZE);
				if (len < 0) {
					printf("Failed to read from TCP socket\n");
					break;
				} else {
					buf[len]=0;
					printf("Len = %d %s\n", len, buf);
					if (nbytes != TOTALSIZE) {
						printf("Only received %ld bytes Does it match?\n", nbytes);
						error++;
						if (error > 5) break;
					}
					lseek(fd, 0, SEEK_SET); // rewind
//					nbytes = 0;
					printf("loop #: %ld, error #: %ld\n", j++, error);
				}
			} // tcp socket
		}
		else printf("select return %d\n", n);
	}
}

void m_client(ULONG tip)
{
	SocketTCP myTCP;
	MultiCastUDP myMUDP;
	ULONG ip = myMUDP.GetAddress(MULTICAST_GROUP_IP);
	fd_set fds;
	int fd, n, len;
	int msock, total, i, j, error;
	struct timeval timeout;
	bool p;
	char buf[BUFFER_SIZE];
	char tbuf[40];
	error = total = i = j = 0;
	
	p = myTCP.Open(TCP_TEST_PORT,tip);
	printf("Open %s returns %d\n", MULTICAST_GROUP_IP, p);
//	if (!p) return;

	fd = open("..\\test.vob", _O_RDONLY | _O_BINARY);
	myMUDP.Open(MULTICAST_PORT);
	myMUDP.Join(ip);
	msock = myMUDP.GetSocketID();
	Sleep(1000);			// start now .....
	while(1) {
		n = read(fd, buf, BUFFER_SIZE);
		if (n <= 0) {
			lseek(fd, 0, SEEK_SET); // rewind
			n = read(fd, buf, BUFFER_SIZE);
			sprintf(tbuf, "Sent %d already. Try to rewind!", j);
			len = strlen(tbuf);
			Sleep(500);
			if (++i > 5) break;
			if (j != TOTALSIZE * i) printf("Data error on %d (%d)\n",j,(TOTALSIZE*i)); 
			if (myTCP.Send(tbuf, len) < 0) {
				printf("Failed to write into sock\n");
				break;
			}
			if (i > 4) break;
		}
		FD_ZERO(&fds);
		FD_SET((UINT)msock, &fds);
		timeout.tv_sec = 0;
		timeout.tv_usec = 1000;
		select(msock+1, NULL, &fds, NULL, &timeout);
		if (FD_ISSET(msock, &fds))
		if (myMUDP.SendTo(MULTICAST_PORT, ip, buf, n)) {
			  j += n;
			  total++;
			  printf("total packets sent: %ld\n", total);
		} else {
			  printf("Failed to write to the multicast channel\n");
			  printf("total packets sent: %ld\n", total);
			  break;
		}
	}
	printf("Total sent %d bytes\n", j);
}

void testfd()
{
	int fd;
	char buf[BUFFER_SIZE];
	int i, j, n;
	i = j = 0;
	fd = open("test.vob", _O_RDONLY | _O_BINARY);
	while((n = read(fd, buf, BUFFER_SIZE)) > 0) {
		j += n;
		printf("%d read data %ld\n", ++i, j);	
	}
	close(fd);
}

int getopt(char *s)
{
	int n = 0;
	printf("\nUsage: \n1: Basic test and this help\n2: TCP server test\n");
	printf("3: TCP receive test [ip]\n4: UDP (Wijet PSM) receive [rip]\n5: UDP send test [uip]\n");
	printf("6: Multicast receive test\n7: Multicast sending test\n");
	printf("8: Multicast server test\n9: Multicast client [ip]\n0: Quit\n");
	scanf("%d", &n);
	printf("require IP?\n");
	scanf("%s", s);
	if (s[0] == 'y' || s[0] == 'Y') {
		printf("input IP please\n");
		scanf("%s", s);
	}
	else s[0] = 0;

	return n;
}
////////////////////////////////////////////////////////////////
// getip1.cpp
//
// ??????????????IP??
// ????????:
//
// cl getip1.cpp wsock32.lib
//
// ??????????????LIB????;????vcvars32.bat
//
#include <winsock.h>
#include <wsipx.h>
#include <wsnwlink.h>
#include <stdio.h>

int internalmain()
{
   ////////////////
   // ??? Windows sockets API. ????? version 1.1
   //
   WORD wVersionRequested = MAKEWORD(1, 1);
   WSADATA wsaData;
   if (WSAStartup(wVersionRequested, &wsaData)) {
      printf("WSAStartup failed %s\n", WSAGetLastError());
      return -1;
   }

   //////////////////
   // ?????.
   //
   char hostname[256];
   int res = gethostname(hostname, sizeof(hostname));
   if (res != 0) {
      printf("Error: %u\n", WSAGetLastError());
      return -1;
   }
   printf("hostname=%s\n", hostname);
   ////////////////
   // ???????????. 
   //
   hostent* pHostent = gethostbyname(hostname);
   if (pHostent==NULL) {
      printf("Error: %u\n", WSAGetLastError());
      return -1;
   }
   //////////////////
   // ?????hostent??.
   //
   hostent& he = *pHostent;
   printf("name=%s\naliases=%s\naddrtype=%d\nlength=%d\n",
      he.h_name, he.h_aliases, he.h_addrtype, he.h_length);

   sockaddr_in sa;
   for (int nAdapter=0; he.h_addr_list[nAdapter]; nAdapter++) {
      memcpy ( &sa.sin_addr.s_addr, he.h_addr_list[nAdapter],he.h_length);
      // ?????IP??.
      printf("Address: %s\n", inet_ntoa(sa.sin_addr)); // ?????
   }
   //////////////////
   // ?? Windows sockets API
   //
   WSACleanup();
   return 0;
}

/******************************************************************************\
* 	INTRFC.CPP
*
*	This program demonstrates how to programmatically enumerate IP interface
*	information such as a system's IP Address, Subnet mask, and broadcast
*	address through the WSAIoctl() API using the SIO_GET_INTERFACE_LIST 
*	option.  Additionally, this sample demonstrates how to interpret IP
*	status flags from each IP interface.  The flags are defined in the
*	Windows Sockets 2 Protocol-Specific Annex specification which can be
*	found in the January 98 MSDN Platform SDK.
*
*
*       Copyright 1996 - 1998 Microsoft Corporation.
*       All rights reserved.
*       This source code is only intended as a supplement to
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the
*       Microsoft samples programs.
\******************************************************************************/
/*
#define _WIN32_WINNT 0x0400
#define WINVER 0x0400

#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
*/
void samplemain(void)
{
	WORD versionRequested;
	int wsError;
	WSADATA winsockData; 
	SOCKET s;
	DWORD bytesReturned;
	char* pAddrString;
	SOCKADDR_IN* pAddrInet;
	u_long SetFlags;
	INTERFACE_INFO localAddr[10];  // Assume there will be no more than 10 IP interfaces 
	int numLocalAddr; 

	versionRequested = MAKEWORD(2, 2);
/*
	wsError = WSAStartup(versionRequested, &winsockData); 
	if (wsError)
	{ 
		fprintf (stderr, "Startup failed\n");
      		return;
	}
*/
	if((s = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0)) == INVALID_SOCKET)
	{
		fprintf (stderr, "Socket creation failed\n");
		WSACleanup();
     		return;
   	}

	// Enumerate all IP interfaces
	fprintf(stderr, "Scanning Interfaces . . .\n\n");
	wsError = WSAIoctl(s, SIO_GET_INTERFACE_LIST, NULL, 0, &localAddr,
                      sizeof(localAddr), &bytesReturned, NULL, NULL);
	if (wsError == SOCKET_ERROR)
	{
		fprintf(stderr, "WSAIoctl fails with error %d\n", GetLastError());
		closesocket(s);
		WSACleanup();
		return;
	}

	closesocket(s);

	// Display interface information
	numLocalAddr = (bytesReturned/sizeof(INTERFACE_INFO));
	for (int i=0; i<numLocalAddr; i++) 
	{
		pAddrInet = (SOCKADDR_IN*)&localAddr[i].iiAddress;
		pAddrString = inet_ntoa(pAddrInet->sin_addr);
		if (pAddrString)
			printf("IP: %s  ", pAddrString);

		pAddrInet = (SOCKADDR_IN*)&localAddr[i].iiNetmask;
		pAddrString = inet_ntoa(pAddrInet->sin_addr);
		if (pAddrString)
			printf(" SubnetMask: %s ", pAddrString);

		pAddrInet = (SOCKADDR_IN*)&localAddr[i].iiBroadcastAddress;
		pAddrString = inet_ntoa(pAddrInet->sin_addr);
		if (pAddrString)
			printf(" Bcast Addr: %s\n", pAddrString);

		SetFlags = localAddr[i].iiFlags;
		if (SetFlags & IFF_UP)
			printf("This interface is up");
		if (SetFlags & IFF_BROADCAST)
			printf(", broadcasts are supported");
		if (SetFlags & IFF_MULTICAST)
			printf(", and so are multicasts");
		if (SetFlags & IFF_LOOPBACK)
			printf(". BTW, this is the loopback interface");
		if (SetFlags & IFF_POINTTOPOINT)
			printf(". BTW, this is a point-to-point link");
		printf("\n\n");
	}

//	WSACleanup();
}

int main(int n, char* a[])
{
	char s[40];
	ULONG ip = htonl(INADDR_LOOPBACK);
	ULONG uip = INADDR_BROADCAST;
	ULONG rip = INADDR_ANY;
	SocketSystem mySockSys;
	printf( "Hello Socket! version is %d\n", mySockSys.getSystemVersion());
	if (n > 2) rip = uip = ip = inet_addr(a[2]);
	if (n > 1) n = atoi(a[1]);
	else { 
		n = getopt(s);
		if (n != 0 && *s) rip = uip = ip = inet_addr(s);
	}
	do {
	switch(n) {
	case 2:					// TCP Server
		server_test();
		break;
	case 3:					// TCP Client
		client_test(ip);
		break;
	case 4:					// UDP receive
		printf("RIP = %u %x  %x %d\n", rip, rip, INADDR_ANY, n);
		receive_test(rip);
		break;
	case 5:					// UDP send
		sending_test(uip);
		break;
	case 6:					// MC receive
		mc_receive_test();
		break;
	case 7:					// MC sending
		mc_send_test();
		break;
	case 8:					// MC server
		m_server();
		break;
	case 9:					// MC client
		m_client(ip);
		break;
	case 10:
		testfd();
		break;
	case 1:					// Basic functions
		simple_test();
//		internalmain();
//		samplemain();
		break;
	default:
		printf("\nUsage: \n1: Basic test and this help\n2: TCP server test\n");
		printf("3: TCP receive test [ip]\n4: UDP (Wijet PSM) receive [rip]\n5: UDP send test [uip]\n");
		printf("6: Multicast receive test\n7: Multicast sending test\n");
		printf("8: Multicast server test\n9: Multicast client [ip]\n\n");
		return -1;
	}
	n = getopt(s);
	if (n != 0 && *s) rip = uip = ip = inet_addr(s);
	} while(n != 0);

	return 0;
}
#endif
