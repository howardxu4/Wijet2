
#include "NICAccess.h"
/**
 *	The NICAccess class is the Network Interface Card discovery class
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

/**
 *	The constructor
 *
 */
NICAccess::NICAccess()
{
	updateMaskIP();
}

/**
 *	The destructor
 *
 */
NICAccess::~NICAccess()
{
}

/**
 *	GetMaskAddr method
 *
 *	@param	index
 *	@return mask address of index
 */
ULONG NICAccess::getMaskAddr( int index ) 
{
	UINT i = (UINT)index;
	if (i > m_nMaskIP && m_nMaskIP) 
		i %= m_nMaskIP;
	return m_MaskIP[i][MASK_ADDR];
}

/**
 *	getIPAddr method
 *
 *	@param	index
 *	@return IP address of index
 */
ULONG NICAccess::getIPAddr( int index )
{
	UINT i = (UINT)index;
	if (i > m_nMaskIP && m_nMaskIP) 
		i %= m_nMaskIP;
	return m_MaskIP[i][IP_ADDR];
}

/**
 *	updateMaskIP method
 *
 */
void NICAccess::updateMaskIP()
{
	int wsError;
	SOCKET s;
	DWORD bytesReturned;
	SOCKADDR_IN* pAddrInet;
	u_long SetFlags;
	INTERFACE_INFO localAddr[10];  // Assume there will be no more than 10 IP interfaces 
	int numLocalAddr; 

	// initialize 
	m_nMaskIP = 0;
	m_NetIpIsZero = false;
	memset(m_MaskIP, 0, sizeof(m_MaskIP));

	if((s = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0)) == INVALID_SOCKET)
     		return;

	// Enumerate all IP interfaces
	wsError = WSAIoctl(s, SIO_GET_INTERFACE_LIST, NULL, 0, &localAddr,
                      sizeof(localAddr), &bytesReturned, NULL, NULL);
	closesocket(s);
	if (wsError == SOCKET_ERROR)
		return;

	// Display interface information
	numLocalAddr = (bytesReturned/sizeof(INTERFACE_INFO));
	for (int i=0; i<numLocalAddr; i++) 
	{

		SetFlags = localAddr[i].iiFlags;
		if (SetFlags & IFF_LOOPBACK) continue;

		pAddrInet = (SOCKADDR_IN*)&localAddr[i].iiAddress;
		if(pAddrInet->sin_addr.S_un.S_addr == 0) {
			m_NetIpIsZero = true;
			continue; // 0.0.0.0
		}
		m_MaskIP[m_nMaskIP][IP_ADDR] = pAddrInet->sin_addr.S_un.S_addr;
		pAddrInet = (SOCKADDR_IN*)&localAddr[i].iiNetmask;
		m_MaskIP[m_nMaskIP][MASK_ADDR] = pAddrInet->sin_addr.S_un.S_addr;
		m_nMaskIP++;

	}
}


