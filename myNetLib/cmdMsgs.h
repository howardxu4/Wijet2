
////////////////////////////
// The Message Header protocol definition class

#if (!defined(_BASE_MESSAGE_DEFINED))
#define _BASE_MESSAGE_DEFINED

#include "myNetWK.h"

#define MSG_VERSION 0x11
#define CMD_VERSION 0x11
#define RSV_FILLER	0xFF

//      0   1   2   3   4   5        6         7       8    9     10   11   12
//
// | VERSION | SIGNATURE | CMDSET | SETVER | COMMAND | RESERVED | LENGTH | BODY |
//		1		4			1		1			1		2			2		*
//		0x11	htonl(IP)	0x33	0x11		002		0xFFFF		n		...
//

class MsgHeader
{
public:
	MsgHeader(UCHAR cmdSet, UCHAR version=MSG_VERSION, UCHAR cmd_ver=CMD_VERSION);
	MsgHeader(char *msg);
	~MsgHeader();
	void setSignature(ULONG addr);
	ULONG getSignature() { return *(ULONG*)&m_msgHeader[SIGNATURE]; };
	void setCommand(UCHAR cmd);
	bool checkMsgHeader();
	void setCommand(UCHAR cmd, ULONG addr);
	char* getMsgHeader();
	UCHAR getCommand() { return m_msgHeader[COMMAND]; };

static 	int  getMsgHeaderLength() {return MSG_LEN;};
static 	int  getMsgBodyStart() { return MSG_BODY; };

private:
	enum {	VERSION=0, 
			SIGNATURE=1, 
			CMD_SET=5, 
			CMD_VER=6, 
			COMMAND=7, 
			RESERVED=8, 
			MSG_LEN=10, 
			MSG_BODY=12
	};
	UCHAR m_msgHeader[MSG_LEN];
};



#define byte	char
#define lpstr	char*


//-------------------------------------------------
// Master Protocol Header: for all OTC WiJET communications
//-------------------------------------------------

// msgVersion: a simple value from 1 and up, to allow validation and
//	future change.
//	All message receivers must verify and follow version.
//	It consists of two nibles of same numerical version value.
//	The msgValue is initially 0x11 and going up as 0x22, 0x33, etc. 
//	This value does not go up quickly.
//	Each actual "Command Set" will have its own protocol version.
// msgSignature: it further contributes to message authentication.
//	It shall be four bytes and carries the sender's IP address -
//	in reverse.  For a node without valid IP address, repeat
//	the msgVersion value.
// msgCommandSetID: a value to identify the command set being carried.
//	Use the 0xnn format for each command set.

typedef struct s_MsgHeader {
	  unsigned byte	msgVersion;		// See instructions on forming version
      unsigned byte msgSignature[4];// See instructions
	  unsigned byte	commandSetID;	// Allowing multiple command sets
	  unsigned byte	msgCommand[1];	// Begin the command
} msgHeader;

typedef struct s_CommandPSM2PSM {
	unsigned byte commandSetID;		// Must be same as in header
	unsigned byte csVersion;		// Version to allow synchronization &
									//	future change.
									//	Value format is like msgVersion.
	unsigned byte msgCommandResponse; 	// Actual command code
								// command code is even & Response = command + 1
								// Usually Ack response has no body, 
								// NACK should provide error code.
	unsigned short reserved;	// Initialize all reserved to 0xFF
	unsigned byte commandResponseBodyLength;// length of just body.
	unsigned byte commandResponseBody[1];	// Begin the body/parameter
} commandHeader ;

// Command set ID's

#define COMANDSETDISCOVERY	(unsigned byte)0x11
#define COMANDSETCONNECTION	(unsigned byte)0x22
#define COMANDSETPSM2PSM	(unsigned byte)0x33
#define COMANDSETCONTROL2WiJET	(unsigned byte)0x44

//-------------------------------------------------
// PSM to PSM Commands: for Remote & Moderator
//	PSM2PSM commands: command codes are even values
//	#define COMANDSETPSM2PSM	(unsigned byte)0x33
//-------------------------------------------------

//	Do not use command code 000 for now. 

