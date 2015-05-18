#include "SocketICMP.h"
/**
 *	The SocketICMP class implement Ping method
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

// defines
#define ICMP_ECHO_REQUEST   8       // ICMP echo request message type
#define ICMP_ECHO_REPLY     0       // ICMP echo reply message type
#define ICMP_MIN            8       // minimum ICMP packet(just header)
#define STATUS_FAILED       0xFFFF  // error exit status 
#define DEF_PACKET_SIZE     32      // default packet size
#define MAX_PACKET          256    // maximum packet size
#define DEF_PING			4		// default number of times to ping

// IP header: minimum 20 bytes
typedef struct _iphdr 
{
    unsigned int   h_len:4;         // length of the header: 4 bits
    unsigned int   version:4;       // version of IP: 4 bits
    unsigned char  tos;             // type of service
    unsigned short total_len;       // total length of the packet
    unsigned short ident;           // unique identifier
    unsigned short frag_and_flags;  // flags + fragment offset
    unsigned char  ttl;             // time to live
    unsigned char  proto;           // protocol (TCP, UDP etc)
    unsigned short checksum;        // IP header checksum
    unsigned int   sourceIP;        // source IP address
    unsigned int   destIP;          // destination IP address
}IpHeader;

#ifndef _WIN32
#define GetCurrentProcessId getpid
#define Sleep usleep
ULONG GetTickCount() {
  struct timeval timestamp;
  gettimeofday(&timestamp, 0);
  return (timestamp.tv_sec*1000000 + timestamp.tv_usec);
}
#endif


// ICMP header: echo request or reply message format, minimum 8 bytes
// 12 bytes if include the ICMP data portion (timestamp)
typedef struct _icmphdr 
{
    BYTE   i_type;      // ICMP message type (8 or 0)
    BYTE   i_code;      // type sub code: always 0
    USHORT i_cksum;     // ICMP message checksum 
    USHORT i_id;        // identifier, own process ID
    USHORT i_seq;       // sequence number
    ULONG  timestamp;   // ICMP data: not in standard header,
                        // space reserved for timestamp
}IcmpHeader;

/**
 *	The constructor
 *
 */
SocketICMP::SocketICMP()
{}

/**
 *	The destructor
 *
 */
SocketICMP::~SocketICMP()
{}

/**
 *	Ping method
 *
 *	@param	host
 *	@return	succussful with true
 */
bool SocketICMP::Ping(unsigned long host)
{
	SOCKET        sockRaw;
	fd_set        fds;
	struct        timeval timedout;
	struct        sockaddr_in dest,from;
	int           bread;					// return status for recvfrom()
	int           bwrote;					// return status for sendto()
	int           datasize = 0;
#ifdef _WIN32
	int           fromlen = sizeof(from);
#else
	socklen_t	  fromlen = sizeof(from);
#endif
	int			    timeout = 50;				// msec
	struct timeval   sendto_timeout;
	struct timeval	 recvfrom_timeout;
	char          *icmp_data;
	char          *recvbuf;
	USHORT        seq_no = 0;
	USHORT        process_ID;
	bool		  rtnValue = false;

	int	ping_times   = DEF_PING;
	int ntransmitted = 0;
	
	char		data[MAX_PACKET];	// using stack instead of heap
	char		rbuf[MAX_PACKET];

	// create an ICMP socket using WSASocket to create a raw socket
	// settings should be for Internet & ICMP Protocol 
#ifdef _WIN32
	sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0);
	if(sockRaw == INVALID_SOCKET){
		return false;
	}
#else
	sockRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sockRaw == -1)
		return false;

#endif
	sendto_timeout.tv_sec = 0;
	sendto_timeout.tv_usec  = timeout*1000;  
	recvfrom_timeout.tv_sec = 0;
	recvfrom_timeout.tv_usec = timeout*1000; 


	// initialize some variables
	memset(&dest,0,sizeof(dest));
	dest.sin_addr.s_addr = host;
	dest.sin_family = AF_INET;

	// note that winsock layer will prepend appropriate IP header
	// before sending out ICMP datagram through the raw socket

	// actual IP packet size sent = 32 bytes + datasize(as in argv[2])
	// IP header(20 bytes) + ICMP header(12 bytes) +
	// ICMP datasize(default 32bytes) -> default is 64 bytes

	// check datasize: maximum 1024 - 32 = 992 bytes 
	datasize = DEF_PACKET_SIZE + sizeof(IcmpHeader);  

	// malloc transmit and receive buffer spaces and initialise 
	icmp_data = (char *) data;	
	recvbuf = (char *) rbuf;	
	memset(icmp_data,0,MAX_PACKET);
	memset(recvbuf,0,MAX_PACKET);
  
	// get current process ID
	process_ID = (USHORT)GetCurrentProcessId();

	// Fill up the static part of the ICMP echo request message
	// refer to the function for further details
	fill_icmp_data(icmp_data,datasize,process_ID);
	
	while(ntransmitted != ping_times) {
		// fill up dynamic part of ICMP header before sending
		// i.e. the timestamp, sequence number and the check sum
		// use GetTickCount() to retrieve the current system time
		// use the check_sum() provided to calculate the check sum needed

		((IcmpHeader*)icmp_data)->timestamp=GetTickCount();
		((IcmpHeader*)icmp_data)->i_seq=seq_no++;

		((IcmpHeader*)icmp_data)->i_cksum=0;	
		((IcmpHeader*)icmp_data)->i_cksum=checksum((USHORT*)icmp_data, datasize);

		
		// set send timeout option use setsockopt() to set send timeout option
		bread=setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendto_timeout, sizeof(sendto_timeout));
		if(bread == SOCKET_ERROR) break;

		// Send IP datagram ICMP ECHO REQUEST message to pinged station
		// use sendto() to send a datagram to the pinged station 
		// return status to be kept in the bwrote variable
		bwrote=sendto(sockRaw, icmp_data, datasize, 0, (struct sockaddr *)&dest, datasize);
		ntransmitted++;

		// check return status for any error 
		if (bwrote == SOCKET_ERROR) break;
		
		FD_ZERO(&fds);
		FD_SET(sockRaw, &fds);
		timedout.tv_sec  =  0;
		timedout.tv_usec = timeout * 1000;
		bread = select(sockRaw + 1, &fds, 0, 0, &timedout);
		if (bread == 0) continue;
		if (bread < 0) break;

		bread=setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvfrom_timeout, sizeof(recvfrom_timeout));
		if(bread == SOCKET_ERROR) break;

		// Receive IP datagram ICMP ECHO REPLY message from pinged station
		// use recvfrom() to receive a datagram from it
		// return status to be kept in the bread variable
		bread = recvfrom(sockRaw, recvbuf, MAX_PACKET, 0, (struct sockaddr *)&from, &fromlen);

		// check return status for any error
		if (bread == SOCKET_ERROR) break;

		// Decode echo reply message from pinged station
		// refer to the function for further details
		if (decode_resp(recvbuf, bread, &from, process_ID)) {
			rtnValue = true;
			break;
		}

		// delay before sending the next datagram
		Sleep(50);

	} // end while loop
