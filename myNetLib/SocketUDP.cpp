
#include "SocketUDP.h"
/**
 *	The SocketUDP class is the UDP Networking class inheriting from SocketUDP
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

/**
 *	The constructor
 *
 */
SocketUDP::SocketUDP()
{
}

/**
 *	The destructor
 *
 */
SocketUDP::~SocketUDP()
{
}

/**
 *	Create method
 *
 *	@return	succussful with true	
 */
bool SocketUDP::Create()
{
	SocketBase::Create(SOCK_DGRAM);
	if (m_sock < 0) return false;
	return (SetReuseAddr());
}

/**
 *	Open method
 *
 *	@param	port
 *	@param	iaddr
 *	@return	succussful with true
 */
bool SocketUDP::Open(const UINT port, ULONG iaddr)
{
	if(!Create()) return false;
	return (Bind(port, iaddr));
}

/**
 *	SetBroadcast method
 *
 *	@return	succussful with true
 */
bool SocketUDP::SetBroadcast()
{
	const int one = 1;
	return (setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, 
		(char*)&one, sizeof(one)) == 0);
}

/**
 *	SendTo method
 *
 *	@param	port
 *	@param	address
 *	@param	message
 *	@param	size
 *	@return	succussful with true
 */
bool SocketUDP::SendTo(const UINT port, ULONG address, char *message, int size)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = m_family;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = address;
	return (sendto (m_sock, message, size, 0, 
		(struct sockaddr *)&addr, sizeof(addr)) != SOCKET_ERROR) ; 
}

/**
 *	RecvFrom method
 *
 *	@param	buffer
 *	@param	buflen
 *	@param	addr
 *	@return the number of bytes received
 */
int SocketUDP::RecvFrom(char *buffer, int buflen, struct sockaddr_in* addr)
{
	if (addr != NULL) {
#ifdef _WIN32
		int size = sizeof (sockaddr);
#else
		socklen_t size = sizeof(sockaddr);
#endif
		return recvfrom (m_sock, buffer, buflen, 0, (sockaddr*) addr, &size);
	}
	return Recv(buffer, buflen);
}

