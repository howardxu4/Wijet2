
////////////////////////////
// The ICMP socket class

#if (!defined(_ICMP_SOCKET_DEFINED))
#define _ICMP_SOCKET_DEFINED

#include "SocketSystem.h"

class SocketICMP
{
public:
	SocketICMP();
	virtual ~SocketICMP();

	// Ping
	//		Ping the host to check reachable
	bool Ping(unsigned long host);

private:
	bool decode_resp(char *buf, int bytes, struct sockaddr_in *from, USHORT process_ID);
	USHORT checksum(USHORT *buffer, int size);
	void fill_icmp_data(char * icmp_data, int datasize, USHORT process_ID);
};

#endif	// _ICMP_SOCKET_DEFINED
