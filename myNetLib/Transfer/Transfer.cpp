
// Transfer.cpp: implementation of the Transfer class.
//
// This class handles the information transfered between the two objects that 
// are among: FILE, SCREEN, TCPSVR, TCPCLIENT, UDPSEND, UDPRECV
//
//////////////////////////////////////////////////////////////////////

#include "Transfer.h"

/**
 *	The constructor
 *
 */
Transfer::Transfer()
{
	m_bufsize = this->DEFBUFF;
	m_ip = htonl(INADDR_LOOPBACK);
	m_port = 9800;
	m_buf = new char[m_bufsize+10];
	m_file = NULL;
	m_step = 4000000;
	m_in = stdin;
	m_out = stdout;
	m_sleep = 0;
	m_pos = 0;
#ifndef _WIN32
	m_lflag = 0;
#endif
}

/**
 *	The destructor
 *
 */
Transfer::~Transfer()
{
	delete [] m_buf;
	if (m_file != NULL) delete []m_file;
	termctrl(false);
}

/**
 *	termctrl method
 *
 *	@param flag - on or off
 */
void Transfer::termctrl(bool flag)
{
#ifndef _WIN32
	struct termios tmp;
	tcgetattr(0, &tmp);
	if (flag) {
		m_lflag = tmp.c_lflag;
		tmp.c_lflag &= ~(ICANON | ECHO);
	}
	else if (m_lflag != 0)
		tmp.c_lflag = m_lflag;
	tcsetattr(0, TCSANOW, &tmp);
#endif
}

/**
 *	check method
 *
 *	@param	fmt
 *	@param	code
 */
void Transfer::check(char *fmt, int code)
{
	fprintf(stderr, "!!!!!!!! Attention !!!!!!!!!: ");
	fprintf(stderr, fmt, code);
}

/**
 *	trace method
 *
 *	@param	fmt
 *	@param	any
 */
void Transfer::trace(char *fmt, int any)
{		
	fprintf(stderr, fmt, any);
}

/**
 *	setBufferSize method
 *
 *	@param	size
 */
void Transfer::setBufferSize(int size)
{
	if (size > this->DEFBUFF) {
		delete [] m_buf;
		m_bufsize = size;
		m_buf = new char[m_bufsize+10];
	}
}

/**
 *	setIP method
 *
 *	@param	ip
 *	@param	defIP
 *	@return	IP reachable with zero
 */
int Transfer::setIP(char *ip, unsigned long defIP)
{
	SocketICMP myPing;
	m_ip = inet_addr(ip);
	if (!myPing.Ping(m_ip)) {
		fprintf(stderr, "Can't reach IP %s, change to Default IP %lx\n", ip, defIP);
		m_ip = htonl(defIP);
		return 1;
	}
	return 0;
}

/**
 *	setPort method
 *
 *	@param	port
 */
void Transfer::setPort(int port)
{
	if (port > 0) m_port = port;
	else check("Port can't be negative %d\n", port);
}

/**
 *	setFile method
 *
 *	@param	file
 */
void Transfer::setFile(char *file)
{
	if (file != NULL) {
		if (m_file != NULL) delete [] m_file;
		m_file = new char[ strlen(file) + 1];
		strcpy(m_file, file);
	}
}

/**
 *	setFd method
 *
 *	@param	in
 */
void Transfer::setFd(bool in)
{
	FILE *fd;
	if (in) {
		if (m_file != NULL) {
			fd = fopen(m_file, "rb");
			if (fd == NULL) check("Open file fails (%d) Change to STDIN\n", in);
			else m_in = fd;
		}
	}
	else {
		if (m_file != NULL) {
			fd = fopen(m_file, "wb");
			if (fd == NULL) check("Open file fails (%d) Change to STDOUT\n", in);
			else m_out = fd;
		}
	}
}

/**
 *	setStep method
 *
 *	@param	step
 */
void Transfer::setStep(long step)
{
	m_step = step;
}

/**
 *	setSleep method
 *
 *	@param	sleep
 */
void Transfer::setSleep(long sleep)
{
	m_sleep = sleep;
}

/**
 *	kbhit method
 *
 *	@return keboard hit status
 */
