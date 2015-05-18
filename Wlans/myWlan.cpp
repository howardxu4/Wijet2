

/**
 *      The myWlan class wrappered the wlan system
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#include "myWlan.h"

/**
 *      The constructor
 *
 */
myWlan::myWlan(char* file)
{
	m_wlan = NULL;
	m_wa = NULL;
        m_Log = new LogTrace("Wlan");
        m_Log->setLog(LogTrace::ERROR);
	if (file) init(file);
}

/**
 *      The destructor
 *
 */
myWlan::~myWlan()
{
	if (m_wlan) delete m_wlan;
	if (m_wa) delete m_wa;
}

/**
 *      init method
 *
 */
int myWlan::init(char* file)
{
	try {
		m_wlan = new wlan_common( WLANIFNAME, file);
		m_wlan->setLog(m_Log);
		m_wa = new wlan_advanced(m_wlan);
		m_Log->log(LogTrace::INFO, "wlan start using file %s", file); 		}
	catch (...) {
		if (m_wlan) { delete m_wlan; m_wlan = NULL; } 
		m_Log->log(LogTrace::ERROR, "wlan start fails on %s", file);
		return -1;
	}
	return 0;
}

/**
 *	isAvailable method to check whether wlan initilized
 */
bool myWlan::isAvailable()
{
	if (m_wlan == NULL)  m_Log->log(LogTrace::WARN, "wlan uninitialized");
	return (m_wlan != NULL);
}

/**
 *	startUp() method to start wlan configuration setting
 */
int myWlan::startUp()
{
        if (!isRadioOn()) {
                m_Log->log(LogTrace::ERROR, "The Wireless card is not ready.");
                return -1;
        }
	try {
                RadioModeT mode = m_wlan->getMode();    
		if (mode == MODE_MASTER) {
			getProperty gP(gP.P_SCAN);
			m_wlan->SetSSID(gP.getStr("SSID"));
			m_wlan->SetChannel(gP.getInt("Channel"));
		}
		m_wlan->SetAll();
	}
        catch(...) {
                m_Log->log(LogTrace::ERROR, "exception happened!");
                return -1;
        }		
	return 0;
}

int myWlan::setScan()
{
	if (!isRadioOn()) {
		m_Log->log(LogTrace::ERROR, "The Wireless card is not ready.");	
		return -1;
	} 
	try{
	        bool interference;
		unsigned short  ssidIndex;
		int opMode = m_wlan->m_wlist.getIntValue(OPMODE);
		int cur_channel = m_wlan->getChannel();
		RadioModeT mode = m_wlan->getMode();	// Save Mode

	m_Log->log(LogTrace::INFO, "Get Chnannel %d Mode %d SSID %s", cur_channel,
	        mode, m_wlan->getSsid()); 

		m_wlan->SetRadioMode(MODE_MANAGED);	// Change to Station
	m_Log->log(LogTrace::INFO, "Change mode to managed");
		NetworkScanResultT *list = m_wa->ScanForExistingWirelessNetworks();
	m_Log->log(LogTrace::INFO, "Get scan list");
 	
        if (opMode == 0) 				// if AutoMode
        if (mode == MODE_MASTER) {
		// change cflict SSID to append a number
                ssidIndex = m_wa->ComputeNextAvailableSSID(m_wlan->getSsid(), 1, 9, list);
                if (ssidIndex != 0 && mode == MODE_MASTER) {
                        char tmpssid[SSID_LENGTH+1];
                        sprintf(tmpssid, "%s%02d", m_wlan->getSsid(), ssidIndex);
                        m_wlan->SetSSID(tmpssid);	// Set new SSID
			m_Log->log(LogTrace::INFO, "new SSID = %s", tmpssid);
                }
		m_Log->log(LogTrace::INFO, "NextAvailable SSID index: %d ",ssidIndex );
	}

	cur_channel = m_wa->ComputeOptimalChannel(list);
       	// check region prevent using illegal channel

	m_Log->log(LogTrace::INFO, "OptimalChannel = %d", cur_channel);
       	// Always set channel to make 11a have a correct channel configured.
	//if (cur_channel != m_wlan->getChannel()) 
               	m_wlan->SetChannel(cur_channel);  // Set new Channel
       
	m_Log->log(LogTrace::INFO, "Auto mode %d  check SSID", opMode); 

        if ( mode == MODE_MASTER) {
        interference = m_wa->InterferenceIsDetected(list, m_wlan->getChannel());
        }
	else {
		// for Station waht to do?
	interference = m_wa->InterferenceIsDetected(list, m_wlan->getChannel());
	}	
	if (interference) m_Log->log(LogTrace::INFO, "WARNING: Interferenc");
	else m_Log->log(LogTrace::INFO, "none");

  	char detectedStr[80];
	int prev_channel = 0;
	int channels[16];
	detectedStr[0] = ' ';
	detectedStr[1] = 0;
  	if (list != NULL) {
    		NetworkScanResultT *next = list;
    		while(next != NULL) {
			bool found = false;
			for (int j=0; j<prev_channel; j++)
				if (next->channel == channels[j]) 
					found = true;
			if (next->channel != cur_channel && found == false)	
			{
				char cStr[10];
				if (detectedStr[1] == 0)
					sprintf(cStr, "%d", next->channel);
				else
					sprintf(cStr, ",%d", next->channel);
				strcat(detectedStr, cStr);	
				channels[prev_channel++] = next->channel;
				m_Log->log(LogTrace::INFO, "channel = %d\n", next->channel);

       			}
       			next = next->next;
    		}
  	}
	else  m_Log->log(LogTrace::WARN, "Scan List is NULL!");

// need write a scan.txt file for banner use
	LdProp slist;
	slist.setProperty("SSID", m_wlan->getSsid());
	slist.setProperty("Channel", cur_channel);
	slist.setProperty("RF activities detected", detectedStr);
	slist.setProperty("Interference",interference);
	slist.setProperty("Mode", (char*)(mode==MODE_MASTER?"Access Point":"Station"));
	slist.setProperty("Security", m_wlan->getWepEnabled());
	slist.saveToFile(WJ_START_SCAN_FILE);

	m_Log->log(LogTrace::INFO, "Save to %s", WJ_START_SCAN_FILE);
// change back
	if (mode != MODE_MANAGED)  
		m_wlan->SetRadioMode(mode);
//	m_wlan->SetAll();
	m_Log->log(LogTrace::INFO, "Final Get Chnannel %d Mode %d SSID %s", cur_channel,
        mode, m_wlan->getSsid());
	}
	catch(...) {
		m_Log->log(LogTrace::ERROR, "exception happened!");
		return -1;
	}
	return 0;
}

int myWlan::checkChannel()
{
	int n = -1;
	unsigned short channel;
	int radioMode = m_wlan->m_wlist.getIntValue(RADIOMODE);
	if (radioMode == 0) {	// AP Mode
		m_wlan->GetChannel(&channel, true);
		n = channel;
	}
	else
	try {
		LinkStatusT status;
		m_wlan->GetLinkStatus(&status);
		if (status.linkQuality > 0) {
			m_wlan->GetChannel(&channel, true);
			n = channel;
		}
	}
	catch(...) {}
	return n;
}
