#include "AutoIpProcess.h"


int AutoIpProcess::CheckChanel()
{
      //+++++++++++++++++++++++++++++++++++++++++++++++++09292004
      //
      // To prevent WiJET communicate with PSM when it is in
      // North America regulatory and current linked channel is over 11.
      //
/*
#ifdef PRISM54
      if ((currentCh = getCurrentChannel(sock, myIfName)) > 0) {
#else
      if (get_prismoid(WJ_START_IF_NAME, DOT11_OID_CHANNEL, (void *)&currentCh, 4) == 0) {
#endif
          if (wlanConf.region == 2 && currentCh > 11) {
              //North America regulatory and channel > 11.  Block the response
              fromLen = sizeof(ipHandshake);
              n = recvfrom(sock, (char *)&rMsg, sizeof(discoveryReqMsg), 0,
                           (struct sockaddr *)&fromAddr, &fromLen);
              if (n <= 0) {
                  WiJETLogHandler(wijet_log, "CHANNEL out of range: read packet error!", WIJET_LOG_LEVEL1);
              }
              updateChannelDisplay(&wlanConf, option);
              checkFlag = 1;
          }
      } else return -1;
*/
//	m_Log->log(LogTrace::INFO, "Check Channel");
	return 0;
}

int AutoIpProcess::HandleDiscovery()
{
#define GATEWAY_IP sizeof(unsigned long)
	int inSession = 0;
	ipHandshake sMsg;
	int sock = this->m_ScanUDP.GetSocketID();
	struct sockaddr_in  fromAddr;
	int n = this->m_ScanUDP.RecvFrom((char *)&m_Message, sizeof(discoveryReqMsg) + GATEWAY_IP, &fromAddr);
	if (n <= 0) {
        	m_Log->log(LogTrace::ERROR, "recv errno = %d", errno);
		return -1;
        }

// Moderartor control   02-16-2005
	if (moderatorIP != 0 && fromAddr.sin_addr.s_addr != grantedIP)
        	inSession = 1;
	else 
		inSession = m_shm.getState();
	if (inSession) {
		sMsg.drp.t.type = TYPE_IN_SESSION;
		sMsg.drp.t.type = htons(sMsg.drp.t.type);
		for (int i = 0; i< 6; i++)
			sMsg.drp.r.mac[i] = myNetMAC[i];
		*((CARD32 *)sMsg.drp.r.ipAddr) = myNetIpAddr;
		sMsg.drp.extLen     = sizeof(discoveryRespExt);
		sMsg.drp.e.dNameLen = htons(strlen(&deviceName[0]));
		strcpy((char*)&sMsg.drp.e.dName[0], &deviceName[0]);
		sMsg.drp.cmd        = DISCOVERY_RESP;

/*      02-16-2005
 * Append Type and Moderator IP on the end of device name
 */
		sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-6] = 0;
		sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-5] = 'E';