int Transfer::kbhit()
{
#ifdef _WIN32
	return _kbhit();
#else
	fd_set fds;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(0, &fds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	return select(1, &fds, NULL, NULL, &tv);
#endif
}

/**
 *	getch method
 *
 *	@return keycode withot waiting
 */
int Transfer::getch()
{
#ifdef _WIN32
	if (this->kbhit())
	return _getch();
	else return 0;
#else
	if (m_lflag == 0) termctrl(true);
	return getchar();
#endif
}

/**
 *	controlFile method -- control input file reading
 *
 */
void Transfer::controlFile()
{
	bool pause = false;
return;
	do {
        int inkey = this->getch();
		if (inkey > 0) {
			pause = false;
			switch(inkey) {
			case 'q':
				fseek(m_in, 0, SEEK_END);
				break;
			case 'a':
				m_pos += m_step;
				fseek(m_in, m_pos, SEEK_SET);
				trace("Now (foward) is on %ld\n", m_pos);
				break;
			case 'b':
				m_pos -= m_step;
				if (m_pos < 0) m_pos = 0;
				trace("Now (back) is on %ld\n", m_pos);
				fseek(m_in, m_pos, SEEK_SET);
				break;
			case 'r':
				m_pos = 0;
				trace("Start from begining %d\n", m_pos);
				fseek(m_in, m_pos, SEEK_SET);
				break;
			case 'p':
				trace("Paused on %ld\n", m_pos);
				pause = true;
				break;
			default:
				trace("a - advance\nb -back\nr restart\np - pause\n", 0);
				trace("Now (continue) is on %ld\n", m_pos);
				break;
			}
		}	
	}while (pause);
	if (m_sleep && m_pos % 100 == 0) Sleep(m_sleep);
}

/**
 *	FileVsScreen method -- transfer message between file and screen
 *
 *	@param	infile
 *	@param	outfile
 *	@return	succussful with zero
 */
int Transfer::FileVsScreen(char *infile, char *outfile)
{
	int num;
	if (infile != NULL) this->setFile(infile);
	if (infile!=NULL) setFd(this->INPUT);
	if (outfile != NULL) this->setFile(outfile);
	if (outfile != NULL) setFd(this->OUTPUT);
	try {
		m_pos = 0;
		while((num = fread(m_buf, 1, m_bufsize, m_in)) > 0) {
			fwrite(m_buf, 1, num, m_out);
			m_pos += num;
			if (m_in != stdin) controlFile();
		}
	} catch(...) {
		check("something are wrong!\n", 1);
		return this->CATCHER;
	}
	fclose(m_in);
	fclose(m_out);
	return 0;
}

/**
 *	UDPSender method -- send message
 *
 *	@param	file
 *	@param	ip
 *	@param	port
 *	@return	succussful with zero
 */
int Transfer::UDPSender(char *file, char *ip, int port)
{
	int num, rtn;
	SocketUDP myUDP;
	if (file != NULL) this->setFile(file);
	if (ip != NULL) this->setIP(ip, INADDR_BROADCAST);
	if (port>0) this->setPort(port);

	setFd(this->INPUT);
	rtn = myUDP.Open();
	if (!rtn) {
		fclose(m_in);
		check("open UDP return %d\n", rtn);
		return this->SOCKERR;
	}
	if (m_ip == INADDR_BROADCAST) {
		rtn = myUDP.SetBroadcast();
		if (!rtn) check("SetBroadcast returns %d\n", rtn);
	}
	rtn = myUDP.Connect(m_port, m_ip);
	if (!rtn) {
		check("UDP Connect return %d\n", rtn);
		return this->SOCKERR;
	}
	myUDP.SetNonBlocking();
	try {
		m_pos = 0;
		while((num = fread(m_buf, 1, m_bufsize, m_in))>0) {
			m_pos += num;
	//		rtn = myUDP.SendTo(m_port, m_ip, m_buf, num);
	//		rtn = myUDP.Send(m_buf, num);
			rtn = myUDP.WriteExact(m_buf, &num);
			if (rtn < 1) check("Send return = %d\n", rtn);
			if (m_in != stdin) controlFile();
		}
	}
	catch(...) {
		check("stopped within transfer...\n", 1);
		return this->CATCHER;
	}
	fclose(m_in);  
	return 0;
}

/**
 *	UDPReceiver method -- eceive message and output
 *
 *	@param	ip
 *	@param	port
 *	@param	file
 *	@return	succussful with zero
 */
int Transfer::UDPReceiver(char *ip, int port, char *file)
{
	int num, rtn;
	SocketUDP myUDP;
	struct sockaddr_in raddr;
	if (ip != NULL) this->setIP(ip, INADDR_ANY);
	if (port>0) this->setPort(port);
	if (file != NULL) this->setFile(file);

	setFd(this->OUTPUT);
	rtn = myUDP.Open(m_port, INADDR_ANY);
	if (!rtn) {
		check("open UDP return %d\n", rtn);
		return this->SOCKERR;
	}
	myUDP.SetRcvTime(100000);
	try {
		while (1) {
			num = myUDP.RecvFrom(m_buf, m_bufsize, &raddr);
			if (num > 0) {
				if (ip == NULL || *(UINT *)(&raddr.sin_addr) == m_ip)
					fwrite(m_buf, 1, num, m_out);
			}
			else trace ("Receive return %d\n", num);
		}
	}
	catch(...) { 
		check("stopped within transfer...\n", 1);
		return this->CATCHER;
	}
	return 0;
}

/**
 *	TCPClientSend method -- send message 
 *
 *	@param	file
 *	@param	ip
 *	@param	port
 *	@return	succussful with zero
 */
int Transfer::TCPClientSend(char *file, char *ip, int port)
{
	int num, rtn;
	SocketTCP myTCP;
	if (ip != NULL) this->setIP(ip, INADDR_LOOPBACK);
	if (port>0) this->setPort(port);
	if (file != NULL) this->setFile(file);

	setFd(this->INPUT);
	rtn = myTCP.Open(m_port,m_ip);
	if (!rtn) { 
		check("open client return %d\n", rtn);
		fclose(m_in); 
		return this->SOCKERR;
	}
	myTCP.SetKeepAlive(5);
	myTCP.SetNonBlocking();
	try {
		m_pos = 0;
		while((num = fread(m_buf, 1, m_bufsize, m_in))>0) {
			m_pos += num;
	//		rtn = myTCP.Send(m_buf, num);
			rtn = myTCP.WriteExact(m_buf, &num);
			if (rtn < 1) check("Send return = %d\n", rtn);
			if (m_in != stdin) controlFile();
		}
		fclose(m_in);
	}
	catch(...) {
		trace("\nClient ID %d closed\n", myTCP.GetSocketID());
		return this->CATCHER;
	}
	return 0;
}

/**
 *	TCPClientReceive method -- receive message and output
 *
 *	@param	ip
 *	@param	port
 *	@param	file
 *	@return	succussful with zero
 */
int Transfer::TCPClientReceive(char *ip, int port, char*file)
{
	int num, rtn;
	SocketTCP myTCP;
	if (ip != NULL) this->setIP(ip, INADDR_LOOPBACK);
	if (port>0) this->setPort(port);
	if (file != NULL) this->setFile(file);

	setFd(this->OUTPUT);
	try {
		rtn = myTCP.Open(m_port, m_ip);
		if (!rtn) {
			check("open client return %d\n", rtn);
			return this->SOCKERR;
		}
		myTCP.SetKeepAlive(5);
		myTCP.SetRcvTime(5000);
		while((num = myTCP.Recv(m_buf, m_bufsize))>0) { 
			fwrite(m_buf, 1, num, m_out);
		}
	}
	catch(...) {
		check("something are wrong!\n", 1);
		return this->CATCHER;
	}
	fclose(m_out);
	return 0;
}

/**
 *	TCPServerSend method -- wait client connect then send a file to it
 *
 *	@param	file
 *	@param	port
 *	@return	succussful with zero
 */
int Transfer::TCPServerSend(char *file, int port)
{
    int num, rtn;
	SocketTCP myTCP;
	struct sockaddr_in addr;
	if (file != NULL) this->setFile(file);
	if (port>0) this->setPort(port);

	rtn = myTCP.OpenServer(m_port);
	if (!rtn) {
		check("Server start failuer... (return %d)\n", rtn);
		return this->SOCKERR;
	}
	try {
		while(1) {
			SocketTCP* myClient = myTCP.Accept(&addr);
			fprintf(stderr, "New Client Addr %u %s Port %d  Family %d\n",
		*(UINT *)(&addr.sin_addr), inet_ntoa(addr.sin_addr), addr.sin_port, addr.sin_family);
			myClient->SetKeepAlive(5);
			myClient->SetNonBlocking();
			try {
				setFd(this->INPUT);
				m_pos = 0;
				while((num = fread(m_buf, 1, m_bufsize, m_in))>0) {
					m_pos += num;
	//				rtn = myClient->Send(m_buf, num);
					rtn = myClient->WriteExact(m_buf, &num);
					if (rtn < 1) check("Send return = %d\n", rtn);
					if (m_in != stdin) controlFile();
				}
				fclose(m_in);
			}
			catch(...) {
				trace("\nClient ID %d closed\n", myClient->GetSocketID());
				break;
			}
			trace("End of play\n", 0);
			myClient->Close();
		}
	}
	catch(...) { 
		check("something are wrong!\n", 1);
		return this->CATCHER;
	}
	return 0;
}

/**
 *	TCPServerReceive method -- wait Client to receive message and output
 *
 *	@param	port
 *	@param	file
 *	@return	succussful with zero
 */
int Transfer::TCPServerReceive(int port, char *file)
{
    int num, rtn;
	SocketTCP myTCP;
	struct sockaddr_in addr;
    if (port>0) this->setPort(port);
	if (file != NULL) this->setFile(file);

	rtn = myTCP.OpenServer(m_port);
	if (!rtn) {
		check("Server start failuer... (return %d)\n", rtn);
		return this->SOCKERR;
	}
	try {
		while(1) {
			SocketTCP* myClient = myTCP.Accept(&addr);
			fprintf(stderr, "New Client Addr %u %s Port %d  Family %d\n",
		*(UINT *)(&addr.sin_addr), inet_ntoa(addr.sin_addr), addr.sin_port, addr.sin_family);
			myClient->SetKeepAlive(5);
			myClient->SetRcvTime(5000);
			memset(&addr, 0, sizeof(addr));
			setFd(this->OUTPUT);
			while((num = myClient->Recv(m_buf, m_bufsize))>0) 
				fwrite(m_buf, 1, num, m_out);
			trace("\nClient ID %d closed\n", myClient->GetSocketID());
			fclose(m_out);
		}
	}
	catch(...) { 
		check("something are wrong!\n", 1);
		return this->CATCHER;
	}
	return 0;
}

