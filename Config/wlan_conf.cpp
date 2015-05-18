#include "ConfigCommon.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ConfigCommon cfg;
LdProp	wlist;

#define REGION		"region"
#define X8211MODE	"standard"
#define RADIOMODE	"radioMode"
#define WPKENABLED      "dot11PrivacyInvoked"
#define OPMODE		"operationMode"
#define SSID		"SSID"
#define CHANNEL		"CHANNEL"
#define OUTPOWER	"outputPower"

#define OUTPUT_POWER_MAX	12
#define SSID_LENGTH		31

char* aChannels[] = {
		"165 (5.825 GHz)",
		"161 (5.805 GHz)",
		"160 (5.800 GHz)",
		"157 (5.785 GHz)",
		"153 (5.765 GHz)",
		"152 (5.760 GHz)",
		"149 (5.745 GHz)",
		"64 (5.320 GHz)",
		"60 (5.300 GHz)",
		"58 (5.290 GHz)",
		"56 (5.280 GHz)",
		"52 (5.260 GHz)",
		"50 (5.250 GHz)",
		"48 (5.240 GHz)",
		"44 (5.220 GHz)",
                "42 (5.210 GHz)",
                "40 (5.200 GHz)",
		"36 (5.180 GHz)",	// 802.11 A
                "1 (2412 Mhz)",         // 802.11 B|G
                "2 (2417 Mhz)",
                "3 (2422 Mhz)",
                "4 (2427 Mhz)",
                "5 (2432 Mhz)",
                "6 (2437 Mhz)",
                "7 (2442 Mhz)",
                "8 (2447 Mhz)",
                "9 (2452 Mhz)",
                "10 (2457 Mhz)",
                "11 (2462 Mhz)",        // NA
                "12 (2467 Mhz)",
                "13 (2472 Mhz)",        // EU
                "14 (2484 Mhz)",        // JP
		NULL
};

char* aOutPowers[] = {
		"10 %",
		"20 %",
		"25 %",
		"30 %",
		"40 %",
		"50 %",
		"60 %",
		"70 %",
		"75 %",
		"80 %",
		"90 %",
		"Full",
		NULL
};

char* a80211Modes[] = {
		"802.11 Auto",
		"802.11 A",
		"802.11 B",
		"802.11 G",
		NULL
};

char* aRadioModes[] = {
		"Access Point",
		"Infrastructure Station",
		"Disable Wireless",
		NULL
};

char* aSecuOptions[] = {
		"None",
		"WEP",
		"WPA",
		NULL
};

char *csmodes[]   = {
		"Automatic", 
		"Manual",
		NULL
};

char* checked[] = { "", "checked" };

int convert(int channel)
{
	int i = 0;
	while (aChannels[i] != NULL) {
		if (channel == atoi(aChannels[i]))
			return i;
		i++;
	}
	return 23;	
}

