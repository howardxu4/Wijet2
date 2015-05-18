
#include "SocketTCP.h"
/**
 *	The SocketTCP class is the TCP Networking class inheriting from SockBase
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

/**
 *	The constructor
 *
 */
SocketTCP::SocketTCP()
{
}

/**
 *	The destructor
 *
 */
SocketTCP::~SocketTCP()
{
}

/**
 *	Create method
 *
 *	@return	succussful with true	
 */
bool SocketTCP::Create()
{
	SocketBase::Create(SOCK_STREAM);
	if (m_sock < 0) return false;
	if (!SetReuseAddr()) return false;
	return (SetNoDelay());
}

/**
 *	SetNoDelay method
 *
 *	@return	succussful with true
 */
bool SocketTCP::SetNoDelay()
{
	const int one = 1;
	return (setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, 
		(char*)&one, sizeof(one)) == 0);
}

/**
 *	SetKeepAlive method
 *
 *	@return	succussful with true
 */
bool SocketTCP::SetKeepAlive(int time)
{
	int t = time;
	return (setsockopt(m_sock, SOL_SOCKET, SO_KEEPALIVE,
		(char*)&t, sizeof(int)) == 0);
}

/**
 *	Listen method
 *
 *	@param	nQueue
 *	@return	succussful with true
 */
bool SocketTCP::Listen(int nQueue)
{
	return (listen(m_sock, nQueue) >= 0);
}

/**
 *	OpenServer method
 *
 *	@param	port
 *	@param	nQueue
 *	@param	iaddr
 *	@return	succussful with true
 */
bool SocketTCP::OpenServer(int port, int nQueue, ULONG iaddr)
{
	if(!Create()) return false;
	if(!Bind(port, iaddr)) return false;
	return (Listen(nQueue));
}

/**
 *	Open method
 *
 *	@param	port
 *	@param	addr
 *	@return	succussful with true
 */
bool SocketTCP::Open(int port, ULONG addr)
{
	if(!Create()) return false;
	return (Connect(port, addr));
}

/**
 *	Accept	method
 *
 *	@param	addr
 *	@return new SocketTCP object
 */
SocketTCP* SocketTCP::Accept(struct sockaddr_in *addr)
{
	int new_socket_id;
	SocketTCP * new_socket;
#ifdef _WIN32
	int addrlen = sizeof(struct sockaddr);
#else
	socklen_t	addrlen = sizeof(struct sockaddr);
#endif
	// Accept an incoming connection
	if (addr==NULL) 
		new_socket_id = accept(m_sock, NULL, 0);
	else
		new_socket_id = accept(m_sock, (sockaddr*)addr, &addrlen);
	if (new_socket_id < 0) return NULL;

	// Create a new SocketTCP and return it
	new_socket = new SocketTCP();
	new_socket->SetSocketID(new_socket_id);
	// Attempt to set the new socket's options
	new_socket->SetNoDelay();

	return new_socket;
}