#ifdef _WIN32
	closesocket(sockRaw);
#else
	close(sockRaw);
#endif
	return rtnValue;
}


/********************************************************************\
 * 
 *  Response is an IP packet which includes the IP header, ICMP header
 *  and ICMP data(timestamp). Decode the IP header to locate the ICMP
 *  data and calculate timing statistics
 *
\********************************************************************/

/**
 *	decode_resp method
 *
 *	@param	buf
 *	@param	bytes
 *	@param	from
 *	@param	process_ID
 *	@return	succussful with true
 */
bool SocketICMP::decode_resp
(char *buf, int bytes, struct sockaddr_in *from, USHORT process_ID) 
{
    IpHeader *iphdr;
    IcmpHeader *icmphdr;
    unsigned short iphdrlen;

    iphdr = (IpHeader *)buf;
    iphdrlen = iphdr->h_len * 4 ; // number of 32-bit words *4 = bytes
    if (bytes  < iphdrlen + ICMP_MIN) return false;

    icmphdr = (IcmpHeader*)(buf + iphdrlen);

    // ICMP echo reply message expected
    if (icmphdr->i_type != ICMP_ECHO_REPLY) return false;
    
    // ICMP echo reply message is sent to all processes that have
    // created raw sockets for ICMP protocol.
    // ignore those not sent by this process
    if (icmphdr->i_id != process_ID) return false;
	return true;
}

/********************************************************************\
 *
 *  Checksum routine for Internet Protocol family headers.
 *
 *  Refer to "Computing the Internet Checksum" by R. Braden, D. Borman
 *  and C. Partridge, Computer Communication Review, Vol. 19, No. 2,
 *  April 1989, pp. 86-101, for details on computing this checksum.
 *
\********************************************************************/

/**
 *	checksum method
 *
 *	@param	buffer
 *	@param	size
 *	@return	the check sum
 */
USHORT SocketICMP::checksum
(USHORT *buffer, int size) 
{
  unsigned long cksum=0; // assume long == 32 bits

  /*
     Using a 32-bit accumulator (sum), add sequential 16-bit words to
     it, and at the end, fold back all the carry bits from the top 16
     bits into the lower 16 bits.
   */

  // add 16-bit word at a time
  while(size >1) 
  {
    cksum+=*buffer++;
    size -=sizeof(USHORT);
  }

  // last byte left?
  if(size) 
  {
    cksum += *(UCHAR*)buffer;
  }

  // add back carry outs from top 16 bits to low 16 bits
  cksum = (cksum >> 16) + (cksum & 0xffff); // add high-16 to low-16
  cksum += (cksum >>16);                    // add carry
  
  // return ones-complement, truncate to 16 bits
  return (USHORT)(~cksum);  
}

/********************************************************************\
 * 
 *  Fill in various stuff in ICMP request message
 *
\********************************************************************/

/**
 *	fill_icmp_data method
 *
 *	@param	icmp_data
 *	@param	datasize
 *	@param	process_ID
 */
void SocketICMP::fill_icmp_data
(char * icmp_data, int datasize, USHORT process_ID)
{
  IcmpHeader *icmp_hdr;
  char *datapart;

  icmp_hdr = (IcmpHeader*)icmp_data;
  icmp_hdr->i_code  = 0;                 // always zero
  icmp_hdr->i_cksum = 0;                 // checksum to be filled later
  icmp_hdr->i_seq   = 0;                 // initialise sequence number

  
  // Procedure 20 : fill up the ping message type and the current process ID 
  icmp_hdr->i_type  = ICMP_ECHO_REQUEST;  // ICMP message type ICMP_ECHO_REQUEST
										 // was defined as 8 earlier in the top
										 // of the program.
  icmp_hdr->i_id    = process_ID;
										 // We had to specify the process ID in
										 // the ID field of the ICMP request
										 // in the event that the user initiates
										 // more than one instance of the ping
										 // program, this will allow us to uniquely
										 // identify this datagram to be from which
										 // instance of the ping program.


  // place some junk in the buffer.
  datapart = icmp_data + sizeof(IcmpHeader);
  memset(datapart,'A', datasize - sizeof(IcmpHeader));
}
