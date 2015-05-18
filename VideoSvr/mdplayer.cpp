#include "mdplayer.h"

MdPlay::MdPlay(LogTrace* log)
{
	if (log!=NULL) m_Log = log;
	else
	{
	m_Log = new LogTrace("MdPlay");
	m_Log->setLog(LogTrace::ERROR);
	}
	m_continue = 1;
	m_pause = 0;
	m_Data = NULL;
	m_buffer = new char[9000]; 
	m_multicastIP = 0;
	m_eos = false;
	m_myThread = 0;
	m_size = 0;
	m_Log->log(LogTrace::INFO, "Media Player class");
	m_useACCEL = true;
	system("umount -f /mnt1");         // 10-09-06
}

MdPlay::~MdPlay()
{
	stop();
	delete m_buffer;
//	system("umount -f /mnt1");
	m_Log->log(LogTrace::INFO, "destruction");
}

int MdPlay::init(SocketTCP* cmd)
{
	m_Command = cmd;
	int n;
	if (!m_xineListener.OpenServer(VIDEOJET_XINE_PORT)) {
		m_Log->log(LogTrace::ERROR, "Xine Socket Open fails");
		return -3;
	}
	
	if (m_Command == NULL) n = 2;	// Test mode
	else
	n = ProcessStartStreamingRequest(); // 0: Stream 1: Mapping
        if (n < 0) {
			m_Log->log(LogTrace::ERROR, "Failed to get a start media streaming request");
			return -1;
        }
        if (n == 0) {
			if (!m_dataListener.OpenServer(VIDEOJET_DATA_PORT)) {
				m_Log->log(LogTrace::ERROR, "Data Socket Open fails");
				return -2;
			}
			strcpy(m_mrl, XINEMRL);
        }
	
	m_Output = NULL;
	m_rect[0] = m_rect[1] = m_rect[2] = m_rect[3] = 0;

	return 0;
}

void MdPlay::run()
{
	fd_set fds;
	int n, maxfd, xinefd, cmdfd;
	struct timeval timeout;
	
	m_Log->log(LogTrace::INFO, "Media Player running... [%d]", m_useACCEL);
	xinefd = m_xineListener.GetSocketID();

	if (strcmp(m_mrl, XINEMRL) != 0) {	// MAPPING
        while (m_continue) {
                timeout.tv_sec = VIDEOJET_TIMEOUT;
                timeout.tv_usec = 0;
		cmdfd = m_Command->GetSocketID();
                FD_ZERO(&fds);
                FD_SET(cmdfd,  &fds);
                FD_SET(xinefd, &fds);
		maxfd = cmdfd;
                if (xinefd > maxfd) maxfd = xinefd;
                n = select(maxfd + 1, &fds, NULL, NULL, &timeout);
                if (n <= 0) {		// TIMEOUT ?
                	m_Log->log(LogTrace::INFO, "Waitting to get a cmd or data packet");
			continue;
                }

		// handle command packets
                if (FD_ISSET(cmdfd, &fds)) {
			n = ProcessControlRequest();
			if (n < 0) {
				m_Log->log(LogTrace::ERROR, "Failed to process control request");
				break;
			}
                }

                // handle xine connection
                if (FD_ISSET(xinefd, &fds)) {
                        m_Output = m_xineListener.Accept();
                }
	    }
	}
	else
	while (m_continue) {
		timeout.tv_sec = VIDEOJET_TIMEOUT;
		timeout.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(m_Command->GetSocketID(),  &fds);
		FD_SET(xinefd, &fds);
		if (m_Data == NULL) {
			FD_SET(m_dataListener.GetSocketID(), &fds);
			maxfd = m_dataListener.GetSocketID();
		}
		else {
			FD_SET(m_Data->GetSocketID(), &fds);
			maxfd = m_Data->GetSocketID();
		}
		if (maxfd < m_Command->GetSocketID())
			maxfd = m_Command->GetSocketID();
		if (xinefd > maxfd) maxfd = xinefd;
		n = select(maxfd + 1, &fds, NULL, NULL, &timeout);
		if (n <= 0) {
                        if (m_Data == NULL) continue;
			m_Log->log(LogTrace::ERROR, "Failed to get a cmd or data packet");
			// if TIME_OUT ?
			break;
        	}

        // handle command packets
		if (FD_ISSET(m_Command->GetSocketID(), &fds)) {
			n = ProcessControlRequest();
			if (n < 0) {
				m_Log->log(LogTrace::ERROR, "Failed to process control request");
				break;
			}
		}

		// handle xine connection
		if (FD_ISSET(xinefd, &fds)) {
			m_Output = m_xineListener.Accept();
		}

        // handle data packets
		if (m_pause != 0)
			continue;

		if (m_Data == NULL) {
			if (FD_ISSET(m_dataListener.GetSocketID(), &fds))
				m_Data = m_dataListener.Accept();
		}
		else {
			if (FD_ISSET(m_Data->GetSocketID(), &fds)) {
                		n = ProcessDataPacketRequest();
				if (m_multicastIP != 0) {
					// ... 
				}
				else if (n == 0 && ((m_bid+1) % VIDEOJET_MIN_HEART_BEAT == 0)) 
				SendOneStatePacket(VIDEOJET_STATE_ALIVE);	
			}
		}
	}
}