//			((verInfo.pcode &  0x00FF) == PCODE_VIDEO) ? 'V':'G';
		memcpy(&sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-4], &moderatorIP, 4);
		this->m_ScanUDP.SendTo(SERVERPORT, fromAddr.sin_addr.s_addr,
        		(char *)&sMsg, sizeof(discoveryRespMsg));

	}
	else {
		if ((fromAddr.sin_addr.s_addr & *((CARD32 *)m_Message.drq.r.nMask)) ==
                (*((CARD32 *)m_Message.drq.r.ipAddr) & *((CARD32 *)m_Message.drq.r.nMask)) ) {
	unsigned int  maxMask;
/*
 * Retrieve my current IP address, subnet mask, MAC address.                     */
	m_Log->log(LogTrace::DEBUG, "(1).[%d] DISCOVERY RECV: from IP: %s    PORT: %d,  %d bytes", state,
                inet_ntoa(fromAddr.sin_addr), ntohs(fromAddr.sin_port), n);
//	m_Log->log(LogTrace::DEBUG, "CHECK  IP   %08X  &  %08X  & %08X ", fromAddr.sin_addr.s_addr,  *((CARD32 *)m_Message.drq.r.nMask), *((CARD32 *)m_Message.drq.r.ipAddr));

                if (state == 0) 
			this->RetrieveIP(sock);

/*
 * Is the offered IP address in the subnet as mine?
 */

		maxMask =  (*(unsigned int *)m_Message.drq.r.nMask);
		if (maxMask < (unsigned int)myNetMask) maxMask = (unsigned int)myNetMask;

//		if( (*((CARD32 *)m_Message.drq.r.ipAddr) & *((CARD32 *)m_Message.drq.r.nMask))
//                        == (myNetIpAddr & myNetMask) &&

// use max mask to operate both IP      12-14-2005

		if(  (*((unsigned int *)m_Message.drq.r.ipAddr) & maxMask)
                        == ((unsigned int)myNetIpAddr & maxMask) &&
                        (*((CARD32 *)m_Message.drq.r.ipAddr) != myNetIpAddr) ) {
//
// Also make sure that the offered IP address is not same as mine.
//

                        sMsg.drp.t.type = TYPE_ACK;
                        psmNetMask = *((CARD32 *)m_Message.drq.r.nMask);
                        state = 1;
			typeFlag = 0;
                 } else if(m_autoIpMode) {
/* It is in different subnet.  Take the offered IP and change my setup. */

//myNetIpAddr = *((CARD32 *)rMsg.drq.r.ipAddr);
//myNetMask = *((CARD32 *)rMsg.drq.r.nMask);
                        psmNetMask = *((CARD32 *)m_Message.drq.r.nMask);
                        if (state == 0) {
//
// Avoid using subnet's broadcast address from the offered IP address
//

                          if ( (*((CARD32 *)m_Message.drq.r.ipAddr) & ~(*((CARD32 *)m_Message.drq.r.nMask))) !=
                               ~(*((CARD32 *)m_Message.drq.r.nMask)) ){
//
// Modify my IP address and Net Mask
//


           m_Log->log(LogTrace::DEBUG, "From %08X IP change from %08X to %08X Mask change from %08X to %08X",
          *(unsigned int*)&fromAddr.sin_addr.s_addr,
          (unsigned int)myNetIpAddr,  *(unsigned int*)&m_Message.drq.r.ipAddr,
          (unsigned int)myNetMask, *(unsigned int*)&m_Message.drq.r.nMask);

          this->SetIpUsing(sock, &myIfName[0], SIOCSIFADDR, (*((CARD32 *)m_Message.drq.r.ipAddr)));
          this->SetIpUsing(sock, &myIfName[0], SIOCSIFNETMASK, (*((CARD32 *)m_Message.drq.r.nMask)));

// Change Default Gateway on new message length
// 02222005
          if (n > (int)sizeof(discoveryReqMsg)) {
                 char *p = (char*)&m_Message;
                 unsigned long setingGatway = *(unsigned long*)(p+sizeof(discoveryReqMsg));
                 if (setingGatway != 0 && setingGatway != currentGatway) {
                     SetDefaultGateway((struct in_addr *)&currentGatway, 1);        // remove
                     currentGatway = setingGatway;
                     SetDefaultGateway((struct in_addr *)&currentGatway, 0); // add
                  }
           }

           myNetIpAddr = (*((CARD32 *)m_Message.drq.r.ipAddr));

	   m_shm.tellWD(ShMemory::M_AIP);
           }
       }
       sMsg.drp.t.type = TYPE_MORE;
       state = 2;
       typeFlag = 1;
     }
     else {	// Fixed IP mode and within different Subnet, Good luck
	state = 1;
     }
     sMsg.drp.t.type = htons(sMsg.drp.t.type);
     for (int i = 0; i< 6; i++)
               sMsg.drp.r.mac[i] = myNetMAC[i];
               *((CARD32 *)sMsg.drp.r.ipAddr) = myNetIpAddr;
               sMsg.drp.extLen = sizeof(discoveryRespExt);
               sMsg.drp.e.dNameLen = htons(strlen(deviceName));
               strcpy((char *)&sMsg.drp.e.dName[0], &deviceName[0]);
               sMsg.drp.cmd      = DISCOVERY_RESP;

		m_Log->log(LogTrace::DEBUG, "(2)[%d].Send DISCOVERY RESP...", state);


/* 02-16-2005
 * Append Type and Moderator IP on the end of device name
 */
                sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-6] = 0;
                sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-5] = 'E';
 //                     ((verInfo.pcode &  0x00FF) == PCODE_VIDEO) ? 'V':'G';
                      memcpy(&sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-4], &moderatorIP, 4);

                    int nn = this->m_ScanUDP.SendTo(SERVERPORT, fromAddr.sin_addr.s_addr,
			 (char *)&sMsg, sizeof(discoveryRespMsg));
		 m_Log->log(LogTrace::DEBUG, "Send RESP return %d", nn);		
          } // end of if(fromAddr.sin_...

	}
        return 0;
}
int AutoIpProcess::HandleIpConfirm()
{
        struct sockaddr_in  fromAddr;	
        int n = this->m_ScanUDP.RecvFrom((char *)&m_Message, sizeof(ipAddrConfirmMsg), &fromAddr);

	if (n <= 0)
		m_Log->log(LogTrace::ERROR, "IP confirm received error %d", n);
	else {
		m_Log->log(LogTrace::DEBUG, "(3)[0].IP address confirm received");
		typeFlag = 0;
		state = 0;
	}

        return 0;
}

