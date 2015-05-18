
#include "SocketBase.h"
/**
 *	The SocketBase class is the base class for Network family class
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

/**
 *	The constructor
 *
 */
SocketBase::SocketBase()
{
	m_family = AF_INET;
	m_sock = -1;
	m_error = 0;
}

/**
 *	The destructor
 *
 */
SocketBase::~SocketBase()
{
	Close();
}

/**
 *	Create method
 *
 *	@param	type: SOCK_STREAM, SOCK_DGRAM
 */
void SocketBase::Create(int type)
{
	Close();
	m_sock = socket(m_family, type, 0);
}

/**
 *	Close method
 *
 */
void SocketBase::Close()
{
	if (m_sock >= 0)
	{
#ifdef _WIN32
          shutdown(m_sock, SD_BOTH);
          closesocket(m_sock);
#else
	  shutdown(m_sock, 2);
	  close(m_sock);
#endif
	  m_sock = -1;
	}
	m_error = 0;
}

/**
 *	SetOption method
 *	@param	level
 *	@param	OPT
 *	@param	para
 *	@param	size
 *	@return	succussful with true
 */
bool SocketBase::SetOption(int level, int OPT, char* para, int size)
{
	return (setsockopt(m_sock, level, OPT, para, size) == 0);
}

/**	SetReuseAddr method
 *
 *	@return	succussful with true
 */
bool SocketBase::SetReuseAddr()
{
	const int one = 1;
	return (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, 
		(char*)&one, sizeof(one)) == 0);
}

/**
 *	SetRcvTime method
 *
 *	@param	time
 *	@return	succussful with true
 */
bool SocketBase::SetRcvTime(int time)
{
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = time*1000;
	return (setsockopt(m_sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&t,sizeof(t))==0);
}

/**
 *	SetSndTime method
 *
 *	@param	time
 *	@return	succussful with true
 */
bool SocketBase::SetSndTime(int time)
{
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = time*1000;
	return (setsockopt(m_sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&t,sizeof(t))==0);
}

/**
 *	Bind method
 *
 *	@param	port
 *	@param	address
 *	@return	succussful with true
 */
bool SocketBase::Bind(const UINT port, ULONG address)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = m_family;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = address;
	return bind(m_sock, (struct sockaddr *)&addr, sizeof(addr)) == 0;
}

/**
 *	Connect method
 *
 *	@param	port
 *	@param	address
 *	@return	succussful with true
 */
bool SocketBase::Connect(const UINT port, ULONG address)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = m_family;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = address;
	return connect(m_sock, (struct sockaddr *)&addr, sizeof(addr)) == 0;
}

/**
 *	Send method
 *
 *	@param	buffer
 *	@param	buflen
 *	@return the number of bytes sent
 */
int SocketBase::Send(const char *buffer, int buflen)
{
	return send(m_sock, buffer, buflen, 0);
}

/**
 *	Recv method
 *	
 *	@param	buffer
 *	@param	buflen
 *	@return the number of bytes received
 */
int SocketBase::Recv(char *buffer, int buflen, int flag)
{
	return recv(m_sock, buffer, buflen, flag);
}

/**
 *	SetNonBlocking method
 *
 *	@return	successful with zero. 
 *		Otherwise, a value of SOCKET_ERROR is returned
 */
int SocketBase::SetNonBlocking()
{
	ULONG ioctlopt = 1;
#ifdef	_WIN32
	return (ioctlsocket(m_sock, FIONBIO, &ioctlopt));
#else
	return (ioctl(m_sock, FIONBIO, &ioctlopt));
#endif
}

#define MAX_WAIT_TIME 4000

/**
 * ReadExact method
 * 
 *	@param	buf
 *	@param	length
 *	@return values fall into three categories:
 *		1) < 0. This indicates some error (such as
 *         timeouts or system errors)
 *		2) = 0. This indicates that the peer has
 *         closed the connection. The value 
 *         pointed to by the length may be
 *         changed to reflect the number of
 *         bytes read so far.
 *		3) = 1. This means the required number of
 *         bytes have been successfully read.
 */