//  Start Xine UI application

void* callXine(void* p)
{
	char cmdline[200];
	MdPlay* mp = (MdPlay*)p;
	system(mp->getMapping());	// make sure mapping done 10-09-06
	mp->m_Log->log(LogTrace::INFO, mp->getMapping());
	sprintf(cmdline,"xine %s -V %s -f",  mp->getMRL(), 
		mp->m_useACCEL? "xxmc":"auto");
	mp->m_Log->log(LogTrace::INFO, "Start Xine applicatione: %s", cmdline);
	system(cmdline);
	pthread_exit(0);
}

// Start Xine as thread to handle TCP mrl 

void* startMyXine(void* p)
{
	MdPlay* mp = (MdPlay*)p;
	char vdrv[5];
	strcpy(vdrv, mp->m_useACCEL? "xxmc":"auto");
	mp->m_Log->log(LogTrace::INFO, "Start Xine player %s", mp->getMRL()); 
	myXineSetLog(mp->m_Log);
        if (myXineInit() < 0) {
              mp->m_Log->log(LogTrace::ERROR, "Xine initialization fails\n");
        }
	else if (myXineWindow(mp->m_rect[0], mp->m_rect[1], mp->m_rect[2], mp->m_rect[3], mp->getParent()) < 0)
		mp->m_Log->log(LogTrace::ERROR, "Fail on Xine Window %d %d %d %d",
			mp->m_rect[0], mp->m_rect[1], mp->m_rect[2], mp->m_rect[3]);
	else if (myXineDriver(vdrv, "auto") < 0)
		mp->m_Log->log(LogTrace::ERROR, "Fail on Xine Driver %s %s",
			vdrv, "auto");
	else if (myXinePlay(mp->getMRL()) < 0)
		mp->m_Log->log(LogTrace::ERROR, "Fail on Xine Play");

	myXineCleanUp();
        myXineExit();

	pthread_exit(0);
}

int MdPlay::ProcessStartStreamingRequest()
{
	short cmd, version, mode;
	unsigned short block_size, heart_beat;
	int n, len;

    m_Log->log(LogTrace::INFO, "Process Start stream request");

	len = 6;
	n = m_Command->ReadExact(m_buffer, &len);
	if (n < 0) 
    	return -1;

	cmd = (short) m_buffer[0];
	version = (short) ((m_buffer[1] & 0xf0) >> 4);
        mode = (short) (m_buffer[1] & 0x0f) ;

  // validation
    if (cmd != VIDEOJET_START_STREAMING) return -1; // not the right cmd
    if (version == VIDEOJET_PROTOCOL_VERSION_WIJET2) {
		len = *((int *)&m_buffer[2]);
		if (len < 0 || len > 200)  return -1;
		n = m_Command->ReadExact(m_buffer, &len);
		if (n < 0) 
			return -1;
		mapDriver(m_buffer);		
        m_Log->log(LogTrace::INFO, "Xine MRL is %s", m_mrl);

		return 1; // the WiJET2 version
	}
  // validation
	if (version != VIDEOJET_PROTOCOL_VERSION) return -1; // not the right version
	if (mode != VIDEOJET_SYNCH_MODE && mode != VIDEOJET_ASYNCH_MODE) 
		return -1; // not the right synchronization mode
  
        block_size = (unsigned short) ((m_buffer[2] << 8) + m_buffer[3]);
        heart_beat = (unsigned short) ((m_buffer[4] << 8) + m_buffer[5]);

	if (heart_beat > VIDEOJET_MAX_HEART_BEAT ||heart_beat < VIDEOJET_MIN_HEART_BEAT) 
		return -1;
	return 0;
}