#define PSM2PSMControl	(unsigned byte)0x02	// Asks to Control a remote PSM
// This command wakes up the PSM, if in daemon state.
// The remote control function should be implemented as a daemon.
		// For now, a PSM should only accept one remote master.
typedef struct s_PSM2PSMControl {
lpstr	*paramsControl ;	// temporary: a string of repeating "-parameter value"
		// Example parameters: -M for moderator, which forces its way and 
// preempts other master.
} PSM2PSMControlParams ;
//	Example of NACK:
		// A PSM may be started with the [default] option of ignoring remote control.

#define PSM2PSMGetUnitSelection	(unsigned byte)0x04	// Retrieve list of WiJET/s interested by the PSM
		// If PSM is in "auto," then list of discovered WiJET/s
		// If PSM is in "User list," then the current selection.
typedef struct s_PSM2PSMGetUnitSelection {
	lpstr	*paramsGetUnitSelection ;	// List of WiJET/s by their IP addr
} PSM2PSMGetUnitSelectionParams ;

#define PSM2PSMEstablish	(unsigned byte)0x06	// Establish Connection to WiJET/s
// Tell controlled PSM to connect with specified WiJET/s
typedef struct s_PSM2PSMEstablish {
	lpstr	*paramsEstablish ;	// list of WiJET/s by their.IPaddr's
} PSM2PSMEstablishParams ;

#define PSM2PSMGetConnectedUnits	(unsigned byte)0x08
	// Retrieve connected WiJET/s with the PSM
typedef struct s_PSM2PSMGetConnectedUnits {
	lpstr	*paramsGetConnectedUnits ;	// List of connected WiJET/s
} PSM2PSMGetConnectedUnitsParams ;

#define PSM2PSMDisconnect	(unsigned byte)0x10	// Disconnect all WiJET/s
typedef struct s_PSM2PSMDisconnet {
lpstr	*paramsDisconnet ;	// For Nack, give reasons
		// For command, -M is for moderator to force the disconnect
} PSM2PSMDisconnectParams ;

#define PSM2PSMExit	(unsigned byte)012	// Exit the remote PSM - back to daemon state
// This is really a "Sleep": Put PSM into daemon mode, awaiting remote Control command to wake up.
	// Should we use "Sleep" for this and "Exit" for complete exit?.
typedef struct s_PSM2PSMExit {
	lpstr	*paramsExit ;	// Like what?  Remote PSM should Ack/Nack upon exit.
} PSM2PSMExitParams ;

#define PSM2PSMExitDaemon	(unsigned byte)0x20	// Completely Exit & close the remote PSM
typedef struct s_PSM2PSMExitDaemon {
	lpstr	*paramsExit ;	// Like what?  Remote PSM should Ack/Nack upon exit.
} PSM2PSMExitDaemonParams ;


#define PSM2PSMLaunchPPT	(unsigned byte)0x30	// 
typedef struct s_PSM2PSMLaunchPPT {
	lpstr	*paramsLaunchPPT ;	// One PPT file to be launched in full path name
} PSM2PSMLaunchPPTParams ;
		// Example of Nack: file not found or otherwise failed to open

#define PSM2PSMPlayVideo	(unsigned byte)0x40
typedef struct s_PSM2PSMPlayVideo {
lpstr	*paramsPlayVideo ;	// One video file to be launched in full path name
} PSM2PSMPlayVideoParams ;
// Parameter list tells play mode, e.g., Repeat or not
// Each "Play Video" command stops the current video.

// The following is a slippery slope but we don't want to carry many Windows commands.

#define PSM2PSMOpenFile	(unsigned byte) 0x50
// This is for user to change the presentation/application on a PC screen
// Paremeter lists full path of 1 file

// Enter "Control/Tab"
// This allows the user to swap between file/presentation/applications.
// CloseCurrentWindow
// Controlled PSM shall close the current application/file/presentation on the PSM -
//	it must not be the PSM itself.
	

#define PSM2PSMRunScript (unsigned byte) 0x52

#define PSM2PSMInforMsg (unsigned byte) 0x60

// PSM to Moderator

#define PSM2PSMRegister (unsigned byte) 0x70

#define PSM2PSMQueryStat (unsigned byte) 0x80

#define PSM2PSMUnRegister (unsigned byte) 0x90

