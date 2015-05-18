
////////////////////////////
// The Net Interface Card Access class

#if (!defined(_NIC_ACCESS_DEFINED))
#define _NIC_ACCESS_DEFINED

#include "SocketSystem.h"

class NICAccess
{
public:
	NICAccess();
	virtual ~NICAccess();
	void	updateMaskIP();
	int		getNumMaskIP(){ return m_nMaskIP; };
	ULONG	getMaskAddr( int index=0 );
	ULONG	getIPAddr( int index=0 );
	bool	isNetIPZero(){ return m_NetIpIsZero; };

private:
	enum { MASK_ADDR = 0, IP_ADDR = 1, MAX_SIZE = 10 };
	ULONG	m_MaskIP[MAX_SIZE][2];		// support multiple interface cards
	UINT	m_nMaskIP;
	bool	m_NetIpIsZero;
};

#endif	// _NIC_ACCESS_DEFINED