int AutoIpProcess::HandleIpOffer()
{
        struct sockaddr_in  fromAddr;
        ipHandshake sMsg;
	int len, n, i;
	m_Message.ipA.numOfUnit = ntohs(m_Message.ipA.numOfUnit);
	m_Log->log(LogTrace::DEBUG, "(4)[%d]. %d of offering IP address received.", state, m_Message.ipA.numOfUnit);

	if (m_Message.ipA.numOfUnit <= 0) {
		if (m_Message.ipA.numOfUnit < 0) {
			m_Log->log(LogTrace::ERROR, "OFFER error: negative number of unit!");
			return -1;
		}
/* Clean out the packet from socket buffer */
		len = 4 + OFFER_MAX * sizeof(macIpAddr);
        	n = this->m_ScanUDP.RecvFrom((char *)&m_Message, len, &fromAddr);
	} else {
/*
 * There are numOfUnit offered IP address.  Read it out.
 * Take it if I need it.
 */
 		len = 4 + (m_Message.ipA.numOfUnit * sizeof(macIpAddr));
                n = this->m_ScanUDP.RecvFrom((char *)&m_Message, len, &fromAddr);
 		if (n > 0) {
			for (i=0; i < m_Message.ipA.numOfUnit; i++) {
				if (memcmp(&m_Message.ipA.aOffer[i].mac[0], &myNetMAC[0], 6) == 0)
					 break;
			}
			if (i == m_Message.ipA.numOfUnit) {
				m_Log->log(LogTrace::WARN, "No matching IP offered!\n");
				return -1;
                        } else {
//
// Found an IP address for me.
// If I have not completed my IP address updating yet,
// grap the offered IP address and update my IP address .
//
		//if (typeFlag == 1)
			if (state == 2 || state == 1) {
				int sock = m_ScanUDP.GetSocketID();
				myNetIpAddr = *((CARD32 *)m_Message.ipA.aOffer[i].ipAddr);
				myNetMask = psmNetMask;

                                m_Log->log(LogTrace::INFO, "Got IP address offered and update my address. i=%d", i);
                                m_Log->log(LogTrace::INFO, "myNetIpAddr=%08X,  myNetMask=%08X\n",
                                        (unsigned int)myNetIpAddr, (unsigned int)myNetMask);

//
// Avoid using subnet's broadcast address from the offered IP address.
//
				if ((myNetIpAddr & ~(myNetMask)) != ~(myNetMask)) {
				SetIpUsing(sock, myIfName, SIOCSIFADDR, (myNetIpAddr));
				SetIpUsing(sock, myIfName, SIOCSIFNETMASK, (myNetMask));
				state = 0;
				}
			}
			sMsg.drp.cmd        = IPADDR_ACCEPT;
                        sMsg.drp.t.type     = TYPE_ACK;
                        for (i = 0; i< 6; i++)
                        	sMsg.drp.r.mac[i] = myNetMAC[i];
                        *((CARD32 *)sMsg.drp.r.ipAddr) = myNetIpAddr;
                        sMsg.drp.extLen     = sizeof(discoveryRespExt);
                        sMsg.drp.e.dNameLen = htons(strlen(&deviceName[0]));                            strcpy((char*)&sMsg.drp.e.dName[0], &deviceName[0]);

                        m_Log->log(LogTrace::DEBUG, "(5)[%d]. Send IP address accept\n", state);

/*                                      02-16-2005
 * Append Type and Moderator IP on the end of device name
 */
			sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-6] = 0;
 			sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-5] = 'E';
//??                        ((verInfo.pcode &  0x00FF) == PCODE_VIDEO) ? 'V':'G';
                        memcpy(&sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-4], &moderatorIP, 4);
                    this->m_ScanUDP.SendTo(SERVERPORT, fromAddr.sin_addr.s_addr,                         (char *)&sMsg, sizeof(discoveryRespMsg));


                 }
                 
           } else {
                        m_Log->log(LogTrace::ERROR, "IP offering packet receive error!");
			return -1;
                } //end of if(n>0)                                           
        } // end of if(rMsg.ipA.numOfUnit
        return 0;
}

