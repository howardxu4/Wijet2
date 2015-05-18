
#if !defined(_TRANSFER_FILE_SCREEN_SOCKET_DEFINED)
#define _TRANSFER_FILE_SCREEN_SOCKET_DEFINED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include "..\myNetWK.h"
#include <conio.h>
#else
#include "../myNetWK.h"
#include <termios.h>
#endif

class Transfer  
{
public:
	Transfer();
	virtual ~Transfer();
	void setBufferSize(int size);
	int  setIP(char *ip, unsigned long defIP);
	void setPort(int port);
	void setFile(char *file);
	void setFd(bool in);
	void setStep(long step);
	void setSleep(long sleep);

	int FileVsScreen(char *infile=NULL, char *outfile=NULL);
	int UDPSender(char *file=NULL, char *ip=NULL, int port=-1);
	int UDPReceiver(char *ip=NULL, int port=-1, char *file=NULL);
	int TCPClientSend(char *file=NULL, char *ip=NULL, int port=-1);
	int TCPClientReceive(char *ip=NULL, int port=-1, char*file=NULL);
	int TCPServerSend(char *file=NULL, int port=-1);
	int TCPServerReceive(int port=-1, char *file=NULL);

private:
	enum {	CATCHER=-2,
			SOCKERR=-1,
			OUTPUT=0,
			INPUT=1,
			DEFBUFF=1024 
	};

	int		m_bufsize;
	unsigned long	m_ip;
	unsigned short	m_port;
	char		*m_buf;
	char		*m_file;
	long		m_pos;
	long		m_step;
	long		m_sleep;
	FILE		*m_in;
	FILE		*m_out;
#ifndef _WIN32
	tcflag_t	m_lflag;	
#endif

	void check(char *fmt, int code);
	void trace(char *fmt, int any);
	void controlFile();
	void termctrl(bool flag);
	int  kbhit();
	int  getch();
};

#endif // !defined(_TRANSFER_FILE_SCREEN_SOCKET_DEFINED)
