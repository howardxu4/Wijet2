#include "myWlan.h"

#ifdef ORIGIN_CODE

int wlan_startup(wlan_common& wlan)
{
	bool fromRadio = false;
//      WirelessStandardT standard;
        RadioModeT mode, tmode;
	char modeStr[24];
	char ssid[33];
	unsigned short channel, ssidIndex, cur_channel;
	bool interference;
	wlan_advanced wa(&wlan);

//	wlan.GetWirelessStandard(&standard, fromRadio);
        cout << "protocol: " << wlan.getStandard() << endl;

//      wlan.GetRadioMode(&mode, fromRadio);
	mode = wlan.getMode();

	if (mode == MODE_MANAGED) 
		strcpy (modeStr, "Infrastructure Station");
	else if(mode == MODE_ADHOC)
		strcpy (modeStr, "Ad Hoc Station");
	else	// MODE_MASTER
		strcpy (modeStr, "Local Access Point");
	cout << "radio mode: " << mode << " - " << modeStr << endl;

        wlan.GetSSID(ssid, 32, fromRadio);

        cout << "ssid: " << wlan.getSsid() << endl;

        wlan.GetChannel(&channel, fromRadio);
        cout << "channel: " << wlan.getChannel() << endl;

	tmode = MODE_MANAGED;
	wlan.SetRadioMode(tmode);

	NetworkScanResultT *list = wa.ScanForExistingWirelessNetworks();
    	wa.DisplayScanResult(list);
	
	// if AutoMode
	if (mode == MODE_MASTER) {
    		ssidIndex = wa.ComputeNextAvailableSSID(ssid, 1, 9, list);
		if (ssidIndex != 0) {
			char tmpssid[SSID_LENGTH+1];
			sprintf(tmpssid, "%s%d", ssid, ssidIndex);
			strcpy(ssid, tmpssid);
			wlan.SetSSID(ssid);
		}
		cout << "NextAvailable SSID index: " << ssidIndex << endl;
	        
		cur_channel = wa.ComputeOptimalChannel(list);
		// check region prevent using illegal channel

		if (cur_channel != channel) {
			channel = cur_channel;
 			wlan.SetChannel(channel);
		}
	}
	if (mode == MODE_MASTER) {
        interference = wa.InterferenceIsDetected(list, channel);
        cout << (interference? "WARNING: Interferenc": "none") << endl;
	}
	else { // MODE_MANAGED
    	interference = wa.InterferenceIsDetected(list, channel);
     	cout << (interference? "WARNING: Interferenc": "none") << endl;
	}

	if (mode != tmode)
		 wlan.SetRadioMode(mode);
	return 0;
}

void getShow(wlan_common& wlan, bool fromRadio, bool toSet)
{
   try {
	MacAddressT mac;
	WirelessStandardT standard;
	RadioModeT mode;
	bool enable;
	unsigned short useKeyIndex, numberKeys;
	WepKeyT keys[4];
	AuthenticationOptionT authOption;
	char ssid[10];
	unsigned short channel;
	LinkStatusT status;
	numberKeys = fromRadio? 1:4;

	wlan.GetMacAddress(&mac);
	wlan.ShowMacAddress(&mac);
	
	wlan.GetWirelessStandard(&standard, fromRadio);
	cout << "protocol: " << wlan.getStandard() << endl;
	if (toSet) wlan.SetWirelessStandard(standard);

	wlan.GetRadioMode(&mode, fromRadio);
	cout << "radio mode: " << wlan.getMode() << endl;
	mode = MODE_MANAGED;
	if (toSet) wlan.SetRadioMode(mode);

	wlan.GetWepKeys(&enable, &useKeyIndex, &keys[0], numberKeys, fromRadio);
	cout << " enable: " << enable << " useKeyIndex = " << useKeyIndex << endl;
        for(int i=0; i<numberKeys; i++)
                cout << "read key[" << keys[i].id << "]: " << keys[i].key << endl;
	if (toSet) wlan.SetWepKeys(enable, useKeyIndex, &keys[0], numberKeys);

	wlan.GetAuthOption(&authOption, fromRadio);
	cout << "authOption : " << wlan.getAuthOption() << endl;
	if(toSet) wlan.SetAuthOption(authOption);

	wlan.GetSSID(ssid, 9, fromRadio);
	cout << "ssid: " << wlan.getSsid() << endl;
	if(toSet) wlan.SetSSID(ssid);

	wlan.GetChannel(&channel, fromRadio);
	cout << "channel: " << wlan.getChannel() << endl;
	if(toSet) wlan.SetChannel(channel);

	wlan.GetLinkStatus(&status);
	cout << "quality: " << status.linkQuality << ", level: " << status.signalLevel << endl;

    }

    catch (WLAN_EXCEPTION *e) {
    cout << e->description() << endl;
    } catch(...) {
    cout << "caught an unknown exception\n";
    }
}

int main(int argc, char **argv)
{
  
//	bool fromRadio = false;
//	bool toSet = false;

  try {
    wlan_common wlan("ath0", "conf/nofile2.txt", false);

    cout << "start show from object" << endl;

    getShow(wlan, false, false);

    cout << "**** start show from radio" << endl;

    getShow(wlan, true, true);

    cout << "**** set showing changes" << endl;
    getShow(wlan, false, false);

    cout << endl << "======== write to conf/nofile.txt =======" << endl;
    wlan.Serialize("conf/nofile.txt");


    wlan_advanced wa(&wlan);
    NetworkScanResultT *list = wa.ScanForExistingWirelessNetworks();
    wa.DisplayScanResult(list);
    cout << wa.ComputeNextAvailableSSID("2WIRE", 975, 976, list) << endl;
    if (wa.InterferenceIsDetected(list, 5)) {
      cout << "Yes" << endl; 
    } else {
      cout << "NO" << endl;
    }

    cout << "optimal channel = " << wa.ComputeOptimalChannel(list) << endl;

    wlan_startup(wlan);

  } catch(WLAN_EXCEPTION *e) {
    cout << e->description() << endl;
  } catch(...) {
    cout << "caught an unknown exception\n";
  }
  return 0;
}

#else 

int main(int argc, char **argv)
{
        char modeStr[24];
        myWlan wlan(WLAN_CONFIG);

        cout << "protocol: " << wlan.getStandard() << endl;

        int mode = wlan.getMode();

        if (mode == MODE_MANAGED)
                strcpy (modeStr, "Infrastructure Station");
        else if(mode == MODE_ADHOC)
                strcpy (modeStr, "Ad Hoc Station");
        else    // MODE_MASTER
                strcpy (modeStr, "Local Access Point");
        cout << "radio mode: " << mode << " - " << modeStr << endl;

        cout << "ssid: " << wlan.getSsid() << endl;

        cout << "channel: " << wlan.getChannel() << endl;

        wlan.startUp();

        return 0;
}
#endif