int AutoIpProcess::HandleReconnect()
{
	struct sockaddr_in  fromAddr;
        ipHandshake sMsg;
	int len = sizeof(reconnectMsg);
	int n = this->m_ScanUDP.RecvFrom((char *)&m_Message, len, &fromAddr);

	if (n <= 0) 
		return -1;

        m_Log->log(LogTrace::INFO, "(*). Reconnect responds.");

	sMsg.rc.cmd = RECONNECT_RESP;
	sMsg.rc.pad = 0;
	this->m_ScanUDP.SendTo(SERVERPORT, fromAddr.sin_addr.s_addr,                         (char *)&sMsg, sizeof(reconnectMsg));
        return 0;
}

int AutoIpProcess::HandleRelease()
{
        struct sockaddr_in  fromAddr;
	int len = sizeof(releaseMsg);
	int n = this->m_ScanUDP.RecvFrom((char *)&m_Message, len, &fromAddr);

	m_Log->log(LogTrace::INFO, "Release message Handled");
	if (n <= 0) 
                    return -1;
	unsigned long ip = *(unsigned long*)(&fromAddr.sin_addr);
	
        if ( (unsigned long)QueryIP() == ip ) // Add on 6-16-06
		system("sdmsg Banner");
        return 0;
}

int AutoIpProcess::HandleQuery()
{
        int inSession = 0;
	unsigned long cIP = 0;
        ipHandshake sMsg;
//      int sock = this->m_ScanUDP.GetSocketID();
        struct sockaddr_in  fromAddr;
        int n = this->m_ScanUDP.RecvFrom((char *)&m_Message, sizeof(discoveryReqMsg), &fromAddr);
        if (n <= 0) {
                m_Log->log(LogTrace::ERROR, "recv errno = %d", errno);
                return -1;
        }

// only if not from loopback need to check insession
	if (ntohl(*((unsigned int*)&m_Message.drq.r.ipAddr)) == INADDR_LOOPBACK)
	{
		if (myIfName[0] == 'e')  sMsg.drp.t.type = 6;
		else sMsg.drp.t.type = m_wlan->checkChannel(); //using on Banner
	}
	else {
// Moderartor control   02-16-2005
        if (moderatorIP != 0 && fromAddr.sin_addr.s_addr != grantedIP)
                inSession = 1;
        else
                inSession = m_shm.getState();
        if (inSession) { 
	        cIP =  QueryIP();
                sMsg.drp.t.type = TYPE_IN_SESSION;
	}
	else
		sMsg.drp.t.type = TYPE_ACK;
	}
        sMsg.drp.t.type = htons(sMsg.drp.t.type);
        for (int i = 0; i< 6; i++)
                sMsg.drp.r.mac[i] = myNetMAC[i];
        *((CARD32 *)sMsg.drp.r.ipAddr) = myNetIpAddr;
        sMsg.drp.extLen     = sizeof(discoveryRespExt);
        sMsg.drp.e.dNameLen = htons(strlen(&deviceName[0]));
        strcpy((char*)&sMsg.drp.e.dName[0], &deviceName[0]);
        sMsg.drp.cmd        = DISCOVERY_RESP;

/*      02-16-2005
 * Append Type and Moderator IP on the end of device name
 *	03-29-2006
 * Append current connected IP on the end of device name
 */
        sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-10] = 0;
	memcpy(&sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-9], &cIP, 4);
        sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-5] = 'E';
//             ((verInfo.pcode &  0x00FF) == PCODE_VIDEO) ? 'V':'G';
        memcpy(&sMsg.drp.e.dName[DEVICE_NAME_MAX_LEN-4], &moderatorIP, 4);
        this->m_ScanUDP.SendTo(SERVERPORT, fromAddr.sin_addr.s_addr,
               (char *)&sMsg, sizeof(discoveryRespMsg));

	return 0;
}
