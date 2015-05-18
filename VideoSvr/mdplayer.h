
#if !defined(_MULTIMEDIA_VIDEO_PLAYER_DEFINED)
#define _MULTIMEDIA_VIDEO_PLAYER_DEFINED


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../LogTrace/LogTrace.h"
#include "../myNetLib/myNetWK.h"
#include "../myNetLib/Sender.h"
#include "../XwinObj/XwinObject.h"
#include "../myXine/myXine.h"
#include "../myNetLib/cmdMsgs.h"
#include "../LoadFile/getProperty.h"

/*
 * port numbers
 * --------------------------------------------
 * Change the default port numbers to what you 
 * intend to use. Keep in mind that avoiding
 * using port numbers less than 1024, which
 * are reserved although some systems allow
 * you to use them.
 *
 */
#define VIDEOJET_CMD_PORT               9800
#define VIDEOJET_DATA_PORT              9801
#define VIDEOJET_XINE_PORT		10001

#define MULTICAST_UDP_PORT		5600

/* 
 * connection settings
 * -----------------------------------------------
 * timeout and number of retries before giving up
 * MAX_WAIT_TIME was defined in Socket.h and
 * measured in ms; VIDEOJET_TIMEOUT is measured 
 * in seconds.
 */
#define MAX_WAIT_TIME			8000
#define VIDEOJET_TIMEOUT                (MAX_WAIT_TIME/1000)
#define VIDEOJET_TIMEOUT_RETRY          3

#define VIDEOJET_MIN_HEART_BEAT         40
#define VIDEOJET_HEART_BEAT_BUF         10
#define VIDEOJET_MAX_HEART_BEAT         800


#define VIDEOJET_PROTOCOL_VERSION       0x00
#define VIDEOJET_DRIVER_MEMORY          (32*1024*1024)
#define VIDEOJET_DRIVER_NUM_BUFFS       960

// shared buffers, not for the internal buffers
// for internal buffers, let us fix it to 40 
// first and will adjust it according to tests
#define VIDEOJET_SHARED_BUFFERS         10
#define VIDEOJET_AUDIO_TIMEOUT          400000
#define VIDEOJET_GETBUF_RETRY           1000
#define VIDEOJET_INIT_CONN              100000

#define VIDEOJET_SYNCH_MODE	        0x00
#define VIDEOJET_ASYNCH_MODE            0x01

/*
 * Internal states
 * -----------------------------------------
 * internal states of the VIDEOJET.
 */
#define VIDEOJET_STATE_UNINITIALIZED    0xff
#define VIDEOJET_STATE_ALIVE            0x00
#define VIDEOJET_STATE_READY            0x01
#define VIDEOJET_STATE_DECLINE          0x02
#define VIDEOJET_STATE_EOS              0x03
#define VIDEOJET_STATE_MYBID            0x04

/*
 * Command packet types
 *
 */
#define VIDEOJET_START_STREAMING 0x00
#define VIDEOJET_SET_PLAY_MODE   0x01
#define VIDEOJET_START_PLAYING   0x02
#define VIDEOJET_STOP_PLAYING    0x03
#define VIDEOJET_FAST_REWIND     0x04
#define VIDEOJET_FAST_FORWARD    0x05
#define VIDEOJET_QUERY_STATE     0x06
#define VIDEOJET_PAUSE_PLAYING   0x07
#define VIDEOJET_RESUME_PLAYING  0x08
#define VIDEOJET_CLEAR_BUFFER    0x09
#define VIDEOJET_DATA_PACKET     0x10
#define VIDEOJET_MULTICASTIP	 0x11

#define VIDEOJET_START_MYXINE	 0x20
#define VIDEOJET_START_XINE	 0x21
#define VIDEOJET_QUERY_START	 0x22
#define VIDEOJET_QUERY_ACCEL     0x23

enum {
  M1S = 0, M2T, M2P, M4T, MPEG2_VIDEO, MPEG4_VIDEO, VOB,
  AC3, PCM, RPCM, MPEG_AUDIO,
  MP3, AVI, DIVX,
  MWA, MWV
};

#define VIDEOJET_PROTOCOL_VERSION_WIJET2       0x02
#define XINEMRL         "tcp://127.0.0.1:10001"

#define MAXBUFSIZE	40

class MdPlay
{
public:
	MdPlay(LogTrace* log=NULL);
	~MdPlay();
	int init(SocketTCP* cmd);
	void run();
        inline void stop() { m_continue= 0; }
	void start();
	inline Window getParent() { return (Window)m_parent; }
	inline char* getMRL() { return m_mrl; }
	inline void noACCEL() { m_useACCEL = false; }
	inline char* getMapping() { return m_mapping; }
	int test();

protected:
	int ProcessStartStreamingRequest();	
	int ProcessSetPlayModeRequest();
	int ProcessSetMulticastIP();

	int ProcessControlRequest();
	int ProcessDataPacketRequest();

private:
	void output(int len);
	int SendOneStatePacket(int type);
	int SendRequest(char *cmd);
        void RetrieveAndAdjustVideoInfo();
	void GetDelta(int *x, int *y);
	void mapDriver(char *cmd);

public:
	LogTrace*       m_Log;
	int m_rect[4];
        bool            m_useACCEL;

private:
        SocketTCP       m_dataListener;
	SocketTCP	m_xineListener;
	SocketBase*	m_Data;
//      SocketTCP*      m_Data;
        SocketTCP*      m_Command;
	SocketTCP*	m_Output;
	int		m_pause;
        int             m_continue;
	long		m_bid;
	long		m_mybid;
	char*		m_buffer;
	unsigned int	m_multicastIP;
	bool 		m_eos;
	XwinObject      m_Win;			// Compile Holder??
	pthread_t 	m_myThread;
	int		m_size;
	char*		m_buffers[MAXBUFSIZE];
	unsigned int	m_parent;
	char		m_mrl[120];
	char		m_mapping[160];
};

class VideoPlayer
{
public:
	VideoPlayer();
	~VideoPlayer();
	int start(int n);
	int run();
	inline void stop() { m_continue= 0; };
	inline void process(int n) { if (n == SIGTRAP) setLog();
                if (n == SIGUSR1) stopLog(); }
	int test();

private:
	void setLog();
        inline void stopLog() { if (m_Log) m_Log->stopLog(); };
        SocketTCP 	m_cmdListener;
	int		m_continue;
	LogTrace*	m_Log;

};

#endif // !defined(_MULTIMEDIA_VIDEO_PLAYER_DEFINED)