void showPage()
{	
	int r = wlist.getIntValue(REGION);
	r = r==0? 32: r==1?31:29;
	int k = wlist.getIntValue(OPMODE);
	cfg.putHeader("Pegasus Wireless CYNALYNX -  Basic Wireless");
	cfg.putScript("config.js");
	cfg.startPage("Basic Wireless","The controls on this page affect the standard wireless settings in use.");
	cfg.putTag("<form name=wlan_secu action=wlan_conf.cgi method=post enctype=multipart/form-data>");

	cfg.putSubtitle("Wireless Configuration");
        cfg.skipRow();

        cfg.actionRow("802.11 mode", cfg.getSelect("x80211mode", 4, a80211Modes, wlist.getIntValue(X8211MODE), "onChange=ValidateChannel()"));
        cfg.dispRow("&nbsp;", "Select the 802.11 wireless LAN standard that will be supported. <br>802.11 b/g - supports channels in the 2.4GHz spectrum<br>802/11 a - supports channels in the 5.0GHz spectrum");
        cfg.skipRow();

	cfg.actionRow("Radio mode", cfg.getSelect("radiomode", 3, aRadioModes, wlist.getIntValue(RADIOMODE), "onChange=RadioModeHandler()"));
	cfg.dispRow("&nbsp;", "Select the unit's radio operation mode. The CYNALYNX unit can operate as an ACCESS POINT (default) or as an INFRASTRUCTURE STATION.<br>If the radio is set to operate as an INFRASTRUCTURE STATION, enter the correct SSID of the access point to which the CYNALYNX will be assigned in the &quot;SSID/Network &quot; name field below. <br>Disable Wireless: Selecting this mode will turn off all radio functions. When the radio is disabled, the CYNALYNX can still be assessable via a LAN connection. Select this feature if your deployment does not require the CYNALYNX to be accessed using a wireless connection. <br>(See manual for more information)");
        cfg.skipRow();
	cfg.actionRow("Security option", cfg.getSelect("wepEnabled", 3, aSecuOptions, wlist.getIntValue(WPKENABLED)));
	cfg.dispRow("&nbsp;", "Select the wireless encryption policy that will be enforced <br>(WEP/WPA/WPA2)");
	cfg.skipRow();
	cfg.actionRow("Operation mode", cfg.getFValue("<input type=radio name=csmode value=Automatic  onClick='CSModeHandler()' %s>Automatic <input type=radio name=csmode value=Manual onClick='CSModeHandler()' %s>Manual", checked[!k], checked[!!k]));
	cfg.dispRow("&nbsp;", "AUTOMATIC: Selecting this mode will allow the CYNALYNX to automatically choose a channel and SSID when it boots up. <br>MANUAL: Selecting this mode will assign the CYNALYNX to the SSID entered in the &quot;SSID/Network&quot; name field below.<br>(See manual for more information)");
        cfg.skipRow();
	cfg.actionRow("SSID /network name",cfg.getFValue("<input type=text name=ssid maxLength=30 size=34 value='%s' onblur='ValidateSSIDlength()' >", wlist.getValue(SSID)));
	cfg.dispRow("&nbsp;", "Use this field to assign the SSID / Network name. SSIDs are case-sensitive and be careful not to include trailing spaces.");
        cfg.skipRow();
        cfg.actionRow("Default channel", cfg.getSelect("channel", r, aChannels,convert(wlist.getIntValue(CHANNEL)), "onChange='ValidateChannel()'"));
        cfg.dispRow("&nbsp;", "Select a set channel using the dropdown box. Choose a channel supported by the 802.11 mode selected. For best performance, choose a channel not in use in the CYNALYNX unit's immediate operating area.<br>This setting is ignored when the CYNALYNX is in INFRASTRUCTURE STATION mode.");
        cfg.skipRow();

#if	0
        cfg.actionRow("Output power", cfg.getSelect("outpower", OUTPUT_POWER_MAX, aOutPowers, wlist.getIntValue(OUTPOWER), " "));
        cfg.dispRow("&nbsp;", "This control sets the antenna transmission power. In a busy wireless environment, decreasing the transmission power of all wireless devices in the vicinity can improve overall performance.");
        cfg.skipRow();
#endif

        cfg.actionRow("Apply settings", "<input type=submit name=applySecConf value=Apply onClick='return ValidateChannel();'> <input type=reset value=Cancel id=reset1 name=reset1>");
	cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();
	cfg.runScript("CSModeHandler()");
	cfg.putTag("</form>");
	cfg.endPage();
}

int SetWireless()
{

  int x, i = 0;
  char s[100];
  if (cgiFormSelectSingle("x80211mode", a80211Modes, 4, &i, 0+1) == cgiFormSuccess) {
        wlist.setValue(X8211MODE, x=i);
  }
  i = 0;
  if (cgiFormSelectSingle("radiomode", aRadioModes, 3, &i, 0+1) == cgiFormSuccess) {
	wlist.setValue(RADIOMODE, i);
  }
  i = 0;
  if (cgiFormSelectSingle("wepEnabled", aSecuOptions, 3, &i, 0+1) == cgiFormSuccess) {
	wlist.setValue(WPKENABLED, i);
  }
  i = 0;
  if (cgiFormStringNoNewlines("ssid", s, SSID_LENGTH-1) == cgiFormSuccess) {
	wlist.setValue(SSID, s);
  }
  i = 0;
  if (cgiFormSelectSingle("csmode", csmodes, 2, &i, 0+1) == cgiFormSuccess) {
	wlist.setValue(OPMODE, i);
  }
  i = 0;
  if (cgiFormSelectSingle("channel", aChannels, 32, &i, 6) == cgiFormSuccess) {
	i = atoi(aChannels[i]);	
	wlist.setValue(CHANNEL, i);
  }
  else  i = wlist.getIntValue(CHANNEL);
 
  //  double check channel setting
  //  if (x > 1 && i < 18) x = 23;		// Channel 6
  //  else if (x <=1 && i > 17) x = 16;		// Channel 40
  //  if (x > 10) wlist.setValue(CHANNEL, x);

  i = 0;
  if (cgiFormSelectSingle("outpower", aOutPowers, OUTPUT_POWER_MAX, &i,
                           OUTPUT_POWER_MAX-1) == cgiFormSuccess) {
	wlist.setValue(OUTPOWER, i);
  }
  // Changing the wlan driver settings may causes the connection
  wlist.saveToFile(WLAN_CONFIG);

//  should tell watch dog ??
//  Reboot();
  return 1;
}

int cgiMain() 
{
	wlist.noComment(SSID);
	wlist.loadFromFile(WLAN_CONFIG);

	if (cgiFormSubmitClicked("applySecConf") == cgiFormSuccess) {
		SetWireless();
		wlist.init(20);	
		wlist.loadFromFile(WLAN_CONFIG);
	}
	showPage();
	return 0;
}