int MdPlay::ProcessSetPlayModeRequest()
{
	int length = 1;
	int n; // function return code
	char buffer[10];
	char *buf = &buffer[0];

    m_Log->log(LogTrace::INFO, "Set Play mode");

	n = m_Command->ReadExact(buf, &length);
	if (n <= 0) return -1;

	switch(buf[0]) {
	case M1S: case M2T: case M2P: case M4T:
	case MPEG2_VIDEO: case MPEG4_VIDEO: case VOB:
		length = 2;
		n = m_Command->ReadExact(buf, &length);
		if (n <= 0) return -2;			// X
		m_rect[0] = ntohs(*(short*)&buf[0]);
		length = 2;
 		n = m_Command->ReadExact(buf, &length);
		if (n <= 0) return -3;			// Y
		m_rect[1] = ntohs(*(short*)&buf[0]);
		length = 2;
		n = m_Command->ReadExact(buf, &length);
		if (n <= 0) return -4;			// W
		m_rect[2] = ntohs(*(short*)&buf[0]); 
		break;

	case AC3: case PCM: case RPCM: case MPEG_AUDIO:
		length = 2;
		n = m_Command->ReadExact(buf, &length);
		if (n <= 0) return -1;			// SampleRate
		length = 2;
		n = m_Command->ReadExact(buf, &length);
		if (n <= 0) return -1;			// Channel
		length = 2;
		n = m_Command->ReadExact(buf, &length);
		if (n <= 0) return -1;			// BitsPerSample
		break;

	default:
		return -1;
	}
	length = 2; // f5
	n = m_Command->ReadExact(buf, &length);
	if (n <= 0) return -5;				// H
	m_rect[3] =  ntohs(*(short*)&buf[0]);
	length = 1; // f6: bit rate
	n = m_Command->ReadExact(buf, &length);
	if (n <= 0) return -6;				// Bit Rate
	length = 1; // f7: speed_factor
	n = m_Command->ReadExact(buf, &length);
	if (n <= 0) return -7;				// Speed Factor
	length = 1; // f8: forward_or_backward
	n = m_Command->ReadExact(buf, &length);
	if (n <= 0) return -8;				// Fw or Bw
	length = 1; // f9: faster_or_slower
	n = m_Command->ReadExact(buf, &length);
	if (n <= 0) return -9;				// Fast or Slow
	length = 6; // reserved field
	n = m_Command->ReadExact(buf, &length);
	if (n <= 0) return -10;				// Reserved

// successfully get the set play mode packet

	RetrieveAndAdjustVideoInfo();
	
	pthread_create( &m_myThread, NULL, startMyXine, (void*)this);
	usleep(20000);
	return 1;	// WiJET retorn 0 WiJET2 return 1
}


