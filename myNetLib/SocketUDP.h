
////////////////////////////
// UDP Socket class

#if (!defined(_UDP_SOCKET_DEFINED))
#define _UDP_SOCKET_DEFINED

#include "SocketBase.h"

class SocketUDP:public SocketBase
{
public:
	SocketUDP();
	virtual ~SocketUDP();

	// Create
	//		Create a DGRAM socket 
	bool Create();

	// Open
	//		Open a UDP socket
	bool Open(const UINT port=0, ULONG address=INADDR_ANY);

	// SetBroadcast
	//		Set Broadcast  option
	bool SetBroadcast();

	// SendTo
	//		Send message to specified port on IP
	bool SendTo(const UINT port, ULONG address, char *message, int size);

	// RecvFrom
	//		Receive message with from address 
	int RecvFrom(char *buffer, int buflen, struct sockaddr_in* addr);

};

#endif	// _UDP_SOCKET_DEFINED
