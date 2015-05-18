
////////////////////////////
// The Sender class

#if (!defined(_SENDER_CLASS_DEFINED))
#define _SENDER_CLASS_DEFINED

#include "myNetWK.h"

class Sender
{
public:
	Sender(int port);
	~Sender();
	int SetIP(unsigned long ip, unsigned long defIP=INADDR_LOOPBACK);
	int SetIP(char *ip, unsigned long defIP=INADDR_LOOPBACK);
	void SetProtocol(int protocol);
	int Open();	
	int Send(char *msg, int len);
	int Echo(char *buf, int len);

	enum {
		PROT_TCP ,
		PROT_UDP ,
		PROT_MUL 
	};
private:
	unsigned short	m_port;
	unsigned long	m_ip;
	int		m_protocol;
	SocketBase*	m_socket;
};
#endif	// _SENDER_CLASS_DEFINED

