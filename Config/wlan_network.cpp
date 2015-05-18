#include "ConfigCommon.h"
#include "ConfigTools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ConfigCommon cfg;
ConfigTools tool;
LdProp  clist;

#define DEVICENAME	"DNAME"
#define	IPADDR		"IPADDR"
#define	IPMASK		"IPMASK"
#define	GATEWAY		"GATEWAY"
#define	FIPMODE		"FIPMODE"
#define PDNS    	"PDNS"
#define ADNS    	"ADNS"


#define CHECKED		"checked"

void showPage() 
{
	const char *mp, *dp;
	int i = atoi(clist.getValue(FIPMODE));
	mp = i ? "": CHECKED;
	dp = i ? CHECKED: "";
	cfg.putHeader("Pegasus Wireless CYNALYNX -  Network");
	cfg.putScript("validate.js");
	cfg.startPage(" Network","This page contains network connections settings.");
	cfg.putTag("<form name=wlan_admin action=wlan_network.cgi method=post enctype=multipart/form-data>");

        cfg.putSubtitle("Network settings");
	cfg.actionRow("Device name", cfg.getFValue("<input type=text name=devname maxlength=30 size=34 value='%s'>", clist.getValue(DEVICENAME))); 
	cfg.dispRow("&nbsp;", "Enter a unique name to identify the unit. The maximum length of this field is 30 characters. Any readable character is allowed."); 
        cfg.skipRow();
	cfg.actionRow("IP addressing mode", cfg.getFValue("<input type=radio name=fipmode value=Manual-Static %s >Manual-Static<input type=radio name=fipmode value=DHCP %s >DHCP", (char*)mp, (char*)dp));

	cfg.dispRow("&nbsp;", "<b>Manual-Static:</b> CYNALYNX is assigned a fixed IP address. It will retain the assigned IP address in all instances.<br> <b>DHCP:</b> CYNALYNX is assigned an IP address by a DHCP server. The unit will retain this IP address until the lease expires or it is rebooted.");
        cfg.skipRow();
	cfg.actionRow("IP address",cfg.getFValue("<input type=text name=ipaddr size=20 maxlength=15 value='%s'>", clist.getValue(IPADDR)));
	cfg.dispRow("&nbsp;", "This field specifies the unit's IP address when the Manual-Static IP address mode is selected.  Enter a valid IP address.");
        cfg.skipRow();
        cfg.actionRow("Subnet mask", cfg.getFValue("<input type=text name=ipmask size=20 maxlength=15 value='%s'>", clist.getValue(IPMASK)));
        cfg.dispRow("&nbsp;", "This field specifies the unit's subnet mask when the Manual-Static IP address mode is selected.  Enter a valid subnet mask.");
        cfg.skipRow();
        cfg.actionRow("Default gateway", cfg.getFValue("<input type=text name=gateway size=20 maxlength=15 value='%s'>", clist.getValue(GATEWAY)));
        cfg.dispRow("&nbsp;", "This field specifies the unit's default gateway when the Manual-Static IP address mode is selected.  Enter a valid default gateway IP address.");
        cfg.skipRow();
        cfg.actionRow("preferred DNS server", cfg.getFValue("<input type=text name=PrefDNS size=20 maxlength=15 value=%s>",  clist.getValue(PDNS)));
	cfg.dispRow("&nbsp;", "This field specifies the unit's preferred DNS server address.  Enter a valid DNS IP address.");
	cfg.skipRow();

        cfg.actionRow("Alternate DNS server", cfg.getFValue("<input type=text name=AltDNS size=20 maxlength=15 value=%s>",  clist.getValue(ADNS)));
	cfg.dispRow("&nbsp;", "This field specifies the unit's alternate DNS server address.  Enter a valid DNS IP address.");
	cfg.skipRow();

        cfg.actionRow("Apply network settings", "<input type=submit name=applyNetSetting value=Apply onClick='return ValidateNet();'> <input type=reset name=cancelNetSetting value=Cancel>");
	cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new network settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();

	cfg.putTag("</form>");
	cfg.endPage();
}

