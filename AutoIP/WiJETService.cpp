#include "AutoIpProcess.h"


#define BURN_LOG_FILE "temp.txt"

/*
 * GetPreviousBurnTime() - retrieve the time previously spent on the burning
 * test so that the burning test time can be accumulated.
 * The main task is to parse the burning test log file and get the information
 * from this file.
 */
int AutoIpProcess::GetPreviousBurnTime(char *log_dom_sn)
{
  FILE *fp = NULL;
  char line[1024], backup[1024];
  int time, hours, minutes, seconds;
 
  if ((fp = fopen(BURN_LOG_FILE, "r")) == NULL) {
      *log_dom_sn = 0;
  }
  while(fgets(line, 1023, fp)) {
    strcpy(backup, line);
  }
  sscanf(backup, "Elapsed Time (%d seconds, or %d: %d: %d)     DOM-SN: %s",
         &time, &hours, &minutes, &seconds, log_dom_sn);
  return time;
}

// Query connected PC IP from WiJET service

int  AutoIpProcess::QueryIP()
{
	SocketTCP myTCP;
	char buf[60];
	unsigned int n = 0;
	myTCP.Open(WIJET2_PORT, htonl(INADDR_LOOPBACK));
	strcpy(buf, "AskIP");
	if (myTCP.Send(buf, strlen(buf) +1) > 0) {
		if (myTCP.Recv(buf, 60) > 0) {
			sscanf(buf, "%u", &n);
               		m_Log->log(LogTrace::INFO, "Connected IP is %u", n);
		} 
	}
	return n;
}

// Using external file to store the global variables between processes
#define WIJET_MEM_FILE "memtemp.txt"
Bool  AutoIpProcess::accessModeratorVariables(Bool isRead)
{
        FILE *fd;
        if ((fd = fopen(WIJET_MEM_FILE, isRead?"r":"w")) == NULL)
        return False;
        if (isRead)
                fscanf(fd, "%ld %ld", &moderatorIP, &grantedIP);
        else
                fprintf(fd, "%ld %ld", moderatorIP, grantedIP);
        fclose(fd);
        return True;
}

int  AutoIpProcess::CheckMessage(unsigned char *buf, int n)
{
	int rtn = -1;
	buf[n] = 0;
	MsgHeader myMsgHeader((char*)&buf[0]);
        if (myMsgHeader.checkMsgHeader()) 
                rtn = myMsgHeader.getCommand();
        
        return rtn;
}

void  AutoIpProcess::ProcessModerator()
{
        unsigned long ip;
	struct sockaddr_in addr;
	SocketTCP *myTCP;
        myTCP = m_ModeratorTCP.Accept(&addr);
        m_Log->log(LogTrace::INFO, "Start Process Moderator Message");

        if (myTCP != NULL) {
                char buf[80];
                int n = myTCP->Recv((char*)buf, 80);
		ip = *(unsigned long*)&addr.sin_addr;

                if (n > 0) {
                        int i = CheckMessage((unsigned char*)buf, n);
	        m_Log->log(LogTrace::INFO,"Check retur %d from size %d", i, n);
			if (i < 0) {
				sprintf(buf, "Sorry, wrong message format");
			} 
			else if (i == PSM2WiJETQueryDiscoveryInfo) {	// from PSM query
                                ipHandshake *sMsg = (ipHandshake*)&buf[0];
				int inSession = 0;
				if (moderatorIP != 0 && ip != grantedIP)
                			inSession = 1;
				else
					inSession =  m_shm.getState();
				sMsg->drp.t.type = inSession? TYPE_IN_SESSION: TYPE_ACK;
				sMsg->drp.t.type = htons(sMsg->drp.t.type);
				for (i = 0; i < 6; i++)
					sMsg->drp.r.mac[i] = myNetMAC[i];
				*((CARD32 *)sMsg->drp.r.ipAddr) = myNetIpAddr;
				sMsg->drp.extLen = sizeof(discoveryRespExt);
				sMsg->drp.e.dNameLen = htons(strlen(&deviceName[0]));
				strcpy((char*)&sMsg->drp.e.dName[0], &deviceName[0]);
				sMsg->drp.cmd =  DISCOVERY_RESP;
				sMsg->drp.e.dName[DEVICE_NAME_MAX_LEN-6] = 0;
				sMsg->drp.e.dName[DEVICE_NAME_MAX_LEN-5] = 'E';
				memcpy(&sMsg->drp.e.dName[DEVICE_NAME_MAX_LEN-4], &moderatorIP, 4);
				myTCP->Send(buf, sizeof(discoveryRespMsg));
				sprintf(buf, "OK");			
	
			}
                        else if (i == MdPlayer2WiJETGetDisplayDelta) {   // from inner MdPlayer
                                int x =0, y = 0;
                                getDelta(&x, &y);
                                *(short*)&buf[10] = 10;
                                *(int*)&buf[12] = x;
                                *(int*)&buf[16] = y;
                                myTCP->Send(buf, 20);
				sprintf(buf , "OK");
                        }
                        else {
			    unsigned int inSessionIP = QueryIP();
			    if (moderatorIP == 0 || moderatorIP == ip) {
                                if (moderatorIP != 0 || inSessionIP == 0 ||
                                        (moderatorIP == 0 && ip == inSessionIP) ) {
                                        if (i == CONTROLLER2WiJETTakeControl) {
                                                char tmp[40];
                                                strcpy(tmp, &buf[12]);
                                                moderatorIP = ip;
                                                grantedIP = inet_addr(&buf[12]);                                                sprintf(buf, "Ha ... %ld Gip %s Sin %s", moderatorIP, tmp,
                                                        inet_ntoa(*(struct in_addr *)(&inSessionIP)));
                                                accessModeratorVariables(False);                                        }
                                        else if (i == CONTROLLER2WiJETGrantAccess){
                                                sprintf(buf, "Xi ... %ld -> 0 %s", moderatorIP,
                                                        inet_ntoa(*(struct in_addr *)(&inSessionIP)));
                                                moderatorIP = grantedIP = 0;
                                                accessModeratorVariables(False);                                        }
                                        else if (i == PSM2WiJETSetDisplayDelta) {
                                                // set display delta
                                                n = *(short*)&buf[10];
                                                if (n >= 8) {
                                                        int x, y;
                                                        x = ntohl(*(int*)&buf[12]);
                                                        y = ntohl(*(int*)&buf[16]);

                                                        setDelta(x, y);
                                                        sprintf(buf, "Set X = %d Y = %d", x, y);

                                                }
                                        }
                                        else {
                                                sprintf(buf, "Sorry, wrong message");
                                        }
                                }
                                else
printf(buf,"Sorry, is using by (%s) Mod %ld",
                                                inet_ntoa(*(struct in_addr *)(&inSessionIP)), moderatorIP);
                        }
                        else
                                sprintf(buf,"Sorry, permission deny (%s)",
                                        inet_ntoa(*(struct in_addr *)(&moderatorIP)));
                    }        
		myTCP->Send(buf, strlen(buf));
	        m_Log->log(LogTrace::INFO, buf);

                }
                myTCP->Close();
        }
	else {
        	m_Log->log(LogTrace::ERROR, "Failure in Moderator Server");
		m_ModeratorTCP.Close();
		m_ModeratorTCP.OpenServer(WIJET_PORT);	
	}
}