//-------------------------------------------------
// Moderator / Controller PSM to WiJET Commands
//	Commands for WiJET.Video multicast are included in the set.
//	#define COMANDSETCONTROL2WiJET	(unsigned byte)0x44
//-------------------------------------------------

//	Do not use command code 000 for now. 

#define CONTROLLER2WiJETTakeControl	(unsigned byte)0xA2
	// Moderator will control the access to the WiJET
// This command tells the receiving WiJET that access by a PSM
//	must first be granted by the Moderator.
//	For now, a WiJET can only be granted to one PSM.
//	Note that a PSM may access multiple WiJET.
typedef struct s_CONTROLLER2WiJETTakeControl {
lpstr	*paramsTakeControl ;	// temporary: a string of repeating "-parameter value"
		// Example parameters: -F[orce] to replace previous Moderator, if any.
} CONTROLLER2WiJETTakeControlParams ;

#define CONTROLLER2WiJETGrantAccess	(unsigned byte)0xA4
	// Moderator grants access of WiJET to a PSM
// This command tells the receiving WiJET to only connect with a given PSM.
//	A -F[orce] parameter tells the WiJET to immediately disconnect with
//	current WiJET, if not the one with the grant.
typedef struct s_CONTROLLER2WiJETGrantAccess {
lpstr	*paramsTakeControl ;	// temporary: a string of repeating "-parameter value"
		// Parameters: "-P[SM] IP-Addr" IP address of the granted PSM
// This assumes that each IP address can only run one PSM.
// Set -P to 0.0.0.0 to grant access to any PSM.
// Set -F and this command force off current PSM.
} CONTROLLER2WiJETGrantAccessParams ;

#define PSM2WiJETSetDisplayDelta					(unsigned byte)0xB2
// PSM sets the display delta to WiJET on none 4:3 ratio of resolution
// Command parameters include -X[dx] -Y[dy] 

#define MdPlayer2WiJETGetDisplayDelta				(unsigned byte)0xB4
// MdPlayer get the display delta from WiJET 
// Response parameters include -X[dx] -Y[dy].

#define MdPlayer2PSMStartPlayingMode				(unsigned byte)0xB6
// MdPlayer start the playing mode to PSM 

#define MdPlayer2PSMStopPlayingMode					(unsigned byte)0xB8
// MdPlayer stop the playing mode to PSM 


#define PSM2WiJETQueryDiscoveryInfo					(unsigned byte)0xC2
// PSM gets the Discovery information 
// Response parameters include cmd, type, protctrl, macIpAddr, extlen, dlen, dName.

#define WiJET2PSMCommandInfo                                      (unsigned byte)0xC4
//	to stop vncviewer

#define PSM2WiJETSetReverse                                       (unsigned byte)0xC6
// the PSM set back info and screen size
// Command parameters include wx, wy, wr

#define PSM2WiJETSetPass                                          (unsigned byte)0xC8
// the PSM set pass to display banner page

#define	MAC2WiJETuMount											  (unsigned byte)0xCA
// the Mac umount the mapped disk

// Wijet internal send  Command information

//-------------------------------------------------
// All CYNALYNX modules to set WiJET State Commands
//	the command codes are even values, response codes add one
//	#define COMANDSETWiJETSTATE	(unsigned byte)0x55
//-------------------------------------------------

//	Do not use command code 000 for now. 
//	All the internal command use string directly


#define AUTOIPASKIP	"AskIP"
// AutoIP ask the current connected IP

#define BANNERINVOKE	"Localr"
//  keyborad 'l' typed to switch to local mode
 
#define IDLESTATE	"Banner"
// default WiJET state as no one connected 

#define VNCVIEWER	"WiJET"
// VncViewer informs the PSM connected
// Command parameters include ip 

#define	MDPLAYER	"DeskTop"
// the MdPlayer query the current window id and size

#define VNCDESKTOP	"Window"
// the VncDesktop provides the window id and size
// Command parameters include wd, wx, wy

#define X11VNCSVR	"Remote"
// the Vnc11 server informs the VncClient is connected

#define IRCOMMAND	"IrCmd"
// the IR mode key switch to local or turn on/off soft keyboard 

#endif	// _BASE_MESSAGE_DEFINED