int MdPlay::ProcessControlRequest()
{
	int n, len;

	len = 1;
	n = m_Command->ReadExact( m_buffer, &len);
	if (n <= 0) {
        m_Log->log(LogTrace::ERROR, "Failure on read %d", n);
		return -1;
	}
//    m_Log->log(LogTrace::INFO, "Process Control (Type %d)", m_buffer[0]);
	switch( m_buffer[0] ) {
		case VIDEOJET_MULTICASTIP:
			return ProcessSetMulticastIP();
		break;
		case VIDEOJET_SET_PLAY_MODE:
			return ProcessSetPlayModeRequest();
		break;
		case VIDEOJET_START_PLAYING:
			m_Log->log(LogTrace::INFO, "Get start playing request");
			// prepare 
		break;
		case VIDEOJET_PAUSE_PLAYING:
			SendRequest("speed_0");
            m_Log->log(LogTrace::INFO, "Get pause playing request");
			m_pause = 1;
			// send pause to Xine
		break;
		case VIDEOJET_RESUME_PLAYING:
			SendRequest("speed_4");
            m_Log->log(LogTrace::INFO, "Get resume playing request");
			m_pause = 0;
			// send resume to Xine
		break;
		case VIDEOJET_CLEAR_BUFFER:
            m_Log->log(LogTrace::INFO, "Get clear buffer request");
			// terminate thread
		break;
		case VIDEOJET_STOP_PLAYING:
			SendRequest("exit");
			stop();
            m_Log->log(LogTrace::INFO, "Get stop playing request");
			// stop play
		break;
		case VIDEOJET_QUERY_STATE:
			if (m_pause == 0) {	// handle the end of video
				if (m_Output != NULL) {
					m_Output->Close();
					delete m_Output;
					m_Output = NULL;
				}
			}
			if (SendRequest("query") <= 0) m_eos = true;;
            m_Log->log(LogTrace::INFO, "Get query state request");
//		switch(1) {
//			case VIDEOJET_STATE_ALIVE:
			if (m_eos)
				SendOneStatePacket(VIDEOJET_STATE_EOS);
			else
                SendOneStatePacket(VIDEOJET_STATE_ALIVE);
//			break;
//			case VIDEOJET_STATE_READY:
//				SendOneStatePacket(VIDEOJET_STATE_READY);
//			break;
//			case VIDEOJET_STATE_DECLINE:
//				SendOneStatePacket(VIDEOJET_STATE_READY);
//			break;
//			default:
//			break;
//		}
		break;
		case VIDEOJET_START_MYXINE:
			m_Log->log(LogTrace::INFO, "Get start MYXine  request");
			RetrieveAndAdjustVideoInfo();
			pthread_create( &m_myThread, NULL, startMyXine, (void*)this);
		break;
		case VIDEOJET_START_XINE:
			m_Log->log(LogTrace::INFO, "Get Start Xine  request");
			pthread_create( &m_myThread, NULL, callXine, (void*)this);

		break;
		case VIDEOJET_QUERY_START:
                        m_Log->log(LogTrace::INFO, "Get Query Start request");
			if (SendRequest("query") <= 0)
				SendOneStatePacket(VIDEOJET_STATE_DECLINE);
			else 
				SendOneStatePacket(VIDEOJET_STATE_READY);
		break;
		case VIDEOJET_QUERY_ACCEL:
                         m_Log->log(LogTrace::INFO, "Get Query Accelerator %x", m_useACCEL);
                        if (m_useACCEL == false)	// return 0 or !0
                                SendOneStatePacket(VIDEOJET_STATE_ALIVE);
                        else
                                SendOneStatePacket(VIDEOJET_STATE_EOS);
		break;
		default:
			m_Log->log(LogTrace::INFO, "Get unknown  request");

		break;
	}
	return 0;
}

int MdPlay::ProcessSetMulticastIP()
{
	m_Log->log(LogTrace::INFO, "Process Set Multicast IP");
	int n,  len;
	len  = sizeof(int);
  	n = m_Command->ReadExact((char*)&m_multicastIP, &len);
	m_Log->log(LogTrace::INFO, "Seccess [%d %d] to set multicast IP %x ", n, len, m_multicastIP);
	if (m_multicastIP != 0) {
		MultiCastUDP* mcUDP = new MultiCastUDP();
		if (mcUDP->Open(MULTICAST_UDP_PORT))
			if (mcUDP->Join(m_multicastIP))
				m_Data = mcUDP;
		if (m_Data == NULL) {
			m_Log->log(LogTrace::ERROR, "Set Multicast fails");
			n = -1;
		}
		else
		    m_Data->SetNonBlocking();
	}
  	return n;
}

int MdPlay::ProcessDataPacketRequest()
{
    int data_len = -1;
    int n, len;

//        m_Log->log(LogTrace::INFO, "Process Data Packet");

    if (m_multicastIP == 0) {		// TCP protocol
    len = 1;
    n = m_Data->ReadExact( m_buffer, &len);
	if (n < 0 || m_buffer[0] != VIDEOJET_DATA_PACKET)
		return -1;

	len = 4;
	n = m_Data->ReadExact((char *)&m_bid, &len);
	if (n <= 0) return -1;
	m_bid = ntohl(m_bid);

	len = 2;
	n = m_Data->ReadExact((char *) &data_len, &len);
	if (n <= 0) return -1;

	data_len = ntohs(data_len);
	if (data_len != 0 ) {
		n = m_Data->ReadExact( m_buffer, &data_len);
		if ( n <= 0) return -1;
		output(data_len);
	}
    }
    else {
	#define	BUFFER_SIZE 8200		// 8192 + 6
	
	n = m_Data->Recv(m_buffer, BUFFER_SIZE);
	if (n <= 0) return -1;
	if (n == 1) {
		if (m_buffer[0] != VIDEOJET_DATA_PACKET) return -1;
		return 1;
	}
	memcpy( &m_bid, &m_buffer[0], sizeof(long));
	m_bid = ntohl(m_bid);
	memcpy( &data_len, &m_buffer[4], sizeof(short));
	data_len = ntohs(data_len);
	if (data_len > 0 && data_len < BUFFER_SIZE) {
		memmove(&m_buffer[0], &m_buffer[6], data_len);
		output(data_len);
		if (m_bid - m_mybid > 1)	// found losting  
			SendOneStatePacket(VIDEOJET_STATE_MYBID);		
		m_mybid = 1 + m_bid;
	}
	else
		m_Log->log(LogTrace::ERROR, "Data length wrong (%d)!", data_len);  
    }
    return 0;
}