void ShowNetworkSettingFailureMessage()
{
	cfg.putHeader("Pegasus Wireless CYNALYNX - Failure Message");
	cfg.startPage("Failure Message", "This page contains the possible failure reasons on the network setting.");
	cfg.dispRow("<font class=f1 color=red>Network setting failure</font>", "&nbsp;");
	cfg.dispRow("&nbsp;", "Several factors may cause the failure:<br>1. Bad subnet mask format.</br>2. The ip and gateway are not under the same subnet.<br>Try the setting again.");

	cfg.putTag("<form name=wlan_admin action=wlan_network.cgi method=post enctype=multipart/form-data>");
        cfg.skipRow();
        cfg.actionRow("&nbsp;", "<input type=submit name=continue value=Continue>");
        cfg.putTag("</form>");
        cfg.endPage();
}

int badMask(u_long mask, u_long addr) {
    if (addr & (mask = ~mask))
        return 1;
    mask = ntohl(mask);
    if (mask & (mask+1))
       return 1;
    return 0;
}

int  SetNetwork()
{
#define IP_ADDR_MAX_LEN        15
#define DEVICE_NAME_MAX_LEN      30

char ipaddress[IP_ADDR_MAX_LEN + 1];
char subnetmask[IP_ADDR_MAX_LEN + 1];
char defaultgateway[IP_ADDR_MAX_LEN + 1];
char devicename[DEVICE_NAME_MAX_LEN+1];

    struct in_addr  iaddr, maddr, gaddr;
    int    fipmode;
    
static char *fipmodes[] = {
                           "Manual-Static",
                           "DHCP" };

    if (cgiFormStringNoNewlines("ipaddr", ipaddress, IP_ADDR_MAX_LEN+1) == cgiFormSuccess) {
      if (inet_aton(ipaddress, &iaddr) != 0) {
	clist.setValue(IPADDR, ipaddress);
      } else {
        return 0;
      }
    }

    if (cgiFormStringNoNewlines("ipmask", subnetmask, IP_ADDR_MAX_LEN+1) == cgiFormSuccess) {
      if (inet_aton(subnetmask, &maddr) != 0) {
        // Is it a bad subnet mask?
        if (badMask(maddr.s_addr, 0))
            return 2;
	clist.setValue(IPMASK, subnetmask);
      } else {
        return 0;
      }
    }

    if (cgiFormStringNoNewlines("gateway", defaultgateway, IP_ADDR_MAX_LEN+1) == cgiFormSuccess) {
      if (inet_aton(defaultgateway, &gaddr) != 0) {
	clist.setValue(GATEWAY, defaultgateway);
      } else {
        return 0;
      }
    }
    if ((iaddr.s_addr & maddr.s_addr) != (gaddr.s_addr & maddr.s_addr)) {
        return 3;
    }

    if (cgiFormStringNoNewlines("devname", devicename, DEVICE_NAME_MAX_LEN+1) == cgiFormSuccess) {
	clist.setValue(DEVICENAME, devicename);
    }

    if (cgiFormRadio("fipmode", fipmodes, 2, &fipmode, 0) == cgiFormSuccess) {
	clist.setValue(FIPMODE, fipmode);
    }

    if (cgiFormStringNoNewlines("PrefDNS", ipaddress, 20) == cgiFormSuccess) {         if (inet_aton(ipaddress, &iaddr) == 0) ipaddress[0] = 0;
        clist.setValue(PDNS, ipaddress);
    }
    if (cgiFormStringNoNewlines("AltDNS", ipaddress, 20) == cgiFormSuccess) {
        if (inet_aton(ipaddress, &iaddr) == 0) ipaddress[0] = 0;
        clist.setValue(ADNS, ipaddress);
    }

    clist.saveToFile(OTC_CONFIG);

    // inform watch dog for changing
//    tool.Changes(tool.DHCPD, 0);

//?    system("touch /tmp/reboot.sof");
//?    system("sync");
//?    Reboot();
// watchdog should decide reboot or not

   return 0;
}

int HandleUpdate()
{
  if (cgiFormSubmitClicked("applyNetSetting") == cgiFormSuccess) {
	return SetNetwork();	  
  }
  return 0;
} 

int cgiMain() 
{
        clist.loadFromFile(OTC_CONFIG);
	int code = HandleUpdate();
	if (code == 0)
		showPage();
	else 
		ShowNetworkSettingFailureMessage();
	return 0;	
}
