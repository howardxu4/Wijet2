#include "ConfigCommon.h"
#include "ConfigTools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ConfigCommon cfg;
ConfigTools tool;
LdProp  clist;
LdProp  wlist;

#define BANNERSHOW      "BNSHOW"

#define X8211MODE       "standard"
#define RADIOMODE	"radioMode"
#define OPMODE		"operationMode"
#define CHANNEL         "CHANNEL"
#define SSID            "SSID"

#define SSID_LENGTH             31

char* a80211Modes[] = {
                "802.11A",
                "802.11G",
                NULL
};

char* aRadioModes[] = {
                "Access Point",
                "Infrastructure Station",
                "Disable Wireless",
                NULL
};

char* checked[] = { "", "checked" };

void showPage() 
{
        const char*pb;
    	pb = clist.getIntValue(BANNERSHOW) ? "checked" : " ";
	int k = wlist.getIntValue(X8211MODE) > 1? 1:0;

	cfg.putHeader("Pegasus Wireless CYNALYNX -  Options");

	cfg.putTag("<style type=\"text/css\">.fb { FONT-SIZE: 11px; BACKGROUND: #ffffff; FONT-FAMILY: Verdana, Arial, Helvetica, sans-serif } </style>");
	cfg.putTag("<script type=\"text/javascript\"> function Reboot() { if (!confirm('The unit will be rebooted and the wireless connection will be terminated. Do you want to proceed?')) return false;  return true; } </script>");

	cfg.startPage(" Options","This page contains controls to reboot the CYNALYNX unit, toggle the banner display information, switch the wireless mode, and change the ssid options.");

if (strcmp(cgiRemoteAddr, "127.0.0.1") == 0) {

	cfg.putTag("<p>This page contains controls to reboot the CYNALYNX unit, toggle the banner display information, switch the wireless mode, and change the ssid options.</p>");

	cfg.putTag("<form name=wlan_optn action=wlan_optn.cgi method=post enctype=multipart/form-data><br>");

	cfg.putSubtitle("Device Control");
        cfg.skipRow();
	cfg.actionRow("Reboot the unit", "<input type=submit name=reboot value='Proceed with reboot' onClick='return Reboot();'>");
	cfg.dispRow("&nbsp;", "Rebooting the unit will apply all saved changes.");
	cfg.skipRow();

        cfg.actionRow("Banner information enabled", cfg.getFValue("<input type=checkbox name=hinfoEnabled %s>", (char*)pb));
        cfg.dispRow("&nbsp;", "Use this control to enable or display the start up page system status reports.");
        cfg.skipRow();

        cfg.actionRow("802.11 mode", cfg.getFValue("<input type=radio name=x80211mode value=802.11A %s>802.11 A <input type=radio name=x80211mode value=802.11G %s>802.11 G", checked[!k], checked[k]));
        cfg.dispRow("&nbsp;", "Select the 802.11 wireless LAN standard that will be supported. <br>802.11 b/g - supports channels in the 2.4GHz spectrum<br>802/11 a - supports channels in the 5.0GHz spectrum");
        cfg.skipRow();

        cfg.actionRow("Radio mode", cfg.getSelect("radiomode", 3, aRadioModes, wlist.getIntValue(RADIOMODE), ""));
        cfg.dispRow("&nbsp;", "Select the unit's radio operation mode. The CYNALYNX unit can operate as an ACCESS POINT (default) or as an INFRASTRUCTURE STATION.<br>If the radio is set to operate as an INFRASTRUCTURE STATION, enter the correct SSID of the access point to which the CYNALYNX will be assigned in the &quot;SSID/Network &quot; name field below. <br>Disable Wireless: Selecting this mode will turn off all radio functions. When the radio is disabled, the CYNALYNX can still be assessable via a LAN connection. Select this feature if your deployment does not require the CYNALYNX to be accessed using a wireless connection. <br>(See manual for more information)");
        cfg.skipRow();

        cfg.actionRow("SSID /network name",cfg.getFValue("<input type=text name=ssid maxLength=30 size=34 value='%s' >", wlist.getValue(SSID)));
        cfg.dispRow("&nbsp;", "Use this field to assign the SSID / Network name. SSIDs are case-sensitive and be careful not to include trailing spaces.");
        cfg.skipRow();

        cfg.actionRow("Apply settings", "<input type=submit name=applyOptn value=Apply > <input type=reset value=Cancel id=reset1 name=reset1>");
        cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();

	cfg.putTag("</form>");
}
else
	cfg.putTag("<script language=javascript>document.location.href='wijetpsm.exe';</script>");
	
	cfg.endPage();
}

int HandleUpdate()
{
  int n = 0, i = 0;
  char s[100];
  if (cgiFormSubmitClicked("reboot") == cgiFormSuccess) {  
	tool.Changes(tool.ADMIN, 0);
  }
  else if (cgiFormSubmitClicked("applyOptn") == cgiFormSuccess) {
  	if (cgiFormSelectSingle("x80211mode", a80211Modes, 2, &i, 0+1) == cgiFormSuccess) {
		i = i? 3:1;
		if (wlist.getIntValue(X8211MODE) != i) {
        		wlist.setValue(X8211MODE, i);
			n = wlist.getIntValue(CHANNEL);
			if (n > 14 && i > 1)			// G
				wlist.setValue(CHANNEL, 6);
			else if (n < 36 && i < 2)		// A
				wlist.setValue(CHANNEL, 40);
  		}
	}
	if (cgiFormSelectSingle("radiomode", aRadioModes, 3, &i, 0+1) == cgiFormSuccess) {
		if (i != wlist.getIntValue(RADIOMODE) ) {
        		wlist.setValue(RADIOMODE, i);
			if ( i < 2 ) wlist.setValue(OPMODE, i);
			n = 1;
		}
  	}

	if (cgiFormStringNoNewlines("ssid", s, SSID_LENGTH-1) == cgiFormSuccess) {
		if (strcmp(s, wlist.getValue(SSID)) != 0) {
			wlist.setValue(SSID, s);
			n = 1;
		}
	}
	if (n != 0) wlist.saveToFile(WLAN_CONFIG);
	i = 0;
        if (cgiFormCheckboxSingle("hinfoEnabled") == cgiFormSuccess)
                i = 1;
        if (clist.getIntValue(BANNERSHOW) != i) {
                clist.setValue(BANNERSHOW, i);
		clist.saveToFile(OTC_CONFIG);
        }

  }
  return 0;
} 

int cgiMain() 
{
	wlist.noComment(SSID);
        clist.loadFromFile(OTC_CONFIG);
	wlist.loadFromFile(WLAN_CONFIG);

	int code = HandleUpdate();
	if (code == 0)
		showPage();
	return 0;	
}
