
#include "SocketSystem.h"
/**
 *	The SocketSystem class is the Socket System class for Windows Socket
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

/**
 *	The constructor
 *
 */
SocketSystem::SocketSystem()
{
  // Initialise the socket subsystem

#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
 
	wVersionRequested = MAKEWORD( 2, 0 );
	m_status = (WSAStartup( wVersionRequested, &wsaData ) == 0);
	m_socketVersion = wsaData.wVersion;
#endif
}

/**
 *	The destructor
 *
 */
SocketSystem::~SocketSystem()
{
#ifdef _WIN32
	if (m_status)
		WSACleanup();
#endif
}

