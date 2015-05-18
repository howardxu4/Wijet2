
////////////////////////////
// The Socket System class

#if (!defined(_SOCKET_SYSTEM_DEFINED))
#define _SOCKET_SYSTEM_DEFINED

#ifdef _WIN32

#pragma comment(lib, "ws2_32.lib")
#pragma pack(4)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>              // header file for winsock 2.0  
#include <ws2tcpip.h>		// WinSock2 Extension for TCP/IP protocols	
#include <stdio.h>

#else
//	Linux Include files

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <semaphore.h>
#include <error.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

//	Simulate Windows types
#define SOCKET_ERROR -1

#define SOCKET	int
#define BYTE	char
#define UCHAR	unsigned char
#define USHORT	unsigned short
#define ULONG   unsigned long
#define UINT	unsigned int

#define	Sleep	usleep

#endif		// _WINN32

class SocketSystem
{
public:
	SocketSystem();
	~SocketSystem();

#ifdef _WIN32
	bool getSystemStatus() {return m_status;};
	WORD getSystemVersion() {return m_socketVersion;};
private:
	bool m_status;
    WORD m_socketVersion;
#endif

};

#endif	// _SOCKET_SYSTEM_DEFINED