int SocketBase::ReadExact(char *buf, int *length)
{
	int n, result;
	fd_set fds;
	struct timeval tv;
	int totalTimeWaited = 0;
	int len = *length;

	while(len > 0) {
		FD_ZERO(&fds);
		FD_SET((UINT)m_sock, &fds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		result = select(m_sock + 1, &fds, NULL, NULL, &tv);
		if (result == 0) {							// timed out
			totalTimeWaited += 1000;
			if (totalTimeWaited >= MAX_WAIT_TIME) return -1;
		} else if (result < 0){						// error in select
			return result;
		} else if (FD_ISSET((UINT)m_sock, &fds)) {	// packets arrived
			totalTimeWaited = 0;// clear timer
			n =  recv(m_sock, buf, len, 0);
			if (n > 0) {			// read some data
				buf += n;
				len -= n;
			} else if ( n == 0) {	// peer reset the socket
				*length -= len;	// bytes read so far
				return 0;
			} else {				// error in read data
#ifdef _WIN32
				if (WSAGetLastError() != WSAEWOULDBLOCK) return n;
#else
				if (errno != EWOULDBLOCK && errno != EAGAIN) return n;
#endif
			}
		}
	}
	return 1; // success
}

/**
 * WriteExact method
 * 
 *	@param	buf
 *	@param	length
 *	@return values fall into three categories:
 *		1) < 0. This indicates some error has occurred.
 *		2) = 0. This indicates that the peer has closed
 *         the connection. The value pointed to by
 *         length has been changed to reflect the 
 *         number of bytes written so far.
 *		3) > 0. This means length of bytes have been 
 *         successfully written.
 */
int SocketBase::WriteExact(char *buf, int *length)
{
	int n, result;
	fd_set fds;
	struct timeval tv;
	int totalTimeWaited = 0;
	int len = *length;

	while(len > 0) {
		FD_ZERO(&fds);
		FD_SET((UINT)m_sock, &fds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		result = select(m_sock+1, NULL, &fds, NULL, &tv);
		if (result == 0) {							// timed out
			totalTimeWaited += 1000;
			if (totalTimeWaited >= MAX_WAIT_TIME) return -1;
		} else if (result < 0){						// error in select
			return result;
		} else if (FD_ISSET((UINT)m_sock, &fds)) {	// sending packets 
			totalTimeWaited = 0;// clear timer
			n =  send(m_sock, buf, len, 0);
			if (n > 0) {				// write some data
				buf += n;
				len -= n;		// bytes write so far
			} else if (n == 0){			// peer resets the socket
				*length -= len;	
				return 0;
			} else {					// error in write data
#ifdef _WIN32
				if (WSAGetLastError() != WSAEWOULDBLOCK) return n;
#else
				if (errno != EWOULDBLOCK) return n;
#endif
			}
		}
	}
	return 1; // success
}

/**
 *	GetAddress method
 *
 *	@param	address
 *	@return long integer of address
 */
ULONG SocketBase::GetAddress(const char * address)
{
	ULONG addr = inet_addr(address);
	// Was the string a valid IP address?
	if (addr == INADDR_NONE)
	{
	  // No, so get the actual IP address of the host name specified
	  struct hostent *pHost;
	  pHost = gethostbyname(address);
	  if (pHost != NULL)
	  {
		  if (pHost->h_addr == NULL) return INADDR_NONE;
		  addr = ((struct in_addr *)pHost->h_addr)->s_addr;
	  }
	}
	return addr;
}

/**
 *	GetSockName	method
 *
 *	@return the string of name
 */
char* SocketBase::GetSockName()
{
	struct sockaddr_in	sockinfo;
	struct in_addr		address;
#ifdef	_WIN32
	int			sockinfosize = sizeof(sockinfo);
#else
        socklen_t               sockinfosize = sizeof(sockinfo);
#endif
	char*				name;

	// Get the peer address for the client socket
	getsockname(m_sock, (struct sockaddr *)&sockinfo, &sockinfosize);
	memcpy(&address, &sockinfo.sin_addr, sizeof(address));

	name = inet_ntoa(address);
	if (name == NULL)
		return "<unavailable>";
	else
		return name;
}
