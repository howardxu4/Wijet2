/******************************************************************
*
* This software code is the property of OTC Wireless, Inc.
* This intellectual property is proprietary and confidential, and 
*	protected by US and International Copyright Laws.
* In whole and in part it may not be copied, published, or
*	otherwise disclosed or distributed in any form or
*	by any means, including but not limited to translation
*	and reengineering, without explicit prior written permission
*	from Pegasus Wireless.
*
* Copyright 2006 Pegasus Wireless, Corp.  All rights reserved.
*
*/ /***************************************************************/

/******************************************************************
*
* This software may contain materials from other sources.
* They are all properly acquired with permission or from open
*	distribution.
* Wherever required, proper disclosure and inclusion statements 
*	have been posted.
*
*/ /***************************************************************/
////////////////////////////
// The Message Header protocol definition class

#include "cmdMsgs.h"

MsgHeader::MsgHeader(UCHAR cmdSet, UCHAR version, UCHAR cmd_ver)
{
	m_msgHeader[VERSION]=version;
	m_msgHeader[CMD_SET]=cmdSet;
	m_msgHeader[CMD_VER]=cmd_ver;
	m_msgHeader[RESERVED]=RSV_FILLER;
	m_msgHeader[RESERVED+1]=RSV_FILLER;
}

MsgHeader::MsgHeader(char *msg)
{
	memcpy(&m_msgHeader[0], msg, MSG_LEN);
}

MsgHeader::~MsgHeader() {}

void MsgHeader::setSignature(ULONG addr)
{
	*(ULONG*)&m_msgHeader[SIGNATURE]=addr;
}

void MsgHeader::setCommand(UCHAR cmd)
{
	m_msgHeader[COMMAND]=cmd;
}

void MsgHeader::setCommand(UCHAR cmd, ULONG addr)
{
	m_msgHeader[COMMAND]=cmd;
	*(ULONG*)&m_msgHeader[SIGNATURE]=addr;
}

bool MsgHeader::checkMsgHeader()
{
	bool rtn = false;
	if (m_msgHeader[VERSION] != MSG_VERSION) return rtn;
	if (m_msgHeader[CMD_VER] != CMD_VERSION) return rtn;
	if (m_msgHeader[RESERVED] != RSV_FILLER) return rtn;
	if (m_msgHeader[RESERVED+1] != RSV_FILLER) return rtn;
	switch(m_msgHeader[CMD_SET]) {
	case COMANDSETDISCOVERY:
		break;
	case COMANDSETCONNECTION:
		break;
	case COMANDSETPSM2PSM:
		switch(m_msgHeader[COMMAND]) {
		case PSM2PSMControl:			// To PSM
		case PSM2PSMGetUnitSelection:
		case PSM2PSMEstablish:
		case PSM2PSMGetConnectedUnits:
		case PSM2PSMDisconnect:
		case PSM2PSMExit:
		case PSM2PSMExitDaemon:
		case PSM2PSMLaunchPPT:
		case PSM2PSMPlayVideo:
		case PSM2PSMOpenFile:
		case PSM2PSMRunScript:
		case PSM2PSMInforMsg:
		case PSM2PSMRegister:			// To Moderator
		case PSM2PSMUnRegister:
		case PSM2PSMQueryStat:			// To Both 
		case WiJET2PSMCommandInfo:		// Kill vncviewer 060706
			rtn = true;
			break;
		default:
			break;
		}
		break;
	case COMANDSETCONTROL2WiJET:
		switch(m_msgHeader[COMMAND]) {
		case CONTROLLER2WiJETTakeControl:
		case CONTROLLER2WiJETGrantAccess:
			rtn = true;
			break;
		case PSM2WiJETSetDisplayDelta:
		case MdPlayer2WiJETGetDisplayDelta:	// internal 
		case MdPlayer2PSMStartPlayingMode:
		case MdPlayer2PSMStopPlayingMode:
		case PSM2WiJETQueryDiscoveryInfo:
		case PSM2WiJETSetReverse:		// Server
		case PSM2WiJETSetPass:			// Pass
		case MAC2WiJETuMount:			// uMount
			rtn = true;
			break;
		default:
			break;
		}
		break; 
	default:
		break;
	}
	return rtn;
}

char* MsgHeader::getMsgHeader()
{
	return (char*)&m_msgHeader[0];
}
