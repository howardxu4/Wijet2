
#include "MultiCastUDP.h"
/**
 *	The MultiCastUDP class is the MultiCast UDP Networking class inheriting from SockBase
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

/**
 *	The constructor
 *
 */
MultiCastUDP::MultiCastUDP()
{
}

/**
 *	The destructor
 *
 */
MultiCastUDP::~MultiCastUDP()
{
}

/**
 *	Open method
 *
 *	@param	port
 *	@param	off
 *	@param	ttl
 *	@param	iaddr
 *	@return	succussful with true
 */
bool MultiCastUDP::Open(int port, int off, int ttl, ULONG iaddr)
{
	if (!SocketUDP::Create()) return false;
	if(!Bind (port, iaddr)) return false;
	SetLoop(off);
	return (SetTTL(ttl));
}

/**
 *	Join method
 *
 *	@param	grpaddr
 *	@param	iaddr
 *	@return	succussful with true
 */
bool MultiCastUDP::Join(ULONG grpaddr, ULONG iaddr)
{
	struct ip_mreq imr; 
	memset(&imr, 0, sizeof(imr));
	imr.imr_multiaddr.s_addr  = grpaddr;
	imr.imr_interface.s_addr  = iaddr;
	return SetOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&imr, sizeof(imr));  
}

/**
 *	Drop method
 *
 *	@param	grpaddr
 *	@param	iaddr
 *	@return	succussful with true
 */
bool MultiCastUDP::Drop(ULONG grpaddr, ULONG iaddr)
{
	struct ip_mreq imr; 
	memset(&imr, 0, sizeof(imr));
	imr.imr_multiaddr.s_addr  = grpaddr;
	imr.imr_interface.s_addr  = iaddr;
	return SetOption(IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&imr, sizeof(imr));  
}

/**
 *	SetLoop method
 *
 *	@param	off
 *	@return	succussful with true
 */
bool MultiCastUDP::SetLoop(int off)
{
	return SetOption(IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&off, sizeof(int));
}

/**
 *	SetTTL method
 *
 *	@param	ttl
 *	@return	succussful with true
 */
bool MultiCastUDP::SetTTL(int ttl)
{
	return SetOption(IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(int));
}

/**
 *	SetInterface method
 *
 *	@param	iaddr
 *	@return	succussful with true
 */
bool MultiCastUDP::SetInterface(ULONG iaddr)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = iaddr;
	return SetOption(IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr, sizeof(addr));  
}

