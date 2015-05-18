
////////////////////////////
// MultiCast UDP Socket class

#if (!defined(_MultiCast_UDP_DEFINED))
#define _MultiCast_UDP_DEFINED

#include "SocketUDP.h"

class MultiCastUDP:public SocketUDP
{
public:
	MultiCastUDP();
	virtual ~MultiCastUDP();

	// Open
	//		Open a MultiCast UDP socket 
	bool Open(int port, int off=0, int ttl=32, ULONG iaddr=INADDR_ANY);

	// Join
	//		Join to a group
	bool Join(ULONG grpaddr, ULONG iaddr=INADDR_ANY);

	// Drop
	//		Drop from a group
	bool Drop(ULONG grpaddr, ULONG iaddr=INADDR_ANY);

	// SetLoop
	//		Set Loop option
	bool SetLoop(int off=0);

	// SetTTL
	//		Set TTL option
	bool SetTTL(int ttl);

	// SetInterface
	//		Set Interface
	bool SetInterface(ULONG iaddr=INADDR_ANY);
};

#endif // _MultiCast_UDP_DEFINED