void MdPlay::output(int len)
{
	if (m_myThread == 0) {
        	m_Log->log(LogTrace::DEBUG, "The playing thread hasn't start");	
		// IN TEST ONLY ?
	    	RetrieveAndAdjustVideoInfo();
		pthread_create( &m_myThread, NULL, startMyXine, (void*)this);
	}
	if (m_Output) {
		if (m_size > 0) {
			int l = len;
			for(int i=0; i<m_size; i++) {
				m_Output->WriteExact(m_buffers[i], &l);
				free(m_buffers[i]);
				usleep(1+m_size);
				m_Log->log(LogTrace::DEBUG,"Otput Sleep %d",i); 
			}
			m_size = 0; 
		}
		m_Output->WriteExact(m_buffer, &len);
	}
	else if (len > 0) {
		m_Log->log(LogTrace::INFO, "Xine Socket is not ready!!");
		usleep(10000);
		if (m_size < MAXBUFSIZE-1) {
			m_buffers[m_size] = (char*)malloc(len);
			memcpy(m_buffers[m_size], m_buffer, len);
			m_size++;
		}
	}
	if ((m_bid % 200) == 0)
	m_Log->log(LogTrace::DEBUG, "Output Data Packet %d  [%d]", len, m_bid);
}

int MdPlay:: SendOneStatePacket(int type)
{
  long bid_network; // block id in network order
  int len = 1;

  switch(type) {
  case VIDEOJET_STATE_ALIVE:
  case VIDEOJET_STATE_EOS:
    m_buffer[0] = (char)type;  
    break;

  case VIDEOJET_STATE_READY:
  case VIDEOJET_STATE_DECLINE:

    len = 5;
    m_buffer[0] = type;
    bid_network = htonl(m_bid);
    m_buffer[1] = ((char *) &bid_network)[0];
    m_buffer[2] = ((char *) &bid_network)[1];
    m_buffer[3] = ((char *) &bid_network)[2];
    m_buffer[4] = ((char *) &bid_network)[3];
    break;

  case VIDEOJET_STATE_MYBID:
    m_buffer[0] = m_bid - m_mybid;
    break;

  default: // unknown type
    m_Log->log(LogTrace::ERROR,"Unsupported packet type %x", type);
    return -1;
  }
  if ( m_Command->WriteExact(m_buffer, &len) <=0) 
	m_Log->log(LogTrace::ERROR, "Write Fails in Send State Packet");
  else 
	m_Log->log(LogTrace::INFO, "Send State Packet %x", type);
  return 0;
}

int MdPlay::SendRequest(char *cmd)
{
	int n = 0;
	Sender mySender(XINE_SVR_PORT);
        n = mySender.Send(cmd, strlen(cmd)+1);
        if (n > 0 && strstr(cmd, "query")) {
        	char buffer[20];
                n = mySender.Echo(buffer, 20);
		if (n > 0 && strstr(buffer, "2")) n = 1;
		else n = 0;
	}
	m_Log->log(LogTrace::INFO,"SendRequest %s return %d", cmd, n);
	return n;
}


