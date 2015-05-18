
////////////////////////////
// The Base socket class

#if (!defined(_BASE_SOCKET_DEFINED))
#define _BASE_SOCKET_DEFINED

#include "SocketSystem.h"

class SocketBase
{
public:
	SocketBase();
	virtual ~SocketBase();

protected:
	inline void SetSocketID(int sid) {m_sock = sid;};

public:
	//	GetSocketID
	//		Get the socket id
	int  GetSocketID() {return m_sock;};

	// Create
	//		Create a socket (SOCK_STREAM|SOCK_DGRAM)
	void Create(int type);

	// Close
	//		Close the currently attached socket
	void Close();

	// SetOption
	//		Set the socket option		
	bool SetOption(int level, int OPT, char* para, int size);

	// SetReuseAddr
	//		Set Reuse Address option
	bool SetReuseAddr();

	// SetRcvTime0
	//		Set Receive Time option
	bool SetRcvTime(int time);

	// SetSndTime0
	//		Set Send Time option
	bool SetSndTime(int time);

	// Bind
	//		Bind the attached socket to the specified port
	bool Bind(const UINT port=0, ULONG address=INADDR_ANY);

	// Connect
	//		Make a socket connection to the specified port
	//        to the IP.
	bool Connect(const UINT port, ULONG address);

	// Send
	//		Send message
	int Send(const char *buffer, int buflen);

	// Recv
	//		Receive message
	int Recv(char *buffer, int buflen, int flag=0);

	// SetNonBlocking
	//		Set NonBlocking IO option
	int SetNonBlocking();

	// ReadExact
	//		Read Exact length of data
	int ReadExact(char *buf, int *length);

	// WriteExact
	//		Write Exact length of data
	int WriteExact(char *buf, int *length);

	// GetAddress
	//		Get IP from a IP string
	ULONG GetAddress(const char * address);

	// GetSockName
	//		Get Socket Name
	char* GetSockName();

	// GetError
	//		Get Socket Error info
	inline int GerError() { return m_error; };

protected:
	int m_error;
	int m_sock;
	int m_family;
};

#endif	// _BASE_SOCKET_DEFINED


