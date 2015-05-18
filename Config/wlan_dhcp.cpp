#include "ConfigCommon.h"
#include "ConfigTools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DHCPS 	"DHCPS"
#define DSTART 	"DSTART"
#define DEND	"DEND"
#define DTIME	"DTIME"
#define PDNS	"PDNS"
#define ADNS	"ADNS"

#define DHCPD_LEASES	"/var/state/dhcp/dhcpd.leases"
#define LEASE		"lease"
#define STARTS		"starts"
#define ENDS		"ends"
#define HARDWARE	"hardware"
#define BINDSTAT	"binding state"
#define CLNTHOST	"client-hostname"

char* tHeader[] = {
	"IP",
	"HOST",
	"Start", 
	"End", 
	NULL
};

ConfigCommon cfg;
ConfigTools tool;
LdProp  clist;

char* trimQuote(char* p)
{
	int i = strlen(p);
	if (i > 0) {
		if (*(p+i-1) == '"') *(p+i-1) = 0;
		if (*p == '"') p++;
	}
	return p;
}

char* getPhase(char*p, int n)
{
	while(*p && *p==' ') p++;	// find None space
	for(int i = 1; i < n; i++) {
		while(*p && *p!=' ') p++;       // find Space
		if (*p) p++;
	}
	if (*p) {
		char *q = p;
		while(*q && *q!=' ' && *q != ';') q++;
		*q = 0;
	}
	return p;
}

void showPage() 
{
	const char*pb;
	pb = clist.getIntValue(DHCPS) ? "checked" : " ";
	cfg.putHeader("Pegasus Wireless CYNALYNX - DHCP");
	cfg.startPage("DHCP Server", "The setting on this page controls the DHCP service provided by the CYNALYNX.");
	cfg.putTag("<form name=wlan_dhcp action=wlan_dhcp.cgi method=post enctype=multipart/form-data>");

	cfg.putSubtitle("DHCP Server Configuration settings");
        cfg.skipRow();
	cfg.actionRow("DHCP server Enabled", cfg.getFValue("<input type=checkbox name=dhcpServer %s>", (char*)pb));
	cfg.dispRow("&nbsp;", "The CYNALYNX must be assigned a fixed IP address to use this service.<br>WARNING: Do not use the CYNALYNX DHCP Server functions if another DHCP service is already in effect.");
        cfg.skipRow();

	cfg.actionRow("DHCP address range start", cfg.getFValue("<input type=text name=dhcpAddrST size=10 value=%s>",  clist.getValue(DSTART)));
	cfg.dispRow("&nbsp;","The start address in the range of IP addresses that will be served.");
        cfg.skipRow();

	cfg.actionRow("DHCP address range end", cfg.getFValue("<input type=text name=dhcpAddrED size=10 value=%s>",  clist.getValue(DEND)));
	cfg.dispRow("&nbsp;","The last address in the range of IP addresses that will be served.");
	cfg.skipRow();

	cfg.actionRow("DHCP timeout in minutes", cfg.getFValue("<input type=text name=dhcpTimeout size=10 value=%s>",  clist.getValue(DTIME)));
	cfg.dispRow("&nbsp;","The maximum lease time ");

#ifdef WLANONLY
	cfg.actionRow("WLAN client Only", cfg.getFValue("<input type=checkbox name=clientsOnly%s>",  " "));
	cfg.dispRow("&nbsp;", "Activate DHCP server for wireless LAN side clients only. Unchecking causes the DHCP server to process requests from both wired and wireless LAN.");
#endif
        cfg.skipRow();
        cfg.actionRow("Apply settings", "<input type=submit name=applyAdvConf value=Apply > <input type=reset value=Cancel id=reset1 name=reset1>");
	cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();

	if (clist.getIntValue(DHCPS) != 0) {
	cfg.putTag("</form>");
        cfg.putTag("<form name=wlan_dhclient action=wlan_dhcp.cgi method=post enctype=multipart/form-data>");

	cfg.skipArea();
	cfg.putSubtitle("DHCP Client table");

	LdConf llist;
	int i;
	llist.loadFromFile(DHCPD_LEASES);
	i = llist.getIndex(LEASE, OneLine::GSTR);

	if (i != -1) {
	cfg.startTable("", 6, tHeader);
	do { 
		int k = llist.getIndex(BINDSTAT, OneLine::MASK, i);
		char *p = getPhase(llist.getLine(k), 3);
		if (strcmp(p, "active") == 0) {
		k = i;
		cfg.putFmtTag("<tr><td>%s</td>", getPhase(llist.getLine(k),2));
		k = llist.getIndex(CLNTHOST, OneLine::MASK, i);
		cfg.putFmtTag("<td>%s</td>", trimQuote(getPhase(llist.getLine(k),2)));
		k = llist.getIndex(STARTS, OneLine::MASK, i);
		p = getPhase(llist.getLine(k),4);
		cfg.putFmtTag("<td>%s-%s</td>",getPhase(llist.getLine(k),3),p);
		k = llist.getIndex(ENDS, OneLine::MASK, i);
		p = getPhase(llist.getLine(k),4);
		cfg.putFmtTag("<td>%s-%s</td></tr>", getPhase(llist.getLine(k),3), p);
		}
		i = llist.getIndex(LEASE, OneLine::GSTR, i+1);
	} while (i != -1);

	cfg.endTable();
	cfg.skipRow();
	}

	cfg.actionRow("Check DHCP client", "<input type=submit name=dhcpRefresh value='Refresh' >");
	cfg.dispRow("&nbsp;", "Refresh DHCP table"); 
	}

	cfg.skipRow();
	cfg.putTag("</form>");
	
	cfg.endPage();
}

void HandleUpdate()
{
	char s[40];
	int i = 0, n = 0;
	if (cgiFormCheckboxSingle("dhcpServer") == cgiFormSuccess)
                i = 1;
	if (clist.getIntValue(DHCPS) != i) {
		n = i+1;
		clist.setValue(DHCPS, i);
	}
	if (cgiFormStringNoNewlines("dhcpAddrST", s, 10) == cgiFormSuccess) {
		i = atoi(s);
		clist.setValue(DSTART, i);
	}
        if (cgiFormStringNoNewlines("dhcpAddrED", s, 10) == cgiFormSuccess) {
                i = atoi(s);
                clist.setValue(DEND, i);
        }
        if (cgiFormStringNoNewlines("dhcpTimeout", s, 10) == cgiFormSuccess) {
                i = atoi(s);
                clist.setValue(DTIME, i);
        }
	// inform watch dog for changing
        if (n != 0) tool.Changes(tool.DHCPD, n);
	clist.saveToFile(OTC_CONFIG);
}

int cgiMain()
{
        clist.loadFromFile(OTC_CONFIG);
	if (cgiFormSubmitClicked("applyAdvConf") == cgiFormSuccess) 
		HandleUpdate();
	showPage();
	return 0;
}