void MdPlay::RetrieveAndAdjustVideoInfo()
{
	int wx, wy;
        Sender mySender(WIJET2_PORT);
        m_parent = 0;
        if (mySender.Send("DeskTop", 8) > 0) {
        	char buffer[40];
                if (mySender.Echo(buffer, 40) > 0) {
                	sscanf(buffer, "%u_%d_%d", &m_parent, &wx, &wy);
			m_Log->log(LogTrace::INFO,"Win: %u X: %d Y: %d",
                                m_parent, wx, wy);
                

		if (m_rect[2] > 10 || m_rect[3] > 10) {
		int Ratio1, Ratio2, Ratio3;
        	int x, y;
        	x = y = 0;
       		GetDelta(&x, &y);
        	m_Log->log(LogTrace::INFO, "GetDelta X %d Y %d", x, y);

		if (wx == 1024 && wy == 768) {
            		Ratio1 = 64;   Ratio2 = 45;  Ratio3 = 40;
        	} else if (wx == 800 && wy == 600) {
            		Ratio1 = 10;   Ratio2 = 9;   Ratio3 = 8;
        	} else {
            		Ratio1 = 8;    Ratio2 = 9;   Ratio3 = 1;
        	}
		for (int i=0; i<4; i++)
			m_rect[i] = (m_rect[i]*Ratio1)/Ratio2;
//		m_rect[0] += x;
//		m_rect[1] += y;
		if (m_rect[2] > wx-m_rect[0]) m_rect[2] = wx-m_rect[0];
                if (m_rect[3] > wy-m_rect[1]) m_rect[3] = wy-m_rect[1];

        	m_Log->log(LogTrace::INFO, "Window  X %d Y %d W: %d H %d",
			m_rect[0], m_rect[1], m_rect[2], m_rect[3]);
		}
	}	// Echo  
	}	// Send
}

 
#define WIJET_PORT (30300)

void MdPlay::GetDelta(int *x, int *y)
{
        char buf[40];
        Sender mySender(WIJET_PORT);
	MsgHeader myMsgHeader(COMANDSETCONTROL2WiJET);
	myMsgHeader.setCommand(MdPlayer2WiJETGetDisplayDelta, INADDR_LOOPBACK);
	memcpy(buf, myMsgHeader.getMsgHeader(), myMsgHeader.getMsgHeaderLength());
        *(short*)&buf[myMsgHeader.getMsgHeaderLength()] = 0;
	if (mySender.Send(&buf[0], myMsgHeader.getMsgBodyStart()) > 0) {
		if(mySender.Echo(&buf[0],40) > 0) {
                        if (strncmp(buf, "Sorry", 5) != 0) {
                        *x = *(int*)&buf[12];
                        *y = *(int*)&buf[16];
                        }
                }
        }
}

void MdPlay::mapDriver(char *cmd)
{
        enum {  CMD_TYPE,
                CMD_SHAREDPT,
                CMD_PASSWORD,
                CMD_IP,
                CMD_FL,
                CMD_END
        };
        static char* mappedDriver[] = {
                "dvd",	"/mnt1",
                "vcd",	"/mnt1",
                "file",	"/mnt1",
                NULL
        };

        int pos[6];
        int i, j = 0;
        int driver;

        // Map ,type,sharedPt,password,Ip,file
        for (i = 3, j = 0; cmd[i] && j<6; i++)
                if (cmd[i] == ',') {
                        pos[j++] = i+1;
                        cmd[i] = 0;
                }
        if (j == CMD_END) {
                driver = (cmd[pos[CMD_TYPE]] - '0');
                if (driver < 0 || driver > 2) driver = 2;

#ifndef	USINGINNERMOUNT
		sprintf(m_mapping, "mt //%s/%s %s",  
			&cmd[pos[CMD_IP]], &cmd[pos[CMD_SHAREDPT]],
			mappedDriver[driver*2 + 1]);
		system(m_mapping);
#else

                sprintf(m_mapping, "smbmount //%s/%s %s -o password=%s",
                        &cmd[pos[CMD_IP]], &cmd[pos[CMD_SHAREDPT]],
                        mappedDriver[driver*2 + 1], &cmd[pos[CMD_PASSWORD]]);
                system(m_mapping);
#endif
	m_Log->log(LogTrace::INFO, m_mapping);
        m_Log->log(LogTrace::DEBUG, "T: %s SH: %s PW: %s IP: %s FL: %s",
              &cmd[pos[CMD_TYPE]], &cmd[pos[CMD_SHAREDPT]],
              &cmd[pos[CMD_PASSWORD]], &cmd[pos[CMD_IP]], &cmd[pos[CMD_FL]]);
	      if (driver == 1)
		sprintf(m_mrl, "/%s/MPEGAV",  mappedDriver[driver*2 + 1]);
	      else
              	sprintf(m_mrl, "\"%s:/%s/%s\"",
                  mappedDriver[driver*2], mappedDriver[driver*2 + 1], &cmd[pos[CMD_FL]]);  
			  
        }
        else m_Log->log(LogTrace::ERROR, "Wrong Mapping string");
}

