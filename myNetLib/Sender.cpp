
#include "Sender.h"
/**
 *      The Sender class is a class for sending message to Network
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

/**
 *      The constructor
 *
 */
Sender::Sender(int port)
{
	m_port = port;
	m_ip = htonl(INADDR_LOOPBACK);
	m_protocol = PROT_TCP;
	m_socket = new SocketTCP();
}

/**
 *      The destructor
 *
 */
Sender::~Sender()
{
	delete m_socket;
}

/**
 *	SetIP method
 *
 *      @param  ip
 *      @param  defIP
 *      @return IP reachable with zero
 */
int Sender::SetIP(unsigned long ip, unsigned long defIP)
{
        SocketICMP myPing;
        m_ip = ip;
        if (!myPing.Ping(m_ip)) {
                fprintf(stderr, "Can't reach IP %lx, change to Default IP %lx\n", ip, defIP);
                m_ip = htonl(defIP);
                return 1;
        }
        return 0;
}

int Sender::SetIP(char *ip, unsigned long defIP)
{
        return SetIP(inet_addr(ip), defIP);
}

/**
 *	SetProtocol method
 *
 *	$param	protocol
 */
void Sender::SetProtocol(int protocol)
{
	delete m_socket;
	m_protocol = protocol;
	if (m_protocol == PROT_UDP) 
		m_socket = new SocketUDP();
	else if (m_protocol == PROT_MUL)
		m_socket = new MultiCastUDP();
	else 
		m_socket = new SocketTCP();
	
}
	
/**
 *	Open method
 *
 *	@return	successful with true
 */
int Sender::Open()
{
	if (m_protocol == PROT_UDP)
                return ((SocketUDP*)m_socket)->Open( m_port, m_ip );
        else if (m_protocol == PROT_MUL) {
                if (((MultiCastUDP*)m_socket)->Open( m_port ))
			return ((MultiCastUDP*)m_socket)->Join( m_ip );
		return 0;
	}
        else
		return ((SocketTCP*)m_socket)->Open( m_port, m_ip );

}

/**
 *      Send method - sending message
 *
 *	@param	msg
 *	@param	len
 *	@return number of bytes sent
 */
int Sender::Send(char *msg, int len)
{
	int rtn;
	if (m_socket->GetSocketID() == -1) 
		rtn = Open();
	else	
		rtn = 1;
	if (rtn)
		return m_socket->WriteExact(msg, &len);
	return rtn;
}
/**
 *      Echo method - receive echo
 *
 *      @param  buf
 *      @param  len
 *      @return number of bytes receive
 */
int Sender::Echo(char *buf, int len)
{
	return m_socket->Recv(buf, len);
}
