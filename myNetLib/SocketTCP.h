
////////////////////////////
// TCP Socket class

#if (!defined(_TCP_SOCKET_DEFINED))
#define _TCP_SOCKET_DEFINED

#include "SocketBase.h"

class SocketTCP:public SocketBase
{
public:
	SocketTCP();
	virtual ~SocketTCP();

	// Create
	//		Create a DGRAM socket 
	bool Create();

	// SetNoDelay
	//		Set No Delay option
	bool SetNoDelay();

	// SetKeepAlive
	//		Set Keep Alive option
	bool SetKeepAlive(int time);

	// Listen
	//		Listen the connection
	bool Listen(int nQueue=5);

	// OpenServer
	//		Open a Server Socket
	bool OpenServer(int port, int nQueue=5, ULONG iaddr=INADDR_ANY);

	// Open
	//		Open a Client socket
	bool Open(int port, ULONG addr);

	// Accept
	//		Accept a incoming connection socket
	SocketTCP* Accept(struct sockaddr_in *addr=NULL);
};

#endif	// _TCP_SOCKET_DEFINED