///-----------------------------///
//	VideoPlayer class
///-----------------------------///

VideoPlayer::VideoPlayer()
{
        m_Log = new LogTrace("VideoPlayer");
	setLog();
        m_continue = 1;
}
VideoPlayer::~VideoPlayer()
{
        stop();
}

/**
 *      setLog method
 */
void VideoPlayer::setLog()
{
        char* f = NULL;
        getProperty gP(gP.P_OTC);
        int i = LogTrace::ERROR;
        if (gP.getInt("MODULES") & 4) {         // ST 1: AI 2: MD 4:  WD 8
                i = gP.getInt("DEBUG");
                if (i != 0) f = "/www/log/mdplay";
        }
        m_Log->setLog(i, f);
}

int VideoPlayer::start(int n)
{
    if (!m_cmdListener.OpenServer(VIDEOJET_CMD_PORT)) { 
            m_Log->log(LogTrace::ERROR, "Command Socket Open fails");
            return -2;
    }

    while (m_continue) {
        struct sockaddr_in addr;
        SocketTCP* cmd = m_cmdListener.Accept(&addr);
        m_Log->log(LogTrace::INFO, "New Client Addr %u Port %d \n", *(UINT*)(&addr.sin_addr), addr.sin_port);

	    struct sched_param pprio;
        int pid = fork();
        if (pid < 0) {        // failed to fork
            m_Log->log(LogTrace::ERROR, "Failed to fork");
            break;
        } else if (pid > 0) { // parent process
            // cmd->Close();
            sched_yield();
            waitpid(pid, NULL, 0);
        } else {              // child process
            // change its scheduling priority to the highest possible
            pprio.sched_priority = -20;
            sched_setscheduler(0, SCHED_OTHER, &pprio);
			// m_cmdListener.Close();
            m_Log->log(LogTrace::INFO, "Get a new cmd connection");
        	MdPlay* md = new MdPlay(m_Log);
			if (n > 1) md->noACCEL();
			if (md->init(cmd) == 0) 
				md->run();
            m_Log->log(LogTrace::INFO, "Exit !!");
			delete md;
			cmd->Close();
			exit(0);
		}
	}
	return 0;
}

VideoPlayer* vp;

void handler(int sig)
{
        switch(sig) {
        case SIGTERM:
        case SIGABRT:
                vp->stop();
        break;
        default:
                vp->process(sig);
        break;
        }
}

#ifdef VIDEO_TESTING

int MdPlay::test()
{
    fd_set fds;
    int n, maxfd, xinefd;
    struct timeval timeout;

    m_Log->log(LogTrace::INFO, "Media Player running...");
    maxfd = xinefd = m_xineListener.GetSocketID();

	strcpy(m_mrl, "/mnt1/Clip05.mpg"); 
    while (m_continue) {
            timeout.tv_sec = VIDEOJET_TIMEOUT;
            timeout.tv_usec = 0;
            FD_ZERO(&fds);
            FD_SET(xinefd, &fds);
            maxfd = xinefd;
            n = select(maxfd + 1, &fds, NULL, NULL, &timeout);
            if (n <= 0) {
                    m_Log->log(LogTrace::ERROR, "Failed to get a cmd or data packet");
		    output(0);
                    if (SendRequest("query") <= 0) m_continue = 0;
                    sleep(10); 
            }

    // handle xine connection
            if (FD_ISSET(xinefd, &fds)) {
                    m_Output = m_xineListener.Accept();
            }

	}
	return 0;
}

int VideoPlayer::test()
{
        SocketTCP *cmd = NULL;
        MdPlay* md = new MdPlay();
		if (md->init(cmd) == 0) 
			md->test();
        m_Log->log(LogTrace::INFO, "Exit !!");
        delete md;
     	return 0;
}

int main(int n, char*a[])
{
	vp = new VideoPlayer();
	for (int i=1; i<=SIGTERM; i++)
		signal(i, handler);
	if (n > 1) vp->test();
	else vp->start(n);
	return 0;
}
#else

int main(int n, char*a[])
{
	vp = new VideoPlayer();
	for (int i=1; i<=SIGTERM; i++)
		signal(i, handler);
        vp->start(n);
        return 0;
}

#endif
